/**
 * AES unit test: ECB encrypt/decrypt with 256-bit key
 **/

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/drivers/aes.h"

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

static struct pi_device aes_device;

static uint8_t aes_key[32] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B,
    0x1C, 0x1D, 0x1E, 0x1F,
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
    0x32, 0x54, 0x6E, 0x20,
    0xE7, 0x2B, 0xA8, 0x5A,
    0x8A, 0xBF, 0x43, 0x69,
    0xA1, 0xEE, 0x8E, 0xA0,
    0x32, 0x54, 0x6E, 0x20,
    0xE7, 0x2B, 0xA8, 0x5A,
    0x8A, 0xBF, 0x43, 0x69,
    0xA1, 0xEE, 0x8E, 0xA0
};

static int aes_open(void)
{
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);
    conf.key_len = PI_AES_KEY_256;
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

    printf("Encrypt data\n");
    ret = pi_aes_encrypt(&aes_device, (uint32_t*) src, (uint32_t*) dst, 8);
    for (int i = 0; i < 32; i++)
    {
        if (dst[i] != exp_enc[i])
        {
            errors++;
            printf("ERROR: encrypted data invalid - dst: %02x, exp_enc: %02x\n", dst[i], exp_enc[i]);
        }
    }
    printf("\nDecrypting data\n");
    ret += pi_aes_decrypt(&aes_device, (uint32_t*) dst, (uint32_t*) dst2, 8);
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

