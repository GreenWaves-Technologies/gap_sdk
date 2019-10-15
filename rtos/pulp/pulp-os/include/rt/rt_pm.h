/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __RT_RT_PM_H__
#define __RT_RT_PM_H__

#include "rt/rt_data.h"


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup PM Power management
 * 
 */



/**        
 * @addtogroup PM
 * @{        
 */

/**@{*/

#if PULP_CHIP != CHIP_VEGA && PULP_CHIP != CHIP_GAP9

/** \enum rt_pm_domain_e
 * \brief Power domains.
 *
 * Describes the available power domains (other than the chip itself).
 */
typedef enum {
  RT_PM_DOMAIN_MAIN=0   /*!< Main power domain. */
} rt_pm_domain_e;

#endif



/** \enum rt_pm_domain_state_e
 * \brief Domain power states.
 *
 * Describes the power state of a domain.
 */
typedef enum {
  RT_PM_DOMAIN_STATE_OFF=0,   /*!< OFF. The domain is powered-down and is not accessible */
  RT_PM_DOMAIN_STATE_ON=1     /*!< ON. The domain is powered-up and is accessible */
} rt_pm_domain_state_e;



#if PULP_CHIP != CHIP_VEGA && PULP_CHIP != CHIP_GAP9

/** \enum rt_pm_state_e
 * \brief Power states.
 *
 * Describes the power state of the chip.
 */
typedef enum {
  RT_PM_STATE_SLEEP       = 0,     /*!< Sleep state. Everything is shut down, except few memories which can still be in retentive mode to keep some data. */
  RT_PM_STATE_DEEP_SLEEP  = 1,     /*!< Deep sleep state. Everything is shut down, inluding memories, and all data is lost. */
} rt_pm_state_e;

#endif


/** \enum rt_pm_state_flags_e
 * \brief Power states flags.
 *
 * Describes the way to change power states.
 */
typedef enum {
  RT_PM_STATE_FAST       = 1,     /*!< Fast mode. The runtime does the minimum to change the power state. When going to sleep mode, only retentive data are kept and the execution will start again from the main after wake-up. The same when going to deep mode but no data is kept. In both cases, every resource must be reallocated or reopened (e.g. memory allocation, cluster or camera). */
} rt_pm_state_flags_e;



/** \enum rt_pm_wakeup_e
 * \brief Power wakeup state.
 *
 * Describes the power state before the wakeup.
 */
typedef enum {
  RT_PM_WAKEUP_COLD       = 0,     /*!< Cold boot. The chip was off before waking up. */
  RT_PM_WAKEUP_DEEPSLEEP  = 1,     /*!< Deep sleep boot. The chip was in deep sleep mode before waking up. */
  RT_PM_WAKEUP_SLEEP      = 2,     /*!< Sleep boot. The chip was insleep mode before waking up. */
} rt_pm_wakeup_e;



/** \brief Clear all wakeup signals
 *
 * This clears all the pending wakeup signals so that the chip can
 * go to deep sleep mode. Any wakeup source (like RTC or GPIO) occuring after
 * this call will be buffered again and will prevent the chip from going
 * to deep sleep.
 *
 */
void rt_pm_wakeup_clear_all();



/** \enum rt_pm_wakeup_gpio_mode_e
 * \brief GPIO wakeup mode.
 *
 * Describes the way a GPIO can wakeup the chip.
 */
typedef enum {
  RT_PM_WAKEUP_GPIO_RISING_EDGE   = 0,  /*!< Rising edge. The GPIO can wakeup the chip when switching from 0 to 1. */
  RT_PM_WAKEUP_GPIO_FALLING_EDGE  = 1,  /*!< Falling edge. The GPIO can wakeup the chip when switching from 1 to 0. */
  RT_PM_WAKEUP_GPIO_HIGH          = 2,  /*!< Level high. The GPIO can wakeup the chip when it is 1. */
  RT_PM_WAKEUP_GPIO_LOW           = 3,  /*!< Level low. The GPIO can wakeup the chip when it is 0. */
} rt_pm_wakeup_gpio_mode_e;



/** \brief Configure GPIO wakeup
 *
 * Enable or disable chip wakeup from deep sleep from a GPIO.
 * 
 * \param   active If 1, enable chip wakeup from GPIO, if 0, disable it.
 * \param   gpio   Number of the GPIO which can wakeup the chip.
 * \param   mode   Wakeup mode.
 */
void rt_pm_wakeup_gpio_conf(int active, int gpio, rt_pm_wakeup_gpio_mode_e mode);



/** \brief Switch power state.
 *
 * Switch the chip to the specified power state. 
 * 
 * \param     state New power state.
 * \param     flags Flags to control hoe to switch the power state.
 * \return          0 if the operation is successful, -1 otherwise.
 */
int rt_pm_state_switch(rt_pm_state_e state, rt_pm_state_flags_e flags);



/** \brief Switch domain power state.
 *
 * Switch the specified domain to the specified power state. This cannot
 * be applied to the chip.
 * 
 * \param     domain The power domain on which the power state must be changed.
 * \param     state New power state.
 * \return          0 if the operation is successful, -1 otherwise.
 */
int rt_pm_domain_state_switch(rt_pm_domain_e domain, rt_pm_domain_state_e state, rt_event_t *event);



/** \brief Get power state before wake-up.
 *
 * This returns the power state of the chip before the last wake-up. 
 * 
 * \return          The power state.
 */
rt_pm_wakeup_e rt_pm_wakeup_state();


//!@}

/**        
 * @}
 */


/// @cond IMPLEM

/// @endcond

#endif
