/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupChips
 *
 * \addtogroup GAP9
 * \{
 *
 * \defgroup GAP9_Timers GAP9 Timers
 *
 * \brief GAP9 Timers
 *
 * This part enumerates available Timers on chip **GAP9**.
 *
 * There are a total of 4 timers :
 * * 4 on Fabric Controller
 * * 2 on Cluster
 *
 * \addtogroup GAP9_Timers
 * \{
 */

/**
 * \enum pi_timer_e
 *
 * \brief Timers.
 *
 * List of available timers.
 */
typedef enum
{
    SYS_TIMER  = 0, /*!< FC TIMER_0 used as SysTick timer by preemptive RTOS. */
    FC_TIMER_0 = 0, /*!< FC Timer_0_Low. FC_TIMER_0 and FC_TIMER_1 can be used
                         together to form a 64 bits timer. */
    FC_TIMER_1 = 1, /*!< FC Timer_0_High. */
    FC_TIMER_2 = 2, /*!< FC Timer_1_Low. FC_TIMER_2 and FC_TIMER_3 can be used
                         together to form a 64 bits timer. */
    FC_TIMER_3 = 3, /*!< FC Timer_1_High. */
    CL_TIMER_0 = 4, /*!< Cluster Timer_0_Low. */
    CL_TIMER_1 = 5  /*!< Cluster Timer_0_High. */
} pi_timer_e;

/**
 * \} end of GAP9_Timers
 *
 * \} end of GAP9
 */

#ifdef __cplusplus
}
#endif
