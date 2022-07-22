/* 
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

#define PRINTF(...) printf(__VA_ARGS__)

static struct pi_device aes_device;



static uint8_t aes_key_ecb_128[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

static uint8_t aes_key[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

static uint8_t src[32] = {
    0, 0, 0, 1,
    1, 1, 1, 2,
    2, 2, 2, 3,
    3, 3, 3, 4,
    0, 0, 0, 1,
    1, 1, 1, 2,
    2, 2, 2, 3,
    3, 3, 3, 4,
};
static uint8_t dst[32] = {0};
static uint8_t dst2[32] = {0};

/* result from OpenSSL encryption */
static uint8_t exp_enc[32] = {
    0xAF, 0x07, 0xF7, 0x8A,
    0xEC, 0xCC, 0xDD, 0x76,
    0xFB, 0x10, 0x08, 0x05,
    0xD7, 0x44, 0x7B, 0x07,
    0xAF, 0x07, 0xF7, 0x8A,
    0xEC, 0xCC, 0xDD, 0x76,
    0xFB, 0x10, 0x08, 0x05,
    0xD7, 0x44, 0x7B, 0x07
};
#define BUFFER_ALIGN_SIZE      ( 512 )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 2)

int test_aes_dc(void)
{
    char *rcv_buff;
    uint32_t hyper_buff;
    struct pi_device hyper;
    struct pi_hyperram_conf conf;
    pi_hyperram_conf_init(&conf);
    conf.ram.aes_conf.enabled = 1;
    conf.ram.aes_conf.mode = PI_AES_MODE_ECB;
    conf.ram.aes_conf.key_len = PI_AES_KEY_128;
    conf.ram.aes_conf.key = (uint32_t*) aes_key_ecb_128;
    conf.ram.aes_conf.iv = NULL;

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    pi_open_from_conf(&hyper, &conf);
    if (pi_ram_open(&hyper))
    {
        return 3;
    }

    if (pi_ram_alloc(&hyper, &hyper_buff, BUFFER_SIZE))
    {
        return 4;
    }

    printf("Writing data (on-the-fly encryption)\n");
    apb_soc_ctrl_feature_disablement_disable_crypto_set(apb_soc_ctrl,1);
    pi_watchdog_timer_set(16*1024);
    pi_watchdog_start();
    pi_ram_read(&hyper, hyper_buff, rcv_buff, BUFFER_SIZE);
    pi_watchdog_stop();

    pi_ram_free(&hyper, hyper_buff, BUFFER_SIZE);
    pi_ram_close(&hyper);

    return 0;
}

static int test_entry()
{
    uint8_t *buff, *rcv_buff;
    uint32_t hyper_buff = 0;
    struct pi_device ram = {0};
    struct pi_hyperram_conf hyper_conf = {0};
    printf("test_entry\n");

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }
    // open ram
    /* Init & open ram. */
    pi_hyperram_conf_init(&hyper_conf);
    (&hyper_conf)->xip_en = 1; // we want to use xip on this device
    (&hyper_conf)->baudrate = 50*1000*1000;
    printf("baudrate at hyper init %x\n",(&hyper_conf)->baudrate);
    pi_open_from_conf(&ram, &hyper_conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    uint32_t hyper_buff_raw = 0;
    if (pi_ram_alloc(&ram, &hyper_buff_raw, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", hyper_buff, (uint32_t) BUFFER_SIZE+BUFFER_ALIGN_SIZE);
    }
    hyper_buff = (hyper_buff_raw + BUFFER_ALIGN_SIZE) & (~(BUFFER_ALIGN_SIZE-1));
    printf("hyper_buff aligned = %x\n",hyper_buff);


    // Now, try to read, if no magic value, write it
    
    pi_ram_read(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");
    uint8_t need_write=0;
    /* Verification for normal read / write */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff[i])
        {
            need_write = 1;
            break;
        }
    }
    if(need_write)
    {
        pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
        printf("Write sync done.\n");
        pi_ram_free(&ram, hyper_buff_raw, BUFFER_SIZE+BUFFER_ALIGN_SIZE);
        pi_ram_close(&ram);
    }
    else
    {
        printf("test success\n");
        return 0;
    }
    int error = 0;
#if (TEST_QK == 1)
    pi_watchdog_timer_set(1024);
    pi_watchdog_start();   
    /* De-Clock Gating */
    apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 0;

    /* De-Reset IP */
    apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 1;

    /* Wait initialized */
    apb_soc_ctrl_feature_disablement_disable_crypto_set(apb_soc_ctrl,1);
    hal_quiddikey_start_allowed(0);
    pi_watchdog_stop();
#endif

#if (AES == 1)
    /* clock gating & reset */
    hal_udma_ctrl_reset_disable(UDMA_AES_DUAL_CORE_ID(0));
    hal_udma_ctrl_cg_disable(UDMA_AES_DUAL_CORE_ID(0));

    apb_soc_ctrl_feature_disablement_disable_crypto_set(apb_soc_ctrl,1);
    
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);
    conf.key_len = PI_AES_KEY_128;
    conf.key = (uint32_t*) aes_key;
    conf.iv = NULL;

    pi_open_from_conf(&aes_device, &conf);
    pi_aes_open(&aes_device);

    pi_watchdog_timer_set(16*1024);
    pi_watchdog_start();
    pi_aes_encrypt(&aes_device, (uint32_t*) src, (uint32_t*) dst, 8);
    pi_watchdog_stop();
#endif

#if (AES == 2)
    test_aes_dc();
#endif
    printf("Test Failure\n");
    return -1;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
