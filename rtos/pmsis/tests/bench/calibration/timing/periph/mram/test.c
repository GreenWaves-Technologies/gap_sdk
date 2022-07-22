/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pmsis.h"
#include "stdio.h"
#include "bsp/bsp.h"
#ifdef USE_HYPERFLASH
#include <bsp/flash/hyperflash.h>
#else
#include <bsp/flash/spiflash.h>
#endif
#include "bsp/flash.h"
#include <string.h>
#include "calib.h"

#define CYCLE_ERROR_NOMINAL 0.11
#define CYCLE_ERROR_CORNER 0.26

#define HYPER_FLASH 0
#define SPI_FLASH 1
#define MFLASH 2

#ifndef FULL_PLOT
#define FULL_PLOT 0
#endif

#define BUFF_SIZE (4 * 1024)
#define NB_ITER 4

static int init = 0;
static PI_L2 char buff[2][BUFF_SIZE];
static uint32_t hyper_buff[2];
static struct pi_device hyper;

#ifdef USE_HYPERFLASH
#define FLASH_NAME "hyperflash"
#elif defined(USE_MRAM)
#define FLASH_NAME "mram"
#else
#define FLASH_NAME "spiflash"
#endif

#define BANDWIDTH 25000000

int bench_open(int periph_freq, int hyper_freq, int fc_freq, int cycle_ref, float cycle_error_corner)
{
    pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_ACTIVE_CYCLES));
    pi_perf_reset();
    pi_perf_start();

#if defined(USE_HYPERFLASH)
    struct pi_hyperflash_conf conf;
    pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
    struct pi_mram_conf conf;
    pi_mram_conf_init(&conf);
#elif defined(USE_SPIFLASH)
#if defined(CONFIG_ATXP032)
    struct pi_atxp032_conf conf;
    pi_atxp032_conf_init(&conf);
#else
    struct pi_spiflash_conf conf;
    pi_spiflash_conf_init(&conf);
#endif
#endif

    if (periph_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq * 1000000);
    }

    if (fc_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq * 1000000);
    }

    pi_open_from_conf(&hyper, &conf);

    if (hyper_freq)
      conf.baudrate = hyper_freq * 1000000;

    if (pi_flash_open(&hyper))
        return -1;

    pi_flash_close(&hyper);

    pi_perf_stop();

    int cycles = pi_perf_read(PI_PERF_CYCLES);

#ifdef CALIB
    printf("#define OPEN_%d_%d_%d %d\n", periph_freq, hyper_freq, fc_freq, cycles);
#else
    float time = (float)cycles * 1000000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

    float error = 0.0;
    if (cycle_ref)
    {
        error = ((float)cycles - (float)cycle_ref) / cycle_ref;
        if (error < 0)
        {
            error = -error;
        }
    }

    if (cycle_ref)
    {
        if (error > cycle_error_corner)
        {
            printf("  ERROR DETECTED (expected: %d, got: %d, error: %f, expected error: %f) !!!!\n", cycle_ref, cycles, error, cycle_error_corner);
            return -1;
        }
    }

    printf("Open done (cycles: %d, error: %f)\n", cycles, error);
#endif
}


int bench_erase(int nb_iter, uint32_t addr, int size, int sect, int periph_freq, int hyper_freq, int fc_freq, int cycle_ref, float cycle_error_corner)
{
#if defined(USE_HYPERFLASH)
    struct pi_hyperflash_conf conf;
    pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
    struct pi_mram_conf conf;
    pi_mram_conf_init(&conf);
#elif defined(USE_SPIFLASH)
#if defined(CONFIG_ATXP032)
    struct pi_atxp032_conf conf;
    pi_atxp032_conf_init(&conf);
#else
    struct pi_spiflash_conf conf;
    pi_spiflash_conf_init(&conf);
#endif
#endif

    if (periph_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq * 1000000);
    }

    if (fc_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq * 1000000);
    }

    pi_open_from_conf(&hyper, &conf);

    if (hyper_freq)
      conf.baudrate = hyper_freq * 1000000;

    if (pi_flash_open(&hyper))
        return -1;

    for (int i=0; i<3; i++)
    {
        pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_ACTIVE_CYCLES));
        pi_perf_reset();
        pi_perf_start();

        for (int i = 0; i < nb_iter; i++)
        {
            if (size == -1)
            {
                pi_flash_erase_chip(&hyper);
            }
            else if (sect)
            {
                pi_flash_erase_sector(&hyper, addr);
            }
            else
            {
                pi_flash_erase(&hyper, addr, size);
            }
        }

        pi_perf_stop();
    }

    pi_flash_close(&hyper);

    int cycles = pi_perf_read(PI_PERF_CYCLES);

#ifdef CALIB
    printf("#define ERASE_%d_%d_%d_%d %d\n", periph_freq, size, hyper_freq, fc_freq, cycles);
#else
    float time = (float)cycles * 1000000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

    float error = 0.0;
    if (cycle_ref)
    {
        error = ((float)cycles - (float)cycle_ref) / cycle_ref;
        if (error < 0)
        {
            error = -error;
        }
    }

    printf("Erase done (size: %d, cycles: %d, cycles_ref: %d, error: %f, fc_frequency: %d, mram_frequency: %d)\n", size, cycles, cycle_ref, error, fc_freq, hyper_freq);

    if (cycle_ref)
    {
        if (error > cycle_error_corner)
        {
            printf("  ERROR DETECTED (expected: %d, got: %d, error: %f, expected error: %f) !!!!\n", cycle_ref, cycles, error, cycle_error_corner);
            return -1;
        }
    }

#endif

    return 0;
}


int bench_transfer(int is_read, int is_2d, int size, int len, int stride, int l2_offset, int flash_offset, int periph_freq, int hyper_freq, int fc_freq, int cycle_ref, float cycle_error_corner, int *instr, int *load)
{
#if defined(USE_HYPERFLASH)
    struct pi_hyperflash_conf conf;
    pi_hyperflash_conf_init(&conf);
#elif defined(USE_MRAM)
    struct pi_mram_conf conf;
    pi_mram_conf_init(&conf);
#elif defined(USE_SPIFLASH)
#if defined(CONFIG_ATXP032)
    struct pi_atxp032_conf conf;
    pi_atxp032_conf_init(&conf);
    conf.baudrate = 25000000;
#else
    struct pi_spiflash_conf conf;
    pi_spiflash_conf_init(&conf);
    conf.baudrate = 25000000;
#endif
#endif

    if (periph_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq * 1000000);
    }

    if (fc_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq * 1000000);
    }

    pi_open_from_conf(&hyper, &conf);

    if (hyper_freq)
      conf.baudrate = hyper_freq * 1000000;

    if (pi_flash_open(&hyper))
        return -1;

    if (!init)
    {
        init = 1;

        for (int i=0; i<BUFF_SIZE; i++)
        {
            buff[0][i] = i;
        }

        pi_flash_erase(&hyper, 0, BUFF_SIZE*2);
        pi_flash_program(&hyper, 0, buff[0], BUFF_SIZE);
        pi_flash_program(&hyper, BUFF_SIZE, buff[0], BUFF_SIZE);
    }

    if (load)
    {
        pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_ACTIVE_CYCLES));
    }
    else
    {
        pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_INSTR));
    }

    pi_task_t tasks[NB_ITER];

    for (int j = 0; j < 3; j++)
    {
        if (is_2d)
        {
            pi_perf_reset();
            pi_perf_start();

            for (int i = 0; i < NB_ITER; i++)
            {
                pi_flash_copy_2d(&hyper, 0, buff[0] + l2_offset, size, stride, len, is_read);
            }

            pi_perf_stop();
        }
        else
        {
            pi_perf_reset();
            pi_perf_start();

            for (int i = 0; i < NB_ITER; i++)
            {
                pi_flash_copy(&hyper, 0 + flash_offset, buff[0] + l2_offset, size, is_read);
            }

            pi_perf_stop();
        }
    }


    if (instr)
        *instr = pi_perf_read(PI_PERF_INSTR) / NB_ITER;

    if (load)
    {
        *load = pi_perf_read(PI_PERF_ACTIVE_CYCLES) * 100 / pi_perf_read(PI_PERF_CYCLES);
    }

    int cycles = pi_perf_read(PI_PERF_CYCLES);
    float time = (float)cycles * 1000000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

    int total_size = size * NB_ITER;
    float bandwidth = (float)total_size * 1000 / time;
    float bytes_per_cycle = (float)total_size / cycles;

    pi_flash_close(&hyper);

#ifdef CALIB
    printf("#define %s_%s_%d_%d_%d_%d_%d_%d_%d_%d %d\n", is_read ? "READ" : "WRITE", is_2d ? "2D" : "1D", size, len, stride, l2_offset, flash_offset, periph_freq, hyper_freq, fc_freq, cycles);
#else
    float error = 0.0;
    if (cycle_ref)
    {
        error = ((float)cycles - (float)cycle_ref) / cycle_ref;
        if (error < 0)
        {
            error = -error;
        }
    }

    printf("Use case done (fread: %d, 2d: %d, size: %d, line: %d, fc_freq: %d, mram_freq: %d, cycles: %d, cycles_ref: %d, bandwidth: %f MBytes/s, %f bytes/cycle, error: %f)\n", is_read, is_2d, size, len, fc_freq, hyper_freq, cycles, cycle_ref, bandwidth, bytes_per_cycle, error);

    if (cycle_ref)
    {
        if (error > cycle_error_corner)
        {
            printf("  ERROR DETECTED (expected: %d, got: %d, error: %f, expected error: %f) !!!!\n", cycle_ref, cycles, error, cycle_error_corner);
            return -1;
        }
    }

#endif


    return 0;
}

int test_entry()
{
    int errors = 0;

    // TODO since PMU is nto correctly timed, we cannot check the open time since
    // mram is powered up
    //errors += bench_open(0, 20, 200, OPEN_0_20_200, CYCLE_ERROR_NOMINAL);
    //errors += bench_open(0, 20, 100, OPEN_0_20_100, CYCLE_ERROR_NOMINAL);
    //errors += bench_open(0, 10, 200, OPEN_0_10_200, CYCLE_ERROR_NOMINAL);
    //errors += bench_open(0, 10, 100, OPEN_0_10_100, CYCLE_ERROR_NOMINAL);
    // errors += bench_transfer(1, 1, 1<<10, 1<<0, 0, 1, 0, 0, 40, 300, READ_2D_1024_1_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    
    errors += bench_erase(4, 0, 16,   0, 0, 40, 300, ERASE_0_16_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 32,   0, 0, 40, 300, ERASE_0_32_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 128,  0, 0, 40, 300, ERASE_0_128_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 512,  0, 0, 40, 300, ERASE_0_512_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 1024, 0, 0, 40, 300, ERASE_0_1024_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 2048, 0, 0, 40, 300, ERASE_0_2048_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 16,   0, 0, 20, 300, ERASE_0_16_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 32,   0, 0, 20, 300, ERASE_0_32_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 128,  0, 0, 20, 300, ERASE_0_128_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 512,  0, 0, 20, 300, ERASE_0_512_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 1024, 0, 0, 20, 300, ERASE_0_1024_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 2048, 0, 0, 20, 300, ERASE_0_2048_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 16,   0, 0, 10, 300, ERASE_0_16_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 32,   0, 0, 10, 300, ERASE_0_32_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 128,  0, 0, 10, 300, ERASE_0_128_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 512,  0, 0, 10, 300, ERASE_0_512_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 1024, 0, 0, 10, 300, ERASE_0_1024_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 2048, 0, 0, 10, 300, ERASE_0_2048_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 16,   0, 0, 10, 100, ERASE_0_16_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 32,   0, 0, 10, 100, ERASE_0_32_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 128,  0, 0, 10, 100, ERASE_0_128_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 512,  0, 0, 10, 100, ERASE_0_512_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 1024, 0, 0, 10, 100, ERASE_0_1024_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 2048, 0, 0, 10, 100, ERASE_0_2048_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 0,    1, 0, 40, 300, ERASE_0_0_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 0,    1, 0, 20, 300, ERASE_0_0_20_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 0,    1, 0, 10, 300, ERASE_0_0_10_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(4, 0, 0,    1, 0, 10, 100, ERASE_0_0_10_100, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(1, 0, -1,   0, 0, 40, 300, ERASE_CHIP_0_0_40_300, CYCLE_ERROR_NOMINAL);
    errors += bench_erase(1, 0, -1,   0, 0, 10, 300, ERASE_CHIP_0_0_10_300, CYCLE_ERROR_NOMINAL);

    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 10, 200,  WRITE_1D_1024_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 10, 100,  WRITE_1D_1024_0_0_0_0_0_10_100,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<4, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_16_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<5, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_32_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<6, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_64_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<7, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_128_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<8, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_256_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<9, 0, 0, 0, 0, 0, 20, 200,   WRITE_1D_512_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 20, 200,  WRITE_1D_1024_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<11, 0, 0, 0, 0, 0, 20, 200,  WRITE_1D_2048_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<12, 0, 0, 0, 0, 0, 20, 200,  WRITE_1D_4096_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<4, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_16_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<5, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_32_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<6, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_64_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<7, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_128_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<8, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_256_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<9, 0, 0, 0, 0, 0,  10, 200,  WRITE_1D_512_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 20, 100,  WRITE_1D_1024_0_0_0_0_0_20_100,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<11, 0, 0, 0, 0, 0, 10, 200,  WRITE_1D_2048_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<12, 0, 0, 0, 0, 0, 10, 200,  WRITE_1D_4096_0_0_0_0_0_10_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<4, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_16_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<5, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_32_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<6, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_64_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<7, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_128_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<8, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_256_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<9, 0, 0, 0, 0, 0,  40, 300,  WRITE_1D_512_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 40, 300,  WRITE_1D_1024_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<11, 0, 0, 0, 0, 0, 40, 300,  WRITE_1D_2048_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(0, 0, 1<<12, 0, 0, 0, 0, 0, 40, 300,  WRITE_1D_4096_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);

    // Do transfers at various frequencies to check bottlenecks
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 20,  READ_1D_1024_0_0_0_0_0_40_20,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 40,  READ_1D_1024_0_0_0_0_0_40_40,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 50,  READ_1D_1024_0_0_0_0_0_40_50,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 75,  READ_1D_1024_0_0_0_0_0_40_75,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 100, READ_1D_1024_0_0_0_0_0_40_100, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 125, READ_1D_1024_0_0_0_0_0_40_125, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 150, READ_1D_1024_0_0_0_0_0_40_150, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 175, READ_1D_1024_0_0_0_0_0_40_175, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 200, READ_1D_1024_0_0_0_0_0_40_200, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 210, READ_1D_1024_0_0_0_0_0_40_210, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 220, READ_1D_1024_0_0_0_0_0_40_220, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 230, READ_1D_1024_0_0_0_0_0_40_230, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 240, READ_1D_1024_0_0_0_0_0_40_240, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 250, READ_1D_1024_0_0_0_0_0_40_250, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 275, READ_1D_1024_0_0_0_0_0_40_275, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 300, READ_1D_1024_0_0_0_0_0_40_300, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 325, READ_1D_1024_0_0_0_0_0_40_325, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 350, READ_1D_1024_0_0_0_0_0_40_350, CYCLE_ERROR_CORNER, NULL, NULL);

    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 20,  READ_1D_1024_0_0_0_0_0_20_20,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 40,  READ_1D_1024_0_0_0_0_0_20_40,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 50,  READ_1D_1024_0_0_0_0_0_20_50,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 100, READ_1D_1024_0_0_0_0_0_20_100, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 110, READ_1D_1024_0_0_0_0_0_20_110, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 112, READ_1D_1024_0_0_0_0_0_20_112, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 114, READ_1D_1024_0_0_0_0_0_20_114, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 116, READ_1D_1024_0_0_0_0_0_20_116, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 118, READ_1D_1024_0_0_0_0_0_20_118, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 120, READ_1D_1024_0_0_0_0_0_20_120, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 130, READ_1D_1024_0_0_0_0_0_20_130, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 140, READ_1D_1024_0_0_0_0_0_20_140, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 150, READ_1D_1024_0_0_0_0_0_20_150, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 160, READ_1D_1024_0_0_0_0_0_20_160, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 170, READ_1D_1024_0_0_0_0_0_20_170, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 180, READ_1D_1024_0_0_0_0_0_20_180, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 190, READ_1D_1024_0_0_0_0_0_20_190, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 250, READ_1D_1024_0_0_0_0_0_20_250, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 10,  READ_1D_1024_0_0_0_0_0_10_10,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 40,  READ_1D_1024_0_0_0_0_0_10_40,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 60,  READ_1D_1024_0_0_0_0_0_10_60,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 68,  READ_1D_1024_0_0_0_0_0_10_68,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 100, READ_1D_1024_0_0_0_0_0_10_100, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 10, 200, READ_1D_1024_0_0_0_0_0_10_200, CYCLE_ERROR_CORNER, NULL, NULL);

    // Do transfers of all sizes to check how latency and bandwidth are combined
    errors += bench_transfer(1, 0, 1<<0, 0, 0, 0, 0, 0, 20, 200,  READ_1D_1_0_0_0_0_0_20_200,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<1, 0, 0, 0, 0, 0, 20, 200,  READ_1D_2_0_0_0_0_0_20_200,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<2, 0, 0, 0, 0, 0, 20, 200,  READ_1D_4_0_0_0_0_0_20_200,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<3, 0, 0, 0, 0, 0, 20, 200,  READ_1D_8_0_0_0_0_0_20_200,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<4, 0, 0, 0, 0, 0, 20, 200,  READ_1D_16_0_0_0_0_0_20_200,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<5, 0, 0, 0, 0, 0, 20, 200,  READ_1D_32_0_0_0_0_0_20_200,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<6, 0, 0, 0, 0, 0, 20, 200,  READ_1D_64_0_0_0_0_0_20_200,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<7, 0, 0, 0, 0, 0, 20, 200,  READ_1D_128_0_0_0_0_0_20_200,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<8, 0, 0, 0, 0, 0, 20, 200,  READ_1D_256_0_0_0_0_0_20_200,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<9, 0, 0, 0, 0, 0, 20, 200,  READ_1D_512_0_0_0_0_0_20_200,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 20, 200, READ_1D_1024_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<11, 0, 0, 0, 0, 0, 20, 200, READ_1D_2048_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<12, 0, 0, 0, 0, 0, 20, 200, READ_1D_4096_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<13, 0, 0, 0, 0, 0, 20, 200, READ_1D_8192_0_0_0_0_0_20_200,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<14, 0, 0, 0, 0, 0, 20, 200, READ_1D_16384_0_0_0_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<15, 0, 0, 0, 0, 0, 20, 200, READ_1D_32768_0_0_0_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);

    errors += bench_transfer(1, 0, 1<<0, 0, 0, 0, 0, 0,  40, 300, READ_1D_1_0_0_0_0_0_40_300,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<1, 0, 0, 0, 0, 0,  40, 300, READ_1D_2_0_0_0_0_0_40_300,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<2, 0, 0, 0, 0, 0,  40, 300, READ_1D_4_0_0_0_0_0_40_300,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<3, 0, 0, 0, 0, 0,  40, 300, READ_1D_8_0_0_0_0_0_40_300,     CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<4, 0, 0, 0, 0, 0,  40, 300, READ_1D_16_0_0_0_0_0_40_300,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<5, 0, 0, 0, 0, 0,  40, 300, READ_1D_32_0_0_0_0_0_40_300,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<6, 0, 0, 0, 0, 0,  40, 300, READ_1D_64_0_0_0_0_0_40_300,    CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<7, 0, 0, 0, 0, 0,  40, 300, READ_1D_128_0_0_0_0_0_40_300,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<8, 0, 0, 0, 0, 0,  40, 300, READ_1D_256_0_0_0_0_0_40_300,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<9, 0, 0, 0, 0, 0,  40, 300, READ_1D_512_0_0_0_0_0_40_300,   CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 0, 0, 0, 40, 300, READ_1D_1024_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<11, 0, 0, 0, 0, 0, 40, 300, READ_1D_2048_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<12, 0, 0, 0, 0, 0, 40, 300, READ_1D_4096_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<13, 0, 0, 0, 0, 0, 40, 300, READ_1D_8192_0_0_0_0_0_40_300,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<14, 0, 0, 0, 0, 0, 40, 300, READ_1D_16384_0_0_0_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<15, 0, 0, 0, 0, 0, 40, 300, READ_1D_32768_0_0_0_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    

    // Do small tranfers at various frequencies to check latency
    errors += bench_transfer(1, 0, 1, 0, 0, 0, 0, 0, 20, 250,  READ_1D_1_0_0_0_0_0_20_250,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0, 0, 0, 20, 150,  READ_1D_1_0_0_0_0_0_20_150,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0, 0, 0, 10, 200,  READ_1D_1_0_0_0_0_0_10_200,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0, 0, 0, 10, 100,  READ_1D_1_0_0_0_0_0_10_100,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0, 0, 0, 5, 250,   READ_1D_1_0_0_0_0_0_5_250,   CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0, 0, 0, 20, 250,  READ_1D_3_0_0_0_0_0_20_250,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0, 0, 0, 20, 250,  READ_1D_4_0_0_0_0_0_20_250,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0, 0, 0, 20, 250,  READ_1D_8_0_0_0_0_0_20_250,  CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0, 0, 0, 20, 250, READ_1D_16_0_0_0_0_0_20_250, CYCLE_ERROR_CORNER, NULL, NULL);

    // Do flash misaligned transfers
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    0, 0, 20, 250,  READ_1D_1_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    1, 0, 20, 250,  READ_1D_1_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 2, 0, 0, 0,    1, 0, 20, 250,  READ_1D_2_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0,    1, 0, 20, 250,  READ_1D_3_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0,    1, 0, 20, 250,  READ_1D_4_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 5, 0, 0, 0,    1, 0, 20, 250,  READ_1D_5_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 7, 0, 0, 0,    1, 0, 20, 250,  READ_1D_7_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0,    1, 0, 20, 250,  READ_1D_8_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 9, 0, 0, 0,    1, 0, 20, 250,  READ_1D_9_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 15, 0, 0, 0,   1, 0, 20, 250,  READ_1D_15_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0,   1, 0, 20, 250,  READ_1D_16_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 17, 0, 0, 0,   1, 0, 20, 250,  READ_1D_17_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 31, 0, 0, 0,   1, 0, 20, 250,  READ_1D_31_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 32, 0, 0, 0,   1, 0, 20, 250,  READ_1D_32_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 33, 0, 0, 0,   1, 0, 20, 250,  READ_1D_33_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1023, 0, 0, 0, 1, 0, 20, 250,  READ_1D_1023_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1024, 0, 0, 0, 1, 0, 20, 250,  READ_1D_1024_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1025, 0, 0, 0, 1, 0, 20, 250,  READ_1D_1025_0_0_0_1_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    31, 0, 20, 250, READ_1D_1_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 2, 0, 0, 0,    31, 0, 20, 250, READ_1D_2_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0,    31, 0, 20, 250, READ_1D_3_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0,    31, 0, 20, 250, READ_1D_4_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 5, 0, 0, 0,    31, 0, 20, 250, READ_1D_5_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 7, 0, 0, 0,    31, 0, 20, 250, READ_1D_7_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0,    31, 0, 20, 250, READ_1D_8_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 9, 0, 0, 0,    31, 0, 20, 250, READ_1D_9_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 15, 0, 0, 0,   31, 0, 20, 250, READ_1D_15_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0,   31, 0, 20, 250, READ_1D_16_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 17, 0, 0, 0,   31, 0, 20, 250, READ_1D_17_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 31, 0, 0, 0,   31, 0, 20, 250, READ_1D_31_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 32, 0, 0, 0,   31, 0, 20, 250, READ_1D_32_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 33, 0, 0, 0,   31, 0, 20, 250, READ_1D_33_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1023, 0, 0, 0, 31, 0, 20, 250, READ_1D_1023_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1024, 0, 0, 0, 31, 0, 20, 250, READ_1D_1024_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1025, 0, 0, 0, 31, 0, 20, 250, READ_1D_1025_0_0_0_31_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    16, 0, 20, 250, READ_1D_1_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 2, 0, 0, 0,    16, 0, 20, 250, READ_1D_2_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0,    16, 0, 20, 250, READ_1D_3_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0,    16, 0, 20, 250, READ_1D_4_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 5, 0, 0, 0,    16, 0, 20, 250, READ_1D_5_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 7, 0, 0, 0,    16, 0, 20, 250, READ_1D_7_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0,    16, 0, 20, 250, READ_1D_8_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 9, 0, 0, 0,    16, 0, 20, 250, READ_1D_9_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 15, 0, 0, 0,   16, 0, 20, 250, READ_1D_15_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0,   16, 0, 20, 250, READ_1D_16_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 17, 0, 0, 0,   16, 0, 20, 250, READ_1D_17_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 31, 0, 0, 0,   16, 0, 20, 250, READ_1D_31_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 32, 0, 0, 0,   16, 0, 20, 250, READ_1D_32_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 33, 0, 0, 0,   16, 0, 20, 250, READ_1D_33_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1023, 0, 0, 0, 16, 0, 20, 250, READ_1D_1023_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1024, 0, 0, 0, 16, 0, 20, 250, READ_1D_1024_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1025, 0, 0, 0, 16, 0, 20, 250, READ_1D_1025_0_0_0_16_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    15, 0, 20, 250, READ_1D_1_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 2, 0, 0, 0,    15, 0, 20, 250, READ_1D_2_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0,    15, 0, 20, 250, READ_1D_3_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0,    15, 0, 20, 250, READ_1D_4_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 5, 0, 0, 0,    15, 0, 20, 250, READ_1D_5_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 7, 0, 0, 0,    15, 0, 20, 250, READ_1D_7_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0,    15, 0, 20, 250, READ_1D_8_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 9, 0, 0, 0,    15, 0, 20, 250, READ_1D_9_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 15, 0, 0, 0,   15, 0, 20, 250, READ_1D_15_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0,   15, 0, 20, 250, READ_1D_16_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 17, 0, 0, 0,   15, 0, 20, 250, READ_1D_17_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 31, 0, 0, 0,   15, 0, 20, 250, READ_1D_31_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 32, 0, 0, 0,   15, 0, 20, 250, READ_1D_32_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 33, 0, 0, 0,   15, 0, 20, 250, READ_1D_33_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1023, 0, 0, 0, 15, 0, 20, 250, READ_1D_1023_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1024, 0, 0, 0, 15, 0, 20, 250, READ_1D_1024_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1025, 0, 0, 0, 15, 0, 20, 250, READ_1D_1025_0_0_0_15_0_20_250, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1, 0, 0, 0,    8, 0, 20, 250,  READ_1D_1_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 2, 0, 0, 0,    8, 0, 20, 250,  READ_1D_2_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 3, 0, 0, 0,    8, 0, 20, 250,  READ_1D_3_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 4, 0, 0, 0,    8, 0, 20, 250,  READ_1D_4_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 5, 0, 0, 0,    8, 0, 20, 250,  READ_1D_5_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 7, 0, 0, 0,    8, 0, 20, 250,  READ_1D_7_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 8, 0, 0, 0,    8, 0, 20, 250,  READ_1D_8_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 9, 0, 0, 0,    8, 0, 20, 250,  READ_1D_9_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 15, 0, 0, 0,   8, 0, 20, 250,  READ_1D_15_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 16, 0, 0, 0,   8, 0, 20, 250,  READ_1D_16_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 17, 0, 0, 0,   8, 0, 20, 250,  READ_1D_17_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 31, 0, 0, 0,   8, 0, 20, 250,  READ_1D_31_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 32, 0, 0, 0,   8, 0, 20, 250,  READ_1D_32_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 33, 0, 0, 0,   8, 0, 20, 250,  READ_1D_33_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1023, 0, 0, 0, 8, 0, 20, 250,  READ_1D_1023_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1024, 0, 0, 0, 8, 0, 20, 250,  READ_1D_1024_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 0, 1025, 0, 0, 0, 8, 0, 20, 250,  READ_1D_1025_0_0_0_8_0_20_250,  CYCLE_ERROR_NOMINAL, NULL, NULL);
    
    // Do L2 misaligned transfers
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 1, 0, 0, 20, 120, READ_1D_1024_0_0_1_0_0_20_120, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 2, 0, 0, 20, 120, READ_1D_1024_0_0_2_0_0_20_120, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 3, 0, 0, 20, 120, READ_1D_1024_0_0_3_0_0_20_120, CYCLE_ERROR_CORNER, NULL, NULL);
    errors += bench_transfer(1, 0, 1<<10, 0, 0, 1, 0, 0, 20, 200, READ_1D_1024_0_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);

    // Do 2D transfers with big size and various line length to see the cost of line start
    errors += bench_transfer(1, 1, 1<<10, 1<<0, 0, 1, 0, 0, 20, 200, READ_2D_1024_1_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<1, 0, 1, 0, 0, 20, 200, READ_2D_1024_2_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<2, 0, 1, 0, 0, 20, 200, READ_2D_1024_4_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<3, 0, 1, 0, 0, 20, 200, READ_2D_1024_8_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<4, 0, 1, 0, 0, 20, 200, READ_2D_1024_16_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<5, 0, 1, 0, 0, 20, 200, READ_2D_1024_32_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<6, 0, 1, 0, 0, 20, 200, READ_2D_1024_64_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<7, 0, 1, 0, 0, 20, 200, READ_2D_1024_128_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<8, 0, 1, 0, 0, 20, 200, READ_2D_1024_256_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<9, 0, 1, 0, 0, 20, 200, READ_2D_1024_512_0_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);

    errors += bench_transfer(1, 1, 1<<10, 1<<0, 0, 1, 0, 0, 40, 300, READ_2D_1024_1_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<1, 0, 1, 0, 0, 40, 300, READ_2D_1024_2_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<2, 0, 1, 0, 0, 40, 300, READ_2D_1024_4_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<3, 0, 1, 0, 0, 40, 300, READ_2D_1024_8_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<4, 0, 1, 0, 0, 40, 300, READ_2D_1024_16_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<5, 0, 1, 0, 0, 40, 300, READ_2D_1024_32_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<6, 0, 1, 0, 0, 40, 300, READ_2D_1024_64_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<7, 0, 1, 0, 0, 40, 300, READ_2D_1024_128_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<8, 0, 1, 0, 0, 40, 300, READ_2D_1024_256_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<9, 0, 1, 0, 0, 40, 300, READ_2D_1024_512_0_1_0_0_40_300, CYCLE_ERROR_NOMINAL, NULL, NULL);

    // Do the same with misaligned lines
    errors += bench_transfer(1, 1, 1<<10, 1<<0, 15, 1, 0, 0, 20, 200, READ_2D_1024_1_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<1, 15, 1, 0, 0, 20, 200, READ_2D_1024_2_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<2, 15, 1, 0, 0, 20, 200, READ_2D_1024_4_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<3, 15, 1, 0, 0, 20, 200, READ_2D_1024_8_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<4, 15, 1, 0, 0, 20, 200, READ_2D_1024_16_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<5, 15, 1, 0, 0, 20, 200, READ_2D_1024_32_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<6, 15, 1, 0, 0, 20, 200, READ_2D_1024_64_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<7, 15, 1, 0, 0, 20, 200, READ_2D_1024_128_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<8, 15, 1, 0, 0, 20, 200, READ_2D_1024_256_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);
    errors += bench_transfer(1, 1, 1<<10, 1<<9, 15, 1, 0, 0, 20, 200, READ_2D_1024_512_15_1_0_0_20_200, CYCLE_ERROR_NOMINAL, NULL, NULL);

    return errors;
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
