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

static uint8_t aes_key_ecb_256[32] = {
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B,
    0x1C, 0x1D, 0x1E, 0x1F,
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B,
    0x1C, 0x1D, 0x1E, 0x1F,
};

#define NB_CONFIGURATIONS (2)

static pi_aes_utils_conf_t aes_conf[NB_CONFIGURATIONS] = {
    {
        /* ECB 128 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_128,
        .key = (uint32_t*) aes_key_ecb_128,
        .iv = NULL,
    },
    {
        /* ECB 256 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_256,
        .key = (uint32_t*) aes_key_ecb_256,
        .iv = NULL,
    },
};

static uint8_t* aes_buff[2];
static uint8_t* buff[2];
static uint8_t* rcv_buff[2];
static uint32_t hyper_buff[2];
static struct pi_device hyper_naked;
static struct pi_device hyper_aes;

int test_configuration(pi_aes_utils_conf_t* aes_conf)
{

    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    memcpy(&(conf.ram.aes_conf), aes_conf, sizeof(pi_aes_utils_conf_t));

    printf("Allocate data\n");
    //TODO move buffer alloc outside this function to do it only once
    for (int i=0; i<2; i++)
    {
        buff[i] = pmsis_l2_malloc(BUFF_SIZE);
        if (buff[i] == NULL)
        {
            return 1;
        }

        aes_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
        if (aes_buff[i] == NULL)
        {
            return 1;
        }

        rcv_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
        if (rcv_buff[i] == NULL)
        {
            return 2;
        }
    }

    printf("Open hyper device\n");
    pi_open_from_conf(&hyper_aes, &conf);

    if (pi_ram_open(&hyper_aes))
    {
        return 3;
    }

    printf("Allocate ram data\n");
    if (pi_ram_alloc(&hyper_aes, &hyper_buff[0], BUFF_SIZE))
    {
        return 4;
    }

    if (pi_ram_alloc(&hyper_aes, &hyper_buff[1], BUFF_SIZE))
    {
        return 5;
    }

    printf("Initialize buffers\n");
    //TODO move buffer init outside this function to do it only once
    for (int i=0; i<BUFF_SIZE; i++)
    {
        buff[0][i] = i & 0x7f;
        buff[1][i] = i | 0x80;
    }

    printf("writing data (on-the-fly encryption)\n");
    pi_ram_write(&hyper_aes, hyper_buff[0], buff[0], BUFF_SIZE);
    pi_ram_write(&hyper_aes, hyper_buff[1], buff[1], BUFF_SIZE);

    printf("Verify data written in hyperram\n");
    {
        pi_device_t aes_device;
        /* open simple AES device */
        pi_aes_conf_t aes_single_conf;
        pi_aes_conf_init(&aes_single_conf);
        aes_single_conf.mode = aes_conf->mode;
        aes_single_conf.key_len = aes_conf->key_len;
        aes_single_conf.key = aes_conf->key;
        aes_single_conf.iv = aes_conf->iv;
        pi_open_from_conf(&aes_device, &aes_single_conf);
        if(PI_OK != pi_aes_open(&aes_device))
        {
            printf("Could not open AES device\n");
            return 7;
        }

        printf("Encrypting data using AES\n");
        pi_aes_encrypt(&aes_device, buff[0], aes_buff[0], BUFF_SIZE / 4);
        pi_aes_encrypt(&aes_device, buff[1], aes_buff[1], BUFF_SIZE / 4);

        printf("Retrieving encrypted data\n");
        // crashes with an xip_data_exception_handler !
        pi_ram_read(&hyper_naked, hyper_buff[0], rcv_buff[0], BUFF_SIZE);
        pi_ram_read(&hyper_naked, hyper_buff[1], rcv_buff[1], BUFF_SIZE);

        pi_aes_close(&aes_device);

        printf("Verifying encrypted data\n");
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < BUFF_SIZE; i++)
            {
                if (aes_buff[j][i] != rcv_buff[j][i])
                {
                    printf("Error, buffer: %d, index: %d, expected: 0x%x,"
                            "read: 0x%x\n", j, i, aes_buff[j][i], rcv_buff[j][i]);
                    return 6;
                }
            }
        }
        printf("data verified\n");
    }


    printf("reading data (on-the-fly decryption)\n");
    pi_ram_read(&hyper_aes, hyper_buff[0], rcv_buff[0], BUFF_SIZE);
    pi_ram_read(&hyper_aes, hyper_buff[1], rcv_buff[1], BUFF_SIZE);

    printf("verifying data\n");
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
    printf("data verified\n");

    pi_ram_free(&hyper_aes, hyper_buff[0], BUFF_SIZE);
    pi_ram_free(&hyper_aes, hyper_buff[1], BUFF_SIZE);
    pi_ram_close(&hyper_aes);

    return 0;
}

int test_entry()
{
    printf("Entering main controller (mode: hyperram)\n");

    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);

    printf("Open naked hyper device\n");
    pi_open_from_conf(&hyper_naked, &conf);

    if (pi_ram_open(&hyper_naked))
    {
        return 3;
    }

    for (uint8_t i = 0; i < NB_CONFIGURATIONS; i++)
    {
        printf("Testing configuration #%u\n", i);
        int result = test_configuration(&aes_conf[i]);
        if (0 != result)
        {
            printf("Error during test #%u\n", i);
            pmsis_exit(result);
        }
    }

    printf("closing naked hyper\n");
    pi_ram_close(&hyper_naked);

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
