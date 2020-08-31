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
 * This example shows how to capture PDM samples on 4 microphones using 2 interfaces and dual-data rate (DDR).
 */

#include "pmsis.h"
#include <bsp/bsp.h>

#define NB_ELEM 256
#define BUFF_SIZE (NB_ELEM*2)

// We need 2 buffers per interface
static uint16_t buff[2][2][NB_ELEM];
// And one device per interface
static struct pi_device i2s[2];


static int test_entry()
{

    printf("Entering main controller\n");

    pi_bsp_init();

    // Get default I2S interface config
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    // Configure first interface for PDM 44100KHz DDR
    // Also gives the 2 buffers for double-buffering the sampling
    // PDM samples will be demodulated automatically by the driver
    i2s_conf.pingpong_buffers[0] = buff[0][0];
    i2s_conf.pingpong_buffers[1] = buff[0][1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = 44100;
    i2s_conf.itf = 0;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = 16;
    i2s_conf.channels = 2;

    pi_open_from_conf(&i2s[0], &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s[0]))
      return -1;

    // Configure second interface with same parameters but with different buffers
    i2s_conf.pingpong_buffers[0] = buff[1][0];
    i2s_conf.pingpong_buffers[1] = buff[1][1];
    i2s_conf.itf = 1;

    pi_open_from_conf(&i2s[1], &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s[1]))
      return -1;

    // Start sampling, the driver will use the double-buffers we provided to store
    // the incoming samples
    if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_START, NULL))
      return -1;

    if (pi_i2s_ioctl(&i2s[1], PI_I2S_IOCTL_START, NULL))
      return -1;

    short *chunk[2];
    unsigned int size;

    // Capture a few samples
    for (int i=0; i<3; i++)
    {
        // Once it returns, chunk will point to the next available buffer
        // containing samples.
        pi_i2s_read(&i2s[0], (void **)&chunk[0], &size);
        pi_i2s_read(&i2s[1], (void **)&chunk[1], &size);
    }

    // Now stop sampling
    pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_ioctl(&i2s[1], PI_I2S_IOCTL_STOP, NULL);

    // Close the driver
    pi_i2s_close(&i2s[0]);
    pi_i2s_close(&i2s[1]);

    // And display a few samples
    // In DDR mode, the 2 channels are interleaved one by one in the same buffer
    printf("Interface 0 / channel 0\n");
    for (int i=0; i<16; i++)
    {
      printf("  Sample %d: %d\n", i, chunk[0][i*2]);
    }
    printf("Interface 0 / channel 1\n");
    for (int i=0; i<16; i++)
    {
      printf("  Sample %d: %d\n", i, chunk[0][i*2+1]);
    }
    printf("Interface 1 / channel 0\n");
    for (int i=0; i<16; i++)
    {
      printf("  Sample %d: %d\n", i, chunk[1][i*2]);
    }
    printf("Interface 1 / channel 1\n");
    for (int i=0; i<16; i++)
    {
      printf("  Sample %d: %d\n", i, chunk[1][i*2+1]);
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
