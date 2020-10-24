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


#ifndef __ARCHI_CHIPS_HONEY_MEMORY_MAP_H__
#define __ARCHI_CHIPS_HONEY_MEMORY_MAP_H__



/*
 * L2
 */

#define ARCHI_L2_ADDR                 0x1c000000



/*
 * SOC PERIPHERALS
 */

#define ARCHI_SOC_PERIPHERALS_ADDR    0x1A100000


#define ARCHI_FLL_OFFSET              0x00000000
#define ARCHI_GPIO_OFFSET             0x00001000
#define ARCHI_UART_OFFSET             0x00003000
#define ARCHI_APB_SOC_CTRL_OFFSET     0x00004000
#define ARCHI_STDOUT_OFFSET           0x00010000



#define ARCHI_FLL_ADDR               ( ARCHI_SOC_PERIPHERALS_ADDR + ARCHI_FLL_OFFSET )
#define ARCHI_GPIO_ADDR              ( ARCHI_SOC_PERIPHERALS_ADDR + ARCHI_GPIO_OFFSET )
#define ARCHI_UART_ADDR              ( ARCHI_SOC_PERIPHERALS_ADDR + ARCHI_UART_OFFSET )
#define ARCHI_APB_SOC_CTRL_ADDR      ( ARCHI_SOC_PERIPHERALS_ADDR + ARCHI_APB_SOC_CTRL_OFFSET )
#define ARCHI_STDOUT_ADDR            ( ARCHI_SOC_PERIPHERALS_ADDR + ARCHI_STDOUT_OFFSET )




/*
 * CLUSTER
 */

#define ARCHI_CLUSTER_ADDR              0x10000000
#define ARCHI_CLUSTER_SIZE              0x00400000
#define ARCHI_CLUSTER_GLOBAL_ADDR(cid)  (0x10000000 + (cid)*ARCHI_CLUSTER_SIZE)



/*
 * CLUSTER PERIPHERALS
 */

#define ARCHI_CLUSTER_PERIPHERALS_OFFSET 0x00200000

#define ARCHI_CLUSTER_CTRL_OFFSET        0x00000000
#define ARCHI_TIMER_OFFSET               0x00000400
#define ARCHI_EU_OFFSET                  0x00000800
#define ARCHI_ICACHE_CTRL_OFFSET         0x00001400


#define ARCHI_CLUSTER_PERIPHERALS_ADDR             ( ARCHI_CLUSTER_ADDR + ARCHI_CLUSTER_PERIPHERALS_OFFSET )
#define ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) ( ARCHI_CLUSTER_GLOBAL_ADDR(cid) + ARCHI_CLUSTER_PERIPHERALS_OFFSET )


#define ARCHI_CLUSTER_CTRL_ADDR        ( ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_CLUSTER_CTRL_OFFSET )
#define ARCHI_ICACHE_CTRL_ADDR         ( ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_ICACHE_CTRL_OFFSET )
#define ARCHI_TIMER_ADDR               ( ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET )
#define ARCHI_EU_ADDR                  ( ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_EU_OFFSET )



#define ARCHI_DEMUX_OFFSET            0x00004000

#define ARCHI_DEMUX_ADDR              ( ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_DEMUX_OFFSET )

#endif
