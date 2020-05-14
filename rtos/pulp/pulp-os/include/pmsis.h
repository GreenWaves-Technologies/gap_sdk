/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __PMSIS__H__
#define __PMSIS__H__

#include <stdint.h>
#include <stddef.h>

#include "pmsis/rtos/pulpos.h"

#include "rt/rt_data.h"

#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis/mem_slab.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cluster_sync/cl_to_fc_delegate.h"
#include "pmsis/cluster/cl_malloc.h"
#if PULP_CHIP_FAMILY == CHIP_GAP
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/chips/gap8/pad.h"
#include "pmsis/chips/gap8/gpio.h"
#elif PULP_CHIP_FAMILY == CHIP_VEGA
#include "pmsis/chips/vega/perf.h"
#include "pmsis/chips/vega/pad.h"
#include "pmsis/chips/vega/gpio.h"
#elif PULP_CHIP_FAMILY == CHIP_GAP9
#include "pmsis/chips/gap9/perf.h"
#include "pmsis/chips/gap9/pad.h"
#include "pmsis/chips/gap9/gpio.h"
#else
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/chips/gap8/pad.h"
#include "pmsis/chips/gap8/gpio.h"
#endif
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "pmsis/rtos/os_frontend_api/freq.h"
#include "pmsis/rtos/malloc/cl_l1_malloc.h"
#include "pmsis/rtos/malloc/l2_malloc.h"
#include "pmsis/rtos/malloc/fc_l1_malloc.h"
#include "pmsis/drivers/perf.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2s.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/drivers/dmacpy.h"
#include "pmsis/drivers/pwm.h"
#include "pmsis/drivers/pad.h"
#include "pmsis/cluster/dma/cl_dma.h"
#include "rt/hwce.h"

#include "pmsis/data/data.h"

#include "pmsis/implem/implem.h"

#include "pmsis/rtos/rtos.h"

#include "hal/utils.h"
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"

#endif

