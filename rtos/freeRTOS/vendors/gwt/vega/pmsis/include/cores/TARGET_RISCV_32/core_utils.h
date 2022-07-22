#ifndef __PMSIS_CORE_UTILS__
#define __PMSIS_CORE_UTILS__


/** FC_CLUSTER_ID Definitions */
#define FC_CLUSTER_ID                 31                /**< FC CLuster ID */
#define FC_CORE_ID                    0                /**< FC CLuster ID */

/**
  \ingroup  CMSIS_Core_IDFunctionInterface
  \defgroup CMSIS_Core_IDFunctions ID Functions
  \brief    Functions that manage Cluster and Core ID.
  @{
 */

static inline uint32_t __native_core_id() {
  int hart_id;
  asm volatile ("csrr %0, 0x014" : "=r" (hart_id) : );
  return hart_id & 0x1f;
}

static inline uint32_t __native_cluster_id() {
  int hart_id;
  asm volatile ("csrr %0, 0x014" : "=r" (hart_id) : );
  return (hart_id >> 5) & 0x3f;
}

static inline uint32_t __native_is_fc() {
  return ( __native_cluster_id() == FC_CLUSTER_ID);
}

#define FF1(x)            __builtin_pulp_ff1((x))
#define FL1(x)            __builtin_pulp_fl1((x))

#endif
