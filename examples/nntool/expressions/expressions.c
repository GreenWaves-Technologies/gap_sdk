/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s
/* PMSIS includes. */
#ifndef __EMUL__
	#include "pmsis.h"
#endif

/* Autotiler includes. */
#include "expressions.h"
#include "expressionsKernels.h"
#include "gaplib/fs_switch.h"

#define AT_INPUT_SIZE	80*12
#define AT_OUTPUT_SIZE	80*12

#if defined(USE_HYPER) || defined(__EMUL__)
   AT_HYPERFLASH_FS_EXT_ADDR_TYPE expressions_L3_Flash = 0;
#elif defined(USE_SPI)
   AT_QSPIFLASH_FS_EXT_ADDR_TYPE expressions_L3_Flash = 0;
#endif

signed char *Input_1;
signed char *Input_2;
signed char *Input_3;
signed char *Input_4;
signed char *Output_1;
signed char *Output_2;
char *FileName;

static void RunExpressions(){
    printf("Running on cluster\n");
#ifdef PERF
    printf("Start timer\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
#endif
    __PREFIX(CNN)(Input_1, Input_2, Input_3, Input_4, Output_1, Output_2);
    printf("Runner completed\n");
}

int body(void){
    Input_1 = (signed char *) AT_L2_ALLOC(0, AT_INPUT_SIZE);
    Input_2 = (signed char *) AT_L2_ALLOC(0, AT_INPUT_SIZE);
    Input_3 = (signed char *) AT_L2_ALLOC(0, AT_INPUT_SIZE);
    Input_4 = (signed char *) AT_L2_ALLOC(0, AT_INPUT_SIZE);
    if (Input_1==NULL || Input_2==NULL || Input_3==NULL || Input_4==NULL){
        printf("Error allocating one of the Inputs\n");
        return 1;
    }

    switch_file_t File = (switch_file_t) 0;
    switch_fs_t fs;
    __int_ssize_t len;
    __FS_INIT(fs);

    FileName = __XSTR(INPUT_1);
    File = __OPEN_READ(fs, FileName);
    if (File == 0) {
        printf("Failed to open file, samples/input_0.dat\n"); return 1;
    }
    len = __READ(File, Input_1, AT_INPUT_SIZE); if (!len) return 1;

    FileName = __XSTR(INPUT_2);
    File = __OPEN_READ(fs, FileName);
    if (File == 0) {
        printf("Failed to open file, samples/input_1.dat\n"); return 1;
    }
    len = __READ(File, Input_2, AT_INPUT_SIZE); if (!len) return 1;

    FileName = __XSTR(INPUT_3);
    File = __OPEN_READ(fs, FileName);
    if (File == 0) {
        printf("Failed to open file, samples/input_2.dat\n"); return 1;
    }
    len = __READ(File, Input_3, AT_INPUT_SIZE); if (!len) return 1;

    FileName = __XSTR(INPUT_4);
    File = __OPEN_READ(fs, FileName);
    if (File == 0) {
        printf("Failed to open file, samples/input_3.dat\n"); return 1;
    }
    len = __READ(File, Input_4, AT_INPUT_SIZE); if (!len) return 1;

    Output_1 = (signed char *) AT_L2_ALLOC(0, AT_OUTPUT_SIZE);
    Output_2 = (signed char *) AT_L2_ALLOC(0, AT_OUTPUT_SIZE);
    if (Output_1==NULL || Output_2==NULL){
        printf("Error allocating one of the Inputs\n");
        return 1;
    }

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

    printf("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (__PREFIX(CNN_Construct)())
    {
        printf("Graph constructor exited with an error\n");
        return 1;
    }

#ifndef __EMUL__
    printf("Call cluster\n");
    struct pi_cluster_task task = {0};
    task.entry = RunExpressions;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    task.slave_stack_size = (unsigned int) SLAVE_STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    RunExpressions();
#endif

    printf("\nOutput_1: \n");
    for (int i=0; i<AT_OUTPUT_SIZE; i++){
        printf("%d, ", Output_1[i]);
    }
    printf("\nOutput_2: \n");
    for (int i=0; i<AT_OUTPUT_SIZE; i++){
        printf("%d, ", Output_2[i]);
    }
    printf("\n");

    __PREFIX(CNN_Destruct)();

    #ifndef __EMUL__
        // Close the cluster
        pi_cluster_close(&cluster_dev);
    #endif

    AT_L2_FREE(0, Input_1, AT_INPUT_SIZE);
    AT_L2_FREE(0, Input_2, AT_INPUT_SIZE);
    AT_L2_FREE(0, Input_3, AT_INPUT_SIZE);
    AT_L2_FREE(0, Input_4, AT_INPUT_SIZE);
    AT_L2_FREE(0, Output_1, AT_OUTPUT_SIZE);
    AT_L2_FREE(0, Output_2, AT_OUTPUT_SIZE);
    printf("Ended\n");

    pmsis_exit(0);
    return 0;
}

int main()
{
    printf("\n\n\t *** NNTOOL expressions Example ***\n\n");
#ifdef __EMUL__
    body();
#else
    return pmsis_kickoff((void *) body);
#endif
}
