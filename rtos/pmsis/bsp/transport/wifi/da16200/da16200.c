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


#include "bsp/wifi.h"
#include "bsp/bsp.h"
#include "bsp/transport/at_commands.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef DEBUG_DA16200
#define DA16200_DEBUG printf
#else
#define DA16200_DEBUG(...) ((void) 0)
#endif


#define PI_DA16200_UART_BAUDRATE  ( 115200 ) /*!< Baudrate used by DA16200 module (default value). */
#define PI_DA16200_UART_DATA_BITS ( 8 )      /*!< Data bits used by DA16200 module (default value). */


/**
 * \struct da16200_t
 * 
 * @brief Structure for BSP WiFi DA16200 module
 */
typedef struct
{
	struct pi_device uart_device;
	char *uart_byte;   // Byte used to receive response from uart
	char *uart_buffer; // Buffer used to receive respoonse from uart

    char *header;         // Packet header
	char *client_ip_port; // IP + Port of the client: <ip>,<port>
} da16200_t;

#define MAX_HEADER_SIZE           32
#define MAX_CLIENT_IP_PORT_SIZE   21

/**
 * @brief Error response codes.
 * Help managing errors.
 * 
 * @note Codes are negatives. However, we manage them with their positive
 * values to be faster by not doing any multiplication by -1. In addition,
 * we parse, and get them in their positive values.
 * 
 * @return uint8_t Code corresponding to the error
 * \retval 1 to 12, plus 99
 */
typedef enum
{
    UNKNOWN_CMD              = 1,
    INSUFFICIENT_PARAM       = 2,
    TOO_MANY_PARAM           = 3,
    WRONG_PARAM_VALUE        = 4,
    UNSUPPORTED_FUNCT        = 5,
    NOT_CONNECTED_TO_AN_AP   = 6,
    NO_RESULT                = 7,
    RESP_BUFFER_OVERFLOW     = 8,
    FUNCT_NOT_CONFIGURED     = 9,
    CMD_TIMEOUT              = 10,
    NVRAM_WRITE_FAIL         = 11,
    RETENTION_MEM_WRITE_FAIL = 12,
    UNKNOWN_ERR              = 99
} da16200_err_code_e;

/**
 * @brief Event that will be wait
 * 
 * @return uint8_t Code corresponding to the event
 */
typedef enum
{
    STARTUP                  = 0,  // Event when module is ready
    CONNECTED_TO_AP          = 1,  // Event when connection to AP is established
    CLIENT_CONNECTED         = 2,  // Event when a client connects to TCP server socket
    MSG_RECEIVED             = 10, // Event when receiving data through TCP server socket
    CLIENT_DISCONNECTED      = 20, // Event when a client disconnects from TCP server socket
} da16200_event_e;

/**
 * \struct cb_args_t
 */
typedef struct
{
    uint8_t has_error_code;
    da16200_t *da16200;
    struct pi_task *task;          // Internal callback, for recursiveness mainly
    struct pi_task *user_callback; // Callback of the user
} cb_args_t;

/**
 * \struct tx_arg_t
 * 
 * @brief Structure to store needed informations for uart_send.
 * Needed for async send method.
 */
typedef struct
{
    uint32_t size; // Size of the buffer
    char *buffer;
} tx_args_t;

/**
 * \struct header_t
 * 
 * @brief Structure to describe a packet's header received from the client.
 */
typedef struct
{
    char *header; // Header of the message received from client
    int8_t code; // Code for the header
} header_t;


/*******************************************************************************
 * Driver data
 ******************************************************************************/

pi_task_t rx_cb, tx_cb;
static cb_args_t g_param, g_param_tx;
static header_t g_header;
static tx_args_t g_tx_args;

static volatile at_resp_state_e g_at_resp_state = PI_AT_RESP_NOT_STARTED;


/*******************************************************************************
 * Function declaration
 ******************************************************************************/

static int8_t __pi_da16200_open(struct pi_device *device);
static void __pi_da16200_close(struct pi_device *device);

static int8_t __pi_da16200_connect_to_ap(struct pi_device *device, const char *ssid, const char *pwd, pi_task_t *callback);
static int8_t __pi_da16200_disconnect_from_ap(struct pi_device *device);

static int8_t __pi_da16200_create_tcp_server(struct pi_device *device, uint16_t port);
static int8_t __pi_da16200_wait_client_connection(struct pi_device *device, char *resp, pi_task_t *callback);
static int8_t __pi_da16200_wait_client_event(struct pi_device *device, char *resp, pi_task_t *callback);

static ssize_t __pi_da16200_data_get_async(struct pi_device *device, uint32_t ext_addr,
                                void *buffer, uint32_t size, pi_task_t *task);
static ssize_t __pi_da16200_data_send_async(struct pi_device *device, uint32_t ext_addr,
                                const char *buffer, uint32_t size, pi_task_t *task);

static int __pi_da16200_ioctl(struct pi_device *device, uint32_t cmd, void *arg);
static int8_t __pi_da16200_at_cmd(struct pi_device *device, char *cmd, char *resp);


/*******************************************************************************
 * Board specific functions
 ******************************************************************************/

// These functions can be overloaded by the board-specific bsp part
// in order to overload default behavior

// void __attribute__((weak)) bsp_da16200_conf_init(struct pi_da16200_conf *conf)
// {
// 	conf->uart_itf = (uint8_t) CONFIG_DA16200_UART_ITF;
// }

// int __attribute__((weak)) bsp_da16200_open()
// {
// 	return 0;
// }


/*******************************************************************************
 * Internal functions
 ******************************************************************************/

static char * l_trim(char *string)
{
    char * original;
    char * p;
    int16_t trimmed;

    trimmed = 0;
    original = string;
    p = original;

    do
    {
        if  (*original != ' ' || trimmed)
        {
            trimmed = 1;
            *p++ = *original;
        }
    } while (*original++ != 0);

    return string;
}

/**
 * @brief Convert uint16_t to string
 * 
 * @param[in] input uint16_t number to convert
 * @param[out] output char* to store result
 */
static void uint16_to_str(uint16_t input, char *output)
{
    uint8_t len;

    //--- the result is right justified - so we go from bottom to top
    for (len = 0; len < 5; len++)
    {
        output[len] = ' ';
    }

    output[len--] = 0;

    while (1)
    {
        output[len] = input % 10u + 48;
        input = input / 10u;

        if (input == 0)
        {
            break;
        }

        len--;
    }
}

/**
 * @brief Compare two buffers
 * 
 * @param[in] buff1 char* Buffer 1
 * @param[in] buff2 char* Buffer 2
 * @param[in] idx1 uint8_t Start buffer 1 at this index
 * @param[in] idx2 uint8_t Start buffer 2 at this index
 * @param[in] size uint8_t Nb of index to compare on each buffer
 * 
 * @return uint8_t Code : buffers are aquals or not
 * \retval 0 Buffers are equals
 * \retval 1 Buffers are differents
 */
static uint8_t compare(char *buff1, char *buff2, int idx1, int idx2, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        if (buff1[idx1 + i] != buff2[idx2 + i])
        {
            return 1;
        }
    }

    return 0;
}


/**
 * @brief Drop incoming bytes until "\r\n" sequence.
 * Useful to drop useless response when we do not know its size.
 * 
 * @note Asynchrone. Release the callback of the user when "\r\n" is found.
 */
static void __pi_da16200_drop_byte()
{
    static char prev_byte = 0;

    if ((prev_byte != S3char) || (*(g_param.da16200->uart_byte) != S4char))
    {
        prev_byte = *(g_param.da16200->uart_byte);
        pi_task_callback(g_param.task, __pi_da16200_drop_byte, NULL);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           g_param.task);
    }
    else
    {
        prev_byte = 0;
        pi_task_push(g_param.user_callback);
    }
}

/**
 * @brief Save incoming bytes until "\r\n" sequence.
 * Useful when user wants to parse response, and we do not know its size.
 * 
 * 
 * @note Asynchrone. Release the callback of the user when "\r\n" is found.
 */
static void __pi_da16200_save_byte(void *resp)
{
    static uint32_t index = 1; // Index 0 is for the header code
    static char prev_byte = 0;

    if ((prev_byte != S3char) || (*(g_param.da16200->uart_byte) != S4char))
    {
        prev_byte = *(g_param.da16200->uart_byte);
        *((char *) resp + index) = prev_byte;
        index++;
        pi_task_callback(g_param.task, __pi_da16200_save_byte, resp);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           g_param.task);
    }
    else
    {
        *((char *) resp) = (char *) g_header.code;
        *((char *) resp + index - 1) = '\0';
        index = 1;
        prev_byte = 0;
        pi_task_push(g_param.user_callback);
    }
}

/**
 * @brief Get the startup message
 * 
 * @param arg Buffer to save the startup message (if NULL, no saving)
 * 
 * @note Synchrone. 
 */
static void __pi_da16200_startup_received(void *arg)
{
    static uint32_t index = 0;
    static char prev_byte = 0;

    uint8_t end = 0;

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*(g_param.da16200->uart_byte) == S4char))
    {
        if ((g_param.has_error_code == 0) ||
            ((compare(g_param.da16200->uart_buffer, "+INIT:DONE,", index-13, 0, 11) == 0) &&
             ((g_param.da16200->uart_buffer[index - 2] == '0') ||
              (g_param.da16200->uart_buffer[index - 2] == '1')
             )
            )
           )
        {
            g_param.da16200->uart_buffer[--index] = '\0';
            if (arg != NULL)
            {
                sprintf((char *) arg, g_param.da16200->uart_buffer);
            }
            end = 1;
            pi_task_push(g_param.user_callback);
        }
        else
        {
            printf("Got unsollicited response : %s\n", g_param.da16200->uart_buffer);
        }
    }

    if (end == 0)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*(g_param.da16200->uart_byte) == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            g_param.da16200->uart_buffer[index++] = *(g_param.da16200->uart_byte);
        }
    }
    prev_byte = *(g_param.da16200->uart_byte);

    if (end == 0)
    {
        pi_task_callback(g_param.task, __pi_da16200_startup_received, arg);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           g_param.task);
    }
}

/**
 * @brief Read the size of the data in the message sent by the client,
 * and then read the message.
 * 
 * @param arg Buffer to save the data (if NULL, uses uart_buffer of the module instance)
 * 
 * @note Asynchrone. Release the callback of the user when all the message is read.
 */
static void __pi_da16200_get_size(void *arg)
{
    static uint32_t size = 0;

    if (*(g_param.da16200->uart_byte) != ',')
    {
        size = size * 10 + ((int8_t) *(g_param.da16200->uart_byte)) - 48;
        pi_task_callback(g_param.task, __pi_da16200_get_size, arg);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                            (void *) g_param.da16200->uart_byte,
                            sizeof(char),
                            g_param.task);
    }
    else
    {
        if (arg != NULL)
        {
            *((char *) arg) = (char *) g_header.code;
            pi_uart_read_async(&(g_param.da16200->uart_device),
                                arg + 1,
                                size + 2,
                                g_param.user_callback);
        }
        else
        {
            g_param.da16200->uart_buffer[0] = (char *) g_header.code;
            pi_uart_read_async(&(g_param.da16200->uart_device),
                                g_param.da16200->uart_buffer + 1,
                                size + 2,
                                g_param.user_callback);
        }
        size = 0;
    }
}

/**
 * @brief Parse header of the packet until being at the size of the data
 * 
 * @param arg Buffer to save the data (if NULL, uses uart_buffer of the module instance)
 * 
 * @note Asynchrone.
 */
static void __pi_da16200_go_to_size(void *arg)
{
    static uint8_t delim_counter = 0;

    if (*(g_param.da16200->uart_byte) != ',')
    {
        pi_task_callback(g_param.task, __pi_da16200_go_to_size, arg);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                            (void *) g_param.da16200->uart_byte,
                            sizeof(char),
                            g_param.task);
    }
    else
    {
        delim_counter++;
        if (delim_counter == 3)
        {
            pi_task_callback(g_param.task, __pi_da16200_get_size, arg);
            pi_uart_read_async(&(g_param.da16200->uart_device),
                                (void *) g_param.da16200->uart_byte,
                                sizeof(char),
                                g_param.task);
            delim_counter = 0;
        }
        else
        {
            pi_task_callback(g_param.task, __pi_da16200_go_to_size, arg);
            pi_uart_read_async(&(g_param.da16200->uart_device),
                                (void *) g_param.da16200->uart_byte,
                                sizeof(char),
                                g_param.task);
        }
    }
}

/**
 * @brief Parse event from a client.
 * 
 * @param arg Buffer to save response (if NULL, no saving,
 * except on message received: uses uart_buffer of the module instance if arg==NULL)
 * 
 * @note Asynchrone.
 */
static void __pi_da16200_data_received(void *arg)
{
    // Check if this we get an known event
    // Starting to compare header at index 2 due to the first 2 bytes '\r' & '\n'

    /* Special event when receiving a message.
    Parsing size, and read message in one time
    */
    if (compare(g_param.da16200->uart_buffer, PI_AT_PREFIX_MSG_RECEIVED, 2, 0, 6) == 0)
    {
        g_header.code = 49;
        pi_task_callback(g_param.task, __pi_da16200_go_to_size, arg);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                            (void *) g_param.da16200->uart_byte,
                            sizeof(char),
                            g_param.task);
        DA16200_DEBUG("Received +TRDTS\n");
    }
    // Special event when client disconnects. Unpredictable.
    else if (compare(g_param.da16200->uart_buffer, PI_AT_PREFIX_CLIENT_DISCONNECTED, 2, 0, 6) == 0)
    {
        if (arg != NULL)
        {
            g_header.code = 50;
            pi_task_callback(g_param.task, __pi_da16200_save_byte, arg);
        }
        else
        {
            pi_task_callback(g_param.task, __pi_da16200_drop_byte, NULL);
        }
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           g_param.task);
        DA16200_DEBUG("Received +TRXTS\n");
    }
    // Classic packets
    else if (compare(g_param.da16200->uart_buffer, g_header.header, 2, 0, 6) == 0)
    {
        if (arg != NULL)
        {
            pi_task_callback(g_param.task, __pi_da16200_save_byte, arg);
        }
        else
        {
            pi_task_callback(g_param.task, __pi_da16200_drop_byte, NULL);
        }
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           g_param.task);
        DA16200_DEBUG("Received %s\n", g_header.header);
    }
    else
    {
        printf("Unsollicited event: %c.\n", g_param.da16200->uart_buffer);
    }
}

/**
 * @brief Callback to get the OK | ERROR response of the message send previously
 * 
 * @param arg 
 */
static void __pi_da16200_resp_received(void *arg)
{
    /* Beware with printf.
       If no flow control you should not put any,
       uart is too fast for printf.
       With flow control, you can. */

    static uint32_t index = 0;
    static char prev_byte = 0;

    uint8_t end = 0;

    // if (*(g_param.da16200->uart_byte) == '\r') printf("current = '\\r'\n");
    // else if (*(g_param.da16200->uart_byte) == '\n') printf("current = '\\n'\n");
    // else if (*(g_param.da16200->uart_byte) == '\0') printf("current = '\\0'\n");
    // else printf("current: %c\n", *(g_param.da16200->uart_byte));

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*(g_param.da16200->uart_byte) == S4char))
    {
        /* Check if OK or ERROR */
        if (((g_param.da16200->uart_buffer[index - 3] == 'O') &&
             (g_param.da16200->uart_buffer[index - 2] == 'K')
            ) ||
            ((g_param.da16200->uart_buffer[index - 9] == 'E') &&
             (g_param.da16200->uart_buffer[index - 8] == 'R') &&
             (g_param.da16200->uart_buffer[index - 7] == 'R') &&
             (g_param.da16200->uart_buffer[index - 6] == 'O') &&
             (g_param.da16200->uart_buffer[index - 5] == 'R') &&
             (g_param.da16200->uart_buffer[index - 4] == ':') &&
             (g_param.da16200->uart_buffer[index - 3] == '-')
            ) ||
            ((g_param.da16200->uart_buffer[index - 10] == 'E') &&
             (g_param.da16200->uart_buffer[index - 9] == 'R') &&
             (g_param.da16200->uart_buffer[index - 8] == 'R') &&
             (g_param.da16200->uart_buffer[index - 7] == 'O') &&
             (g_param.da16200->uart_buffer[index - 6] == 'R') &&
             (g_param.da16200->uart_buffer[index - 5] == ':') &&
             (g_param.da16200->uart_buffer[index - 4] == '-')
            )
           )
        {
            g_param.da16200->uart_buffer[--index] = '\0';
            if (arg != NULL)
            {
                strcpy((char *) arg, g_param.da16200->uart_buffer);
            }
            end = 1;
            index = 0;
            pi_task_push(g_param.user_callback);
            //DA16200_DEBUG("Got OK or ERROR\n");
        }
        else
        {
            /* Got this case when the response has some text
            Schema is then as following:
            <S3><S4><text><S3><S4><S3><S4><code><S3><S4>
            We need therefore to parse again to get the code (OK|ERROR) */
            DA16200_DEBUG("Got something else than OK or ERROR -> Continue\n");
            --index;
        }
    }

    if (end == 0)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*(g_param.da16200->uart_byte) == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            g_param.da16200->uart_buffer[index++] = *(g_param.da16200->uart_byte);
        }
    }

    prev_byte = *(g_param.da16200->uart_byte);

    if (end == 0)
    {
        pi_task_callback(&rx_cb, __pi_da16200_resp_received, arg);
        pi_uart_read_async(&(g_param.da16200->uart_device),
                           (void *) g_param.da16200->uart_byte,
                           sizeof(char),
                           &rx_cb);
    }
}

/**
 * @brief Read a message from the client (asynchrone).
 * 
 * @param device 
 * @param[in] data_type 
 * @param[out] resp Buffer to store response (if NULL, no saving)
 * @param[in] callback Task created by the user/application, to be notify when the data have been received
 * 
 * @return int32_t 
 */
static int32_t __pi_da16200_read_data_async(struct pi_device *device, uint8_t data_type, char *resp, pi_task_t *callback)
{
    da16200_t *da16200 = (da16200_t *) device->data;

    g_at_resp_state = PI_AT_RESP_NOT_STARTED;

    g_param.has_error_code = 1;
    g_param.da16200 = da16200;
    g_param.task = &rx_cb;
    g_param.user_callback = callback;

    switch (data_type)
    {
        case STARTUP:
        {
            /* Since STARTUP is the first event received from the client,
            we have to drop all potential bytes sent before.
            Therefore we read them one by one for an easier parsing.
            */
            pi_task_callback(&rx_cb, __pi_da16200_startup_received, resp);
            pi_uart_read_async(&(da16200->uart_device), (void *) da16200->uart_byte, sizeof(char), &rx_cb);
            return CMD_RES_OK;
        }

        case MSG_RECEIVED:
        {
            // Nothing to do here, will be managed in callback
            break;
        }

        // case CLIENT_DISCONNECTED:
        // {
        //     sprintf(g_header.header, PI_AT_PREFIX_CLIENT_DISCONNECTED);
        //     g_header.code = 50;
        //     break;
        // }

        case CONNECTED_TO_AP:
        {
            strcpy(g_header.header, PI_AT_PREFIX_CONNECTED_TO_AP);
            g_header.code = 51;
            break;
        }

        case CLIENT_CONNECTED:
        {
            strcpy(g_header.header, PI_AT_PREFIX_CLIENT_CONNECTED);
            g_header.code = 52;
            break;
        }

        default:
        {
            return CMD_RES_ERR;
        }
    }

    pi_task_callback(&rx_cb, __pi_da16200_data_received, (void *) resp);
    pi_uart_read_async(&(da16200->uart_device), (void *) da16200->uart_buffer, 9, &rx_cb);

    return CMD_RES_OK;
}

/**
 * @brief Read a message from the client (synchrone).
 * 
 * @param device 
 * @param[in] data_type 
 * @param[out] resp Buffer to store response (if NULL, no saving)
 * 
 * @return int32_t 
 */
static int32_t __pi_da16200_read_data(struct pi_device *device, uint8_t data_type, char *resp)
{
    pi_task_t task;
    pi_task_block(&task);
    int32_t res = __pi_da16200_read_data_async(device, data_type, resp, &task);
    pi_task_wait_on(&task);
    return res;
}

/**
 * @brief Callback called by __pi_da16200_data_send_async() to send the buffer.
 * Prevents to concatenate the buffer to the header.
 * 
 * @param args Unused
 * 
 * @note Uses g_param_tx and g_tx_args
 */
static void __pi_da16200_data_send_async_cb(void *args)
{
    da16200_t *da16200 = (da16200_t *) g_param_tx.da16200;
    pi_uart_write_async(&(da16200->uart_device), (void *) g_tx_args.buffer, g_tx_args.size, g_param_tx.user_callback);
}

static int8_t enable_fc_pads(void)
{
    #define GPIO_UART1_RTS ( GPIO_MIKROE_CS  ) // Output
    #define GPIO_UART1_CTS ( GPIO_MIKROE_PWM ) // Input

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
 * IOCTL functions (not present in the API)
 ******************************************************************************/

static int8_t __pi_da16200_uart_settings_set(struct pi_device *device,
                                             struct pi_da16200_conf *conf)
{
    char param[15], cmd[17];
    sprintf(param, "%d,%d,%c,%d,%d",
            conf->uart_baudrate,
            PI_DA16200_UART_DATA_BITS,
            conf->parity_bits,
            conf->stop_bits,
            conf->use_ctrl_flow
           );
    strcpy(cmd, __XSTRING(PI_AT_UART_SETTINGS_SET));
    strcat(cmd, param);
    return __pi_da16200_at_cmd(device, cmd, NULL);
}


/*******************************************************************************
 * API implementation
 ******************************************************************************/

static int8_t __pi_da16200_open(struct pi_device *device)
{
    #define GPIO_DA16200_PWRON ( GPIO_MIKROE_3V3 )

	pi_device_t gpio_pwron;
    struct pi_gpio_conf gpio_pwron_conf;

    pi_gpio_conf_init(&gpio_pwron_conf);
    gpio_pwron_conf.port = GPIO_DA16200_PWRON / 32;
    pi_open_from_conf(&gpio_pwron, &gpio_pwron_conf);

    if (pi_gpio_open(&gpio_pwron))
    {
        return -1;
    }

	DA16200_DEBUG("Open bsp da16200\n");
	bsp_da16200_open();

	/* Config device GPIO & pads. */
    pi_gpio_pin_configure(&gpio_pwron, GPIO_DA16200_PWRON, PI_GPIO_OUTPUT);
    pi_gpio_pin_write(&gpio_pwron, GPIO_DA16200_PWRON, 0);
    pi_time_wait_us(10000);


    /* Allocate space for da16200_t and its fields. */
    void *l2_malloc = (void *) pi_l2_malloc(sizeof(da16200_t)
                                            + sizeof(char)
                                            + sizeof(char) * PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH
                                            + sizeof(char) * MAX_HEADER_SIZE
                                            + sizeof(char) * MAX_CLIENT_IP_PORT_SIZE);
	if (l2_malloc == NULL)
	{
		printf("Malloc failed for da16200_t\n");
		return -2;
	}
    da16200_t *da16200 = (da16200_t *) l2_malloc;
	da16200->uart_byte      = (char *) (l2_malloc + sizeof(da16200_t));
    da16200->uart_buffer    = (char *) (l2_malloc + sizeof(da16200_t) + sizeof(char));
    da16200->header         = (char *) (l2_malloc + sizeof(da16200_t) + sizeof(char) * (1 + PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH));
    da16200->client_ip_port = (char *) (l2_malloc + sizeof(da16200_t)
                                        + sizeof(char) * (1 + PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH + MAX_HEADER_SIZE));

    device->data = (void *) da16200;


	/* Init and open UART device. */
    struct pi_da16200_conf *conf = (struct pi_da16200_conf *)device->config;

    struct pi_uart_conf uart_conf;
    pi_uart_conf_init(&uart_conf);
    uart_conf.uart_id       = conf->uart_itf;
    uart_conf.baudrate_bps  = conf->uart_baudrate;
    uart_conf.enable_rx     = 1;
    uart_conf.enable_tx     = 1;
    uart_conf.use_ctrl_flow = conf->use_ctrl_flow;
    pi_open_from_conf(&(da16200->uart_device), &uart_conf);

    if (pi_uart_open(&(da16200->uart_device)))
    {
        pi_l2_free(da16200->uart_byte, sizeof(char));
        pi_l2_free(da16200->uart_buffer, sizeof(char) * PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH);
        pi_l2_free(da16200->header, sizeof(char) * MAX_HEADER_SIZE);
        pi_l2_free(da16200->client_ip_port, sizeof(char) * MAX_CLIENT_IP_PORT_SIZE);
        pi_l2_free(da16200, sizeof(da16200_t));
        return -4;
    }

    DA16200_DEBUG("Enable DA16200\n");
    /* Enable DA16200. */
    pi_gpio_pin_write(&gpio_pwron, GPIO_DA16200_PWRON, 1);
    pi_time_wait_us(20000);

    /* Set pads for flow control */
    if (conf->use_ctrl_flow)
    {
        enable_fc_pads();
    }

    DA16200_DEBUG("Read startup message\n");
    __pi_da16200_read_data(device, STARTUP, NULL);

	/* Test if DA16200 is active. */
    cmd_res_e da16200_active = __pi_da16200_at_cmd(device, "", NULL);
    if (da16200_active == CMD_RES_ERR)
    {
        return -5;
    }
    return 0;
}

static void __pi_da16200_close(struct pi_device *device)
{
	da16200_t *da16200 = (da16200_t *) device->data;
	pi_uart_close(&(da16200->uart_device));
	pi_l2_free(da16200->uart_byte, sizeof(char));
	pi_l2_free(da16200->uart_buffer, sizeof(char) * PI_AT_WIFI_DEFAULT_RESP_ARRAY_LENGTH);
    pi_l2_free(da16200->header, sizeof(char) * MAX_HEADER_SIZE);
	pi_l2_free(da16200->client_ip_port, sizeof(char) * MAX_CLIENT_IP_PORT_SIZE);
    pi_l2_free(da16200, sizeof(da16200_t));
}

/**
 * @brief Connect to an AP
 * 
 * @param device 
 * @param[in] ssid 
 * @param[in] pwd Password. 8 ~ 63 characters. If NULL, security protocol is OPEN.
 * @param[in] callback Task to notify when the module is connected to the AP.
 * @return int8_t 
 */
static int8_t __pi_da16200_connect_to_ap(struct pi_device *device, const char *ssid, const char *pwd, pi_task_t *callback)
{
    char str[200];
    strcpy(str, __XSTRING(PI_AT_CONNECT_TO_AP));

    strcat(str, "'");
    strcat(str, ssid);
    strcat(str, "'");

    if (pwd != NULL)
    {
        strcat(str, ",");
        strcat(str, "'");
        strcat(str, pwd);
        strcat(str, "'");
    }

    if (__pi_da16200_at_cmd(device, str, NULL) != CMD_RES_OK)
    {
        return -1;
    }
    return __pi_da16200_read_data_async(device, CONNECTED_TO_AP, NULL, callback);
}

/**
 * @brief Disconnect from the currently associated AP
 * 
 * @param device 
 * @return int8_t 
 */
static int8_t __pi_da16200_disconnect_from_ap(struct pi_device *device)
{
    return __pi_da16200_at_cmd(device, __XSTRING(PI_AT_DISCONNECT_FROM_AP), NULL);
}

static int8_t __pi_da16200_create_tcp_server(struct pi_device *device, uint16_t port)
{
    char str[20];
    strcpy(str, __XSTRING(PI_AT_OPEN_TCP_SERVER));

    char port_str[10] = {0};
    uint16_to_str(port, port_str);
    l_trim(port_str);
    strcat(str, port_str);

    return __pi_da16200_at_cmd(device, str, NULL);
}

/**
 * @brief Wait for a TCP client connection
 * 
 * @param device 
 * @param[out] resp Buffer to store the connection event, useful if parsing is needed.
 * @param[in] callback Task to notify when a client is connecting.
 * @return int8_t 
 */
static int8_t __pi_da16200_wait_client_connection(struct pi_device *device, char *resp, pi_task_t *callback)
{
    return __pi_da16200_read_data_async(device, CLIENT_CONNECTED, resp, callback);
}

/**
 * @brief Wait an event (eg. msg) from the client
 * 
 * @param device 
 * @param[out] resp Buffer to store the event (msg) sent by the client.
 * @param[in] callback Task to notify when an event is received.
 * @return int8_t 
 * 
 * @note Asynchrone.
 */
static int8_t __pi_da16200_wait_client_event(struct pi_device *device, char *resp, pi_task_t *callback)
{
    return __pi_da16200_read_data_async(device, MSG_RECEIVED, resp, callback);
}

/**
 * @brief Read data directly from uart (asynchrone).
 * 
 * @param device 
 * @param ext_addr Unused
 * @param buffer Buffer to store the response (if NULL, no saving)
 * @param size Size of data to read
 * @param task Callback for the user
 * 
 * @return ssize_t 
 */
static ssize_t __pi_da16200_data_get_async(struct pi_device *device, uint32_t ext_addr,
                                    void *buffer, uint32_t size, pi_task_t *task)
{
    da16200_t *da16200 = (da16200_t *) device->data;
    pi_uart_read_async(&(da16200->uart_device), buffer, size, task);

	return 0;
}

/**
 * @brief Send a message to the client (asynchrone).
 * Construct the header and send it. Call callback when finish that will send the buffer.
 * This callback prevent to concatenate the buffer to the header.
 * 
 * @param[in] device 
 * @param ext_addr Unused
 * @param[in] buffer Message (char *) to send to the client
 * @param[in] size Size of the message
 * @param[out] task Callback when process is finished. NOT NULL !
 * 
 * @return ssize_t Operation status
 * \retval 0 if success
 * \retval -1 if error
 * 
 * @warning MAX SIZE = 2048. Otherwhise the module will return an error.
 */
static ssize_t __pi_da16200_data_send_async(struct pi_device *device, uint32_t ext_addr,
                                    const char *buffer, uint32_t size, pi_task_t *task)
{
    if (task == NULL) return -1;

	da16200_t *da16200 = (da16200_t *) device->data;

    /* ----------- Initialization of global variables -------------------------------- */

    g_param_tx.da16200        = da16200;
    g_param_tx.user_callback  = task;

    g_tx_args.size            = size;
    g_tx_args.buffer          = buffer;

    /* ----------- Setup sequence to send -----------------------------------------------
     * Sequence to send: \eS0<size>,<client_ip>,<client_port>,r,<msg> 
     */

    char size_str[10] = {0};
    sprintf(size_str, "%d", size);

    uint32_t header_length = 3 + strlen(size_str) + 1 + strlen(da16200->client_ip_port) + 3;

    strcpy(da16200->header, (char*) "\eS0");
    strcat(da16200->header, size_str);
    strcat(da16200->header, ",");
    strcat(da16200->header, da16200->client_ip_port);
    strcat(da16200->header, ",r,");

    /* ----------- Sending ----------------------------------------------------------- */
    //DA16200_DEBUG("Sending size = %d + %d\n", header_length, size);
    pi_task_callback(&tx_cb, __pi_da16200_data_send_async_cb, NULL);
    pi_uart_write_async(&(da16200->uart_device), (void *) da16200->header, header_length, &tx_cb);

    return 0;
}

static int __pi_da16200_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    da16200_t *da16200 = (da16200_t *) device->data;
    bsp_da16200_ioctl_cmd_e func_id = (bsp_da16200_ioctl_cmd_e) cmd;
    switch (func_id)
    {
		case BSP_DA16200_TCP_SERVER_CONFIGURE:
		{
            __pi_da16200_create_tcp_server(device, *((uint16_t *) arg));
		}
			break;

		case BSP_DA16200_TCP_CLIENT_CONFIGURE:
		{
			/**
			 * @todo
			 */
            printf("Warning: This case is not available for now.\n");
		}
			break;

		case BSP_DA16200_UART_CONFIGURE:
		{
			struct pi_da16200_conf *conf = (struct pi_da16200_conf *) arg;
			cmd_res_e res = __pi_da16200_uart_settings_set(device, conf);
			if (res == CMD_RES_OK)
			{
				struct pi_uart_conf uart_conf;
				pi_uart_conf_init(&uart_conf);
				uart_conf.uart_id        = conf->uart_itf;
				uart_conf.baudrate_bps   = conf->uart_baudrate;
				uart_conf.enable_rx      = 1;
				uart_conf.enable_tx      = 1;
				uart_conf.use_ctrl_flow  = conf->use_ctrl_flow;
				uart_conf.parity_mode    = (conf->parity_bits == 'n') ? 0 : 1;
				uart_conf.stop_bit_count = conf->stop_bits;

				pi_uart_ioctl(&(da16200->uart_device), PI_UART_IOCTL_ABORT_RX,
							NULL);
				pi_uart_ioctl(&(da16200->uart_device), PI_UART_IOCTL_ABORT_TX,
							NULL);
				pi_uart_ioctl(&(da16200->uart_device), PI_UART_IOCTL_CONF_SETUP,
							&uart_conf);
			}
        #ifndef __PLATFORM_RTL__
            /* wait 40ms for module to update */
            pi_time_wait_us(40000);
        #endif
		}
			break;

		default:
		{
			printf("Error: unknown ioctl command !\n");
			return -1;
		}
    }

    return 0;
}

/**
 * @brief Send AT command
 * 
 * @param device 
 * @param[in] cmd Command to send without 'AT' prefix, nor '\\r' suffix
 * @param[out] resp Buffer to store response (if NULL, no saving)
 * @return int8_t 
 */
static int8_t __pi_da16200_at_cmd(struct pi_device *device, char *cmd, char *resp)
{
    da16200_t *da16200 = (da16200_t *) device->data;

    /* ----------- Setup sequence to send -----------------------------------------------
     * Sequence to send: AT<cmd>\r 
     */

    uint32_t cmd_length = strlen(cmd);
    uint32_t seq_length = 2 + cmd_length + 1;
    char *seq_string = (char *) pi_l2_malloc(sizeof(char) * seq_length);
    if (seq_string == NULL)
    {
        return -4;
    }

    strcpy(seq_string, (char*) "AT");
    if (cmd_length != 0)
    {
        strcat(seq_string, (char*) cmd);
    }
    strcat(seq_string, (char*) S3str);


    /* ----------- Setup callback to parse code OK | ERROR --------------------------- */
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;

    pi_task_t task;
    pi_task_block(&task);

	pi_task_t rx_cb = {0};
    g_param.has_error_code = 1;
    g_param.da16200        = da16200;
    g_param.task           = &rx_cb;
    g_param.user_callback  = &task;

    pi_task_callback(&rx_cb, __pi_da16200_resp_received, resp);
    pi_uart_read_async(&(da16200->uart_device), (void *) da16200->uart_byte, sizeof(uint8_t), &rx_cb);


    /* ----------- Sending ----------------------------------------------------------- */
    DA16200_DEBUG("Sending: %s\n", seq_string);
    pi_uart_write(&(da16200->uart_device), seq_string, seq_length);


    /* ----------- Waiting callback to be triggered ---------------------------------- */
    pi_task_wait_on(&task);


    /* ----------- Check result code ------------------------------------------------- */
    cmd_res_e cmd_res = CMD_RES_NA;
    uint32_t last_char_pos = strlen((const char *) da16200->uart_buffer);
    if ((da16200->uart_buffer[last_char_pos - 2] == 'O') &&
        (da16200->uart_buffer[last_char_pos - 1] == 'K'))
    {
        DA16200_DEBUG("OK response received !\n");
        cmd_res = CMD_RES_OK;
    }
    else if ((da16200->uart_buffer[last_char_pos - 8] == 'E') &&
             (da16200->uart_buffer[last_char_pos - 7] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 6] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 5] == 'O') &&
             (da16200->uart_buffer[last_char_pos - 4] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 3] == ':') &&
             (da16200->uart_buffer[last_char_pos - 2] == '-')
            )
    {
        int8_t code = da16200->uart_buffer[last_char_pos - 1];
        DA16200_DEBUG("ERROR response received ! Code: -%d\n", code);
        cmd_res = CMD_RES_ERR;
    }
    else if ((da16200->uart_buffer[last_char_pos - 9] == 'E') &&
             (da16200->uart_buffer[last_char_pos - 8] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 7] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 6] == 'O') &&
             (da16200->uart_buffer[last_char_pos - 5] == 'R') &&
             (da16200->uart_buffer[last_char_pos - 4] == ':') &&
             (da16200->uart_buffer[last_char_pos - 3] == '-')
            )
        {
        int8_t code = da16200->uart_buffer[last_char_pos - 2] * 10 
                       + da16200->uart_buffer[last_char_pos - 1];
        DA16200_DEBUG("ERROR response received ! Code: -%d\n", code);
        cmd_res = CMD_RES_ERR;
    }
    else
    {
        DA16200_DEBUG("Unsolicited/unrecognised response received: \"%s\" (len: %ld) !\n", da16200->uart_buffer, last_char_pos);
        cmd_res = CMD_RES_UNSOL;
    }


    /* ----------- Post process ------------------------------------------------------ */
    pi_l2_free(seq_string, sizeof(char) * seq_length);

    return cmd_res;
}


/*******************************************************************************
 * API definition and module initialization (setup default values)
 ******************************************************************************/

static pi_wifi_api_t g_da16200_wifi_api =
{
    /* WiFi specific API. */
	.at_cmd                 = &__pi_da16200_at_cmd,
    .connect_to_ap          = &__pi_da16200_connect_to_ap,
	.disconnect_from_ap     = &__pi_da16200_disconnect_from_ap,
    .create_tcp_server      = &__pi_da16200_create_tcp_server,
    .wait_client_connection = &__pi_da16200_wait_client_connection,
    .wait_client_event      = &__pi_da16200_wait_client_event,
};

static pi_device_api_t g_da16200_api =
{
    .open              = &__pi_da16200_open,
    .close             = &__pi_da16200_close,
	.read              = &__pi_da16200_data_get_async,
	.write             = &__pi_da16200_data_send_async,
	.ioctl             = &__pi_da16200_ioctl,

	.specific_api      = &g_da16200_wifi_api,
};

void pi_wifi_da16200_conf_init(struct pi_device *device, struct pi_da16200_conf *conf)
{
	bsp_da16200_conf_init(conf);

	conf->uart_baudrate            = PI_DA16200_UART_BAUDRATE;
	conf->parity_bits              = 'n'; /* NO parity    */
	conf->stop_bits                = 1;   /* 1 stop bit   */
	conf->use_ctrl_flow            = 1;   /* Flow control */

	device->api = &g_da16200_api;
}


/*******************************************************************************
 * External functions (other than init)
 ******************************************************************************/

void pi_wifi_da16200_client_ip_port_set(struct pi_device *device, const char *client_ip_port)
{
    da16200_t *da16200 = (da16200_t *) device->data;
    sprintf((char *) (da16200->client_ip_port), client_ip_port);
}
