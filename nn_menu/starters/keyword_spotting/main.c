/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __EMUL__
	/* PMSIS includes. */
	#include "pmsis.h"
#else
	#define pmsis_exit exit
	#define pi_l2_malloc malloc
	#define pi_l2_free(ptr,size) free(ptr)
#endif  /* __EMUL__ */

/* Autotiler includes. */
#include "Gap.h"
#ifdef SMALL
	#include "KWS_ds_cnn_s_quantKernels.h"
#endif
#ifdef MEDIUM
    #include "KWS_ds_cnn_m_quantKernels.h"
#endif
#ifdef LARGE
	#include "KWS_ds_cnn_l_quantKernels.h"
#endif
#include "gaplib/ImgIO.h"

typedef signed char KWS_IMAGE_IN_T;
L2_MEM short int *ResOut;
char *ImageName = NULL;
L2_MEM KWS_IMAGE_IN_T *ImageIn;
AT_HYPERFLASH_FS_EXT_ADDR_TYPE __PREFIX(_L3_Flash) = 0;

static void Runkws()
{
  printf("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  __PREFIX(CNN)(ImageIn, ResOut);
  printf("Runner completed\n");

  //Checki Results
  int rec_digit = 0;
  int highest = ResOut[0];
  printf("Results: \n");
  for(int i = 0; i < 12; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
    printf("class %d: %d\n", i, ResOut[i]);
  }
  printf("\n");

  printf("Recognized:\t%d\n", rec_digit);
}


void kws_ds_cnn(void)
{
    printf("Entering main controller\n");

    /* Input image size. */
    unsigned int W = AT_INPUT_WIDTH, H = AT_INPUT_HEIGHT;
    char *ImageFromFile = (char *) pi_l2_malloc(sizeof(KWS_IMAGE_IN_T) * W * H);
    if (ImageFromFile == NULL){
        printf("Failed to allocate Memory for Image (%d bytes)\n", sizeof(KWS_IMAGE_IN_T) * W * H);
        pmsis_exit(-1);
    }
    printf("Reading image\n");
    if (ReadImageFromFile(ImageName, W, H, 1, ImageFromFile, W*H*sizeof(KWS_IMAGE_IN_T), IMGIO_OUTPUT_CHAR, 0)){
        printf("Failed to load image %s or dimension mismatch Expects [%dx%d]\n", ImageName, W, H);
        pmsis_exit(-2);
    }
    printf("Finished reading image\n");

    ImageIn = (KWS_IMAGE_IN_T *) ImageFromFile;

    #ifdef PRINT_IMAGE
        printf("input_at = np.arrray([\n");
        for (int i=0; i<49; i++) {
            printf("[");
            for (int j=0; j<10; j++) {
                printf("%d, ", ImageIn[i*10+j]);
            }
            printf("],\n");
        }
        printf("])\n");
    #endif  /* PRINT_IMAGE */

    ResOut = (KWS_IMAGE_IN_T *) pi_l2_malloc(12 * sizeof(KWS_IMAGE_IN_T));
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 12*sizeof(KWS_IMAGE_IN_T));
        pmsis_exit(-3);
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
    #endif  /* __EMUL__ */

    printf("Constructor\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    int err_construct = __PREFIX(CNN_Construct)();
    if (err_construct)
    {
        printf("Graph constructor exited with error: %d\n", err_construct);
        pmsis_exit(-5);
    }

    printf("Call cluster\n");
	#ifndef __EMUL__
	    // struct pi_cluster_task task;
	    // task.entry = &Runkws;
	    // task.arg = NULL;
	    // task.stack_size = (unsigned int) STACK_SIZE;
	    // pi_cluster_send_task_to_cl(&cluster_dev, &task);

		struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
		if(task==NULL) {
		  printf("pi_cluster_task alloc Error!\n");
		  pmsis_exit(-1);
		}
		printf("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
		memset(task, 0, sizeof(struct pi_cluster_task));
		task->entry = &Runkws;
		task->stack_size = STACK_SIZE;
		task->slave_stack_size = SLAVE_STACK_SIZE;
		task->arg = NULL;
		pi_cluster_send_task_to_cl(&cluster_dev, task);
	#else
	    Runkws();
	#endif
    #ifdef PERF
    {
        unsigned int TotalCycles = 0, TotalOper = 0;
        printf("\n");
        for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
            printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i],
                   AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
            TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
        }
        printf("\n");
        printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
        printf("\n");
    }
    #endif  /* PERF */

    __PREFIX(CNN_Destruct)();
    pi_l2_free(ResOut,12*sizeof(KWS_IMAGE_IN_T));

    #ifndef __EMUL__    
        // Close the cluster
        pi_cluster_close(&cluster_dev);
    #endif
    printf("Ended\n");
    pmsis_exit(0);
}


#ifndef __EMUL__
int main()
{
	printf("\n\n\t *** NNTOOL KWS Example ***\n\n");

    #define __XSTR(__s) __STR(__s)
    #define __STR(__s) #__s
    ImageName = __XSTR(AT_IMAGE);
    return pmsis_kickoff((void *) kws_ds_cnn);
}
#else
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: mnist [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    printf("\n\n\t *** IMAGENET EMUL ***\n\n");
    kws_ds_cnn();
    return 0;
}
#endif