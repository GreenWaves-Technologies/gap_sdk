/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>

#include "Gap.h"
#include "MnistKernels.h"
#include "ImgIO.h"

#ifdef __EMUL__
#ifdef PERF
#undef PERF
#endif
#endif


#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

AT_HYPERFLASH_FS_EXT_ADDR_TYPE Mnist_L3_Flash = 0;

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
#ifdef MNIST_16BIT
  typedef short int MNIST_IMAGE_IN_T;
#else
  #ifdef MNIST_8BIT
  typedef signed char MNIST_IMAGE_IN_T;
  #endif
#endif
L2_MEM MNIST_IMAGE_IN_T *ImageIn;

#ifdef PERF
L2_MEM rt_perf_t *cluster_perf;
#endif

static void RunMnist()

{
  printf("Running on cluster\n");
#ifdef PERF
  gap_cl_starttimer();
  gap_cl_resethwtimer();
#endif
  MnistCNN(ImageIn, ResOut);
  printf("Runner completed\n");

#ifndef NO_IMAGE
  //Checki Results
  int rec_digit = 0;
  short int highest = ResOut[0];
  for(int i = 1; i < 10; i++) {
    if(ResOut[i] > highest) {
      highest = ResOut[i];
      rec_digit = i;
    }
  }
  printf("\n");

  printf("Recognized: %d\n", rec_digit);
#else
  printf("image loading disabled so no sensible result\n");
#endif
}

#if defined(__EMUL__) && !defined(LINK_IMAGE_HEADER)
int main(int argc, char *argv[]) 
{
  if (argc < 2) {
    printf("Usage: mnist [image_file]\n");
    exit(1);
  }
  char *ImageName = argv[1];
  if (dt_open_dump_file(TENSOR_DUMP_FILE)) {
    printf("Failed to open tensor dump file %s.\n", TENSOR_DUMP_FILE);
    exit(1);
  }
#else
int main()
{
#ifdef LINK_IMAGE_NAME
#define __STRING1(__s) #__s
#define __STRING(__s) __STRING1(__s)
  char *ImageName = __STRING(LINK_IMAGE_NAME);
#else
  char *ImageName = "../../../samples/5223_5.pgm";
#endif

#endif
  unsigned int Wi, Hi;

  //Input image size
  unsigned int W = 28, H = 28;

  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 8)) return -1;

  //To use file IO system
#ifndef __EMUL__
#if !defined(NO_IMAGE) && !defined(LINK_IMAGE_HEADER)
  printf("Connecting to bridge\n");
  rt_bridge_connect(1, NULL);
  printf("Connected to bridge\n");
#endif
#endif

  unsigned char * ImageInChar = (unsigned char *) rt_alloc(RT_ALLOC_L2_CL_DATA, W*H*sizeof(MNIST_IMAGE_IN_T));

  if (ImageInChar==0) {
    printf("Failed to allocate Memory for Image (%d bytes)\n", W*H*sizeof(MNIST_IMAGE_IN_T));
    return 1;
  }

#ifndef NO_IMAGE
  printf("Reading image\n");
  //Reading Image from Bridge
  if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageInChar, W*H*sizeof(unsigned char))==0) || (Wi!=W) || (Hi!=H)) {
    printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
    return 1;
  }
  printf("Finished reading image\n");
#endif
#ifdef PRINT_IMAGE
  for (int i=0; i<H; i++) {
    for (int j=0; j<W; j++) {
      printf("%03d, ", ImageInChar[W*i + j]);
    }
    printf("\n");
  }
#endif
  ResOut = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 10*sizeof(short int));

  if (ResOut==0) {
    printf("Failed to allocate Memory for Result (%d bytes)\n", 10*sizeof(short int));
    return 1;
  }

#ifdef MNIST_8BIT
  printf("Preprocess 8 bit\n");
  ImageIn = (MNIST_IMAGE_IN_T *)ImageInChar;
  for (int i=W*H-1; i>=0; i--) {
    ImageIn[i] = (MNIST_IMAGE_IN_T) (ImageInChar[i]>>1);
  }
#else
#ifdef MNIST_16BIT
  printf("Preprocess 16 bit\n");
  ImageIn = (MNIST_IMAGE_IN_T *)ImageInChar;
  for (int i=W*H-1; i>=0; i--) {
    ImageIn[i] = ImageInChar[i] << 7;
  }
#else
#error No bit size selected
#endif
#endif
  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  printf("Constructor\n");

  // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
  if (MnistCNN_Construct())
  {
    printf("Graph constructor exited with an error\n");
    return 1;
  }

  printf("Call cluster\n");
  // Execute the function "RunCifar10" on the cluster.
  rt_cluster_call(NULL, CID, (void *) RunMnist, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);

  MnistCNN_Destruct();
#ifdef PERF
	{
		unsigned int TotalCycles = 0, TotalOper = 0;
		printf("\n");
		for (int i=0; i<(sizeof(MnistPerf)/sizeof(unsigned int)); i++) {
			printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", MnistLName[i], MnistPerf[i], MnistOperCount[i], ((float) MnistOperCount[i])/ MnistPerf[i]);
			TotalCycles += MnistPerf[i]; TotalOper += MnistOperCount[i];
		}
		printf("\n");
		printf("%45s: %10d, Operation: %10d, Operation/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
		printf("\n");
	}
#endif
  // Close the cluster
  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

#ifdef __EMUL__
  dt_close_dump_file();
#else
#if !defined(NO_IMAGE) && !defined(LINK_IMAGE_HEADER)
  rt_bridge_disconnect(NULL);
#endif
#endif

  printf("Ended\n");
  return 0;
}
