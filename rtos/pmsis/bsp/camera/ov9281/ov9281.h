/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __CAM_OV9281_H__
#define __CAM_OV9281_H__


/*
 *  Address of Camera Registers
 */

#define OV9281_SENSOR_NAME "ov9281"
#define OV9281_DEV_ADDR    (0xC0)

#define BIT(nr) (1UL << (nr))

#define OV9281_LINK_FREQ_400MHZ		400000000
/* pixel rate = link frequency * 2 * lanes / BITS_PER_SAMPLE */
#define OV9281_PIXEL_RATE		(OV9281_LINK_FREQ_400MHZ * 2 * 2 / 10)
#define OV9281_XVCLK_FREQ		24000000

#define CHIP_ID				0x9281
#define OV9281_REG_CHIPID_H		0x300a
#define OV9281_REG_CHIPID_L		0x300b

#define OV9281_REG_CTRL_MODE		0x0100
#define OV9281_SW_RESET		0x0103
#define OV9281_MODE_SW_STANDBY		0x0
#define OV9281_MODE_STREAMING		BIT(0)

#define OV9281_REG_EXPOSURE		0x3500
#define	OV9281_EXPOSURE_MIN		4
#define	OV9281_EXPOSURE_STEP		1
#define OV9281_VTS_MAX			0x7fff

#define OV9281_REG_GAIN_H		0x3508
#define OV9281_REG_GAIN_L		0x3509
#define OV9281_GAIN_H_MASK		0x07
#define OV9281_GAIN_H_SHIFT		8
#define OV9281_GAIN_L_MASK		0xff
#define OV9281_GAIN_MIN			0x10
#define OV9281_GAIN_MAX			0xf8
#define OV9281_GAIN_STEP		1
#define OV9281_GAIN_DEFAULT		0x10

#define OV9281_REG_TEST_PATTERN		0x5e00
#define OV9281_TEST_PATTERN_ENABLE	0x80
#define OV9281_TEST_PATTERN_DISABLE	0x0

#define OV9281_REG_VTS			0x380e

#define REG_NULL			0xFFFF

#define OV9281_REG_VALUE_08BIT		1
#define OV9281_REG_VALUE_16BIT		2
#define OV9281_REG_VALUE_24BIT		3

#define OV9281_LANES			1
#define OV9281_BITS_PER_SAMPLE		8


#endif
