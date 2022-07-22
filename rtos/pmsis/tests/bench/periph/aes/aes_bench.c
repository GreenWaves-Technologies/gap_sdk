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

#define NB_ITERATIONS (2)

/* size is in block (ie 16 bytes) */
static int test_sizes[] = {
    1,
    10,
    100,
    200,
    500,
    1000,
};

#define NB_TESTS (sizeof(test_sizes)/sizeof(test_sizes[0]))

typedef struct
{
    int nb_iter;
    int encryption_total_cycles;
    int decryption_total_cycles;
} bench_result_t;

bench_result_t aes_ecb_128_results[NB_TESTS];
bench_result_t aes_ecb_256_results[NB_TESTS];
bench_result_t aes_cbc_128_results[NB_TESTS];
bench_result_t aes_cbc_256_results[NB_TESTS];

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
        //PRINTF("AES open succeeded\n");
    }
    else
    {
        PRINTF("AES open failed\n");
        pmsis_exit(-1);
    }

    return ret;
}

static int aes_close(struct pi_device* aes_device)
{
    pi_aes_close(aes_device);
    return 0;
}

static void aes_bench(pi_device_t* aes_device, bench_result_t results[])
{
    uint32_t perf = 0;
    for (size_t i = 0; i < NB_TESTS; i++)
    {
        //printf("test %d\n", i);
        const int size = 16 * test_sizes[i];
        /* alloc memory */
        uint32_t* src = pi_l2_malloc(size);
        uint32_t* dst = pi_l2_malloc(size);

        /* init bench cell */
        results[i].nb_iter = 0;
        results[i].encryption_total_cycles = 0;
        results[i].decryption_total_cycles = 0;

        /* bench loop */
        for (int j = 0; j < NB_ITERATIONS; j++)
        {
            //printf("iter %d size %d\n", j, size);
            /* encrypt benchmark */
            pi_perf_conf(1 << PI_PERF_CYCLES);
            pi_perf_reset();
            pi_perf_start();

            pi_aes_encrypt(aes_device, src, dst, size/4);

            pi_perf_stop();
            perf = pi_perf_read(PI_PERF_CYCLES);
            results[i].encryption_total_cycles += perf;

            /* decrypt benchmark */
            pi_perf_conf(1 << PI_PERF_CYCLES);
            pi_perf_reset();
            pi_perf_start();

            pi_aes_decrypt(aes_device, src, dst, size/4);

            pi_perf_stop();
            perf = pi_perf_read(PI_PERF_CYCLES);
            results[i].decryption_total_cycles += perf;

            results[i].nb_iter++;
        }

        /* free data */
        pi_l2_free(src, size);
        pi_l2_free(dst, size);
    }
}

void display_results(bench_result_t results[])
{
    for (size_t i = 0; i < NB_TESTS; i++)
    {
        printf("size (bytes) %5d - cycles: encryption %6d, decryption %6d\n",
                test_sizes[i] * 16,
                results[i].encryption_total_cycles / results[i].nb_iter,
                results[i].decryption_total_cycles / results[i].nb_iter
              );
    }
}

/**
 * \brief launch the main test
 *
 */
int test_main(void)
{
    PRINTF("Application start\n");

    int ret = 0;

    ret = aes_open(&aes_device_ecb_128, PI_AES_MODE_ECB, PI_AES_KEY_128, aes_key_ecb_128, NULL);
    ret += aes_open(&aes_device_ecb_256, PI_AES_MODE_ECB, PI_AES_KEY_256, aes_key_ecb_256, NULL);
    ret += aes_open(&aes_device_cbc_128, PI_AES_MODE_CBC, PI_AES_KEY_128, aes_key_cbc_128, iv_128);
    ret += aes_open(&aes_device_cbc_256, PI_AES_MODE_CBC, PI_AES_KEY_256, aes_key_cbc_256, iv_256);

    aes_bench(&aes_device_ecb_128, aes_ecb_128_results);
    aes_bench(&aes_device_ecb_256, aes_ecb_256_results);
    aes_bench(&aes_device_cbc_128, aes_cbc_128_results);
    aes_bench(&aes_device_cbc_256, aes_cbc_256_results);

    ret += aes_close(&aes_device_ecb_128);
    ret += aes_close(&aes_device_ecb_256);
    ret += aes_close(&aes_device_cbc_128);
    ret += aes_close(&aes_device_cbc_256);

    /* display results */

    printf("AES ECB 128 results:\n");
    display_results(aes_ecb_128_results);
    printf("AES ECB 256 results:\n");
    display_results(aes_ecb_256_results);
    printf("AES CBC 128 results:\n");
    display_results(aes_cbc_128_results);
    printf("AES CBC 256 results:\n");
    display_results(aes_cbc_256_results);

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

