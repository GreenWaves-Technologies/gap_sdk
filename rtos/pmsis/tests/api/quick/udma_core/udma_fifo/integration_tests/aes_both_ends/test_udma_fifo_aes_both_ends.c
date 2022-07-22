/**
 * AES unit test: CBC encrypt/decrypt with 256-bit key
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
    0x10, 0x21, 0x32, 0x43,
    0x14, 0x25, 0x36, 0x47,
    0x18, 0x29, 0x3A, 0x4B,
    0x1C, 0x2D, 0x3E, 0x4F,
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
static uint8_t dst3[32] = {0};
static uint8_t dst4[32] = {0};

/* result from OpenSSL encryption */
static uint8_t exp_enc[32] = {
    0x6A, 0x32, 0x56, 0xF6,
    0x1D, 0x19, 0xEB, 0x57,
    0x43, 0x43, 0xA0, 0xC4,
    0xB2, 0x7C, 0xF6, 0x2C,
    0xDB, 0x6B, 0x4E, 0x6E,
    0x3E, 0x11, 0xC7, 0x53,
    0xFC, 0xB2, 0xBB, 0x14,
    0x57, 0x2F, 0xFD, 0x80,
};

static int aes_open(void)
{
    struct pi_aes_conf conf;
    pi_aes_conf_init(&conf);
    conf.mode = PI_AES_MODE_CBC;
    conf.key_len = PI_AES_KEY_256;
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

    uint32_t fifo_input_id = 0;
    uint32_t fifo_output_id = 0;

    pi_device_t fifo_input = {0};
    pi_device_t fifo_output = {0};
    pi_udma_fifo_conf_t conf = {0};

    /* initialize fifo */
    pi_udma_fifo_conf_init(&conf);
    pi_open_from_conf(&fifo_input, &conf);
    pi_open_from_conf(&fifo_output, &conf);
    ret = pi_udma_fifo_open(&fifo_input);
    if (0 != ret)
    {
        printf("Could not open udma_fifo\n");
        pmsis_exit(-1);
    }
    ret = pi_udma_fifo_open(&fifo_output);
    if (0 != ret)
    {
        printf("Could not open udma_fifo\n");
        pmsis_exit(-1);
    }

    /* get IDs */
    pi_udma_fifo_ioctl(&fifo_input, PI_UDMA_FIFO_GET_ID, &fifo_input_id);
    pi_udma_fifo_ioctl(&fifo_output, PI_UDMA_FIFO_GET_ID, &fifo_output_id);

    printf("Input fifo id: %d\n", fifo_input_id);
    printf("Output fifo id: %d\n", fifo_output_id);

    /* encrypt data */
    pi_task_t block = {0};
    pi_task_block(&block);

    printf("Encrypt data while pushing data to a FIFO\n");
    ret = pi_aes_encrypt_async(&aes_device, (uint32_t*) fifo_input_id, (uint32_t*) dst, 8, &block);
    /* need to push data manually */
    for (int i = 0; i < 32; i++)
    {
        pi_udma_fifo_ioctl(&fifo_input, PI_UDMA_FIFO_PUSH8, &(src[i]));
    }
    for (int i = 0; i < 32; i++)
    {
        if (dst[i] != exp_enc[i])
        {
            errors++;
            printf("ERROR: encrypted data invalid - dst: %02x, exp_enc: %02x\n", dst[i], exp_enc[i]);
        }
    }

    /* decrypt data */
    /* => pop data manually */
    printf("\nDecrypting data while popping data from a FIFO\n");
    ret += pi_aes_decrypt_async(&aes_device, (uint32_t*) dst, (uint32_t*) fifo_output_id, 8, &block);

    for (int i = 0; i < 32; i++)
    {
        pi_udma_fifo_ioctl(&fifo_output, PI_UDMA_FIFO_POP8, &(dst2[i]));
    }
    pi_aes_ioctl(&aes_device, PI_AES_STOP_FIFO_MODE, NULL);

    for (int i = 0; i < 32; i++)
    {
        if (src[i] != dst2[i])
        {
            errors++;
            printf("ERROR: decrypted data invalid - expected %02x, got %02x\n", src[i], dst2[i]);
        }
    }

    /* do a simple decryption (without using the FIFO to push or pop)
     * to verify that we came back to a working state */
    printf("\nDecrypting data using classic linear channels\n");
    ret += pi_aes_decrypt(&aes_device, (uint32_t*) dst, (uint32_t*) dst3, 8);

    for (int i = 0; i < 32; i++)
    {
        if (src[i] != dst3[i])
        {
            errors++;
            printf("ERROR: decrypted data invalid - expected %02x, got %02x\n", src[i], dst3[i]);
        }
    }

    /* test the both fifo ends case */
    printf("\nDecrypting data using FIFO on both ends\n");
    ret += pi_aes_decrypt_async(&aes_device, (uint32_t*) fifo_input_id, (uint32_t*) fifo_output_id, 8, &block);

    for (int i = 0; i < 32; i++)
    {
        pi_udma_fifo_ioctl(&fifo_input, PI_UDMA_FIFO_PUSH8, &(dst[i]));
    }
    for (int i = 0; i < 32; i++)
    {
        pi_udma_fifo_ioctl(&fifo_output, PI_UDMA_FIFO_POP8, &(dst4[i]));
    }
    pi_aes_ioctl(&aes_device, PI_AES_STOP_FIFO_MODE, NULL);

    for (int i = 0; i < 32; i++)
    {
        if (src[i] != dst4[i])
        {
            errors++;
            printf("ERROR: decrypted data invalid - expected %02x, got %02x\n", src[i], dst4[i]);
        }
    }
    printf("\n");

    pi_udma_fifo_close(&fifo_input);
    pi_udma_fifo_close(&fifo_output);

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

