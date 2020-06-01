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
#include "ImgIO.h"

#if defined(__PULP_OS__)
#include "bridge_stubs.h"
#endif  /* __PULP_OS__ */

#ifdef __EMUL__
#define pmsis_exit exit
#define pi_l2_malloc(x) malloc(x)
#define pi_l2_free(x,y) free(x)
#ifdef PERF
#undef PERF
#endif
#endif

/*L2_MEM short int in_img_txt[] = { 
#include "./in_feat.txt"
  };*/
  
#define STACK_SIZE      1024

AT_HYPERFLASH_FS_EXT_ADDR_TYPE kws_L3_Flash = 0;

#ifdef __EMUL__
  #include <sys/types.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <sys/param.h>
  #include <string.h>
  #ifndef TENSOR_DUMP_FILE
    #define TENSOR_DUMP_FILE "tensor_dump_file.dat"
  #endif
#endif

// Softmax always outputs Q15 short int even from 8 bit input
L2_MEM short int *ResOut;
#ifdef KWS_16BIT
  typedef short int KWS_IMAGE_IN_T;
#else
  #ifdef KWS_8BIT
  typedef signed char KWS_IMAGE_IN_T;
  #endif
#endif


L2_MEM KWS_IMAGE_IN_T *ImageIn;
  
char *ImageName = NULL;

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
  short int highest = ResOut[0];
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

    unsigned int Wi = 0, Hi = 0;
    /* Input image size. */
    unsigned int W = 40, H = 98;
    #if !defined(__EMUL__)
    #if !defined(NO_IMAGE) && !defined(LINK_IMAGE_HEADER)
    BRIDGE_Init();
    printf("Connecting to bridge !\n");
    BRIDGE_Connect(1, NULL);
    printf("Connected to bridge !\n");
    #endif  /* NO_IMAGE && LINK_IMAGE_HEADER */
    #endif  /* __EMUL__ */

    unsigned char *ImageInChar = (unsigned char *) pi_l2_malloc(sizeof(KWS_IMAGE_IN_T) * W * H);
    printf("=====>imageinchar %p\n",ImageInChar);
    if (ImageInChar == NULL)
    {
        printf("Failed to allocate Memory for Image (%d bytes)\n", sizeof(KWS_IMAGE_IN_T) * W * H);
        pmsis_exit(-1);
    }

    printf("Reading image\n");
    //Reading Image from Bridge
    if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageInChar, W*H*sizeof(short int))==0) || (Wi!=W) || (Hi!=H))
    {
        printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
        pmsis_exit(-2);
    }
    printf("Finished reading image\n");

    #if defined(PRINT_IMAGE)
    for (int i=0; i<H; i++)
    {
      printf("%d:\t",i*W);
        for (int j=0; j<W; j++)
        {
	        printf("%04d, ", ((short int*)ImageInChar)[W*i + j]<<6);
        }
        printf("\n");
    }
    #endif  /* PRINT_IMAGE */

    ImageIn = (KWS_IMAGE_IN_T *) ImageInChar;


    ResOut = (short int *) pi_l2_malloc(12 * sizeof(short int));
    if (ResOut == NULL)
    {
        printf("Failed to allocate Memory for Result (%d bytes)\n", 10*sizeof(short int));
        pmsis_exit(-3);
    }

    #if defined(KWS_8BIT)
    printf("Preprocess 8 bit\n");
    for (int i=W*H-1; i>=0; i--)
    {
        ImageIn[i] = (KWS_IMAGE_IN_T) (ImageInChar[i]>>1);
    }
    #else
    #if defined(KWS_16BIT)
    printf("Preprocess 16 bit\n");
    for (int i=W*H-1; i>=0; i--)
    {
        ImageIn[i] = ImageInChar[i] << 7;
    }
    #else
    #error No bit size selected
    #endif  /* KWS_16BIT */
    #endif  /* KWS_8BIT */
    
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
    pi_l2_free(ResOut,12*sizeof(short int));
    
    #if defined(PERF)
    {
        unsigned int TotalCycles = 0, TotalOper = 0;
        printf("\n");
        for (unsigned int i=0; i<(sizeof(kwsPerf)/sizeof(unsigned int)); i++)
        {
            printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", kwsLName[i], kwsPerf[i], kwsOperCount[i], ((float) kwsOperCount[i])/ kwsPerf[i]);
            TotalCycles += kwsPerf[i]; TotalOper += kwsOperCount[i];
        }
        printf("\n");
        printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
        printf("\n");
    }
    #endif  /* PERF */

#ifndef __EMUL__    
    // Close the cluster
    pi_cluster_close(&cluster_dev);
#endif
    
#if defined(__EMUL__)

#else
#if !defined(NO_IMAGE) && !defined(LINK_IMAGE_HEADER)
    BRIDGE_Disconnect(NULL);
#endif  /* NO_IMAGE && LINK_IMAGE_HEADER */
#endif  /* __EMUL__ */

    
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
    #define __STRING1(__s) #__s
  //    #define __STRING(__s) __STRING1(__s)
    ImageName = __STRING(LINK_IMAGE_NAME);
    #else
    ImageName = "./images/feature_0_1.pgm";
    #endif  /* LINK_IMAGE_NAME */
    printf("\n\n\t *** NNTOOL KWS Example ***\n\n");
    return pmsis_kickoff((void *) test_kws);
}
#endif  /* (__EMUL__) && (LINK_IMAGE_HEADER) */
