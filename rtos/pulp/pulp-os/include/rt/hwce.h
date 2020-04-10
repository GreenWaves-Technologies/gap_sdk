/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __DRIVERS__HWCE_H__
#define __DRIVERS__HWCE_H__

#include <stdint.h>

struct pi_hwce_conv_s {
    void *weights;
    void *input;
    void *output;
    int width;
    int height;
    int nif;
    int nof;
    int norm;
    int weight_stride;
    uint32_t bias;
    struct pi_hwce_conv_s *next;
    char filter_size;
    char set_bias;
    char fix;
    char first;
    char type;
};

void pi_hwce_open();

void pi_hwce_close();

void pi_hwce_3x3_16x16_feat_init(struct pi_hwce_conv_s *conv, void *weights, void *input, void *output, int width, int height);

void pi_hwce_3x3_16x16_feat_start(struct pi_hwce_conv_s *conv);

void pi_hwce_wait(struct pi_hwce_conv_s *conv);

#endif