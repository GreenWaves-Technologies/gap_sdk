/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "pmsis.h"
#include "stdio.h"
#include <bsp/bsp.h>
#include <bsp/ram/hyperram.h>

#define BUFF_SIZE 2048

static uint8_t aes_key_ecb_128[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

#define NB_CONFIGURATIONS (1)

static pi_aes_utils_conf_t aes_conf[NB_CONFIGURATIONS] = {
    {
        /* ECB 128 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_128,
        .key = (uint32_t*) aes_key_ecb_128,
        .iv = NULL,
    },
};

#define NB_HYPERS (2)

static uint8_t* buff[NB_HYPERS];
static uint8_t* rcv_buff[NB_HYPERS];
static uint32_t hyper_buff[NB_HYPERS];
static struct pi_device hyper0_aes;
static struct pi_device hyper1_aes;

static struct pi_device* hypers[NB_HYPERS] = {&hyper0_aes, &hyper1_aes};

int test_configuration(pi_aes_utils_conf_t* aes_conf)
{

    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    memcpy(&(conf.ram.aes_conf), aes_conf, sizeof(pi_aes_utils_conf_t));

    printf("Allocate data\n");
    for (int i = 0; i < NB_HYPERS; i++)
    {
        buff[i] = pi_l2_malloc(BUFF_SIZE);
        if (buff[i] == NULL)
        {
            return 1;
        }

        rcv_buff[i] = pi_l2_malloc(BUFF_SIZE);
        if (rcv_buff[i] == NULL)
        {
            return 2;
        }
    }

    for (uint8_t i = 0; i < NB_HYPERS; i++)
    {
        printf("Open hyper%d device\n", i);
        conf.hyper_itf = i;
        pi_open_from_conf(hypers[i], &conf);

        if (pi_ram_open(hypers[i]))
        {
            return 3;
        }

        printf("Allocate ram data\n");
        if (pi_ram_alloc(hypers[i], &hyper_buff[i], BUFF_SIZE))
        {
            return 4;
        }
    }

    printf("Initialize buffers\n");
    for (int i=0; i<BUFF_SIZE; i++)
    {
        buff[0][i] = i & 0x7f;
        buff[1][i] = i | 0x80;
    }

    pi_task_t blocks[4];
    for (uint8_t i = 0; i < 4; i++)
    {
        pi_task_block(&blocks[i]);
    }

    printf("writing/reading data (on-the-fly [en/de]cryption)\n");
    pi_ram_write_async(hypers[0], hyper_buff[0], buff[0], BUFF_SIZE, &blocks[0]);
    pi_ram_write_async(hypers[1], hyper_buff[1], buff[1], BUFF_SIZE, &blocks[1]);
    pi_ram_read_async(hypers[0], hyper_buff[0], rcv_buff[0], BUFF_SIZE, &blocks[2]);
    pi_ram_read_async(hypers[1], hyper_buff[1], rcv_buff[1], BUFF_SIZE, &blocks[3]);

    for (uint8_t i = 0; i < 4; i++)
    {
        pi_task_wait_on(&blocks[i]);
    }

    printf("verifying data\n");
    for (int j = 0; j < NB_HYPERS; j++)
    {
        for (int i = 0; i<BUFF_SIZE; i++)
        {
            unsigned char expected = (j == 0) ? (i & 0x7f) : (i | 0x80);

            if (expected != rcv_buff[j][i])
            {
                printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, rcv_buff[j][i]);
                return 6;
            }
        }
    }
    printf("data verified\n");

    for (uint8_t i = 0; i < NB_HYPERS; i++)
    {
        pi_ram_free(hypers[i], hyper_buff[i], BUFF_SIZE);
        pi_ram_close(hypers[i]);
    }

    return 0;
}

int test_entry()
{
    printf("Entering main controller (mode: hyperram)\n");

    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    conf.hyper_cs = 1;

    for (uint8_t i = 0; i < NB_CONFIGURATIONS; i++)
    {
        printf("Testing configuration #%u\n", i);
        int result = test_configuration(&aes_conf[i]);
        if (0 != result)
        {
            printf("Error during test #%u\n", i);
            pmsis_exit(result);
        }
    }

    printf("TEST SUCCESS\n");

    return 0;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
