/* *****************************************************************************
 * Copyright (c) 2022 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ****************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bsp/wifi.h"
#include "bsp/bsp.h"
#include "bsp/transport/at_commands.h"


// Set AP SSID
#define AP_SSID                     "Galaxy A12CBEA"
// Set AP password - if the AP is OPEN remain this NULL
#define AP_PASSWORD                 "sqtv7882"

// Set Local port on which the TCP server will be created
#define LOCAL_PORT                  5228

#define BUFFER_SIZE                 512
#define MAX_TX_DATA_SIZE            2048

char tx_buffer[MAX_TX_DATA_SIZE + 50];
PI_L2 char rx_buffer[BUFFER_SIZE + 50];
char client_request[BUFFER_SIZE];

struct pi_device da16200;

#define DEFAULT_RESPONSE "Hi from server (DA16200)!"


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
    uint8_t i = 0;
    for (uint8_t counter = 0; counter < count; i++)
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
    int8_t errors = 0;

    printf("\nInit test\n*****\n");
    pi_freq_set(PI_FREQ_DOMAIN_FC, 370*1000*1000);
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 50*1000*1000);

    struct pi_da16200_conf conf = {0};
    pi_wifi_da16200_conf_init(&da16200, &conf);
    conf.uart_baudrate = 921600;
#ifndef FLOW_CONTROL
    conf.use_ctrl_flow = 0;
#endif
    pi_open_from_conf(&da16200, &conf);

    errors += pi_wifi_open(&da16200);
    if (errors)
    {
        printf("Failed to open wifi da16200 module : %d\n", errors);
        pmsis_exit(-1);
    }
    printf("WiFi DA16200 module opened!\n");


    printf("\nInit Completed\nLaunch test\n*****\n\n");

    pi_task_t task;

    // ------------------------------------------------------------------------
    printf("--- Connecting to the access point ---\n");
    pi_task_block(&task);
    pi_wifi_connect_to_ap(&da16200, AP_SSID, AP_PASSWORD, &task);
    pi_task_wait_on(&task);

    // ------------------------------------------------------------------------
    printf("\n--- Open a TCP server socket ---\n");
    pi_wifi_create_tcp_server(&da16200, LOCAL_PORT);

    printf("Waiting for client connection\n");
    pi_task_block(&task);
    pi_wifi_wait_client_connection(&da16200, rx_buffer, &task);

    char client_ip_port[25]; // IP + PORT of the client
    pi_task_wait_on(&task);

    parse_response(rx_buffer, client_ip_port, ',', 1);
    printf("Client's IP and port: %s\n", client_ip_port);
    pi_wifi_da16200_client_ip_port_set(&da16200, client_ip_port);

    // ------------------------------------------------------------------------
    printf("\n--- Start communication with client ---\n");
    while (1)
    {
        pi_task_block(&task);
        memset(rx_buffer, 0, BUFFER_SIZE + 50);
        pi_wifi_wait_client_event(&da16200, rx_buffer, &task);
        pi_task_wait_on(&task);

        if (rx_buffer[0] == 49)
        {
            parse_response(rx_buffer, client_request, ',', 4);
            printf("Message from client: %s\n\n", client_request);

            memset(tx_buffer, 0, MAX_TX_DATA_SIZE + 50);
            sprintf(tx_buffer, DEFAULT_RESPONSE);

            pi_task_block(&task);
            memset(rx_buffer, 0, 4);
            pi_wifi_data_get_async(&da16200, rx_buffer, 0, &task);
            pi_wifi_data_send_async(&da16200, tx_buffer, strlen(tx_buffer), NULL);
            pi_task_wait_on(&task);
        }
        else if (rx_buffer[1] == 50)
        {
            printf("Client disconnected!\n");

            printf("Waiting for client connection\n");
            pi_task_block(&task);
            pi_wifi_wait_client_connection(&da16200, rx_buffer, &task);

            memset(client_ip_port, 0, 25);
            pi_task_wait_on(&task);

            parse_response(rx_buffer, client_ip_port, ',', 1);
            printf("Client's IP and port: %s\n", client_ip_port);
            pi_wifi_da16200_client_ip_port_set(&da16200, client_ip_port);

            printf("\n--- Start communication with client ---\n");
        }
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
