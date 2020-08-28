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
 * This example shows how to capture PDM samples and dump them to a WAV file on the workstation.
 */

#include "pmsis.h"
#include "wav_out.h"
#include <bsp/bsp.h>

#define NB_ELEM (8192*28)
#define BUFF_SIZE (NB_ELEM*2)
#define NB_BUFFERS 1
#define TOTAL_BUFF_SIZE (BUFF_SIZE*NB_BUFFERS)

static uint8_t buffers[TOTAL_BUFF_SIZE];
static struct pi_device i2s;

static int test_entry()
{
    pi_mem_slab_t mem_slab;

    printf("Entering main controller\n");

    pi_bsp_init();

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
    printf("Freq = %ld\n", pi_freq_get(PI_FREQ_DOMAIN_FC));

    // Get default I2S interface config
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    pi_mem_slab_init(&mem_slab, buffers, BUFF_SIZE, NB_BUFFERS);

    // Configure first interface for PDM 44100KHz DDR
    // Also gives the 2 buffers for double-buffering the sampling
    i2s_conf.mem_slab = &mem_slab;
    i2s_conf.options = PI_I2S_OPT_MEM_SLAB;
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = 44100;
    //i2s_conf.frame_clk_freq = 48000;
    i2s_conf.itf = 0;
    i2s_conf.channels = 2;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = 16;

    pi_open_from_conf(&i2s, &i2s_conf);

  printf("%s %d\n", __FILE__, __LINE__);
    // Open the driver
    if (pi_i2s_open(&i2s))
      return -1;
  printf("%s %d\n", __FILE__, __LINE__);

    // Start sampling, the driver will use the double-buffers we provided to store
    // the incoming samples
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
      return -1;

    short *chunk;
    unsigned int size;

    // Capture a few samples
    printf("Capturing data...\n");
    pi_time_wait_us(1000);

    for (int i=0; i<NB_BUFFERS; i++)
    {
        // Once it returns, chunk will point to the next available buffer
        // containing samples.
        pi_i2s_read(&i2s, (void **)&chunk, &size);
    }

    // Now stop sampling
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);

    // Close the driver
    pi_i2s_close(&i2s);

    dump_wav("../../../out_file.wav", i2s_conf.word_size, i2s_conf.frame_clk_freq,
             i2s_conf.channels, (void *)buffers, TOTAL_BUFF_SIZE);

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
