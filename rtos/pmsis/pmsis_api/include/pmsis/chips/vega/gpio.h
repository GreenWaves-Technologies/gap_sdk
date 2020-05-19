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

#ifndef __PMSIS_CHIPS_GAP9_V1_GPIO_H__
#define __PMSIS_CHIPS_GAP9_V1_GPIO_H__

#include "pmsis/chips/vega/pad.h"

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup GAP9_V1
 * @{
 */


/**
 * \defgroup GAP9_V1_Padframe GAP9_V1 Padframe
 *
 * This part enumerates available GPIOs name on GAP9_V1 chip.
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
 * \addtogroup GAP9_V1_Padframe
 * @{
 */

/**
 * \enum pi_pad_e
 * \brief Pad numbers.
 *
 * This is used to identify pads.
 *
 *        |    31   |7       4|3        0|
 * GPIO : | IS_GPIO | PAD_NUM | GPIO_NUM |
 */
typedef enum
{
    PI_GPIO_NONE,
} pi_gpio_e;

/**
 * @}
 */

#endif  /* __PMSIS_CHIPS_GPIO_PAD_H__ */
