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
/* FLL. */
#include "fll/pmsis_fll.h"

/* Event unit. */
#include "gap_eu/pmsis_eu.h"

/* GPIO. */
#include "gpio/gpio.h"

/* ITC. */
#include "itc/pmsis_itc.h"

/* Perf counters. */
//#include "perf/pmsis_perf.h"

/* PMU. */
#if defined(__GAP9__)
#include "pmu_v3.h"
#endif

/* Soc_ctrl. */
#include "soc_ctrl/soc_ctrl_pad.h"

/* Soc events generator. */
#include "soc_eu/pmsis_soc_eu.h"

/* UDMA. */
#include "udma/udma_core.h"
#include "udma/udma_cpi.h"
#include "udma/udma_ctrl.h"
#include "udma/udma_hyper.h"
#include "udma/udma_i2c.h"
#include "udma/udma_sdio.h"
#include "udma/udma_spim/udma_spim.h"
#include "udma/udma_uart.h"

#endif  /* __PMSIS_HAL_H__ */
