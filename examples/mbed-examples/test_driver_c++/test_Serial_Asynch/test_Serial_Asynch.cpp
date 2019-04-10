 /**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/rawserial.html
 */
/* mbed Example Program
 * Copyright (c) 2006-2014 ARM Limited
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
#include "mbed.h"

Serial serial(USBTX, USBRX); // tx, rx

const uint8_t buffer_tx[] = "How do you do today\r\n";

// Read buffer's size should larger than write buffer size for test only
uint8_t buffer_rx[25];

static void serial_write_callback(int events)
{
    if(events == SERIAL_EVENT_TX_COMPLETE)
        printf("TX Callback, event = %d!\n", events);
    else if(events == SERIAL_EVENT_RX_COMPLETE)
    {
        printf("RX Callback, event = %d!\n", events);
        // Read finished, print read buffer
        printf("%s\n", buffer_rx);
    }
    else
    {
        printf("Test failed!\n");
        return;
    }

    printf("Test success!\n");
}

int main() {
    serial.baud(115200);

    event_callback_t event;

    event = serial_write_callback;

    serial.read((uint8_t *)buffer_rx, sizeof(buffer_tx), event, SERIAL_EVENT_RX_ALL, 0);

    serial.write((uint8_t *)buffer_tx, sizeof(buffer_tx), event, SERIAL_EVENT_TX_ALL);

    wait(50);
}
