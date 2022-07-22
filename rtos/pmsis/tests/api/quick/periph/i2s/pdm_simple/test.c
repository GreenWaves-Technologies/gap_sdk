/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies * * Licensed under the Apache License, Version 2.0 (the "License");
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

#define DATA_SIZE       (64)                     //Datasize in bits
#define BLOCK_SIZE      (DATA_SIZE/8)

static struct pi_device i2s;
static uint8_t *buff[2];

static void i2s_reg_dump( unsigned char itf )
{
#define I2S_BASE       ( UDMA_I2S_ADDR(itf) )
    printf("I2S%d reg dump \n =============================================\n", itf);
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

static int open_testbench()
{
    // Plug I2S testbench verif to I2S interface
    pi_testbench_i2s_verif_config_t i2s_config = {.is_pdm=1 };
    if (pi_testbench_i2s_verif_open(pi_testbench_get(), 0, &i2s_config))
        return -1;

    // Start PDM iterator on sdi rising edge
    pi_testbench_i2s_verif_slot_config_t config_rx_0 = { .is_rx=1, .word_size=32 };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 0, &config_rx_0))
        return -1;

    pi_testbench_i2s_verif_slot_start_config_t start_config_rx_0 = { .type=PI_TESTBENCH_I2S_VERIF_PDM_RX_FILE_READER, .pdm_rx_file_reader= { .nb_samples=1024, .filepath=(uint32_t)"infile_0.txt" } };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 0, &start_config_rx_0))
        return -1;

    // Start PDM iterator on sdi falling edge
    pi_testbench_i2s_verif_slot_config_t config_rx_1 = { .is_rx=1, .word_size=32 };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 1, &config_rx_1))
        return -1;

    pi_testbench_i2s_verif_slot_start_config_t start_config_rx_1 = { .type=PI_TESTBENCH_I2S_VERIF_PDM_RX_FILE_READER, .pdm_rx_file_reader= { .nb_samples=1024, .filepath=(uint32_t)"infile_1.txt" } };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 1, &start_config_rx_1))
        return -1;

    // Start PDM iterator on sdo rising edge
    pi_testbench_i2s_verif_slot_config_t config_rx_2 = { .is_rx=1, .word_size=32 };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 2, &config_rx_2))
        return -1;

    pi_testbench_i2s_verif_slot_start_config_t start_config_rx_2 = { .type=PI_TESTBENCH_I2S_VERIF_PDM_RX_FILE_READER, .pdm_rx_file_reader= { .nb_samples=1024, .filepath=(uint32_t)"infile_2.txt" } };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 2, &start_config_rx_2))
        return -1;

    // Start PDM iterator on sdi falling edge
    pi_testbench_i2s_verif_slot_config_t config_rx_3 = { .is_rx=1, .word_size=32 };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 3, &config_rx_3))
        return -1;

    pi_testbench_i2s_verif_slot_start_config_t start_config_rx_3 = { .type=PI_TESTBENCH_I2S_VERIF_PDM_RX_FILE_READER, .pdm_rx_file_reader= { .nb_samples=1024, .filepath=(uint32_t)"infile_3.txt" } };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 3, &start_config_rx_3))
        return -1;

#if 0
    // Configure TX slot
    pi_testbench_i2s_verif_slot_config_t config_tx = { .is_rx=0, .word_size=32 };
    if (pi_testbench_i2s_verif_slot_open(pi_testbench_get(), 0, 0, &config_tx))
        return -1;

    // Start file dumper on TX slot
    pi_testbench_i2s_verif_slot_start_config_t start_config_tx = { .type=PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER, .tx_file_dumper= { .nb_samples=-1, .filepath=(uint32_t)"outfile.txt" } };
    if (pi_testbench_i2s_verif_slot_start(pi_testbench_get(), 0, 0, &start_config_tx))
        return -1;
#endif

    return 0;
}


static int open_i2s(struct pi_device *i2s)
{
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

    i2s_conf.frame_clk_freq = 2000000;         // In pdm mode, the frame_clk_freq = i2s_clk
    i2s_conf.itf = 0;                          // Which sai interface
    i2s_conf.format |= PI_I2S_FMT_DATA_FORMAT_PDM;     // Choose PDM mode
    i2s_conf.pdm_polarity = 0x3;                       // 2b'11 slave on both SDI and SDO (SDO under test)
    i2s_conf.pdm_diff = 0;                             // Set differential mode on pairs (under test)

    pi_open_from_conf(i2s, &i2s_conf);

    if (pi_i2s_open(i2s))
        return -1;

    //i2s_reg_dump(0);
    if (pi_i2s_ioctl(i2s, PI_I2S_IOCTL_START, NULL))
        return -1;

    return 0;
}

static int test_entry()
{
    printf("Entering main controller\n");

    
    if (open_testbench())
    {
        return -1;
    }

    if (open_i2s(&i2s))
    {
        return -1;
    }
    printf("I2S started\n");

    /* 
     * Due to the PDM samples directly stream to SFU 
     * on signal: pdm_rx_data_o
     */
    while(1);


    pi_i2s_ioctl(&i2s, PI_I2S_IOCTL_STOP, NULL);

    return 0;
}


void test_kickoff(void *arg)
{
  int ret = test_entry();
  printf("Test end with %d errors\n");
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
