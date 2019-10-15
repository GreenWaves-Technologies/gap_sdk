/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/digitalinout.html
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

DigitalInOut mypin(GPIO_A31);

int main()
{
    // check that mypin object is initialized and connected to a pin
    if(mypin.is_connected()) {
        printf("mypin is initialized and connected!\n\r");
    }

    // Optional: set mode as PullUp/PullDown/PullNone/OpenDrain
    mypin.mode(PullNone);

    while(1) {
        // write to pin as output
        mypin.output();
        mypin = !mypin; // toggle output
        printf("mypin.read() = %d \n\r",mypin.read());
        wait(0.01);

        // read from pin as input
        mypin.input();
        // read X is normal here
        printf("mypin.read() = %d \n\r",mypin.read());
        wait(0.01);
    }
}
