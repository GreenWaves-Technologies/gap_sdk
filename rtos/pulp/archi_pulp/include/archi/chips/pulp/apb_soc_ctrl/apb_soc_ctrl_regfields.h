
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_APB_SOC_CTRL_REGFIELD__
#define __ARCHI_APB_SOC_CTRL_REGFIELD__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)


#endif




//
// REGISTERS FIELDS
//

// Number of clusters (access: R)
#define APB_SOC_CTRL_INFO_NB_CL_BIT                                  0
#define APB_SOC_CTRL_INFO_NB_CL_WIDTH                                16
#define APB_SOC_CTRL_INFO_NB_CL_MASK                                 0xffff
#define APB_SOC_CTRL_INFO_NB_CL_RESET                                0x8

// Number of cores (access: R)
#define APB_SOC_CTRL_INFO_NB_CORES_BIT                               16
#define APB_SOC_CTRL_INFO_NB_CORES_WIDTH                             16
#define APB_SOC_CTRL_INFO_NB_CORES_MASK                              0xffff0000
#define APB_SOC_CTRL_INFO_NB_CORES_RESET                             0x1

// FC Boot Address (access: R/W)
#define APB_SOC_CTRL_FC_BOOT_ADDR_BIT                                0
#define APB_SOC_CTRL_FC_BOOT_ADDR_WIDTH                              32
#define APB_SOC_CTRL_FC_BOOT_ADDR_MASK                               0xffffffff
#define APB_SOC_CTRL_FC_BOOT_ADDR_RESET                              0x1a000000

// FC Fetch Enable (access: R/W)
#define APB_SOC_CTRL_FC_FETCH_FC_FE_BIT                              0
#define APB_SOC_CTRL_FC_FETCH_FC_FE_WIDTH                            1
#define APB_SOC_CTRL_FC_FETCH_FC_FE_MASK                             0x1
#define APB_SOC_CTRL_FC_FETCH_FC_FE_RESET                            0x0

// Chip status register. The SW can store the exit value value so that the external loader can get it. (access: R/W)
#define APB_SOC_CTRL_CORESTATUS_STATUS_BIT                           0
#define APB_SOC_CTRL_CORESTATUS_STATUS_WIDTH                         32
#define APB_SOC_CTRL_CORESTATUS_STATUS_MASK                          0xffffffff
#define APB_SOC_CTRL_CORESTATUS_STATUS_RESET                         0x0

// End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished. (access: R/W)
#define APB_SOC_CTRL_CORESTATUS_EOC_BIT                              31
#define APB_SOC_CTRL_CORESTATUS_EOC_WIDTH                            1
#define APB_SOC_CTRL_CORESTATUS_EOC_MASK                             0x80000000
#define APB_SOC_CTRL_CORESTATUS_EOC_RESET                            0x0

#endif
