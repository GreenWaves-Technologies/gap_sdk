/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/portinout.html
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
// Toggle all four LEDs

#include "mbed.h"

// LED1 = PA.18  LED2 = PA.20  LED3 = PA.21  LED4 = PA.23
#define LED_MASK 0x00B40000

PortInOut ledport(PortA, LED_MASK);

int main() {
    int v = ledport;
    ledport.output();
    while(1) {
        ledport = LED_MASK;
        printf("LEDs turn on\n");
        wait(0.01);
        ledport = 0;
        printf("LEDs turn off\n");
        wait(0.01);
    }
}
