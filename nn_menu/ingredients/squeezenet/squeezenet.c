/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#ifndef __EMUL__
  /* PMSIS includes. */
  #include "pmsis.h"
#endif
#include "squeezenet.h"
#include "squeezenetKernels.h"
#include "gaplib/ImgIO.h"

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s

#define AT_INPUT_SIZE (AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)
#define NUM_CLASSES 1001

AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;
int max_class;
int max_value;

// Softmax always outputs Q15 short int even from 8 bit input
L2_MEM short int *ResOut;
typedef unsigned char IMAGE_IN_T;
char *ImageName;
#ifdef __EMUL__
unsigned char * __restrict__ Input_1;
#else
extern  unsigned char * __restrict__ Input_1;
#endif

static void RunNetwork()
{
  printf("Running on cluster\n");
#ifdef PERF
  printf("Start timer\n");
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif

#ifdef __EMUL__
  __PREFIX(CNN)(Input_1, ResOut);
#else
  __PREFIX(CNN)(ResOut);
#endif
  printf("Runner completed\n");

  printf("\n");
  
  //Checki Results
  max_class=0;
  max_value=0;
  for (int i=0;i<NUM_CLASSES;i++){
    if(ResOut[i]>max_value){
      max_value=ResOut[i];
      max_class=i;
    }
  }

  printf("Class detected: %d, with value: %d\n", max_class, max_value);
  //FIX2FP(max_value,15);
  printf("\n");
}

int start()
{
  #ifdef __EMUL__
    Input_1 = AT_L2_ALLOC(0, AT_INPUT_SIZE*sizeof(IMAGE_IN_T));
    if (Input_1==0){
      printf("error allocating %ld \n", AT_INPUT_SIZE*sizeof(IMAGE_IN_T));
      return 1;
    }
  #else
    ImageName = __XSTR(AT_IMAGE);
    pi_freq_set(PI_FREQ_DOMAIN_FC,250000000);
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    pi_cluster_conf_init(&conf);
    pi_open_from_conf(&cluster_dev, (void *)&conf);
    if (pi_cluster_open(&cluster_dev))
      {
          printf("Cluster open failed !\n");
          pmsis_exit(-4);
      }
    pi_freq_set(PI_FREQ_DOMAIN_CL,175000000);
    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));

    if(task==NULL) {
      printf("pi_cluster_task alloc Error!\n");
      pmsis_exit(-1);
    }

    task->entry = &RunNetwork;
    task->stack_size = STACK_SIZE;
    task->slave_stack_size = SLAVE_STACK_SIZE;
    task->arg = NULL;
    printf("Stack sizes: %d %d\n", STACK_SIZE, SLAVE_STACK_SIZE);
  #endif

  ResOut = (short int *) AT_L2_ALLOC(0, NUM_CLASSES*sizeof(short int));
  if (ResOut==0) {
    printf("Failed to allocate Memory for Result (%ld bytes)\n", NUM_CLASSES*sizeof(short int));
    return 1;
  }

  printf("Constructor\n");
  // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
  if (__PREFIX(CNN_Construct)())
  {
    printf("Graph constructor exited with error\n");
    return 1;
  }

  printf("Reading image\n");
  if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS,
                        Input_1, AT_INPUT_SIZE*sizeof(IMAGE_IN_T), IMGIO_OUTPUT_CHAR, 0)) {
    printf("Failed to load image %s\n", ImageName);
    return 1;
  }
  printf("Finished reading image\n");

  #ifdef __EMUL__
    RunNetwork();
  #else
    pi_cluster_send_task_to_cl(&cluster_dev, task);
  #endif
  
  __PREFIX(CNN_Destruct)();
  #ifndef __EMUL__
    pi_cluster_close(&cluster_dev);
  #endif

  #ifdef PERF
  	unsigned int TotalCycles = 0, TotalOper = 0;
  	printf("\n");
  	for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
  		printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
  		TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
  	}
  	printf("\n");
  	printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
  	printf("\n");

    #ifdef GROUND_TRUTH
    if (max_class != GROUND_TRUTH) {printf("Error class predicted: %d ground truth: %d\n", max_class, GROUND_TRUTH); pmsis_exit(-1);}
    else                           printf("Correct prediction\n");
    #endif
  #endif

  printf("Ended\n");
  #ifdef __EMUL__
    return 0;
  #else
    pmsis_exit(0);
  #endif
}

#ifndef __EMUL__
int main(void)
{
  printf("\n\n\t *** NNTOOL SQUEEZENET ***\n\n");
  return pmsis_kickoff((void *) start);
}
#else
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
      PRINTF("Usage: squeezenet [image_file]\n");
      exit(-1);
  }
  ImageName = argv[1];
  PRINTF("\n\n\t *** NNTOOL SQUEEZENET emul ***\n\n");
  start();
  return 0;
}
#endif

