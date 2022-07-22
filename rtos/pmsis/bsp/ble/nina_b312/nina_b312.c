/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bsp/bsp.h"
#include "bsp/ble/nina_b312.h"
#include "nina_b312_defines.h"
#include <string.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

/* Factory default S3(CR) & S4(LF) response format character.  */
#define S3char  '\r'            /*!< CR char, ASCII 0x0D. */
#define S4char  '\n'            /*!< LF char, ASCII 0x0A. */
/* Same, but to use in string functions (strcat, strcmp, etc...). */
#define S3str  "\r"             /*!< CR string.  */
#define S4str  "\n"             /*!< LF string. */

#define PI_NINA_B312_UART_BAUDRATE   ( 115200 ) /*!< Baudrate used by NINA_B312 module(default value). */
#define PI_NINA_B312_DATA_BITS       ( 8 ) /*!< Data bits used by NINA_B312 module(default value). */

typedef struct
{
    struct pi_device uart_device; /*!< UART interface used to communicate with BLE module.  */
    char *byte;                   /*!< Byte used to receive response. */
    char *buffer;                 /*!< Buffer used to receive response. */
} nina_b312_t;

typedef enum
{
    PI_AT_RESP_NOT_STARTED,       /*!< Response not received yet from BLE module.  */
    PI_AT_RESP_IN_PROGRESS,       /*!< Response transmission has started. */
    PI_AT_RESP_DONE               /*!< Response transmission received, with end of resp. */
} at_resp_state_e;

typedef enum
{
    CMD_RES_OK    = 0,             /*!< OK reponse. */
    CMD_RES_ERR   = -1,            /*!< ERROR response. */
    CMD_RES_UNSOL = -2,            /*!< UNSOLICITED_RESPONSE response. */
    CMD_RES_NA    = -3             /*!< Non authorized response. */
} cmd_res_e;

/**
 * \struct cb_args
 */
typedef struct
{
    uint8_t has_error_code;
    nina_b312_t *nina;
    struct pi_task *task;
} cb_args_t;


/*******************************************************************************
 * Driver data
 ******************************************************************************/

static cb_args_t g_param = {0};

static volatile at_resp_state_e g_at_resp_state;

static pi_device_api_t g_nina_b312_api = {0};
static pi_ble_api_t g_nina_b312_ble_api = {0};


/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static int __pi_nina_b312_open(struct pi_device *device);

static int __pi_nina_b312_close(struct pi_device *device);

static int __pi_nina_hd_reset();

static ssize_t __pi_nina_b312_data_get(struct pi_device *device, uint32_t ext_addr,
                                       void *buffer, uint32_t size, pi_task_t *task);

static ssize_t __pi_nina_b312_data_send(struct pi_device *device, uint32_t ext_addr,
                                        const void *buffer, uint32_t size, pi_task_t *task);

static int __pi_nina_b312_ioctl(struct pi_device *device, uint32_t cmd, void *arg);

static int32_t __pi_nina_b312_at_cmd(struct pi_device *device, const char *cmd,
                                     char *resp, uint32_t size);

static int32_t __pi_nina_b312_peer_connect(struct pi_device *device, const char *addr);

static int32_t __pi_nina_b312_peer_disconnect(struct pi_device *device, const char *addr);

/*******************************************************************************
 * Board specific functions
 ******************************************************************************/

// These functions can be overloaded by the board-specific bsp part
// in order to overload default behavior

void __attribute__((weak)) bsp_nina_b312_conf_init(struct pi_nina_b312_conf *conf)
{
    conf->uart_itf = (uint8_t) CONFIG_NINA_B312_UART_ID;
}

int __attribute__((weak)) bsp_nina_b312_open()
{
    return 0;
}

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static void __pi_nina_b312_at_cmd_send(nina_b312_t *nina, const char *cmd)
{
    uint32_t cmd_length = strlen(cmd);
    uint32_t length = cmd_length + 2 + 1; /* cmd length + "AT" + '\r'. */
    char *cmd_string = (char *) pi_l2_malloc(sizeof(char) * length);
    if (cmd_string == NULL)
    {
        return;
    }
    strcpy((char*) cmd_string, (char*) "AT");
    if (cmd_length != 0)
    {
        strcat((char*) cmd_string, (char*) cmd);
    }
    strcat((char*) cmd_string, (char*) S3str);
    DEBUG_PRINTF("Sending: %s\n", cmd_string);
    int success = pi_uart_write(&(nina->uart_device), cmd_string, strlen(cmd_string));
    pi_l2_free(cmd_string, sizeof(char) * length);
}

static void __pi_nina_b312_startup_received(void *arg)
{
    /* Beware with printf.
       If no control flow you should not put some,
       uart is too fast for printf.
       With control flow, you can. */

    cb_args_t *param = (cb_args_t *) arg;
    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*(g_param.nina->byte) == S4char))
    {
        /* Check if +STARTUP
        (default factory message after wake up) */
        if ((0 == g_param.has_error_code) ||
            ((g_param.nina->buffer[index - 9] == '+') &&
             (g_param.nina->buffer[index - 8] == 'S') &&
             (g_param.nina->buffer[index - 7] == 'T') &&
             (g_param.nina->buffer[index - 6] == 'A') &&
             (g_param.nina->buffer[index - 5] == 'R') &&
             (g_param.nina->buffer[index - 4] == 'T') &&
             (g_param.nina->buffer[index - 3] == 'U') &&
             (g_param.nina->buffer[index - 2] == 'P')
            )
           )
        {
            g_param.nina->buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
        else
        {
            printf("Got unsollicited response : %s\n", g_param.nina->buffer);
        }
    }

    if (0 == end)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*(g_param.nina->byte) == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            g_param.nina->buffer[index++] = *(g_param.nina->byte);
        }
    }

    prev_byte = *(g_param.nina->byte);

    if (0 == end)
    {
        pi_task_callback(g_param.task, __pi_nina_b312_startup_received, &g_param);
        pi_uart_read_async(&(g_param.nina->uart_device),
                           (void *) g_param.nina->byte,
                           sizeof(uint8_t),
                           g_param.task);
    }
}

static void __pi_nina_b312_data_received(void *arg)
{
    /* Beware with printf.
       If no flow control you should not put some,
       uart is too fast for printf.
       With flow control, you can. */

    cb_args_t *param = (cb_args_t *) arg;
    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    // if (*(g_param.nina->byte) == '\r') printf("current = '\\r'\n");
    // else if (*(g_param.nina->byte) == '\n') printf("current = '\\n'\n");
    // else if (*(g_param.nina->byte) == '\0') printf("current = '\\0'\n");
    // else printf("current: %c\n", *(g_param.nina->byte));

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*(g_param.nina->byte) == S4char))
    {
        /* Check if ERROR or OK */
        if ((0 == g_param.has_error_code) ||
            (((g_param.nina->buffer[index - 3] == 'O') &&
             (g_param.nina->buffer[index - 2] == 'K')
            ) ||
            ((g_param.nina->buffer[index - 6] == 'E') &&
             (g_param.nina->buffer[index - 5] == 'R') &&
             (g_param.nina->buffer[index - 4] == 'R') &&
             (g_param.nina->buffer[index - 3] == 'O') &&
             (g_param.nina->buffer[index - 2] == 'R'))
            )
           )
        {
            g_param.nina->buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
        else
        {
            /* Got this case when the response has some text
            Schema is then as following:
            <S3><S4><text><S3><S4><S3><S4><code><S3><S4>
            We need therefore to parse again to get the code (OK|ERROR) */
            DEBUG_PRINTF("Got something else than OK or ERROR -> Continue\n");
            --index;
        }
    }

    if (0 == end)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*(g_param.nina->byte) == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            g_param.nina->buffer[index++] = *(g_param.nina->byte);
        }
    }
    prev_byte = *(g_param.nina->byte);

    if (0 == end)
    {
        pi_task_callback(g_param.task, __pi_nina_b312_data_received, &g_param);
        pi_uart_read_async(&(g_param.nina->uart_device),
                           (void *) g_param.nina->byte,
                           sizeof(uint8_t),
                           g_param.task);
    }
}

static int32_t __pi_nina_b312_read_startup(struct pi_device *device)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_t rx_cb = {0};
    g_param.has_error_code = 1;
    g_param.nina = nina;
    g_param.task = &rx_cb;
    pi_task_callback(&rx_cb, __pi_nina_b312_startup_received, &g_param);
    pi_uart_read_async(&(nina->uart_device), (void *) nina->byte, sizeof(uint8_t), &rx_cb);

    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    DEBUG_PRINTF("Got response: %s\n", nina->buffer);

    return CMD_RES_OK;
}

static int32_t __pi_nina_b312_wait_for_event(nina_b312_t *nina, char *resp)
{
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_t rx_cb = {0};
    g_param.has_error_code = 0;
    g_param.nina = nina;
    g_param.task = &rx_cb;
    pi_task_callback(&rx_cb, __pi_nina_b312_data_received, &g_param);
    pi_uart_read_async(&(nina->uart_device), (void *) nina->byte, sizeof(uint8_t), &rx_cb);

    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    DEBUG_PRINTF("Got unsolicited response: %s\n", nina->buffer);
    uint32_t resp_len = strlen((const char *) nina->buffer);
    strcpy((void *) resp, (void *) nina->buffer);
    return resp_len;
}

static int32_t __pi_nina_b312_data_mode_exit(nina_b312_t *nina)
{
#ifndef __PLATFORM_RTL__
    pi_time_wait_us(1000 * 1000);
#endif
    pi_uart_write(&(nina->uart_device), (void *) '+', 1);
    pi_uart_write(&(nina->uart_device), (void *) '+', 1);
    pi_uart_write(&(nina->uart_device), (void *) '+', 1);
#ifndef __PLATFORM_RTL__
    pi_time_wait_us(1000 * 1000);
#endif
    return 0;
}

static int32_t __pi_nina_b312_data_mode_enter(struct pi_device *device)
{
    char str[20];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_O_DATA_MODE));
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_device_unbond(struct pi_device *device, const char *addr)
{
    char str[60];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_DEVICE_UNBOND_SET));
    strcat(str, addr);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_device_role(struct pi_device *device, const char *role)
{
    char str[20];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_DEVICE_ROLE_SET));
    strcat(str, role);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_device_name(struct pi_device *device, const char *name)
{
    char str[128];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_DEVICE_NAME_SET));
    strcat(str, name);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_configuration_save(struct pi_device *device)
{
    char str[10];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_CFG_SAVE));
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_device_reboot(struct pi_device *device)
{
    char str[10];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_DEVICE_REBOOT));
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_uart_settings_set(struct pi_device *device,
                                                struct pi_nina_b312_conf *conf)
{
    char num[30], str[40];
    uint8_t ctrl_flow_ena = (conf->use_ctrl_flow != 1) ? 2 : 1; /* 2=disable : 1=enable */
    sprintf(num, "%ld,%d,%d,%d,%d,%d",
            conf->baudrate,                   /* Default baudrate is 115200. */
            ctrl_flow_ena,                    /* Control flow */
            (uint8_t) PI_NINA_B312_DATA_BITS, /* Programmed to 8 data bits. */
            conf->stop_bits,                  /* Stop bits. */
            conf->parity_bits,                /* Parity bits: 1=no parity, 2=Odd, 3=Even. */
            1                                 /* 0=Change after reboot,
                                                 1=Immediate change(after OK, wait 40 ms). */
        );
    strcpy(str, __XSTRING(PI_AT_NINA_B312_UART_SETTINGS_SET));
    strcat(str, num);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static void __pi_nina_b312_server_configure(struct pi_device *device)
{
    struct pi_nina_b312_conf *conf = (struct pi_nina_b312_conf *) device->config;
    nina_b312_t *nina = (nina_b312_t *) device->data;
    char str[10];

    /* Turn off echo. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_ECHO_OFF));
    __pi_nina_b312_at_cmd(device, str, NULL, 0);
    /* Factory reset. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_FACTORY_RESET));
    __pi_nina_b312_at_cmd(device, str, NULL, 0);
    /* Unbound devices. */
    __pi_nina_b312_device_unbond(device, "FFFFFFFFFFFF");
    /* Config device role. */
    __pi_nina_b312_device_role(device, __XSTRING(PI_AT_NINA_B312_SERVER_ROLE));
    /* Config device name. */
    __pi_nina_b312_device_name(device, conf->local_name);
    /* UART settings. */
    __pi_nina_b312_uart_settings_set(device, conf);
}

static void __pi_nina_b312_client_configure(struct pi_device *device)
{
    struct pi_nina_b312_conf *conf = (struct pi_nina_b312_conf *) device->config;
    nina_b312_t *nina = (nina_b312_t *) device->data;
    char str[10];

    /* Turn off echo. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_ECHO_OFF));
    __pi_nina_b312_at_cmd(device, str, NULL, 0);
    /* Factory reset. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_FACTORY_RESET));
    __pi_nina_b312_at_cmd(device, str, NULL, 0);
    /* Unbound devices. */
    __pi_nina_b312_device_unbond(device, "FFFFFFFFFFFF");
    /* Config device role. */
    __pi_nina_b312_device_role(device, __XSTRING(PI_AT_NINA_B312_CLIENT_ROLE));
    /* Config device name. */
    __pi_nina_b312_device_name(device, conf->local_name);
    // /* Bluetooth mode */
    // __pi_nina_b312_at_cmd(device, "+UBTMODE=2", NULL, 0); // 2=BLE
    /* UART settings. */
    __pi_nina_b312_uart_settings_set(device, conf);
}

static void __pi_nina_b312_device_id_get(struct pi_device *device, struct pi_nina_b312_id *info)
{
    struct pi_nina_b312_conf *conf = (struct pi_nina_b312_conf *) device->config;
    nina_b312_t *nina = (nina_b312_t *) device->data;
    char str[10];

    /* Manufacturer ID. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_MANUFACTURER_ID));
    __pi_nina_b312_at_cmd(device, str, info->manufacturer_id, sizeof(info->manufacturer_id));
    /* Model ID. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_MODEL_ID));
    __pi_nina_b312_at_cmd(device, str, info->model_id, sizeof(info->model_id));
    /* Software version ID. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_SW_VERSION_ID));
    __pi_nina_b312_at_cmd(device, str, info->sw_ver_id, sizeof(info->sw_ver_id));
    /* Serial number ID. */
    strcpy(str, __XSTRING(PI_AT_NINA_B312_SERIAL_NUMBER));
    __pi_nina_b312_at_cmd(device, str, info->serial_num, sizeof(info->serial_num));
}

static int enable_fc_pads(void)
{
    pi_device_t gpio_rts;
    pi_device_t gpio_cts;
    struct pi_gpio_conf gpio_rts_conf;
    struct pi_gpio_conf gpio_cts_conf;

    pi_gpio_conf_init(&gpio_rts_conf);
    pi_gpio_conf_init(&gpio_cts_conf);

    gpio_rts_conf.port = GPIO_UART1_RTS / 32;
    gpio_cts_conf.port = GPIO_UART1_CTS / 32;

    pi_open_from_conf(&gpio_rts, &gpio_rts_conf);
    pi_open_from_conf(&gpio_cts, &gpio_cts_conf);

    if (pi_gpio_open(&gpio_rts)) return -1;
    if (pi_gpio_open(&gpio_cts)) return -1;

    pi_pad_set_function(GPIO_UART1_RTS, PI_PAD_FUNC0);
    pi_pad_set_function(GPIO_UART1_CTS, PI_PAD_FUNC0);

    pi_pad_set_mux_group(GPIO_UART1_RTS, PI_PAD_MUX_GROUP_UART1_RTS);
    pi_pad_set_mux_group(GPIO_UART1_CTS, PI_PAD_MUX_GROUP_UART1_CTS);

    /* configure gpio output */
    pi_gpio_flags_e output_flags = PI_GPIO_OUTPUT;
    pi_gpio_flags_e input_flags = PI_GPIO_INPUT;
    pi_gpio_pin_configure(&gpio_rts, GPIO_UART1_RTS, output_flags);
    pi_gpio_pin_configure(&gpio_cts, GPIO_UART1_CTS, input_flags);

    return 0;
}

/*******************************************************************************
 * API implementation
 ******************************************************************************/

static int __pi_nina_b312_open(struct pi_device *device)
{
    pi_device_t gpio_nina_pwron;
    struct pi_gpio_conf gpio_conf_nina_pwron;

    pi_gpio_conf_init(&gpio_conf_nina_pwron);
    gpio_conf_nina_pwron.port = GPIO_NINA_PWRON / 32;
    pi_open_from_conf(&gpio_nina_pwron, &gpio_conf_nina_pwron);

    if (pi_gpio_open(&gpio_nina_pwron))
    {
        return -1;
    }

    DEBUG_PRINTF("Open bsp nina b312\n");
    bsp_nina_b312_open();

    /* Config device GPIO & pads. */
    pi_gpio_pin_configure(&gpio_nina_pwron, GPIO_NINA_PWRON, PI_GPIO_OUTPUT);
    pi_gpio_pin_write(&gpio_nina_pwron, GPIO_NINA_PWRON, 0);

    pi_time_wait_us(20000);

    struct pi_nina_b312_conf *conf = (struct pi_nina_b312_conf *) device->config;
    /* Nina_b312 device. */
    nina_b312_t *nina = (nina_b312_t *) pi_l2_malloc(sizeof(nina_b312_t));
    if (nina == NULL)
    {
        return -1;
    }
    nina->byte = (char *) pi_l2_malloc(sizeof(char) * 1);
    if (nina->byte == NULL)
    {
        pi_l2_free(nina, sizeof(nina_b312_t));
        return -2;
    }
    nina->buffer = (char *) pi_l2_malloc(sizeof(char) * (uint32_t) PI_AT_RESP_ARRAY_LENGTH);
    if (nina->byte == NULL)
    {
        pi_l2_free(nina->byte, sizeof(sizeof(char) * 1));
        pi_l2_free(nina, sizeof(nina_b312_t));
        return -3;
    }
    device->data = (void *) nina;

    /* Init and open UART device. */
    struct pi_uart_conf uart_conf = {0};
    pi_uart_conf_init(&uart_conf);
    uart_conf.uart_id       = conf->uart_itf;
    uart_conf.baudrate_bps  = PI_NINA_B312_UART_BAUDRATE;
    uart_conf.enable_rx     = 1;
    uart_conf.enable_tx     = 1;
    uart_conf.use_ctrl_flow = conf->use_ctrl_flow;
    pi_open_from_conf(&(nina->uart_device), &uart_conf);

    if (pi_uart_open(&(nina->uart_device)))
    {
        pi_l2_free(nina->buffer, sizeof(sizeof(char) * (uint32_t) PI_AT_RESP_ARRAY_LENGTH));
        pi_l2_free(nina->byte, sizeof(sizeof(char) * 1));
        pi_l2_free(nina, sizeof(nina_b312_t));
        return -4;
    }

    DEBUG_PRINTF("Enable nina b312\n");
    /* Enable Nina_B312. */
    pi_gpio_pin_write(&gpio_nina_pwron, GPIO_NINA_PWRON, 1);
    pi_time_wait_us(20000);

    /* Set pads for flow control */
    enable_fc_pads();

    /* Hardware reset of the module
    Ensure that +STARTUP will be send */
    __pi_nina_hd_reset();

    DEBUG_PRINTF("Read startup\n");
    __pi_nina_b312_read_startup(device);

    /* Do not need to wait for stability
    We wait for +STARTUP message */
// #ifndef __PLATFORM_RTL__
//     pi_time_wait_us(1 * 1000 * 1000);
// #endif

    /* Test if NINA_B312 is active. */
    cmd_res_e nina_active = __pi_nina_b312_at_cmd(device, "", NULL, 0);
    if (nina_active == CMD_RES_ERR)
    {
        return -5;
    }
    return 0;
}

static int __pi_nina_b312_close(struct pi_device *device)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    pi_uart_close(&(nina->uart_device));
    pi_l2_free(nina->buffer, sizeof(sizeof(char) * (uint32_t) PI_AT_RESP_ARRAY_LENGTH));
    pi_l2_free(nina->byte, sizeof(sizeof(char) * 1));
    pi_l2_free(nina, sizeof(nina_b312_t));
    return 0;
}

static int __pi_nina_hd_reset()
{
    pi_device_t gpio_rst;
    struct pi_gpio_conf gpio_rst_conf;

    pi_gpio_conf_init(&gpio_rst_conf);

    gpio_rst_conf.port = GPIO_NINA_RST / 32;

    pi_open_from_conf(&gpio_rst, &gpio_rst_conf);

    if (pi_gpio_open(&gpio_rst))
    {
        return -1;
    }

    /* set pad to gpio mode */
    pi_pad_set_function(GPIO_NINA_RST, PI_PAD_FUNC1);

    /* configure gpio output */
    pi_gpio_flags_e flags = PI_GPIO_OUTPUT;
    pi_gpio_pin_configure(&gpio_rst, GPIO_NINA_RST, flags);

    pi_gpio_pin_write(&gpio_rst, GPIO_NINA_RST, 0);
    pi_time_wait_us(20000);
    pi_gpio_pin_write(&gpio_rst, GPIO_NINA_RST, 1);
    pi_time_wait_us(20000);

    return 0;
}

static ssize_t __pi_nina_b312_data_get(struct pi_device *device, uint32_t ext_addr,
                                       void *buffer, uint32_t size, pi_task_t *task)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    pi_uart_read_async(&(nina->uart_device), buffer, size, task);
    return 0;
}

static ssize_t __pi_nina_b312_data_send(struct pi_device *device, uint32_t ext_addr,
                                        const void *buffer, uint32_t size, pi_task_t *task)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    pi_uart_write_async(&(nina->uart_device), (void *) buffer, size, task);
    return 0;
}

static int __pi_nina_b312_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    pi_ble_ioctl_cmd_e func_id = (pi_ble_ioctl_cmd_e) cmd;
    switch (func_id)
    {
    case PI_NINA_B312_MODEL_INFO :
    {
        struct pi_nina_b312_id *info = (struct pi_nina_b312_id *) arg;
        __pi_nina_b312_device_id_get(device, info);
    }
        break;

    case PI_NINA_B312_SERVER_CONFIGURE :
    {
        __pi_nina_b312_server_configure(device);
    }
        break;

    case PI_NINA_B312_CLIENT_CONFIGURE :
    {
        __pi_nina_b312_client_configure(device);
    }
        break;

    case PI_NINA_B312_UART_CONFIGURE :
    {
        struct pi_nina_b312_conf *conf = (struct pi_nina_b312_conf *) arg;
        cmd_res_e res = __pi_nina_b312_uart_settings_set(device, conf);
        if (CMD_RES_OK == res)
        {
            struct pi_uart_conf uart_conf = {0};
            pi_uart_conf_init(&uart_conf);
            uart_conf.uart_id = conf->uart_itf;
            uart_conf.baudrate_bps = PI_NINA_B312_UART_BAUDRATE;
            uart_conf.enable_rx = 1;
            uart_conf.enable_tx = 1;
            uart_conf.use_ctrl_flow = conf->use_ctrl_flow;
            /* modify baudrate, parity & stop */
            uart_conf.baudrate_bps = conf->baudrate;
            uart_conf.parity_mode = (conf->parity_bits != 1) ? 1 : 0;
            uart_conf.stop_bit_count = conf->stop_bits;
            pi_uart_ioctl(&(nina->uart_device), PI_UART_IOCTL_ABORT_RX, NULL);
            pi_uart_ioctl(&(nina->uart_device), PI_UART_IOCTL_ABORT_TX, NULL);
            pi_uart_ioctl(&(nina->uart_device), PI_UART_IOCTL_CONF_SETUP,
                    &uart_conf);
        }
#ifndef __PLATFORM_RTL__
        /* wait 40ms for module to update */
        pi_time_wait_us(40000);
#endif
    }
        break;

    case PI_NINA_B312_DATA_MODE_ENTER :
    {
        return __pi_nina_b312_data_mode_enter(device);
    }
        break;

    case PI_NINA_B312_DATA_MODE_EXIT :
    {
        __pi_nina_b312_data_mode_exit(nina);
    }
        break;

    case PI_NINA_B312_WAIT_FOR_EVENT :
    {
        char *buffer = (char *) arg;
        return __pi_nina_b312_wait_for_event(nina, buffer);
    }
        break;

    default :
        printf("Error: unknown ioctl command !\n");
        return -1;
    }
    return 0;
}

static int32_t __pi_nina_b312_at_cmd(struct pi_device *device, const char *cmd,
                                     char *resp, uint32_t size)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_t rx_cb = {0};
    g_param.has_error_code = 1;
    g_param.nina = nina;
    g_param.task = &rx_cb;
    pi_task_callback(&rx_cb, __pi_nina_b312_data_received, &g_param);
    pi_uart_read_async(&(nina->uart_device), (void *) nina->byte, sizeof(uint8_t), &rx_cb);
    __pi_nina_b312_at_cmd_send(nina, cmd);

    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    DEBUG_PRINTF("Got response: %s\n", nina->buffer);

    cmd_res_e cmd_res = CMD_RES_NA;
    uint32_t last_char_pos = strlen((const char *) nina->buffer);
    if ((nina->buffer[last_char_pos - 2] == 'O') &&
        (nina->buffer[last_char_pos - 1] == 'K'))
    {
        DEBUG_PRINTF("OK response received !\n");
        cmd_res = CMD_RES_OK;
    }
    else if((nina->buffer[last_char_pos - 5] == 'E') &&
            (nina->buffer[last_char_pos - 4] == 'R') &&
            (nina->buffer[last_char_pos - 3] == 'R') &&
            (nina->buffer[last_char_pos - 2] == 'O') &&
            (nina->buffer[last_char_pos - 1] == 'R'))
    {
        DEBUG_PRINTF("ERROR response received !\n");
        cmd_res = CMD_RES_ERR;
    }
    else
    {
        DEBUG_PRINTF("Unsolicited/unrecognised response received: \"%s\" (len: %ld) !\n", nina->buffer, last_char_pos);
        cmd_res = CMD_RES_UNSOL;
    }
    if (size)
    {
        strcpy((void *) resp, (void *) nina->buffer);
    }
    return cmd_res;
}

static int32_t __pi_nina_b312_peer_connect(struct pi_device *device, const char *addr)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    char str[60];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_PEER_CONNECT));
    strcat(str, addr);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

static int32_t __pi_nina_b312_peer_disconnect(struct pi_device *device, const char *addr)
{
    nina_b312_t *nina = (nina_b312_t *) device->data;
    char str[60];
    strcpy(str, __XSTRING(PI_AT_NINA_B312_PEER_DISCONNECT));
    strcat(str, addr);
    return __pi_nina_b312_at_cmd(device, str, NULL, 0);
}

void pi_ble_nina_b312_conf_init(struct pi_device *device, struct pi_nina_b312_conf *conf)
{
    bsp_nina_b312_conf_init(conf);
    /* Bind NINA_B312 API to BLE API. */
    g_nina_b312_api.open  = &__pi_nina_b312_open;
    g_nina_b312_api.close = &__pi_nina_b312_close;
    g_nina_b312_api.read  = &__pi_nina_b312_data_get;
    g_nina_b312_api.write = &__pi_nina_b312_data_send;
    g_nina_b312_api.ioctl = &__pi_nina_b312_ioctl;
    /* BLE specific API. */
    g_nina_b312_ble_api.at_cmd          = &__pi_nina_b312_at_cmd;
    g_nina_b312_ble_api.peer_connect    = &__pi_nina_b312_peer_connect;
    g_nina_b312_ble_api.peer_disconnect = &__pi_nina_b312_peer_disconnect;
    g_nina_b312_api.specific_api        = (void *) &g_nina_b312_ble_api;
    char *str = "GreenWaves-GAP9_EVK";
    strcpy(conf->local_name, str);
    conf->baudrate = (uint32_t) PI_NINA_B312_UART_BAUDRATE; /* Default baudrate. */
    conf->parity_bits = 1;           /* No parity. */
    conf->stop_bits = 1;             /* 1 stop bit. */
    conf->use_ctrl_flow = 0;         /* No control flow */
    device->api = &g_nina_b312_api;
}
