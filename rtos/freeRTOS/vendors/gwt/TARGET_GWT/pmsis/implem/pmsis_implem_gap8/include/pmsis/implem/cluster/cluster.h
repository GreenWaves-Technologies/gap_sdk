/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_CLUSTER_CLUSTER_H__
#define __PMSIS_IMPLEM_CLUSTER_CLUSTER_H__

/* ID of the mutex used to lock DMA. */
#define CL_DMA_MUTEX_ID       ( 0 )

/*
 * SHOULD NOT BE HERE !
 * Function is implemented in cl_to_fc_delegate.h
 */
void cl_wait_task(uint8_t *done);
void cl_notify_task_done(uint8_t *done, uint8_t cluster_id);

/* pmsis_api includes. */
#include "pmsis/cluster/cl_malloc.h"
#include "pmsis/cluster/cl_pmsis_api.h"
/* Cluster sync. */
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
/* Cluster team. */
#include "pmsis/cluster/cluster_team/cl_team.h"
/* DMA. */
#include "pmsis/cluster/dma/cl_dma.h"

/* PMSIS cluster drivers includes. */
#include "pmsis/implem/cluster/delegate/uart/uart_cl_internal.h"
#include "pmsis/implem/cluster/delegate/hyperbus/hyperbus_cl_internal.h"
#include "pmsis/implem/cluster/malloc/cl_malloc_internal.h"
/* DMA. */
#include "pmsis/implem/cluster/dma/cl_dma.h"

/* PMSIS cluster data includes. */
#include "pmsis/implem/cluster/cluster_data.h"

#endif  /* __PMSIS_IMPLEM_CLUSTER_CLUSTER_H__ */
