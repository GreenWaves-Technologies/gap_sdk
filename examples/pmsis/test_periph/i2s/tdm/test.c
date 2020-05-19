/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
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

/* This example shows how to use I2S TDM to smaple data from multiple channels.
 * It receives the samples from I2S channels and send them to the flash to
 * respect real-time constraints. Once the sampling is done, it reads back
 * the samples from flash and send them to a file on the workstation through
 * JTAG. */

#include "pmsis.h"
#include "bsp/bsp.h"
#include "wav_out.h"

//#define DUMP_SAMPLES 1
#define DUMP_WAV 1

#define NB_ELEM 256
#define WHOLE_SIZE (NB_ELEM*256*16)

#define ELEM_SIZE (sizeof(uint32_t))
#define BUFF_SIZE  (NB_ELEM * ELEM_SIZE)

#define NB_CHANNELS 4
#define NB_ACTIVE_CHANNELS 4

PI_L2 static uint8_t ch_buff[NB_CHANNELS][2][BUFF_SIZE];
static struct pi_device i2s;

static int test_entry()
{
    int32_t errors = 0;

    printf("Entering main controller\n");

    // First prepare the flash for storing the samples
    struct pi_device flash;
    struct pi_hyperflash_conf flash_conf;
    struct pi_flash_info flash_info;
    
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&flash, &flash_conf);

    if (pi_flash_open(&flash))
        return -1;

    pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *)&flash_info);
    uint32_t flash_addr = ((flash_info.flash_start + flash_info.sector_size - 1) & ~(flash_info.sector_size - 1));

    pi_flash_erase(&flash, flash_addr, WHOLE_SIZE);

    // Then prepare the I2S channel
    pi_bsp_init();

    // The configuration given when the driver is opened, is the global one
    // which applies to all channels like the number of channels, sampling rate
    // and so on.
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = 48000;
    i2s_conf.itf = 0;
    i2s_conf.word_size = 32;
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
        i2s_conf.options = PI_I2S_OPT_IS_RX | PI_I2S_OPT_PINGPONG | PI_I2S_OPT_ENABLED;
        i2s_conf.block_size = BUFF_SIZE;
        i2s_conf.pingpong_buffers[0] = ch_buff[i][0];
        i2s_conf.pingpong_buffers[1] = ch_buff[i][1];
        pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_CONF_SET, &i2s_conf);
    }

    // Start sampling.
    // Starting from there, the driver will alternate between the 2 pingpong
    // buffers for each channel.
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
        return -1;

    uint16_t *chunk;
    uint32_t size;
    uint32_t current_flash_addr = flash_addr;

    for (int j=0; j<WHOLE_SIZE; j++)
    {
        for (int i=0; i<(int) NB_ACTIVE_CHANNELS; i++)
        {
            // For each channel, wait for one buffer of sample and immediately
            // send it to flash. ANother buffer is already being filled in the
            // meantime.
            pi_i2s_channel_read(&i2s, i, (void **)&chunk, (size_t *)&size);
            pi_flash_program(&flash, current_flash_addr + i*WHOLE_SIZE, chunk, BUFF_SIZE);
        }

        j += size;
        current_flash_addr += size;
    }

    // Sampling is done, close everything
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s);

    printf("Finished sampling\n");

    // Finally, read back the samples from flash and send them to files
    // through JTAG
    uint16_t *temp_buff = (uint16_t *)ch_buff[0][0];
    for (int chan=0; chan<(int) 1; chan++)
    {
#ifdef DUMP_WAV
        char path[32];
        sprintf(path, "out_file_%d.wav", chan);
        dump_wav_open(path, i2s_conf.word_size, i2s_conf.frame_clk_freq,
                1, WHOLE_SIZE);
#endif

        current_flash_addr = flash_addr + 2048;
        int whole_size = WHOLE_SIZE - 2048;

        while (whole_size > 0)
        {
            int iter_size = BUFF_SIZE;
            if (iter_size > whole_size)
                iter_size = whole_size;
            
            pi_flash_read(&flash, flash_addr, temp_buff, iter_size);

#ifdef DUMP_SAMPLES
            printf("Interface %d / channel %d\n", i2s_conf.itf, chan);
            for (int i=0; i<4; i++)
            {
                printf("  Sample %d: %lx\n", i, temp_buff[i]);
            }
#endif

#ifdef DUMP_WAV
            dump_wav_write(temp_buff, iter_size);
#endif

            flash_addr += iter_size;
            whole_size -= iter_size;
        }

#ifdef DUMP_WAV
        dump_wav_close();
#endif
    }

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
    printf("PMSIS I2S TDM example\n");
    return pmsis_kickoff((void *) test_kickoff);
}
