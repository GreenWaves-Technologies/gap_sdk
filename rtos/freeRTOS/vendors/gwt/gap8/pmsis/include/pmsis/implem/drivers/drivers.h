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

#ifndef __PMSIS_DRIVERS_INCLUDES_H__
#define __PMSIS_DRIVERS_INCLUDES_H__

/* PMSIS api includes. */
/* Chips specifics. */
#include "pmsis/chips/gap8/perf.h"
#include "pmsis/chips/gap8/pad.h"
#include "pmsis/chips/gap8/gpio.h"
#include "pmsis/chips/gap8/pmu.h"

/* Drivers. */
#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/dmacpy.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2s.h"
#include "pmsis/drivers/pad.h"
#include "pmsis/drivers/perf.h"
#include "pmsis/drivers/pmu.h"
#include "pmsis/drivers/pwm.h"
#include "pmsis/drivers/rtc.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/uart.h"

/* PMSIS drivers includes. */
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/drivers/fll/fll.h"
#include "pmsis/implem/drivers/perf/perf.h"
#include "pmsis/implem/drivers/pmu/pmu_internal.h"
#include "pmsis/implem/drivers/pwm/pwm_internal.h"
#include "pmsis/implem/drivers/rtc/rtc_internal.h"
#include "pmsis/implem/drivers/timer/timer.h"
#include "pmsis/implem/drivers/udma/cpi/cpi_internal.h"
#include "pmsis/implem/drivers/udma/dmacpy/dmacpy_internal.h"
#include "pmsis/implem/drivers/udma/hyperbus/hyperbus_internal.h"
#include "pmsis/implem/drivers/udma/i2c/i2c_internal.h"
#include "pmsis/implem/drivers/udma/i2s/i2s_internal.h"
#include "pmsis/implem/drivers/udma/uart/uart_internal.h"

#endif  /* __PMSIS_DRIVERS_INCLUDES_H__ */
