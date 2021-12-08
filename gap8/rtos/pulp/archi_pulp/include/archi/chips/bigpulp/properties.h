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


#ifndef __ARCHI_CHIPS_BIGPULP_PROPERTIES_H__
#define __ARCHI_CHIPS_BIGPULP_PROPERTIES_H__

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1

#define ARCHI_L1_SIZE                  262144


/*
 * MEMORY ALIAS
 */

#define ARCHI_HAS_L1_ALIAS             1
#define ARCHI_NO_L1_TINY               1



/*
 * IP VERSIONS
 */

#define PERIPH_VERSION      2
#define TIMER_VERSION       2
#define APB_SOC_VERSION     2
#define STDOUT_VERSION      2
#define EU_VERSION          3
#define RISCV_VERSION       4
#define MCHAN_VERSION       6


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER   1
#define ARCHI_L1_TAS_BIT    20
#define ARCHI_CLUSTER_NB_PE 8
#if PULP_CHIP == CHIP_BIGPULP_ZU9EG
#define ARCHI_NB_CLUSTER 2
#endif



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
#define ARCHI_EVT_MPU_ERROR   28
#define ARCHI_CL_EVT_SOC_EVT     30
#define ARCHI_EVT_SOC_FIFO    31

/*
 * Host Architecture
 */
#define ARM     0
#define ARM64   1

#if (PULP_CHIP == CHIP_BIGPULP) || (PULP_CHIP == CHIP_BIGPULP_ZUX) || (PULP_CHIP == CHIP_BIGPULP_JUNO)
    #define HOST_ARCH   ARM64
#elif (PULP_CHIP == CHIP_BIGPULP_Z_7045) || (PULP_CHIP == CHIP_HERO_Z_7045) || (PULP_CHIP == CHIP_BIGPULP_ZU9EG)
    #define HOST_ARCH   ARM
#elif (PULP_CHIP == CHIP_BIGPULP_STANDALONE)
#else
    #error "Failed to define HOST_ARCH for unknown configuration!"
#endif

/*
 * RAB
 */
#define RAB_PORT_ID_WIDTH            1
#define RAB_L1_N_SLICES_PORT_0       4
#define RAB_L1_N_SLICES_PORT_1      32
#define RAB_L2_N_SETS               32
#define RAB_L2_N_ENTRIES_PER_SET    32

/*
 * AXI
 */
#define AXI_USER_WIDTH          6
#define AXI_ID_WIDTH_CORE       4
#define AXI_ID_WIDTH_CLUSTER    2
#if PULP_CHIP == CHIP_BIGPULP       // Juno
    #define AXI_ID_WIDTH_SOC    3
#else
    #define AXI_ID_WIDTH_SOC    1
#endif
#define AXI_ID_WIDTH            (AXI_ID_WIDTH_CORE + AXI_ID_WIDTH_CLUSTER + AXI_ID_WIDTH_SOC)

/*
 * PERIPHS
 */

#define ARCHI_HAS_UART       1

#endif
