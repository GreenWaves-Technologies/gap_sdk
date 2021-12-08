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


#ifndef __ARCHI_CHIPS_HONEY_PROPERTIES_H__
#define __ARCHI_CHIPS_HONEY_PROPERTIES_H__


/*
 * FEATURES
 */ 

#define ARCHI_HAS_UART 1  

#define ARCHI_L1_SIZE                  65536


/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1



/*
 * IP VERSIONS
 */

#define MCHAN_VERSION       4
#define EU_VERSION          1
#define PERIPH_VERSION      1
#define ICACHE_CTRL_VERSION 1
#define TIMER_VERSION       1
#define APB_SOC_VERSION     1
#define STDOUT_VERSION      1
#define FLL_VERSION         0
#define RISCV_VERSION       3
#define GPIO_VERSION        0


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
 * FLLS
*/

#define ARCHI_NB_FLL  1



/*
 * CLUSTER EVENTS
 */


#define ARCHI_EVT_GPIO  9

#define ARCHI_EVT_SPIM0 10
#define ARCHI_EVT_SPIM1 11

#define ARCHI_EVT_UART  12



#endif
