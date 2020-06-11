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

/* This example shows how to use I2S to send data to a DAC and/or speaker. */

#include "pmsis.h"
#include "bsp/bsp.h"

#define NB_ELEM            ( 256 )
#define ELEM_SIZE          ( sizeof(uint16_t) )
#define BUFF_SIZE          ( NB_ELEM * ELEM_SIZE )

#define NB_CHANNELS        ( 1 )
#define NB_ACTIVE_CHANNELS ( 1 )

PI_L2 static uint8_t ch_buff[NB_CHANNELS][2][BUFF_SIZE];
static struct pi_device i2s;

static void buffer_init(void *buffer, uint32_t size)
{
    uint8_t *buff = (uint8_t *) buffer;
    for (uint32_t i=0; i<size; i++)
    {
        buff[i] = i | 0x55;
    }
}

static int test_entry()
{
    int32_t errors = 0;

    printf("Entering main controller\n");

    // The configuration given when the driver is opened, is the global one
    // which applies to all channels like the number of channels, sampling rate
    // and so on.
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = 44100;
    i2s_conf.itf = 0;
    i2s_conf.word_size = 32;
    i2s_conf.mem_word_size = 32;
    i2s_conf.channels = NB_CHANNELS;
    i2s_conf.options = PI_I2S_OPT_TDM;

    pi_open_from_conf(&i2s, &i2s_conf);
    errors = pi_i2s_open(&i2s);

    if (errors)
    {
        printf("Error opening i2s : %lx\n", errors);
        return -1;
    }

    // Now that the global configuration is given, configure each channel
    // independently.
    for (int i=0; i<(int) NB_ACTIVE_CHANNELS; i++)
    {
        // Enabled RX for slot i with pingpong buffers
        i2s_conf.channel_id = i;
        i2s_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED;
        i2s_conf.block_size = BUFF_SIZE;
        i2s_conf.pingpong_buffers[0] = ch_buff[i][0];
        i2s_conf.pingpong_buffers[1] = ch_buff[i][1];
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CONF_SET, &i2s_conf);

        /* Init data buffer to send. */
        buffer_init(ch_buff[i][0], BUFF_SIZE);
        buffer_init(ch_buff[i][1], BUFF_SIZE);
    }

    // Start sending.
    // Starting from there, the driver will alternate between the 2 pingpong
    // buffers for each channel.
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
        return -1;

    for (int i=0; i<(int) NB_ACTIVE_CHANNELS; i++)
    {
        /* Buffer arg is NULL because we are using pingpong buffers. */
        pi_i2s_channel_write(&i2s, i, NULL, BUFF_SIZE);
    }

    // Transfer is done, close everything
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s);

    if (errors)
        printf("TEST FAILURE\n");
    else
        printf("TEST SUCCESS\n");

    return errors;
}

void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    printf("PMSIS I2S Output example\n");
    return pmsis_kickoff((void *) test_kickoff);
}
