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
#endif  /* __EMUL__ */

/* Autotiler includes. */
#include "Gap.h"
#include "kwsKernels.h"
#include "gaplib/ImgIO.h"

#ifdef __EMUL__
#define pmsis_exit exit
#define pi_l2_malloc(x) malloc(x)
#define pi_l2_free(x,y) free(x)
#ifdef PERF
#undef PERF
#endif
#endif

#define STACK_SIZE      1024

AT_HYPERFLASH_FS_EXT_ADDR_TYPE kws_L3_Flash = 0;

// Softmax always outputs Q15 short int even from 8 bit input
#ifdef KWS_16BIT
  typedef short int KWS_IMAGE_IN_T;
  L2_MEM short int *ResOut;
#else
  #ifdef KWS_8BIT
  typedef signed char KWS_IMAGE_IN_T;
  L2_MEM char *ResOut;
  #endif
#endif

char *ImageName = NULL;



L2_MEM KWS_IMAGE_IN_T *ImageIn;

int kwsCNN_layers(
		short int *__restrict__ Input_1,
		short int *__restrict__ Output_1);



static void Runkws()
{
  printf("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  kwsCNN(ImageIn, ResOut);
  printf("Runner completed\n");

#ifndef NO_IMAGE
  //Checki Results
  int rec_digit = 0;
  KWS_IMAGE_IN_T highest = ResOut[0];
  printf("Results: \n");
  for(int i = 1; i < 12; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
    printf("class %d: %d\n", i, ResOut[i]);
  }
  printf("\n");

  printf("Recognized: %d\n", rec_digit);
#else
  printf("image loading disabled so no sensible result\n");
#endif
}

void test_kws(void)
{
    printf("Entering main controller\n");

    /* Input image size. */
    unsigned int W = 40, H = 98;
    short int *ImageFromFile = (short int *) pi_l2_malloc(sizeof(KWS_IMAGE_IN_T) * W * H);
    printf("=====>imageinfile %p\n", ImageFromFile);
    if (ImageFromFile == NULL)
    {
        printf("Failed to allocate Memory for Image (%d bytes)\n", sizeof(KWS_IMAGE_IN_T) * W * H);
        pmsis_exit(-1);
    }

    printf("Reading image\n");
    //Reading Image from Bridge
    #ifdef KWS_16BIT
      img_io_out_t out_type = IMGIO_OUTPUT_SHORT;
    #endif
    #ifdef KWS_8BIT
      img_io_out_t out_type = IMGIO_OUTPUT_CHAR;
    #endif
    if (ReadImageFromFile(ImageName, W, H, 1, ImageFromFile, W*H*sizeof(KWS_IMAGE_IN_T), out_type, 0))
    {
        printf("Failed to load image %s or dimension mismatch Expects [%dx%d]\n", ImageName, W, H);
        pmsis_exit(-2);
    }
    printf("Finished reading image\n");

    #if defined(PRINT_IMAGE)
    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
	        printf("%d, ", ImageFromFile[W*i + j]);
        }
        //printf("\n");
    }
    #endif  /* PRINT_IMAGE */

    ImageIn = (KWS_IMAGE_IN_T *) ImageFromFile;

    ResOut = (KWS_IMAGE_IN_T *) pi_l2_malloc(12 * sizeof(KWS_IMAGE_IN_T));
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 12*sizeof(KWS_IMAGE_IN_T));
        pmsis_exit(-3);
    }
    
    #if !defined(__EMUL__)
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
    if (kwsCNN_Construct())
    {
        printf("Graph constructor exited with an error\n");
        pmsis_exit(-5);
    }
    
    printf("Call cluster\n");

#ifndef __EMUL__
    struct pi_cluster_task task = {0};
    task.entry = Runkws;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    Runkws();
#endif
    
    kwsCNN_Destruct();
    pi_l2_free(ResOut,12*sizeof(KWS_IMAGE_IN_T));
    
    #if defined(PERF)
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

#ifndef __EMUL__    
    // Close the cluster
    pi_cluster_close(&cluster_dev);
#endif    
    printf("Ended\n");
    pmsis_exit(0);
}

#if defined(__EMUL__) && !defined(LINK_IMAGE_HEADER)
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: kws [image_file]\n");
        exit(-1);
    }
    ImageName = argv[1];
    printf("\n\n\t *** NNTOOL KWS Example ***\n\n");
    test_kws();
}
#else
int main()
{
    #if defined(LINK_IMAGE_NAME)
        #define __XSTR(__s) __STR(__s)
        #define __STR(__s) #__s
        ImageName = __XSTR(LINK_IMAGE_NAME);
    #else
        ImageName = "./images/feature_q16_0.pgm";
    #endif  /* LINK_IMAGE_NAME */
    printf("\n\n\t *** NNTOOL KWS Example ***\n\n");
    return pmsis_kickoff((void *) test_kws);
}
#endif  /* (__EMUL__) && (LINK_IMAGE_HEADER) */
