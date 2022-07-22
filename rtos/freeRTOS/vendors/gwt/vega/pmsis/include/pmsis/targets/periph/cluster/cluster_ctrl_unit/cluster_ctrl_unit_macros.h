
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

#ifndef __ARCHI_CLUSTER_CTRL_UNIT_MACROS__
#define __ARCHI_CLUSTER_CTRL_UNIT_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define CLUSTER_CTRL_UNIT_EOC_EOC_GET(value)               (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_EOC_EOC_GETS(value)              (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_EOC_EOC_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_EOC_EOC(val)                     ((val) << 0)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GET(value)        (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GETS(value)       (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0(val)              ((val) << 0)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GET(value)        (GAP_BEXTRACTU((value),1,1))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GETS(value)       (GAP_BEXTRACT((value),1,1))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1(val)              ((val) << 1)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GET(value)        (GAP_BEXTRACTU((value),1,2))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GETS(value)       (GAP_BEXTRACT((value),1,2))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SET(value,field)  (GAP_BINSERT((value),(field),1,2))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2(val)              ((val) << 2)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GET(value)        (GAP_BEXTRACTU((value),1,3))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GETS(value)       (GAP_BEXTRACT((value),1,3))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SET(value,field)  (GAP_BINSERT((value),(field),1,3))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3(val)              ((val) << 3)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GET(value)        (GAP_BEXTRACTU((value),1,4))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GETS(value)       (GAP_BEXTRACT((value),1,4))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4(val)              ((val) << 4)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GET(value)        (GAP_BEXTRACTU((value),1,5))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GETS(value)       (GAP_BEXTRACT((value),1,5))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SET(value,field)  (GAP_BINSERT((value),(field),1,5))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5(val)              ((val) << 5)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GET(value)        (GAP_BEXTRACTU((value),1,6))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GETS(value)       (GAP_BEXTRACT((value),1,6))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SET(value,field)  (GAP_BINSERT((value),(field),1,6))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6(val)              ((val) << 6)

#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GET(value)        (GAP_BEXTRACTU((value),1,7))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GETS(value)       (GAP_BEXTRACT((value),1,7))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
#define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7(val)              ((val) << 7)

#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GET(value)         (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GETS(value)        (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SET(value,field)   (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(val)               ((val) << 0)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GET(value)      (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GETS(value)     (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0(val)            ((val) << 0)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GET(value)      (GAP_BEXTRACTU((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GETS(value)     (GAP_BEXTRACT((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1(val)            ((val) << 1)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GET(value)      (GAP_BEXTRACTU((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GETS(value)     (GAP_BEXTRACT((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2(val)            ((val) << 2)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GET(value)      (GAP_BEXTRACTU((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GETS(value)     (GAP_BEXTRACT((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3(val)            ((val) << 3)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GET(value)      (GAP_BEXTRACTU((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GETS(value)     (GAP_BEXTRACT((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4(val)            ((val) << 4)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GET(value)      (GAP_BEXTRACTU((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GETS(value)     (GAP_BEXTRACT((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5(val)            ((val) << 5)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GET(value)      (GAP_BEXTRACTU((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GETS(value)     (GAP_BEXTRACT((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6(val)            ((val) << 6)

#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GET(value)      (GAP_BEXTRACTU((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GETS(value)     (GAP_BEXTRACT((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
#define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7(val)            ((val) << 7)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GET(value) (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GETS(value) (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0(val)       ((val) << 0)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GET(value) (GAP_BEXTRACTU((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GETS(value) (GAP_BEXTRACT((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1(val)       ((val) << 1)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GET(value) (GAP_BEXTRACTU((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GETS(value) (GAP_BEXTRACT((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2(val)       ((val) << 2)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GET(value) (GAP_BEXTRACTU((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GETS(value) (GAP_BEXTRACT((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3(val)       ((val) << 3)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GET(value) (GAP_BEXTRACTU((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GETS(value) (GAP_BEXTRACT((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4(val)       ((val) << 4)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GET(value) (GAP_BEXTRACTU((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GETS(value) (GAP_BEXTRACT((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5(val)       ((val) << 5)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GET(value) (GAP_BEXTRACTU((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GETS(value) (GAP_BEXTRACT((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6(val)       ((val) << 6)

#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GET(value) (GAP_BEXTRACTU((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GETS(value) (GAP_BEXTRACT((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7(val)       ((val) << 7)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GET(value)   (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GETS(value)  (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0(val)         ((val) << 0)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GET(value)   (GAP_BEXTRACTU((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GETS(value)  (GAP_BEXTRACT((value),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1(val)         ((val) << 1)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GET(value)   (GAP_BEXTRACTU((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GETS(value)  (GAP_BEXTRACT((value),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2(val)         ((val) << 2)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GET(value)   (GAP_BEXTRACTU((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GETS(value)  (GAP_BEXTRACT((value),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3(val)         ((val) << 3)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GET(value)   (GAP_BEXTRACTU((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GETS(value)  (GAP_BEXTRACT((value),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4(val)         ((val) << 4)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GET(value)   (GAP_BEXTRACTU((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GETS(value)  (GAP_BEXTRACT((value),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5(val)         ((val) << 5)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GET(value)   (GAP_BEXTRACTU((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GETS(value)  (GAP_BEXTRACT((value),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6(val)         ((val) << 6)

#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GET(value)   (GAP_BEXTRACTU((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GETS(value)  (GAP_BEXTRACT((value),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
#define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7(val)         ((val) << 7)

#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GET(value)         (GAP_BEXTRACTU((value),32,0))
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GETS(value)        (GAP_BEXTRACT((value),32,0))
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
#define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA(val)               ((val) << 0)

#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL(val)     ((val) << 0)

#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL(val)     ((val) << 0)

#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL(val) ((val) << 0)

#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
#define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL(val) ((val) << 0)

#endif

#endif
