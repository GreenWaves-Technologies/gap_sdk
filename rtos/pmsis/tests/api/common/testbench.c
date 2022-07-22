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

static struct pi_device bench;
static struct pi_device *testbench_ptr = NULL;


#define PI_TESTBENCH_CMD_GPIO_LOOPBACK 1

typedef struct {
    pi_device_t ctrl_dev;
} pi_testbench_t;


typedef struct {
    uint16_t cmd;
    uint16_t size;
} pi_testbench_req_header_t;


#define PI_TESTBENCH_CMD_GPIO_LOOPBACK  1
#define PI_TESTBENCH_CMD_UART_CHECKER   2
#define PI_TESTBENCH_CMD_SET_STATUS     3
#define PI_TESTBENCH_CMD_GPIO_PULSE_GEN 4
#define PI_TESTBENCH_CMD_GET_TIME_PS    5
#define PI_TESTBENCH_CMD_SPIM_VERIF_SETUP 6
#define PI_TESTBENCH_CMD_I2S_VERIF_SETUP 7
#define PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP 8
#define PI_TESTBENCH_CMD_I2S_VERIF_SLOT_START 9

#define PI_TESTBENCH_MAX_REQ_SIZE 256

/* TODO: It's better to align between freeRTOS and pulp */
#ifndef ARCHI_APB_SOC_CTRL_ADDR
#define ARCHI_APB_SOC_CTRL_ADDR (APB_SOC_CTRL_ADDR)
#endif


typedef enum {
    PI_TESTBENCH_REQ_UART_CHECKER_MODE_LOOPBACK,
    PI_TESTBENCH_REQ_UART_CHECKER_MODE_TRAFFIC_GEN
} pi_testbench_req_uart_checker_mode_e;


typedef struct {
    uint8_t input;
    uint8_t output;
    uint8_t enabled;
} pi_testbench_req_gpio_checker_t;


typedef struct {
    int64_t first_delay_ps;
    int64_t duration_ps;
    int64_t period_ps;
    uint8_t gpio;
    uint8_t enabled;
} pi_testbench_req_gpio_pulse_gen_t;


typedef struct {
    uint32_t status;
} pi_testbench_req_set_status_t;


typedef struct {
    uint32_t baudrate;
    uint8_t id;
    uint8_t mode;
    uint8_t flow_control;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t word_size;
    uint8_t usart;
    uint8_t usart_is_external;
    uint8_t enabled;
} pi_testbench_req_uart_checker_t;


typedef struct {
    uint8_t enabled;
    uint8_t itf;
    uint8_t cs;
    uint8_t is_master;
    uint8_t polarity;
    uint8_t phase;
    uint16_t mem_size_log2;
} pi_testbench_req_spim_verif_setup_t;


typedef struct {
    uint32_t sampling_rate;
    uint8_t itf;
    uint8_t enabled;
    uint8_t word_size;
    uint8_t nb_slots;
} pi_testbench_req_i2s_verif_setup_t;


typedef struct {
    uint8_t itf;
    uint8_t is_rx;
    uint8_t slot;
    uint8_t enabled;
    uint8_t word_size;
} pi_testbench_req_i2s_verif_slot_setup_t;


typedef struct {
    union {
        pi_testbench_req_gpio_checker_t gpio;
        pi_testbench_req_gpio_pulse_gen_t gpio_pulse_gen;
        pi_testbench_req_uart_checker_t uart;
        pi_testbench_req_set_status_t set_status;
        pi_testbench_req_spim_verif_setup_t spim_verif_setup;
        pi_testbench_req_i2s_verif_setup_t i2s_verif_setup;
        pi_testbench_req_i2s_verif_slot_setup_t i2s_verif_slot_setup;
    };
} pi_testbench_req_t;



static void pi_testbench_uart_setup_pads(int uart_id)
{
    if (uart_id == 0)
    {
    #if defined(__GAP9_5__)
        pi_pad_set_function(PI_PAD_064, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_065, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_066, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_067, PI_PAD_FUNC0);
    #else
        pi_pad_set_function(PI_PAD_060, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_061, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_062, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_063, PI_PAD_FUNC0);
    #endif
    }
    else if (uart_id == 1)
    {
    #if defined(__GAP9_5__)
        pi_pad_set_function(PI_PAD_069, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_070, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_071, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_072, PI_PAD_FUNC0);

        pi_pad_set_mux_group(PI_PAD_071, PI_PAD_MUX_GROUP_UART1_CTS);
        pi_pad_set_mux_group(PI_PAD_072, PI_PAD_MUX_GROUP_UART1_RTS);

        // In case we go to deep sleep, keep uart TX to 1 to not trigger some random communication
        pi_pad_set_sleep_cfg(PI_PAD_070, PI_PAD_SLEEP_CFG_OUTPUT_ENABLED | PI_PAD_SLEEP_CFG_OUTPUT_VALUE_1);
    #else
        pi_pad_set_function(PI_PAD_065, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_066, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_067, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_068, PI_PAD_FUNC0);

        pi_pad_set_mux_group(PI_PAD_067, PI_PAD_MUX_GROUP_UART1_CTS);
        pi_pad_set_mux_group(PI_PAD_068, PI_PAD_MUX_GROUP_UART1_RTS);
        // In case we go to deep sleep, keep uart TX to 1 to not trigger some random communication
        pi_pad_set_sleep_cfg(PI_PAD_066, PI_PAD_SLEEP_CFG_OUTPUT_ENABLED | PI_PAD_SLEEP_CFG_OUTPUT_VALUE_1);
    #endif
    
    }
    else if (uart_id == 2)
    {
        pi_pad_set_function(PI_PAD_027, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_028, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_029, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_030, PI_PAD_FUNC0);

        pi_pad_set_mux_group(PI_PAD_027, PI_PAD_MUX_GROUP_UART2_RX);
        pi_pad_set_mux_group(PI_PAD_028, PI_PAD_MUX_GROUP_UART2_TX);
        pi_pad_set_mux_group(PI_PAD_029, PI_PAD_MUX_GROUP_UART2_CTS);
        pi_pad_set_mux_group(PI_PAD_030, PI_PAD_MUX_GROUP_UART2_RTS);
    }
    else if (uart_id == 3)
    {
        pi_pad_set_function(PI_PAD_034, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_035, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_036, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_037, PI_PAD_FUNC0);

        pi_pad_set_mux_group(PI_PAD_034, PI_PAD_MUX_GROUP_UART3_RX);
        pi_pad_set_mux_group(PI_PAD_035, PI_PAD_MUX_GROUP_UART3_TX);
        pi_pad_set_function(PI_PAD_036, PI_PAD_036_UART3_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_037, PI_PAD_037_UART3_RTS_FUNC2);
    }
    else if (uart_id == 4)
    {
    #if defined(__GAP9_5__)
        pi_pad_set_function(PI_PAD_086, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_087, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_088, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_089, PI_PAD_FUNC0);

        pi_pad_set_function(PI_PAD_086, PI_PAD_086_UART4_RX_FUNC2);
        pi_pad_set_function(PI_PAD_087, PI_PAD_087_UART4_TX_FUNC2);
        pi_pad_set_function(PI_PAD_088, PI_PAD_088_UART4_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_089, PI_PAD_089_UART4_RTS_FUNC2);
    #else        
        pi_pad_set_function(PI_PAD_082, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_083, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_084, PI_PAD_FUNC0);
        pi_pad_set_function(PI_PAD_085, PI_PAD_FUNC0);

        pi_pad_set_function(PI_PAD_082, PI_PAD_082_UART4_RX_FUNC2);
        pi_pad_set_function(PI_PAD_083, PI_PAD_083_UART4_TX_FUNC2);
        pi_pad_set_function(PI_PAD_084, PI_PAD_084_UART4_CTS_FUNC2);
        pi_pad_set_function(PI_PAD_085, PI_PAD_085_UART4_RTS_FUNC2);
    #endif
    }
}


int pi_testbench_prepare_pads(struct pi_testbench_conf *conf)
{
    pi_open_from_conf(&bench, conf);

    if (pi_testbench_open(&bench))
        return 0;
}


int pi_testbench_open(struct pi_device *device)
{
    struct pi_testbench_conf *conf = (struct pi_testbench_conf *)device->config;

    pi_testbench_t *bench = pi_l2_malloc(sizeof(pi_testbench_t));
    if (bench == NULL)
        return -1;

    device->data = (void *)bench;

    struct pi_uart_conf uart_conf;

    pi_uart_conf_init(&uart_conf);

    pi_testbench_uart_setup_pads(conf->uart_id);

    uart_conf.uart_id = conf->uart_id;
    uart_conf.baudrate_bps = conf->uart_baudrate;
    uart_conf.enable_rx = 1;
    uart_conf.enable_tx = 1;

    pi_open_from_conf(&bench->ctrl_dev, &uart_conf);

    if (pi_uart_open(&bench->ctrl_dev))
    {
        pi_l2_free(bench, sizeof(pi_testbench_t));
        return -1;
    }

    return 0;
}


void pi_testbench_conf_init(struct pi_testbench_conf *conf)
{
    conf->ctrl_type = PI_TESTBENCH_CTRL_TYPE_UART;
    conf->uart_baudrate = 115200;
    conf->uart_id = 0;
}

void pi_testbench_gpio_loopback(pi_device_t *device, int output, int input, int enabled)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_GPIO_LOOPBACK, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_req_t req = { .gpio={.output=output, .input=input, .enabled=enabled } };
    pi_uart_write(&bench->ctrl_dev, &req, sizeof(req));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);
}


void pi_testbench_gpio_pulse_gen(pi_device_t *device, int gpio, int enabled, int64_t first_delay_ps, int64_t duration_ps, int64_t period_ps)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_GPIO_PULSE_GEN, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_req_t req = { .gpio_pulse_gen={.gpio=gpio, .enabled=enabled, .first_delay_ps=first_delay_ps, .duration_ps=duration_ps, .period_ps=period_ps } };
    pi_uart_write(&bench->ctrl_dev, &req, sizeof(req));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);
}


void pi_testbench_set_status(pi_device_t *device, int status)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_SET_STATUS, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_req_t req = { .set_status={.status=status } };
    pi_uart_write(&bench->ctrl_dev, &req, sizeof(req));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);
}


uint64_t pi_testbench_get_time_ps(pi_device_t *device)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_GET_TIME_PS, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    uint64_t time = 0;
    pi_uart_read(&bench->ctrl_dev, (uint8_t *)&time, 8);

    return time;
}


int pi_testbench_uart_traffic_gen(pi_device_t *device, pi_device_t *uart_device, struct pi_uart_conf *conf)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_open_from_conf(uart_device, conf);

    if (pi_uart_open(uart_device))
        return -1;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_UART_CHECKER, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_req_t req = { .uart={
        .baudrate=conf->baudrate_bps,
        .id=conf->uart_id,
        .mode=PI_TESTBENCH_REQ_UART_CHECKER_MODE_TRAFFIC_GEN,
        .flow_control=1,
        .stop_bits=conf->stop_bit_count,
        .parity=conf->parity_mode,
        .word_size=conf->word_size,
        .usart=conf->is_usart,
        .usart_is_external=0,
        .enabled=1
    }};

    pi_uart_write(&bench->ctrl_dev, &req, sizeof(req));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


void pi_testbench_uart_traffic_gen_rx(pi_device_t *device, int start, int incr, int size, int bandwidth, int nb_iter)
{
    char command[256];
    sprintf(command, "TRAFFIC_RX %d %d %d %d %d", start, incr, size, bandwidth, nb_iter);
    pi_uart_write(device, command, strlen(command) + 1);
}


void pi_testbench_uart_traffic_gen_tx(pi_device_t *device, int start, int incr, int size, int iter_size)
{
    char command[256];
    sprintf(command, "TRAFFIC_TX %d %d %d %d", start, incr, size, iter_size);
    pi_uart_write(device, command, strlen(command) + 1);
}


void pi_testbench_uart_traffic_gen_start(pi_device_t *device)
{
    char command[256];
    sprintf(command, "START");
    pi_uart_write(device, command, strlen(command) + 1);

    pi_uart_ioctl(device, PI_UART_IOCTL_FLUSH, NULL);

}


int pi_testbench_uart_traffic_gen_status(pi_device_t *device)
{
    char command[256];
    sprintf(command, "STATUS");
    pi_uart_write(device, command, strlen(command) + 1);

    pi_uart_ioctl(device, PI_UART_IOCTL_FLUSH, NULL);

    char status[3];
    pi_uart_read(device, status, 3);

    return strcmp(status, "OK");
}


int pi_testbench_spim_verif_setup(pi_device_t *device, int enabled, int itf, int cs, int polarity, int phase, int mem_size_log2)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_SPIM_VERIF_SETUP, .size=sizeof(pi_testbench_req_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_req_t req = { .spim_verif_setup={.enabled=enabled, .itf=itf, .cs=cs, .polarity=polarity, .phase=phase, .mem_size_log2=mem_size_log2 } };
    pi_uart_write(&bench->ctrl_dev, &req, sizeof(req));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


int pi_testbench_i2s_verif_open(pi_device_t *device, int itf, pi_testbench_i2s_verif_config_t *config)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_I2S_VERIF_SETUP, .size=sizeof(pi_testbench_i2s_verif_config_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    config->itf = itf;
    config->enabled = 1;
    pi_uart_write(&bench->ctrl_dev, config, sizeof(pi_testbench_i2s_verif_config_t));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


int pi_testbench_i2s_verif_close(pi_device_t *device, int itf)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_I2S_VERIF_SETUP, .size=sizeof(pi_testbench_i2s_verif_config_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_i2s_verif_config_t config;
    config.itf = itf;
    config.enabled = 1;
    pi_uart_write(&bench->ctrl_dev, &config, sizeof(pi_testbench_i2s_verif_config_t));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


int pi_testbench_i2s_verif_slot_open(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_config_t *config)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP, .size=sizeof(pi_testbench_i2s_verif_slot_config_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    config->itf = itf;
    config->slot = slot;
    config->enabled = 1;
    pi_uart_write(&bench->ctrl_dev, config, sizeof(pi_testbench_i2s_verif_slot_config_t));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


int pi_testbench_i2s_verif_slot_close(pi_device_t *device, int itf, int slot)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_I2S_VERIF_SLOT_SETUP, .size=sizeof(pi_testbench_i2s_verif_slot_config_t) };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    pi_testbench_i2s_verif_slot_config_t config;
    config.itf = itf;
    config.slot = slot;
    config.enabled = 0;
    pi_uart_write(&bench->ctrl_dev, &config, sizeof(pi_testbench_i2s_verif_slot_config_t));

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}


int pi_testbench_i2s_verif_slot_start(pi_device_t *device, int itf, int slot, pi_testbench_i2s_verif_slot_start_config_t *config)
{
    pi_testbench_t *bench = (pi_testbench_t *)device->data;

    int size = sizeof(pi_testbench_i2s_verif_slot_start_config_t);

    if (config->type == PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER)
    {
        config->tx_file_dumper.filepath_len = strlen((char *)config->tx_file_dumper.filepath) + 1;
        size += config->tx_file_dumper.filepath_len;
    }

    pi_testbench_req_header_t header = { .cmd = PI_TESTBENCH_CMD_I2S_VERIF_SLOT_START, .size=size };
    pi_uart_write(&bench->ctrl_dev, &header, sizeof(header));

    config->itf = itf;
    config->slot = slot;
    pi_uart_write(&bench->ctrl_dev, config, sizeof(pi_testbench_i2s_verif_slot_start_config_t));

    if (config->type == PI_TESTBENCH_I2S_VERIF_TX_FILE_DUMPER)
    {
        pi_uart_write(&bench->ctrl_dev, (void *)config->tx_file_dumper.filepath, config->tx_file_dumper.filepath_len);
    }

    pi_uart_ioctl(&bench->ctrl_dev, PI_UART_IOCTL_FLUSH, NULL);

    return 0;
}
