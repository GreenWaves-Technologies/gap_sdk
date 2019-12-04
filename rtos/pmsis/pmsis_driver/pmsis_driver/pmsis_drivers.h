#ifndef __PMSIS_DRIVER_INCLUDES_H__
#define __PMSIS_DRIVER_INCLUDES_H__

/* pmsis_api includes. */
/* Chips specifics. */
#include "pmsis/chips/gap8/perf.h"

/* Drivers. */
#include "pmsis/drivers/cpi.h"
#include "pmsis/drivers/gpio.h"
#include "pmsis/drivers/hyperbus.h"
#include "pmsis/drivers/i2c.h"
#include "pmsis/drivers/i2s.h"
#include "pmsis/drivers/pad.h"
#include "pmsis/drivers/perf.h"
#include "pmsis/drivers/pwm.h"
#include "pmsis/drivers/spi.h"
#include "pmsis/drivers/uart.h"

/* PMSIS drivers includes. */
#include "pmsis_driver/cpi/cpi_internal.h"
#include "pmsis_driver/hyper/hyperbus_internal.h"
#include "pmsis_driver/i2c/i2c_internal.h"
#include "pmsis_driver/i2s/i2s_internal.h"
#include "pmsis_driver/perf/perf.h"
#include "pmsis_driver/pmu/pmsis_pmu.h"
#include "pmsis_driver/pwm/pwm_internal.h"
#include "pmsis_driver/timer/timer.h"
#include "pmsis_driver/uart/uart_internal.h"

#include "pmsis_driver/pmsis_fc_event.h"

#endif  /* __PMSIS_DRIVER_INCLUDES_H__ */
