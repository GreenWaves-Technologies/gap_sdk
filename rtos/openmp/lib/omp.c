#include "pmsis.h"
#include "omp.h"

int omp_get_thread_num(void)
{
    return pi_core_id();
}

int omp_get_num_threads(void)
{
    return pi_cl_team_nb_cores();
}
