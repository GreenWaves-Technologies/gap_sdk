/** ==========================================================================
 * @file     plp_math_extract.h
 * @brief    Public header file for PULP DSP Library
 * @version  V0
 * @date     16. May 2019
 * =========================================================================== */
/*
 * Copyright (C) 2019 ETH Zurich and University of Bologna. All rights reserved.
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
 */

#ifndef __PLP_MATH_H__
#define __PLP_MATH_H__

#include <stdint.h>
#include <math.h>

#define FAST_MATH_TABLE_SIZE 512

typedef float float32_t;

extern const float32_t sinTable_f32[FAST_MATH_TABLE_SIZE + 1];

float32_t plp_cos_f32s_xpulpv2(float32_t x);
float32_t plp_sin_f32s_xpulpv2(float32_t x);

#endif