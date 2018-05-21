/**
 * @file
 * Mbed OS Standard Test
 * Reference : https://docs.mbed.com/docs/mbed-os-api-reference/en/latest/APIs/interfaces/digital/I2C/
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
// Read temperature from LM75BD

I2C i2c(I2C0_SDA, I2C0_SCL);

const int addr = 0xA0;
char cmd[3];
char result;

int main() {
    printf("Test start\n");
    i2c.frequency(200000);
    cmd[0] = 0x00;
    cmd[1] = 0x00;
    cmd[2] = 0x5A;
    i2c.write(addr, cmd, 3);

    wait(0.5);

    cmd[0] = 0x00;
    cmd[1] = 0x00;
    i2c.write(addr, cmd, 2, 1);
    i2c.read(addr, &result, 1);

    printf("read %x\n", result);
    if (result == 0x5A) return 0;
    else return -1;
}


