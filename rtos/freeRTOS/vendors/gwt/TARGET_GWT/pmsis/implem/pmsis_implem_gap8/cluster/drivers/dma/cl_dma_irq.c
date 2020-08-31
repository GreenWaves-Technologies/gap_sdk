/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#if (FEATURE_CLUSTER == 1)

#include "pmsis/implem/cluster/dma/cl_dma.h"

void pi_cl_dma_2d_handler()
{
    pi_cl_dma_cmd_t *copy = fifo_first;
    hal_compiler_barrier();
    hal_cl_dma_tid_free(copy->tid);
    if (!copy->size)
    {
        fifo_first = fifo_first->next;
        if (fifo_first == NULL)
        {
            fifo_last = NULL;
        }
        hal_compiler_barrier();
        hal_eu_cluster_evt_trig_set(DMA_SW_IRQN, 0);
        copy = fifo_first;
    }
    hal_compiler_barrier();

    hal_compiler_barrier();
    if ((copy != NULL) && (copy->size))
    {
        uint32_t iter_length = (copy->size < copy->length) ? copy->size : copy->length;
        uint32_t dma_cmd = copy->cmd | (iter_length << DMAMCHAN_CMD_LEN_Pos);
        uint32_t loc = copy->loc;
        uint32_t ext = copy->ext;
        hal_compiler_barrier();
        copy->loc = loc + iter_length;
        copy->ext = ext + copy->stride;
        copy->size = copy->size - iter_length;
        copy->tid = hal_cl_dma_tid_get();
        hal_cl_dma_1d_transfer_push(dma_cmd, loc, ext);
    }
    hal_compiler_barrier();
}

#endif  /* FEATURE_CLUSTER */
