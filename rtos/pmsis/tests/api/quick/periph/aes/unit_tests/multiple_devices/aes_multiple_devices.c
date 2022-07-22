/**
 * AES unit test: ECB encrypt/decrypt with 128-bit key
 **/

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/drivers/aes.h"

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

static struct pi_device aes_device_ecb_128;
static struct pi_device aes_device_ecb_256;
static struct pi_device aes_device_cbc_128;
static struct pi_device aes_device_cbc_256;

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

static uint8_t aes_key_cbc_128[16] = {
    0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B,
    0x2C, 0x2D, 0x2E, 0x2F,
};

static uint8_t iv_128[16] = {
    0x40, 0x41, 0x42, 0x43,
    0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B,
    0x4C, 0x4D, 0x4E, 0x4F,
};

static uint8_t aes_key_cbc_256[32] = {
    0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, 0x3E, 0x3F,
    0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, 0x3E, 0x3F,
};

static uint8_t iv_256[16] = {
    0x50, 0x51, 0x52, 0x53,
    0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B,
    0x5C, 0x5D, 0x5E, 0x5F,
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

static uint8_t enc_out[4][32] = {{0}, {0}, {0}, {0}};
static uint8_t dec_out[4][32] = {{0}, {0}, {0}, {0}};

/* result from OpenSSL encryption */
static uint8_t exp_enc[4][32] = {
    {
        0xAF, 0x07, 0xF7, 0x8A,
        0xEC, 0xCC, 0xDD, 0x76,
        0xFB, 0x10, 0x08, 0x05,
        0xD7, 0x44, 0x7B, 0x07,
        0xAF, 0x07, 0xF7, 0x8A,
        0xEC, 0xCC, 0xDD, 0x76,
        0xFB, 0x10, 0x08, 0x05,
        0xD7, 0x44, 0x7B, 0x07
    },
    {
        0x2F, 0xE4, 0xEC, 0x41,
        0x7D, 0x26, 0x02, 0x45,
        0xBB, 0x75, 0xC8, 0x24,
        0x99, 0xAE, 0x84, 0x64,
        0x2F, 0xE4, 0xEC, 0x41,
        0x7D, 0x26, 0x02, 0x45,
        0xBB, 0x75, 0xC8, 0x24,
        0x99, 0xAE, 0x84, 0x64
    },
    {
        0xF2, 0x97, 0xCA, 0x12,
        0x4A, 0x0D, 0xE6, 0xE9,
        0x9E, 0x81, 0x43, 0x84,
        0x70, 0x95, 0xCB, 0x3E,
        0xED, 0xDA, 0xB7, 0x20,
        0x61, 0x7D, 0x35, 0x9E,
        0x98, 0xC7, 0xB0, 0x4F,
        0xA4, 0xB5, 0x5F, 0x4E
    },
    {
        0x83, 0x79, 0xCA, 0xBC,
        0x88, 0x86, 0x9B, 0xAE,
        0x8E, 0xCD, 0x02, 0x42,
        0x8E, 0x06, 0x39, 0xF7,
        0x37, 0xCD, 0xAB, 0x1E,
        0x2D, 0xFC, 0xB0, 0xB1,
        0xF7, 0x1E, 0xA0, 0x9D,
        0xE7, 0x8A, 0x81, 0x2B
    },
};

static volatile int done = 0;
static void __encrypt_callback(void* arg)
{
    printf("Encrypt callback!\n");
    done += 1;
}

static void __decrypt_callback(void* arg)
{
    printf("Decrypt callback!\n");
    done += 1;
}

static int aes_open(struct pi_device* aes_device,
                    uint32_t mode,
                    uint32_t key_len,
                    uint8_t* key,
                    uint8_t* iv
                    )
{
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);

    conf.mode = mode;
    conf.key_len = key_len;
    conf.key = (uint32_t*) key;
    conf.iv = (uint32_t*) iv;

    pi_open_from_conf(aes_device, &conf);
    int ret = pi_aes_open(aes_device);

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

static int aes_close(struct pi_device* aes_device)
{
    pi_aes_close(aes_device);
    return 0;
}

static int aes_crypt(void)
{
    int ret = 0;
    int errors = 0;

    /* prepare async tasks */
    pi_task_t cb_enc_ecb_128;
    pi_task_t cb_enc_ecb_256;
    pi_task_t cb_enc_cbc_128;
    pi_task_t cb_enc_cbc_256;
    pi_task_callback(&cb_enc_ecb_128, __encrypt_callback, NULL);
    pi_task_callback(&cb_enc_ecb_256, __encrypt_callback, NULL);
    pi_task_callback(&cb_enc_cbc_128, __encrypt_callback, NULL);
    pi_task_callback(&cb_enc_cbc_256, __encrypt_callback, NULL);

    pi_task_t cb_dec_ecb_128;
    pi_task_t cb_dec_ecb_256;
    pi_task_t cb_dec_cbc_128;
    pi_task_t cb_dec_cbc_256;
    pi_task_callback(&cb_dec_ecb_128, __decrypt_callback, NULL);
    pi_task_callback(&cb_dec_ecb_256, __decrypt_callback, NULL);
    pi_task_callback(&cb_dec_cbc_128, __decrypt_callback, NULL);
    pi_task_callback(&cb_dec_cbc_256, __decrypt_callback, NULL);

    printf("Encrypt data\n");
    ret = pi_aes_encrypt_async(&aes_device_ecb_128, (uint32_t*) src, (uint32_t*) enc_out[0], 8, &cb_enc_ecb_128);
    ret = pi_aes_encrypt_async(&aes_device_ecb_256, (uint32_t*) src, (uint32_t*) enc_out[1], 8, &cb_enc_ecb_256);
    ret = pi_aes_encrypt_async(&aes_device_cbc_128, (uint32_t*) src, (uint32_t*) enc_out[2], 8, &cb_enc_cbc_128);
    ret = pi_aes_encrypt_async(&aes_device_cbc_256, (uint32_t*) src, (uint32_t*) enc_out[3], 8, &cb_enc_cbc_256);

    while(done < 4)
    {
        pi_yield();
    }
    done = 0;

    /* verify encryption results */
    for (int j = 0; j < 4; j++)
    {
        printf("Verifying enc_out[%d]\n", j);
        //TODO
        for (int i = 0; i < 32; i++)
        {
            if (exp_enc[j][i] != enc_out[j][i])
            {
                errors++;
                printf("ERROR: encrypted data invalid - exp_enc: %02x, enc_out: %02x\n", exp_enc[j][i], enc_out[j][i]);
            }
        }
    }

    printf("\nDecrypting data\n");
    ret = pi_aes_decrypt_async(&aes_device_ecb_128, (uint32_t*) enc_out[0], (uint32_t*) dec_out[0], 8, &cb_dec_ecb_128);
    ret = pi_aes_decrypt_async(&aes_device_ecb_256, (uint32_t*) enc_out[1], (uint32_t*) dec_out[1], 8, &cb_dec_ecb_256);
    ret = pi_aes_decrypt_async(&aes_device_cbc_128, (uint32_t*) enc_out[2], (uint32_t*) dec_out[2], 8, &cb_dec_cbc_128);
    ret = pi_aes_decrypt_async(&aes_device_cbc_256, (uint32_t*) enc_out[3], (uint32_t*) dec_out[3], 8, &cb_dec_cbc_256);

    while(done < 4)
    {
        pi_yield();
    }

    /* verify decryption results */
    for (int j = 0; j < 4; j++)
    {
        printf("Verifying dec_out[%d]\n", j);
        for (int i = 0; i < 32; i++)
        {
            if (src[i] != dec_out[j][i])
            {
                errors++;
                printf("ERROR: decrypted data invalid - expected %02x, got %02x\n", src[i], dec_out[j][i]);
            }
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

    ret = aes_open(&aes_device_ecb_128, PI_AES_MODE_ECB, PI_AES_KEY_128, aes_key_ecb_128, NULL);
    ret += aes_open(&aes_device_ecb_256, PI_AES_MODE_ECB, PI_AES_KEY_256, aes_key_ecb_256, NULL);
    ret += aes_open(&aes_device_cbc_128, PI_AES_MODE_CBC, PI_AES_KEY_128, aes_key_cbc_128, iv_128);
    ret += aes_open(&aes_device_cbc_256, PI_AES_MODE_CBC, PI_AES_KEY_256, aes_key_cbc_256, iv_256);

    ret += aes_crypt();

    ret += aes_close(&aes_device_ecb_128);
    ret += aes_close(&aes_device_ecb_256);
    ret += aes_close(&aes_device_cbc_128);
    ret += aes_close(&aes_device_cbc_256);

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

