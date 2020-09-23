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

/* This file shall be used for verification purpose only. */

#ifndef __PMSIS_HAL_H__
#define __PMSIS_HAL_H__

/* Hal includes. */

/* Event unit. */
#include "event_unit/event_unit.h"

/* Efuse */
#include "efuse/pmsis_efuse.h"

/* GPIO. */
#include "gpio/gpio.h"

/* PWM */
#include "pwm/pwm.h"
#include "pwm/pwm_ctrl.h"

/* RTC */
#include "rtc/rtc.h"

/* Soc_ctrl. */
#include "soc_ctrl/pmu_ctrl.h"
#include "soc_ctrl/soc_ctrl_pad.h"

/* PMU. */
#include "pmu/pmu_dlc.h"

/* Soc events generator. */
#include "soc_eu/soc_eu.h"

/* UDMA. */
#include "udma/udma_core.h"
#include "udma/udma_cpi.h"
#include "udma/udma_ctrl.h"
#include "udma/udma_dmacpy.h"
#include "udma/udma_hyper.h"
#include "udma/udma_i2c.h"
#include "udma/udma_i2s.h"
#include "udma/udma_spim.h"
#include "udma/udma_uart.h"

/* Cluster. */
#include "pmsis/implem/hal/cluster/cluster_event_unit.h"
/* DMA. */
#include "dma/dma.h"

#endif  /* __PMSIS_HAL_H__ */
