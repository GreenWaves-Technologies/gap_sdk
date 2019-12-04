#ifndef __PMSIS_PMU_H__
#define __PMSIS_PMU_H__
#include <stdint.h>


/* PMU State:
   State:          SOC_LP/HP, SOC_CLUSTER_LP/HP, RETENTIVE, DEEP_SLEEP.
   Bits[2:1]:      Regulator State, Bit[0], Cluster On/Off
   FllClusterDown  Cluster FLL stop mode: FLL_FAST_WAKEUP, FLL_RETENTIVE or FLL_SHUTDOWN
   DCDC_Settings[]
   REGU_NV         Nominal,   BaseValue + Incr*ValueNv             BaseValue and Incr are constants (550, 50)
   REGU_LV         Low,       BaseValue + Incr*ValueLv             BaseValue and Incr are constants (550, 50)
   REGU_RET        Retentive, BaseValue + Incr*ValueRet            BaseValue and Incr are constants (550, 50)
   REGU_OFF        0
   Fll SoC:        Frequency:      Value in Hz
   Fll Cluster:    Frequency:      Value in Hz
*/
typedef enum _pmu_fll_cluster_down {
  PMU_FLL_FAST_WAKEUP=0,    /* Fll is kept active and ready to be used */
  PMU_FLL_RETENTIVE=1,      /* Fll is stopped and it's state is kept in retention */
  PMU_FLL_SHUTDOWN=2        /* Fll is stopped and has to be re initialized when restarting */
} pmu_fll_cluster_down_t;



typedef struct _pmu_state {
  uint8_t   State;
  uint8_t   FllClusterDown;
  uint8_t   DCDC_Settings[4];
  uint32_t  frequency[2];
} pmu_state_t;
#include "pmu_gap8.h"
// TODO:port gap8 pmu and put include here

#endif
