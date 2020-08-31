/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
char hello[20];

void test_uart_helloworld(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;
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

    /* Write though uart. */
    #if (ASYNC)
    pi_task_t wait_task = {0};
    pi_task_block(&wait_task);
    pi_uart_write_async(&uart, hello, strlen(hello), &wait_task);
    pi_task_wait_on(&wait_task);
    #else
    pi_uart_write(&uart, hello, strlen(hello));
    #endif

    pi_uart_close(&uart);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Uart HelloWorld ***\n\n");
    return pmsis_kickoff((void *) test_uart_helloworld);
}
