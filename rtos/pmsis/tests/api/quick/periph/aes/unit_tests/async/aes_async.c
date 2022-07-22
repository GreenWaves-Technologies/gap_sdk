/**
 * AES unit test: ECB encrypt/decrypt with 128-bit key
 **/

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/drivers/aes.h"

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

static struct pi_device aes_device;

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


static volatile int done = 0;
static void __encrypt_callback(void* arg)
{
    printf("Encrypt callback!\n");
    done = 1;
}

static void __decrypt_callback(void* arg)
{
    printf("Decrypt callback!\n");
    done = 1;
}

static int aes_open(void)
{
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);
    conf.key = (uint32_t*) aes_key;
    conf.iv = NULL;

    pi_open_from_conf(&aes_device, &conf);
    int ret = pi_aes_open(&aes_device);

    if(!ret)
    {
        PRINTF("AES open succeeded\n");
    }
    else
    {
        PRINTF("AES open failed\n");
    }

    return ret;
}

static int aes_close(void)
{
    pi_aes_close(&aes_device);
    return 0;
}

static int aes_crypt(void)
{
    int ret = 0;
    int errors = 0;

    /* prepare async tasks */
    pi_task_t callback_encrypt;
    pi_task_callback(&callback_encrypt, __encrypt_callback, NULL);

    pi_task_t callback_decrypt;
    pi_task_callback(&callback_decrypt, __decrypt_callback, NULL);

    printf("Encrypt data\n");
    ret = pi_aes_encrypt_async(&aes_device, (uint32_t*) src, (uint32_t*) dst, 8, &callback_encrypt);

    while(!done)
    {
        pi_yield();
    }
    done = 0;

    for (int i = 0; i < 32; i++)
    {
        if (dst[i] != exp_enc[i])
        {
            errors++;
            printf("ERROR: encrypted data invalid - dst: %02x, exp_enc: %02x\n", dst[i], exp_enc[i]);
        }
    }
    printf("\nDecrypting data\n");

    ret += pi_aes_decrypt_async(&aes_device, (uint32_t*) dst, (uint32_t*) dst2, 8, &callback_decrypt);
    while(!done)
    {
        pi_yield();
    }

    for (int i = 0; i < 32; i++)
    {
        if (src[i] != dst2[i])
        {
            errors++;
            printf("ERROR: decrypted data invalid - expected %02x, got %02x\n", src[i], dst2[i]);
        }
    }
    printf("\n");
    return ret + errors;
}

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
int test_main(void)
{
    PRINTF("Application start\n");

    int ret = 0;

    ret = aes_open();

    ret += aes_crypt();

    ret += aes_close();

    if(ret)
    {
        printf("test returned with %d errors\n", ret);
    }
    else
    {
        printf("Test succeeded\n");
    }
    pmsis_exit(ret);
    while(1);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

