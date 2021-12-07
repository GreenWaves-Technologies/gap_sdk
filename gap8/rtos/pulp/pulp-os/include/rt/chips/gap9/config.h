/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __RT__CHIPS__GAP9__CONFIG_H__
#define __RT__CHIPS__GAP9__CONFIG_H__

#include "archi/pulp_defs.h"

#define ARCHI_HAS_FC 1
#define PLP_HAS_FC 1
#define PLP_NO_BUILTIN 1
#define __RT_ALLOC_L2_MULTI 1
#define ARCHI_NB_PE 9
#define __BENCH__ 1
#define BENCH 1
#define PULP_RT_VERSION_RELEASE 0
#define PULP_RT_VERSION_BENCH 1
#define PULP_RT_VERSION_PROFILE0 2
#define PULP_RT_VERSION_PROFILE1 3
#define PULP_RT_VERSION_DEBUG 4
#define PULP_RT_VERSION PULP_RT_VERSION_BENCH
#define PULP
#define __PULP__
#define __ri5cy__ 1
#define PULP_CHIP CHIP_GAP9
#define CONFIG_GAP9 1
#define PULP_CHIP_STR gap9
#define PULP_CHIP_FAMILY CHIP_GAP9
#define PULP_CHIP_FAMILY_STR gap9
#define PULP_CHIP_VERSION 0
#define PULP_CORE CORE_RISCV_V4
#define FP_SW_EMUL 1
#define ARCHI_CORE_HAS_PULPV2 1
#define CORE_PULP_BUILTINS 1
#define ARCHI_CORE_HAS_1_10 1
#define ARCHI_CORE_HAS_PULPV2 1
#define ARCHI_CORE_HAS_CPLX 1
#define ARCHI_CORE_HAS_SECURITY 1
#define ARCHI_CORE_HAS_1_10 1

#endif
