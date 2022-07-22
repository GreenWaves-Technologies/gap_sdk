/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __PMSIS__H__
#define __PMSIS__H__


#include <stdint.h>
#include <stddef.h>

#include <hal/pulp.h>

#include <pos/data/data.h>



#define PI_INLINE_CL_TEAM_0 static inline
#define PI_INLINE_CL_TEAM_1 static inline
#define PI_INLINE_HYPER_LVL_0 static inline
#define PI_INLINE_OCTOSPI_LVL_0 static inline
#define PI_INLINE_I2S_LVL_0 static inline
#define PI_INLINE_FREQ_LVL_0


#if defined(__GAP9__)
#include "pmsis/chips/gap9/perf.h"
#include "pmsis/chips/gap9/pad.h"
#include "pmsis/chips/gap9/gpio.h"
#include "pmsis/chips/gap9/gap9.h"
#include "pmsis/chips/gap9/pmu.h"
#include <malloc/alloc_types.h>
#include <malloc/cl_alloc.h>
#include <malloc/alloc.h>
#include <malloc/alloc_pool.h>
#else
#include "pmsis/chips/pulp/pulp.h"
#endif

#include <pmsis/rtos/pi_log.h>
#include "pmsis/errno.h"
#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/cluster/cl_malloc.h"
#include "pmsis/rtos/assert.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "pmsis/rtos/os_frontend_api/freq.h"
#include "pmsis/rtos/os_frontend_api/os.h"
#include "pmsis/rtos/malloc/cl_l1_malloc.h"
#include "pmsis/rtos/malloc/l2_malloc.h"
#include "pmsis/rtos/malloc/fc_l1_malloc.h"
#include "pmsis/mem_slab.h"
#include "pmsis/drivers/perf.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/octospi.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2c_slave.h"
#include "pmsis/drivers/i2s.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/drivers/ffc.h"
#include "pmsis/drivers/pwm.h"
#include "pmsis/drivers/pad.h"
#include "pmsis/drivers/aes.h"
#include "pmsis/drivers/rtc.h"
#include "pmsis/drivers/udma_fifo.h"
#include "pmsis/drivers/udma_datamove.h"
#include "pmsis/drivers/udma_timestamp.h"
#include "pmsis/cluster/dma/cl_dma.h"


#include "hal/utils.h"
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"

#include <pos/implem/implem.h>

#ifdef __PLATFORM_GVSOC__
#include <pmsis/platforms/gvsoc.h>
#endif

#endif

