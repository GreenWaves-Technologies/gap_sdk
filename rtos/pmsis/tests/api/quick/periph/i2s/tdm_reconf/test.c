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
#include "test.h"



typedef struct 
{
    struct pi_device i2s;
    i2s_slot_test_t slot_test_rx[16];
    i2s_slot_test_t slot_test_tx[16];
    pi_task_t slot_reconf_task[16];
    uint32_t tx_slots;
    uint32_t rx_slots;
    uint8_t is_ext_clk;
    uint8_t itf;
    int random_mute;
    uint8_t slot_state[16];
    int slot_usecase[16];
}
itf_test_t;


PI_L2 static itf_test_t itf_tests[ARCHI_UDMA_NB_I2S];


static void slot_reconf(int itf, int slot);


static void slot_reconf_handle(void *arg)
{
    slot_reconf(((int)arg) >> 16, ((int)arg) & 0xFFFF);
}


static void slot_init(int itf, int slot)
{
    itf_test_t *test = &itf_tests[itf];
    i2s_slot_new(&test->slot_test_rx[slot]);
}


static void slot_reconf(int itf, int slot)
{
    itf_test_t *test = &itf_tests[itf];
    test_desc_t *desc = &test_desc;
    itf_desc_t *itf_desc = &desc->itf_descs[itf];
    slot_desc_t *slot_desc = &itf_desc->slot_descs[slot];
    slot_usecase_t *usecase = &slot_desc->usecases[test->slot_usecase[slot]];
    int delay = 0;
    int enqueue = 1;

    if (test->slot_state[slot] == 0)
    {
        delay = usecase->unmuted_time;
        //printf("Unmuting itf %d slot %d delay %d width %d\n", itf, slot, delay, usecase->width);
        test->slot_state[slot]++;

        int slot_width = usecase->width;
        int slot_format = usecase->msb_first | (usecase->left_align << 1) | (usecase->sign_extend << 2);
        i2s_slot_config_t i2s_slot_config = {
            .itf=itf, .slot=slot, .is_rx=1, .word_size=slot_width, .nb_elem=BUFFER_NB_ELEM, .elem_size=ELEM_SIZE, .format=slot_format, .slot_disable=1
        };

        if (i2s_slot_init(NULL, &test->slot_test_rx[slot], &test->i2s, &i2s_slot_config))
            return;

        int iter = itf_desc->nb_slots;

        if (FULL_DUPLEX)
        {
            iter *= 2;
        }

        i2s_slot_start_config_t start_config = {
            .type=I2S_VERIF_RX_ITER,
            .nb_samples=64,
            .testbench_config={
                .type=PI_TESTBENCH_I2S_VERIF_RX_ITER,
                .rx_iter= { .nb_samples=-1, .incr_start=/* test->slot_usecase[slot] */ 0x1d + (slot*2+itf*32)*2, .incr_end=(1ULL << slot_width) - 1, .incr_value=iter*3 }
            }
        };

        if (i2s_slot_start(&test->slot_test_rx[slot], &start_config))
            return;

        pi_i2s_slots_enable(&test->i2s, 1<<slot, 1);

    }
    else if (test->slot_state[slot] == 1)
    {
        //printf("Muting itf %d slot %d delay %d\n", itf, slot, delay);
        test->slot_state[slot]++;

        enqueue = 0;

        pi_i2s_slots_stop_async(&test->i2s, 1<<slot, pi_task_callback(&test->slot_reconf_task[slot], slot_reconf_handle, (void *)((itf << 16) | slot)));
    }
    else if (test->slot_state[slot] == 2)
    {
        delay = usecase->muted_time;
        //printf("Muting2 itf %d slot %d delay %d\n", itf, slot, delay);
        test->slot_state[slot]++;

        if (i2s_slot_stop(&test->slot_test_rx[slot]))
            return;

        i2s_slot_deinit(&test->slot_test_rx[slot]);

    }
    else
    {
        delay = usecase->muted_time;
        //printf("DONE itf %d slot %d\n", itf, slot);
        test->slot_usecase[slot]++;

        if (test->slot_usecase[slot] < slot_desc->nb_usecases)
        {
            test->slot_state[slot] = 0;
        }
        else
        {
            enqueue = 0;
            i2s_slot_end(&test->slot_test_rx[slot]);
        }
    }

    if (enqueue)
    {
        pi_task_push_delayed_us(
            pi_task_callback(&test->slot_reconf_task[slot], slot_reconf_handle, (void *)((itf << 16) | slot)),
            delay
        );
    }
}



static int test_entry()
{
    int errors = 0;

    pi_freq_set(PI_FREQ_DOMAIN_FC, 300000000);

    test_desc_t *desc = &test_desc;

    for (int itf=0; itf<desc->nb_itf; itf++)
    {
        itf_desc_t *itf_desc = &desc->itf_descs[itf];
        itf_test_t *test = &itf_tests[itf];

        int ext_sck = (EXT_CLKS >> itf) & 1;
        int ext_ws = (EXT_WSS >> itf) & 1;
        int sai0_sck = (SAI0_CLKS >> itf) & 1;
        int sai0_ws = (SAI0_WSS >> itf) & 1;
        int clk_pol = (CLK_POLS >> itf) & 1;
        int ws_pol = (WS_POLS >> itf) & 1;
        int use_fast_clk = (USE_FAST_CLK >> itf) & 1;

        i2s_config_t config = {
            .itf=itf, .sampling_freq=itf_desc->sampling_freq, .full_duplex=FULL_DUPLEX,
            .word_size=WORD_SIZE, .nb_slots=itf_desc->nb_slots,
            .is_ext_clk=ext_sck, .is_ext_ws=ext_ws, .is_sai0_clk=sai0_sck,
            .is_sai0_ws=sai0_ws, .clk_polarity=clk_pol, .ws_polarity=ws_pol,
            .use_fast_clk=use_fast_clk, .fast_clk_freq=FAST_CLK_FREQ
        };

        if (i2s_init(&test->i2s, &config) == NULL)
            return -1;

        for (int slot=0; slot<16; slot++)
        {
            if ((RX_SLOTS >> slot) & 1)
            {
                test->slot_state[slot] = 0;
                test->slot_usecase[slot] = 0;
                test->slot_test_rx[slot].i2s = NULL;

                slot_init(itf, slot);
                slot_reconf(itf, slot);
            } 
        }
    }

    for (int itf=0; itf<desc->nb_itf; itf++)
    {
        itf_test_t *test = &itf_tests[itf];
        int ext_sck = (EXT_CLKS >> itf) & 1;

        if (ext_sck)
        {
            pi_testbench_i2s_verif_start(pi_testbench_get(), test->itf);
        }

        if (pi_i2s_ioctl(&test->i2s, PI_I2S_IOCTL_START, NULL))
            return -1;
    }

    for (int itf=0; itf<desc->nb_itf; itf++)
    {
        itf_test_t *test = &itf_tests[itf];

        for (int i=0; i<16; i++)
        {
            if ((RX_SLOTS >> i) & 1)
            {
                errors += i2s_slot_wait(&test->slot_test_rx[i]);
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
