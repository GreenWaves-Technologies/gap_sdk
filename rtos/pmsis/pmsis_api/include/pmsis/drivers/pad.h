/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __PMSIS_DRIVERS_PAD_H__
#define __PMSIS_DRIVERS_PAD_H__

#include <stdint.h>

/**
 * \ingroup groupDrivers
 *
 * \defgroup Padframe Padframe
 *
 * \brief Padframe
 *
 * The padframe driver provides support for controlling PADs.
 *
 * \addtogroup Padframe
 * \{
 */

/**
 * \enum pi_pad_func_e
 *
 * \brief Pad functions.
 *
 * This is used to identify the function for each pad.
 */
typedef enum
{
    PI_PAD_FUNC0                     = 0, /*!< Alternate func0, default func.*/
    PI_PAD_FUNC1                     = 1, /*!< Alternate func1. */
    PI_PAD_FUNC2                     = 2, /*!< Alternate func2, pads as GPIOs. */
    PI_PAD_FUNC3                     = 3  /*!< Alternate func3. */
} pi_pad_func_e;


/**
 * \cond IMPLEM
 */
#define PI_PAD_PULL_OFFSET           ( 0 )
#define PI_PAD_DRIVE_OFFSET          ( 1 )
#define PI_PAD_SLEEPCFG_DIR_OFFSET   ( 0 )
#define PI_PAD_SLEEPCFG_STATE_OFFSET ( 1 )
/**
 * \endcond
 */


/**
 * \enum pi_pad_flags_e
 * \brief Pad configuration flags.
 *
 * Flags are used to configure pad : drive strength, pull activation.
 */
typedef enum
{
    PI_PAD_PULL_DISABLE = (0 << PI_PAD_PULL_OFFSET),  /*!< Disable pull. */
    PI_PAD_PULL_ENABLE  = (1 << PI_PAD_PULL_OFFSET),  /*!< Enable pull. */
    PI_PAD_DS_LOW       = (0 << PI_PAD_DRIVE_OFFSET), /*!< Low drive strength. */
    PI_PAD_DS_HIGH      = (1 << PI_PAD_DRIVE_OFFSET)  /*!< High drive strength. */
} pi_pad_flags_e;

/**
 * \enum pi_pad_sleepcfg_flags_e
 *
 * \brief Pad sleep configuration flags.
 *
 * Flags are used to configure pad : direction, active state.
 */
typedef enum
{
    PI_PAD_SLEEPCFG_INPUT       = (0 << PI_PAD_SLEEPCFG_DIR_OFFSET),   /*!< Pad is an input. */
    PI_PAD_SLEEPCFG_OUTPUT      = (1 << PI_PAD_SLEEPCFG_DIR_OFFSET),   /*!< Pad is an output. */
    PI_PAD_SLEEPCFG_ACTIVE_LOW  = (0 << PI_PAD_SLEEPCFG_STATE_OFFSET), /*!< Pad is active low. */
    PI_PAD_SLEEPCFG_ACTIVE_HIGH = (1 << PI_PAD_SLEEPCFG_STATE_OFFSET)  /*!< Pad is active high. */
} pi_pad_sleepcfg_flags_e;

/**
 * \brief Set the function of one pad.
 *
 * This function can be used to configure the function of the specified pad
 * in the case that it supports several functions.
 *
 * \param pad  Pad number. See the chip specific configuration for more details.
 * \param function Pad function. See the chip specific configuration for more
 *   details.
 */
void pi_pad_set_function(pi_pad_e pad, pi_pad_func_e function);

/**
 * \brief Set the function of all pads.
 *
 * This function can be used to configure the function of all the pads
 * in the case that they support several functions.
 *
 * \param pad_values Pad values. This is an array of 32 bit values, with one bit
 *   per pad and one 32 bit value per group of 32 pads.
 */
void pi_pad_init(uint32_t pad_values[]);

/**
 * \brief Set the configuration for a pin.
 *
 * This function configures the pull activation and drive strength of a given pin.
 *
 * \param pad            Pad to configure.
 * \param cfg            Pad configuration, set of flags.
 */
void pi_pad_set_configuration(pi_pad_e pad, pi_pad_flags_e cfg);

/**
 * \brief Set the sleep configuration for a set of pins.
 *
 * This function configures pads behaviour when microcontroller goes into sleep
 * or deep sleep mode.
 * The array sent to this function should be filled for available pads(cf \ref pi_pad_e),
 * with 2 bits per pad(active state and direction).
 *
 * Ex : Pad 10 - PI_PAD_18_A43_CAM_PCLK
 *
 * uint32_t array[3] = {0};
 *
 * array[PI_PAD_18_A43_CAM_PCLK / 16] |= ((PI_PAD_SLEEPCFG_INPUT | PI_PAD_SLEEPCFG_ACTIVE_HIGH)
 *                                         << PI_PAD_18_A43_CAM_PCLK);
 *
 * \param sleepcfg       Pads sleep configuration.
 * \param sleep_ena      Enable/disable pad sleep mode.
 */
void pi_pad_sleepcfg_set(uint32_t sleepcfg[], uint8_t sleep_ena);


/**
 * \} end of Padframe
 */

#endif  /* __PMSIS_DRIVERS_PAD_H__ */
