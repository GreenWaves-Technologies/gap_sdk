/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __ARCHI_FULMINE_APB_SOC_H__
#define __ARCHI_FULMINE_APB_SOC_H__
 
#define APB_SOC_INFO_OFFSET          0x10
#define APB_SOC_INFO_CORES_OFFSET    (APB_SOC_INFO_OFFSET + 2)
#define APB_SOC_INFO_CLUSTERS_OFFSET (APB_SOC_INFO_OFFSET)
 
#define APB_SOC_NB_CORE_BIT     16

#define SOC_CTRL_PADFUN0              ( ARCHI_APB_SOC_CTRL_ADDR + 0x00 )
#define SOC_CTRL_PADFUN1              ( ARCHI_APB_SOC_CTRL_ADDR + 0x04 )


#define ARCHI_APB_SOC_PADFUN_NB 2


#endif