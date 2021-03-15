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

#ifndef __PMSIS_AES_UTILS_H__
#define __PMSIS_AES_UTILS_H__

#if defined(__GAP9__)

#include "pmsis.h"
#include "pmsis/drivers/aes.h"

/**
 * \brief structure containing parameters for AES configuration
 */
typedef struct pi_aes_utils_conf
{
    uint8_t enabled; /*!< 0 for disabled, any other value for enabled */
    uint8_t qk_en ; /*!< 0 for disabled, any other value for enabled */
    pi_aes_mode_e mode;
    pi_aes_key_len_e key_len;
    uint32_t* key;
    uint32_t* key_code0;
    uint32_t* key_code1;
    pi_device_t *qk_dev;
    uint32_t* iv;
} pi_aes_utils_conf_t;

#endif

#endif
