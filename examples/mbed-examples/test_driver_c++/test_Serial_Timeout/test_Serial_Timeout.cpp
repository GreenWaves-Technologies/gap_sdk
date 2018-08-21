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

Serial pc(USBTX, USBRX); // tx, rx

Thread thread;
Timeout flipper;

uint8_t buffer_rx[5];

void timeout() {
    if (!pc.readable()) {
        printf("TIMEOUT check: read failed, release uart read!\n");

        pc.abort_read();

    } else {
        printf("TIMEOUT check: read success, do something!\n");
        printf("Received bytes : %s\n", buffer_rx);
    }
}

int main() {
    pc.baud(115200);

    /* Non-Blocking read 5 bytes - hello */
    pc.read((uint8_t *)buffer_rx, 5, NULL, SERIAL_EVENT_RX_ALL, 0);

    /* Timeout check in 10s */
    flipper.attach(&timeout, 10);

    printf("Please input 5 bytes in 10s!\n");

    /* Wait the end of read, or you can do other things */
    wait(15);

    while (1) {
        printf("Please continue input byte in 1s!\n");

        wait(1);

        /* Blocking read */
        printf("Get Char = %x\n", pc.getc());
    }
}
