#define _L1_Memory_SIZE_big 50000
extern void Dense(
		Word16 * __restrict__ In,
		Word16 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker);


#ifndef __CNNKERNEL_H__
#define __CNNKERNEL_H__

#include "KernelLibStdTypes.h"
//#include "CnnKernelsInit.h"
#include "CNN_BasicKernels.h"
extern char *L1_Memory; /* Size given for generation: 50000 bytes, used: 37488 bytes */

extern void ConvLayer(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker);
#endif

void Dense_halfchar(
		Word16 * __restrict__ In,
		Word8 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker);
