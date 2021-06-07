/*
 * Copyright 2021 GreenWaves Technologies, SAS
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


#ifndef __FLOAT_MATH_FUNCS_H__
#define __FLOAT_MATH_FUNCS_H__
#include <stdint.h>
#include "pulp_dsp/plp_math_extract.h"

float32_t ffast_cos(float32_t val);
float32_t ffast_sin(float32_t val);
float32_t ffast_sigmoid(float32_t val, float32_t alpha);
float32_t fsigmoid(float32_t val);
#endif