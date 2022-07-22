/* PMSIS includes */
#include "pmsis.h"

#include "pmsis/drivers/udma_fifo.h"
#include "pmsis/drivers/aes.h"

/* --------- */
/* constants */
/* --------- */

#define MAX_PUSH_SIZE (1000)

/* --------- */
/* variables */
/* --------- */
static pi_device_t dummy_aes = {0}; /* to enable udma clock */

static pi_device_t udma_fifo_device = {0};
static pi_udma_fifo_conf_t conf = {0};

static uint32_t pushed_values[MAX_PUSH_SIZE];

/* ---------------- */
/* static functions */
/* ---------------- */

static int test_pushpop8_nominal(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;

    printf("Test push/pop 8 - size %d\n", push_size);

    for (uint64_t i = 0; i < push_size; i++)
    {
        pushed_values[i] = i * (UINT8_MAX / push_size);
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_PUSH8, &pushed_values[i]);
    }
    for (uint32_t j = 0; j < push_size; j++)
    {
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_POP8, &result);
        if (result != pushed_values[j])
        {
            printf("PUSH/POP-8: expected %d, got %d\n", pushed_values[j], result);
            return -1;
        }
    }
    return 0;
}

static int test_pushpop16_nominal(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;

    printf("Test push/pop 16 - size %d\n", push_size);

    for (uint64_t i = 0; i < push_size; i++)
    {
        pushed_values[i] = i * (UINT16_MAX / push_size);
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_PUSH16, &pushed_values[i]);
    }
    for (uint32_t j = 0; j < push_size; j++)
    {
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_POP16, &result);
        if (result != pushed_values[j])
        {
            printf("PUSH/POP-16: expected %d, got %d\n", pushed_values[j], result);
            return -1;
        }
    }
    return 0;
}

static int test_pushpop24_nominal(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;
    const uint32_t UINT24_MAX = ((1 << 24) - 1);

    printf("Test push/pop 24 - size %d\n", push_size);

    for (uint64_t i = 0; i < push_size; i++)
    {
        pushed_values[i] = i * (UINT24_MAX / push_size);
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_PUSH24, &pushed_values[i]);
    }
    for (uint32_t j = 0; j < push_size; j++)
    {
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_POP24, &result);
        if (result != pushed_values[j])
        {
            printf("PUSH/POP-24: expected %d, got %d\n", pushed_values[j], result);
            return -1;
        }
    }
    return 0;
}

static int test_pushpop32_nominal(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;

    printf("Test push/pop 32 - size %d\n", push_size);

    for (uint64_t i = 0; i < push_size; i++)
    {
        pushed_values[i] = i * (UINT32_MAX / push_size);
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_PUSH32, &pushed_values[i]);
    }
    for (uint32_t j = 0; j < push_size; j++)
    {
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_POP32, &result);
        if (result != pushed_values[j])
        {
            printf("PUSH/POP-32: expected %d, got %d\n", pushed_values[j], result);
            return -1;
        }
    }
    return 0;
}

/* main test */
static void test_push_pop(void)
{
    /* need to open an udma peripheral to enable clock in fifo channel */
    struct pi_device aes_device;
    struct pi_aes_conf aes_conf;
    pi_aes_conf_init(&aes_conf);
    pi_open_from_conf(&aes_device, &aes_conf);
    int ret = pi_aes_open(&aes_device);
    if (0 != ret)
    {
        printf("Could not open AES to enable udma clock\n");
        pmsis_exit(-1);
    }

    /* open a fifo channel */
    pi_udma_fifo_conf_init(&conf);
    pi_open_from_conf(&udma_fifo_device, &conf);
    ret = pi_udma_fifo_open(&udma_fifo_device);

    if (0 != ret)
    {
        printf("Could not open udma_fifo\n");
        pmsis_exit(-1);
    }

    for (uint32_t push_size = 5; push_size < MAX_PUSH_SIZE; push_size += 331)
    {
        /* resize the buffer to fit all data */
        uint32_t new_size = push_size * sizeof(uint32_t);
        pi_udma_fifo_ioctl(&udma_fifo_device, PI_UDMA_FIFO_SET_SIZE, &new_size);

        /* execute the push/pop test for 8 bits */
        ret = test_pushpop8_nominal(&udma_fifo_device, push_size);
        if (0 != ret)
        {
            printf("Test push/pop8 failed\n");
            pmsis_exit(-1);
        }
        /* execute the push/pop test for 16 bits */
        ret = test_pushpop16_nominal(&udma_fifo_device, push_size);
        if (0 != ret)
        {
            printf("Test push/pop16 failed\n");
            pmsis_exit(-1);
        }
        /* execute the push/pop test for 24 bits */
        ret = test_pushpop24_nominal(&udma_fifo_device, push_size);
        if (0 != ret)
        {
            printf("Test push/pop24 failed\n");
            pmsis_exit(-1);
        }
        /* execute the push/pop test for 32 bits */
        ret = test_pushpop32_nominal(&udma_fifo_device, push_size);
        if (0 != ret)
        {
            printf("Test push/pop32 failed\n");
            pmsis_exit(-1);
        }
    }

    /* free channels */
    pi_udma_fifo_close(&udma_fifo_device);

    /* close dummy device */
    pi_aes_close(&aes_device);

    pmsis_exit(0);
}

/* -------------- */
/* Program Entry. */
/* -------------- */
int main(void)
{
    printf("\n\n\t *** PMSIS UDMA FIFO Push/Pop Tests ***\n\n");
    return pmsis_kickoff((void *) test_push_pop);
}

