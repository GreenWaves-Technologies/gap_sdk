/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
char hello[20];

void test_uart_helloworld(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;
    struct pi_device uart;
    struct pi_uart_conf conf;

    /* Init & open uart. */
    pi_uart_conf_init(&conf);
    conf.enable_tx = 1;
    conf.enable_rx = 0;
    conf.baudrate_bps = 115200;
    pi_open_from_conf(&uart, &conf);
    if (pi_uart_open(&uart))
    {
        printf("Uart open failed !\n");
        pmsis_exit(-1);
    }

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    sprintf(hello, "[%d %d] Hello World !\n", cluster_id, core_id);

    uint32_t timeout_us = 10000;
    int32_t timeout_id = pi_udma_timeout_alloc(PI_UDMA_TIMEOUT_MODE_TRANSFER);
    if (timeout_id == -1)
    {
        printf("Timeout alloc failed !\n");
        pmsis_exit(-2);
    }
    pi_uart_ioctl(&uart, PI_UART_IOCTL_ATTACH_TIMEOUT_TX, (void *) timeout_id);
    /* Write though uart. */
    #if (ASYNC)
    pi_task_t wait_task;
    pi_task_block(&wait_task);
    pi_task_timeout_set(&wait_task, timeout_us);
    pi_uart_write_async(&uart, hello, strlen(hello), &wait_task);
    pi_task_wait_on(&wait_task);
    errors = pi_task_transfer_end_result_get(&wait_task);
    #else
    errors = pi_uart_write_timeout(&uart, hello, strlen(hello), timeout_us);
    #endif  /* ASYNC */

    pi_uart_close(&uart);

    if (timeout_us && errors)
    {
        printf("Timeout reached %ld us ! Test failed with %ld remaining bytes...\n",
               timeout_us, errors);
        errors = 0;
    }
    else
    {
        errors = -6;
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Uart HelloWorld ***\n\n");
    return pmsis_kickoff((void *) test_uart_helloworld);
}
