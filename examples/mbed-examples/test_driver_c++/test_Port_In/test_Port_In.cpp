/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/portin.html
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
// Switch on an LED if any of mbed pins 0-5 is high

#include "mbed.h"

PortIn     p(PortA, 0x0000003F);   // p0-p5
DigitalOut ind(GPIO_A31);

int main() {
    while(1) {
        int pins = p.read();
        if(pins) {
            ind = 1;
            printf("LED: 1\n");
        } else {
            ind = 0;
            printf("LED: 0\n");
        }
    }
}
