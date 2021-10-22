/* PMSIS includes */
#include "pmsis.h"

void run_cli(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** FreeRTOS CLI ***\n\n");
    return pmsis_kickoff((void *) run_cli);
}

