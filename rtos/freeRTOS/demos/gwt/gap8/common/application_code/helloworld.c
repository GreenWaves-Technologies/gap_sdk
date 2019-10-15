/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
PI_L2 char hello[20];

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    sprintf(hello, "[%d %d] Hello World!\n", cluster_id, core_id);
    printf(hello);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** FreeRTOS HelloWorld *** \n\n");
    return pmsis_kickoff((void *) helloworld);
}

