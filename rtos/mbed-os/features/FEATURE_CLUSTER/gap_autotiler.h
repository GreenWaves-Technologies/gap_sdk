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

#ifndef _GAP_AUTOTILER_H_
#define _GAP_AUTOTILER_H_

#include <cmsis.h>
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include "gap_hwce.h"


#ifdef FEATURE_CLUSTER

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define AUTOTILE_DMAMCHAN_Memcpy_1D(a,b,c,d,e,f)            DMAMCHAN_Memcpy_1D((uint32_t) a, (uint32_t) b, (uint16_t) c, (uint8_t) d, (dma_req_t *) f)
#define AUTOTILE_DMAMCHAN_Memcpy_2D(a,b,c,d,e,f,g,h)        DMAMCHAN_Memcpy_2D((uint32_t) a, (uint32_t) b, (uint16_t) c, (uint16_t) d, (uint16_t) e, (uint8_t) f, (dma_req_t *) h);

#define GAP_UDMA_L32TCDM GAP_DMA_L22TCDM
#define GAP_UDMA_TCDM2L3 GAP_DMA_TCDM2L2

#define AUTOTILE_CLUSTER_CoresFork(a,b,c)                   CLUSTER_CoresFork(b,c)

#define rt_team_barrier()                                   CLUSTER_SynchBarrier()
#define rt_nb_pe()                                          8
#define rt_core_id()                                        __core_ID()

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* FEATURE_CLUSTER */

#endif


