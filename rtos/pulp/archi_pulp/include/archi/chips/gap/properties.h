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


#ifndef __ARCHI_CHIPS_GAP_PROPERTIES_H__
#define __ARCHI_CHIPS_GAP_PROPERTIES_H__

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1
#define ARCHI_HAS_FC_TCDM              1

#define ARCHI_L1_SIZE                  65536


/*
 * MEMORY ALIAS
 */

#define ARCHI_HAS_L1_ALIAS             1



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
#define EU_VERSION          3
#define FLL_VERSION         1
#define RISCV_VERSION       4
#define HWCE_VERSION        4
#define MCHAN_VERSION       6
#define EFUSE_VERSION       1
#define PADS_VERSION        2
#define PWM_VERSION         1


/*
 * SOC
 */

#define ARCHI_PWM_NB          1
#define ARCHI_PWM_NB_TIMERS 4


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER          1
#define ARCHI_L1_TAS_BIT           20
#define ARCHI_CLUSTER_NB_PE        8
#if PULP_CHIP != CHIP_GAP
#define ARCHI_HAS_CLUSTER_CLK_GATE 1
#endif

/*
 * CLUSTER EVENT UNIT
 */

//#define ARCHI_HAS_NO_BARRIER    1
//#define ARCHI_HAS_NO_DISPATCH   1
//#define ARCHI_HAS_NO_MUTEX   1



/*
 * FC
 */

#define ARCHI_FC_CID        32
#define ARCHI_HAS_FC_EU     1
#define ARCHI_FC_HAS_ICACHE 1
#define ARCHI_HAS_FC        1



/*
 * CLOCKS
 */

#define ARCHI_REF_CLOCK_LOG2 15
#define ARCHI_REF_CLOCK      (1<<ARCHI_REF_CLOCK_LOG2)



/*
 * HWS
 */

#define ARCHI_EU_NB_HW_MUTEX 1


/*
 * UDMA
 */

#define ARCHI_UDMA_HAS_RF    1
#define ARCHI_UDMA_HAS_SPIM  1
#define ARCHI_UDMA_HAS_HYPER 1
#define ARCHI_UDMA_HAS_UART  1
#define ARCHI_UDMA_HAS_I2C   1
#define ARCHI_UDMA_HAS_I2S   1
#define ARCHI_UDMA_HAS_TCDM  1
#define ARCHI_UDMA_HAS_CAM   1

#define ARCHI_UDMA_NB_RF    1
#define ARCHI_UDMA_NB_SPIM  2
#define ARCHI_UDMA_NB_HYPER 1
#define ARCHI_UDMA_NB_UART  1
#define ARCHI_UDMA_NB_I2C   2
#define ARCHI_UDMA_NB_I2S   1
#define ARCHI_UDMA_NB_TCDM  1
#define ARCHI_UDMA_NB_CAM   1

#define ARCHI_UDMA_RF_ID(id)              0
#define ARCHI_UDMA_SPIM_ID(id)            (1 + (id))
#define ARCHI_UDMA_HYPER_ID(id)           3
#define ARCHI_UDMA_UART_ID(id)            4
#define ARCHI_UDMA_I2C_ID(id)             (5 + (id))
#define ARCHI_UDMA_TCDM_ID(id)            7
#define ARCHI_UDMA_I2S_ID(id)             8
#define ARCHI_UDMA_CAM_ID(id)             9

#define ARCHI_NB_PERIPH                   10



/*
 * FLLS
*/

#define ARCHI_NB_FLL  2



/*
 * GPIO
*/

#define ARCHI_NB_GPIO 32



/*
 * SOC EVENTS
 */

#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2 1
#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT (1<<ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2)
#define ARCHI_SOC_EVENT_UDMA_FIRST_EVT    0
#define ARCHI_SOC_EVENT_UDMA_NB_EVT       19
#define ARCHI_SOC_EVENT_UDMA_NB_TGEN_EVT  0
#define ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT 20
#define ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT 10

#define ARCHI_SOC_EVENT_PERIPH_FIRST_EVT(x)     ((x)*ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT)

#define ARCHI_SOC_EVENT_NB_TOTAL     64

#define ARCHI_SOC_EVENT_I2C0_RX      10
#define ARCHI_SOC_EVENT_I2C0_TX      11

#define ARCHI_SOC_EVENT_LVDS_HP0     20
#define ARCHI_SOC_EVENT_LVDS_HP1     21
#define ARCHI_SOC_EVENT_SPIM0_EOT    22
#define ARCHI_SOC_EVENT_SPIM1_EOT    23
#define ARCHI_SOC_EVENT_SPIM2_EOT    24
#define ARCHI_SOC_EVENT_UART_EXTRA   25
#define ARCHI_SOC_EVENT_I2C0_EXTRA   26
#define ARCHI_SOC_EVENT_I2C1_EXTRA   27
#define ARCHI_SOC_EVENT_I2S_EXTRA    28
#define ARCHI_SOC_EVENT_CAM_EXTRA    29

#define ARCHI_SOC_EVENT_CLUSTER_ON_OFF   31
#define ARCHI_SOC_EVENT_MSP              32
#define ARCHI_SOC_EVENT_ICU_MODE_CHANGED 33
#define ARCHI_SOC_EVENT_ICU_OK           34
#define ARCHI_SOC_EVENT_ICU_DELAYED      35
#define ARCHI_SOC_EVENT_PICL_OK          36
#define ARCHI_SOC_EVENT_SCU_OK           37
#define ARCHI_SOC_EVENT_PMU_FIRST_EVENT  ARCHI_SOC_EVENT_CLUSTER_ON_OFF
#define ARCHI_SOC_EVENT_PMU_NB_EVENTS    7

#define ARCHI_SOC_EVENT_GPIO         42

#define ARCHI_SOC_EVENT_RTC_APB_IRQ      43
#define ARCHI_SOC_EVENT_RTC_IRQ          44
#define ARCHI_SOC_EVENT_RTC_FIRST_EVT    ARCHI_SOC_EVENT_RTC_APB_IRQ

#define ARCHI_SOC_EVENT_ADV_TIMER0       38
#define ARCHI_SOC_EVENT_ADV_TIMER1       39
#define ARCHI_SOC_EVENT_ADV_TIMER2       40
#define ARCHI_SOC_EVENT_ADV_TIMER3       41
#define ARCHI_SOC_EVENT_ADV_TIMER_FIRST_EVT    ARCHI_SOC_EVENT_ADV_TIMER0
#define ARCHI_SOC_EVENT_ADV_TIMER_NB_EVT       4


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
 * CLUSTER EVENTS
 */

#define ARCHI_CL_EVT_DMA0        8
#define ARCHI_CL_EVT_DMA1        9
#define ARCHI_EVT_TIMER0      10
#define ARCHI_EVT_TIMER1      11
#define ARCHI_CL_EVT_ACC0        12
#define ARCHI_CL_EVT_ACC1        13
#define ARCHI_CL_EVT_ACC2        14
#define ARCHI_CL_EVT_ACC3        15
#define ARCHI_CL_EVT_BAR         16
#define ARCHI_CL_EVT_MUTEX       17
#define ARCHI_CL_EVT_DISPATCH    18
#define ARCHI_EVT_MPU_ERROR   28
#define ARCHI_CL_EVT_SOC_EVT     30
#define ARCHI_EVT_SOC_FIFO    31



/*
 * FC EVENTS
 */

#define ARCHI_FC_EVT_TIMER0_LO        10
#define ARCHI_FC_EVT_TIMER0_HI        11
#define ARCHI_FC_EVT_SOC_EVT          27
#if PULP_CHIP == CHIP_GAP8_REVC
#define ARCHI_FC_IRQ_HYPER_TX         29
#define ARCHI_FC_IRQ_HYPER_RX         30
#endif


#endif
