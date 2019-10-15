/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
#ifndef __RT__RT_PAD_H__
#define __RT__RT_PAD_H__


/**
* @ingroup groupDrivers
*/



/**
 * @defgroup PADFRAME Padframe
 *
 * The padframe driver provides support for controlling PADs.
 *
 */

/**
 * @addtogroup PADFRAME
 * @{
 */

/**@{*/


/** \brief Set the pad function
 *
 * This function can be used to configure the function of the specified pad
 * in case it can have several functions.
 *
 * \param pad  Pad number. See the chip specific configuration for more details.
 * \param function Pad function. See the chip specific configuration for more details.
 */
void rt_pad_set_function(rt_pad_e pad, rt_pad_func_e function);



//!@}



/**
 * @} end of GPIO PADFRAME
 */


/// @cond IMPLEM

#if 0
/** \enum rt_pad_cmd_e
 * \brief Padframe commands.
 *
 * This enum defines the commands which which can be used to control the padframe.
 */
typedef enum {
  RT_PAD_CMD_FUNC = 0,       /*!< Command for configuring the Resolution. */
} rt_pad_cmd_e;

void rt_pad_control(rt_pad_cmd_e cmd, void *arg);
#endif


#if PULP_CHIP != CHIP_VEGA && PULP_CHIP != CHIP_GAP9

/** \enum rt_pad_sleep_state_e
 * \brief Pad state when the chip is sleeping.
 *
 * Describes the available pad state when the chip is sleeping.
 */
typedef enum {
  RT_PAD_SLEEP_STATE_DEFAULT     = 0, /*!< Default state. */
} rt_pad_sleep_state_e;

#endif



void __rt_padframe_init();
rt_padframe_profile_t *rt_pad_profile_get(char *profile_string) ;
void rt_padframe_set(rt_padframe_profile_t *profile) ;

void rt_pad_sleep_state(rt_pad_e pad, rt_pad_sleep_state_e state);

/// @endcond


#endif
