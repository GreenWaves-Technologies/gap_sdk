
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

#ifndef __ARCHI_HWCE_MACROS__
#define __ARCHI_HWCE_MACROS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include "archi/gap_utils.h"

#endif




//
// REGISTERS FIELDS MACROS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#define HWCE_TRIGGER_ANY_GET(value)                        (GAP_BEXTRACTU((value),32,0))
#define HWCE_TRIGGER_ANY_GETS(value)                       (GAP_BEXTRACT((value),32,0))
#define HWCE_TRIGGER_ANY_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
#define HWCE_TRIGGER_ANY(val)                              ((val) << 0)

#define HWCE_ACQUIRE_ID_ERR_GET(value)                     (GAP_BEXTRACTU((value),8,0))
#define HWCE_ACQUIRE_ID_ERR_GETS(value)                    (GAP_BEXTRACT((value),8,0))
#define HWCE_ACQUIRE_ID_ERR_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
#define HWCE_ACQUIRE_ID_ERR(val)                           ((val) << 0)

#define HWCE_ACQUIRE_ERR_GET(value)                        (GAP_BEXTRACTU((value),24,8))
#define HWCE_ACQUIRE_ERR_GETS(value)                       (GAP_BEXTRACT((value),24,8))
#define HWCE_ACQUIRE_ERR_SET(value,field)                  (GAP_BINSERT((value),(field),24,8))
#define HWCE_ACQUIRE_ERR(val)                              ((val) << 8)

#define HWCE_FINISHED_JOBS_JOBS_GET(value)                 (GAP_BEXTRACTU((value),32,0))
#define HWCE_FINISHED_JOBS_JOBS_GETS(value)                (GAP_BEXTRACT((value),32,0))
#define HWCE_FINISHED_JOBS_JOBS_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
#define HWCE_FINISHED_JOBS_JOBS(val)                       ((val) << 0)

#define HWCE_STATUS_ST_GET(value)                          (GAP_BEXTRACTU((value),1,0))
#define HWCE_STATUS_ST_GETS(value)                         (GAP_BEXTRACT((value),1,0))
#define HWCE_STATUS_ST_SET(value,field)                    (GAP_BINSERT((value),(field),1,0))
#define HWCE_STATUS_ST(val)                                ((val) << 0)

#define HWCE_RUNNING_JOB_ID_GET(value)                     (GAP_BEXTRACTU((value),8,0))
#define HWCE_RUNNING_JOB_ID_GETS(value)                    (GAP_BEXTRACT((value),8,0))
#define HWCE_RUNNING_JOB_ID_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
#define HWCE_RUNNING_JOB_ID(val)                           ((val) << 0)

#define HWCE_SOFT_CLEAR_ANY_GET(value)                     (GAP_BEXTRACTU((value),32,0))
#define HWCE_SOFT_CLEAR_ANY_GETS(value)                    (GAP_BEXTRACT((value),32,0))
#define HWCE_SOFT_CLEAR_ANY_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
#define HWCE_SOFT_CLEAR_ANY(val)                           ((val) << 0)

#define HWCE_GEN_CONFIG0_QF_GET(value)                     (GAP_BEXTRACTU((value),6,0))
#define HWCE_GEN_CONFIG0_QF_GETS(value)                    (GAP_BEXTRACT((value),6,0))
#define HWCE_GEN_CONFIG0_QF_SET(value,field)               (GAP_BINSERT((value),(field),6,0))
#define HWCE_GEN_CONFIG0_QF(val)                           ((val) << 0)

#define HWCE_GEN_CONFIG0_NF_GET(value)                     (GAP_BEXTRACTU((value),1,6))
#define HWCE_GEN_CONFIG0_NF_GETS(value)                    (GAP_BEXTRACT((value),1,6))
#define HWCE_GEN_CONFIG0_NF_SET(value,field)               (GAP_BINSERT((value),(field),1,6))
#define HWCE_GEN_CONFIG0_NF(val)                           ((val) << 6)

#define HWCE_GEN_CONFIG0_NY_GET(value)                     (GAP_BEXTRACTU((value),1,7))
#define HWCE_GEN_CONFIG0_NY_GETS(value)                    (GAP_BEXTRACT((value),1,7))
#define HWCE_GEN_CONFIG0_NY_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
#define HWCE_GEN_CONFIG0_NY(val)                           ((val) << 7)

#define HWCE_GEN_CONFIG0_UNS_GET(value)                    (GAP_BEXTRACTU((value),1,8))
#define HWCE_GEN_CONFIG0_UNS_GETS(value)                   (GAP_BEXTRACT((value),1,8))
#define HWCE_GEN_CONFIG0_UNS_SET(value,field)              (GAP_BINSERT((value),(field),1,8))
#define HWCE_GEN_CONFIG0_UNS(val)                          ((val) << 8)

#define HWCE_GEN_CONFIG0_VECT_GET(value)                   (GAP_BEXTRACTU((value),2,9))
#define HWCE_GEN_CONFIG0_VECT_GETS(value)                  (GAP_BEXTRACT((value),2,9))
#define HWCE_GEN_CONFIG0_VECT_SET(value,field)             (GAP_BINSERT((value),(field),2,9))
#define HWCE_GEN_CONFIG0_VECT(val)                         ((val) << 9)

#define HWCE_GEN_CONFIG0_CONV_GET(value)                   (GAP_BEXTRACTU((value),2,11))
#define HWCE_GEN_CONFIG0_CONV_GETS(value)                  (GAP_BEXTRACT((value),2,11))
#define HWCE_GEN_CONFIG0_CONV_SET(value,field)             (GAP_BINSERT((value),(field),2,11))
#define HWCE_GEN_CONFIG0_CONV(val)                         ((val) << 11)

#define HWCE_GEN_CONFIG0_NCP_GET(value)                    (GAP_BEXTRACTU((value),1,13))
#define HWCE_GEN_CONFIG0_NCP_GETS(value)                   (GAP_BEXTRACT((value),1,13))
#define HWCE_GEN_CONFIG0_NCP_SET(value,field)              (GAP_BINSERT((value),(field),1,13))
#define HWCE_GEN_CONFIG0_NCP(val)                          ((val) << 13)

#define HWCE_GEN_CONFIG0_RND_GET(value)                    (GAP_BEXTRACTU((value),1,14))
#define HWCE_GEN_CONFIG0_RND_GETS(value)                   (GAP_BEXTRACT((value),1,14))
#define HWCE_GEN_CONFIG0_RND_SET(value,field)              (GAP_BINSERT((value),(field),1,14))
#define HWCE_GEN_CONFIG0_RND(val)                          ((val) << 14)

#define HWCE_GEN_CONFIG0_WSTRIDE_GET(value)                (GAP_BEXTRACTU((value),16,16))
#define HWCE_GEN_CONFIG0_WSTRIDE_GETS(value)               (GAP_BEXTRACT((value),16,16))
#define HWCE_GEN_CONFIG0_WSTRIDE_SET(value,field)          (GAP_BINSERT((value),(field),16,16))
#define HWCE_GEN_CONFIG0_WSTRIDE(val)                      ((val) << 16)

#define HWCE_GEN_CONFIG1_PIXSHIFTL_GET(value)              (GAP_BEXTRACTU((value),5,0))
#define HWCE_GEN_CONFIG1_PIXSHIFTL_GETS(value)             (GAP_BEXTRACT((value),5,0))
#define HWCE_GEN_CONFIG1_PIXSHIFTL_SET(value,field)        (GAP_BINSERT((value),(field),5,0))
#define HWCE_GEN_CONFIG1_PIXSHIFTL(val)                    ((val) << 0)

#define HWCE_GEN_CONFIG1_PIXMODE_GET(value)                (GAP_BEXTRACTU((value),2,8))
#define HWCE_GEN_CONFIG1_PIXMODE_GETS(value)               (GAP_BEXTRACT((value),2,8))
#define HWCE_GEN_CONFIG1_PIXMODE_SET(value,field)          (GAP_BINSERT((value),(field),2,8))
#define HWCE_GEN_CONFIG1_PIXMODE(val)                      ((val) << 8)

#define HWCE_GEN_CONFIG1_PIXSHIFTR_GET(value)              (GAP_BEXTRACTU((value),5,16))
#define HWCE_GEN_CONFIG1_PIXSHIFTR_GETS(value)             (GAP_BEXTRACT((value),5,16))
#define HWCE_GEN_CONFIG1_PIXSHIFTR_SET(value,field)        (GAP_BINSERT((value),(field),5,16))
#define HWCE_GEN_CONFIG1_PIXSHIFTR(val)                    ((val) << 16)

#define HWCE_Y_TRANS_SIZE_SIZE_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_TRANS_SIZE_SIZE_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_TRANS_SIZE_SIZE_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_TRANS_SIZE_SIZE(val)                        ((val) << 0)

#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_GET(value)        (GAP_BEXTRACTU((value),16,0))
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_GETS(value)       (GAP_BEXTRACT((value),16,0))
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
#define HWCE_Y_LINE_STRIDE_LENGTH_LENGTH(val)              ((val) << 0)

#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_GET(value)        (GAP_BEXTRACTU((value),16,16))
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_GETS(value)       (GAP_BEXTRACT((value),16,16))
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE_SET(value,field)  (GAP_BINSERT((value),(field),16,16))
#define HWCE_Y_LINE_STRIDE_LENGTH_STRIDE(val)              ((val) << 16)

#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_GET(value)        (GAP_BEXTRACTU((value),16,0))
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_GETS(value)       (GAP_BEXTRACT((value),16,0))
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
#define HWCE_Y_FEAT_STRIDE_LENGTH_LENGTH(val)              ((val) << 0)

#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_GET(value)        (GAP_BEXTRACTU((value),16,16))
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_GETS(value)       (GAP_BEXTRACT((value),16,16))
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE_SET(value,field)  (GAP_BINSERT((value),(field),16,16))
#define HWCE_Y_FEAT_STRIDE_LENGTH_STRIDE(val)              ((val) << 16)

#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_GET(value)             (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_GETS(value)            (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_OUT_3_BASE_ADDR_ADDR(val)                   ((val) << 0)

#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_GET(value)             (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_GETS(value)            (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_OUT_2_BASE_ADDR_ADDR(val)                   ((val) << 0)

#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_GET(value)             (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_GETS(value)            (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_OUT_1_BASE_ADDR_ADDR(val)                   ((val) << 0)

#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_GET(value)             (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_GETS(value)            (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_OUT_0_BASE_ADDR_ADDR(val)                   ((val) << 0)

#define HWCE_Y_IN_3_BASE_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_IN_3_BASE_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_IN_3_BASE_ADDR_ADDR(val)                    ((val) << 0)

#define HWCE_Y_IN_2_BASE_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_IN_2_BASE_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_IN_2_BASE_ADDR_ADDR(val)                    ((val) << 0)

#define HWCE_Y_IN_1_BASE_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_IN_1_BASE_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_IN_1_BASE_ADDR_ADDR(val)                    ((val) << 0)

#define HWCE_Y_IN_0_BASE_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
#define HWCE_Y_IN_0_BASE_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
#define HWCE_Y_IN_0_BASE_ADDR_ADDR(val)                    ((val) << 0)

#define HWCE_X_TRANS_SIZE_SIZE_GET(value)                  (GAP_BEXTRACTU((value),32,0))
#define HWCE_X_TRANS_SIZE_SIZE_GETS(value)                 (GAP_BEXTRACT((value),32,0))
#define HWCE_X_TRANS_SIZE_SIZE_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
#define HWCE_X_TRANS_SIZE_SIZE(val)                        ((val) << 0)

#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_GET(value)        (GAP_BEXTRACTU((value),16,0))
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_GETS(value)       (GAP_BEXTRACT((value),16,0))
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
#define HWCE_X_LINE_STRIDE_LENGTH_LENGTH(val)              ((val) << 0)

#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_GET(value)        (GAP_BEXTRACTU((value),16,16))
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_GETS(value)       (GAP_BEXTRACT((value),16,16))
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE_SET(value,field)  (GAP_BINSERT((value),(field),16,16))
#define HWCE_X_LINE_STRIDE_LENGTH_STRIDE(val)              ((val) << 16)

#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_GET(value)        (GAP_BEXTRACTU((value),16,0))
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_GETS(value)       (GAP_BEXTRACT((value),16,0))
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
#define HWCE_X_FEAT_STRIDE_LENGTH_LENGTH(val)              ((val) << 0)

#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_GET(value)        (GAP_BEXTRACTU((value),16,16))
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_GETS(value)       (GAP_BEXTRACT((value),16,16))
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE_SET(value,field)  (GAP_BINSERT((value),(field),16,16))
#define HWCE_X_FEAT_STRIDE_LENGTH_STRIDE(val)              ((val) << 16)

#define HWCE_X_IN_BASE_ADDR_ADDR_GET(value)                (GAP_BEXTRACTU((value),32,0))
#define HWCE_X_IN_BASE_ADDR_ADDR_GETS(value)               (GAP_BEXTRACT((value),32,0))
#define HWCE_X_IN_BASE_ADDR_ADDR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
#define HWCE_X_IN_BASE_ADDR_ADDR(val)                      ((val) << 0)

#define HWCE_W_BASE_ADDR_ADDR_GET(value)                   (GAP_BEXTRACTU((value),32,0))
#define HWCE_W_BASE_ADDR_ADDR_GETS(value)                  (GAP_BEXTRACT((value),32,0))
#define HWCE_W_BASE_ADDR_ADDR_SET(value,field)             (GAP_BINSERT((value),(field),32,0))
#define HWCE_W_BASE_ADDR_ADDR(val)                         ((val) << 0)

#define HWCE_JOB_CONFIG0_LBUFLEN_GET(value)                (GAP_BEXTRACTU((value),10,0))
#define HWCE_JOB_CONFIG0_LBUFLEN_GETS(value)               (GAP_BEXTRACT((value),10,0))
#define HWCE_JOB_CONFIG0_LBUFLEN_SET(value,field)          (GAP_BINSERT((value),(field),10,0))
#define HWCE_JOB_CONFIG0_LBUFLEN(val)                      ((val) << 0)

#define HWCE_JOB_CONFIG0_NOYCONST_GET(value)               (GAP_BEXTRACTU((value),16,16))
#define HWCE_JOB_CONFIG0_NOYCONST_GETS(value)              (GAP_BEXTRACT((value),16,16))
#define HWCE_JOB_CONFIG0_NOYCONST_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
#define HWCE_JOB_CONFIG0_NOYCONST(val)                     ((val) << 16)

#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_GET(value)      (GAP_BEXTRACTU((value),4,0))
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_GETS(value)     (GAP_BEXTRACT((value),4,0))
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK_SET(value,field) (GAP_BINSERT((value),(field),4,0))
#define HWCE_JOB_CONFIG1_VECT_DISABLE_MASK(val)            ((val) << 0)

#define HWCE_JOB_CONFIG1_WOF_PARAM_GET(value)              (GAP_BEXTRACTU((value),6,8))
#define HWCE_JOB_CONFIG1_WOF_PARAM_GETS(value)             (GAP_BEXTRACT((value),6,8))
#define HWCE_JOB_CONFIG1_WOF_PARAM_SET(value,field)        (GAP_BINSERT((value),(field),6,8))
#define HWCE_JOB_CONFIG1_WOF_PARAM(val)                    ((val) << 8)

#define HWCE_JOB_CONFIG1_WIF_PARAM_GET(value)              (GAP_BEXTRACTU((value),6,16))
#define HWCE_JOB_CONFIG1_WIF_PARAM_GETS(value)             (GAP_BEXTRACT((value),6,16))
#define HWCE_JOB_CONFIG1_WIF_PARAM_SET(value,field)        (GAP_BINSERT((value),(field),6,16))
#define HWCE_JOB_CONFIG1_WIF_PARAM(val)                    ((val) << 16)

#define HWCE_JOB_CONFIG1_LO_GET(value)                     (GAP_BEXTRACTU((value),1,24))
#define HWCE_JOB_CONFIG1_LO_GETS(value)                    (GAP_BEXTRACT((value),1,24))
#define HWCE_JOB_CONFIG1_LO_SET(value,field)               (GAP_BINSERT((value),(field),1,24))
#define HWCE_JOB_CONFIG1_LO(val)                           ((val) << 24)

#define HWCE_JOB_CONFIG1_LN_GET(value)                     (GAP_BEXTRACTU((value),1,25))
#define HWCE_JOB_CONFIG1_LN_GETS(value)                    (GAP_BEXTRACT((value),1,25))
#define HWCE_JOB_CONFIG1_LN_SET(value,field)               (GAP_BINSERT((value),(field),1,25))
#define HWCE_JOB_CONFIG1_LN(val)                           ((val) << 25)

#endif

#endif
