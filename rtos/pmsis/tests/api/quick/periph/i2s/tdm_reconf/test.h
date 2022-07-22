/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
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

#ifndef __TEST_H__
#define __TEST_H__

typedef struct {
    int unmuted_time;
    int muted_time;
    int width;
    int msb_first;
    int left_align;
    int sign_extend;
} slot_usecase_t;

typedef struct
{
    int nb_usecases;
    slot_usecase_t *usecases;
} slot_desc_t;


typedef struct
{
    int sampling_freq;
    int nb_slots;
    slot_desc_t *slot_descs;
} itf_desc_t;


typedef struct
{
    int nb_itf;
    itf_desc_t *itf_descs;
} test_desc_t;


extern test_desc_t test_desc;

#endif