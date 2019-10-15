#include "AT_RING_BUFFER.h"

L2_MEM rt_event_sched_t sched;

#define GAP_RING_BUFFER_SIZE 256

RT_L2_DATA uint8_t rx_buffer[GAP_RING_BUFFER_SIZE];

rt_uart_t *uart;

int read_index = 0;

static void irq_rx(void *arg)
{
    rt_uart_read(uart, rx_buffer, GAP_RING_BUFFER_SIZE, rt_event_get(&sched, irq_rx, 0));
}

static int rx_remain_size()
{
    return pulp_read32(ARCHI_UDMA_ADDR + UDMA_UART_OFFSET(0) + UDMA_CHANNEL_RX_OFFSET + UDMA_CHANNEL_SIZE_OFFSET);
}

int AT_COMMAND_Init(int baudrate)
{
    /* Initialize a event scheduler which controls the execution of events */
    rt_event_sched_init(&sched);

    /* Allocate 4 event elements in the scheduler */
    if (rt_event_alloc(&sched, 4)) return -1;

    rt_uart_conf_t conf;

    rt_uart_conf_init(&conf);

    // This will tell the driver to use the uart 0 interface (in case there are several)
    conf.itf = 0;
    conf.baudrate = baudrate;

    uart = rt_uart_open(NULL, &conf, NULL);
    if (uart == NULL) return -1;

    rt_uart_read(uart, rx_buffer, GAP_RING_BUFFER_SIZE, rt_event_get(&sched, irq_rx, 0));

    return 0 ;
}

void AT_COMMAND_Write(char *command,int length)
{
    rt_uart_write(uart, command, length, NULL);
}

static int AT_COMMAND_Poll()
{
    int read_available_bytes = GAP_RING_BUFFER_SIZE - rx_remain_size();

    /* If L2 has data,  allow reading */
    if (read_available_bytes != read_index)
        return 1;
    else
        return 0;
}

int AT_COMMAND_Read(char *r_buf, int length)
{
    int data_read= 0;
    while (data_read < length)
    {
        /* POLLING */
        if (AT_COMMAND_Poll())
        {
            r_buf[data_read++] = rx_buffer[read_index++];

            /* Re-cycle*/
            if(read_index == GAP_RING_BUFFER_SIZE ) {
                read_index = 0;
            }

            if(read_index == GAP_RING_BUFFER_SIZE - 10) {
                rt_event_execute(&sched, 1);
            }
        }
    }

    return data_read;
}
