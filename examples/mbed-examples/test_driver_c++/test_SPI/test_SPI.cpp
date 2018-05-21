/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://docs.mbed.com/docs/mbed-os-api-reference/en/latest/APIs/interfaces/digital/SPI/
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

SPI spi(SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN0_A9); // mosi, miso, sclk, ssel

// Chip select is contolled by inner ip
// DigitalOut cs(SPI1_CSN1_B2);

int main() {
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);
    int whoami = 0;
    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0x9F);

    // Send a dummy byte to receive the contents of the WHOAMI register
    // Here we use explicite transfer, so always return 0.
    // If user wants to read something, please use spi_master_read
    whoami = spi.write(0x00);
    printf("WHOAMI register = 0x%X\n", whoami);




    // In fact, the two write can be replaced by read.
    // It will send command 0x8F followed by a dummy cycle and read the response.
    whoami = spi.read(0x9F);
    printf("WHOAMI register = 0x%X\n", whoami);

    return 0;
}
