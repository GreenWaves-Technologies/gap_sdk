/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "pmsis.h"
#include "stdio.h"
#include <bsp/bsp.h>
#include <bsp/ram/hyperram.h>

#define BUFF_SIZE 2048

static uint8_t aes_key_ecb_128[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

static char *buff[2];
static char *rcv_buff[2];
static uint32_t hyper_buff[2];
static struct pi_device hyper;

int test_entry()
{
    printf("Entering main controller (mode: hyperram)\n");

    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    conf.ram.aes_conf.enabled = 1;
    conf.ram.aes_conf.mode = PI_AES_MODE_ECB;
    conf.ram.aes_conf.key_len = PI_AES_KEY_128;
    conf.ram.aes_conf.key = (uint32_t*) aes_key_ecb_128;
    conf.ram.aes_conf.iv = NULL;

    for (int i=0; i<2; i++)
    {
        buff[i] = pmsis_l2_malloc(BUFF_SIZE);
        if (buff[i] == NULL)
        {
            return 1;
        }

        rcv_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
        if (rcv_buff[i] == NULL)
        {
            return 2;
        }
    }

    pi_open_from_conf(&hyper, &conf);

    if (pi_ram_open(&hyper))
    {
        return 3;
    }

    if (pi_ram_alloc(&hyper, &hyper_buff[0], BUFF_SIZE))
    {
        return 4;
    }

    if (pi_ram_alloc(&hyper, &hyper_buff[1], BUFF_SIZE))
    {
        return 5;
    }

    for (int i=0; i<BUFF_SIZE; i++)
    {
        buff[0][i] = i & 0x7f;
        buff[1][i] = i | 0x80;
    }

    printf("Writing data (on-the-fly encryption)\n");
    pi_ram_write(&hyper, hyper_buff[0], buff[0], BUFF_SIZE);
    pi_ram_write(&hyper, hyper_buff[1], buff[1], BUFF_SIZE);
    printf("Reading data (on-the-fly decryption)\n");
    pi_ram_read(&hyper, hyper_buff[0], rcv_buff[0], BUFF_SIZE);
    pi_ram_read(&hyper, hyper_buff[1], rcv_buff[1], BUFF_SIZE);

    printf("Verifying data\n");
    for (int j=0; j<2; j++)
    {
        for (int i=0; i<BUFF_SIZE; i++)
        {
            unsigned char expected;
            if (j == 0)
            {
                expected = i & 0x7f;
            }
            else
            {
                expected = i | 0x80;
            }
            if (expected != rcv_buff[j][i])
            {
                printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, rcv_buff[j][i]);
                return 6;
            }
        }
    }

    pi_ram_free(&hyper, hyper_buff[0], BUFF_SIZE);
    pi_ram_free(&hyper, hyper_buff[1], BUFF_SIZE);
    pi_ram_close(&hyper);

    printf("TEST SUCCESS\n");

    return 0;
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
