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
#define ARCHI_FC_EVT_SOC_EVT FC_IRQ_SOC_EVENT_FIFO_EVT
#endif

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

    printf("REF CLK MUX: %x \n", *(volatile unsigned int *)(long)(0x1a1040dc));
    printf("=============================================\n");
}



static int init_itf_test(int itf, int sampling_freq, int full_duplex, int word_size, int nb_slots, uint32_t rx_slots,
    uint32_t tx_slots, int buffer_nb_elem, int elem_size, int nb_samples, int ext_sck, int ext_ws, int sai0_sck, int sai0_ws,
    int clk_pol, int ws_pol, uint64_t rx_slots_width_0, uint64_t rx_slots_width_1, uint64_t tx_slots_width_0, uint64_t tx_slots_width_1, uint64_t rx_slots_format, uint64_t tx_slots_format, 
    int use_fast_clk, uint64_t fast_clk_freq, int mute, uint32_t rx_slots_bypass, uint32_t tx_slots_bypass, uint32_t ws_delay, uint32_t incr, uint64_t rx_frames, uint64_t tx_frames, uint64_t rx_slabs, uint64_t tx_slabs)
{
    i2s_test_config_t config = {
        .itf=itf, .sampling_freq=sampling_freq, .full_duplex=full_duplex, .word_size=word_size, .nb_slots=nb_slots,
        .rx_slots=rx_slots, .tx_slots=tx_slots, .buffer_nb_elem=buffer_nb_elem, .elem_size=elem_size, .nb_samples=nb_samples,
        .is_ext_clk=ext_sck, .is_ext_ws=ext_ws, .is_sai0_clk=sai0_sck, .is_sai0_ws=sai0_ws, .clk_polarity=clk_pol, .ws_polarity=ws_pol,
        .rx_slots_width_0=rx_slots_width_0, .rx_slots_width_1=rx_slots_width_1, .tx_slots_width_0=tx_slots_width_0, .tx_slots_width_1=tx_slots_width_1, .rx_slots_format=rx_slots_format, .tx_slots_format=tx_slots_format,
        .use_fast_clk=use_fast_clk, .fast_clk_freq=fast_clk_freq, .random_mute=mute, .rx_slots_bypass=rx_slots_bypass, .tx_slots_bypass=tx_slots_bypass,
        .ws_delay=ws_delay, .incr=incr, .rx_frames=rx_frames, .tx_frames=tx_frames, .rx_slabs=rx_slabs, .tx_slabs=tx_slabs
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

static int deinit_itf_test(int itf)
{
    return i2s_test_deinit(&itf_tests[itf]);
}



static int test_entry()
{
    int errors = 0;

    printf("STA\n");

    // Put FC at high speed, since it is quite busy doing verification in background
    pi_freq_set(PI_FREQ_DOMAIN_FC, 370000000);


    for (int j=0; j<NB_ITER; j++)
    {
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
                int ws_delay = (WS_DELAYS >> (i*4)) & 0xF;
                int sampling_freq = i == 0 ? SAMPLING_FREQ_0 : i == 1 ? SAMPLING_FREQ_1 : SAMPLING_FREQ_2;

#ifndef __FREERTOS__
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
#endif

                if (init_itf_test(i, sampling_freq, FULL_DUPLEX, WORD_SIZE, NB_SLOTS, RX_SLOTS, 
                            TX_SLOTS, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws, 
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT, 
                            use_fast_clk, FAST_CLK_FREQ, MUTE, RX_SLOTS_BYPASS, TX_SLOTS_BYPASS, ws_delay, INCR, RX_FRAMES, TX_FRAMES, RX_SLABS, TX_SLABS))
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

#if INJECT_STALL == 1
        // If  stalls are actove, inject one one iteration over 2
        if ((j & 1) == 0)
        {
            // Computes the number of cycles required to miss the refill of the 3 UDMA pending buffer and a bit more,
            // to make sure we trigger the error
            int time = (int64_t)BUFFER_NB_ELEM * 3 * 1000000 / SAMPLING_FREQ_0;

            // First wait before errors on first transfer is ignored
            pi_time_wait_us(time);

            // TO prevent the refill mask the i2s interrupt during the estimated time (one nop equal one cycle)
            pi_irq_mask_disable(ARCHI_FC_EVT_SOC_EVT);

            pi_time_wait_us(time);

            pi_irq_mask_enable(ARCHI_FC_EVT_SOC_EVT);
        }
#endif

        for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
        {
            if ((ITFS >> i) & 1)
            {
#if INJECT_STALL == 1
                // In case we are injecting stalls, account errors as usual if we are in the iteration
                // where we are not injecting, otherwise check that we get an error
                int iter_errors = wait_itf_test(i);
                int expected_error = __builtin_pulp_cnt(RX_SLOTS) + __builtin_pulp_cnt(TX_SLOTS);

#if RX_FRAMES != 0 || TX_FRAMES !=0
                if ((j & 1) == 0)
                {
                    if (iter_errors == 0)
                    {
                        errors++;
                    }
                }
#else
                if ((j & 1) == 0)
                {
                    if (iter_errors != expected_error)
                    {
                        errors++;
                    }
                }
#endif
                else
                {
                    errors += wait_itf_test(i);
                }
#else
                errors += wait_itf_test(i);
#endif
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

        for (int i=0; i<ARCHI_UDMA_NB_I2S; i++)
        {
            if ((ITFS >> i) & 1)
            {
                if (deinit_itf_test(i))
                    return -1;
            }
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
