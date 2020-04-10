#ifndef __MNISTKERNEL_H__
#define __MNISTKERNEL_H__

#include "AutoTilerLibTypes.h"
#include "CNN_BasicKernels.h"
#include "MnistKernels.h"
#define _L1_Memory_SIZE 43232
#define _Mnist_L2_Memory_SIZE 22568
extern char *L1_Memory; /* Size given for generation: 48000 bytes, used: 43232 bytes */
extern char *Mnist_L2_Memory; /* Size used for generation: 22568 bytes */
extern void Conv5x5ReLUMaxPool2x2_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out);
extern void Conv5x5ReLUMaxPool2x2_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out);
extern void LinearLayerReLU_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out);
extern void SoftMax_0(
		short int * __restrict__ In,
		short int * __restrict__ Out);
extern int MnistCNN_Construct();
extern int MnistCNN_Destruct();
extern int MnistCNN(
		short int *__restrict__ Input0,
		short int *__restrict__ Output0);
extern unsigned int AT_GraphOperInfosNames[4];
#endif
