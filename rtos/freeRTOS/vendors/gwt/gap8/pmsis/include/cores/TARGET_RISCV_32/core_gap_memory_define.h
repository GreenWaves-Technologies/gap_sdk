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

/*! @brief Data will be put by the linker in FC TCDM or in cluster L1 if there is no FC. */
#define GAP_FC_DATA       __attribute__((section(".fcTcdm")))
/*! @brief Data will be put by the linker at the beginning of the FC TCDM or in cluster L1 if there is no FC. */
#define GAP_FC_TINY_DATA  __attribute__((section(".fcTcdmTiny")))
/*! @brief Data will be put in FC TCDM through global address. */
#define GAP_FC_GLOBAL_DATA  __attribute__((section(".fcTcdm_g")))

#define GAP_EXPORT        __attribute__ ((export))
#define GAP_IMPORT        __attribute__ ((import))

#ifdef FEATURE_CLUSTER
/*! @brief Data will be put by the linker in both FC TCDM and in cluster L1 at the same aliased address
 * This can be used to instantiate the same variable on both side which can then be used transparently
 * by the code on both side
 */
#define GAP_L1_TINY_DATA  __attribute__ ((tiny)) __attribute__ ((section(".l1clusterTiny")))
#define GAP_L1_FC_DATA    __attribute__ ((tiny)) __attribute__((section(".l1FcTiny")))
/*! @brief Data will be put inside cluster L1 memory through global address. */
#define GAP_L1_GLOBAL_DATA  __attribute__((section(".l1cluster_g")))
#endif  /* FEATURE_CLUSTER */



/* Latest macros for memory location. */
#define PI_CL_L1           GAP_L1_GLOBAL_DATA
#define PI_FC_L1           GAP_FC_GLOBAL_DATA
#define PI_L2              GAP_L2_DATA

/* For legacy, keep those. */
#define PI_L1              PI_CL_L1
#define FC_L1_MEM          PI_FC_L1


#endif  /* __CORE_GAP_COMMON_H */
