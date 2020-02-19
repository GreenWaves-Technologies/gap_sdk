#ifndef __PMSIS_BACKEND_NATIVE_PMU_API_H__
#define __PMSIS_BACKEND_NATIVE_PMU_API_H__

#include "pmsis_types.h"
#include "FreeRTOS_util.h"
#include "pmsis_gcc.h"
#include "pmsis_backend_native_types.h"
#include "gap_pmu.h"

static inline void __os_native_pmu_cluster_poweroff(void)
{
    PMU_ClusterPowerOff();
}

static inline void __os_native_pmu_cluster_poweron(void)
{
    PMU_ClusterPowerOn();
}

#endif
