/* 
 * Copyright (C) 2021 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "stdio.h"


#define RX_BUFFER_SIZE 16
#define TX_BUFFER_SIZE 16
#define RX_DELAY 200
#define TX_DELAY 170


static pi_task_t rx_task;
static pi_task_t tx_task;
static unsigned char rx_buffer[RX_BUFFER_SIZE];
static unsigned char tx_buffer[TX_BUFFER_SIZE];
static pi_device_t uart;
static pi_task_t end_task;
static unsigned char expected_rx = 0;
static unsigned char current_tx = 0;
static int start_tx = 1;


static void handle_rx(void *arg);
static void handle_tx(void *arg);


static void enqueue_tx(void *arg)
{
    for (int i=0; i<TX_BUFFER_SIZE; i++)
    {
        tx_buffer[i] = current_tx++;
    }

    pi_uart_write_async(&uart, tx_buffer, TX_BUFFER_SIZE, pi_task_callback(&tx_task, handle_tx, NULL));
}


static void handle_tx(void *arg)
{
    pi_task_push_delayed_us(pi_task_callback(&tx_task, enqueue_tx, NULL), TX_DELAY);
}



static void enqueue_rx(void *arg)
{
    pi_uart_read_async(&uart, rx_buffer, RX_BUFFER_SIZE, pi_task_callback(&rx_task, handle_rx, NULL));
}


static void handle_rx(void *arg)
{
    if (start_tx)
    {
        start_tx = 0;
        enqueue_tx(NULL);
    }
    for (int i=0; i<RX_BUFFER_SIZE; i++)
    {
        if (rx_buffer[i] != expected_rx)
        {
            printf("Error, expected %x, got %x\n", expected_rx, rx_buffer[i]);
            exit(1);
        }
        expected_rx += 1;
    }

    pi_task_push_delayed_us(pi_task_callback(&rx_task, enqueue_rx, NULL), RX_DELAY);
}


int test_entry()
{
    struct pi_uart_conf conf;

    pi_uart_conf_init(&conf);

    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.uart_id = 0;
    conf.use_ctrl_flow = 1;
    conf.baudrate_bps = 10000000;

    pi_open_from_conf(&uart, &conf);

    if (pi_uart_open(&uart))
      return -1;

    enqueue_rx(NULL);

    pi_task_wait_on(pi_task_block(&end_task));

    return 0;
}

void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
