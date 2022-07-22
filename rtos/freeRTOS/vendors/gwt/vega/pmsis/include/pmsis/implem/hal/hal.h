/*
 * Copyright (c) 2021, GreenWaves Technologies, Inc.
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

#pragma once

#include "pmsis/targets/target.h"

/* Hal includes. */

/* FLL. */
#include "pmsis/implem/hal/fll/fll.h"

/* GPIO. */
#include "pmsis/implem/hal/gpio/gpio.h"

/* UDMA. */
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"
#include "pmsis/implem/hal/udma/udma_hyper.h"
#include "pmsis/implem/hal/udma/udma_i2c.h"
#include "pmsis/implem/hal/udma/udma_i2s.h"
#include "pmsis/implem/hal/udma/udma_uart.h"
//#include "pmsis/implem/hal/udma/udma_cpi.h"
//#include "pmsis/implem/hal/udma/udma_spim.h"

/* SOC_CTRL. */
#include "pmsis/implem/hal/soc_ctrl/pmu_ctrl.h"
#include "pmsis/implem/hal/soc_ctrl/soc_ctrl_pad.h"

/* PWM */
#include "pmsis/implem/hal/pwm/pwm.h"
#include "pmsis/implem/hal/pwm/pwm_ctrl.h"

/* SOC_EU. */
#include "pmsis/implem/hal/soc_eu/soc_eu.h"

/* PMU. */
#include "pmsis/implem/hal/pmu/pmu.h"

/* RTC */
#include "pmsis/implem/hal/rtc/rtc.h"

/* FC_ICACHE. */
#include "pmsis/implem/hal/fc_icache/fc_icache.h"

/* ITC */
#include "pmsis/implem/hal/itc/itc.h"

/* I3C */
#include "pmsis/implem/hal/i3c/i3c.h"

/* Efuse */
#include "pmsis/implem/hal/efuse/pmsis_efuse.h"


/* Cluster. */
#include "pmsis/implem/hal/cluster/cluster_ctrl_unit.h"
#include "pmsis/implem/hal/cluster/cluster_event_unit.h"
#include "pmsis/implem/hal/cluster/cluster_icache_ctrl.h"
#include "pmsis/implem/hal/cluster/cluster_dma.h"


/** STDOUT access. */
static inline void hal_stdout_putc_set(uint32_t cluster_id, uint32_t core_id, uint8_t c)
{
    uint32_t base = (uint32_t) _stdout(cluster_id);
    GAP_WRITE(base, STDOUT_PUTC_OFFSET + (core_id << 3), c);
}
