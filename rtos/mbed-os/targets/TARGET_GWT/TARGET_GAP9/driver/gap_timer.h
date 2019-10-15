/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
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
 Modifications copyright (C) 2018 GreenWaves Technologies

 - Change all reload value to compare value
 - Change file name apb_timer.h to gap_timer.h
 - Add Timer_ReadCycle function

 */

#ifndef _GAP_TIMER_H
#define _GAP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Supported Number of Timers */
#define NUM_TIMERS  4
#define TIMER0  0
#define TIMER1  1
#define TIMER0_CLUSTER  2
#define TIMER1_CLUSTER  3

/* Default compare */
#define TIMER_DEFAULT_COMPARE    0xFFFFFFFF

/*
 * Timer_Initialize(): Initializes an hardware timer
 * timer: timer to be Initialized
 * time_us: timer compare value in us - 0 to compare to timer max value
 *          time_us = tick_value / TIMER_TICK_US
 */
void Timer_Initialize(uint32_t timer, uint32_t time_us);

/*
 * Timer_Enable(): Enables an hardware timer
 * timer: timer to be enabled
 */
void Timer_Enable(uint32_t timer);

/*
 * Timer_Disable(): Disables an hardware timer
 * timer: timer to be disabled
 */
void Timer_Disable(uint32_t timer);

/*
 * Timer_isEnabled(): verifies if a timer is enabled
 * timer: timer to be verified
 * @return: 0 disabled - 1 enabled
 */
uint32_t Timer_isEnabled(uint32_t timer);

/*
 * Timer_ReadCycle(): provides timer cycle VALUE
 * timer: timer to be read
 * @return: timer cycle VALUE
 */
uint32_t Timer_ReadCycle(uint32_t timer);

/*
 * Timer_Read(): provides timer VALUE
 * timer: timer to be read
 * @return: timer VALUE
 */
uint32_t Timer_Read(uint32_t timer);

/*
 * Timer_SetInterrupt(): sets timer Interrupt
 * timer: timer on which interrupt is set
 * time_us: compareing time in us
 */
void Timer_SetInterrupt(uint32_t timer, uint32_t time_us);

/*
 * Timer_DisableInterrupt(): disables timer interrupt
 * timer: timer on which interrupt is disabled
 */
void Timer_DisableInterrupt(uint32_t timer);

/*
 * Timer_ClearInterrupt(): clear timer interrupt
 * timer: timer on which interrupt needs to be cleared
 */
void Timer_ClearInterrupt(uint32_t timer);

/*
 * Timer_GetIRQn(): returns IRQn of a Timer
 * timer: timer on which IRQn is defined - 0 if it is not defined
 */
uint32_t Timer_GetIRQn(uint32_t timer);

/*
 * Timer_GetTicksUS(): returns the number of Ticks per us
 * timer: timer associated with the Ticks per us
 * @return: Ticks per us - 0 if the timer is disables
 */
uint32_t Timer_GetTicksUS(uint32_t timer);

/*
 * Timer_GetCompareValue(): returns the load value of the selected
 * timer.
 * timer: timer associated with the Ticks per us
 * @return: compare value of the selected singletimer
 */
uint32_t Timer_GetCompareValue(uint32_t timer);

#ifdef __cplusplus
}
#endif
#endif /* _GAP_TIMER_H */
