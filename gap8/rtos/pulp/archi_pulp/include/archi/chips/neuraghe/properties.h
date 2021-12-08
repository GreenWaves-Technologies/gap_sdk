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


#ifndef __ARCHI_CHIPS_NEURAGHE_PROPERTIES_H__
#define __ARCHI_CHIPS_NEURAGHE_PROPERTIES_H__

/*
 * MEMORIES
 */ 

#define ARCHI_HAS_L2                   1
#define ARCHI_HAS_L1                   1

#define ARCHI_L1_SIZE                  0x400




/*
 * IP VERSIONS
 */

#define STDOUT_VERSION      2
#define RISCV_VERSION       4
#define EU_VERSION          3
#define TIMER_VERSION       2


/*
 * CLUSTER
 */

#define ARCHI_HAS_CLUSTER          1
#define ARCHI_L1_TAS_BIT    20
#define ARCHI_CLUSTER_NB_PE 1




#endif
