
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


/* Autotiler includes. */
#include "wav2letter_int8.h"
#include "wav2letter_int8Kernels.h"

#ifdef __EMUL__
#define pmsis_exit(n) exit(n)
#endif

#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif

AT_HYPERFLASH_FS_EXT_ADDR_TYPE wav2letter_int8_L3_Flash = 0;

/* Inputs */
L2_MEM signed char input_1[11544];
/* Outputs */
L2_MEM short int output_1[4292];

static void cluster()
{
    #ifdef PERF
    printf("Start timer\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

    wav2letter_int8CNN(input_1, output_1);
    printf("Runner completed\n");
}

int test_wav2letter_int8(void)
{
    printf("Entering main controller\n");
    /* ----------------> 
     * Put here Your input settings
     * <---------------
     */
    

#ifndef __EMUL__
    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }
#endif
    int cur_fc_freq = pi_freq_set(PI_FREQ_DOMAIN_FC, 250000000);
    if (cur_fc_freq == -1)
    {
        printf("Error changing frequency !\nTest failed...\n");
        pmsis_exit(-4);
    }

    int cur_cl_freq = pi_freq_set(PI_FREQ_DOMAIN_CL, 175000000);
    if (cur_cl_freq == -1)
    {
        printf("Error changing frequency !\nTest failed...\n");
        pmsis_exit(-5);
    }
#ifdef __GAP9__
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 250000000);
#endif
    printf("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (wav2letter_int8CNN_Construct())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-6);
    }

    printf("Call cluster\n");
#ifndef __EMUL__
    struct pi_cluster_task task = {0};
    task.entry = cluster;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    task.slave_stack_size = (unsigned int) SLAVE_STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    cluster();
#endif

    wav2letter_int8CNN_Destruct();

#ifdef PERF
    {
      unsigned int TotalCycles = 0, TotalOper = 0;
      printf("\n");
      for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
        TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
      }
      printf("\n");
      printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
      printf("\n");
    }
#endif

    printf("Ended\n");
    pmsis_exit(0);
    return 0;
}

#ifndef __EMUL__
int main()
{
    printf("\n\n\t *** NNTOOL wav2letter_int8 Example ***\n\n");
    return pmsis_kickoff((void *) test_wav2letter_int8);
}
#else
int main(int argc, char *argv[])
{
    printf("\n\n\t *** NNTOOL wav2letter_int8 Example ***\n\n");
    test_wav2letter_int8();
    return 0;
}
#endif