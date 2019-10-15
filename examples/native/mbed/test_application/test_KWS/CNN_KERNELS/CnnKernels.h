#ifndef __CNNKERNEL_H__
#define __CNNKERNEL_H__

#include "KernelLibStdTypes.h"
#include "CnnKernelsInit.h"
#include "CNN_BasicKernels.h"
#define _L1_Memory_SIZE 49968
#define _L2_Memory_SIZE 0
extern char *L1_Memory; /* Size given for generation: 50000 bytes, used: 49968 bytes */
extern char *L2_Memory; /* Size used for generation: 0 bytes */


extern void ConvLayer1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker);
extern void ConvLayer3(
		short int * __restrict__ In,
		Word8 * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker);
extern void ConvLayer2(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker);
extern void Dense2(
		Word16 * __restrict__ In,
		Word8 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker);
extern void Dense1(
		Word16 * __restrict__ In,
		Word16 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker);

extern void Conv6x10_HWCE_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		Kernel_T *Ker);

extern void Conv8x20MaxPool2x2_HWCE_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		Kernel_T *Ker);

#endif
