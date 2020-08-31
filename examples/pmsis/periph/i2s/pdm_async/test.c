/*
 * Copyright (C) 2018 GreenWaves Technologies
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

/*
 * This example shows how to capture PDM samples asynchronously using callbacks
 */

#include "pmsis.h"
#include <bsp/bsp.h>

#define NB_ELEM 256
#define BUFF_SIZE (NB_ELEM*2)

static uint16_t buff[2][NB_ELEM];
static struct pi_device i2s;
static int end = 0;
static pi_task_t task;
static short *chunk;


// This callback is called when a transfer is finished
// Just reenqueue another buffer unless we are done
static void end_of_capture(void *arg)
{
    unsigned int size;

    pi_i2s_read_status(&task, (void **)&chunk, &size);

    end--;
    if (end)
        pi_i2s_read_async(&i2s, pi_task_callback(&task, end_of_capture, NULL));
}



static int test_entry()
{
    printf("Entering main controller\n");

    pi_bsp_init();

    // Get default I2S interface config
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    // Configure it for PDM 44100KHz
    // Also gives the 2 buffers for double-buffering the sampling
    // PDM samples will be demodulated automatically by the driver
    i2s_conf.pingpong_buffers[0] = buff[0];
    i2s_conf.pingpong_buffers[1] = buff[1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = 44100;
    i2s_conf.itf = 0;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = 16;

    pi_open_from_conf(&i2s, &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s))
      return -1;

    // Start sampling, the driver will use the double-buffers we provided to store
    // the incoming samples
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
      return -1;

    // Capture a few samples asynchronously
    end = 3;

    // The driver will push the task soon as the next buffer
    // containing samples is available
    pi_i2s_read_async(&i2s, pi_task_callback(&task, end_of_capture, NULL));

    // Wait until 3 buffers are ready
    while(*(volatile int *)&end != 0)
    {
        pi_yield();
    }

    // Now stop sampling
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);

    // Close the driver
    pi_i2s_close(&i2s);

    // And display a few samples
    for (int i=0; i<32; i++)
    {
      printf("Sample %d: %d\n", i, chunk[i]);
    }

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
