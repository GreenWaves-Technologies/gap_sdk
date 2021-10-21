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


#include "DSP_Lib.h"

float32_t ffast_cos(float32_t val) {
    return plp_cos_f32s_xpulpv2(val);
}

float32_t ffast_sin(float32_t val) {
    return plp_sin_f32s_xpulpv2(val);
}

float32_t ffast_sigmoid(float32_t val, float32_t alpha) {
    return 0.5F * (val * alpha / (1.0F + fabs(val*alpha))) + 0.5F;
}

float32_t fsigmoid(float32_t val) {
    return 1.0F / (1.0F + exp(-val));
}
