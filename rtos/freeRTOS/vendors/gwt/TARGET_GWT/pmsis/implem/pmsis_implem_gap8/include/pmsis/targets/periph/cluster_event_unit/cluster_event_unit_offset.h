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

#ifndef __ARCHI_CLUSTER_EVENT_UNIT_OFFSET_H__
#define __ARCHI_CLUSTER_EVENT_UNIT_OFFSET_H__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif


/*! Event_Unit Demux offset */
#define CLUSTER_DEMUX_EU_CORE_OFFSET              ( 0x00000000 )
#define CLUSTER_DEMUX_EU_LOOP_OFFSET              ( 0x00000060 )
#define CLUSTER_DEMUX_EU_DISPATCH_OFFSET          ( 0x00000080 )
#define CLUSTER_DEMUX_EU_MUTEX_OFFSET             ( 0x000000C0 )
#define CLUSTER_DEMUX_EU_SW_EVENT_OFFSET          ( 0x00000100 )
#define CLUSTER_DEMUX_EU_BARRIER_OFFSET           ( 0x00000200 )



/*! Event_Unit Core Demux */
#define CL_DEMUX_EU_CORE_EVENT_MASK               ( 0x00 )
#define CL_DEMUX_EU_CORE_EVENT_MASK_AND           ( 0x04 )
#define CL_DEMUX_EU_CORE_EVENT_MASK_OR            ( 0x08 )
#define CL_DEMUX_EU_CORE_IRQ_MASK                 ( 0x0C )
#define CL_DEMUX_EU_CORE_IRQ_MASK_AND             ( 0x10 )
#define CL_DEMUX_EU_CORE_IRQ_MASK_OR              ( 0x14 )
#define CL_DEMUX_EU_CORE_STATUS                   ( 0x18 )
#define CL_DEMUX_EU_CORE_BUFFER                   ( 0x1C )
#define CL_DEMUX_EU_CORE_BUFFER_MASKED            ( 0x20 )
#define CL_DEMUX_EU_CORE_BUFFER_IRQ_MASKED        ( 0x24 )
#define CL_DEMUX_EU_CORE_BUFFER_CLEAR             ( 0x28 )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK              ( 0x2C )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_AND          ( 0x30 )
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_OR           ( 0x34 )
#define CL_DEMUX_EU_CORE_EVENT_WAIT               ( 0x38 )
#define CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR         ( 0x3C )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK             ( 0x40 )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_AND         ( 0x44 )
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_OR          ( 0x48 )

/*! Event_Unit Loop Demux */
#define CL_DEMUX_EU_LOOP_STATE                    ( 0x00 )
#define CL_DEMUX_EU_LOOP_START                    ( 0x04 )
#define CL_DEMUX_EU_LOOP_END                      ( 0x08 )
#define CL_DEMUX_EU_LOOP_INCR                     ( 0x0C )
#define CL_DEMUX_EU_LOOP_CHUNK                    ( 0x10 )
#define CL_DEMUX_EU_LOOP_EPOCH                    ( 0x14 )
#define CL_DEMUX_EU_LOOP_SINGLE                   ( 0x18 )

/*! Event_Unit Dispatch Demux */
#define CL_DEMUX_EU_DISPATCH_FIFO_ACCESS          ( 0x00 )
#define CL_DEMUX_EU_DISPATCH_TEAM_CONFIG          ( 0x04 )

/*! Event_Unit Mutex Demux */
#define CL_DEMUX_EU_MUTEX_MUTEX                   ( 0x00 )

/*! Event_Unit SW_Events Demux */
#define CL_DEMUX_EU_SW_EVT_TRIGGER                ( 0x00 )
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT           ( 0x40 )
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT_CLEAR     ( 0x80 )

/*! Event_Unit Barrier Demux */
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK       ( 0x00 )
#define CL_DEMUX_EU_HW_BARRIER_STATUS             ( 0x04 )
#define CL_DEMUX_EU_HW_BARRIER_STATUS_SUMMARY     ( 0x08 )
#define CL_DEMUX_EU_HW_BARRIER_TARGET_MASK        ( 0x0C )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER            ( 0x10 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_SELF       ( 0x14 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT       ( 0x18 )
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR ( 0x1C )

#define CL_DEMUX_EU_HW_BARRIER_SIZE               ( 0x20 )


#endif  /* __ARCHI_CLUSTER_EVENT_UNIT_OFFSET_H__ */
