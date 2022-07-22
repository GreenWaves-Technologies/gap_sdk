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

#include "pmsis.h"
#include "testlib.h"


#define SAMPLING_FREQ 44100
#define BUFFER_SIZE (NB_SAMPLE * 4)
#define NB_SLOTS 8
#define MAX_NB_SLOTS 16

#define NB_I2S ARCHI_UDMA_NB_I2S

static pi_device_t i2s[NB_I2S];
static pi_task_t cb_tasks[NB_I2S][MAX_NB_SLOTS*2];
static uint32_t *buffers[NB_I2S][MAX_NB_SLOTS*2];

static volatile int waiting_slots;


static void handle_end_of_read(void *arg)
{
    //int itf = ((int)arg) & 0xFFFF;
    //int slot = (((int)arg) >> 16) & 0xFFFF;

   waiting_slots--;
}


static void handle_end_of_write(void *arg)
{
    //int itf = ((int)arg) & 0xFFFF;
    //int slot = (((int)arg) >> 16) & 0xFFFF;

    waiting_slots--;
}


static int test_entry()
{
    int errors = 0;

    printf("STA\n");

    waiting_slots = 0;

    // Put FC at high speed, since it is quite busy doing verification in background
    pi_freq_set(PI_FREQ_DOMAIN_FC, 300000000);


    // First enabled all slots of all interfaces
    for (int i=NB_I2S-1; i>=0; i--)
    {
        pi_testbench_i2s_verif_config_t i2s_config = {
            .sampling_freq=SAMPLING_FREQ, .word_size=32, .nb_slots=NB_SLOTS,
            .is_full_duplex=1, .is_ext_clk=i==0, .is_ext_ws=i==0, .is_sai0_clk=i!=0, .is_sai0_ws=i!=0, .ws_delay=1
        };

        if (pi_testbench_i2s_verif_open(pi_testbench_get(), i, &i2s_config))
            return -1;

        struct pi_i2s_conf i2s_conf;
        pi_i2s_conf_init(&i2s_conf);

        i2s_conf.frame_clk_freq = SAMPLING_FREQ;
        i2s_conf.itf = i;
        i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;
        i2s_conf.word_size = 32;
        i2s_conf.channels = 8;
        i2s_conf.options = PI_I2S_OPT_TDM | PI_I2S_OPT_EXT_WS | PI_I2S_OPT_EXT_CLK | PI_I2S_OPT_FULL_DUPLEX;
        
        if (i != 0)
        {
            i2s_conf.options |= PI_I2S_OPT_EXT_CLK_INT_ROUTED | PI_I2S_OPT_EXT_WS_INT_ROUTED;
        }

        pi_open_from_conf(&i2s[i], &i2s_conf);

        if (pi_i2s_open(&i2s[i]))
            return -1;

        for (int j=0; j<16; j++)
        {
            if ((j < 8 && j >= NB_SLOTS) || (j >= 8 && j >= 8 + NB_SLOTS))
                continue;

            int is_rx = j < 8;
            int slot = j % 8;
            pi_testbench_i2s_verif_slot_config_t config = { .is_rx=is_rx, .word_size=32, .format=1 };
            if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), i, slot, &config))
                return -1;

            if (is_rx)
            {
                pi_testbench_i2s_verif_slot_start_config_t start_config = { .type=PI_TESTBENCH_I2S_VERIF_RX_ITER, .rx_iter={ .nb_samples=-1, .incr_start=0, .incr_end=0xffffffff, .incr_value=1}};
                if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), i, slot, &start_config))
                    return -1;
            }
            else
            {
                char filename[64];
                sprintf(filename, "outfile_test_itf%d_slot%d.txt", i, slot);
                pi_testbench_i2s_verif_slot_start_config_t start_config = { .type=PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER, .tx_file_dumper={ .nb_samples=-1, .filepath=(uint32_t)filename}};
                if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), i, slot, &start_config))
                    return -1;
            }

            waiting_slots++;

            struct pi_i2s_channel_conf channel_conf;
            pi_i2s_channel_conf_init(&channel_conf);

            if (j < 8)
            {
                channel_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
            }
            else
            {
                channel_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED;
            }

            channel_conf.pingpong_buffers[0] = pi_l2_malloc(BUFFER_SIZE);
            if (channel_conf.pingpong_buffers[0] == NULL)
            {
                printf("Failed to allocate\n");
                return -1;
            }
            channel_conf.pingpong_buffers[1] = pi_l2_malloc(BUFFER_SIZE);
            if (channel_conf.pingpong_buffers[1] == NULL)
            {
                printf("Failed to allocate\n");
                return -1;
            }

            buffers[i][j] = channel_conf.pingpong_buffers[0];

            if (!is_rx)
            {
                for (int k=0; k<NB_SAMPLE; k++)
                {
                    ((uint32_t *)channel_conf.pingpong_buffers[0])[k] = k;
                    ((uint32_t *)channel_conf.pingpong_buffers[1])[k] = NB_SAMPLE + k;
                }
            }

            channel_conf.block_size = BUFFER_SIZE;
            channel_conf.word_size = 32;
            channel_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S | PI_I2S_CH_FMT_DATA_ORDER_MSB;

            if (pi_i2s_channel_conf_set(&i2s[i], slot, &channel_conf))
                return -1;


            if (is_rx)
            {
                pi_i2s_channel_read_async(&i2s[i], slot, pi_task_callback(&cb_tasks[i][j], handle_end_of_read, (void *)(i || (j<<16))));
            }
            else
            {
                pi_i2s_channel_write_async(&i2s[i], slot, NULL, BUFFER_SIZE, pi_task_callback(&cb_tasks[i][j], handle_end_of_write, (void *)(i || (j<<16))));
            }


        }

        pi_testbench_i2s_verif_start(pi_testbench_get(), i);
    }





#if defined(USE_GLOBAL_ENABLE)

    while (waiting_slots)
    {
        int itf_team = (1 << NB_I2S) - 1;
        if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
            return -1;
            
        for (int i=NB_I2S-1; i>=0; i--)
        {
            if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
                return -1;
            
            pi_time_wait_us(100);
        }

        pi_time_wait_us(500);

        if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
            return -1;

        for (int i=NB_I2S-1; i>=0; i--)
        {
            if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_STOP, NULL))
                return -1;
        }
    }

    for (int i=0; i<NB_I2S; i++)
    {
        for (int j=0; j<NB_SLOTS; j++)
        {
            for (int k=0; k<NB_SAMPLE; k++)
            {
                if (buffers[0][0][k] != buffers[i][j][k])
                {
                    printf("  Detected error (itf: %d, slot: %d, index: %d, expected: %d, got: %d)\n", i, j, k, buffers[0][0][k], buffers[i][j][k]);
                    return -1;
                }
            }
        }
    }

#else

    int itf_team = (1 << NB_I2S) - 1;
    if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
        return -1;
        
    for (int i=NB_I2S-1; i>=0; i--)
    {
        if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
            return -1;
    }


    while (waiting_slots)
    {
        int itf_team = (1 << NB_I2S) - 1;
        if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
            return -1;

        for (int i=0; i<NB_I2S; i++)
        {
            for (int j=0; j<8; j+=2)
            {
                pi_i2s_slots_enable(&i2s[i], 1<<j, 0);
            }
        }
   
        for (int i=NB_I2S-1; i>=0; i--)
        {
            if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
                return -1;
        }

        pi_time_wait_us(100);

        if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
            return -1;

        for (int i=0; i<NB_I2S; i++)
        {
            for (int j=0; j<8; j+=2)
            {
                pi_i2s_slots_enable(&i2s[i], 1<<j, 1);
                pi_time_wait_us(20);
            }
        }

        if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_START, NULL))
            return -1;

        pi_time_wait_us(500);

    }

    if (pi_i2s_ioctl(&i2s[0], PI_I2S_IOCTL_SYNC, (void *)itf_team))
        return -1;

    for (int i=NB_I2S-1; i>=0; i--)
    {
        if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_STOP, NULL))
            return -1;
    }

    for (int i=0; i<NB_I2S; i++)
    {
        for (int j=0; j<NB_SLOTS; j++)
        {
            for (int k=0; k<NB_SAMPLE; k++)
            {
                if ((j & 1) == 0)
                {
                    if (buffers[0][0][k] != buffers[i][j][k])
                    {
                        printf("Detected error (itf: %d, slot: %d, index: %d, expected: %d, got: %d)\n", i, j, k, buffers[0][0][k], buffers[i][j][k]);
                        return -1;
                    }
                }
                else
                {
                    if (buffers[0][1][k] != buffers[i][j][k])
                    {
                        printf("Detected error (itf: %d, slot: %d, index: %d, expected: %d, got: %d)\n", i, j, k, buffers[0][1][k], buffers[i][j][k]);
                        return -1;
                    }
                }
            }
        }
    }

#endif

    for (int i=0; i<NB_I2S; i++)
    {
        i2s_deinit(&i2s[i], i);
    }


    // Then check slot enable
    // Keep half the slots always enabled and mute/unmute the others synchronized together
    // with a delay between the activation of each interface

    if (errors)
      printf("TKO\n");
    else
      printf("TOK\n");

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
