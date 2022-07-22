#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wifi_lp.h"
#include "bsp/transport/at_commands.h"


// Set AP SSID
#define AP_SSID                     ""
// Set AP password - if the AP is OPEN remain this NULL
#define AP_PASSWORD                 ""

// Set Local port on which the TCP server will be created
#define LOCAL_PORT                  5228

#define BUFFER_SIZE 1024

char tx_buffer[BUFFER_SIZE + 50];
PI_L2 char rx_buffer[BUFFER_SIZE + 50];
char client_request[BUFFER_SIZE];
char resp_request[BUFFER_SIZE];

PI_L2 char startup_buffer[13];
char *rx_byte;

struct pi_device uart;
pi_task_t start_cb = {0};
pi_task_t ok_cb = {0};
pi_task_t rx_cb = {0};

static volatile at_resp_state_e g_at_resp_state;

#define DEFAULT_RESPONSE "Hi from server (DA16200)!"


static void startup_received(void *arg)
{
    /* Here, with flow control,
       printf are allowed,
       and any other long process in general */

    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    // if (*rx_byte == '\r') printf("current = '\\r'\n");
    // else if (*rx_byte == '\n') printf("current = '\\n'\n");
    // else if (*rx_byte == '\0') printf("current = '\\0'\n");
    // else printf("current: %c\n", *rx_byte);

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*rx_byte == S4char))
    {
        /* Check if startup message */
        if ((startup_buffer[index - 13] == '+') &&
            (startup_buffer[index - 12] == 'I') &&
            (startup_buffer[index - 11] == 'N') &&
            (startup_buffer[index - 10] == 'I') &&
            (startup_buffer[index -  9] == 'T') &&
            (startup_buffer[index -  8] == ':') &&
            (startup_buffer[index -  7] == 'D') &&
            (startup_buffer[index -  6] == 'O') &&
            (startup_buffer[index -  5] == 'N') &&
            (startup_buffer[index -  4] == 'E') &&
            (startup_buffer[index -  3] == ',') &&
            ((startup_buffer[index - 2] == '0') ||
             (startup_buffer[index - 2] == '1')
            )
           )
        {
            startup_buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
        else
        {
            printf("Got unsollicited response : %s\n", startup_buffer);
        }
    }

    if (0 == end)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*rx_byte == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            startup_buffer[index++] = *rx_byte;
        }
    }
    prev_byte = *rx_byte;

    if (0 == end)
    {
        pi_task_callback(&start_cb, startup_received, NULL);
        pi_uart_read_async(&uart, rx_byte, 1, &start_cb);
    }
}

static void wfjap_received(void *arg)
{
    /* Put some logs only if necessary !
       uart is too fast for printf */

    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    // if (*rx_byte == '\r') printf("current = '\\r'\n");
    // else if (*rx_byte == '\n') printf("current = '\\n'\n");
    // else if (*rx_byte == '\0') printf("current = '\\0'\n");
    // else printf("current: %c\n", *rx_byte);

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*rx_byte == S4char))
    {
        /* Check if +WFJAP */
        if ((rx_buffer[0] == '+') &&
            (rx_buffer[1] == 'W') &&
            (rx_buffer[2] == 'F') &&
            (rx_buffer[3] == 'J') &&
            (rx_buffer[4] == 'A') &&
            (rx_buffer[5] == 'P')
           )
        {
            rx_buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
    }

    if (end == 0)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*rx_byte == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            rx_buffer[index++] = *rx_byte;
        }
    }
    prev_byte = *rx_byte;

    if (end == 0)
    {
        pi_task_callback(&rx_cb, wfjap_received, NULL);
        pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    }
}

static void trcts_received(void *arg)
{
    /* Put some logs only if necessary !
       uart is too fast for printf */

    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*rx_byte == S4char))
    {
        /* Check if +TRCTS */
        if ((rx_buffer[0] == '+') &&
            (rx_buffer[1] == 'T') &&
            (rx_buffer[2] == 'R') &&
            (rx_buffer[3] == 'C') &&
            (rx_buffer[4] == 'T') &&
            (rx_buffer[5] == 'S')
           )
        {
            rx_buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
    }

    if (end == 0)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*rx_byte == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            rx_buffer[index++] = *rx_byte;
        }
    }
    prev_byte = *rx_byte;

    if (end == 0)
    {
        pi_task_callback(&rx_cb, trcts_received, NULL);
        pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    }
}

static void data_received(void *arg)
{
    /* Put some logs only if necessary !
       uart is too fast for printf */

    static uint32_t index = 0;
    static unsigned char prev_byte = 0;

    uint8_t end = 0;

    // if (*rx_byte == '\r') printf("current = '\\r'\n");
    // else if (*rx_byte == '\n') printf("current = '\\n'\n");
    // else if (*rx_byte == '\0') printf("current = '\\0'\n");
    // else printf("current: %c\n", *rx_byte);

    if ((g_at_resp_state == PI_AT_RESP_IN_PROGRESS) &&
        (prev_byte == S3char) && (*rx_byte == S4char))
    {
        /* Check if +TRDTS */
        if ((rx_buffer[0] == '+') &&
            (rx_buffer[1] == 'T') &&
            (rx_buffer[2] == 'R') &&
            (rx_buffer[3] == 'D') &&
            (rx_buffer[4] == 'T') &&
            (rx_buffer[5] == 'S')
           )
        {
            rx_buffer[--index] = '\0';
            g_at_resp_state = PI_AT_RESP_DONE;
            end = 1;
        }
    }

    if (end == 0)
    {
        if ((g_at_resp_state == PI_AT_RESP_NOT_STARTED) &&
            (prev_byte == S3char) && (*rx_byte == S4char))
        {
            index = 0;
            g_at_resp_state = PI_AT_RESP_IN_PROGRESS;
        }
        else if (g_at_resp_state == PI_AT_RESP_IN_PROGRESS)
        {
            rx_buffer[index++] = *rx_byte;
        }
    }
    prev_byte = *rx_byte;

    if (end == 0)
    {
        pi_task_callback(&rx_cb, data_received, NULL);
        pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    }
}

/**
 * @brief Get the "last" part of the given buffer
 * 
 * @param[in] buffer Complete data
 * @param[out] resp To store the last part of the buffer
 * @param[in] delimiter A char to delimit the last part to keep
 * @param[in] count Nb of delimiter to go through it before reaching the last part
 */
static void parse_response(char *buffer, char *resp, char delimiter, uint8_t count)
{
    uint8_t counter = 0;
    uint8_t i = 0;
    for (; counter < count; i++)
    {
        if (buffer[i] == delimiter)
        {
            counter++;
        }
    }
    strcpy((void *) resp, (void *) buffer+i);
}

static int test()
{
    printf("\n\n*****\nEntered test\n*****\n\n");
    pi_freq_set(PI_FREQ_DOMAIN_FC, 370*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 50*1000*1000);

    if (lpwifi_init(&uart, 115200))
    {
        printf("Error opening uart...");
        pmsis_exit(0);
    }

    rx_byte = (char *) pi_l2_malloc(sizeof(char));

    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_callback(&start_cb, startup_received, NULL);
    pi_uart_read_async(&uart, rx_byte, 1, &start_cb);

    enable_cs_wakup();

    // printf("\n --- Factory reset --- \n");
    // lpwifi_factory_reset_device(&uart);

    printf("Waiting for startup message\n");
    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    printf("--- Done ---\n");

    printf("\n***** Init Completed! *****\n\n");


    // ------------------------------------------------------------------------
    // Connect to AP: AT+WFJAPA
    printf("--- Connecting to the access point ---\n");
    // Set callback to parse OK response for successful command send
    pi_task_block(&ok_cb);
    pi_uart_read_async(&uart, rx_buffer, 6, &ok_cb);

    // Set callback to get response
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_callback(&rx_cb, wfjap_received, NULL);

    // Send AT command
    lpwifi_connect_to_ap(&uart, AP_SSID, AP_PASSWORD);

    // Wait for OK
    pi_task_wait_on(&ok_cb);

    // Get response
    pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    // Should receive +WFJAP response when connected to AP
    printf("Received: %s\n", rx_buffer);


    // ------------------------------------------------------------------------
    // TCP server: AT+TRTS
    printf("\n--- Open a TCP server socket ---\n");
    // Set callback to parse OK response for successful command send
    pi_task_block(&ok_cb);
    pi_uart_read_async(&uart, rx_buffer, 6, &ok_cb);

    // Set callback to get response
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_callback(&rx_cb, trcts_received, NULL);

    // Send AT command
    lpwifi_create_tcp_server(&uart, LOCAL_PORT);

    // Wait for OK
    pi_task_wait_on(&ok_cb);

    printf("Waiting for client connection\n");
    // Get response
    pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    while (g_at_resp_state != PI_AT_RESP_DONE)
    {
        pi_yield();
    }
    char client_ip_port[25]; // IP + PORT of the client
    parse_response(rx_buffer, client_ip_port, ',', 1);
    // Should receive AT+TRCTS response when a remote TCP client is connected
    printf("Received: %s\n", rx_buffer);


    // ------------------------------------------------------------------------
    printf("\n--- Start communication with client ---\n");
    // The client will send data first; Setting a callback to get it
    g_at_resp_state = PI_AT_RESP_NOT_STARTED;
    pi_task_callback(&rx_cb, data_received, NULL);
    pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);

    char size[10];
#ifdef PERFS
    long long start = 0;
#endif
    while (1)
    {
        // Waiting request from client
        while (g_at_resp_state != PI_AT_RESP_DONE)
        {
            pi_yield();
        }
#ifdef PERFS
        long long time = pi_time_get_us() - start;
        printf("Took %ld us\n", time);
        printf("Size %d\n", strlen(rx_buffer));
#endif
        parse_response(rx_buffer, client_request, ',', 4);
        printf("Received from client: %s\nMessage: %s\n\n", rx_buffer, client_request);

        memset(resp_request, 0, BUFFER_SIZE);
#ifdef PERFS
        resp_request[0] = 1;
        for (uint16_t i = 1; strlen(resp_request) < BUFFER_SIZE && i < BUFFER_SIZE; i++)
        {
            resp_request[i] = 1;
        }
        resp_request[BUFFER_SIZE] = 0;
#else
        sprintf(resp_request, DEFAULT_RESPONSE);
#endif
        //printf("Response = %s\n", resp_request);
        memset(size, 0, 10);
        sprintf(size, "%d", strlen(resp_request));
        printf("Size = %s\n", size);

        memset(tx_buffer, 0, BUFFER_SIZE + 50);
        sprintf(tx_buffer, "\eS0");
        strcat(tx_buffer, size);
        strcat(tx_buffer, ",");
        strcat(tx_buffer, client_ip_port);
        strcat(tx_buffer, ",");
        strcat(tx_buffer, resp_request);

        // Set callback to parse OK response for successful command send
        pi_task_block(&ok_cb);
        pi_uart_read_async(&uart, rx_buffer, 6, &ok_cb);

        memset(rx_buffer, 0, 50);

#ifdef PERFS
        start = pi_time_get_us();
#endif
        // Respond to client
        lpwifi_send_cmd_payload(&uart, tx_buffer, 5 + strlen(size) + strlen(resp_request) + strlen(client_ip_port));

        // Wait for OK
        pi_task_wait_on(&ok_cb);

        // Callback to get request from client
        g_at_resp_state = PI_AT_RESP_NOT_STARTED;
        pi_task_callback(&rx_cb, data_received, NULL);
        pi_uart_read_async(&uart, rx_byte, 1, &rx_cb);
    }

    return 0;
}

static void test_entry()
{
    int status = test();
    pmsis_exit(status);
}

int main(void)
{
    return pmsis_kickoff((void *)test_entry);
}
