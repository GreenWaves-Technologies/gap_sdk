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



PI_L2 static i2s_test_t itf_tests[ARCHI_UDMA_NB_I2S];

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
    int use_fast_clk, uint64_t fast_clk_freq)
{
    i2s_test_config_t config = {
        .itf=itf, .sampling_freq=sampling_freq, .full_duplex=full_duplex, .word_size=word_size, .nb_slots=nb_slots,
        .rx_slots=rx_slots, .tx_slots=tx_slots, .buffer_nb_elem=buffer_nb_elem, .elem_size=elem_size, .nb_samples=nb_samples,
        .is_ext_clk=ext_sck, .is_ext_ws=ext_ws, .is_sai0_clk=sai0_sck, .is_sai0_ws=sai0_ws, .clk_polarity=clk_pol, .ws_polarity=ws_pol,
        .rx_slots_width_0=rx_slots_width_0, .rx_slots_width_1=rx_slots_width_1, .tx_slots_width_0=tx_slots_width_0, .tx_slots_width_1=tx_slots_width_1, .rx_slots_format=rx_slots_format, .tx_slots_format=tx_slots_format,
        .use_fast_clk=use_fast_clk, .fast_clk_freq=fast_clk_freq, .ws_delay=1
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



static int test_entry()
{
    int errors = 0;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 300000000);

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

            if (sai0_sck)
            {
                // Activate the pull-down of the I2S1 and I2S2 CLK pads
                // to set them to '0' and avoid 'X' propagation in netlist tests
                if (i == 1)
                {
                    apb_soc_ctrl_padcfg13_set(ARCHI_APB_SOC_CTRL_ADDR, 1<<0);
                }
                else if (i == 2)
                {
                    apb_soc_ctrl_padcfg14_set(ARCHI_APB_SOC_CTRL_ADDR, 1<<0);
                }
            }

            if (init_itf_test(i, SAMPLING_FREQ, FULL_DUPLEX, WORD_SIZE, NB_SLOTS, RX_SLOTS, 
                        TX_SLOTS, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws, 
                        clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT, 
                        use_fast_clk, FAST_CLK_FREQ))
                return -1;
        }

        //i2s_reg_dump(i);
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
  return pmsis_kickoff((void *)test_kickoff);
}
