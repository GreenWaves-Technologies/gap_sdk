/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include "pmsis.h"
#include "testbench.h"
#include <string.h>
#include "testlib.h"

struct pi_uart_conf conf;
static pi_device_t bench;

#define NB_UART 5

#define START_VALUE 0x73
#define INCR_VALUE  0x7

#define TOTAL_SIZE (NB_ITER * BUFFER_SIZE)

typedef struct
{
    int id;
    uint8_t *tx_buffer;
    uint8_t *rx_buffer[2];
    int rx_total_size;
    int tx_total_size;
    int rx_buffer_index;
    uint8_t rx_current_val;
    int rx_errors;
    pi_task_t rx_task;
    pi_task_t tx_task;
    struct pi_device uart;
} uart_gen_t;

static uart_gen_t uart_gens[ARCHI_UDMA_NB_UART];
static int nb_uart_gen_wait;

static void end_of_tx(void *arg);

static void end_of_rx(void *arg)
{
    uart_gen_t *uart_gen = arg;

    uart_gen->rx_total_size += BUFFER_SIZE;

    if (uart_gen->rx_total_size < TOTAL_SIZE)
    {
        pi_uart_read_async(&uart_gen->uart, uart_gen->rx_buffer[uart_gen->rx_buffer_index], BUFFER_SIZE, pi_task_callback(&uart_gen->rx_task, end_of_rx, uart_gen));
    }

    uart_gen->tx_total_size += BUFFER_SIZE;

    if (uart_gen->tx_total_size < TOTAL_SIZE)
        pi_uart_write_async(&uart_gen->uart, uart_gen->tx_buffer, BUFFER_SIZE, pi_task_callback(&uart_gen->tx_task, end_of_tx, uart_gen));

    uart_gen->rx_buffer_index ^=1;

    for (int i=0; i<BUFFER_SIZE; i++)
    {
        if (uart_gen->rx_buffer[uart_gen->rx_buffer_index][i] != uart_gen->rx_current_val)
        {
            uart_gen->rx_errors++;
        }

        uart_gen->rx_current_val += INCR_VALUE;
    }

    if ((uart_gen->rx_total_size == BUFFER_SIZE * NB_ITER) && (uart_gen->tx_total_size == BUFFER_SIZE * NB_ITER))
        nb_uart_gen_wait--;
}

static void end_of_tx(void *arg)
{
}


static void setup_pads(int uart_id)
{
    if (uart_id == 1)
    {
#ifdef __GAP9_5__
        pi_pad_set_mux_group(PI_PAD_071, PI_PAD_MUX_GROUP_UART1_CTS);
        pi_pad_set_mux_group(PI_PAD_072, PI_PAD_MUX_GROUP_UART1_RTS);
#else
        pi_pad_set_mux_group(PI_PAD_067, PI_PAD_MUX_GROUP_UART1_CTS);
        pi_pad_set_mux_group(PI_PAD_068, PI_PAD_MUX_GROUP_UART1_RTS);
#endif
    }
    if (uart_id == 2)
    {
        pi_pad_set_mux_group(PI_PAD_027, PI_PAD_MUX_GROUP_UART2_RX);
        pi_pad_set_mux_group(PI_PAD_028, PI_PAD_MUX_GROUP_UART2_TX);
        pi_pad_set_mux_group(PI_PAD_029, PI_PAD_MUX_GROUP_UART2_CTS);
        pi_pad_set_mux_group(PI_PAD_030, PI_PAD_MUX_GROUP_UART2_RTS);
        pi_pad_set_function(PI_PAD_048, PI_PAD_FUNC2);
    }
    if (uart_id == 3)
    {
        pi_pad_set_mux_group(PI_PAD_034, PI_PAD_MUX_GROUP_UART3_RX);
        pi_pad_set_mux_group(PI_PAD_035, PI_PAD_MUX_GROUP_UART3_TX);
        pi_pad_set_function(PI_PAD_036, PI_PAD_036_UART3_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_037, PI_PAD_037_UART3_RTS_FUNC2);
        pi_pad_set_function(PI_PAD_033, PI_PAD_FUNC2);
    }
    if (uart_id == 4)
    {
#ifdef __GAP9_5__
        pi_pad_set_function(PI_PAD_085, PI_PAD_FUNC2);
        pi_pad_set_function(PI_PAD_086, PI_PAD_086_UART4_RX_FUNC2);
        pi_pad_set_function(PI_PAD_087, PI_PAD_087_UART4_TX_FUNC2);
        pi_pad_set_function(PI_PAD_088, PI_PAD_088_UART4_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_089, PI_PAD_089_UART4_RTS_FUNC2);
#else
        pi_pad_set_function(PI_PAD_081, PI_PAD_FUNC2);
        pi_pad_set_function(PI_PAD_082, PI_PAD_082_UART4_RX_FUNC2);
        pi_pad_set_function(PI_PAD_083, PI_PAD_083_UART4_TX_FUNC2);
        pi_pad_set_function(PI_PAD_084, PI_PAD_084_UART4_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_085, PI_PAD_085_UART4_RTS_FUNC2);
#endif
    }
}



static void uart_gen_prepare(int uart_id)
{
    uart_gen_t *uart_gen = &uart_gens[uart_id];

    int start = START_VALUE;
    int incr = INCR_VALUE;
    int baudrate = BAUDRATE;
#ifndef NO_BANDWIDTH_LIMITATION
    int bandwidth = baudrate / 8 / 4;
#else
    int bandwidth = baudrate / 8;
#endif
    int nb_iter = NB_ITER;
    int size = nb_iter * BUFFER_SIZE;

    uart_gen->id = uart_id;

    pi_uart_conf_init(&conf);

    conf.enable_tx = 1;
    conf.enable_rx = 1;
    conf.uart_id = uart_id;
    conf.use_ctrl_flow = 1;
    conf.baudrate_bps = baudrate;
    conf.is_usart = 1;
    conf.usart_polarity = POLARITY;
    conf.usart_phase = PHASE;

    setup_pads(uart_id);

    pi_testbench_uart_traffic_gen(&bench, &uart_gen->uart, &conf);

    nb_uart_gen_wait++;

    char command[256];

    uart_gen->tx_buffer = pi_l2_malloc(BUFFER_SIZE);
    uart_gen->rx_buffer[0] = pi_l2_malloc(BUFFER_SIZE);
    uart_gen->rx_buffer[1] = pi_l2_malloc(BUFFER_SIZE);

    uint32_t value = start;
    for (int i=0; i<BUFFER_SIZE; i++)
    {
        uart_gen->tx_buffer[i] = value;
        value += incr;
    }

    // Command to activate RX traffic
    pi_testbench_uart_traffic_gen_rx_usart(&uart_gen->uart, start, incr, size, bandwidth, nb_iter);

    // Command to activate TX traffic
    pi_testbench_uart_traffic_gen_tx_usart(&uart_gen->uart, start, incr, size, BUFFER_SIZE);

    // Start
    pi_testbench_uart_traffic_gen_start_usart(&uart_gen->uart);

    uart_gen->rx_total_size = 0;
    uart_gen->tx_total_size = 0;
    uart_gen->rx_errors = 0;
    uart_gen->rx_current_val = START_VALUE;
    uart_gen->rx_buffer_index = 1;
}


static void uart_gen_start(int uart_id)
{
    uart_gen_t *uart_gen = &uart_gens[uart_id];

    pi_uart_read_async(&uart_gen->uart, uart_gen->rx_buffer[0], BUFFER_SIZE, pi_task_callback(&uart_gen->rx_task, end_of_rx, uart_gen));
    pi_uart_write_async(&uart_gen->uart, uart_gen->tx_buffer, BUFFER_SIZE, pi_task_callback(&uart_gen->tx_task, end_of_tx, uart_gen));

}

static int uart_gen_stop(int uart_id)
{
    uart_gen_t *uart_gen = &uart_gens[uart_id];

    pi_uart_ioctl(&uart_gen->uart, PI_UART_IOCTL_ABORT_RX, NULL);
    pi_uart_ioctl(&uart_gen->uart, PI_UART_IOCTL_ENABLE_RX, NULL);

    if (pi_testbench_uart_traffic_gen_status_usart(&uart_gen->uart))
        uart_gen->rx_errors++;

    pi_l2_free(uart_gen->tx_buffer, BUFFER_SIZE);
    pi_l2_free(uart_gen->rx_buffer[0], BUFFER_SIZE);
    pi_l2_free(uart_gen->rx_buffer[1], BUFFER_SIZE);

    return uart_gen->rx_errors;
}


static int test_uart()
{
    testlib_hyperram_trafficgen_t traffic_gen0, traffic_gen1;

    int total_errors = 0;

    nb_uart_gen_wait = 0;

#ifdef USE_TRAFFIC_GEN
    testlib_hyperram_trafficgen_config_t config0;
    testlib_hyperram_trafficgen_conf_init(&config0);
    testlib_hyperram_trafficgen_init(&traffic_gen0, &config0);
    testlib_hyperram_trafficgen_start(&traffic_gen0);

    testlib_hyperram_trafficgen_config_t config1;
    testlib_hyperram_trafficgen_conf_init(&config1);
    config1.itf = 1;
    config1.cs = 0;
    testlib_hyperram_trafficgen_init(&traffic_gen1, &config1);
    testlib_hyperram_trafficgen_start(&traffic_gen1);
#endif

    for (int i=0; i<NB_UART; i++)
    {
        if (i == CONFIG_TESTBENCH_UART_ID)
            continue;

        uart_gen_prepare(i);
    }

    for (int i=0; i<NB_UART; i++)
    {
        if (i == CONFIG_TESTBENCH_UART_ID)
            continue;

        uart_gen_start(i);
    }


    while(nb_uart_gen_wait)
    {
        pi_yield();
    }

    for (int i=0; i<NB_UART; i++)
    {
        if (i == CONFIG_TESTBENCH_UART_ID)
            continue;

        int errors = uart_gen_stop(i);
        if (errors)
        {
            printf("%d errors detected on uart %d\n", errors, i);
            total_errors += errors;
        }
    }

#ifdef USE_TRAFFIC_GEN
    total_errors += testlib_hyperram_trafficgen_stop(&traffic_gen0);
    testlib_hyperram_trafficgen_deinit(&traffic_gen0);
    total_errors += testlib_hyperram_trafficgen_stop(&traffic_gen1);
    testlib_hyperram_trafficgen_deinit(&traffic_gen1);
#endif

    return total_errors;
}



static int test_entry()
{
    printf("Entering test\n");

    int errors = 0;
    int index = 0;

    struct pi_testbench_conf conf;

    pi_freq_set(PI_FREQ_DOMAIN_FC, SOC_FREQ);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 200000000);

    setup_pads(CONFIG_TESTBENCH_UART_ID);

    pi_testbench_conf_init(&conf);

    conf.uart_id = CONFIG_TESTBENCH_UART_ID;

    pi_open_from_conf(&bench, &conf);

    if (pi_testbench_open(&bench))
        return -1;

    errors += test_uart();

    if (errors)
        printf("Test failure\n");
    else
        printf("Test success\n");

    pi_testbench_set_status(&bench, errors);

    return errors;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
