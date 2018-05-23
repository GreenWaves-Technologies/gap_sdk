#ifndef __FIRKERNEL_H__
#define __FIRKERNEL_H__

#include "KernelLibStdTypes.h"
#include "FirKernelsInit.h"
#include "FirBasicKernels.h"
#define _FIR_L1_Memory_SIZE 51200
#define _FIR_L2_Memory_SIZE 0
extern char *FIR_L1_Memory; /* Size given for generation: 51200 bytes, used: 51200 bytes */
extern char *FIR_L2_Memory; /* Size used for generation: 0 bytes */
extern void TiledFir40GenericScalar(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir40Generic(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir10GenericScalar(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir10Generic(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir10Opt(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir20GenericScalar(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir20Generic(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
extern void TiledFir20Opt(
		Word16 *  __restrict__ In,
		Word16 *  __restrict__ Coeffs,
		Word16 *  __restrict__ Out,
		Wordu32 Norm,
		Kernel_T *Ker);
#endif
