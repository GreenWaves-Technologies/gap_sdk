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

#ifndef __BSP_CAMERA_HIMAX_HIMAX_H__
#define __BSP_CAMERA_HIMAX_HIMAX_H__

/*
 *  HIMAX camera macros
 */
// Register address
// Read only registers
#define         HIMAX_MODEL_ID_H          0x0000
#define         HIMAX_MODEL_ID_L          0x0001
#define         HIMAX_FRAME_COUNT         0x0005
#define         HIMAX_PIXEL_ORDER         0x0006
// R&W registers
// Sensor mode control
#define         HIMAX_MODE_SELECT         0x0100
#define         HIMAX_IMG_ORIENTATION     0x0101
#define         HIMAX_SW_RESET            0x0103
#define         HIMAX_GRP_PARAM_HOLD      0x0104
// Sensor exposure gain control
#define         HIMAX_INTEGRATION_H       0x0202
#define         HIMAX_INTEGRATION_L       0x0203
#define         HIMAX_ANALOG_GAIN         0x0205
#define         HIMAX_DIGITAL_GAIN_H      0x020E
#define         HIMAX_DIGITAL_GAIN_L      0x020F
// Frame timing control
#define         HIMAX_FRAME_LEN_LINES_H   0x0340
#define         HIMAX_FRAME_LEN_LINES_L   0x0341
#define         HIMAX_LINE_LEN_PCK_H      0x0342
#define         HIMAX_LINE_LEN_PCK_L      0x0343
// Binning mode control
#define         HIMAX_READOUT_X           0x0383
#define         HIMAX_READOUT_Y           0x0387
#define         HIMAX_BINNING_MODE        0x0390
// Test pattern control
#define         HIMAX_TEST_PATTERN_MODE   0x0601
// Black level control
#define         HIMAX_BLC_CFG             0x1000
#define         HIMAX_BLC_TGT             0x1003
#define         HIMAX_BLI_EN              0x1006
#define         HIMAX_BLC2_TGT            0x1007
//  Sensor reserved
#define         HIMAX_DPC_CTRL            0x1008
#define         HIMAX_SINGLE_THR_HOT      0x100B
#define         HIMAX_SINGLE_THR_COLD     0x100C
// VSYNC,HSYNC and pixel shift register
#define         HIMAX_VSYNC_HSYNC_PIXEL_SHIFT_EN  0x1012
// Statistic control and read only
#define         HIMAX_STATISTIC_CTRL      0x2000
#define         HIMAX_MD_LROI_X_START_H   0x2011
#define         HIMAX_MD_LROI_X_START_L   0x2012
#define         HIMAX_MD_LROI_Y_START_H   0x2013
#define         HIMAX_MD_LROI_Y_START_L   0x2014
#define         HIMAX_MD_LROI_X_END_H     0x2015
#define         HIMAX_MD_LROI_X_END_L     0x2016
#define         HIMAX_MD_LROI_Y_END_H     0x2017
#define         HIMAX_MD_LROI_Y_END_L     0x2018
// Automatic exposure gain control
#define         HIMAX_AE_CTRL             0x2100
#define         HIMAX_AE_TARGET_MEAN      0x2101
#define         HIMAX_AE_MIN_MEAN         0x2102
#define         HIMAX_CONVERGE_IN_TH      0x2103
#define         HIMAX_CONVERGE_OUT_TH     0x2104
#define         HIMAX_MAX_INTG_H          0x2105
#define         HIMAX_MAX_INTG_L          0x2106
#define         HIMAX_MIN_INTG            0x2107
#define         HIMAX_MAX_AGAIN_FULL      0x2108
#define         HIMAX_MAX_AGAIN_BIN2      0x2109
#define         HIMAX_MIN_AGAIN           0x210A
#define         HIMAX_MAX_DGAIN           0x210B
#define         HIMAX_MIN_DGAIN           0x210C
#define         HIMAX_DAMPING_FACTOR      0x210D
#define         HIMAX_FS_CTRL             0x210E
#define         HIMAX_FS_60HZ_H           0x210F
#define         HIMAX_FS_60HZ_L           0x2110
#define         HIMAX_FS_50HZ_H           0x2111
#define         HIMAX_FS_50HZ_L           0x2112
#define         HIMAX_FS_HYST_TH          0x2113
// Motion detection control
#define         HIMAX_MD_CTRL             0x2150
#define         HIMAX_I2C_CLEAR           0x2153
#define         HIMAX_WMEAN_DIFF_TH_H     0x2155
#define         HIMAX_WMEAN_DIFF_TH_M     0x2156
#define         HIMAX_WMEAN_DIFF_TH_L     0x2157
#define         HIMAX_MD_THH              0x2158
#define         HIMAX_MD_THM1             0x2159
#define         HIMAX_MD_THM2             0x215A
#define         HIMAX_MD_THL              0x215B
//  Sensor timing control
#define         HIMAX_QVGA_WIN_EN         0x3010
#define         HIMAX_SIX_BIT_MODE_EN     0x3011
#define         HIMAX_PMU_AUTOSLEEP_FRAMECNT  0x3020
#define         HIMAX_ADVANCE_VSYNC       0x3022
#define         HIMAX_ADVANCE_HSYNC       0x3023
#define         HIMAX_EARLY_GAIN          0x3035
//  IO and clock control
#define         HIMAX_BIT_CONTROL         0x3059
#define         HIMAX_OSC_CLK_DIV         0x3060
#define         HIMAX_ANA_Register_11     0x3061
#define         HIMAX_IO_DRIVE_STR        0x3062
#define         HIMAX_IO_DRIVE_STR2       0x3063
#define         HIMAX_ANA_Register_14     0x3064
#define         HIMAX_OUTPUT_PIN_STATUS_CONTROL   0x3065
#define         HIMAX_ANA_Register_17     0x3067
#define         HIMAX_PCLK_POLARITY       0x3068

/*
 * Useful value of Himax registers
 */
#define         HIMAX_RESET         0x01
#define         HIMAX_Pclk_rising_edge    0x00
#define         HIMAX_Pclk_falling_edge   0x01


enum {
  HIMAX_STANDBY = 0x0,
  HIMAX_STREAMING = 0x1,        // I2C triggered streaming enable
  HIMAX_STREAMING2 = 0x3,       // Output N frames
  HIMAX_STREAMING3 = 0x5        // Hardware Trigger
};


#endif
