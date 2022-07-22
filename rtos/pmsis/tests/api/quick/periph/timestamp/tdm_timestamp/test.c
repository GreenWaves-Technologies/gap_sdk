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


#ifdef __FREERTOS__
#define ARCHI_UDMA_NB_I2S UDMA_NB_I2S
#endif

#define FIFO_SIZE       256

PI_L2 static i2s_test_t itf_tests[ARCHI_UDMA_NB_I2S];

static pi_device_t ts_fifo_device = {0};
static pi_udma_fifo_conf_t ts_fifo_conf = {0};
static uint32_t timestamp_val[FIFO_SIZE];
static pi_device_t i2s_timestamp;

void timestamp_reg_dump()
{
#define TS_BASE	( UDMA_TS_ADDR(0) )
    printf("TS BASE: %x \n",   TS_BASE);
    printf("reg CNT: %x \n",                *(volatile unsigned int *)(long)(TS_BASE+0x4));
    printf("reg REG_SETUP_CH7: %x \n",      *(volatile unsigned int *)(long)(TS_BASE+0x24));
    printf("reg EVT ID: %x \n",             *(volatile unsigned int *)(long)(TS_BASE+0x28));
    printf("reg CLK CFG: %x \n",            *(volatile unsigned int *)(long)(TS_BASE+0x2C));
    printf("reg CMP EVT: %x \n",            *(volatile unsigned int *)(long)(ARCHI_UDMA_ADDR + UDMA_CTRL_CFG_EVENT_OFFSET));
}

void i2s_reg_dump( unsigned char itf )
{
    printf("I2S%d reg dump \n =============================================\n", itf);
#define I2S_BASE	( UDMA_I2S_ADDR(itf) )
    printf("I2S BASE: 0x%8x \n",   I2S_BASE);
    for (int i=0; i<16; i++)
        printf("I2S SLOT_CFG_%d: 0x%8X\n", i, *(volatile unsigned int *)(long)(I2S_BASE + (i*0x4)));
    printf("I2S WORD_SIZE_0_1:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x40));
    printf("I2S WORD_SIZE_2_3:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x44));
    printf("I2S WORD_SIZE_4_5:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x48));
    printf("I2S WORD_SIZE_6_7:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x4C));
    printf("I2S WORD_SIZE_8_9:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x50));
    printf("I2S WORD_SIZE_10_11: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x54));
    printf("I2S WORD_SIZE_12_13: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x58));
    printf("I2S WORD_SIZE_14_15: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x5C));

    printf("I2S SLOT_EN: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x60));
    printf("I2S CLKCFG_SETUP: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x64));
    printf("I2S GLB_SETUP: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x68));
    printf("I2S CLK_FAST: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x6C));
    printf("=============================================\n");
}



static int init_itf_test(int itf, int sampling_freq, int full_duplex, int word_size, int nb_slots, uint32_t rx_slots,
    uint32_t tx_slots, int buffer_nb_elem, int elem_size, int nb_samples, int ext_sck, int ext_ws, int sai0_sck, int sai0_ws,
    int clk_pol, int ws_pol, uint64_t rx_slots_width_0, uint64_t rx_slots_width_1, uint64_t tx_slots_width_0, uint64_t tx_slots_width_1, uint64_t rx_slots_format, uint64_t tx_slots_format,
    int use_fast_clk, uint64_t fast_clk_freq, uint32_t ts_evt, uint32_t fifo_id)
{
    i2s_test_config_t config = {
        .itf=itf, .sampling_freq=sampling_freq, .full_duplex=full_duplex, .word_size=word_size, .nb_slots=nb_slots,
        .rx_slots=rx_slots, .tx_slots=tx_slots, .buffer_nb_elem=buffer_nb_elem, .elem_size=elem_size, .nb_samples=nb_samples,
        .is_ext_clk=ext_sck, .is_ext_ws=ext_ws, .is_sai0_clk=sai0_sck, .is_sai0_ws=sai0_ws, .clk_polarity=clk_pol, .ws_polarity=ws_pol,
        .rx_slots_width_0=rx_slots_width_0, .rx_slots_width_1=rx_slots_width_1, .tx_slots_width_0=tx_slots_width_0, .tx_slots_width_1=tx_slots_width_1, .rx_slots_format=rx_slots_format, .tx_slots_format=tx_slots_format,
        .use_fast_clk=use_fast_clk, .fast_clk_freq=fast_clk_freq, 
        .ts_evt=ts_evt, .timestamp=&i2s_timestamp, .fifo_id = fifo_id
    };

    if (i2s_test_init(&itf_tests[itf], &config))
        return -1;


    return 0;
}


static int start_itf_test(int itf)
{
    return i2s_test_start(&itf_tests[itf]);
}


static int wait_itf_test(int itf)
{
    return i2s_test_wait(&itf_tests[itf]);
}


static int stop_itf_test(int itf)
{
    return i2s_test_stop(&itf_tests[itf]);
}

static uint32_t init_timestamp_fifo(pi_device_t *fifo_timestamp, pi_udma_fifo_conf_t *conf)
{
    uint32_t fifo_id = 0;
    uint32_t timestamp_size = sizeof(uint32_t) * FIFO_SIZE;

    pi_udma_fifo_conf_init(conf);
    pi_open_from_conf(fifo_timestamp, conf);
    if (0 != pi_udma_fifo_open(fifo_timestamp))
    {
        printf("Could not open udma_fifo\n");
        return -1;
    }

    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_GET_ID, &fifo_id);
    pi_udma_fifo_ioctl(fifo_timestamp, PI_UDMA_FIFO_SET_SIZE, &timestamp_size);

    return fifo_id;
}

static int test_entry()
{
    int errors = 0;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 300000000);

    /* Set up timestamp */
    struct pi_timestamp_conf ts_conf;

    pi_timestamp_conf_init(&ts_conf);
    ts_conf.cnt_src = PI_TIMESTAMP_CNT_SOC_CLK;
    ts_conf.prescaler = 0x7F;
    pi_open_from_conf(&i2s_timestamp, &ts_conf);
    pi_udma_timestamp_open(&i2s_timestamp);
    pi_udma_timestamp_ioctl( &i2s_timestamp, PI_UDMA_TIMESTAMP_IOCTL_CLR, NULL);

    uint32_t timestamp_fifo_id = init_timestamp_fifo(&ts_fifo_device, &ts_fifo_conf);
    if (!timestamp_fifo_id)
    {
        printf("Timestamp fifo open failed\n");
        return -1;
    }
    
#if INPUT_TS_MODE == 1
    /* Set up timestamp input source */
    pi_timestamp_input_t ts_input;
    ts_input.dest_id = timestamp_fifo_id;
    ts_input.input_type = PI_TIMESTAMP_AUX_INPUT ;
    ts_input.input_sel = 8; //SAI0 
    pi_udma_timestamp_ioctl( &i2s_timestamp, PI_UDMA_TIMESTAMP_IOCTL_SET_INPUT, &ts_input);
#endif


    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        if ((ITFS >> i) & 1)
        {
            int ext_sck = (EXT_CLKS >> i) & 1;
            int ext_ws = (EXT_WSS >> i) & 1;
            int sai0_sck = (SAI0_CLKS >> i) & 1;
            int sai0_ws = (SAI0_WSS >> i) & 1;
            int clk_pol = (CLK_POLS >> i) & 1;
            int ws_pol = (WS_POLS >> i) & 1;
            int use_fast_clk = (USE_FAST_CLK >> i) & 1;
            if (init_itf_test(i, SAMPLING_FREQ, FULL_DUPLEX, WORD_SIZE, NB_SLOTS, RX_SLOTS,
                        TX_SLOTS, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                        clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                        use_fast_clk, FAST_CLK_FREQ, SOC_EVT_TS, timestamp_fifo_id))
                return -1;
        }

    }

    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        if ((ITFS >> i) & 1)
        {
            if (start_itf_test(i))
                return -1;
        }
    }

    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        if ((ITFS >> i) & 1)
        {
            errors += wait_itf_test(i);
        }
    }

    for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
    {
        if ((ITFS >> i) & 1)
        {
            if (stop_itf_test(i))
                return -1;
        }
    }

    uint32_t fifo_size=0;
    pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_GET_LEVEL, &fifo_size);
    if (fifo_size == 0) return -1;

    uint32_t ts_val[2][NB_SAMPLE/BUFFER_NB_ELEM] = {};
    int index[2] = {0};
    for (uint32_t i=0; i<(fifo_size/4); i++)
    {
        pi_udma_fifo_ioctl(&ts_fifo_device, PI_UDMA_FIFO_POP32, &timestamp_val[i]);
        int ts_id = (timestamp_val[i]>>24) & 0x3;
        if (3-ts_id)
            ts_val[(3-ts_id)][index[1]++] = timestamp_val[i];
        else
            ts_val[(3-ts_id)][index[0]++] = timestamp_val[i];
    }
    
    pi_udma_timestamp_close(&i2s_timestamp);

    /* Timestamp Check */
    uint32_t diff[2][NB_SAMPLE/BUFFER_NB_ELEM-1];
    for (int i=0; i<(NB_SAMPLE/BUFFER_NB_ELEM-1); i++)
    {
        diff[0][i] = ts_val[0][i+1] - ts_val[0][i];
        diff[1][i] = ts_val[1][i+1] - ts_val[1][i];
    }

    for (int i=0; i<(NB_SAMPLE/BUFFER_NB_ELEM-2); i++)
    {
        int d0 = __builtin_pulp_abs((diff[0][i] - diff[0][i+1]));
        int d1 = __builtin_pulp_abs((diff[1][i] - diff[1][i+1]));
        if (d0 > 1 || d1 > 1)
        {
            errors ++;
            printf("Time diff is d0 = %d , d1 = %d\n", d0, d1);
        }
    }

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
