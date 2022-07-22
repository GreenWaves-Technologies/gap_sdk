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
#include "bsp/wifi.h"
#include "bsp/transport/da16200.h"
#include "bsp/transport/at_commands.h"


static int8_t test()
{
    int8_t errors = 0;

    printf("\nInit test\n*****\n");

    static struct pi_device da16200;
    struct pi_da16200_conf conf = {0};
    pi_wifi_da16200_conf_init(&da16200, &conf);
    conf.uart_baudrate = 921600;
    conf.use_ctrl_flow = FLOW_CONTROL;
    pi_open_from_conf(&da16200, &conf);

    errors += pi_wifi_open(&da16200);
    if (errors)
    {
        printf("Failed to open wifi da16200 module : %d\n", errors);
        pmsis_exit(-1);
    }
    printf("WiFi DA16200 module opened!\n");


    printf("\nLaunch test\n*****\n\n");

    conf.uart_baudrate = 230400;
    errors += pi_wifi_ioctl(&da16200, BSP_DA16200_UART_CONFIGURE, &conf);
    if (errors)
    {
        printf("Failed to change UART baudrate!\n");
    }
    else
    {
        printf("UART baudrate changed: %d\n", ((struct pi_da16200_conf *)(da16200.config))->uart_baudrate);

        // Go back to default configuration
        conf.uart_baudrate = 115200;
        while (pi_wifi_ioctl(&da16200, BSP_DA16200_UART_CONFIGURE, &conf))
        {
            printf("Error while going back to the default configuration...Retrying\n");
        }
    }

    return errors;
}

static void test_entry()
{
    printf("\n*****\nEnter test\n*****\n");

    int8_t error = test();
    if (error)
    {
        printf("\n*****\nTest failed!\n");
    }
    else
    {
        printf("\n*****\nTest succeed!\n");
    }

    pmsis_exit(error);
}

int main(void)
{
    return pmsis_kickoff((void *)test_entry);
}
