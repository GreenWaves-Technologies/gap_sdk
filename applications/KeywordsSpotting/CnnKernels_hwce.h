#ifndef __CNNKERNELS_HWC_H__
#define __CNNKERNELS_HWC_H__

#include "KernelLibStdTypes.h"
#include "CnnKernelsInit_hwce.h"
#include "CNN_BasicKernels.h"
#define _L1_Memory_SIZE 24064
#define _L2_Memory_SIZE 0
extern char *L1_Memory; /* Size given for generation: 52000 bytes, used: 24064 bytes */
extern char *L2_Memory; /* Size used for generation: 0 bytes */
extern void Conv8x20MaxPool2x2_HWCE_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		Kernel_T *Ker);
extern void Conv6x10_HWCE_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		Kernel_T *Ker);
#endif
