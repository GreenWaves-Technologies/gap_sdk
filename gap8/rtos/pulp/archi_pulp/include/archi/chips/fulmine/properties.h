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


#ifndef __ARCHI_CHIPS_FULMINE_PROPERTIES_H__
#define __ARCHI_CHIPS_FULMINE_PROPERTIES_H__


/*
 * FEATURES
 */ 



/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1

#define ARCHI_L1_SIZE                  65536


/*
 * IP VERSIONS
 */

#define MCHAN_VERSION       5
#define EU_VERSION          1
#define PERIPH_VERSION      1
#define ICACHE_CTRL_VERSION 1
#define TIMER_VERSION       1
#define APB_SOC_VERSION     1
#define STDOUT_VERSION      2
#define OR1K_VERSION        5
#define UDMA_VERSION        1
#define PADS_VERSION        2


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER   1
#define ARCHI_L1_TAS_BIT    20
#define ARCHI_CLUSTER_NB_PE 4





/*
 * CLOCKS
 */

#define ARCHI_REF_CLOCK_LOG2 15
#define ARCHI_REF_CLOCK      (1<<ARCHI_REF_CLOCK_LOG2)




/*
 * CLUSTER EVENTS
 */


#define ARCHI_EVT_DMA    8

#define ARCHI_EVT_UDMA0    10
#define ARCHI_EVT_UDMA1    11
#define ARCHI_EVT_UDMA2    12
#define ARCHI_EVT_UDMA3    13
#define ARCHI_EVT_UDMA4    14
#define ARCHI_EVT_UDMA5    15
#define ARCHI_EVT_UDMA6    16
#define ARCHI_EVT_UDMA7    17



/*
 * UDMA
 */

#define ARCHI_UDMA_HAS_SPIM     1
#define ARCHI_UDMA_HAS_UART     1
#define ARCHI_UDMA_HAS_I2C      1

#define ARCHI_UDMA_NB_SPIM   2
#define ARCHI_UDMA_NB_UART   1
#define ARCHI_UDMA_NB_I2C    1

#define ARCHI_NB_PERIPH                   7

#define ARCHI_UDMA_UART_ID(id)            (1)
#define ARCHI_UDMA_SPIM_ID(id)            (0 + (6*id))
#define ARCHI_UDMA_I2C_ID(id)             (3)
#define ARCHI_UDMA_ADC_ID(id)             (2 + (id))

#define ARCHI_UDMA_EVT_SPIM_TX                 0
#define ARCHI_UDMA_EVT_SPIM_RX                 1
#define ARCHI_UDMA_EVT_SPIM_EOT                2
#define ARCHI_UDMA_EVT_UART_TX                 3
#define ARCHI_UDMA_EVT_UART_RX                 4
#define ARCHI_UDMA_EVT_I2C_TX                  5
#define ARCHI_UDMA_EVT_I2C_RX                  6



#endif
