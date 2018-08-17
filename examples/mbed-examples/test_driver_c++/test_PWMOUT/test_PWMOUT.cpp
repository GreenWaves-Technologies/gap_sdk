/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://os.mbed.com/docs/latest/reference/pwmout.html
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

PwmOut led(PWM0_CH0);

int main() {
    // specify period first, then everything else
    led.period(1.0f);  // 1 second period
    led.write(0.50f);  // 50% duty cycle
    while(1);          // led flashing
}
