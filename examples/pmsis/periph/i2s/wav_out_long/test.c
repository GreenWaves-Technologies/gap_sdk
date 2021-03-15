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

#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

#define SAMPLE_RATE     (80000)
#define TEN_MS_SAMPLE_SIZE (SAMPLE_RATE*2/100) //each sample has 16 bits
#define BUFF_SIZE (TEN_MS_SAMPLE_SIZE)
#define FRAMES    (2000)                 //save 1000 frames 2 mics, so 5 seconds
#define L3_BUFF_SIZE    (FRAMES*BUFF_SIZE)                 

static uint8_t *buff[2];
static struct pi_device i2s;

static struct pi_device ram;
static uint32_t l3_buff;

static void dump_i2s_reg()
{
    //printf("UDMA_I2S ADDR : %x \n",         UDMA_I2S(0) );
    printf("reg CFG_CH0: %x \n",            *(volatile unsigned int *)(long)(0x1A102408));
    printf("reg CFG_CH1: %x \n",            *(volatile unsigned int *)(long)(0x1A102418));
    printf("reg CFG_EXT: %x \n",            *(volatile unsigned int *)(long)(0x1A102420));
    printf("reg CFG_CLKGEN0: %x \n",        *(volatile unsigned int *)(long)(0x1A102424));
    printf("reg CFG_CLKGEN1: %x \n",        *(volatile unsigned int *)(long)(0x1A102428));
    printf("reg CHMOD: %x \n",              *(volatile unsigned int *)(long)(0x1A10242C));
    printf("reg FLT_CH0: %x \n",            *(volatile unsigned int *)(long)(0x1A102430));
    printf("reg FLT_CH1: %x \n",            *(volatile unsigned int *)(long)(0x1A102434));
}

static int l3_init()
{
    /* Init & open ram. */
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        return -1;
    }

    if (pi_ram_alloc(&ram, &l3_buff, (uint32_t) L3_BUFF_SIZE))
    {
        printf("Ram malloc failed !\n");
        return -2;
    }
    printf("L3 buff alloc done: %x\n", l3_buff);
    return 0;
}

static void open_cl()
{
    struct pi_device cluster_dev = {0};
    struct pi_cluster_conf cl_conf = {0};

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
    }
    pi_freq_set(PI_FREQ_DOMAIN_CL, 175000000);
}

static int test_entry()
{
    printf("Entering main controller\n");

    pi_bsp_init();

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
    open_cl();
    printf("Freq = %ld\n", pi_freq_get(PI_FREQ_DOMAIN_FC));
    printf("Freq CL = %ld\n", pi_freq_get(PI_FREQ_DOMAIN_CL));

    buff[0] = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFF_SIZE);
    buff[1] = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFF_SIZE);

    if(buff[0] == NULL || buff[1] == NULL)
    {
        printf("I2S buffer malloc failed\n");
        return -1;
    }

    // Get default I2S interface config
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    // Configure first interface for PDM 44100KHz DDR
    // Also gives the 2 buffers for double-buffering the sampling
    i2s_conf.pingpong_buffers[0] = buff[0];
    i2s_conf.pingpong_buffers[1] = buff[1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = SAMPLE_RATE; //44100;
    i2s_conf.itf = 0;
    i2s_conf.channels = 2;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = 16;
    
#ifdef USE_CLK_EXT
    i2s_conf.options |= PI_I2S_OPT_EXT_CLK;
    i2s_conf.pdm_decimation = (2000000/SAMPLE_RATE);
#else
    i2s_conf.pdm_decimation = 40;
#endif
    i2s_conf.pdm_shift = 5;
  

    pi_open_from_conf(&i2s, &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s))
      return -1;

    if(l3_init())
    {
        printf("L3 init failed\n");
        return -1;
    }

    // Start sampling, the driver will use the double-buffers we provided to store
    // the incoming samples
#ifndef USE_CLK_EXT
    printf("Enable clk and wait 10s\n");
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CLOCK_ENABLE, NULL);
    pi_time_wait_us(10000000);
#endif
    printf("Capturing start\n");
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
      return -1;

    //dump_i2s_reg();
    //printf("wait a bit to see the wav perform\n");

    int16_t *chunk;
    unsigned int size;
    unsigned int index=0;

    // Capture a few samples
    for (int i=0; i<FRAMES; i++)
    {
        // Once it returns, chunk will point to the next available buffer
        // containing samples.
        pi_i2s_read(&i2s, (void **)&chunk, &size);
        pi_ram_write(&ram, (l3_buff+index), chunk, (uint32_t) size);
        index += size;
    }

    // Now stop sampling
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    printf("Capturing done...\n");

    // Close the driver
    pi_i2s_close(&i2s);

    pmsis_l2_malloc_free(buff[1], (uint32_t) BUFF_SIZE);
    pmsis_l2_malloc_free(buff[0], (uint32_t) BUFF_SIZE);

    printf("Generating WAV file, it may takes a while...\n");
    dump_wav("../../../out_file.wav", i2s_conf.word_size, i2s_conf.frame_clk_freq,
             i2s_conf.channels, (void *) l3_buff, L3_BUFF_SIZE, 1, &ram);
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
