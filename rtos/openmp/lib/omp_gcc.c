#include "pmsis.h"
#include <stdbool.h>

#define PRINTF(...)
//#define PRINTF(...) printf(__VA_ARGS__)


// =============== PARALLEL ================

__attribute__((weak)) void GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads,
	       unsigned int flags)
{
    PRINTF("GOMP_Parallel - fn: %x, data: %x, num_threads: %d, flags: %d\n",
            fn, data, num_threads, flags);

    //distribute to cluster
    pi_cl_team_fork(num_threads, fn, data);
}


// ========= BARRIER ==========
__attribute__((weak)) void GOMP_barrier (void)
{
    PRINTF("GOMP_Barrier - core id: %d\n", pi_core_id());
    pi_cl_team_barrier();
}

// ========= SINGLE ==========
__attribute__((weak)) bool GOMP_single_start(void)
{
    PRINTF("GOMP_Single - core id: %d\n", pi_core_id());
    bool result = false;
    if (!pi_core_id())
    {
        result = true;
    }
    return result;
}


// =============== CRITICAL SECTIONS ======================
__attribute__((weak)) void GOMP_critical_start (void)
{
    PRINTF("GOMP_critical_start - core id: %d\n", pi_core_id());
    pi_cl_team_critical_enter();
}

__attribute__((weak)) void GOMP_critical_end (void)
{
    PRINTF("GOMP_critical_exit- core id: %d\n", pi_core_id());
    pi_cl_team_critical_exit();
}

// ================ ATOMICS ================

__attribute__((weak)) void GOMP_atomic_start (void)
{
    PRINTF("GOMP_atomic_start - core id: %d\n", pi_core_id());
    pi_cl_team_critical_enter();
}

__attribute__((weak)) void GOMP_atomic_end (void)
{
    PRINTF("GOMP_atomic_end - core id: %d\n", pi_core_id());
    pi_cl_team_critical_exit();
}
