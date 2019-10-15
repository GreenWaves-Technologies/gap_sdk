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

#include "pmsis/data/data.h"

#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis/cluster/cluster_sync/fc_to_cl_delegate.h"
#include "pmsis/cluster/cl_malloc.h"
#include "pmsis/rtos/os_frontend_api/pmsis_time.h"
#include "pmsis/rtos/os_frontend_api/pmsis_freq.h"
#include "pmsis/rtos/malloc/pmsis_malloc.h"
#include "pmsis/rtos/malloc/pmsis_l1_malloc.h"
#include "pmsis/rtos/malloc/pmsis_l2_malloc.h"
#include "pmsis/drivers/perf.h"
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/uart.h"
#include "pmsis/drivers/pad.h"
#include "pmsis/cluster/dma/cl_dma.h"

#include "pmsis/implem/implem.h"

#include "hal/utils.h"
#include "pmsis/cluster/cluster_sync/cl_synchronisation.h"

#endif

