/*
 * Copyright (C) 2021 GreenWaves Technologies
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
 * This example shows how to trigger an interrupt with the Vesper VM3011 ZPL technology
 */

#include "pmsis.h"
#include <bsp/bsp.h>
#include "gaplib/wavIO.h"

#define I2S_CLK 44100
#define PDM_DEC 64
#define WORD_SIZE 16
#define BIT_RATE (PDM_DEC*I2S_CLK)

//Must be a multiple of 100ms
#define REC_DURATION_MS 3000

#define NB_ELEM ((I2S_CLK*(WORD_SIZE/8))/100)
#define BUFF_SIZE (NB_ELEM)
#define NB_BUFFERS (REC_DURATION_MS/10)
#define TOTAL_BUFF_SIZE (BUFF_SIZE*NB_BUFFERS)

static uint8_t buffers[TOTAL_BUFF_SIZE];
static uint8_t buff[2][BUFF_SIZE];
static struct pi_device i2s;
struct pi_i2s_conf i2s_conf;
pi_gpio_callback_t cb_gpio;
static pi_task_t task;
static struct pi_device gpio;

static volatile int num_iter_on_buffer=0;

static void copy_data(uint8_t *dst, uint8_t *src, uint size)
{
  for (uint i=0; i<size; i++) dst[i] = src[i];
}

static int test_entry()
{

    printf("Entering main controller\n");

    pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
    printf("Freq = %ld\n", pi_freq_get(PI_FREQ_DOMAIN_FC));

    // Get default I2S interface config    
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.pingpong_buffers[0] = buff[0];
    i2s_conf.pingpong_buffers[1] = buff[1];
    i2s_conf.block_size = BUFF_SIZE;
    i2s_conf.frame_clk_freq = I2S_CLK;
    i2s_conf.pdm_decimation = PDM_DEC;
    i2s_conf.itf = 0;
    i2s_conf.channels = 1;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_PDM;
    i2s_conf.word_size = WORD_SIZE;
    pi_open_from_conf(&i2s, &i2s_conf);

    // Open the driver
    if (pi_i2s_open(&i2s))
      return -1;


    //Configuring GPIO A0 on Pin A3 to receive wake up signal from Vesper mic
    struct pi_gpio_conf gpio_conf = {0};
    pi_gpio_conf_init(&gpio_conf);
    pi_open_from_conf(&gpio, &gpio_conf);
    if (pi_gpio_open(&gpio))
    {
        printf("Error opening GPIO\n");
        return -1;
    }

    pi_gpio_e gpio_in = PI_GPIO_A0_PAD_12_A3;
    pi_gpio_notif_e irq_type = PI_GPIO_NOTIF_RISE;
    pi_gpio_flags_e cfg_flags = PI_GPIO_INPUT|PI_GPIO_PULL_DISABLE|PI_GPIO_DRIVE_STRENGTH_LOW;

    /* Configure gpio input. */
    pi_pad_set_function(PI_PAD_12_A3_RF_PACTRL0, PI_PAD_12_A3_GPIO_A0_FUNC1);
    pi_gpio_pin_configure(&gpio, gpio_in, cfg_flags);
    pi_gpio_pin_notif_configure(&gpio, gpio_in, irq_type);

    //Run a start and stop to enable ZERO LISTENING on Vesper
    //The Zero Power Listening is enabled when pdm clock is off
    //To enable it for the first time we need to trigger the clock
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
      return -1;
    pi_time_wait_us(1);
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL))
      return -1;

    for(int iter = 0; iter<10; iter++)
    {
        pi_task_t gpio_block;
        pi_task_block(&gpio_block);
        pi_gpio_pin_task_add(&gpio, gpio_in,&gpio_block, irq_type);
        //Wating for Gpio to trigger interrupt
        pi_task_wait_on(&gpio_block);
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL);

        for (int iter_on_buffer=0;iter_on_buffer<NB_BUFFERS;iter_on_buffer++)
        {
            pi_task_t i2s_block;
            pi_task_block(&i2s_block);
            pi_i2s_read_async(&i2s, &i2s_block);
            pi_task_wait_on(&i2s_block);

            short *chunk;
            unsigned int size;
            pi_i2s_read_status(&i2s_block, (void **)&chunk, &size);
            copy_data((uint8_t *) buffers+BUFF_SIZE*iter_on_buffer, (uint8_t *) chunk, BUFF_SIZE);
        }
        
        //Save output to pc
        #ifdef WRITE_WAV
        char fn[100];
        sprintf(fn, "../../../out_file%03d.wav", iter);
        printf("Writing to %s....\n", fn);
        WriteWavToFile(fn, i2s_conf.word_size, i2s_conf.frame_clk_freq, i2s_conf.channels, buffers, TOTAL_BUFF_SIZE);
        printf("Finished writing\n");
        #endif //WRITE_WAV
        // Now stop sampling and put VM3011 to ZPL mode
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    }
    // Close the driver
    pi_i2s_close(&i2s);

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
