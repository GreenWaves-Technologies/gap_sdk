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



static int init_itf_test(int test_id, int itf, int sampling_freq, int full_duplex, int word_size, int nb_slots, uint32_t rx_slots,
    uint32_t tx_slots, int buffer_nb_elem, int elem_size, int nb_samples, int ext_sck, int ext_ws, int sai0_sck, int sai0_ws,
    int clk_pol, int ws_pol, uint64_t rx_slots_width_0, uint64_t rx_slots_width_1, uint64_t tx_slots_width_0, uint64_t tx_slots_width_1, uint64_t rx_slots_format, uint64_t tx_slots_format,
    int use_fast_clk, uint64_t fast_clk_freq)
{
    i2s_test_config_t config = {
        .itf=itf, .sampling_freq=sampling_freq, .full_duplex=full_duplex, .word_size=word_size, .nb_slots=nb_slots,
        .rx_slots=rx_slots, .tx_slots=tx_slots, .buffer_nb_elem=buffer_nb_elem, .elem_size=elem_size, .nb_samples=nb_samples,
        .is_ext_clk=ext_sck, .is_ext_ws=ext_ws, .is_sai0_clk=sai0_sck, .is_sai0_ws=sai0_ws, .clk_polarity=clk_pol, .ws_polarity=ws_pol,
        .rx_slots_width_0=rx_slots_width_0, .rx_slots_width_1=rx_slots_width_1, .tx_slots_width_0=tx_slots_width_0, .tx_slots_width_1=tx_slots_width_1, .rx_slots_format=rx_slots_format, .tx_slots_format=tx_slots_format,
        .use_fast_clk=use_fast_clk, .fast_clk_freq=fast_clk_freq,
        .test_id=test_id
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

static int test_run (int nb_i2s)
{
    int errors = 0;

    for (int i=0; i<nb_i2s; i++)
    {
        if (start_itf_test(i))
        {
            return -1;
        }
    }

    for (int i=0; i<nb_i2s; i++)
        errors += wait_itf_test(i);

    for (int i=0; i<nb_i2s; i++)
    {
        if (stop_itf_test(i))
        {
            return -2;
        }
    }

    for (int i=0; i<nb_i2s; i++)
    {
        if (i2s_test_deinit(&itf_tests[i]))
            return -1;
    }

    return errors;
}


static int test_entry()
{
	int errors = 0;

	printf("Entering main controller\n");

	pi_freq_set(PI_FREQ_DOMAIN_FC, 300000000);

	// Common configs:
	int word_size = 16;
	int nb_slots = 16;
    int use_fast_clk = 0 ;
    int test_id = 0;

	for (int clk_pol = 0; clk_pol<2; clk_pol ++)
    {
        for(int ws_pol = 0; ws_pol<2; ws_pol ++)
        {
            printf("Test start with clk_pol %d, ws_pol %d\n", clk_pol, ws_pol);
            // ===============================
            // Test 1 half_duplex_int_clk_int_ws
            // slot 0: input, slot 2: output
            printf("Test half_duplex_int_clk_int_ws start \n");

            int nb_i2s = 1;
            int ext_sck = 0;
            int ext_ws = 0;
            int sai0_sck = 0 ;
            int sai0_ws = 0 ;
            int rx_slots = 1;
            int tx_slots = 1<<2;
            int full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots, tx_slots,
                            BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_int_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_int_clk_int_ws failed \n");
                return 1;
            }
            test_id ++;

            // ===============================
            // Test 2 half_duplex_ext_clk_int_ws
            // slot 0: input, slot 2: output
            printf("Test half_duplex_ext_clk_int_ws start\n");

            nb_i2s = 1;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_ext_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_ext_clk_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 3 half_duplex_int_clk_ext_ws
            // slot 0: input, slot 2: output

            printf("Test half_duplex_int_clk_ext_ws start\n");

            nb_i2s = 1;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;


            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_int_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_int_clk_ext_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 4 half_duplex_ext_clk_ext_ws
            // slot 0: input, slot 2: output

            printf("Test half_duplex_ext_clk_ext_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;


            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_ext_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_ext_clk_ext_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 5 full_duplex_int_clk_int_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_int_clk_int_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 0;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_int_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_int_clk_int_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 6 full_duplex_ext_clk_int_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_ext_clk_int_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_ext_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_ext_clk_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 7 full_duplex_int_clk_ext_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_int_clk_ext_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_int_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_int_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;

            // ===============================
            // Test 8 full_duplex_ext_clk_ext_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_ext_clk_ext_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_ext_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_ext_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 9 half_duplex_sai0_int_clk_int_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_int_clk_int_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_int_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_int_clk_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 10 half_duplex_sai0_ext_clk_int_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_ext_clk_int_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if (i) ext_sck = 0;
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_ext_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_ext_clk_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 11 half_duplex_sai0_int_clk_ext_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_int_clk_ext_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_int_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_int_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 12 half_duplex_sai0_ext_clk_ext_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_ext_clk_ext_ws init  \n" );

            nb_i2s = 1;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i) ext_sck=0;
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_ext_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_ext_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 13 half_duplex_sai0_int_clk_sai0_int_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_int_clk_sai0_int_ws  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_int_clk_sai0_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_int_clk_sai0_int_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 14 half_duplex_sai0_ext_clk_sai0_int_ws
            // slot 0: input, slot 2: output
            printf("Test half_duplex_sai0_ext_clk_sai0_int_ws  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_ext_clk_sai0_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_ext_clk_sai0_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 15 half_duplex_sai0_int_clk_sai0_ext_ws
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_int_clk_sai0_ext_ws  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;


            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_int_clk_sai0_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_int_clk_sai0_ext_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 16: half_duplex_sai0_ext_clk_sai0_ext_ws
            // slot 0: input, slot 2: output

            printf("Test half_duplex_sai0_ext_clk_sai0_ext_ws  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 1;
            tx_slots = 1<<2;
            full_duplex = 0;


            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test half_duplex_sai0_ext_clk_sai0_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test half_duplex_sai0_ext_clk_sai0_ext_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 17 full_duplex_sai0_int_clk_int_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_sai0_int_clk_int_ws  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_int_clk_int_ws failed to init\n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_int_clk_int_ws  failed to run \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 18 full_duplex_sai0_ext_clk_int_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_sai0_ext_clk_int_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (i) ext_sck = 0;
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_ext_clk_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_ext_clk_int_ws failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 19 full_duplex_sai0_int_clk_ext_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_sai0_int_clk_ext_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_int_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_int_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 20 full_duplex_sai0_ext_clk_ext_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_sai0_ext_clk_ext_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i) ext_sck = 0;
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_ext_clk_ext_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_ext_clk_ext_ws failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 21 full_duplex_sai0_int_clk_sai0_int_ws
            // All input and output, except slot 4 and 7 muteexit

            printf("Test full_duplex_sai0_int_clk_sai0_int_ws init  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_int_clk_sai0_int_ws failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_int_clk_sai0_int_ws  failed \n" );
                return 1;
            }


            test_id ++;
            // ===============================
            // Test 22 full_duplex_sai0_ext_clk_sai0_int_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test full_duplex_sai0_ext_clk_sai0_int_ws  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 0;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_ext_clk_sai0_int_ws  failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_ext_clk_sai0_int_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 23 full_duplex_sai0_int_clk_sai0_ext_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test full_duplex_sai0_int_clk_sai0_ext_ws  \n" );

            nb_i2s = 2;
            ext_sck = 0;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_int_clk_sai0_ext_ws  failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_int_clk_sai0_ext_ws  failed \n" );
                return 1;
            }

            test_id ++;
            // ===============================
            // Test 24 full_duplex_sai0_ext_clk_sai0_ext_ws
            // 2 SAI no internal routed
            // slot 0: input, slot 2: output

            printf("Test full_duplex_sai0_ext_clk_sai0_ext_ws  \n" );

            nb_i2s = 2;
            ext_sck = 1;
            ext_ws = 1;
            sai0_sck = 0 ;
            sai0_ws = 0 ;
            rx_slots = 0xFF6F;
            tx_slots = 0xFF6F;
            full_duplex = 1;

            for (int i=0; i<nb_i2s; i++)
            {
                if(i)
                {
                    sai0_sck = 1 ;
                    sai0_ws = 1 ;
                    clk_pol = 0;
                    ws_pol = 0;
                }
                if (init_itf_test(test_id, i, SAMPLING_FREQ, full_duplex, word_size, nb_slots, rx_slots,
                            tx_slots, BUFFER_NB_ELEM, ELEM_SIZE, NB_SAMPLE, ext_sck, ext_ws, sai0_sck, sai0_ws,
                            clk_pol, ws_pol, RX_SLOTS_WIDTH_0, RX_SLOTS_WIDTH_1, TX_SLOTS_WIDTH_0, TX_SLOTS_WIDTH_1, RX_SLOTS_FORMAT, TX_SLOTS_FORMAT,
                            use_fast_clk, FAST_CLK_FREQ))
                {
                    printf("Test full_duplex_sai0_ext_clk_sai0_ext_ws  failed \n");
                    return -1;
                }
            }

            errors += test_run(nb_i2s);
            if (errors)
            {
                printf("Test full_duplex_sai0_ext_clk_sai0_ext_ws  failed \n" );
                return 1;
            }

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
