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

//#define ASYNC 1

#define CYCLE_ERROR_NOMINAL 0.10
#define CYCLE_ERROR_CORNER 0.26

#define BUFF_SIZE (4 * 1024)
#define NB_ITER 16

static int init = 0;
static PI_L2 char src_buff[2][BUFF_SIZE];
static PI_L2 char dst_buff[2][BUFF_SIZE];
static struct pi_device dev;

static pi_task_t datamove_tasks[NB_ITER];
static pi_task_t end_task;



#define BANDWIDTH 25000000


static void handle_datamove_end(void *arg)
{
    if (arg)
    {
        pi_task_push_irq_safe((pi_task_t *)arg);
    }
}


int bench_transfer(int src_is_2d, int dst_is_2d, int size, int src_len, int src_stride,
    int dst_len, int dst_stride, int src_l2_offset, int dst_l2_offset, int periph_freq, int fc_freq,
    int cycle_ref, float cycle_error_corner)
{
    float load = 0;
    float instr = 0;
    float active_cycles = 0;
    pi_udma_datamove_conf_t conf;

    if (periph_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_PERIPH, periph_freq * 1000000);
    }

    if (fc_freq != 0)
    {
        pi_freq_set(PI_FREQ_DOMAIN_FC, fc_freq * 1000000);
    }

    pi_udma_datamove_conf_init(&conf);

    pi_open_from_conf(&dev, &conf);

    if (src_is_2d)
    {
        conf.src_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_2D;
        conf.src_trf_cfg.row_len = src_len;
        conf.src_trf_cfg.stride = src_stride;
    }

    if (dst_is_2d)
    {
        conf.dst_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_2D;
        conf.dst_trf_cfg.row_len = dst_len;
        conf.dst_trf_cfg.stride = dst_stride;
    }

    if (pi_udma_datamove_open(&dev))
        return -1;

    if (!init)
    {
        init = 1;

        for (int i=0; i<BUFF_SIZE; i++)
        {
            src_buff[0][i] = i;
        }
    }

    // We do 4 iterations to benchmark with hot cache.
    // The 2 firsts are needed to hot the cache, and the 2 lasts to measure active cycles and
    // instructions.
    // Be carefull to go through all the code in the loop in the 2 first iterations
    // to make sure the 2 last ones are done without any miss.
    for (int j = 0; j < 4; j++)
    {
#ifdef ASYNC
        pi_task_block(&end_task);
#endif

        if (j & 1)
        {
            pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_ACTIVE_CYCLES));
        }
        else
        {
            pi_perf_conf((1 << PI_PERF_CYCLES) | (1 << PI_PERF_INSTR));
        }

        pi_perf_reset();
        pi_perf_start();

#ifdef ASYNC
        for (int i = 0; i < NB_ITER; i++)
        {
            pi_udma_datamove_copy_async(&dev, src_buff[0] + src_l2_offset, dst_buff[0] + dst_l2_offset,
                size, pi_task_irq_callback(&datamove_tasks[i], handle_datamove_end, i == NB_ITER - 1 ? &end_task : NULL));
        }

        pi_task_wait_on(&end_task);
#else
        for (int i = 0; i < NB_ITER; i++)
        {
            pi_udma_datamove_copy(&dev, src_buff[0] + src_l2_offset, dst_buff[0] + dst_l2_offset,
                size);
        }
#endif

        pi_perf_stop();

        if (j & 1)
        {
            active_cycles = (float)pi_perf_read(PI_PERF_ACTIVE_CYCLES);
        }
        else
        {
            instr = (float)pi_perf_read(PI_PERF_INSTR);
        }
    }


    int cycles = pi_perf_read(PI_PERF_CYCLES);
    load = (float)active_cycles * 100 / cycles;
    active_cycles /= NB_ITER;
    instr /= NB_ITER;

    float time = (float)cycles * 1000000000 / pi_freq_get(PI_FREQ_DOMAIN_FC);

    int total_size = size * NB_ITER;
    float bandwidth = (float)total_size * 1000 / time;
    float bytes_per_cycle = (float)total_size / cycles;

    pi_udma_datamove_close(&dev);

#ifdef CALIB
    printf("#define %s_%s_%d_%d_%d_%d_%d_%d_%d_%d %d %d\n", src_is_2d ? "2D" : "1D", dst_is_2d ? "2D" : "1D", 
        size, src_len, src_stride, dst_len, dst_stride, src_l2_offset, dst_l2_offset, periph_freq,
        fc_freq, cycles);
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

    printf("Use case done (src_2d: %d, dst_2d: %d, size: %d, src_line: %d, src_stride: %d, dst_line: %d, dst_stride: %d, fc_freq: %d, periph_freq: %d, cycles: %d, cycles_ref: %d, bandwidth: %f MBytes/s, %f bytes/cycle, error: %f, load: %f, active_cycles: %f, instr: %f)\n",
        src_is_2d, dst_is_2d, size, src_len, src_stride, dst_len, dst_stride, fc_freq, periph_freq, cycles, cycle_ref,
        bandwidth, bytes_per_cycle, error, load, active_cycles, instr);

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

    errors += bench_transfer(0, 0, 1<<10, 0, 0, 0, 0, 0, 0, 50, 50,  0,  CYCLE_ERROR_NOMINAL);
    errors += bench_transfer(0, 0, 1<<11, 0, 0, 0, 0, 0, 0, 50, 50,  0,  CYCLE_ERROR_NOMINAL);
    errors += bench_transfer(0, 0, 1<<12, 0, 0, 0, 0, 0, 0, 50, 50,  0,  CYCLE_ERROR_NOMINAL);
    errors += bench_transfer(0, 0, 1<<13, 0, 0, 0, 0, 0, 0, 50, 50,  0,  CYCLE_ERROR_NOMINAL);

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
