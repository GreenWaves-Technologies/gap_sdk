#include "pmsis.h"

#define PRINTF(...)
//#define PRINTF(...) printf(__VA_ARGS__)


// =============== PARALLEL ================

void GOMP_parallel (void (*fn) (void *), void *data, unsigned num_threads,
	       unsigned int flags)
{
    PRINTF("GOMP_Parallel - fn: %x, data: %x, num_threads: %d, flags: %d\n",
            fn, data, num_threads, flags);

    //distribute to cluster
    pi_cl_team_fork(num_threads, fn, data);
}


// ========= BARRIER ==========
void GOMP_barrier (void)
{
    PRINTF("GOMP_Barrier - core id: %d\n", pi_core_id());
    pi_cl_team_barrier();
}


// =============== CRITICAL SECTIONS ======================
void GOMP_critical_start (void)
{
    PRINTF("GOMP_critical_start - core id: %d\n", pi_core_id());
    pi_cl_team_critical_enter();
}

void GOMP_critical_end (void)
{
    PRINTF("GOMP_critical_exit- core id: %d\n", pi_core_id());
    pi_cl_team_critical_exit();
}

// ================ ATOMICS ================

void GOMP_atomic_start (void)
{
    PRINTF("GOMP_atomic_start - core id: %d\n", pi_core_id());
    pi_cl_team_critical_enter();
}

void GOMP_atomic_end (void)
{
    PRINTF("GOMP_atomic_end - core id: %d\n", pi_core_id());
    pi_cl_team_critical_exit();
}
