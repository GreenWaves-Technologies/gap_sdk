#ifndef __PMSIS_FLL_H__
#define __PMSIS_FLL_H__



#ifdef __GAP9__
typedef enum _fll_type {
  FLL_SOC=0,
  FLL_PERI=1,
  FLL_CLUSTER=2,
} fll_type_t;
#include "fll_gap9.h"
#else
typedef enum _fll_type {
  FLL_SOC=0,
  FLL_PERI=0,
  FLL_CLUSTER=1,
} fll_type_t;
#include "fll_gap8.h"
#endif

#endif
