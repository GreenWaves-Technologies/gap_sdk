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
#include "FP_Lib.h"

// GAPUINO PIN 10, 11, 12, 13
SPI spi(SPI1_MOSI, SPI1_MISO, SPI1_SCLK); // mosi, miso, sclk
DigitalOut cs(SPI1_CSN0_A5);

float read_adc(uint8_t adc_ch, float vref) {
    char reply[2];

    // Construct SPI message
    //  First bit (Start): Logic high (1)
    //  Second bit (SGL/DIFF): 1 to select single mode
    //  Third bit (ODD/SIGN): Select channel (0 or 1)
    //  Fourth bit (MSFB): 0 for LSB first
    //  Next 12 bits: 0 (don't care)
    uint8_t msg = 0x3;
    msg = (msg << 1 | adc_ch) << 5;

    cs = 0;

    // SPI duplex
    reply[0] = spi.write(msg);
    reply[1] = spi.write(0x00);

    cs = 1;

    printf("replt 0: %x \n", reply[0]);
    printf("replt 1: %x \n", reply[1]);

    // Construct single integer out of the reply (2 bytes)
    int adc = (reply[0] << 8) | reply[1];

    // Last bit (0) is not part of ADC value, shift to remove it
    adc = adc >> 1;

    // Calculate voltage form ADC value
    float voltage = (vref * (float)adc) / (float)1024;

    return voltage;
}

int main() {
    cs = 1;

    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1200000);

    while(1) {
        float adc_0 = read_adc(0, 3.3);
        float adc_1 = read_adc(1, 3.3);

        printf("Ch 0: \n");
        printf_fp(adc_0);

        printf("Ch 1: \n");
        printf_fp(adc_1);

        wait(0.2);
    }

    return 0;
}
