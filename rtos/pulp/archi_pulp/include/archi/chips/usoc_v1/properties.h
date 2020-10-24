/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
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


#ifndef __ARCHI_CHIPS_WOLFE_PROPERTIES_H__
#define __ARCHI_CHIPS_WOLFE_PROPERTIES_H__

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1



/*
 * MEMORY ALIAS
 */

#define ARCHI_HAS_L2_ALIAS             1




/*
 * IP VERSIONS
 */

#define UDMA_VERSION        2
#define PERIPH_VERSION      2
#define TIMER_VERSION       2
#define SOC_EU_VERSION      1
#define APB_SOC_VERSION     2
#define STDOUT_VERSION      2
#define GPIO_VERSION        2
#define ITC_VERSION         1
#define RISCV_VERSION       4




/*
 * HWS
 */

#define ARCHI_EU_NB_HW_MUTEX 1



/*
 * FC
 */

#define ARCHI_FC_CID        31
#define ARCHI_HAS_FC_ITC     1



/*
 * CLOCKS
 */

#define ARCHI_REF_CLOCK      (50000000/4)



/*
 * UDMA
 */

#define ARCHI_UDMA_HAS_SPIM  1
#define ARCHI_UDMA_HAS_UART  1
#define ARCHI_UDMA_HAS_I2C   1
#define ARCHI_UDMA_HAS_I2S   1
#define ARCHI_UDMA_HAS_HYPER 1

#define ARCHI_UDMA_NB_SPIM  8
#define ARCHI_UDMA_NB_UART  3
#define ARCHI_UDMA_NB_I2C   4
#define ARCHI_UDMA_NB_I2S   4
#define ARCHI_UDMA_NB_HYPER 1

#define ARCHI_UDMA_SPIM_ID(id)            (3 + (id))
#define ARCHI_UDMA_UART_ID(id)            (0 + (id))
#define ARCHI_UDMA_I2C_ID(id)             (10 + (id))
#define ARCHI_UDMA_I2S_ID(id)             (14 + (id))
#define ARCHI_UDMA_HYPER_ID(id)           (19 + (id))

#define ARCHI_NB_PERIPH                   20



/*
 * SOC EVENTS
 */

#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2 1
#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT (1<<ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2)
#define ARCHI_SOC_EVENT_UDMA_FIRST_EVT   0
#define ARCHI_SOC_EVENT_UDMA_NB_EVT      40
#define ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT 22
#define ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT 8

#define ARCHI_SOC_EVENT_PERIPH_FIRST_EVT(x)     ((x)*ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT)

#define ARCHI_SOC_EVENT_SPIM0_EOT    40
#define ARCHI_SOC_EVENT_SPIM1_EOT    41
#define ARCHI_SOC_EVENT_UART_EXTRA   43
#define ARCHI_SOC_EVENT_I2C0_EXTRA   44
#define ARCHI_SOC_EVENT_I2C1_EXTRA   45
#define ARCHI_SOC_EVENT_I2S_EXTRA    46



#define ARCHI_SOC_EVENT_GPIO         42


#define ARCHI_SOC_EVENT_NB_I2S_CHANNELS  4
#define ARCHI_SOC_EVENT_NB_UDMA_CHANNELS 19

#define ARCHI_SOC_EVENT_SW_EVENT0    48
#define ARCHI_SOC_EVENT_SW_EVENT1    49
#define ARCHI_SOC_EVENT_SW_EVENT2    50
#define ARCHI_SOC_EVENT_SW_EVENT3    51
#define ARCHI_SOC_EVENT_SW_EVENT4    52
#define ARCHI_SOC_EVENT_SW_EVENT5    53
#define ARCHI_SOC_EVENT_SW_EVENT6    54
#define ARCHI_SOC_EVENT_SW_EVENT7    55

#define ARCHI_SOC_EVENT_NB           8

#define ARCHI_SOC_EVENT_REF_CLK_RISE 56



/*
 * FC INTERRUPTS
 */

#define ARCHI_FC_EVT_FIRST_SW   0
#define ARCHI_FC_EVT_NB_SW      8
#define ARCHI_FC_EVT_TIMER0_LO     10
#define ARCHI_FC_EVT_TIMER0_HI     11
#define ARCHI_FC_EVT_CLK_REF    14
#define ARCHI_FC_EVT_GPIO       15
#define ARCHI_FC_EVT_RTC        16
#define ARCHI_FC_EVT_ADV_TIMER0 17
#define ARCHI_FC_EVT_ADV_TIMER1 18
#define ARCHI_FC_EVT_ADV_TIMER2 19
#define ARCHI_FC_EVT_ADV_TIMER3 20
#define ARCHI_FC_EVT_SOC_EVT          26
#define ARCHI_FC_EVT_QUEUE_ERROR      29


#endif
