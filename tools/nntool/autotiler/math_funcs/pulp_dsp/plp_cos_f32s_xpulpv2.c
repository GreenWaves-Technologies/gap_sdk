/* =====================================================================
 * Project:      PULP DSP Library
 * Title:        plp_cos_f32s_xpulpv2.c
 * Description:  Calculates cosine of a floating point number in radians
 *
 * $Date:        30.07.2020
 *
 * Target Processor: PULP cores
 * ===================================================================== */
/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna.
 *
 * Author: Michael Rogenmoser, ETH Zurich
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Notice: project inspired by ARM CMSIS DSP and parts of source code
 * ported and adopted for RISC-V PULP platform from ARM CMSIS DSP
 * released under Copyright (C) 2010-2019 ARM Limited or its affiliates
 * with Apache-2.0.
 */

#include "plp_common_tables.h"
#include "plp_math_extract.h"

/**
 * @brief      Glue code for f32 cosine function
 *
 * @param[in]  x     input value in radians
 *
 * @return     cos(x)
 */

float32_t plp_cos_f32s_xpulpv2(float32_t x) {
    float32_t cosVal, fract, in; /* Temporary input, output variables */
    uint16_t index;              /* Index variable */
    float32_t a, b;              /* Two nearest output values */
    int32_t n;
    float32_t findex;

    /* input x is in radians */
    /* Scale input to [0 1] range from [0 2*PI] , divide input by 2*pi, add 0.25 (pi/2) to read sine
     * table */
    in = x * 0.159154943092f + 0.25f;

    /* Calculation of floor value of input */
    n = (int32_t)in;

    /* Make negative values towards -infinity */
    if (in < 0.0f) {
        n--;
    }

    /* Map input value to [0 1] */
    in = in - (float32_t)n;

    /* Calculation of index of the table */
    findex = (float32_t)FAST_MATH_TABLE_SIZE * in;
    index = (uint16_t)findex;

    /* when "in" is exactly 1, we need to rotate the index down to 0 */
    if (index >= FAST_MATH_TABLE_SIZE) {
        index = 0;
        findex -= (float32_t)FAST_MATH_TABLE_SIZE;
    }

    /* fractional value calculation */
    fract = findex - (float32_t)index;

    /* Read two nearest values of input value from the cos table */
    a = sinTable_f32[index];
    b = sinTable_f32[index + 1];

    /* Linear interpolation process */
    cosVal = (1.0f - fract) * a + fract * b;

    /* Return output value */
    return (cosVal);
}
