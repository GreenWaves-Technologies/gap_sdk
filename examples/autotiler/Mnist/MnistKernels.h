#ifndef __MNISTKERNEL_H__
#define __MNISTKERNEL_H__

#include "AutoTilerLibTypes.h"
#include "CNN_BasicKernels.h"
#define _Mnist_L1_Memory_SIZE 40768
#define _Mnist_L2_Memory_SIZE 0
extern char *Mnist_L1_Memory; /* Size given for generation: 45000 bytes, used: 40768 bytes */
extern char *Mnist_L2_Memory; /* Size used for generation: 0 bytes */
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
extern void LinearLayerReLU_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out);
#endif
