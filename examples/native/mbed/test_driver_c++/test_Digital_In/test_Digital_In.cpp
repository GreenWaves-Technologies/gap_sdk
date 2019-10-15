/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/digitalin.html
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

DigitalIn  mypin(GPIO_A31); // change this to the button on your board
DigitalOut myled(GPIO_A30);

int main()
{
    // check mypin object is initialized and connected to a pin
    if(mypin.is_connected()) {
        printf("mypin is connected and initialized! \n\r");
    }

    // Optional: set mode as PullUp/PullNone
    mypin.mode(PullNone);

    // press the button and see the console / led change
    while(1) {
        printf("mypin has value : %d \n\r", mypin.read());
        myled = mypin; // toggle led based on value of button
        wait(0.02);
    }
}
