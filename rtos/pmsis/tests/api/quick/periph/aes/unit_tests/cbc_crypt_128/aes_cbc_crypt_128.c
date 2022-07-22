/**
 * AES unit test: CBC encrypt/decrypt with 128-bit key
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

static uint8_t aes_iv[16] = {
    0x00, 0x11, 0x22, 0x33,
    0x04, 0x15, 0x26, 0x37,
    0x08, 0x19, 0x2A, 0x3B,
    0x0C, 0x1D, 0x2E, 0x3F,
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
    0x84, 0x59, 0x9E, 0x37,
    0x17, 0x77, 0x4F, 0x41,
    0x64, 0xCC, 0x7D, 0xFF,
    0xD0, 0x97, 0x8E, 0x7F,
    0x8E, 0xDD, 0x4D, 0x6C,
    0x3A, 0xAF, 0xAF, 0x96,
    0x6B, 0xD9, 0xE2, 0x07,
    0x87, 0x7C, 0xF6, 0x51,
};

static int aes_open(void)
{
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);
    conf.mode = PI_AES_MODE_CBC;
    conf.key = (uint32_t*) aes_key;
    conf.iv = (uint32_t*) aes_iv;

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

