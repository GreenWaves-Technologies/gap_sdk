/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include <bsp/ram/hyperram.h>
#include <bsp/bsp.h>
#ifdef USE_SPIRAM
#include <bsp/ram/spiram.h>
#endif
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif


static struct pi_device cluster_dev;
static struct pi_cluster_task cl_task;
static pi_task_t fc_task;

static struct pi_device hyper;
static uint32_t ram_buff[2];
static unsigned char *ram_l2_buff[2];
static pi_task_t ram_fc_tasks[2];
static pi_task_t ram_fc_task_end;
static volatile int ram_end;
static int ram_errors;

struct pi_device flash;
static uint32_t flash_buff[2];
static unsigned char *flash_l2_buff[2];
static pi_task_t flash_fc_tasks[2];
static pi_task_t flash_fc_task_end;
static volatile int flash_end;
static int flash_errors;
static int flash_count;

static int end_time;


static volatile int cluster_end;

#define BUFF_SIZE 2048
#define FLASH_BUFF_SIZE 128

#define FLASH_ADDR 0x40000
#ifdef USE_SPIFLASH
// SPI flash takes too much time to be erase, just do it on a reduced area
#define FLASH_SIZE (1<<11)
#else
#define FLASH_SIZE (TEST_DURATION_US / 100 * FLASH_BUFF_SIZE * 2)  // We have around 25 flash programs per buffer for 5000 us
#endif

#ifdef USE_MRAM
#define DO_ERASE_DURING_PROGRAM 1
#endif

extern void jump_entry();

static void pe_entry(void *arg)
{
    while (!cluster_end)
    {
        jump_entry();
    }
}


static void cluster_entry(void *arg)
{
    pi_cl_team_fork(0, pe_entry, NULL);
}


static int start_cluster()
{
    struct pi_cluster_conf cl_conf;

    cluster_end = 0;

    pi_cluster_conf_init(&cl_conf);
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        return -1;
    }

    pi_cluster_send_task_to_cl_async(&cluster_dev, pi_cluster_task(&cl_task, cluster_entry, NULL), pi_task_block(&fc_task));

    return 0;
}


POS_TEXT_L2 static void hyper_end_of_tx(void *arg);


POS_TEXT_L2 static void hyper_end_of_rx(void *arg)
{
    int index = (int)arg;
    if (pi_time_get_us() < end_time)
    {
        pi_ram_write_async(&hyper, ram_buff[index], ram_l2_buff[index], BUFF_SIZE, pi_task_callback(&ram_fc_tasks[index], hyper_end_of_tx, (void *)index));
    }
    else
    {
        ram_end++;
        for (int i=0; i<BUFF_SIZE; i++)
        {
            unsigned int expected = (index ? i | 0x80 : i & 0x7f) & 0xff;
            if (ram_l2_buff[index][i] != expected)
            {
                ram_errors++;
            }
        }

        if (ram_end == 2)
        {
            pi_task_push(&ram_fc_task_end);
        }
    }
}


POS_TEXT_L2 static void hyper_end_of_tx(void *arg)
{
    int index = (int)arg;

    pi_ram_read_async(&hyper, ram_buff[index], ram_l2_buff[index], BUFF_SIZE, pi_task_callback(&ram_fc_tasks[index], hyper_end_of_rx, (void *)index));
}


static __attribute__((noinline)) void stop_cluster()
{
    cluster_end = 1;
    pi_task_wait_on(&fc_task);
    pi_cluster_close(&cluster_dev);
}


static int start_ram()
{
#ifdef USE_HYPERRAM
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
#elif USE_MRAM
    struct pi_mram_conf flash_conf;
    pi_mram_conf_init(&flash_conf);
#elif defined(CONFIG_APS25XXXN)
    struct pi_aps25xxxn_conf conf;
    pi_aps25xxxn_conf_init(&conf);
#else
    struct pi_spiram_conf conf;
    pi_spiram_conf_init(&conf);
#endif

    ram_end = 0;
    ram_errors = 0;

    pi_task_block(&ram_fc_task_end);
    pi_open_from_conf(&hyper, &conf);

    if (pi_ram_open(&hyper))
        return -1;

    if (pi_ram_alloc(&hyper, &ram_buff[0], BUFF_SIZE))
        return -1;

    if (pi_ram_alloc(&hyper, &ram_buff[1], BUFF_SIZE))
        return -1;

    ram_l2_buff[0] = pmsis_l2_malloc(BUFF_SIZE);
    if (ram_l2_buff[0] == NULL)
        return -1;

    ram_l2_buff[1] = pmsis_l2_malloc(BUFF_SIZE);
    if (ram_l2_buff[1] == NULL)
        return -2;

    for (int i=0; i<BUFF_SIZE; i++)
    {
        ram_l2_buff[0][i] = i & 0x7f;
        ram_l2_buff[1][i] = i | 0x80;
    }

    pi_ram_write_async(&hyper, ram_buff[0], ram_l2_buff[0], BUFF_SIZE, pi_task_callback(&ram_fc_tasks[0], hyper_end_of_tx, (void *)0));
    pi_ram_write_async(&hyper, ram_buff[1], ram_l2_buff[1], BUFF_SIZE, pi_task_callback(&ram_fc_tasks[1], hyper_end_of_tx, (void *)1));

    return 0;
}


static __attribute__((noinline)) int stop_ram()
{
    pi_task_wait_on(&ram_fc_task_end);
    pi_ram_free(&hyper, ram_buff[0], BUFF_SIZE);
    pi_ram_free(&hyper, ram_buff[1], BUFF_SIZE);
    pi_l2_free(ram_l2_buff[0], BUFF_SIZE);
    pi_l2_free(ram_l2_buff[1], BUFF_SIZE);
    pi_ram_close(&hyper);
    return ram_errors;
}


static void flash_end_of_program(void *arg);
static void flash_end_of_erase(void *arg);


static void flash_end_of_read(void *arg)
{
    int index = (int)arg;

    flash_count++;
    //printf("End of read\n");

    if (pi_time_get_us() < end_time && flash_count*FLASH_BUFF_SIZE < FLASH_SIZE)
    {
        flash_buff[index] += FLASH_BUFF_SIZE;
        //printf("Program %x\n", flash_buff[index]);

#ifdef DO_ERASE_DURING_PROGRAM
        pi_flash_erase_async(&flash, flash_buff[index], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[index], flash_end_of_erase, (void *)index));
#else
        pi_flash_program_async(&flash, flash_buff[index], flash_l2_buff[index], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[index], flash_end_of_program, (void *)index));
#endif
    }
    else
    {
        //printf("Flash end\n");
        flash_end++;
        for (int i=0; i<FLASH_BUFF_SIZE; i++)
        {
            unsigned int expected = (index ? i | 0x80 : i & 0x7f) & 0xff;
            if (flash_l2_buff[index][i] != expected)
            {
                flash_errors++;
                printf("FLASH ERROR AT INDEX %x, got %x, expected %x\n", i, flash_l2_buff[index][i], expected);
            }
        }

        if (flash_end == 2)
        {
            pi_task_push(&flash_fc_task_end);
        }
    }
}


static void flash_end_of_program(void *arg)
{
    int index = (int)arg;
    //printf("Flash read\n");
    pi_flash_read_async(&flash, flash_buff[index], flash_l2_buff[index], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[index], flash_end_of_read, (void *)index));
}


static void flash_end_of_erase(void *arg)
{
    int index = (int)arg;
    //printf("Flash read\n");
    pi_flash_program_async(&flash, flash_buff[index], flash_l2_buff[index], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[index], flash_end_of_program, (void *)index));
}


static int prepare_flash()
{
#ifdef USE_HYPERFLASH
    struct pi_hyperflash_conf flash_conf;
    pi_hyperflash_conf_init(&flash_conf);
#elif USE_MRAM
    struct pi_mram_conf flash_conf;
    pi_mram_conf_init(&flash_conf);
#else
#if defined(CONFIG_ATXP032)
    struct pi_atxp032_conf flash_conf;
    pi_atxp032_conf_init(&flash_conf);
#else
    struct pi_spiflash_conf flash_conf;
    pi_spiflash_conf_init(&flash_conf);
#endif
#endif

    pi_open_from_conf(&flash, &flash_conf);

    if (pi_flash_open(&flash))
        return -1;

#ifndef DO_ERASE_DURING_PROGRAM

#ifdef USE_SPIFLASH
    pi_flash_erase(&flash, FLASH_ADDR, FLASH_SIZE*2);
#else
    pi_flash_erase(&flash, FLASH_ADDR, FLASH_SIZE);
    pi_flash_erase(&flash, FLASH_ADDR + FLASH_SIZE, FLASH_SIZE);
#endif
#endif

    return 0;
}


static int start_flash()
{
    flash_end = 0;
    flash_errors = 0;
    flash_count = 0;

    pi_task_block(&flash_fc_task_end);

    flash_buff[0] = FLASH_ADDR;
    flash_buff[1] = FLASH_ADDR + FLASH_SIZE;

    flash_l2_buff[0] = pmsis_l2_malloc(FLASH_BUFF_SIZE);
    if (flash_l2_buff[0] == NULL)
        return -1;

    flash_l2_buff[1] = pmsis_l2_malloc(FLASH_BUFF_SIZE);
    if (flash_l2_buff[1] == NULL)
        return -2;

    for (int i=0; i<FLASH_BUFF_SIZE; i++)
    {
        flash_l2_buff[0][i] = i & 0x7f;
        flash_l2_buff[1][i] = i | 0x80;
    }

#ifdef DO_ERASE_DURING_PROGRAM
    pi_flash_erase_async(&flash, flash_buff[0], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[0], flash_end_of_erase, (void *)0));
    pi_flash_erase_async(&flash, flash_buff[1], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[1], flash_end_of_erase, (void *)1));
#else
    pi_flash_program_async(&flash, flash_buff[0], flash_l2_buff[0], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[0], flash_end_of_program, (void *)0));
    pi_flash_program_async(&flash, flash_buff[1], flash_l2_buff[1], FLASH_BUFF_SIZE, pi_task_callback(&flash_fc_tasks[1], flash_end_of_program, (void *)1));
#endif

    return 0;
}


static __attribute__((noinline)) int stop_flash()
{
    pi_task_wait_on(&flash_fc_task_end);
    pi_l2_free(flash_l2_buff[0], FLASH_BUFF_SIZE);
    pi_l2_free(flash_l2_buff[1], FLASH_BUFF_SIZE);
    pi_flash_close(&flash);
    return flash_errors;
}


static int test_entry()
{
    int errors = 0;

    printf("Entering test\n");

#if defined(TEST_TYPE_FAST) || defined(TEST_TYPE_QUICK)
    int frequencies[] = { 50000000 };
#elif defined(TEST_TYPE_LONGLONG)
    int frequencies[] = { 5000000, 10000000, 50000000, 100000000, 200000000 };
#elif defined(TEST_TYPE_LONG)
    int frequencies[] = { 5000000, 50000000, 200000000 };
#endif

    for (int i=0; i<sizeof(frequencies)/sizeof(int); i++)
    {
        printf("Testing with cluster freq %d\n", frequencies[i]);

        pi_freq_set(PI_FREQ_DOMAIN_CL, frequencies[i]);

 #if !defined(SOC_IDLE)
        if (prepare_flash())
            return -1;
#endif

        end_time = pi_time_get_us() + TEST_DURATION_US;

        if (start_cluster())
            return -1;

 #if !defined(SOC_IDLE)
#if !defined(USE_MRAM)
        if (start_ram())
            return -1;
#endif

        if (start_flash())
            return -1;

#if !defined(USE_MRAM)
        errors += stop_ram();
#endif

        errors += stop_flash();
#endif

#if defined(SOC_IDLE)
        while (pi_time_get_us() < end_time)
        {
            pi_time_wait_us(20);
            pi_soc_idle_us(10);
        }
#endif


        stop_cluster();

        if (errors)
        {
            break;
        }

        printf("Done with flash count: %d\n", flash_count);
    }

    if (errors)
    {
        printf("TEST FAILURE\n");
        return -1;
    }
    else
    {
        printf("TEST SUCCESS\n");
        return 0;
    }
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}