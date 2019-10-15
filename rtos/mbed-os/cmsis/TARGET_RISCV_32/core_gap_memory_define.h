/*
 * Copyright (c) 2017, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CORE_GAP_COMMON_H
#define __CORE_GAP_COMMON_H

/*! @brief Data declared with this attribute will be dropped after startup */
#define GAP_BOOT_DATA     __attribute__((section(".boot.data")))
/*! @brief Code declared with this attribute will be dropped after startup. */
#define GAP_BOOT_CODE     __attribute__((section(".boot")))
/*! @brief Data will be put inside fabric L2 memory. */
#define GAP_L2_DATA       __attribute__((section(".data")))

#if defined(__GAP8__)
/*! @brief Data will be put by the linker in FC TCDM .data. */
#define GAP_FC_DATA       __attribute__((section(".fcdata")))
/*! @brief Data will be put by the linker in FC TCDM .bss, will be initialized to 0. */
#define GAP_FC_VAR        __attribute__((section(".fcbss")))
#elif defined(__GAP9__)
/*! @brief No FC TCDM */
#define GAP_FC_DATA
#define GAP_FC_VAR
#endif

#define GAP_EXPORT        __attribute__ ((export))
#define GAP_IMPORT        __attribute__ ((import))

#ifdef FEATURE_CLUSTER
/*! @brief Data will be put inside cluster L1 memory through global address. */
#define GAP_L1_DATA         __attribute__((section(".l1cluster_g")))
#define GAP_L1_GLOBAL_DATA  GAP_L1_DATA
#define GAP_L1_TINY_DATA    GAP_L1_DATA
#define GAP_L1_FC_DATA      __attribute__((section(".l1FcTiny")))
#endif

#define FC_L1_MEM          GAP_FC_DATA

#endif
