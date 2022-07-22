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

static volatile uint32_t trigger_counter = 0;

/* ---------------- */
/* static functions */
/* ---------------- */

static void event_callback_1(void *arg)
{
    (void) arg;
    trigger_counter++;
}

static int test_event_callback(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;

    printf("Test event_callback - size %d\n", push_size);

    printf("SubTest #1\n");
    /* create the callback */
    pi_task_t my_task = {0};
    pi_task_callback(&my_task, event_callback_1, NULL);

    /* set the callback */
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_CB, &my_task);

    /* enable the callback with a threshold */
    uint32_t threshold_value = push_size; // should be triggered sizeof(uint32_t) times
    const uint32_t expected_trigger_times = sizeof(uint32_t);
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_THRESHOLD, &threshold_value);


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

    if(trigger_counter != expected_trigger_times)
    {
        printf("Expected %d callback triggers, got %d\n", expected_trigger_times, trigger_counter);
        return -1;
    }

    printf("SubTest #2\n");
    /* disable the callback with a null threshold */
    threshold_value = 0;
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_THRESHOLD, &threshold_value);

    /* run the test a second time with callback disabled */
    /* if counter still increase, it is an error */
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

    if(trigger_counter != expected_trigger_times)
    {
        printf("Expected %d callback triggers, got %d\n", expected_trigger_times, trigger_counter);
        return -1;
    }

    printf("SubTest #3\n");
    /* enable the callback with a null threshold */
    threshold_value = push_size; // should be triggered sizeof(uint32_t) times
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_THRESHOLD, &threshold_value);
    /* but set callback to NULL */
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_CB, NULL);

    /* run the test a third time with a NULL callback*/
    /* if counter still increase, it is an error */
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

    if(trigger_counter != expected_trigger_times)
    {
        printf("Expected %d callback triggers, got %d\n", expected_trigger_times, trigger_counter);
        return -1;
    }

    return 0;
}

static int test_event_block(pi_device_t* fifo_dev, uint32_t push_size)
{
    uint32_t result = 0;

    printf("Test event_block - size %d\n", push_size);

    /* create the callback */
    pi_task_t my_task = {0};
    pi_task_block(&my_task);

    /* set the callback */
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_CB, &my_task);

    /* enable the callback with a threshold */
    uint32_t threshold_value = push_size; // should be triggered sizeof(uint32_t) times
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_THRESHOLD, &threshold_value);


    const uint32_t wait_value = push_size / sizeof(uint32_t);
    for (uint64_t i = 0; i < push_size; i++)
    {
        pushed_values[i] = i * (UINT32_MAX / push_size);
        pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_PUSH32, &pushed_values[i]);
        if ((i % wait_value) == (wait_value - 1))
        {
            printf("Waiting\n");
            pi_task_wait_on(&my_task);
            pi_task_block(&my_task);
        }
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

    /* disable the callback with a null threshold */
    threshold_value = 0; // should be triggered sizeof(uint32_t) times
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_THRESHOLD, &threshold_value);
    /* remove the callback */
    pi_udma_fifo_ioctl(fifo_dev, PI_UDMA_FIFO_SET_EVENT_CB, NULL);

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

    /* 1ST TEST */
    uint32_t push_size = 100;
    /* resize the buffer to fit all data */
    uint32_t new_size = push_size * sizeof(uint32_t);
    pi_udma_fifo_ioctl(&udma_fifo_device, PI_UDMA_FIFO_SET_SIZE, &new_size);

    /* execute the event callback test for 32 bits */
    ret = test_event_callback(&udma_fifo_device, push_size);
    if (0 != ret)
    {
        printf("Test event callback failed\n");
        pmsis_exit(-1);
    }

    /* 2ND TEST */
    push_size = 200;
    /* resize the buffer to fit all data */
    new_size = push_size * sizeof(uint32_t);
    pi_udma_fifo_ioctl(&udma_fifo_device, PI_UDMA_FIFO_SET_SIZE, &new_size);

    /* execute the event block test for 32 bits */
    ret = test_event_block(&udma_fifo_device, push_size);
    if (0 != ret)
    {
        printf("Test event callback failed\n");
        pmsis_exit(-1);
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
    printf("\n\n\t *** PMSIS UDMA FIFO Event callback Tests ***\n\n");
    return pmsis_kickoff((void *) test_push_pop);
}

