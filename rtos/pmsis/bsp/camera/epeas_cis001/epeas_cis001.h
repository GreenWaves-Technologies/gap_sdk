/*
 * Copyright (C) 2022 GreenWaves Technologies
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


#ifndef __BSP_CAMERA_EPEAS_CIS001_EPEAS_CIS001_H__
#define __BSP_CAMERA_EPEAS_CIS001_EPEAS_CIS001_H__

#define EPEAS_CIS001_PWM_FREQ 12000000
#define EPEAS_CIS001_PWM_DUTYC      50

/*
 *  EPEAS_CIS001 camera macros
 */

// Register address
// Read only registers
#define 		EPEAS_CIS001_STATUS               0x08
#define 		EPEAS_CIS001_IRQ_FLAG             0x0C
#define 		EPEAS_CIS001_CAL_CLK_BUFF_B0      0x32
#define 		EPEAS_CIS001_CAL_CLK_BUFF_B1      0x33
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B0    0x32
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B1    0x33
#define 		EPEAS_CIS001_CAL_RAMP_COUNT_B2    0x36
#define 		EPEAS_CIS001_VERSION              0x7F
// Write only registers
#define 		EPEAS_CIS001_CAPTURE              0x00
#define 		EPEAS_CIS001_SOFT_RESET           0x09
#define 		EPEAS_CIS001_IRQCLR               0x0D
#define         EPEAS_CIS001_AUTOEXPOSURE         0x1F
#define         EPEAS_CIS001_AUTOEXPTRGT_B0       0x20
// R&W registers
// Sensor mode control
#define 		EPEAS_CIS001_MODE                 0x01
#define 		EPEAS_CIS001_PMODE                0x04
#define 		EPEAS_CIS001_DCMI                 0x05
#define 		EPEAS_CIS001_FLIP                 0x17
#define 		EPEAS_CIS001_RESOLUTION           0x13
// Sensor exposure and gain control
#define         EPEAS_CIS001_ANALOG_GAIN          0x2D
#define         EPEAS_CIS001_EXPOSURE_B0          0x06
#define         EPEAS_CIS001_EXPOSURE_B1          0x07

//Calibration
#define         EPEAS_CIS001_CCLK_CAL             0x2E
#define         EPEAS_CIS001_ABB_EN               0x37


#endif