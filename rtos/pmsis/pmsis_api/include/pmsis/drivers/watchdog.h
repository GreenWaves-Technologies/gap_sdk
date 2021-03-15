#pragma once

#define PI_WATCHDOG_INLINE0 static inline

#include "pmsis/pmsis_types.h"

/**
* @ingroup groupDrivers
*/


/**
 * @defgroup WATCHDOG WATCHDOG
 *
 * The watchdog driver allows setting a watchdog timer to reset the chip
 * in case it gets stuck
 *
 */


/**
 * @addtogroup WATCHDOG
 * @{
 */

/**@{*/

/** \brief Set watchdog timer
 *
 * This function sets the watchdog timer in term of refclk tick count
 *
 * \param tick_count Number of ticks before reset
 */
PI_WATCHDOG_INLINE0 void pi_watchdog_timer_set(uint32_t tick_count);

/** \brief Start watchdog timer
 *
 * This function start the countdown of the timer
 *
 */
PI_WATCHDOG_INLINE0 void pi_watchdog_start(void);

/** \brief Restart watchdog timer
 *
 * This function restart the countdown of the timer (starting from tick
 * count again)
 *
 */
PI_WATCHDOG_INLINE0 void pi_watchdog_timer_rearm(void);

/** \brief Stop watchdog timer
 *
 * This function stops the countdown of the timer
 *
 */
PI_WATCHDOG_INLINE0 void pi_watchdog_stop(void);

//!@}

/**
 * @}
 */
