#ifndef __IMPLEMENTATION_SPECIFIC_DEFINES_H__
#define __IMPLEMENTATION_SPECIFIC_DEFINES_H__

#include "stdlib.h"
#include "string.h"

#if defined(__GAP8__)
#include "system_gap8.h"
#else
#if defined(__VEGA__)
#include "system_vega.h"
#else
#include "system_gap9.h"
#endif
#endif  /* __GAP8__ */

#define __INC_TO_STRING(x) #x

#define PMSIS_TASK_EVENT_KERNEL_PRIORITY     2
#define DEFAULT_MALLOC_INC                   __INC_TO_STRING(pmsis/rtos/malloc/l2_malloc.h)

// default malloc for driver structs etc (might not be compatible with udma!)
#define pi_default_malloc(x)  pi_fc_l1_malloc(x)
#define pi_default_free(x,y)  pi_fc_l1_free(x,y)
#define pi_data_malloc(x)     pmsis_l2_malloc(x)
#define pi_data_free(x,y)     pmsis_l2_malloc_free(x,y)

#define PI_TASK_IMPLEM                          \
    uint8_t destroy;

#define PI_TASK_IMPLEM_NB_DATA 9

#define CLUSTER_TASK_IMPLEM                     \
    uint32_t cluster_team_mask;

#define PMSIS_APP_MAIN int main(int argc, char *argv[])

#define PREEMPT_DISABLE\
    uint32_t __xx_irq = disable_irq();

#define PREEMPT_RESTORE\
    restore_irq(__xx_irq);

#define PREEMPT_ENABLE\
    enable_irq();

//#define PI_INLINE_OCTOSPI_LVL 1
#if defined(PI_INLINE_OCTOSPI_LVL) && (PI_INLINE_OCTOSPI_LVL != 0)
#define PI_INLINE_OCTOSPI_LVL_0 static inline
#endif  /* PI_INLINE_OCTOSPI_LVL */

#endif  /* __IMPLEMENTATION_SPECIFIC_DEFINES_H__ */
