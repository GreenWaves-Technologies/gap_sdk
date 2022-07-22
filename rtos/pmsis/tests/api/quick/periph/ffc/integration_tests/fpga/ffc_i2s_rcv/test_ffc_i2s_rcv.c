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
#include "testbench.h"

#define     TDM_INTERFACE       (0)
#define     NB_CHANNELS         (4) 
#define     FREQ                (44100) 
#define     WORD_SIZE           (32)

#define     MAX_FFC_SIZE        (4)
#define     START_VAL           (0x140000<<8)
#define     INCR                (0x2B)

#define BUFFER_SIZE (256)

static float fl_buffer[NB_CHANNELS][BUFFER_SIZE];
static uint32_t fix_buffer[NB_CHANNELS][BUFFER_SIZE]; //not used in theory

static int open_testbench()
{
    printf("Opening testbench\n");
    // Plug I2S testbench verif to I2S interface
    pi_testbench_i2s_verif_config_t i2s_config = {
        .word_size=32,
        .nb_slots=4,
        .is_full_duplex=0,
        .ws_delay=1
    };
    if (pi_testbench_i2s_verif_open(pi_testbench_get(), TDM_INTERFACE, &i2s_config))
    {
        return -1;
    }

    // Configure RX slot
    for(int i=0; i<NB_CHANNELS; i++)
    {
        pi_testbench_i2s_verif_slot_config_t config_rx = {
            .is_rx=1,
            .word_size=32,
            .format=1
        };
        if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), TDM_INTERFACE, i, &config_rx))
        {
            return -1;
        }

        // Start iterator on RX slot
        pi_testbench_i2s_verif_slot_start_config_t start_config_rx = {
            .type=PI_TESTBENCH_I2S_VERIF_RX_ITER,
            .rx_iter= {
                .nb_samples=-1,
                .incr_start=START_VAL,
                .incr_end = 0xFFFFFFFF,
                .incr_value=(INCR<<i),
            }
        };

        if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), TDM_INTERFACE, i, &start_config_rx))
        {
            return -1;
        }
    }

    return 0;
}

static int open_i2s(struct pi_device *i2s, uint8_t itf, uint8_t ch_size, uint32_t freq, uint8_t word_size)
{
    printf("Opening i2s interface\n");
    // First open I2S interface
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = freq;
    i2s_conf.itf = itf;
    i2s_conf.word_size = word_size;
    i2s_conf.channels = ch_size;
    i2s_conf.options = PI_I2S_OPT_TDM;

    pi_open_from_conf(i2s, &i2s_conf);

    if (pi_i2s_open(i2s))
    {
        printf("Failed to open I2S\n");
        return -1;
    }
    printf("I2S%d opened successfully, with %d slots, freq %d, word size %d\n", itf, ch_size, freq, word_size);
    return 0;
}

static int open_ffc(struct pi_device *ffc, int8_t ffc_id,
        pi_ffc_fixed_type_e fixed_type,
        uint32_t fixed_scale,
        uint32_t fixed_precision,
        pi_ffc_float_type_e float_type,
        pi_ffc_mode_e mode,
        pi_ffc_io_mode_e io_mode
        )
{
    printf("Opening FFC %d\n", ffc_id);

    pi_ffc_conf_t ffc_conf;
    pi_ffc_conf_init(&ffc_conf);

    ffc_conf.itf = ffc_id;
    ffc_conf.mode = mode;
    ffc_conf.io_mode = io_mode;
    ffc_conf.float_type = float_type;
    ffc_conf.fixed_type = fixed_type;
    ffc_conf.fixed_precision = fixed_precision;
    ffc_conf.fixed_scale = fixed_scale;

    pi_open_from_conf(ffc, &ffc_conf);

    /* open */
    if(PI_OK != pi_ffc_open(ffc))
    {
        printf("Failed to open FFC\n");
        return -1;
    }

    pi_ffc_ioctl(ffc, PI_FFC_IOCTL_SET_IO_MODE, (void*) PI_FFC_STREAM_IN_MEMORY_OUT);
    printf("FFC%d opened succesfully\n", ffc_id);
    return 0;
}

static int init_tdm_ch(
        pi_device_t *i2s,
        uint8_t channel_id,
        uint8_t stream_id
        )
{
    struct pi_i2s_channel_conf i2s_channel_conf;
    pi_i2s_channel_conf_init(&i2s_channel_conf);
    i2s_channel_conf.options = PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
    i2s_channel_conf.word_size = 32;
    i2s_channel_conf.block_size = BUFFER_SIZE;

#ifdef TDM_TEST
    i2s_channel_conf.options |= PI_I2S_OPT_MEM_SLAB;
    i2s_channel_conf.mem_slab = &fix_buffer[channel_id]; 
#else
    i2s_channel_conf.asrc_channel = 18 + stream_id; // Driver will add 0xe0, so put 16 to get 0xf2
#endif
    //i2s_channel_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S | PI_I2S_CH_FMT_DATA_ORDER_MSB;

    printf("Setting I2S configuration\n");
    if (pi_i2s_channel_conf_set(i2s, channel_id, &i2s_channel_conf))
    {
        printf("Failed to set i2s configuration via ioctl\n");
        return -1;
    }
    printf("I2S configuration set\n");
    return 0;
}


static int test_entry()
{
    printf("Entering main controller\n");
    pi_device_t i2s;
    pi_device_t ffc[MAX_FFC_SIZE];
    pi_task_t ffc_task[MAX_FFC_SIZE];

#ifdef RTL_TEST
    if (open_testbench())
    {
        printf("Failed to open testbench\n");
        return -1;
    }
#endif

    for(int i=0; i<MAX_FFC_SIZE; i++)
    {
        for(int j=0; j<BUFFER_SIZE; j++)
        {
            fl_buffer[i][j] = 0.567;
            fix_buffer[i][j] = 0;
        }
    }

    if (open_i2s(&i2s, TDM_INTERFACE, NB_CHANNELS, FREQ, WORD_SIZE))
    {
        printf("Failed to open I2S\n");
        return -1;
    }

#ifndef TDM_TEST
    for(int ffc_id=0; ffc_id<MAX_FFC_SIZE; ffc_id++)
    {
        open_ffc(
                &ffc[ffc_id], 
                ffc_id,
                PI_FFC_FIXED_32,
                8,
                23,
                PI_FFC_FLOAT_FP32,
                PI_FFC_FIXED_TO_FLOAT,
                PI_FFC_STREAM_IN_MEMORY_OUT
                );
    }
#endif

    for(int i=0; i<NB_CHANNELS; i++)
    {
        init_tdm_ch(
                &i2s,
                i,
                i
                );
    }

    
    printf("FFC convertion start\n");
    for(int i=0; i<MAX_FFC_SIZE; i++)
    {
        pi_task_block(&ffc_task[i]);
#ifdef TDM_TEST
        pi_i2s_channel_read_async(&i2s, i, &ffc_task[i]);
#else
        pi_ffc_convert_async(&ffc[i], NULL, &fl_buffer[i], BUFFER_SIZE, &ffc_task[i]);
#endif
    }

    // Start sampling, could be delayed until FFC is ready to not drop first samples
    if (pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_START, NULL))
    {
        printf("Failed to start I2S\n");
        return -1;
    }

    uint32_t *_fix_buffer[4];
    uint32_t size;
    for(int i=0; i<MAX_FFC_SIZE; i++) 
    {
        pi_task_wait_on(&ffc_task[i]);
#ifdef TDM_TEST
        pi_i2s_read_status(&ffc_task[i], (void **)&_fix_buffer[i], (size_t *)&size);
#endif
    }

    printf("FFC convert done %ld\n", size);
    
    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);

    for(int i=0; i<MAX_FFC_SIZE; i++)
    {
#ifdef RTL_TEST
        uint32_t val = START_VAL; 
        uint32_t incr = (INCR<<i);
        printf("check ch %d, start val %d, incr %d\n", i, val, incr);
        for(int j=0; j<BUFFER_SIZE; j++)
        {
            printf("val %lx -> fl val %f\n", val, fl_buffer[i][j]);
            val += incr;
        }
#else
        printf("check ch %d\n", i);
        for(int j=0; j<BUFFER_SIZE; j++)
        {
            if(fl_buffer[i][j])
            printf("(%d, %d) fix val %ld -> fl val %f\n", i, j, fix_buffer[i][j], fl_buffer[i][j]);
        }
#endif
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
    printf("========= PMSIS I2S/FFC RX TESTS =========\n");
    return pmsis_kickoff((void *)test_kickoff);
}
