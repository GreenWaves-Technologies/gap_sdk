/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __PMSIS_CHIPS_GAP9_GPIO_H__
#define __PMSIS_CHIPS_GAP9_GPIO_H__

#include "pmsis/chips/gap9/pad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup GAP9
 * @{
 */


/**
 * \defgroup GAP9_Padframe GAP9 Padframe
 *
 * This part enumerates available GPIOs name on GAP9 chip.
 * There are a total of 96 GPIOs.
 *
 * @}
 */

#define PI_GPIO_NUM_SHIFT     0
#define PI_GPIO_NUM_MASK      0xFF
#define PI_GPIO_PAD_SHIFT     8
#define PI_GPIO_PAD_MASK      0xFF00
#define PI_GPIO_IS_GPIO_SHIFT 31
#define PI_GPIO_IS_GPIO_MASK  0x80000000

/**
 * \addtogroup GAP9_Padframe
 * @{
 */

/**
 * \enum pi_pad_e
 * \brief Pad numbers.
 *
 * List of available GPIO pins.
 * This is used to identify pads.
 *
 * GPIO :
 *
 * |    31   |         |15      8|7        0|
 * |---------|---------|---------|----------|
 * | IS_GPIO |         | PAD_NUM | GPIO_NUM |
 */
typedef enum
{
    PI_GPIO_A00 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_000 << PI_GPIO_PAD_SHIFT) | 0),
    PI_GPIO_A01 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_001 << PI_GPIO_PAD_SHIFT) | 1),
    PI_GPIO_A02 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_002 << PI_GPIO_PAD_SHIFT) | 2),
    PI_GPIO_A03 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_003 << PI_GPIO_PAD_SHIFT) | 3),
    PI_GPIO_A04 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_004 << PI_GPIO_PAD_SHIFT) | 4),
    PI_GPIO_A05 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_005 << PI_GPIO_PAD_SHIFT) | 5),
    PI_GPIO_A06 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_006 << PI_GPIO_PAD_SHIFT) | 6),
    PI_GPIO_A07 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_007 << PI_GPIO_PAD_SHIFT) | 7),
    PI_GPIO_A08 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_008 << PI_GPIO_PAD_SHIFT) | 8),
    PI_GPIO_A09 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_009 << PI_GPIO_PAD_SHIFT) | 9),
    PI_GPIO_A10 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_010 << PI_GPIO_PAD_SHIFT) | 10),
    PI_GPIO_A11 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_011 << PI_GPIO_PAD_SHIFT) | 11),
    PI_GPIO_A12 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_012 << PI_GPIO_PAD_SHIFT) | 12),
    PI_GPIO_A13 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_013 << PI_GPIO_PAD_SHIFT) | 13),
    PI_GPIO_A14 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_014 << PI_GPIO_PAD_SHIFT) | 14),
    PI_GPIO_A15 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_015 << PI_GPIO_PAD_SHIFT) | 15),
    PI_GPIO_A16 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_016 << PI_GPIO_PAD_SHIFT) | 16),
    PI_GPIO_A17 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_017 << PI_GPIO_PAD_SHIFT) | 17),
    PI_GPIO_A18 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_018 << PI_GPIO_PAD_SHIFT) | 18),
    PI_GPIO_A19 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_019 << PI_GPIO_PAD_SHIFT) | 19),
    PI_GPIO_A20 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_020 << PI_GPIO_PAD_SHIFT) | 20),
    PI_GPIO_A21 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_021 << PI_GPIO_PAD_SHIFT) | 21),
    PI_GPIO_A22 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_022 << PI_GPIO_PAD_SHIFT) | 22),
    PI_GPIO_A23 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_023 << PI_GPIO_PAD_SHIFT) | 23),
    PI_GPIO_A24 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_024 << PI_GPIO_PAD_SHIFT) | 24),
    PI_GPIO_A25 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_025 << PI_GPIO_PAD_SHIFT) | 25),
    PI_GPIO_A26 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_026 << PI_GPIO_PAD_SHIFT) | 26),
    PI_GPIO_A27 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_027 << PI_GPIO_PAD_SHIFT) | 27),
    PI_GPIO_A28 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_028 << PI_GPIO_PAD_SHIFT) | 28),
    PI_GPIO_A29 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_029 << PI_GPIO_PAD_SHIFT) | 29),
    PI_GPIO_A30 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_030 << PI_GPIO_PAD_SHIFT) | 30),
    PI_GPIO_A31 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_031 << PI_GPIO_PAD_SHIFT) | 31),
    PI_GPIO_A32 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_032 << PI_GPIO_PAD_SHIFT) | 32),
    PI_GPIO_A33 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_033 << PI_GPIO_PAD_SHIFT) | 33),
    PI_GPIO_A34 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_034 << PI_GPIO_PAD_SHIFT) | 34),
    PI_GPIO_A35 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_035 << PI_GPIO_PAD_SHIFT) | 35),
    PI_GPIO_A36 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_036 << PI_GPIO_PAD_SHIFT) | 36),
    PI_GPIO_A37 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_037 << PI_GPIO_PAD_SHIFT) | 37),
    PI_GPIO_A38 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_038 << PI_GPIO_PAD_SHIFT) | 38),
    PI_GPIO_A39 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_039 << PI_GPIO_PAD_SHIFT) | 39),
    PI_GPIO_A40 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_040 << PI_GPIO_PAD_SHIFT) | 40),
    PI_GPIO_A41 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_041 << PI_GPIO_PAD_SHIFT) | 41),
    PI_GPIO_A42 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_042 << PI_GPIO_PAD_SHIFT) | 42),
    PI_GPIO_A43 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_043 << PI_GPIO_PAD_SHIFT) | 43),
    PI_GPIO_A44 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_044 << PI_GPIO_PAD_SHIFT) | 44),
    PI_GPIO_A45 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_045 << PI_GPIO_PAD_SHIFT) | 45),
    PI_GPIO_A46 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_046 << PI_GPIO_PAD_SHIFT) | 46),
    PI_GPIO_A47 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_047 << PI_GPIO_PAD_SHIFT) | 47),
    PI_GPIO_A48 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_048 << PI_GPIO_PAD_SHIFT) | 48),
    PI_GPIO_A49 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_049 << PI_GPIO_PAD_SHIFT) | 49),
    PI_GPIO_A50 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_050 << PI_GPIO_PAD_SHIFT) | 50),
    PI_GPIO_A51 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_051 << PI_GPIO_PAD_SHIFT) | 51),
    PI_GPIO_A52 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_052 << PI_GPIO_PAD_SHIFT) | 52),
    PI_GPIO_A53 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_053 << PI_GPIO_PAD_SHIFT) | 53),
    PI_GPIO_A54 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_054 << PI_GPIO_PAD_SHIFT) | 54),
    PI_GPIO_A55 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_055 << PI_GPIO_PAD_SHIFT) | 55),
    PI_GPIO_A56 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_056 << PI_GPIO_PAD_SHIFT) | 56),
    PI_GPIO_A57 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_057 << PI_GPIO_PAD_SHIFT) | 57),
    PI_GPIO_A58 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_058 << PI_GPIO_PAD_SHIFT) | 58),
    PI_GPIO_A59 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_059 << PI_GPIO_PAD_SHIFT) | 59),
    PI_GPIO_A60 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_060 << PI_GPIO_PAD_SHIFT) | 60),
    PI_GPIO_A61 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_061 << PI_GPIO_PAD_SHIFT) | 61),
    PI_GPIO_A62 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_062 << PI_GPIO_PAD_SHIFT) | 62),
    PI_GPIO_A63 = ((1 << PI_GPIO_IS_GPIO_SHIFT) | (PI_PAD_063 << PI_GPIO_PAD_SHIFT) | 63)
} pi_gpio_e;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PMSIS_CHIPS_GPIO_PAD_H__ */
