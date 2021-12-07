
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __INCLUDE_ARCHI_CHIPS_VEGA_PMU_H__
#define __INCLUDE_ARCHI_CHIPS_VEGA_PMU_H__

#ifndef LANGUAGE_ASSEMBLY

#include <stdint.h>
#include "archi/utils.h"

#endif




//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_pmu_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// GROUP icu0
//

#define PMU_ICU0_OFFSET                          0x2



//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_icu0_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// GROUP icu1
//

#define PMU_ICU1_OFFSET                          0x3



//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_icu1_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// GROUP dmu0
//

#define PMU_DMU0_OFFSET                          0x4



//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_dmu0_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// GROUP icu_soc
//



//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_icu_soc_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// CUSTOM FIELDS
//
#define PMU_SOC_ON_NV 0x00
#define PMU_SOC_ON_LV 0x01
#define PMU_SOC_RET_OFF 0x02
#define PMU_SOC_RET_NV 0x03
#define PMU_SOC_RET_LV 0x04
#define PMU_SOC_RET_RV 0x05
#define PMU_SOC_EXT_OFF 0x06
#define PMU_SOC_EXT_NV 0x07
#define PMU_SOC_EXT_LV 0x08
#define PMU_SOC_EXT_RV 0x09
#define PMU_SOC_CKOFF_NV 0x11
#define PMU_SOC_CKOFF_LV 0x12
#define PMU_SOC_CKOFF_RV 0x13



//
// GROUP icu_cluster
//



//
// REGISTERS
//



//
// REGISTERS FIELDS
//



//
// REGISTERS STRUCTS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS STRUCTS
//

#ifdef __GVSOC__

#endif



//
// REGISTERS GLOBAL STRUCT
//

#ifndef LANGUAGE_ASSEMBLY

typedef struct {
} __attribute__((packed)) pmu_icu_cluster_t;

#endif



//
// REGISTERS ACCESS FUNCTIONS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// REGISTERS FIELDS MACROS
//

#ifndef LANGUAGE_ASSEMBLY

#endif



//
// CUSTOM FIELDS
//
#define PMU_CLUSTER_ON_NV 0x00
#define PMU_CLUSTER_ON_LV 0x01
#define PMU_CLUSTER_RET_OFF 0x02
#define PMU_CLUSTER_RET_NV 0x03
#define PMU_CLUSTER_RET_LV 0x04
#define PMU_CLUSTER_RET_RV 0x05
#define PMU_CLUSTER_EXT_OFF 0x06
#define PMU_CLUSTER_EXT_NV 0x07
#define PMU_CLUSTER_EXT_LV 0x08
#define PMU_CLUSTER_EXT_RV 0x09
#define PMU_CLUSTER_CKOFF_NV 0x11
#define PMU_CLUSTER_CKOFF_LV 0x12
#define PMU_CLUSTER_CKOFF_RV 0x13



//
// CUSTOM FIELDS
//
#define PMU_BOOT 0x01
#define PMU_SOC_ACTIVE_NV 0x01
#define PMU_SOC_ACTIVE_LV 0x02
#define PMU_SOC_CLUSTER_ACTIVE_NV 0x04
#define PMU_SOC_CLUSTER_ACTIVE_LV 0x08
#define PMU_DEEP_SLEEP_ALL_OFF 0x10
#define PMU_DEEP_SLEEP_RETENTIVE 0x20

#endif
