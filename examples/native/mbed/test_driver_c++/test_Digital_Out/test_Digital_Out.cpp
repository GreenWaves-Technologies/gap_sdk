/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/digitalout.html
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

DigitalOut myled(LED1);

int main()
{
    // check that myled object is initialized and connected to a pin
    if(myled.is_connected()) {
        printf("myled is initialized and connected!\n\r");
    }

    // Blink LED
    while(1) {
        myled = 1;          // set LED1 pin to high
        printf("\n\r myled = %d", (uint8_t)myled );
        wait(0.01);

        myled.write(0);     // set LED1 pin to low
        printf("\n\r myled = %d",myled.read() );
        wait(0.01);
    }
}
