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
 * \addtogroup GAP8
 * \{
 *
 * \defgroup GAP8_Timers GAP8 Timers
 *
 * \brief GAP8 Timers
 *
 * This part enumerates available Timers on chip **GAP8**.
 *
 * There are a total of 4 timers :
 * * 2 on Fabric Controller
 * * 2 on Cluster
 *
 * \addtogroup GAP8_Timers
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
    SYS_TIMER  = 0, /*!< FC_TIMER_0 used as SysTick timer by preemptive RTOS. */
    FC_TIMER_0 = 0, /*!< FC Timer_Low. */
    FC_TIMER_1 = 1, /*!< FC Timer_High, can be used as perf counter. */
    CL_TIMER_0 = 2, /*!< Cluster Timer_Low. */
    CL_TIMER_1 = 3  /*!< Cluster Timer_High. */
} pi_timer_e;

/**
 * \} end of GAP8_Timers
 *
 * \} end of GAP8
 */

#ifdef __cplusplus
}
#endif
