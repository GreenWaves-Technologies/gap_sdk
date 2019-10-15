/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
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

#ifndef __RT_RT_API_DECL_H__
#define __RT_RT_API_DECL_H__

#include "rt/rt_data.h"

#include "rt/chips/rt_default.h"
#include "rt/chips/rt_gap.h"
#include "rt/chips/rt_wolfe.h"
#include "rt/chips/rt_vega.h"
#include "rt/chips/rt_gap9.h"
#include "rt/chips/rt_pulpissimo.h"
#include "rt/chips/rt_pulp.h"

#include "rt/rt_irq.h"
#include "rt/rt_utils.h"
#include "rt/rt_extern_alloc.h"
#include "rt/rt_thread.h"
#include "rt/rt_event.h"
#include "rt/rt_flash.h"
#include "rt/rt_dev.h"
#include "rt/rt_periph.h"
#include "rt/rt_cluster.h"
#include "rt/rt_hyper.h"
#include "rt/rt_alloc.h"
#include "rt/rt_debug.h"
#include "rt/rt_config.h"
#include "rt/rt_pe.h"
#include "rt/rt_i2c.h"
#include "rt/rt_camera.h"
#include "rt/rt_himax.h"
#include "rt/rt_ov7670.h"
#include "rt/rt_dma.h"
#include "rt/rt_sync_mc.h"
#include "rt/rt_perf.h"
#include "rt/rt_time.h"
#include "rt/rt_timer.h"
#include "rt/rt_freq.h"
#include "rt/rt_pm.h"
#include "rt/rt_i2s.h"
#include "rt/rt_fs.h"
#include "rt/rt_error.h"
#if defined(ARCHI_UDMA_HAS_UART) && UDMA_VERSION >= 2 || defined(ARCHI_HAS_UART)
#include "rt/rt_uart.h"
#endif
#include "rt/rt_spim.h"
#include "rt/rt_rtc.h"
#if PULP_CHIP_FAMILY == CHIP_GAP
#include "rt/rt_pwm.h"
#endif
//#if defined(APB_SOC_VERSION) && APB_SOC_VERSION == 1
#include "rt/rt_pad.h"
//#endif
#ifdef GPIO_VERSION
#include "rt/rt_gpio.h"
#endif
#include "rt/rt_voltage.h"
#include "rt/rt_bridge.h"
#include "rt/rt_eeprom.h"
#include "rt/rt_task.h"

#endif
