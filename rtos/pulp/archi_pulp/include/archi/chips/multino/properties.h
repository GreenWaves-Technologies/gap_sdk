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


#ifndef __ARCHI_CHIPS_MULTINO_PROPERTIES_H__
#define __ARCHI_CHIPS_MULTINO_PROPERTIES_H__

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1



/*
 * MEMORY ALIAS
 */

#define ARCHI_HAS_L1_ALIAS             1
#define ARCHI_NO_L1_TINY               1

#define ARCHI_L1_SIZE                  262144

/*
 * IP VERSIONS
 */

#define PERIPH_VERSION      2
#define TIMER_VERSION       2
#define APB_SOC_VERSION     2
#define STDOUT_VERSION      2
#define EU_VERSION          3
#define RISCV_VERSION       4
#define GPIO_VERSION        2
#define PADS_VERSION        2
#define MCHAN_VERSION       6


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER    1
#define ARCHI_L1_TAS_BIT     20
#define ARCHI_CLUSTER_NB_PE  2
#define ARCHI_NB_CLUSTER     2




/*
 * HWS
 */

#define ARCHI_EU_NB_HW_MUTEX 1




/*
 * CLOCKS
 */

#define ARCHI_REF_CLOCK_LOG2 15
#define ARCHI_REF_CLOCK      (1<<ARCHI_REF_CLOCK_LOG2)



/*
 * CLUSTER EVENTS
 */
#define ARCHI_EVT_RAB_WAKEUP  0
#define ARCHI_CL_EVT_DMA0        8
#define ARCHI_CL_EVT_DMA1        9
#define ARCHI_EVT_TIMER0_LO      10
#define ARCHI_EVT_TIMER0_HI      11
#define ARCHI_CL_EVT_ACC0        12
#define ARCHI_CL_EVT_ACC1        13
#define ARCHI_CL_EVT_ACC2        14
#define ARCHI_CL_EVT_ACC3        15
#define ARCHI_CL_EVT_BAR         16
#define ARCHI_CL_EVT_MUTEX       17
#define ARCHI_CL_EVT_DISPATCH    18
#define ARCHI_EVT_GPIO        19
#define ARCHI_EVT_UART        20
#define ARCHI_EVT_SPIM0       21
#define ARCHI_EVT_SPIM1       22
#define ARCHI_EVT_CLKREF      23
#define ARCHI_EVT_MPU_ERROR   28
#define ARCHI_CL_EVT_SOC_EVT     30
#define ARCHI_EVT_SOC_FIFO    31

/*
 * PERIPHS
 */

//#define ARCHI_HAS_UART       1

#endif
