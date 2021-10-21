
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */


/* Autotiler includes. */
#include "ssd_model.h"
#include "ssd_modelKernels.h"
#include "gaplib/ImgIO.h"
#ifndef __EMUL__
    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s 
#endif
#ifdef __EMUL__
#define pmsis_exit(n) exit(n)
#endif

#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif

AT_HYPERFLASH_FS_EXT_ADDR_TYPE ssd_model_L3_Flash = 0;

/* Inputs */
/* Outputs */
//L2_MEM bbox_t output_1[40];
L2_MEM short int output_1[40];
L2_MEM signed char output_2[10];
L2_MEM signed char output_3[10];
char *ImageName;

static void cluster()
{
    #ifdef PERF
    printf("Start timer\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

    ssd_modelCNN(output_1, output_2, output_3);
    printf("Runner completed\n");
    for (int i=0; i<10; i++) {
        //printf("CLASS: %d@%d BOX: %d %d %d %d\n", output_1[i].class, output_1[i].score, output_1[i].x, output_1[i].y, output_1[i].w, output_1[i].h);
        printf("CLASS: %3d@%3d BOX: %5d %5d %5d %5d\n", output_2[i], output_3[i], output_1[4*i], output_1[4*i+1], output_1[4*i]+output_1[4*i+2], output_1[4*i+1]+output_1[4*i+3]);
    }

}

int test_ssd_model(void)
{
    printf("Entering main controller\n");
    

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
#endif
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    printf("Constructor\n");
    int ConstructorErr = ssd_modelCNN_Construct();
    if (ConstructorErr)
    {
        printf("Graph constructor exited with error: %d\n(check the generated file ssd_modelKernels.c to see which memory have failed to be allocated)\n", ConstructorErr);
        pmsis_exit(-6);
    }   
    #if defined(MODEL_HWC) || defined(MODEL_NE16)
    int Traspose2CHW = 0;
    #else
    int Traspose2CHW = 1;
    #endif
    printf("Reading image: %d\n", Traspose2CHW);
    if (ReadImageFromFile(ImageName, 300, 300, 3, Input_1, 300*300*3*sizeof(char), IMGIO_OUTPUT_CHAR, Traspose2CHW)) {
        printf("Failed to load image %s\n", ImageName);
        pmsis_exit(-1);
    }
    #ifndef MODEL_NE16
        for (int i=0; i<300*300*3; i++) Input_1[i] = Input_1[i] - 128;
    #endif



    printf("Call cluster\n");
#ifndef __EMUL__
    struct pi_cluster_task task = {0};
    task.entry = cluster;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    task.slave_stack_size = (unsigned int) SLAVE_STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
    ssd_modelCNN_Destruct();
#else
    cluster();
#endif
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
    ImageName = __XSTR(AT_IMAGE);
    printf("\n\n\t *** NNTOOL ssd_model Example ***\n\n");
    return pmsis_kickoff((void *) test_ssd_model);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./exe [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    printf("\n\n\t *** NNTOOL ssd_model Example ***\n\n");
    test_ssd_model();
    return 0;
}
#endif