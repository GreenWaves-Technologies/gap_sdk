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
#include "pmsis_backend/implementation_specific_defines.h"
#include "targets/pmsis_targets.h"
#include "system_gap8.h"
/* pmsis_api includes. */
#include "pmsis/device.h"
#include "pmsis/task.h"
#include "pmsis/pmsis_types.h"
#include "pmsis/rtos/pmsis_os.h"
#include "pmsis/rtos/pmsis_assert.h"

/* PMSIS includes. */
#include "pmsis_driver/pmsis_driver_data.h"
#include "pmsis_driver/pmsis_drivers.h"
#include "pmsis_rtos/os/pmsis_freq.h"
#if defined(FEATURE_CLUSTER)
#include "pmsis_cluster/pmsis_cluster.h"
#endif  /* FEATURE_CLUSTER */

#endif  /* HW_VERIF_ONLY */

/* Hal includes. */
#include "pmsis_hal/pmsis_hal.h"

#endif  /* __PMSIS_H__ */
