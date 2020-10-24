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


#ifndef __ARCHI_CHIPS_VEGA_PROPERTIES_H__
#define __ARCHI_CHIPS_VEGA_PROPERTIES_H__

/*
 * SOC
 */

#define ARCHI_PWM_NB          1
#define ARCHI_PWM_NB_TIMERS 4

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1

#define ARCHI_L1_SIZE                  0x20000

#define ARCHI_MEMORY_POWER             1

/*
 * MEMORY ALIAS
 */

#define ARCHI_HAS_L1_ALIAS             1
#define ARCHI_HAS_L2_ALIAS             1



/*
 * IP VERSIONS
 */

#define FLL_VERSION            1
#define GPIO_VERSION           2
#define UDMA_VERSION           3
#define PERIPH_VERSION         3
#define APB_SOC_VERSION        3
#define ADV_TIMER_VERSION      1
#define SOC_EU_VERSION         2
#define PMU_VERSION            3
#define FC_ICACHE_CTRL_VERSION 2
#define ITC_VERSION            1
#define I3C_VERSION            1
#define TIMER_VERSION          2
#define DPHY_VERSION           1
#define CSI2_VERSION           1
#define MPU_VERSION            1
#define EFUSE_VERSION          1
#define DEBUG_VERSION          1
#define STDOUT_VERSION         2
#define QUIDDIKEY_VERSION      1
#define ROM_VERSION            2
#define RTC_VERSION            1
#define PWM_VERSION            1

#define EU_VERSION             3
#define RISCV_VERSION          4
#define MCHAN_VERSION          7
#define HWCE_VERSION           5

#define CL_CTRL_VERSION        2
#define PADS_VERSION        2


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER   1
#define ARCHI_CL_CID        0

// TAS = Test&Set
#define ARCHI_L1_TAS_BIT    20
#define ARCHI_HAS_CC        1
#define ARCHI_CC_CORE_ID    8
#define ARCHI_CLUSTER_NB_PE 8
#define ARCHI_HAS_CLUSTER_CLK_GATE 1


/*
 * CLUSTER EVENT UNIT
 */

//#define ARCHI_HAS_NO_BARRIER    1
//#define ARCHI_HAS_NO_DISPATCH   1
//#define ARCHI_HAS_NO_MUTEX   1


/*
 * HWS
 */

#define ARCHI_EU_NB_HW_MUTEX 1



/*
 * FC
 */

#define ARCHI_FC_CID        31
#define ARCHI_HAS_FC_ITC     1
#define ARCHI_FC_HAS_ICACHE 1



/*
 * CLOCKS
 */

#define ARCHI_REF_CLOCK_LOG2 15
#define ARCHI_REF_CLOCK      (1<<ARCHI_REF_CLOCK_LOG2) // log2 of ref_clk frequency



/*
 * UDMA
 */
#define ARCHI_UDMA_HAS_SPIM      1
#define ARCHI_UDMA_HAS_HYPER     1
#define ARCHI_UDMA_HAS_SDIO      1
#define ARCHI_UDMA_HAS_UART      1
#define ARCHI_UDMA_HAS_I2C       1
#define ARCHI_UDMA_HAS_I2S       1
#define ARCHI_UDMA_HAS_JTAG_FIFO 1
#define ARCHI_UDMA_HAS_MRAM      1
#define ARCHI_UDMA_HAS_CAM       1
#define ARCHI_UDMA_HAS_CSI2      1
#define ARCHI_UDMA_HAS_FILTER    1

#define ARCHI_UDMA_NB_SPIM      4
#define ARCHI_UDMA_NB_HYPER     1
#define ARCHI_UDMA_NB_SDIO      1
#define ARCHI_UDMA_NB_UART      3
#define ARCHI_UDMA_NB_I2C       1
#define ARCHI_UDMA_NB_I2S       1
#define ARCHI_UDMA_NB_JTAG_FIFO 1
#define ARCHI_UDMA_NB_MRAM      1
#define ARCHI_UDMA_NB_CAM       1
#define ARCHI_UDMA_NB_CSI2      1
#define ARCHI_UDMA_NB_FILTER    1

#define ARCHI_UDMA_HYPER_VERSION 2

#define ARCHI_UDMA_SPIM_ID(id)      (0   + (id))
#define ARCHI_UDMA_UART_ID(id)      (4   + (id))
#define ARCHI_UDMA_I2C_ID(id)       (7   + (id))
#define ARCHI_UDMA_HYPER_ID(id)     (8   + (id))
#define ARCHI_UDMA_JTAG_FIFO_ID(id) (9   + (id))
#define ARCHI_UDMA_SDIO_ID(id)      (10  + (id))
#define ARCHI_UDMA_I2S_ID(id)       (11  + (id))
#define ARCHI_UDMA_CAM_ID(id)       (12  + (id))
#define ARCHI_UDMA_CSI2_ID(id)      (13  + (id))
#define ARCHI_UDMA_MRAM_ID(id)      (14  + (id))
#define ARCHI_UDMA_FILTER_ID(id)    (15  + (id))

#define ARCHI_NB_PERIPH             (16)


/*
 * FLLS
*/

#define ARCHI_NB_FLL  3

/*
 * GPIO
*/

#define ARCHI_NB_GPIO 32



/*
 * SOC EVENTS GENERATOR 
 */

#define ARCHI_SOC_EVENT_PERIPH_EVT_NB     160

#define ARCHI_SOC_EVENT_SW_NB    	 (8)

#define ARCHI_SOC_EVENT_NB_TOTAL     256

#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2 2
#define ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT (1<<ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2)
#define ARCHI_SOC_EVENT_UDMA_FIRST_EVT    0
#define ARCHI_SOC_EVENT_UDMA_NB_EVT       (ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT * ARCHI_NB_PERIPH)
#define ARCHI_SOC_EVENT_UDMA_NB_TGEN_EVT  6


#define ARCHI_SOC_EVENT_PERIPH_FIRST_EVT(x)     ((x)*ARCHI_SOC_EVENT_UDMA_NB_CHANNEL_EVT)

#define ARCHI_SOC_EVENT_SPIM0_RX          0
#define ARCHI_SOC_EVENT_SPIM0_TX          1
#define ARCHI_SOC_EVENT_SPIM0_CMD         2
#define ARCHI_SOC_EVENT_SPIM0_EOT         3

#define ARCHI_SOC_EVENT_SPIM1_RX          4
#define ARCHI_SOC_EVENT_SPIM1_TX          5
#define ARCHI_SOC_EVENT_SPIM1_CMD         6
#define ARCHI_SOC_EVENT_SPIM1_EOT         7

#define ARCHI_SOC_EVENT_SPIM2_RX          8
#define ARCHI_SOC_EVENT_SPIM2_TX          9
#define ARCHI_SOC_EVENT_SPIM2_CMD         10
#define ARCHI_SOC_EVENT_SPIM2_EOT         11

#define ARCHI_SOC_EVENT_SPIM3_RX          12
#define ARCHI_SOC_EVENT_SPIM3_TX          13
#define ARCHI_SOC_EVENT_SPIM3_CMD         14
#define ARCHI_SOC_EVENT_SPIM3_EOT         15

#define ARCHI_SOC_EVENT_UART0_RX          16
#define ARCHI_SOC_EVENT_UART0_TX          17
#define ARCHI_SOC_EVENT_UART0_EOT         18
#define ARCHI_SOC_EVENT_UART0_RX_DATA     19

#define ARCHI_SOC_EVENT_UART1_RX          20
#define ARCHI_SOC_EVENT_UART1_TX          21
#define ARCHI_SOC_EVENT_UART1_EOT         22
#define ARCHI_SOC_EVENT_UART1_RX_DATA     23

#define ARCHI_SOC_EVENT_UART2_RX          24
#define ARCHI_SOC_EVENT_UART2_TX          25
#define ARCHI_SOC_EVENT_UART2_EOT         26
#define ARCHI_SOC_EVENT_UART2_RX_DATA     27

#define ARCHI_SOC_EVENT_I2C0_RX           28
#define ARCHI_SOC_EVENT_I2C0_TX           29

#define ARCHI_SOC_EVENT_HYPER0_RX         32
#define ARCHI_SOC_EVENT_HYPER0_TX         33

#define ARCHI_SOC_EVENT_JTAG_FIFO0_RX     36
#define ARCHI_SOC_EVENT_JTAG_FIFO0_TX     37

#define ARCHI_SOC_EVENT_SDIO0_RX          40
#define ARCHI_SOC_EVENT_SDIO0_TX          41

#define ARCHI_SOC_EVENT_I2S0_RX           44
#define ARCHI_SOC_EVENT_I2S0_TX           45

#define ARCHI_SOC_EVENT_CPI0_RX           48

#define ARCHI_SOC_EVENT_CIS2_0_CH0_RX     52
#define ARCHI_SOC_EVENT_CIS2_0_CH1_RX     53

#define ARCHI_SOC_EVENT_MRAM0_ERASE       56
#define ARCHI_SOC_EVENT_MRAM0_EOTX        57

#define ARCHI_SOC_EVENT_FILTER0_RX        60
#define ARCHI_SOC_EVENT_FILTER0_TX        61

#define ARCHI_SOC_EVENT_CLUSTER_ON_OFF    128
#define ARCHI_SOC_EVENT_CLUSTER_NOT_BUSY  131   
#define ARCHI_SOC_EVENT_CLUSTER_CG_OK     132
#define ARCHI_SOC_EVENT_PICL_OK           133
#define ARCHI_SOC_EVENT_SCU_OK            134
#define ARCHI_SOC_EVENT_ADV_TIMER_0       135       
#define ARCHI_SOC_EVENT_ADV_TIMER_1       136     
#define ARCHI_SOC_EVENT_ADV_TIMER_2       137     
#define ARCHI_SOC_EVENT_ADV_TIMER_3       138     

#define ARCHI_SOC_EVENT_GPIO              139

#define ARCHI_SOC_EVENT_RTC_APB_IRQ       140
#define ARCHI_SOC_EVENT_RTC_IRQ           141

#define ARCHI_SOC_EVENT_SW_EVENT0         160
#define ARCHI_SOC_EVENT_SW_EVENT1         161
#define ARCHI_SOC_EVENT_SW_EVENT2         162
#define ARCHI_SOC_EVENT_SW_EVENT3         163
#define ARCHI_SOC_EVENT_SW_EVENT4         164
#define ARCHI_SOC_EVENT_SW_EVENT5         165
#define ARCHI_SOC_EVENT_SW_EVENT6         166
#define ARCHI_SOC_EVENT_SW_EVENT7         167
#define ARCHI_SOC_EVENT_REFCLK            168  

/*
 * FC EVENTS
 */

#define ARCHI_FC_EVT_FIRST_SW          0
#define ARCHI_FC_EVT_NB_SW             8

#define ARCHI_FC_EVT_DMA_EVT           8
#define ARCHI_FC_EVT_DMA_IRQ           9

#define ARCHI_FC_EVT_TIMER0_LO         10
#define ARCHI_FC_EVT_TIMER0_HI         11
#define ARCHI_FC_EVT_TIMER1_LO         12
#define ARCHI_FC_EVT_TIMER1_HI         13
#define ARCHI_FC_EVT_CLK_REF           14
#define ARCHI_FC_EVT_GPIO              15
#define ARCHI_FC_EVT_CLK_REF_RISE      16
#define ARCHI_FC_EVT_ADV_TIMER0        17
#define ARCHI_FC_EVT_ADV_TIMER1        18
#define ARCHI_FC_EVT_ADV_TIMER2        19
#define ARCHI_FC_EVT_ADV_TIMER3        20
#define ARCHI_FC_EVT_RTC_APB_IRQ       21
#define ARCHI_FC_EVT_RTC_IRQ           22
#define ARCHI_FC_EVT_PICL_OK           24
#define ARCHI_FC_EVT_SCU_OK            25
#define ARCHI_FC_EVT_SOC_EVT           26
#define ARCHI_FC_EVT_QUIDDIKEY_IRQ     27
#define ARCHI_FC_EVT_MPU_ERROR         28
#define ARCHI_FC_EVT_QUEUE_ERROR       29
#define ARCHI_FC_EVT_HP0               30
#define ARCHI_FC_EVT_HP1               31



/*
 * CLUSTER EVENTS
 */

#define ARCHI_CL_EVT_FIRST_SW   0
#define ARCHI_CL_EVT_NB_SW      8

#define ARCHI_CL_EVT_DMA0       8
#define ARCHI_CL_EVT_DMA1       9
#define ARCHI_CL_EVT_TIMER0_LO  10
#define ARCHI_CL_EVT_TIMER0_HI  11
#define ARCHI_CL_EVT_ACC0       12
#define ARCHI_CL_EVT_ACC1       13
#define ARCHI_CL_EVT_ACC2       14
#define ARCHI_CL_EVT_ACC3       15
#define ARCHI_CL_EVT_BAR        16
#define ARCHI_CL_EVT_MUTEX      17
#define ARCHI_CL_EVT_DISPATCH   18
#define ARCHI_CL_EVT_CLUSTER0   22
#define ARCHI_CL_EVT_CLUSTER1   23
#define ARCHI_CL_EVT_SOC_EVT    27

/*
 * EFUSE
 */

#define ARCHI_EFUSE_NB_REGS    128
#define ARCHI_EFUSE_REG_WIDTH  32

#endif
