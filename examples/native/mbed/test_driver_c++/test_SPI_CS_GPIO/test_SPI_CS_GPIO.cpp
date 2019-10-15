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

SPI spi(SPI1_MOSI, SPI1_MISO, SPI1_SCLK); // mosi, miso, sclk
DigitalOut cs(SPI1_CSN0_A5);

int main() {
    // Chip must be deselected
    cs = 1;

    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);

    // Select the device by seting chip select low
    cs = 0;

    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0x8F);

    // Send a dummy byte to receive the contents of the WHOAMI register
    int whoami = spi.write(0x00);
    printf("WHOAMI register = 0x%X\n", whoami);

    // Deselect the device
    cs = 1;

    return 0;
}
