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
#include "Cifar10Kernels.h"
#include "CifarCoeff.h"
#include "CifarData.h"

#define STACK_SIZE      1024
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

L2_MEM short int *Out_Layer0;
L2_MEM short int *Out_Layer1;
L2_MEM short int *Out_Layer2;
L2_MEM rt_perf_t *cluster_perf;

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)

{
  int i, j;
  int Acc = 0;
  int K = 5;

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

unsigned int CheckSum(short int *In, int Size)

{
  int i;
  unsigned S=0;

  for (i=0; i<Size; i++) S += In[i];
  return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)

{
  int i;

  printf("Check sum for %s\n", Mess);

  for (i=0; i<NPlane; i++) {
    printf("\t%2d: %x\n", i, CheckSum(Planes + i*(W*H), W*H));
  }
}


static void RunCifar10()

{
  unsigned int ElapsedTime[3];
  unsigned int instr[3];
  int CheckResults = 1;
  rt_perf_t *perf = cluster_perf;
  // initialize the performance clock
  rt_perf_init(perf);
  // Configure performance counters for counting the cycles
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES)|(1<<RT_PERF_INSTR));

#if RT_HAS_HWCE
    /* Make HWCE event active */
    eu_evt_maskSet(1<<ARCHI_EVT_ACC0);

    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5MaxPool2x2_HWCE_0(ImageIn, Filter_Layer0_HWCE, Out_Layer0, 14, Bias_Layer0, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    instr[0] = rt_perf_get(perf, RT_PERF_INSTR);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);
    if (CheckResults) Check("HWCE Layer0", Out_Layer0, 8, 14, 14);

    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5MaxPool2x2_HWCE_1(Out_Layer0, Filter_Layer1_HWCE, Out_Layer1, 14, Bias_Layer1, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    instr[1] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0];
    ElapsedTime[1] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0];
    if (CheckResults) Check("HWCE Layer1", Out_Layer1, 12, 5, 5);
#else
    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5MaxPool2x2_SW_0(ImageIn, Filter_Layer0, Out_Layer0, 14, Bias_Layer0, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    instr[0] = rt_perf_get(perf, RT_PERF_INSTR);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);
    if (CheckResults) Check("SW   Layer0", Out_Layer0, 8, 14, 14);

    rt_perf_reset(perf);
    rt_perf_start(perf);
    Conv5x5MaxPool2x2_SW_1(Out_Layer0, Filter_Layer1, Out_Layer1, 14, Bias_Layer1, 0);
    rt_perf_stop(perf);
    rt_perf_save(perf);
    instr[1] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0];
    ElapsedTime[1] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0];
    if (CheckResults) Check("SW   Layer1", Out_Layer1, 12, 5, 5);
#endif
  rt_perf_reset(perf);
  rt_perf_start(perf);
  LinearLayerReLU_1(Out_Layer1, Filter_Layer2, 16, Bias_Layer2, 10, Out_Layer2, 0, 0);
  rt_perf_stop(perf);
  rt_perf_save(perf);
  instr[2] = rt_perf_get(perf, RT_PERF_INSTR) - instr[0] - instr[1];
  ElapsedTime[2] = rt_perf_get(perf, RT_PERF_CYCLES)-ElapsedTime[0]-ElapsedTime[1];
  if (CheckResults) Check("SW   Layer2", Out_Layer1, 10, 1, 1);

  printf("Layer0: %7d Cycles, %7d instruments\n", ElapsedTime[0], instr[0]);
  printf("Layer1: %7d Cycles, %7d instruments\n", ElapsedTime[1], instr[1]);
  printf("Layer2: %7d Cycles, %7d instruments\n", ElapsedTime[2], instr[2]);
}

int main()
{

  printf("FC Launched\n");

  Out_Layer0 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 14*14*sizeof(short int)*8);
  printf("Allocating %d: %s\n", 14*14*sizeof(short int)*8, Out_Layer0?"Ok":"Failed");
  Out_Layer1 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 5*5*sizeof(short int)*12);
  printf("Allocating %d: %s\n", 5*5*sizeof(short int)*12, Out_Layer1?"Ok":"Failed");
  Out_Layer2 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 10*sizeof(short int));
  printf("Allocating %d: %s\n", 10*sizeof(short int), Out_Layer2?"Ok":"Failed");
  if (!(Out_Layer0 && Out_Layer1 && Out_Layer2)) {
    printf("Failed to allocated memory, giving up.\n");
    return -1;
  }
#ifdef DMP_PAD_COEFF
  DumpPaddedCoeff("Filter_Layer0_HWCE", Filter_Layer0, 25, 1*8);
  DumpPaddedCoeff("Filter_Layer1_HWCE", Filter_Layer1, 25, 8*12);
#endif

  rt_cluster_mount(MOUNT, CID, 0, NULL);

  // Allocate the memory of L2 for the performance structure
  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) return -1;

  // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  Cifar10_L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _Cifar10_L1_Memory_SIZE);
  if(Cifar10_L1_Memory == NULL) {
    printf("Cifar10_L1_Memory alloc failed\n");
    return -1;
  }

  // Execute the function "RunCifar10" on the cluster.
  rt_cluster_call(NULL, CID, (void *) RunCifar10, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);

  // Close the cluster
  rt_cluster_mount(UNMOUNT, CID, 0, NULL);
  printf("Test success\n");
  return 0;

}

