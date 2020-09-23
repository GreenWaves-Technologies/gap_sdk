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

#ifndef __PMSIS_H__
#define __PMSIS_H__

#include <stddef.h>
#include <inttypes.h>

/* Debug helper. */
#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

#if (!defined(HW_VERIF_ONLY))

/* Backend includes. */
#include "pmsis/backend/implementation_specific_defines.h"
#include "pmsis/targets/target.h"
/* pmsis_api includes. */
#include "pmsis/errno.h"
#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis_types.h"
#include "pmsis/rtos/rtos.h"
#include "pmsis/rtos/assert.h"
#include "pmsis/mem_slab.h"

/* PMSIS includes. */
#include "pmsis/implem/debug.h"
#include "pmsis/implem/drivers/drivers_data.h"
#include "pmsis/implem/drivers/drivers.h"
#include "pmsis/mem_slab/mem_slab.h"
#include "pmsis/rtos/os/pmsis_task.h"
#include "pmsis/rtos/os/pmsis_freq.h"
#include "pmsis/rtos/os/os.h"
#if defined(FEATURE_CLUSTER)
#include "pmsis/implem/cluster/cluster.h"
#endif  /* FEATURE_CLUSTER */

#endif  /* HW_VERIF_ONLY */

/* Hal includes. */
#include "pmsis/implem/hal/hal.h"

#endif  /* __PMSIS_H__ */
