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

 - Change all reload value to compare value.
 - Support only two hardware timer, timer64 -> timer high and timer low of 32 bits.
 - Add Timer_Config function
 - Add Timer_ReadCycle function
 - Change file name from abp_timer.c to gap_timer.c.
 */

#include "cmsis.h"
#include "gap_timer.h"
#include "gap_fll.h"

/* Timer Private Data */
typedef struct {
    /* Timer Definition */
    TimerL_Type   *timerL;
    TimerH_Type   *timerH;
    #ifdef FEATURE_CLUSTER
    TimerL_Type   *CLUSTER_TimerL;
    TimerH_Type   *CLUSTER_TimerH;
    #endif
    /* Timer IRQn */
    uint32_t timerIRQn[NUM_TIMERS];
    /* Timer Compare Value */
    uint32_t timerCompare[NUM_TIMERS];
    /* Timer state */
    uint32_t state[NUM_TIMERS];
} apb_timer_t;

/* Timer state definitions */
#define TIMER_INITIALIZED   (1)
#define TIMER_ENABLED       (1 << 1)

/*
 * This Timer is written for MBED OS and keeps count
 * of the ticks. All the elaboration logic is demanded
 * to the upper layers.
 */
#define TIMER_MAX_VALUE     0xFFFFFFFF
#define TIMER_TICKS_US      (SystemCoreClock/1000000)

/* Timers Array */
static apb_timer_t Timer64;

/*
 * Timer_Initialize(): Initializes an hardware timer
 * timer: timer to be Initialized
 * time_us: timer compare value in us - 0 to compare to timer max value
 *          time_us = tick_value / TIMER_TICKS_US
 */
void Timer_Initialize(uint32_t timer, uint32_t time_us)
{
    uint32_t compare = 0;

    if (timer < NUM_TIMERS)
    {
        if (time_us == 0)
            compare = TIMER_MAX_VALUE;
        else
            compare = (time_us) * TIMER_TICKS_US;

        switch(timer) {
            case 0: Timer64.timerL = TIMERL;
                    Timer64.timerIRQn[timer] = FC_TIMER0_IRQn;
                    break;
            case 1: Timer64.timerH = TIMERH;
                    Timer64.timerIRQn[timer] = FC_TIMER1_IRQn;
                    break;
            #ifdef FEATURE_CLUSTER
            case 2: Timer64.CLUSTER_TimerL = CLUSTER_TIMERL;
                    Timer64.timerIRQn[timer] = FC_TIMER0_IRQn;
                    break;
            case 3: Timer64.CLUSTER_TimerH = CLUSTER_TIMERH;
                    Timer64.timerIRQn[timer] = FC_TIMER1_IRQn;
                    break;
            #endif
            default: break;
        }

        Timer64.timerCompare[timer] = compare;
        Timer64.state[timer] = TIMER_INITIALIZED;
    }
}

static inline uint32_t Timer_Config(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler, char clock_src, char mode_64)
{
    return (enable << TIMERL_CFG_REG_LOW_ENABLE_Pos)
        | (reset << TIMERL_CFG_REG_LOW_RESET_Pos)
        | (irq_enable << TIMERL_CFG_REG_LOW_IRQE_Pos)
        | (event_mask << TIMERL_CFG_REG_LOW_IEM_Pos)
        | (cmp_clr << TIMERL_CFG_REG_LOW_CMP_CLR_Pos)
        | (one_shot << TIMERL_CFG_REG_LOW_ONE_SHOT_Pos)
        | (prescaler_enable << TIMERL_CFG_REG_LOW_PRESCALERE_Pos)
        | (clock_src << TIMERL_CFG_REG_LOW_CLKS_Pos)
        | (prescaler << TIMERL_CFG_REG_LOW_PRESCALER_Pos)
        | (mode_64 << TIMERL_CFG_REG_LOW_64BIT_Pos);
}

/*
 * Timer_Enable(): Enables a hardware timer
 * timer: timer to be enabled
 */
void Timer_Enable(uint32_t timer)
{
    /* The timer has to be contained in a valid range */
    if (timer < NUM_TIMERS) {
        /* Timer has to be already initialized */
        if (Timer64.state[timer] == TIMER_INITIALIZED) {
            /* When RTOS is in use, SysTick will occupy Timer0 */
            #ifndef MBED_CONF_RTOS_PRESENT
            if (timer == TIMER0) {
                /* Compare Value */
                (Timer64.timerL)->COMPARE = Timer64.timerCompare[TIMER0];
                /* Reset Enable Counter */
                (Timer64.timerL)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
                /* Reset value */
                (Timer64.timerL)->VALUE = 0;
            }
            #endif
            if (timer == TIMER1) {
                /* Compare Value */
                (Timer64.timerH)->COMPARE = Timer64.timerCompare[TIMER1];
                /* Reset Enable Counter */
                (Timer64.timerH)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
                /* Reset value */
                (Timer64.timerH)->VALUE = 0;
            }
            #ifdef FEATURE_CLUSTER
            if (timer == TIMER0_CLUSTER) {
                /* Compare Value */
                (Timer64.CLUSTER_TimerL)->COMPARE = Timer64.timerCompare[TIMER0_CLUSTER];
                /* Reset Enable Counter */
                (Timer64.CLUSTER_TimerL)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
                /* Reset value */
                (Timer64.CLUSTER_TimerL)->VALUE = 0;
            }
            if (timer == TIMER1_CLUSTER) {
                /* Compare Value */
                (Timer64.CLUSTER_TimerH)->COMPARE = Timer64.timerCompare[TIMER1_CLUSTER];
                /* Reset Enable Counter */
                (Timer64.CLUSTER_TimerH)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
                /* Reset value */
                (Timer64.CLUSTER_TimerH)->VALUE = 0;
            }
            #endif
            /* Change timer state */
            Timer64.state[timer] |= TIMER_ENABLED;
        }
    }
}

/*
 * Timer_Disable(): Disables a hardware timer
 * timer: timer to be disabled
 */
void Timer_Disable(uint32_t timer)
{
    /* The timer has to be contained in a valid range */
    if (timer < NUM_TIMERS) {
        /* Timer has to be already initialized and enabled */
        if (Timer64.state[timer] == (TIMER_INITIALIZED | TIMER_ENABLED)) {
            switch(timer) {
            case 0: (Timer64.timerL)->CTRL = 0x0;
                break;
            case 1: (Timer64.timerH)->CTRL = 0x0;
                break;
            #ifdef FEATURE_CLUSTER
            case 2: (Timer64.CLUSTER_TimerL)->CTRL = 0x0;
                break;
            case 3: (Timer64.CLUSTER_TimerH)->CTRL = 0x0;
                break;
            #endif
            default: break;
            }
            /* Change timer state */
            Timer64.state[timer] = TIMER_INITIALIZED;
        }
    }
}

/*
 * Timer_isEnabled(): verifies if a timer is enabled
 * timer: timer to be verified
 * @return: 0 disabled - 1 enabled
 */
uint32_t Timer_isEnabled(uint32_t timer)
{
    /* The timer has to be contained in a valid range */
    if (timer < NUM_TIMERS) {
        /* Timer has to be already initialized and enabled */
        if (Timer64.state[timer] == (TIMER_INITIALIZED | TIMER_ENABLED))
            return 1;
    } else {
        return 0;
    }
    return 0;
}

/*
 * Timer_Read(): provides timer cycle VALUE
 * timer: timer to be read
 * @return: timer cycle VALUE
 */
uint32_t Timer_ReadCycle(uint32_t timer)
{
    uint32_t return_value = 0;
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        switch(timer) {
        case 0: return_value = (Timer64.timerL)->VALUE;
            break;
        case 1: return_value = (Timer64.timerH)->VALUE;
            break;
        #ifdef FEATURE_CLUSTER
        case 2: return_value = (Timer64.CLUSTER_TimerL)->VALUE;
            break;
        case 3: return_value = (Timer64.CLUSTER_TimerH)->VALUE;
            break;
        #endif
        default: break;
        }
    }

    return return_value;
}

/*
 * Timer_Read(): provides timer VALUE
 * timer: timer to be read
 * @return: timer VALUE us
 */
uint32_t Timer_Read(uint32_t timer)
{
    return (Timer_ReadCycle(timer) / TIMER_TICKS_US);
}

/*
 * Timer_SetInterrupt(): sets timer Interrupt
 * timer: timer on which interrupt is set
 * time_us: compareing time in us
 */
void Timer_SetInterrupt(uint32_t timer, uint32_t time_us)
{
    uint32_t cmp_time_us = 0;
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        /* Disable Timer */
        Timer_Disable(timer);

        /* Check time us condition */
        if(time_us == TIMER_DEFAULT_COMPARE)
            cmp_time_us = TIMER_MAX_VALUE;
        else
            cmp_time_us = time_us * TIMER_TICKS_US;

        /* Initialize Timer Value */
        Timer64.timerCompare[timer] = cmp_time_us;

        switch(timer) {
        case 0: {
            /* Enable Interrupt */
            (Timer64.timerL)->COMPARE = Timer64.timerCompare[timer];
            (Timer64.timerL)->VALUE = 0;
            /* Enable Counter */
            (Timer64.timerL)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
        }
            break;
        case 1: {
            (Timer64.timerH)->COMPARE = Timer64.timerCompare[timer];
            (Timer64.timerH)->VALUE = 0;
            /* Enable Counter */
            (Timer64.timerH)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
        }
            break;
        #ifdef FEATURE_CLUSTER
        case 2: {
            /* Enable Interrupt */
            (Timer64.CLUSTER_TimerL)->COMPARE = Timer64.timerCompare[timer];
            (Timer64.CLUSTER_TimerL)->VALUE = 0;
            /* Enable Counter */
            (Timer64.CLUSTER_TimerL)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
        }
            break;
        case 3: {
            (Timer64.CLUSTER_TimerH)->COMPARE = Timer64.timerCompare[timer];
            (Timer64.CLUSTER_TimerH)->VALUE = 0;
            /* Enable Counter */
            (Timer64.CLUSTER_TimerH)->CTRL = Timer_Config(1, 1, 1, 0, 1, 0, 0, 0, 0, 0);
        }
            break;
        #endif
        default: break;
        }

        /* Change timer state */
        Timer64.state[timer] |= TIMER_ENABLED;
    }
}

/*
 * Timer_DisableInterrupt(): disables timer interrupt
 * timer: timer on which interrupt is disabled
 */
void Timer_DisableInterrupt(uint32_t timer)
{
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        /* Disable Interrupt */
        switch(timer) {
        case 0: (Timer64.timerL)->CTRL &= ~(1 << TIMERL_CFG_REG_LOW_IRQE_Pos);
            break;
        case 1: (Timer64.timerH)->CTRL &= ~(1 << TIMERL_CFG_REG_LOW_IRQE_Pos);
            break;
        #ifdef FEATURE_CLUSTER
        case 2: (Timer64.CLUSTER_TimerL)->CTRL &= ~(1 << TIMERL_CFG_REG_LOW_IRQE_Pos);
            break;
        case 3: (Timer64.CLUSTER_TimerH)->CTRL &= ~(1 << TIMERL_CFG_REG_LOW_IRQE_Pos);
            break;
        #endif
        default: break;
        }
    }
}

/*
 * Timer_ClearInterrupt(): clear timer interrupt
 * timer: timer on which interrupt needs to be cleared
 */
void Timer_ClearInterrupt(uint32_t timer)
{
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        /* Clear Interrupt */
        /* Already done in compare clear bit */

    }
}

/*
 * Timer_GetIRQn(): returns IRQn of a Timer
 * timer: timer on which IRQn is defined - 0 if it is not defined
 */
uint32_t Timer_GetIRQn(uint32_t timer)
{
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        return Timer64.timerIRQn[timer];
    }
    return 0;
}

/*
 * Timer_GetTicksUS(): returns the number of Ticks per us
 * timer: timer associated with the Ticks per us
 * @return: Ticks per us - 0 if the timer is disables
 */
uint32_t Timer_GetTicksUS(uint32_t timer)
{
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        return TIMER_TICKS_US;
    }
    return 0;
}

/*
 * Timer_GetCompareValue(): returns the compare value of the selected
 * timer.
 * timer: timer associated with the Ticks per us
 * @return: compare value of the selected singletimer
 */
uint32_t Timer_GetCompareValue(uint32_t timer)
{
    /* Verify if the Timer is enabled */
    if (Timer_isEnabled(timer) == 1) {
        if (timer == TIMER1)
            return Timer64.timerCompare[timer] / TIMER_TICKS_US;
        else
            return Timer64.timerCompare[timer] / TIMER_TICKS_US;
    }
    return 0;
}
