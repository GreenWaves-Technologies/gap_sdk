/*
 * Copyright (c) 2013-2018 Arm Limited. All rights reserved.
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
 * -----------------------------------------------------------------------------
 *
 * Project:     CMSIS-RTOS RTX
 * Title:       Cortex Core definitions
 *
 * -----------------------------------------------------------------------------
 */
/*
 Modifications copyright (C) 2018 GreenWaves Technologies

 - Add support for GAP architecture
 */
#ifndef RTX_CORE_C_H_
#define RTX_CORE_C_H_

//lint -emacro((923,9078),SCB) "cast from unsigned long to pointer" [MISRA Note 9]
#include "RTE_Components.h"
#include CMSIS_device_header

#ifndef __RISCV_ARCH_GAP__
#define __RISCV_ARCH_GAP__     1U
#endif

#if ((!defined(__ARM_ARCH_6M__))      && \
     (!defined(__ARM_ARCH_7A__))      && \
     (!defined(__ARM_ARCH_7M__))      && \
     (!defined(__ARM_ARCH_7EM__))     && \
     (!defined(__ARM_ARCH_8M_BASE__)) && \
     (!defined(__ARM_ARCH_8M_MAIN__)))
#if (__RISCV_ARCH_GAP__ != 1U)
#error "Unknown ARM/RISCV Architecture!"
#endif
#endif

#if   (defined(__ARM_ARCH_7A__) && (__ARM_ARCH_7A__ != 0))
#include "rtx_core_ca.h"
#else
#include "rtx_core_cm.h"
#endif

#endif  // RTX_CORE_C_H_
