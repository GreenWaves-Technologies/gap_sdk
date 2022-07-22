/*
 * Copyright (C) 2020 ETH Zurich, University of Bologna and GreenWaves Technologies
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

/*
 * Authors:  Francesco Conti <fconti@iis.ee.ethz.ch>
 *           Gianna Paulin <pauling@iis.ee.ethz.ch>
 *           Renzo Andri <andrire@iis.ee.ethz.ch>
 * Main Test Program for the NE16
 */

#include "pmsis.h"
#include "pmsis/cluster/dma/cl_dma.h"
#include <stdint.h>
#include <stdio.h>

#include "hal_ne16.h"
#include "inc/ne16_cfg.h"
#include "inc/ne16_infeat.h"
#include "inc/ne16_weights.h"
#include "inc/ne16_scale.h"
#include "inc/ne16_scale_bias.h"
#include "inc/ne16_scale_shift.h"
#include "inc/ne16_streamin.h"
#include "inc/ne16_outfeat.h"

#include "interference.h"
#include "lfsr32.h"

static int glob_errors;

static inline ne16_c0_config() {
  NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE,  1);    // 1 channel
  NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE,  28);   // 28 pixels x 1 channel
  NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE,  140);  // 5x28 pixels x 1 channel
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, 16);   // unused in 8-bit output mode 
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, 16);   // 32 channels (half as usual to take into account stridedness) --> HIDDEN CONSTRAINT?? FIXME
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, 208);  // 13 pixels x 32 channels (half as usual to take into account stridedness) --> HIDDEN CONSTRAINT?? FIXME
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, 18);   // 9 filters x 2 byte (16 channels)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, 144);  // 8 weight bits x 9 filters x 2 byte (16 channels)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, 0);
  NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((1 & NE16_MASK_REM_KI) << NE16_SHIFT_REM_KI) | ((32 & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO));
  NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((0 & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | ((0 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
  NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((0 & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | ((0 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
  NE16_WRITE_REG(NE16_REG_NB_KO_KI,  ((1 & NE16_MASK_NB_KI)  << NE16_SHIFT_NB_KI)  | ((1 & NE16_MASK_NB_KO)   << NE16_SHIFT_NB_KO));
  NE16_WRITE_REG(NE16_REG_NB_HO_WO,  ((1 & NE16_MASK_NB_WO)  << NE16_SHIFT_NB_WO)  | ((1 & NE16_MASK_NB_HO)   << NE16_SHIFT_NB_HO));
  NE16_WRITE_REG(NE16_REG_PADDING, ((0 & NE16_MASK_PADDING_TOP) << NE16_SHIFT_PADDING_TOP) | ((0 & NE16_MASK_PADDING_RIGHT) << NE16_SHIFT_PADDING_RIGHT) | ((0 & NE16_MASK_PADDING_BOTTOM) << NE16_SHIFT_PADDING_BOTTOM) | ((0 & NE16_MASK_PADDING_LEFT) << NE16_SHIFT_PADDING_LEFT));
  NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, -119); // bottom asymmetric weight value
  NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((0 & NE16_MASK_FILTER_MASK_TOP) << NE16_SHIFT_FILTER_MASK_TOP) | ((0 & NE16_MASK_FILTER_MASK_RIGHT) << NE16_SHIFT_FILTER_MASK_RIGHT) | ((0 & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM) | ((0 & NE16_MASK_FILTER_MASK_LEFT) << NE16_SHIFT_FILTER_MASK_LEFT));
  NE16_WRITE_REG(NE16_REG_CONFIG, 0);
  NE16_WRITE_REG(NE16_REG_CONFIG, ((7 & NE16_MASK_WBITS_M1)           << NE16_SHIFT_WBITS_M1)          |
                                  ((0 & NE16_MASK_MODE16)             << NE16_SHIFT_MODE16)            |
                                  ((1 & NE16_MASK_OUTQUANT)           << NE16_SHIFT_OUTQUANT)          |
                                  ((0 & NE16_MASK_FILTER_MODE)        << NE16_SHIFT_FILTER_MODE)       | // 3x3 mode
                                  ((0 & NE16_MASK_LINEAR_MODE)        << NE16_SHIFT_LINEAR_MODE)       |
                                  ((1 & NE16_MASK_STRIDED_MODE)       << NE16_SHIFT_STRIDED_MODE)      | // 2x2 strided mode
                                  ((0 & NE16_MASK_NORM_BITS)          << NE16_SHIFT_NORM_BITS)         |
                                  ((0 & NE16_MASK_STREAMIN)           << NE16_SHIFT_STREAMIN)          |
                                  ((1 & NE16_MASK_WEIGHT_OFFSET_CFG)  << NE16_SHIFT_WEIGHT_OFFSET_CFG) |
                                  ((13 & NE16_MASK_QUANT_RIGHT_SHIFT) << NE16_SHIFT_QUANT_RIGHT_SHIFT) |
                                  ((0 & NE16_MASK_QUANT_BITS)         << NE16_SHIFT_QUANT_BITS)        |
                                  ((0 & NE16_MASK_QUANT_NORECT)       << NE16_SHIFT_QUANT_NORECT)      |
                                  ((0 & NE16_MASK_NORM_SHIFT)         << NE16_SHIFT_NORM_SHIFT)        |
                                  ((0 & NE16_MASK_NORM_BIAS)          << NE16_SHIFT_NORM_BIAS));
  NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR,     ne16_weights_c0);
  NE16_WRITE_REG(NE16_REG_SCALE_PTR,       ne16_scale_c0);
  NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, NULL);
  NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR,  NULL);
}

static inline ne16_c1_config() {
  NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE,  32);  // 32 channels
  NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE,  416); // 13 pixels x 32 channels
  NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE,  800); // 5x5 pixels x 32 channels // 5x13 pixels x 32 channels FIXME DOES IT MAKE SENSE?
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, 0);   // unused in 8-bit output mode
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, 32);  // 32 channels
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, 352); // 11 pixels x 32 channels //?
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, 18);   // 9 filters x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, 0);  // 8 weight bits x 9 filters x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, 0);  // 2 channels (major) x 8 weight bits x 9 filters x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((16 & NE16_MASK_REM_KI) << NE16_SHIFT_REM_KI) | ((16 & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO));
  NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((2 & NE16_MASK_REM_WO)  << NE16_SHIFT_REM_WO) | ((2 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO)); // 2 rows/cols of remainder
  NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((4 & NE16_MASK_REM_WI)  << NE16_SHIFT_REM_WI) | ((4 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI)); // 4 rows/cols of remainder
  NE16_WRITE_REG(NE16_REG_NB_KO_KI,  ((2 & NE16_MASK_NB_KI)   << NE16_SHIFT_NB_KI)  | ((2 & NE16_MASK_NB_KO)   << NE16_SHIFT_NB_KO));
  NE16_WRITE_REG(NE16_REG_NB_HO_WO,  ((4 & NE16_MASK_NB_WO)   << NE16_SHIFT_NB_WO)  | ((4 & NE16_MASK_NB_HO)   << NE16_SHIFT_NB_HO));
  NE16_WRITE_REG(NE16_REG_PADDING, ((0 & NE16_MASK_PADDING_TOP) << NE16_SHIFT_PADDING_TOP) | ((0 & NE16_MASK_PADDING_RIGHT) << NE16_SHIFT_PADDING_RIGHT) | ((0 & NE16_MASK_PADDING_BOTTOM) << NE16_SHIFT_PADDING_BOTTOM) | ((0 & NE16_MASK_PADDING_LEFT) << NE16_SHIFT_PADDING_LEFT));
  NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, -114); // bottom asymmetric weight value
  NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((0 & NE16_MASK_FILTER_MASK_TOP) << NE16_SHIFT_FILTER_MASK_TOP) | ((0 & NE16_MASK_FILTER_MASK_RIGHT) << NE16_SHIFT_FILTER_MASK_RIGHT) | ((0 & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM) | ((0 & NE16_MASK_FILTER_MASK_LEFT) << NE16_SHIFT_FILTER_MASK_LEFT));
  NE16_WRITE_REG(NE16_REG_CONFIG, 0);
  NE16_WRITE_REG(NE16_REG_CONFIG, ((7 & NE16_MASK_WBITS_M1)           << NE16_SHIFT_WBITS_M1)          |
                                  ((0 & NE16_MASK_MODE16)             << NE16_SHIFT_MODE16)            |
                                  ((1 & NE16_MASK_OUTQUANT)           << NE16_SHIFT_OUTQUANT)          |
                                  ((1 & NE16_MASK_FILTER_MODE)        << NE16_SHIFT_FILTER_MODE)       | // depthwise mode
                                  ((0 & NE16_MASK_LINEAR_MODE)        << NE16_SHIFT_LINEAR_MODE)       |
                                  ((0 & NE16_MASK_STRIDED_MODE)       << NE16_SHIFT_STRIDED_MODE)      |
                                  ((0 & NE16_MASK_NORM_BITS)          << NE16_SHIFT_NORM_BITS)         |
                                  ((0 & NE16_MASK_STREAMIN)           << NE16_SHIFT_STREAMIN)          |
                                  ((1 & NE16_MASK_WEIGHT_OFFSET_CFG)  << NE16_SHIFT_WEIGHT_OFFSET_CFG) |
                                  ((12 & NE16_MASK_QUANT_RIGHT_SHIFT) << NE16_SHIFT_QUANT_RIGHT_SHIFT) |
                                  ((0 & NE16_MASK_QUANT_BITS)         << NE16_SHIFT_QUANT_BITS)        |
                                  ((0 & NE16_MASK_QUANT_NORECT)       << NE16_SHIFT_QUANT_NORECT)      |
                                  ((0 & NE16_MASK_NORM_SHIFT)         << NE16_SHIFT_NORM_SHIFT)        |
                                  ((0 & NE16_MASK_NORM_BIAS)          << NE16_SHIFT_NORM_BIAS));
  NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR,     ne16_weights_c1);
  NE16_WRITE_REG(NE16_REG_SCALE_PTR,       ne16_scale_c1);
  NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_streamin_c0);
  NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_c1);
  NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, NULL);
  NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR,  NULL);
}

static inline ne16_c2_config() {
  NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE,  32);   // 32 channels
  NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE,  352);  // 11 pixels x 32 channels
  NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE,  800);  // 5x5 pixels x 32 channels // 5x13 pixels x 32 channels FIXME DOES IT MAKE SENSE?
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, 0);    // unused in 8-bit output mode
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, 64);   // 64 channels
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, 704);  // 11 pixels x 64 channels
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, 16);   // 8 weight bits x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, 32);   // 2 chan major x 8 weight bits x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, 0);
  NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((16 & NE16_MASK_REM_KI) << NE16_SHIFT_REM_KI) | ((32 & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO));
  NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((2 & NE16_MASK_REM_WO)  << NE16_SHIFT_REM_WO) | ((2 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO)); // 2 rows/cols of remainder
  NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((2 & NE16_MASK_REM_WI)  << NE16_SHIFT_REM_WI) | ((2 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI)); // 2 rows/cols of remainder
  NE16_WRITE_REG(NE16_REG_NB_KO_KI,  ((2 & NE16_MASK_NB_KI)   << NE16_SHIFT_NB_KI)  | ((2 & NE16_MASK_NB_KO)   << NE16_SHIFT_NB_KO));
  NE16_WRITE_REG(NE16_REG_NB_HO_WO,  ((4 & NE16_MASK_NB_WO)   << NE16_SHIFT_NB_WO)  | ((4 & NE16_MASK_NB_HO)   << NE16_SHIFT_NB_HO));
  NE16_WRITE_REG(NE16_REG_PADDING, ((0 & NE16_MASK_PADDING_TOP) << NE16_SHIFT_PADDING_TOP) | ((0 & NE16_MASK_PADDING_RIGHT) << NE16_SHIFT_PADDING_RIGHT) | ((0 & NE16_MASK_PADDING_BOTTOM) << NE16_SHIFT_PADDING_BOTTOM) | ((0 & NE16_MASK_PADDING_LEFT) << NE16_SHIFT_PADDING_LEFT));
  NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, -122); // bottom asymmetric weight value
  NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((0 & NE16_MASK_FILTER_MASK_TOP) << NE16_SHIFT_FILTER_MASK_TOP) | ((0 & NE16_MASK_FILTER_MASK_RIGHT) << NE16_SHIFT_FILTER_MASK_RIGHT) | ((0 & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM) | ((0 & NE16_MASK_FILTER_MASK_LEFT) << NE16_SHIFT_FILTER_MASK_LEFT));
  NE16_WRITE_REG(NE16_REG_CONFIG, 0);
  NE16_WRITE_REG(NE16_REG_CONFIG, ((7 & NE16_MASK_WBITS_M1)           << NE16_SHIFT_WBITS_M1)          |
                                  ((0 & NE16_MASK_MODE16)             << NE16_SHIFT_MODE16)            |
                                  ((1 & NE16_MASK_OUTQUANT)           << NE16_SHIFT_OUTQUANT)          |
                                  ((2 & NE16_MASK_FILTER_MODE)        << NE16_SHIFT_FILTER_MODE)       | // 1x1 mode
                                  ((0 & NE16_MASK_LINEAR_MODE)        << NE16_SHIFT_LINEAR_MODE)       |
                                  ((0 & NE16_MASK_STRIDED_MODE)       << NE16_SHIFT_STRIDED_MODE)      |
                                  ((0 & NE16_MASK_NORM_BITS)          << NE16_SHIFT_NORM_BITS)         |
                                  ((0 & NE16_MASK_STREAMIN)           << NE16_SHIFT_STREAMIN)          |
                                  ((1 & NE16_MASK_WEIGHT_OFFSET_CFG)  << NE16_SHIFT_WEIGHT_OFFSET_CFG) |
                                  ((12 & NE16_MASK_QUANT_RIGHT_SHIFT) << NE16_SHIFT_QUANT_RIGHT_SHIFT) |
                                  ((0 & NE16_MASK_QUANT_BITS)         << NE16_SHIFT_QUANT_BITS)        |
                                  ((0 & NE16_MASK_QUANT_NORECT)       << NE16_SHIFT_QUANT_NORECT)      |
                                  ((0 & NE16_MASK_NORM_SHIFT)         << NE16_SHIFT_NORM_SHIFT)        |
                                  ((0 & NE16_MASK_NORM_BIAS)          << NE16_SHIFT_NORM_BIAS));
  NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR,     ne16_weights_c2);
  NE16_WRITE_REG(NE16_REG_SCALE_PTR,       ne16_scale_c2);
  NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_streamin_c1);
  NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_c2);
  NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, NULL);
  NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR,  NULL);
}

static inline ne16_f3_config() {
  NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE,  16);      // 16 channels
  NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE,  0);       // unused in linear mode
  NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE,  0);       // unused in linear mode
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, 32);      // 32 bytes
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, 0);       // unused in linear mode
  NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, 0);       // unused in linear mode
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, 7744/8);  // total input channels, divided by weight bits (?)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, 7744);    // 8 weight bits x 31 channels major x 16 channels middle x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, 7744);    // 16 channels middle x 2 byte (16 channels minor)
  NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((4 & NE16_MASK_REM_KI)  << NE16_SHIFT_REM_KI) | ((10 & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO));
  NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((0 & NE16_MASK_REM_WO)  << NE16_SHIFT_REM_WO) | ((0 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
  NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((0 & NE16_MASK_REM_WI)  << NE16_SHIFT_REM_WI) | ((0 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
  NE16_WRITE_REG(NE16_REG_NB_KO_KI,  ((31 & NE16_MASK_NB_KI)  << NE16_SHIFT_NB_KI)  | ((1 & NE16_MASK_NB_KO)   << NE16_SHIFT_NB_KO));
  NE16_WRITE_REG(NE16_REG_NB_HO_WO,  ((1 & NE16_MASK_NB_WO)   << NE16_SHIFT_NB_WO)  | ((1 & NE16_MASK_NB_HO)   << NE16_SHIFT_NB_HO));
  NE16_WRITE_REG(NE16_REG_PADDING, ((0 & NE16_MASK_PADDING_TOP) << NE16_SHIFT_PADDING_TOP) | ((0 & NE16_MASK_PADDING_RIGHT) << NE16_SHIFT_PADDING_RIGHT) | ((0 & NE16_MASK_PADDING_BOTTOM) << NE16_SHIFT_PADDING_BOTTOM) | ((0 & NE16_MASK_PADDING_LEFT) << NE16_SHIFT_PADDING_LEFT));
  NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, -148); // bottom asymmetric weight value
  NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((0 & NE16_MASK_FILTER_MASK_TOP) << NE16_SHIFT_FILTER_MASK_TOP) | ((0 & NE16_MASK_FILTER_MASK_RIGHT) << NE16_SHIFT_FILTER_MASK_RIGHT) | ((0 & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM) | ((0 & NE16_MASK_FILTER_MASK_LEFT) << NE16_SHIFT_FILTER_MASK_LEFT));
  NE16_WRITE_REG(NE16_REG_CONFIG, 0);
  NE16_WRITE_REG(NE16_REG_CONFIG, ((7 & NE16_MASK_WBITS_M1)           << NE16_SHIFT_WBITS_M1)          |
                                  ((0 & NE16_MASK_MODE16)             << NE16_SHIFT_MODE16)            |
                                  ((0 & NE16_MASK_OUTQUANT)           << NE16_SHIFT_OUTQUANT)          |
                                  ((3 & NE16_MASK_FILTER_MODE)        << NE16_SHIFT_FILTER_MODE)       | // 1x1 mode
                                  ((1 & NE16_MASK_LINEAR_MODE)        << NE16_SHIFT_LINEAR_MODE)       |
                                  ((0 & NE16_MASK_STRIDED_MODE)       << NE16_SHIFT_STRIDED_MODE)      |
                                  ((0 & NE16_MASK_NORM_BITS)          << NE16_SHIFT_NORM_BITS)         |
                                  ((0 & NE16_MASK_STREAMIN)           << NE16_SHIFT_STREAMIN)          |
                                  ((1 & NE16_MASK_WEIGHT_OFFSET_CFG)  << NE16_SHIFT_WEIGHT_OFFSET_CFG) |
                                  ((0 & NE16_MASK_QUANT_RIGHT_SHIFT)  << NE16_SHIFT_QUANT_RIGHT_SHIFT) |
                                  ((2 & NE16_MASK_QUANT_BITS)         << NE16_SHIFT_QUANT_BITS)        |
                                  ((1 & NE16_MASK_QUANT_NORECT)       << NE16_SHIFT_QUANT_NORECT)      |
                                  ((0 & NE16_MASK_NORM_SHIFT)         << NE16_SHIFT_NORM_SHIFT)        |
                                  ((0 & NE16_MASK_NORM_BIAS)          << NE16_SHIFT_NORM_BIAS));
  NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR,     ne16_weights_f3);
  NE16_WRITE_REG(NE16_REG_SCALE_PTR,       NULL);
  NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_streamin_c2);
  NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_f3);
  NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, NULL);
  NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR,  NULL);
}

int run_ne16_test() {

  // enable clock
  NE16_CG_ENABLE();

  // setup HCI
  NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
  NE16_RESET_MAXSTALL();   // reset maximum stall
  NE16_SET_MAXSTALL(8);    // set maximum consecutive stall to 8 cycles for cores, DMA side

  // soft-clear NE16
  NE16_WRITE_CMD(NE16_SOFT_CLEAR, NE16_SOFT_CLEAR_ALL);
  for(volatile int kk=0; kk<10; kk++);

  /*
    The first layer (c0) has stride 2. We model it using the usual 3x3 mode, disabling 8 columns, and calling NE16 many times on single subtiles by software.
   */

  volatile int job_id;

  // define strided conv iteration indeces
  int subtile_i_major=0;
  int subtile_j_major=0;

  // acquire first job
  NE16_BARRIER_ACQUIRE(job_id);

  // preload all common configuration for the layer in the first job
  ne16_c0_config();

  // update input / output pointers
  NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_infeat_c0 + 4*28*subtile_i_major + 4*subtile_j_major);
  NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_c0 + 2*13*32*subtile_i_major + 2*32*subtile_j_major);
  subtile_j_major ++;

  // already commit and trigger NE16 computation, while programming the next one
  NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

  // acquire second job
  NE16_BARRIER_ACQUIRE(job_id);

  // preload all common configuration for the layer in the first job
  ne16_c0_config();

  // update input / output pointers
  NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_infeat_c0 + 4*28*subtile_i_major + 4*subtile_j_major);
  NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_c0 + 2*13*32*subtile_i_major + 2*32*subtile_j_major);
  subtile_j_major ++;

  // already commit and trigger NE16 computation, while programming the next one
  NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

  // main iteration strided conv iteration loop (does not need ne16_c0_config any more)
  do {

    // acquire job
    NE16_BARRIER_ACQUIRE(job_id);

    /*
      Input  subtiles: [---|  ( '[' inclusive, '|' exclusive )
      Output subtiles: [***|  ( '[' inclusive, '|' exclusive )
                           0                                                     27
                           +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       subtile_j_major = 0 [---------|
                             [*****|
       subtile_j_major = 1         [---------|
                                     [*****|
       subtile_j_major = 2                 [---------|
                                             [*****|
       subtile_j_major = 3                         [---------|
                                                     [*****|
       subtile_j_major = 4                                 [---------|
                                                             [*****|
       subtile_j_major = 5                                         [---------|
                                                                     [*****|
       subtile_j_major = 6                                                 [-----|XXXX     REM = 3
                                                                             [***|XX       REM = 1
    */

    // update input / output pointers
    if(subtile_i_major==6 && subtile_j_major==6) {
      NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((1 & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | ((1 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
      NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((3 & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | ((3 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
    }
    else if(subtile_i_major==6) {
      NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((0 & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | ((1 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
      NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((0 & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | ((3 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
    }
    else if(subtile_j_major==6) {
      NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((1 & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | ((0 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
      NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((3 & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | ((0 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
    }
    else {
      NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((0 & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | ((0 & NE16_MASK_REM_HO)  << NE16_SHIFT_REM_HO));
      NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((0 & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | ((0 & NE16_MASK_REM_HI)  << NE16_SHIFT_REM_HI));
    }
    NE16_WRITE_REG(NE16_REG_INFEAT_PTR,      ne16_infeat_c0 + 4*28*subtile_i_major + 4*subtile_j_major);
    NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR,     ne16_streamin_c0 + 2*13*32*subtile_i_major + 2*32*subtile_j_major);

    // commit and trigger NE16 computation
    NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

    // update indeces
    subtile_j_major ++;
    if(subtile_j_major==7) {
      subtile_j_major = 0;
      subtile_i_major ++;
    }

  } while(subtile_i_major<7 && subtile_j_major<7);

  // wait for end of computation
  NE16_BARRIER();

  /*
    The second layer (c1) is a depthwise layer. We model it using the 3x3 depthwise mode, hardware-managed.
   */

  // clear NE16 state
  NE16_WRITE_CMD(NE16_SOFT_CLEAR, NE16_SOFT_CLEAR_ALL);
  for(volatile int kk=0; kk<10; kk++);

  // acquire job
  NE16_BARRIER_ACQUIRE(job_id);

  // load configuration for the layer
  ne16_c1_config();

  // commit and trigger NE16 computation
  NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

  // wait for end of computation
  NE16_BARRIER();

  /*
    The third layer (c2) is a 1x1 layer. We model it using the 1x1 mode, hardware-managed.
   */

  // clear NE16 state
  NE16_WRITE_CMD(NE16_SOFT_CLEAR, NE16_SOFT_CLEAR_ALL);
  for(volatile int kk=0; kk<10; kk++);

  // acquire job
  NE16_BARRIER_ACQUIRE(job_id);

  // load configuration for the layer
  ne16_c2_config();

  // commit and trigger NE16 computation
  NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

  // wait for end of computation
  NE16_BARRIER();

  /*
    The fourth layer (f3) is a linear layer.
   */

  // clear NE16 state
  NE16_WRITE_CMD(NE16_SOFT_CLEAR, NE16_SOFT_CLEAR_ALL);
  for(volatile int kk=0; kk<10; kk++);

  // acquire job
  NE16_BARRIER_ACQUIRE(job_id);

  // load configuration for the layer
  ne16_f3_config();

  // commit and trigger NE16 computation
  NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

  // wait for end of computation
  NE16_BARRIER();

  /*
   Disable NE16 and check the results.
   */

  // disable clock
  NE16_CG_DISABLE();

  // set priority to core side
  NE16_SETPRIORITY_CORE();

  int errors = 0;
  printf("Layer c0:\n");
  errors += ne16_compare_int(ne16_streamin_c0, ne16_outfeat_c0, 5408/4);
  printf("Layer c1:\n");
  errors += ne16_compare_int(ne16_streamin_c1, ne16_outfeat_c1, 3872/4);
  printf("Layer c2:\n");
  errors += ne16_compare_int(ne16_streamin_c2, ne16_outfeat_c2, 7744/4);
  printf("Layer f3:\n");
  errors += ne16_compare_int(ne16_streamin_f3, ne16_outfeat_f3, 10);

  return errors;
}

static struct pi_cluster_task task[1];
static struct pi_task events[1];

static void pe_entry(void *arg) {
  printf("%d %s %d\n", pi_core_id(), __FILE__, __LINE__);
  if(pi_core_id() == 0) {
    glob_errors = run_ne16_test();
    interference_flag = 1;
  }
  pi_cl_team_barrier();
}

static void cluster_entry(void *arg) {
  printf("%d %s %d\n", pi_core_id(), __FILE__, __LINE__);
  pi_cl_team_fork(0, pe_entry, 0);
}

static int launch_cluster_task() {
  struct pi_device cluster_dev;
  struct pi_cluster_conf conf;
  struct pi_cluster_task task;

  pi_cluster_conf_init(&conf);
  conf.id = 0;
  glob_errors = 0;

  pi_open_from_conf(&cluster_dev, &conf);
  pi_cluster_open(&cluster_dev);
  pi_freq_set(PI_FREQ_DOMAIN_FC, 5000000);
  pi_time_wait_us(1000);
  pi_freq_set(PI_FREQ_DOMAIN_FC, 50000000);
  pi_freq_set(PI_FREQ_DOMAIN_CL, 5000000);
  pi_time_wait_us(1000);
  pi_freq_set(PI_FREQ_DOMAIN_CL, 50000000);

  pi_cluster_task(&task, cluster_entry, NULL);
  task.nb_cores = 1;
  printf("%d %s %d\n", pi_core_id(), __FILE__, __LINE__);
  if (pi_cluster_send_task_to_cl(&cluster_dev, &task)) return -1;
  printf("%d %s %d\n", pi_core_id(), __FILE__, __LINE__);
  pi_cluster_close(&cluster_dev);

  return glob_errors;
}

int test_entry() {
  printf("Starting test\n");
  int errors = launch_cluster_task();
  if (errors)
    printf("Test failure\n");
  else
    printf("Test success\n");
  return errors;
}

void test_kickoff(void *arg) {
  int ret = test_entry();
  pmsis_exit(ret);
}

int main() {
  return pmsis_kickoff((void *)test_kickoff);
}
