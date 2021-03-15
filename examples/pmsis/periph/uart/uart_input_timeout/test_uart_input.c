/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 10 )

char l2_in[BUFFER_SIZE];
struct pi_device uart;
volatile uint8_t done = 0;
char *sentence = "Type your name(10 letters) :\n";

void uart_rx_cb(void *arg);
void uart_tx_cb(void *arg);

void uart_rx_cb(void *arg)
{
    pi_task_t *task = (pi_task_t *) arg;
    pi_task_callback(task, (void *) uart_tx_cb, NULL);
    sentence = (char *) pmsis_l2_malloc(sizeof(char) * (BUFFER_SIZE + 8));
    sprintf(sentence, "Coucou %s\n", l2_in);
    pi_uart_write_async(&uart, sentence, strlen(sentence), task);
}

void uart_tx_cb(void *arg)
{
    pi_task_t *task = (pi_task_t *) arg;
    done++;
    switch (done)
    {
    case 1 :
        pi_task_callback(task, (void *) uart_rx_cb, task);
        pi_uart_read_async(&uart, l2_in, BUFFER_SIZE, task);
        break;
    default :
        break;
    }
}

void test_uart_input(void)
{
    printf("Entering main controller\n");

    int32_t errors = 0;
    struct pi_uart_conf conf;

    /* Init & open uart. */
    pi_uart_conf_init(&conf);
    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.baudrate_bps = 115200;
    pi_open_from_conf(&uart, &conf);
    if (pi_uart_open(&uart))
    {
        printf("Uart open failed !\n");
        pmsis_exit(-1);
    }

    uint32_t timeout_us = 10000;
    int32_t timeout_id = pi_udma_timeout_alloc(PI_UDMA_TIMEOUT_MODE_TRANSFER);
    if (timeout_id == -1)
    {
        printf("Timeout alloc failed !\n");
        pmsis_exit(-2);
    }
    pi_uart_ioctl(&uart, PI_UART_IOCTL_ATTACH_TIMEOUT_RX, (void *) timeout_id);
    /* Write on uart then wait for data from uart. */
    #if (ASYNC)
    pi_task_t task = {0};
    pi_task_callback(&task, (void *) uart_tx_cb, &task);
    pi_uart_write_async(&uart, sentence, strlen(sentence), &task);
    while (done != 2)
    {
        pi_yield();
    }
    #else
    pi_uart_write(&uart, sentence, strlen(sentence));
    errors = pi_uart_read_timeout(&uart, l2_in, BUFFER_SIZE, timeout_us);
    if (errors == 0)
    {
        sentence = (char *) pmsis_l2_malloc(sizeof(char) * (BUFFER_SIZE + 8));
        sprintf(sentence, "Coucou %s\n", l2_in);
        pi_uart_write(&uart, sentence, strlen(sentence));
    }
    #endif  /* ASYNC */

    pi_uart_close(&uart);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Uart Input Test ***\n\n");
    return pmsis_kickoff((void *) test_uart_input);
}

