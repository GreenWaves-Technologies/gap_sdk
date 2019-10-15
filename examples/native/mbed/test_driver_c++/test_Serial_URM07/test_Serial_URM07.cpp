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

#define HEADER_HI               0x55
#define HEADER_LO               0xAA
#define ADDR_DEFAULT            0x11
#define BAUD_RATE_DEFAULT       0x05 //19200 bps
#define CMD_READ_DIS            0x2
#define CMD_READ_TEMP           0x3
#define CMD_SET_ADDR            0x55
#define CMD_SET_BAUD_RATE       0x8
#define LEN_TX_FRAME            0x7
/*
enum {
    1200bps = 0x00,
    2400bps,
    4800bps,
    9600bps,
    14400bps,
    19200bps,
    28800bps,
    38400bps,
    57600bps,
    115200bps,
    128000bps,
    256000bps
}
*/

Serial serial(USBTX, USBRX); // tx, rx

uint8_t cmd_tx[LEN_TX_FRAME] = {HEADER_HI, HEADER_LO, ADDR_DEFAULT, 0, 0, 0, 0};
uint8_t buffer_rx[8];

void read_distance(){
    unsigned char checksum = 0;
    cmd_tx[3] = 0; // no data
    cmd_tx[4] = CMD_READ_DIS;
    cmd_tx[5] = 0x12;

    for (int i=0; i<6; i++)
        serial.putc(cmd_tx[i]);
    for (int i=0; i<8; i++)
        buffer_rx[i] = serial.getc();

    for (int i=0; i<8; i++)
        printf("rx: %d: %x\n", i, buffer_rx[i]);
}

int main() {
    printf("start to read distance\n");
    serial.baud(19200);
    while(1){
        read_distance();
        wait(1);
    }
}
