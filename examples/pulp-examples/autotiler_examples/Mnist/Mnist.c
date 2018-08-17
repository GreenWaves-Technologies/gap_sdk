/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap8.h"
#include "MnistKernels.h"
#include "MnistCoeffs.def"
#include "ImgIO.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

L2_MEM signed short int *Out_Layer0;
L2_MEM signed short int *Out_Layer1;
L2_MEM signed short int *Out_Layer2;
L2_MEM rt_perf_t *cluster_perf;
RT_L2_DATA unsigned short *ImageIn;
RT_L2_DATA unsigned char *ImageIn_real;


int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)

{
  unsigned int i, j;
  int Acc = 0;
  unsigned int K = 5;

  for (i=0; i<K; i++) {
    for (j=0; j<K; j++) {
      Acc += In[(X+i)*W+Y+j]*Filter[K*i+j];
    }
  }
  return (gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
}


void DumpPlane(char *Mess, short int *Plane, unsigned int W, unsigned int H)

{
  unsigned int i, j;

  printf("----------------- %s ------------------------\n", Mess);
  for (i=0; i<H; i++) {
    printf("%2d: ", i);
    for (j=0; j<W; j++) {
      printf("%4x ", (unsigned short) Plane[i*W+j]);
    }
    printf("\n");
  }
  printf("-----------------------------------------\n");
}

void DumpPaddedCoeff(char *Name, short int *C, unsigned int NTap, unsigned int NFilter)

{
  unsigned int i, j;
  printf("L2_MEM short int %s[] = {\n", Name);
  for (i=0; i<NFilter; i++) {
    for (j=0; j<NTap; j++) {
      printf("%d, ", C[i*NTap+j]);
    }
    printf("0,\n");
  }
  printf("};\n");
}

int CheckSum(short int *In, int Size)

{
  int i;
  int S=0;

  for (i=0; i<Size; i++) S += In[i];
  return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)

{
  int i;
  printf("Check sum for %s\n", Mess);

  for (i=0; i<NPlane; i++) {
    printf("\t%2d: %d\n", i, CheckSum(Planes + i*(W*H), W*H));
  }
}


static void RunMnist()

{
  unsigned int ElapsedTime[3];
  int CheckResults = 0;
  rt_perf_t *perf = cluster_perf;
  // initialize the performance clock
  rt_perf_init(perf);
  // Configure performance counters for counting the cycles
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES));


#ifdef RT_HAS_HWCE
    /* Make HWCE event active */
    eu_evt_maskSet(1<<ARCHI_EVT_ACC0);
#endif

  gap8_resethwtimer();

#if RT_HAS_HWCE
    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5ReLUMaxPool2x2_HWCE_0((short int*) ImageIn, Filter_Layer0, Out_Layer0, 14, Bias_Layer0, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);
    if (CheckResults) Check("HWCE Layer0", Out_Layer0, 8, 14, 14);

    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5ReLUMaxPool2x2_HWCE_1(Out_Layer0, Filter_Layer1, Out_Layer1, 14, Bias_Layer1, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[1] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0];
    if (CheckResults) Check("HWCE Layer1", Out_Layer1, 12, 5, 5);

  #else
    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5ReLUMaxPool2x2_0((short int*)ImageIn, Filter_Layer0, Out_Layer0, 14, Bias_Layer0, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);
    if (CheckResults) Check("SW   Layer0", Out_Layer0, 8, 14, 14);

    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5ReLUMaxPool2x2_1(Out_Layer0, Filter_Layer1, Out_Layer1, 14, Bias_Layer1, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[1] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0];
    if (CheckResults) Check("SW   Layer1", Out_Layer1, 12, 5, 5);
  #endif

  rt_perf_reset(perf);
  rt_perf_start(perf);
  LinearLayerReLU_2(Out_Layer1, Filter_Layer2, 16, Bias_Layer2, 13, Out_Layer2, 10, 0);
  rt_perf_stop(perf);
  rt_perf_save(perf);
  ElapsedTime[2] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0]-ElapsedTime[1];
  if (CheckResults) Check("SW   Layer2", Out_Layer2, 10, 1, 1);

  
  //Chenking Results
  int rec_digit=0;
  int highest=Out_Layer2[0];
  for(int i=1;i<10;i++){
    if(highest<Out_Layer2[i]){
      highest=Out_Layer2[i];
      rec_digit=i;
    }
  }

  printf("Recognized: %d\n",rec_digit);

  printf("Layer0: %7d Cycles\n", ElapsedTime[0]);
  printf("Layer1: %7d Cycles\n", ElapsedTime[1]);
  printf("Layer2: %7d Cycles\n", ElapsedTime[2]);
}

int main()

{

  char *ImageName = "../../../136.pgm";
  unsigned int Wi, Hi;

  //Input image size
  unsigned int W = 28, H = 28;

  printf("Entering main controller\n");

  if (rt_event_alloc(NULL, 8)) return -1;

  //To use file IO system
  rt_bridge_connect(NULL);

  //Allocating input and output image buffers in L2 memory
  ImageIn_real  = (unsigned char *) rt_alloc( RT_ALLOC_L2_CL_DATA, W*H*sizeof(unsigned char));
  ImageIn       = (unsigned short *) rt_alloc( RT_ALLOC_L2_CL_DATA, W*H*sizeof(unsigned short ));
  
  if (ImageIn==0) {
    printf("Failed to allocate Memory for Image (%d bytes)\n", W*H*sizeof(unsigned short));
    return 1;
  }

  //Reading Image from Hyperflash
  if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageIn_real, W*H*sizeof(unsigned char))==0) || (Wi!=W) || (Hi!=H)) {
    printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, W, H, Wi, Hi);
    return 1;
  }


  //Convert in Mnist dataset format
  for(int i=0;i<W*H;i++) ImageIn[i] = ImageIn_real[i]*16;

  //TODO Move this to Cluster
  Out_Layer0 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 24*24*sizeof(short int)*32);
  printf("Allocating %d: %s\n", 24*24*sizeof(short int)*32, Out_Layer0?"Ok":"Failed");
  Out_Layer1 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 4*4*sizeof(short int)*64);
  printf("Allocating %d: %s\n", 4*4*sizeof(short int)*64, Out_Layer1?"Ok":"Failed");
  Out_Layer2 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 10*sizeof(short int));
  printf("Allocating %d: %s\n", 10*sizeof(short int), Out_Layer2?"Ok":"Failed");

  if (!(Out_Layer0 && Out_Layer1 && Out_Layer2)) {
    printf("Failed to allocated memory, giving up.\n");
    return 0;
  }
  
  
#ifdef DMP_PAD_COEFF
  DumpPaddedCoeff("Filter_Layer0_HWCE", Filter_Layer0, 25, 1*32);
  DumpPaddedCoeff("Filter_Layer1_HWCE", Filter_Layer1, 25, 32*64);
#endif

  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate the memory of L2 for the performance structure
  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) return -1;

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  Mnist_L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _Mnist_L1_Memory_SIZE);
  if(Mnist_L1_Memory == NULL) {
    printf("Mnist_L1_Memory alloc failed\n");
    return -1;
  }

  // Execute the function "RunCifar10" on the cluster.
  rt_cluster_call(NULL, CID, (void *) RunMnist, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);


  // Close the cluster
  rt_cluster_mount(UNMOUNT, CID, 0, NULL);

  rt_bridge_disconnect(NULL);

  //Add Buffer Free
  printf("Test success\n");
  return 0;
}

