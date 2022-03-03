/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include "Gap.h"
#include "CNN_BasicKernels_fp16.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

static int FirstDefinedOutput(int F, int Pad, int Stride)

{
	// k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

	return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(int DimIn, int F, int PadL, int Stride)

{
	// k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

	return ((DimIn - ((F-1)/2 - PadL + (F/2)) + Stride-1)/Stride);
}

/* Padded Convolution Border processing

	Zero padding support. Implementation is based on partial convolutions derived from the original filter

	Input feature maps, Output feature maps and filter on half words

		KerConv3x1BorderStrideNx1_fp16
			|------ KerConv2x1from3x1StrideNx1_V_fp16 3x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x3BorderStride1xN_fp16
			|------ KerConv1x2from1x3Stride1xN_H_fp16 1x3 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv3x3BorderStride1_fp16
			|------	KerConv2x3from3x3Stride1_V_fp16	3x3 convolution, stride 1, Left and right 0 padded stripes processing. 
			|------	KerConv3x2from3x3Stride1_H_fp16	3x3 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStride2_fp16
			|------	KerConv2x3from3x3Stride2_V_fp16	3x3 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride2_H_fp16	3x3 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStrideS_fp16
			|------	KerConv2x3from3x3StrideS_V_fp16	3x3 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3StrideS_H_fp16	3x3 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConv5x1BorderStrideNx1_fp16
			|------ KerConv4x1from5x1StrideNx1_V_fp16 5x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x5BorderStride1xN_fp16
			|------ KerConv1x4from1x5Stride1xN_H_fp16 1x5 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv5x5BorderStride1_fp16
			|------	KerConv4x5from5x5Stride1_V_fp16	5x5 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride1_H_fp16	5x5 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride2_fp16
			|------	KerConv4x5from5x5Stride2_V_fp16	5x5 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride2_H_fp16	5x5 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStrideS_fp16
			|------	KerConv4x5from5x5StrideS_V_fp16	5x5 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5StrideS_H_fp16	5x5 convolution, stride S, Top and bottom 0 padded stripes processing.

*/

static void __attribute__ ((noinline)) KerConv2x1from3x1StrideNx1_V_fp16(
	F16 * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16 V0,V1;
	F16 C0,C1;
	F16 *PtIn, *PtOut;

	if (FilterConf) { /* Right Side */
		C0 = Filter[0]; C1 = Filter[1];
	} else { /* Left Side */
		C0 = Filter[1]; C1 = Filter[2];
	}
	PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		F16 Acc = Bias;
		V0 = *PtIn; V1 = *(PtIn+1); PtIn += W;
		Acc += V0*C0; Acc += V1*C1;
		*PtOut = Acc; PtOut+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x2from1x3Stride1xN_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
        F16V C;
        F16 *PtIn = In+Wo_F*1-PadL, *PtOut = Out;
        int V0,V1;
        int C0,C1;

        if (FilterConf) { /* Bottom Side */
                C = *((F16V *)(Filter));
        } else { /* Top Side */
                C = *((F16V *)(Filter+1));
        }
        for (unsigned int i=Wo_F; i<Wo_L; i++) {
                F16V A = *((F16V *)(PtIn+W)) * C; PtIn++;
                *PtOut =  Bias+A[0]+A[1]; PtOut++;
        }
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride1_V_fp16(
	F16 * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2;
	F16V C0, C1, C2;
	F16 *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((F16V*) (Filter + 0*3+0)); C1 = *((F16V*) (Filter + 1*3+0)); C2 = *((F16V*) (Filter + 2*3+0));
	} else {
		C0 = *((F16V*) (Filter + 0*3+1)); C1 = *((F16V*) (Filter + 1*3+1)); C2 = *((F16V*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (F16V){0.0,0.0};
		V1 = *((F16V *) PtIn); PtIn += W;
	} else  { // == 0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((F16V *) PtIn); PtIn += W;
		V1 = *((F16V *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V2 = *((F16V *) PtIn); PtIn += W;
		F16V A = V0*C0 + V1*C1 + V2*C2;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		PtIn -= 2*W;
		V0 = *((F16V *) PtIn); PtIn += W;
		V1 = *((F16V *) PtIn);
		F16V A = V0*C0 + V1*C1;
		*PtOut =  Bias+A[0]+A[1];
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride2_V_fp16(
	F16 * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2;
	F16V C0, C1, C2;
	F16 *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((F16V*) (Filter + 0*3+0)); C1 = *((F16V*) (Filter + 1*3+0)); C2 = *((F16V*) (Filter + 2*3+0));
	} else {
		C0 = *((F16V*) (Filter + 0*3+1)); C1 = *((F16V*) (Filter + 1*3+1)); C2 = *((F16V*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (F16V){0.0,0.0};
	} else  { // == 0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((F16V *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V1 = *((F16V *) PtIn); PtIn += W; V2 = *((F16V *) PtIn); PtIn += W;
		F16V A = V0*C0 + V1*C1 + V2*C2;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = V2;
	}
	if (Bottom) {
		PtIn -= W;
		V0 = *((F16V *) PtIn); PtIn += W;
		V1 = *((F16V *) PtIn);;
		F16V A = V0*C0 + V1*C1;
		*PtOut =  Bias+A[0]+A[1];
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3StrideS_V_fp16(
	F16 * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2;
	F16V C0, C1, C2;
	F16 *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((F16V*) (Filter + 0*3+0)); C1 = *((F16V*) (Filter + 1*3+0)); C2 = *((F16V*) (Filter + 2*3+0));
	} else {
		C0 = *((F16V*) (Filter + 0*3+1)); C1 = *((F16V*) (Filter + 1*3+1)); C2 = *((F16V*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (F16V){0.0,0.0};
	} else  { // == 0
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((F16V *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V1 = *((F16V *) PtIn); PtIn += W; V2 = *((F16V *) PtIn); PtIn += (Stride-2)*W;
		F16V A = V0*C0 + V1*C1 + V2*C2;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = *((F16V *) PtIn); PtIn += W;
	}
	if (Bottom) {
		PtIn -= W;
		V0 = *((F16V *) PtIn); PtIn += W;
		V1 = *((F16V *) PtIn);
		F16V A = V0*C0 + V1*C1;
		*PtOut =  Bias+A[0]+A[1];
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride1_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	F16V X, Y, V0, V1, V2;
	F16V C0, C1, C2;
	F16 *PtIn = In+Wo_F*1-PadL;
	F16 *PtOut = Out;

	if (FilterConf) {
		X = *((F16V *) &Filter[0*3+0]); Y = *((F16V *) &Filter[1*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap_pack2f16(Filter[2], Filter[5]);
	} else {
		X = *((F16V *) &Filter[1*3+0]); Y = *((F16V *) &Filter[2*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap_pack2f16(Filter[5], Filter[8]);
	}
	X = *((F16V *) (PtIn+0*W+0)); Y = *((F16V *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	PtIn += 2;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		V2 = gap_pack2f16(PtIn[0*W], PtIn[1*W]); PtIn++;
		F16V A = V0*C0 + V1*C1 + V2*C2;
		*PtOut =  Bias+A[0]+A[1]; PtOut++;
		V0=V1; V1=V2;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride2_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	F16V X, Y, V0, V1, V2;
	F16V C0, C1, C2;
	F16 *PtIn = In+Wo_F*2-PadL;
	F16 *PtOut = Out;

	if (FilterConf) {
		X = *((F16V *) &Filter[0*3+0]); Y = *((F16V *) &Filter[1*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap_pack2f16(Filter[2], Filter[5]);
	} else {
		X = *((F16V *) &Filter[1*3+0]); Y = *((F16V *) &Filter[2*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap_pack2f16(Filter[5], Filter[8]);
	}
	X = *((F16V *) (PtIn+0*W+0)); Y = *((F16V *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	V0 = gap_pack2f16(PtIn[0], PtIn[W]); PtIn++;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		X = *((F16V *) (PtIn+0*W)); Y = *((F16V *) (PtIn+1*W)); V1 = __builtin_shuffle(X,Y,(v2s){0,2}); V2 = __builtin_shuffle(X,Y,(v2s){1,3}); PtIn+=2;
		F16V A = V0*C0 + V1*C1 + V2*C2;
		*PtOut =  Bias+A[0]+A[1]; PtOut++;
		V0=V2;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3StrideS_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Stride,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	int Fw = 3;
	F16V Cv0, Cv1;
	F16 C0, C1;
	F16 *PtOut = Out;
	In += Wo_F*Stride-PadL;

	if (FilterConf) {
		Cv0 =  *((F16V *) &Filter[0*3+0]); C0 = Filter[0*3+2];
		Cv1 =  *((F16V *) &Filter[1*3+0]); C1 = Filter[1*3+2];
	} else {
		Cv0 =  *((F16V *) &Filter[1*3+0]); C0 = Filter[1*3+2];
		Cv1 =  *((F16V *) &Filter[2*3+0]); C1 = Filter[2*3+2];
	}
	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = In;
		F16V Iv0, A;
		F16 I;
		F16 Acc = Bias;
		Iv0 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		A = Iv0*Cv0; Acc += I*C0;
		Iv0 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		A += Iv0*Cv1; Acc += I*C1;
		*PtOut = A[0]+A[1]+Acc; PtOut++;
		In += Stride;
	}
}


static void __attribute__ ((noinline)) KerConv4x1from5x1StrideNx1_V_fp16(
	F16 * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1;
	F16V C0, C1;
	F16 *PtIn, *PtOut;

	switch (FilterConf) {
		case 2: // [0..4 x 0] => [2..4 x 0] PadL==2
			C0 = *((F16V*) (Filter + 0*5+2)); C1 = gap_pack2f16(Filter[0*5+4], 0);
			break;
		case 1: // [0..4 x 0] => [1..4 x 0] PadL==1
			C0 = *((F16V*) (Filter + 0*5+1)); C1 = *((F16V*) (Filter + 0*5+3));
			break;
		case 3: // [0..4 x 0] => [0..3 x 0] PadR==1
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = *((F16V*) (Filter + 0*5+2));
			break;
		case 4: // [0..4 x 0] => [0..2 x 0] PadR==2
			C0 = gap_pack2f16(0, Filter[0*5+0]); C1 = *((F16V*) (Filter + 0*5+1));
			break;
		case 5: // [0..4 x 0] => [0..2 x 0] PadR==2, Wo==1
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = gap_pack2f16(Filter[0*5+2], 0);
			break;
	}
	PtIn = In + (Ho_F*1-PadOrg[2])*W; PtOut = Out+Ho_F*Wo;
	V0 = * (F16V *) PtIn; V1 = *((F16V *) PtIn + 1); PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		F16V A = V0*C0 + V1*C1;
		V0 = * (F16V *) PtIn; V1 = *((F16V *) PtIn + 1); PtIn += W;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x4from1x5Stride1xN_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	F16V V0, V1;
	F16V C0, C1;
	F16 x0,x1,x2,x3;
	F16 *PtIn = In+Wo_F*1-PadL;
	F16 *PtOut = Out;

	switch (FilterConf) {
		case 2: // [0 x 0..4] => [0 x 2..4] PadT == 2
			C0 = *((F16V*) (Filter + 0*5+2)); C1 = gap_pack2f16(Filter[0*5+4], 0);
			break;
		case 1: // [0 x 0..4] => [0 x 1..4] PadT == 1
			C0 = *((F16V*) (Filter + 0*5+1)); C1 = *((F16V*) (Filter + 0*5+3));
			break;
		case 3: // [0 x 0..4] => [0 x 1..4] PadB == 1
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = *((F16V*) (Filter + 0*5+2));
			break;
		case 4: // [0 x 0..4] => [0 x 2..4] PadB == 2
			C0 = gap_pack2f16(0, Filter[0*5+0]); C1 = *((F16V*) (Filter + 0*5+1));
			break;
		case 5: // [0 x 0..4] => [0 x 2..4] PadB == 2, Ho == 1
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = gap_pack2f16(Filter[0*5+2], 0);
			break;
	}
	x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack2f16(x0,x1); V1 = gap_pack2f16(x2,x3); PtIn++;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		F16V A = V0*C0 + V1*C1;
		x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack2f16(x0,x1); V1 = gap_pack2f16(x2,x3); PtIn++;
		*PtOut =  Bias+A[0]+A[1]; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride1_V_fp16(
	F16 * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	F16V C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	F16 *PtIn, *PtOut;
	int Bottom, PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL == 2
			C0 = *((F16V*) (Filter + 0*5+2)); C1 = gap_pack2f16(Filter[0*5+4], 0);
			C2 = *((F16V*) (Filter + 1*5+2)); C3 = gap_pack2f16(Filter[1*5+4], 0);
			C4 = *((F16V*) (Filter + 2*5+2)); C5 = gap_pack2f16(Filter[2*5+4], 0);
			C6 = *((F16V*) (Filter + 3*5+2)); C7 = gap_pack2f16(Filter[3*5+4], 0);
			C8 = *((F16V*) (Filter + 4*5+2)); C9 = gap_pack2f16(Filter[4*5+4], 0);
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL == 1
			C0 = *((F16V*) (Filter + 0*5+1)); C1 = *((F16V*) (Filter + 0*5+3));
			C2 = *((F16V*) (Filter + 1*5+1)); C3 = *((F16V*) (Filter + 1*5+3));
			C4 = *((F16V*) (Filter + 2*5+1)); C5 = *((F16V*) (Filter + 2*5+3));
			C6 = *((F16V*) (Filter + 3*5+1)); C7 = *((F16V*) (Filter + 3*5+3));
			C8 = *((F16V*) (Filter + 4*5+1)); C9 = *((F16V*) (Filter + 4*5+3));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR == 1
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = *((F16V*) (Filter + 0*5+2));
			C2 = *((F16V*) (Filter + 1*5+0)); C3 = *((F16V*) (Filter + 1*5+2));
			C4 = *((F16V*) (Filter + 2*5+0)); C5 = *((F16V*) (Filter + 2*5+2));
			C6 = *((F16V*) (Filter + 3*5+0)); C7 = *((F16V*) (Filter + 3*5+2));
			C8 = *((F16V*) (Filter + 4*5+0)); C9 = *((F16V*) (Filter + 4*5+2));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR == 2
			C0 = gap_pack2f16(0, Filter[0*5+0]); C1 = *((F16V*) (Filter + 0*5+1));
			C2 = gap_pack2f16(0, Filter[1*5+0]); C3 = *((F16V*) (Filter + 1*5+1));
			C4 = gap_pack2f16(0, Filter[2*5+0]); C5 = *((F16V*) (Filter + 2*5+1));
			C6 = gap_pack2f16(0, Filter[3*5+0]); C7 = *((F16V*) (Filter + 3*5+1));
			C8 = gap_pack2f16(0, Filter[4*5+0]); C9 = *((F16V*) (Filter + 4*5+1));
			break;
		case 5: // [0..4 x 0..4] => [0..2,0 x 0..4] PadR == 2
			C0 = *((F16V*) (Filter + 0*5+0)); C1 = gap_pack2f16(Filter[0*5+2], 0);
			C2 = *((F16V*) (Filter + 1*5+0)); C3 = gap_pack2f16(Filter[1*5+2], 0);
			C4 = *((F16V*) (Filter + 2*5+0)); C5 = gap_pack2f16(Filter[2*5+2], 0);
			C6 = *((F16V*) (Filter + 3*5+0)); C7 = gap_pack2f16(Filter[3*5+2], 0);
			C8 = *((F16V*) (Filter + 4*5+0)); C9 = gap_pack2f16(Filter[4*5+2], 0);
			break;
	}
	if (PadT==2) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = (F16V){0,0}; V3 = (F16V){0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	} else {
		PtIn = In + (Ho_F*1-PadTOrg)*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	}
	PtOut = Out+Ho_F*Wo;
	V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
	if (Ho==1) {
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5;
		*PtOut =  Bias+A[0]+A[1];
		return;
	}
	V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn); PtIn += (W-2);
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V8 = *((F16V *) PtIn); PtIn += 2; V9 = *((F16V *) PtIn); PtIn += (W-2);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7+V8*C8+V9*C9;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
	}
	if (PadB) {
		PtIn -= 4*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
		V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
		V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn); PtIn += (W-2);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7;
		*PtOut =  Bias+A[0]+A[1]; PtOut+=Wo;
		if (PadB==2) {
			V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7;
			F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5;
			*PtOut =  Bias+A[0]+A[1];
		}
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride2_V_fp16(
	F16 * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	F16V C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	F16 *PtIn, *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL==2
			C0 = *((F16V *) (Filter + 0*5+2)); C1 = gap_pack2f16(Filter[0*5+4], 0);
			C2 = *((F16V *) (Filter + 1*5+2)); C3 = gap_pack2f16(Filter[1*5+4], 0);
			C4 = *((F16V *) (Filter + 2*5+2)); C5 = gap_pack2f16(Filter[2*5+4], 0);
			C6 = *((F16V *) (Filter + 3*5+2)); C7 = gap_pack2f16(Filter[3*5+4], 0);
			C8 = *((F16V *) (Filter + 4*5+2)); C9 = gap_pack2f16(Filter[4*5+4], 0);
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((F16V *) (Filter + 0*5+1)); C1 = *((F16V *) (Filter + 0*5+3));
			C2 = *((F16V *) (Filter + 1*5+1)); C3 = *((F16V *) (Filter + 1*5+3));
			C4 = *((F16V *) (Filter + 2*5+1)); C5 = *((F16V *) (Filter + 2*5+3));
			C6 = *((F16V *) (Filter + 3*5+1)); C7 = *((F16V *) (Filter + 3*5+3));
			C8 = *((F16V *) (Filter + 4*5+1)); C9 = *((F16V *) (Filter + 4*5+3));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((F16V *) (Filter + 0*5+0)); C1 = *((F16V *) (Filter + 0*5+2));
			C2 = *((F16V *) (Filter + 1*5+0)); C3 = *((F16V *) (Filter + 1*5+2));
			C4 = *((F16V *) (Filter + 2*5+0)); C5 = *((F16V *) (Filter + 2*5+2));
			C6 = *((F16V *) (Filter + 3*5+0)); C7 = *((F16V *) (Filter + 3*5+2));
			C8 = *((F16V *) (Filter + 4*5+0)); C9 = *((F16V *) (Filter + 4*5+2));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((F16V *) (Filter + 0*5+0)); C1 = *((F16V *) (Filter + 0*5+2)); C1[1] = 0;
			C2 = *((F16V *) (Filter + 1*5+0)); C3 = *((F16V *) (Filter + 1*5+2)); C3[1] = 0;
			C4 = *((F16V *) (Filter + 2*5+0)); C5 = *((F16V *) (Filter + 2*5+2)); C5[1] = 0;
			C6 = *((F16V *) (Filter + 3*5+0)); C7 = *((F16V *) (Filter + 3*5+2)); C7[1] = 0;
			C8 = *((F16V *) (Filter + 4*5+0)); C9 = *((F16V *) (Filter + 4*5+2)); C9[1] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = (F16V){0,0}; V3 = (F16V){0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	} else {
		PtIn = In + (Ho_F*2-PadTOrg)*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	}
	PtOut = Out+Ho_F*Wo;
	V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn); PtIn += (W-2);
		V8 = *((F16V *) PtIn); PtIn += 2; V9 = *((F16V *) PtIn); PtIn += (W-2);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7+V8*C8+V9*C9;
		*PtOut = Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = V4; V1 = V5; V2 = V6; V3 = V7; V4 = V8; V5 = V9;
	}
	if (PadB) {
		int PadTOrg = PadOrg[2];
		PtIn -= 3*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
		V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5;
		if (PadB==1) {
			V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn);
			A += V6*C6+V7*C7;
		}
		*PtOut = Bias+A[0]+A[1];
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5StrideS_V_fp16(
	F16 * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)
{
	F16V V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	F16V C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	F16 *PtIn, *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL==2
			C0 = *((F16V *) (Filter + 0*5+2)); C1 = gap_pack2f16(Filter[0*5+4], 0);
			C2 = *((F16V *) (Filter + 1*5+2)); C3 = gap_pack2f16(Filter[1*5+4], 0);
			C4 = *((F16V *) (Filter + 2*5+2)); C5 = gap_pack2f16(Filter[2*5+4], 0);
			C6 = *((F16V *) (Filter + 3*5+2)); C7 = gap_pack2f16(Filter[3*5+4], 0);
			C8 = *((F16V *) (Filter + 4*5+2)); C9 = gap_pack2f16(Filter[4*5+4], 0);
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((F16V *) (Filter + 0*5+1)); C1 = *((F16V *) (Filter + 0*5+3));
			C2 = *((F16V *) (Filter + 1*5+1)); C3 = *((F16V *) (Filter + 1*5+3));
			C4 = *((F16V *) (Filter + 2*5+1)); C5 = *((F16V *) (Filter + 2*5+3));
			C6 = *((F16V *) (Filter + 3*5+1)); C7 = *((F16V *) (Filter + 3*5+3));
			C8 = *((F16V *) (Filter + 4*5+1)); C9 = *((F16V *) (Filter + 4*5+3));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((F16V *) (Filter + 0*5+0)); C1 = *((F16V *) (Filter + 0*5+2));
			C2 = *((F16V *) (Filter + 1*5+0)); C3 = *((F16V *) (Filter + 1*5+2));
			C4 = *((F16V *) (Filter + 2*5+0)); C5 = *((F16V *) (Filter + 2*5+2));
			C6 = *((F16V *) (Filter + 3*5+0)); C7 = *((F16V *) (Filter + 3*5+2));
			C8 = *((F16V *) (Filter + 4*5+0)); C9 = *((F16V *) (Filter + 4*5+2));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((F16V *) (Filter + 0*5+0)); C1 = gap_pack2f16(Filter[0*5+2], 0);
			C2 = *((F16V *) (Filter + 1*5+0)); C3 = gap_pack2f16(Filter[1*5+2], 0);
			C4 = *((F16V *) (Filter + 2*5+0)); C5 = gap_pack2f16(Filter[2*5+2], 0);
			C6 = *((F16V *) (Filter + 3*5+0)); C7 = gap_pack2f16(Filter[3*5+2], 0);
			C8 = *((F16V *) (Filter + 4*5+0)); C9 = gap_pack2f16(Filter[4*5+2], 0);
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = (F16V){0,0}; V3 = (F16V){0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (F16V){0,0}; V1 = (F16V){0,0};
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	} else {
		PtIn = In + (Ho_F*2-PadTOrg)*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	}
	PtOut = Out+Ho_F*Wo;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
		V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn); PtIn += (W-2);
		V8 = *((F16V *) PtIn); PtIn += 2; V9 = *((F16V *) PtIn); PtIn += ((Stride-4))*W-2;
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7+V8*C8+V9*C9;
		*PtOut = Bias+A[0]+A[1]; PtOut+=Wo;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
	}
	if (PadB) {
		PtIn -= 2*W;
		V0 = *((F16V *) PtIn); PtIn += 2; V1 = *((F16V *) PtIn); PtIn += (W-2);
		V2 = *((F16V *) PtIn); PtIn += 2; V3 = *((F16V *) PtIn); PtIn += (W-2);
		V4 = *((F16V *) PtIn); PtIn += 2; V5 = *((F16V *) PtIn); PtIn += (W-2);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5;
		if (PadB==1) {
			V6 = *((F16V *) PtIn); PtIn += 2; V7 = *((F16V *) PtIn);
			A += V6*C6+V7*C7;
		}
		*PtOut = Bias+A[0]+A[1];
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride1_H_fp16(
	F16 * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	F16V X, Y, V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	F16V C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	F16 *PtIn = In+Wo_F*1-PadL;
	F16 *PtOut = Out;

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [0..4 x 2..4] PadT==2
			X = *((F16V *) &Filter[2*5+0]); Y = *((F16V *) &Filter[3*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[2*5+2]); Y = *((F16V *) &Filter[3*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[14], Filter[19]);
			C5 = gap_pack2f16(Filter[20],0); C6 = gap_pack2f16(Filter[21],0); C7 = gap_pack2f16(Filter[22],0); C8 = gap_pack2f16(Filter[23],0); C9 = gap_pack2f16(Filter[24],0);
			break;
		case 1: // [0..4 x 0..4] => [0..4 x 1..4] PadT==1
			X = *((F16V *) &Filter[1*5+0]); Y = *((F16V *) &Filter[2*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[1*5+2]); Y = *((F16V *) &Filter[2*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[9], Filter[14]);
			X = *((F16V *) &Filter[3*5+0]); Y = *((F16V *) &Filter[4*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[3*5+2]); Y = *((F16V *) &Filter[4*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap_pack2f16(Filter[19], Filter[24]);
			break;
		case 3: // [0..4 x 0..4] => [0..4 x 0..3] PadB==1
			X = *((F16V *) &Filter[0*5+0]); Y = *((F16V *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[0*5+2]); Y = *((F16V *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[4], Filter[9]);
			X = *((F16V *) &Filter[2*5+0]); Y = *((F16V *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[2*5+2]); Y = *((F16V *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap_pack2f16(Filter[14], Filter[19]);
			break;
		case 4: // [0..4 x 0..4] => [0..4 x 0..2] PadB==2
			C0 = gap_pack2f16(0, Filter[0]); C1 = gap_pack2f16(0, Filter[1]); C2 = gap_pack2f16(0, Filter[2]); C3 = gap_pack2f16(0, Filter[3]); C4 = gap_pack2f16(0, Filter[4]);
			// X = *((F16V *) &Filter[2*5+0]); Y = *((F16V *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			// X = *((F16V *) &Filter[2*5+2]); Y = *((F16V *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[1*5+0]); Y = *((F16V *) &Filter[2*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[1*5+2]); Y = *((F16V *) &Filter[2*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap_pack2f16(Filter[9], Filter[14]);
			break;
		case 5: // [0..4 x 0..4] => [0..4 x 0..2,0] PadB==2
			X = *((F16V *) &Filter[0*5+0]); Y = *((F16V *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[0*5+2]); Y = *((F16V *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[4], Filter[9]);
			C5 = gap_pack2f16(Filter[10],0); C6 = gap_pack2f16(Filter[11],0); C7 = gap_pack2f16(Filter[12],0); C8 = gap_pack2f16(Filter[13],0); C9 = gap_pack2f16(Filter[14],0);
			break;
	}
	X = *((F16V *) (PtIn+0*W+0)); Y = *((F16V *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((F16V *) (PtIn+0*W+2)); Y = *((F16V *) (PtIn+1*W+2)); V2 = __builtin_shuffle(X,Y,(v2s){0,2}); V3 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((F16V *) (PtIn+2*W+0)); Y = *((F16V *) (PtIn+3*W+0)); V5 = __builtin_shuffle(X,Y,(v2s){0,2}); V6 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((F16V *) (PtIn+2*W+2)); Y = *((F16V *) (PtIn+3*W+2)); V7 = __builtin_shuffle(X,Y,(v2s){0,2}); V8 = __builtin_shuffle(X,Y,(v2s){1,3});
	PtIn += 4;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		F16 x0, x1, x2, x3;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn++;
		V4 = gap_pack2f16(x0,x1); V9 = gap_pack2f16(x2,x3);
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7+V8*C8+V9*C9;
		*PtOut = Bias+A[0]+A[1]; PtOut++;
		V0=V1; V1=V2; V2=V3; V3=V4; V5=V6; V6=V7; V7=V8; V8=V9;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride2_H_fp16(
	F16 * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	F16V X, Y, V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	F16V C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	F16 *PtIn = In+Wo_F*2-PadL;
	F16 *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT == 2
			X = *((F16V *) &Filter[2*5+0]); Y = *((F16V *) &Filter[3*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[2*5+2]); Y = *((F16V *) &Filter[3*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[14], Filter[19]);
			C5 = gap_pack2f16(Filter[20],0); C6 = gap_pack2f16(Filter[21],0); C7 = gap_pack2f16(Filter[22],0); C8 = gap_pack2f16(Filter[23],0); C9 = gap_pack2f16(Filter[24],0);
			break;
		case 1: // PadT == 1
			X = *((F16V *) &Filter[1*5+0]); Y = *((F16V *) &Filter[2*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[1*5+2]); Y = *((F16V *) &Filter[2*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[9], Filter[14]);
			X = *((F16V *) &Filter[3*5+0]); Y = *((F16V *) &Filter[4*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[3*5+2]); Y = *((F16V *) &Filter[4*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap_pack2f16(Filter[19], Filter[24]);
			break;
		case 3: // PadB == 1
			X = *((F16V *) &Filter[0*5+0]); Y = *((F16V *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[0*5+2]); Y = *((F16V *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[4], Filter[9]);
			X = *((F16V *) &Filter[2*5+0]); Y = *((F16V *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[2*5+2]); Y = *((F16V *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap_pack2f16(Filter[14], Filter[19]);
			break;
		case 4: // PadB == 2
			X = *((F16V *) &Filter[0*5+0]); Y = *((F16V *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((F16V *) &Filter[0*5+2]); Y = *((F16V *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap_pack2f16(Filter[4], Filter[9]);
			C5 = gap_pack2f16(Filter[2*5+0], 0); C6 = gap_pack2f16(Filter[2*5+1], 0); C7 = gap_pack2f16(Filter[2*5+2], 0); C8 = gap_pack2f16(Filter[2*5+3], 0); C9 = gap_pack2f16(Filter[2*5+4], 0);
			break;
	}

	X = *((F16V *) (PtIn+0*W+0)); Y = *((F16V *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((F16V *) (PtIn+0*W+2)); Y = *((F16V *) (PtIn+1*W+2)); V2 = __builtin_shuffle(X,Y,(v2s){0,2});
	X = *((F16V *) (PtIn+2*W+0)); Y = *((F16V *) (PtIn+3*W+0)); V5 = __builtin_shuffle(X,Y,(v2s){0,2}); V6 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((F16V *) (PtIn+2*W+2)); Y = *((F16V *) (PtIn+3*W+2)); V7 = __builtin_shuffle(X,Y,(v2s){0,2});
	PtIn += 3;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		X = *((F16V *) (PtIn+0*W+0)); Y = *((F16V *) (PtIn+1*W+0)); V3 = __builtin_shuffle(X,Y,(v2s){0,2}); V4 = __builtin_shuffle(X,Y,(v2s){1,3});
		X = *((F16V *) (PtIn+2*W+0)); Y = *((F16V *) (PtIn+3*W+0)); V8 = __builtin_shuffle(X,Y,(v2s){0,2}); V9 = __builtin_shuffle(X,Y,(v2s){1,3}); PtIn+=2;
		F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5+V6*C6+V7*C7+V8*C8+V9*C9;
		*PtOut = Bias+A[0]+A[1]; PtOut++;
		V0=V2; V1=V3; V2=V4; V5=V7; V6=V8; V7=V9;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5StrideS_H_fp16(
	F16 * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	int Stride,
	F16 Bias,
	F16 * __restrict__ Out,
	F16 * __restrict__ Filter,
	int FilterConf
	)

{
	int Fw=5;
	F16V C0, C1, C3, C4, C6, C7, C9, C10;
	F16 C2, C5, C8, C11;
	F16 *PtOut = Out;

	In += Wo_F*Stride-PadL;
	switch (FilterConf) {
		case 2: // PadT == 2
			C0 = *((F16V *) &Filter[2*5+0]); C1 = *((F16V *) &Filter[2*5+2]); C2 = Filter[2*5+4];
			C3 = *((F16V *) &Filter[3*5+0]); C4 = *((F16V *) &Filter[3*5+2]); C5 = Filter[3*5+4];
			C6 = *((F16V *) &Filter[4*5+0]); C7 = *((F16V *) &Filter[4*5+2]); C8 = Filter[4*5+4];
			C9 = (F16V) {0,0}; C10 = (F16V) {0,0}; C11 = 0;
			break;
		case 1: // PadT == 1
			C0 = *((F16V *) &Filter[1*5+0]);  C1 = *((F16V *) &Filter[1*5+2]);  C2 = Filter[1*5+4];
			C3 = *((F16V *) &Filter[2*5+0]);  C4 = *((F16V *) &Filter[2*5+2]);  C5 = Filter[2*5+4];
			C6 = *((F16V *) &Filter[3*5+0]);  C7 = *((F16V *) &Filter[3*5+2]);  C8 = Filter[3*5+4];
			C9 = *((F16V *) &Filter[4*5+0]); C10 = *((F16V *) &Filter[4*5+2]); C11 = Filter[4*5+4];
			break;
		case 3: // PadB == 1
			C0 = *((F16V *) &Filter[0*5+0]);  C1 = *((F16V *) &Filter[0*5+2]);  C2 = Filter[0*5+4];
			C3 = *((F16V *) &Filter[1*5+0]);  C4 = *((F16V *) &Filter[1*5+2]);  C5 = Filter[1*5+4];
			C6 = *((F16V *) &Filter[2*5+0]);  C7 = *((F16V *) &Filter[2*5+2]);  C8 = Filter[2*5+4];
			C9 = *((F16V *) &Filter[3*5+0]); C10 = *((F16V *) &Filter[3*5+2]); C11 = Filter[3*5+4];
			break;
		case 4: // PadB == 2
			C0 = *((F16V *) &Filter[0*5+0]);  C1 = *((F16V *) &Filter[0*5+2]);  C2 = Filter[0*5+4];
			C3 = *((F16V *) &Filter[1*5+0]);  C4 = *((F16V *) &Filter[1*5+2]);  C5 = Filter[1*5+4];
			C6 = *((F16V *) &Filter[2*5+0]);  C7 = *((F16V *) &Filter[2*5+2]);  C8 = Filter[2*5+4];
			C9 = (F16V) {0,0}; C10 = (F16V) {0,0}; C11 = 0;
			break;
	}
	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = In;
		F16V Iv0, Iv1;
		F16 I;
		F16 Acc = Bias;
		Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		F16V A = Iv0*C0 + Iv1*C1;
		Acc += I*C2;

		Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		A += Iv0*C3 + Iv1*C4; Acc += I*C5;

		Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		A += Iv0*C6 + Iv1*C7; Acc += I*C8;

		Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		A += Iv0*C9 + Iv1*C10; Acc += I*C11;

		*PtOut = A[0]+A[1]+Acc; PtOut++; In += Stride;
	}
}

static void __attribute__ ((noinline)) KerConvNxNStrideS_Border_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias
	)
{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;      // iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;      // iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
		for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			ht -= Stride; hb -= Stride;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
		for (unsigned int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			hb -= Stride;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw2;
		for (unsigned int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wl -= Stride; wr -= Stride;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
		for (unsigned int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wr -= Stride;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= Stride; wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= Stride; wr -= Stride;
				}
				hb -= Stride;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConvNxMStrideSxSy_Border_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	v4s PadOrg,
	F16 Bias
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;     // iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;     // iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
		for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			ht -= StrideY; hb -= StrideY;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
		for (unsigned int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			hb -= StrideY;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw2;
		for (unsigned int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wl -= StrideX; wr -= StrideX;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
		for (unsigned int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wr -= StrideX;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= StrideX; wr -= StrideX;
				}
				ht -= StrideY; hb -= StrideY;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= StrideX;
				}
				ht -= StrideY; hb -= StrideY;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= StrideX; wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++)
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConvNxMDxDyStrideSxSy_Border_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int Dw,
	int Dh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	v4s PadOrg,
	F16 Bias
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int TFw = Dw*(Fw-1)+1, TFh = Dh*(Fh-1)+1;
	int TFw2 = (TFw-1)/2, TFh2 = (TFh-1)/2;
	int Hi_F = TFh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;     // iff Hi_L>Hi_F
	int Wi_F = TFw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;     // iff Wi_L>Wi_F
	#ifndef Prec
	int Prec=10;
	#endif
	int InvDh = ((1<<Prec)+Dh-1)/Dh;
	int InvDw = ((1<<Prec)+Dw-1)/Dw;

	/*
	Here we assume that for a given filter output we don't have padding on both side of the input.
	Thanks to this assumption we can simplify a bit where filter starts and stops in the input.
	Either starts at 0 if (right or bottom) and stops at a place function of the padding or
	stops at Fw/Fh if (left or bottom) and starts  a place function of the padding
	*/
	if (PadT) { /* Top */
		int ht = PadTOrg;
		for (unsigned int h=0; h<Ho_F; h++) {
			int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
			int Fh_min = hta;
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			ht -= StrideY;
		}
	}
	if (PadB) { /* Bottom */
		int hb = H - (Hi_L+StrideY) + TFh2;
		for (unsigned int h=Ho_L; h<Ho; h++) {
			int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
			int Fh_max = AT_CLIP_POS(hba, Fh);
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh_max; i++)
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			hb -= StrideY;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg;
		for (unsigned int w=0; w<Wo_F; w++) {
			int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
			int Wl_min = wla;
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wl -= StrideX;
		}
	}
	if (PadR) { /* Right */
		int wr = W - (Wi_L+StrideX) + TFw2;
		for (unsigned int w=Wo_L; w<Wo; w++) {
			int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
			int Wr_max = AT_CLIP_POS(wra, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = Bias;
				for (unsigned int i=0; i<Fh; i++)
					for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Out[Wo*h+w] = Acc;
			}
			wr -= StrideX;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg;
				int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
					int Wl_min = wla, Fh_min = hta;
					for (unsigned int i=Fh_min; i<Fh; i++)
						for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= StrideX;
				}
				ht -= StrideY;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wr = W - (Wi_L+StrideX) + TFw2;
				int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = AT_CLIP_POS(wra, Fw), Fh_min = hta;
					for (unsigned int i=Fh_min; i<Fh; i++)
						for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= StrideX;
				}
				ht -= StrideY;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int hb = H - (Hi_L+StrideY) + TFh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg;
				int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = Bias;
					int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
					int Wl_min = wla, Fh_max = AT_CLIP_POS(hba, Fh);
					for (unsigned int i=0; i<Fh_max; i++)
						for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wl -= StrideX;
				}
				hb -= StrideY;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int hb = H - (Hi_L+StrideY) + TFh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wr = W - (Wi_L+StrideX) + TFw2;
				int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
				for (unsigned int w=Wo_L; w<Wo; w++) {
					F16 Acc = Bias;
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = AT_CLIP_POS(wra, Fw), Fh_max = AT_CLIP_POS(hba, Fh);
					for (unsigned int i=0; i<Fh_max; i++)
						for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Out[Wo*h+w] = Acc;
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

/* Wrapper functions */

static void __attribute__ ((noinline)) KerConv3x1BorderStrideNx1_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x1from3x1StrideNx1_V_fp16(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0);
	if (PadR) KerConv2x1from3x1StrideNx1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv1x3BorderStride1xN_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) KerConv1x2from1x3Stride1xN_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0);
	if (PadB) KerConv1x2from1x3Stride1xN_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride1_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int Fh=3, Fw=3, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride1_V_fp16(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3Stride1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3Stride1_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3Stride1_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride2_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int Fh=3, Fw=3, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride2_V_fp16(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3Stride2_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3Stride2_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3Stride2_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStrideS_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	/* Stride > 2 */
	int Fh=3, Fw=3;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3StrideS_V_fp16(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3StrideS_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3StrideS_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3StrideS_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv5x1BorderStrideNx1_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) {
		if (Wo_F==2) {
			KerConv4x1from5x1StrideNx1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 2);
			KerConv4x1from5x1StrideNx1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+1, Filter, 1);
		} else KerConv4x1from5x1StrideNx1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, PadL);
	}
	if (PadR) {
		if  ((Wo-Wo_L)==2) {
			KerConv4x1from5x1StrideNx1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-2, Filter, 3);
			KerConv4x1from5x1StrideNx1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 4);
		} else if (Wo==1) KerConv4x1from5x1StrideNx1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 5);
		else KerConv4x1from5x1StrideNx1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, PadR+2);
	}
}

static void __attribute__ ((noinline)) KerConv1x5BorderStride1xN_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) {
		if(Ho_F==2) {
			KerConv1x4from1x5Stride1xN_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 2);
			KerConv1x4from1x5Stride1xN_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F+Wo, Filter, 1);
		} else KerConv1x4from1x5Stride1xN_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, PadT);
	}
	if (PadB) {
		if((Ho-Ho_L)==2) {
			KerConv1x4from1x5Stride1xN_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3);
			KerConv1x4from1x5Stride1xN_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4);
		} else if (Ho==1) KerConv1x4from1x5Stride1xN_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 5);
		else KerConv1x4from1x5Stride1xN_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
	}
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride1_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	int Fh=5, Fw=5, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL==2) {
		KerConv4x5from5x5Stride1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 2);
		KerConv4x5from5x5Stride1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+1, Filter, 1);
	} else if (PadL==1) KerConv4x5from5x5Stride1_V_fp16(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 1);
	if (PadR==2) {
		if (Wo==1) KerConv4x5from5x5Stride1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 5);
		else {
			KerConv4x5from5x5Stride1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-2, Filter, 3);
			KerConv4x5from5x5Stride1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 4);
		}
	} else if (PadR==1) KerConv4x5from5x5Stride1_V_fp16(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 3);
	if (PadT==2) {
		KerConv5x4from5x5Stride1_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 2);
		KerConv5x4from5x5Stride1_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F+Wo, Filter, 1);
	} else if (PadT==1) KerConv5x4from5x5Stride1_H_fp16(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 1);
	if (PadB==2) {
		if (Ho==1) KerConv5x4from5x5Stride1_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L)*Wo+Wo_F, Filter, 5);
		else {
			KerConv5x4from5x5Stride1_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3);
			KerConv5x4from5x5Stride1_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4);
		}
	} else if (PadB==1) KerConv5x4from5x5Stride1_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride2_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	/* F=5 and stride=2 => No more than 1 output on each padded side */
	int Fh=5, Fw=5, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5Stride2_V_fp16(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, PadL);
	if (PadR) KerConv4x5from5x5Stride2_V_fp16(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, PadR+2);

	if (PadT) KerConv5x4from5x5Stride2_H_fp16(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, PadT);
	if (PadB) KerConv5x4from5x5Stride2_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStrideS_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	F16 Bias)

{
	/* Stride > 2 */
	int Fh=5, Fw=5;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5StrideS_V_fp16(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out, Filter, PadL);
	if (PadR) KerConv4x5from5x5StrideS_V_fp16(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out+Wo-1, Filter, PadR+2);

	if (PadT) KerConv5x4from5x5StrideS_H_fp16(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Wo_F, Filter, PadT);
	if (PadB) KerConv5x4from5x5StrideS_H_fp16(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
}

/* Convolution, body processing (covers both padded and non padded variants)

	Input feature maps, Output feature maps and filter on half words

		KerConv1x1Stride1_Body_fp16		1x1 convolution, stride 1
		KerConv1x1Stride2_Body_fp16		1x1 convolution, stride 2
		KerConv1x1StrideS_Body_fp16		1x1 convolution, stride S

		KerConv3x1Stride1x1_Body_fp16		3x1 convolution, stride 1x1
		KerConv3x1Stride2x1_Body_fp16		3x1 convolution, stride 2x1
		KerConv1x3Stride1x1_Body_fp16		1x3 convolution, stride 1x1
		KerConv1x3Stride1x2_Body_fp16		1x3 convolution, stride 1x2
		KerConv3x3Stride1_Body_fp16		3x3 convolution, stride 1
		KerConv3x3Stride2_Body_fp16		3x3 convolution, stride 2
		KerConv3x3StrideS_Body_fp16		3x3 convolution, stride S

		KerConv5x1Stride1x1_Body_fp16		5x1 convolution, stride 1x1
		KerConv5x1Stride2x1_Body_fp16		5x1 convolution, stride 2x1
		KerConv1x5Stride1x1_Body_fp16		1x5 convolution, stride 1x1
		KerConv1x5Stride1x2_Body_fp16		1x5 convolution, stride 1x2
		KerConv5x5Stride1_Body_fp16		5x5 convolution, stride 1
		KerConv5x5Stride2_Body_fp16		5x5 convolution, stride 2
		KerConv5x5StrideS_Body_fp16		5x5 convolution, stride S

		KerConvNxNStrideS_Body_fp16		NxN convolution, stride S
		KerConvNxMStrideSxSy_Body_fp16		NxM convolution, stride Sx, Sy
		KerConvNxMDxDyStrideSxSy_Body_fp16	NxM convolution, dilation Dx,Dy, stride Sx, Sy
*/

static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias)

{
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16  c0 = Filter[0];
	F16V C0 = (F16V) {c0, c0};
	F16V B = (F16V) {Bias, Bias};
	int IterW = Wo_L-Wo_F;
	if (IterW&0x1) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			F16V *LineOut = (F16V *) (&Out[Wo*h+Wo_F]);
			F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for (unsigned int w=0; w<(IterW/2); w++) {
				LineOut[w] = B + ((F16V *)PtI)[w]*C0;
			}
			Out[Wo*h+Wo_L-1] = Bias + PtI[IterW-1]*c0;
		}
	} else {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			F16V *LineOut = (F16V *) (&Out[Wo*h+Wo_F]);
			F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for (unsigned int w=0; w<(IterW/2); w++) {
				LineOut[w] = B + ((F16V *)PtI)[w]*C0;
			}
		}
	}
}



static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_2In_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	unsigned int Stride = 1;
	unsigned int PadL = Pad[0], PadT = Pad[2];

	F16 C0 = Filter[0], C1 = Filter[1];
	int IterW = Wo_L-Wo_F;
       	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		F16 *LineOut = &Out[Wo*h+Wo_F];
		F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=0; w<IterW; w++) {
			LineOut[w] = Bias + PtI[w]*C0 + PtI[w+W*H]*C1;
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_2In_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	unsigned int Stride = 2;
	unsigned int PadL = Pad[0], PadT = Pad[2];

	F16 C0 = Filter[0], C1 = Filter[1];
	int IterW = Wo_L-Wo_F;
       	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		F16 *LineOut = &Out[Wo*h+Wo_F];
		F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=0; w<IterW; w++) {
			LineOut[w] = Bias + PtI[2*w]*C0 + PtI[2*w+W*H]*C1;
		}
	}
}



static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16 C0 = Filter[0];
	F16V B = (F16V) {Bias, Bias};
	int IterW = Wo_L-Wo_F;
	if (IterW&0x1) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			F16V *LineOut = (F16V *) (&Out[Wo*h+Wo_F]);
			F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for (unsigned int w=0; w<(IterW/2); w++) {
				LineOut[w] = B + gap_pack2f16(PtI[4*w]*C0, PtI[4*w+2]*C0);
			}
			Out[Wo*h+Wo_L-1] = Bias + PtI[2*(IterW-1)]*C0;
		}
	} else {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			F16V *LineOut = (F16V *) (&Out[Wo*h+Wo_F]);
			F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for (unsigned int w=0; w<(IterW/2); w++) {
				LineOut[w] = B + gap_pack2f16(PtI[4*w]*C0, PtI[4*w+2]*C0);
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1StrideS_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	F16 Bias
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16 C0 = Filter[0];
	int Fw = 1, Fh = 1;
	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		F16 *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 I = *PtI; PtI+=Stride;
			*PtO = Bias + I*C0; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv3x1Stride1x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0 = *((F16V *) &Filter[0]);
	F16 C1 = Filter[2];
	F16V V0;
	F16SV V1;
	int StrideX = 1;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;
	v2s Mask = (v2s) {1,2};

	for (int h=Ho_F; h<Ho_L; h++) {
		F16 *PtO = PtO1;
		F16 *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		V0 = ((F16V *) PtI)[0]; V1.S = PtI[2]; PtI+=3;
		for (int w=Wo_F; w<Wo_L; w++) {
			F16V A = V0*C0;
			*PtO = Bias + A[0]+A[1]+V1.S*C1; PtO++;
			V0 = __builtin_shuffle(V0, V1.V, Mask);
			V1.S = *PtI++;
		}
		PtO1+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv3x1Stride2x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)
{
	F16V C0 = *((F16V *) &Filter[0]);
	F16 C1 = Filter[2];
	F16V V0;
	F16 V1;
	unsigned short int StrideX = 2;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16V *PtI = (F16V *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		F16 *PtO = PtO1;
		V0 = *PtI++; V1 = *(F16 *) PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			F16V A = V0*C0;
			*PtO = Bias + A[0]+A[1]+V1*C1; PtO+=Wo;
			V0 = *PtI++; V1 = *(F16 *) PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x3Stride1x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0 = *((F16V *) &Filter[0]);
	F16 C1 = Filter[2];
	F16V V0;
	F16SV V1;
	v2s Mask = (v2s) {1,2};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = (F16 *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		F16 *PtO = PtO1;
		V0[1] = *PtI; PtI = PtI+W;
		V1.S = *PtI; PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V0 = __builtin_shuffle(V0, V1.V, Mask);
			V1.S = *PtI; PtI = PtI+W;
			F16V A = V0*C0;
			*PtO = Bias + A[0]+A[1]+V1.S*C1; PtO+=Wo;

		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x3Stride1x2_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0 = *((F16V *) &Filter[0]);
	F16 C1 = Filter[2];
	F16V V0;
	F16 V1;
	v2s Mask = (v2s) {1,2};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = (F16 *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		F16 *PtO = PtO1;
		V1 = *PtI; PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V0 = (F16V) gap_pack2f16(V1, *PtI); PtI = PtI+W;
			V1 = *PtI; PtI = PtI+W;
			F16V A = V0*C0;
			*PtO = Bias + A[0]+A[1]+V1*C1; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0 = *((F16V *) &Filter[0]),	     C1 = *((F16V *) &Filter[3]),
	     C2 = gap_pack2f16(Filter[2], Filter[5]), C3 = *((F16V *) &Filter[6]),
	     C4 = gap_pack2f16(Filter[8], 0);
	F16V V0, V1, V2, V3, V4;
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-4;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16V *PtI = (F16V *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		F16 *PtO = PtO1;
		V0 = *PtI++; V2 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
		V1 = *PtI++; V3 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2); V2 = __builtin_shuffle(V2, V3, (v2s){0,2});
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V3 = *PtI++; V4 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
			F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4;
			V0 = V1; V1 = V3;
			V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
			*PtO = Bias + A[0]+A[1]; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride2_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0 = *((F16V *) &Filter[0]), C1 = gap_pack2f16(Filter[2], 0),
	     C2 = *((F16V *) &Filter[3]), C3 = gap_pack2f16(Filter[5], 0),
	     C4 = *((F16V *) &Filter[6]), C5 = gap_pack2f16(Filter[8], 0);
	F16V V0, V1, V2, V3, V4, V5;
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-4;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16V *PtI = (F16V *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		F16 *PtO = PtO1;
		V0 = *PtI++; V1 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V2 = *PtI++; V3 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
			V4 = *PtI++; V5 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-2);
			F16V A = V0*C0+V1*C1+V2*C2+V3*C3+V4*C4+V5*C5;
			V0 = V4; V1 = V5;
			*PtO = Bias + A[0]+A[1]; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3StrideS_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	F16 Bias
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16V Cv0 = *((F16V *)(Filter+0)),
	     Cv1 = *((F16V *)(Filter+3)),
	     Cv2 = *((F16V *)(Filter+6));
	F16 C0 = Filter[2],
	    C1 = Filter[5],
	    C2 = Filter[8];
	int Fw = 3, Fh = 3;
	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 *PtI = In + (h*Stride-PadT)*W + (w*Stride-PadL);
			F16V Iv0 = *((F16V *) PtI); PtI+=2; F16 I = *PtI; PtI+=W-(Fw-1);
			F16V A = Iv0*Cv0;
			F16 Acc = I*C0;
			Iv0 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv1; Acc += I*C1;
			Iv0 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv2; Acc += I*C2;
			*PtO = Bias + A[0]+A[1]+Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv5x1Stride1x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)
{
	F16V C0 = *((F16V *) &Filter[0]), C1 = *((F16V *) &Filter[2]);
	F16 C2 = Filter[4];
	F16V V0, V1;
	v2s Mask = (v2s) {1,2};
	F16SV V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	F16 *PtO = Out+Wo*Ho_F+Wo_F;

	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		F16V *PtI = (F16V *) (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		V0[1] = *((F16 *)PtI);  PtI = (F16V *) ((F16 *)PtI + 1);
		V1 = *PtI++;
		V2.S = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI + 1);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			V0 = __builtin_shuffle(V0, V1, Mask); V1 = __builtin_shuffle(V1, V2.V, Mask);
			V2.S = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI + 1);
			F16V A = V0*C0+V1*C1;
			*PtO = Bias + A[0]+A[1]+V2.S*C2; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv5x1Stride2x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)
{
	F16V C0 = *((F16V *) &Filter[0]), C1 = *((F16V *) &Filter[2]);
	F16 C2 = Filter[4];
	F16V V0, V1;
	v2s Mask = (v2s) {1,2};
	F16 V2;
	unsigned short int StrideX = 2;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	F16 *PtO = Out+Wo*Ho_F+Wo_F;

	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		F16 *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		V1 = *((F16V *)PtI); PtI+=2;
		V2 = *PtI; PtI++;
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			V0 = V1; V1 = gap_pack2f16(V2, *PtI); PtI++; V2 = *PtI; PtI++;
			F16V A = V0*C0+V1*C1;
			*PtO = Bias + A[0]+A[1]+V2*C2; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv1x5Stride1x1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)
{
	F16V C0 = *((F16V *) &Filter[0]), C1 = *((F16V *) &Filter[2]);
	F16 C2 = Filter[4];
	F16V V0, V1;
	v2s Mask = (v2s) {1,2};
	F16SV V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		F16 *PtO = Out+Wo*Ho_F+w;
		F16 x1,x2;
		V0[1] = *PtI; PtI = PtI + W;
		x1 = *PtI; PtI = PtI + W;
		x2 = *PtI; PtI = PtI + W;
		V1 = gap_pack2f16(x1, x2);
		V2.S = *PtI; PtI = PtI + W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V0 = __builtin_shuffle(V0, V1, Mask); V1 = __builtin_shuffle(V1, V2.V, Mask);
			V2.S = *PtI; PtI = PtI + W;
			F16V A = V0*C0+V1*C1;
			*PtO = Bias + A[0]+A[1]+V2.S*C2; PtO+=Wo;
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x5Stride1x2_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)
{
	F16V C0 = *((F16V *) &Filter[0]), C1 = *((F16V *) &Filter[2]);
	F16 C2 = Filter[4];
	F16V V0, V1;
	F16 V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16 *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		F16 *PtO = Out+Wo*Ho_F+w;
		F16 x1,x2;
		x1 = *PtI; PtI += W; x2 = *PtI; PtI += W; V1 = gap_pack2f16(x1, x2);
		V2 = *PtI; PtI += W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V0 = V1; V1 = gap_pack2f16(V2, *PtI); PtI += W; V2 = *PtI; PtI += W;
			F16V A = V0*C0+V1*C1;
			*PtO = Bias + A[0]+A[1]+V2*C2; PtO+=Wo;
		}
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride1_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0  = *((F16V *) &Filter[0]),	      C1  = *((F16V *) &Filter[2]),
	     C2  = *((F16V *) &Filter[5]),	      C3  = *((F16V *) &Filter[7]),
	     C4  = *((F16V *) &Filter[10]),	     C5  = *((F16V *) &Filter[12]),
	     C6  = *((F16V *) &Filter[15]),	     C7  = *((F16V *) &Filter[17]),
	     C8  = *((F16V *) &Filter[20]),	     C9  = *((F16V *) &Filter[22]),
	     C10 = gap_pack2f16(Filter[4], Filter[9]),  C11 = gap_pack2f16(Filter[14], Filter[19]),
	     C12 = gap_pack2f16(Filter[24], 0);

	F16V V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
	v2s Mask  = {1,2};

	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W - 8;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16V *PtI = (F16V *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		F16 *PtO = PtO1;
		F16 X, Y;
		V0 = *PtI++; V1 = *PtI++; X = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4);
		V2 = *PtI++; V3 = *PtI++; Y = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4); V10 = gap_pack2f16(X, Y);
		V4 = *PtI++; V5 = *PtI++; X = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4);
		V6 = *PtI++; V7 = *PtI++; Y = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4); V11 = gap_pack2f16(X, Y);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V8 = *PtI++; V9 = *PtI++; V12 = *PtI; PtI = (F16V *) ((F16 *)PtI+W-4);
			F16V A = V0*C0+V1*C1+V10*C10+V2*C2+V3*C3+V4*C4+V5*C5+V11*C11+V6*C6+V7*C7+V8*C8+V9*C9+V12*C12;
			*PtO = Bias + A[0]+A[1]; PtO+=Wo;
			V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
			V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride2_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	F16 Bias
	)

{
	F16V C0  = *((F16V *) &Filter[0]),	      C1  = *((F16V *) &Filter[2]),
	     C2  = *((F16V *) &Filter[5]),	      C3  = *((F16V *) &Filter[7]),
	     C4  = *((F16V *) &Filter[10]),	     C5  = *((F16V *) &Filter[12]),
	     C6  = *((F16V *) &Filter[15]),	     C7  = *((F16V *) &Filter[17]),
	     C8  = *((F16V *) &Filter[20]),	     C9  = *((F16V *) &Filter[22]),
	     C10 = gap_pack2f16(Filter[4], Filter[9]),  C11 = gap_pack2f16(Filter[14], Filter[19]),
	     C12 = gap_pack2f16(Filter[24], 0.0);
	F16V V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;

	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16 *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-8;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		F16V *PtI = (F16V *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		F16 *PtO = PtO1;
		F16 X, Y;
		V0 = *PtI++; V1 = *PtI++; X = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4);
		V2 = *PtI++; V3 = *PtI++; Y = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4); V10 = gap_pack2f16(X, Y);
		V4 = *PtI++; V5 = *PtI++; X = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4); V11 = gap_pack2f16(X, 0);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			V6 = *PtI++; V7 = *PtI++; X = *((F16 *) PtI); PtI = (F16V *) ((F16 *)PtI+W-4); V11[1] = X; // = gap_pack2f16((int)V11, X);
			V8 = *PtI++; V9 = *PtI++; V12 = *PtI;	 PtI = (F16V *) ((F16 *)PtI+W-4);
			F16V A = V0*C0+V1*C1+V10*C10+V2*C2+V3*C3+V4*C4+V5*C5+V11*C11+V6*C6+V7*C7+V8*C8+V9*C9+V12*C12;
			*PtO = Bias + A[0]+A[1]; PtO+=Wo;
			V10 = V11; V11 = V12; V0 = V4; V1 = V5; V2 = V6; V3 = V7; V4 = V8; V5 = V9;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5StrideS_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	F16 Bias
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	F16V Cv0 = *((F16V *)(Filter+ 0)), Cv1 = *((F16V *)(Filter+ 2)),
	     Cv2 = *((F16V *)(Filter+ 5)), Cv3 = *((F16V *)(Filter+ 7)),
	     Cv4 = *((F16V *)(Filter+10)), Cv5 = *((F16V *)(Filter+12)),
	     Cv6 = *((F16V *)(Filter+15)), Cv7 = *((F16V *)(Filter+17)),
	     Cv8 = *((F16V *)(Filter+20)), Cv9 = *((F16V *)(Filter+22));
	F16 C0 = Filter[ 4],
	    C1 = Filter[ 9],
	    C2 = Filter[14],
	    C3 = Filter[19],
	    C4 = Filter[24];
	int Fw = 5, Fh = 5;
	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;

	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 *PtI = In + (h*Stride-PadT)*W + (w*Stride-PadL);
			F16V Iv0, Iv1, A;
			F16 I, Acc;
			Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A = Iv0*Cv0+Iv1*Cv1; Acc = I*C0;
			Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv2+Iv1*Cv3; Acc += I*C1;
			Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv4+Iv1*Cv5; Acc += I*C2;
			Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv6+Iv1*Cv7; Acc += I*C3;
			Iv0 = *((F16V *) PtI); PtI+=2; Iv1 = *((F16V *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			A += Iv0*Cv8+Iv1*Cv9; Acc += I*C4;
			*PtO = Bias + A[0]+A[1]+Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConvNxNStrideS_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	F16 Bias
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fw*i+j];
			}
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConvNxMStrideSxSy_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	F16 Bias
	)
{
       unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL+j)]*Filter[Fw*i+j];
			}
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConvNxMDxDyStrideSxSy_Body_fp16(
	F16 *__restrict__ In,
	F16 *__restrict__ Out,
	F16 *__restrict__ Filter,
	int Fw,
	int Fh,
	int Dw,
	int Dh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	F16 Bias
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	F16 *PtO = Out+Wo*Ho_F+Wo_F;
	F16 *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			F16 Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadT+i*Dh)*W + (w*StrideX-PadL+j*Dw)]*Filter[Fw*i+j];
			}
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are half words (_fp16) Dim=1,3,5,N, Stride=1,2,S

	Output feature maps are evaluated in parallel, one per core

	Argument data type: KerConv_fp16_T

	KerParConvDW1x1Stride1_fp16
	KerParConvDW1x1Stride2_fp16
	KerParConvDW1x1StrideS_fp16

	KerParConvDW3x1Stride1x1_fp16
		|------	KerConv3x1Stride1x1_Body_fp16
		|------	KerConv3x1StrideNx1_Border_fp16
	KerParConvDW3x1Stride2x1_fp16
		|------	KerConv3x1Stride2x1_Body_fp16
		|------	KerConv3x1StrideNx1_Border_fp16
	KerParConvDW1x3Stride1x1_fp16
		|------	KerConv1x3Stride1x1_Body_fp16
		|------	KerConv1x3Stride1xN_Border_fp16
	KerParConvDW1x3Stride1x2_fp16
		|------	KerConv1x3Stride1x2_Body_fp16
		|------	KerConv1x3Stride1xN_Border_fp16
	KerParConvDW3x3Stride1_fp16
		|------	KerConv3x3Stride1_Body_fp16
		|------	KerConv3x3Stride1_Border_fp16
	KerParConvDW3x3Stride2_fp16
		|------	KerConv3x3Stride2_Body_fp16
		|------	KerConv3x3Stride2_Border_fp16
	KerParConvDW3x3StrideS_fp16
		|------	KerConv3x3StrideS_Body_fp16
		|------	KerConv3x3StrideS_Border_fp16

	KerParConvDW5x1Stride1x1_fp16
		|------	KerConv5x1Stride1x1_Body_fp16
		|------	KerConv5x1StrideNx1_Border_fp16
	KerParConvDW5x1Stride2x1_fp16
		|------	KerConv5x1Stride2x1_Body_fp16
		|------	KerConv5x1StrideNx1_Border_fp16
	KerParConvDW1x5Stride1x1_fp16
		|------	KerConv1x5Stride1x1_Body_fp16
		|------	KerConv1x5Stride1xN_Border_fp16
	KerParConvDW1x5Stride1x2_fp16
		|------	KerConv1x5Stride1x2_Body_fp16
		|------	KerConv1x5Stride1xN_Border_fp16
	KerParConvDW5x5Stride1_fp16
		|------	KerConv5x5Stride1_Body_fp16
		|------	KerConv5x5Stride1_Border_fp16
	KerParConvDW5x5Stride2_fp16
		|------	KerConv5x5Stride2_Body_fp16
		|------	KerConv5x5Stride2_Border_fp16
	KerParConvDW5x5StrideS_fp16
		|------	KerConv5x5StrideS_Body_fp16
		|------	KerConv5x5StrideS_Border_fp16

	KerParConvDWNxNStrideS_fp16
		|------	KerConvNxNStrideS_Body_fp16
		|------	KerConvNxNStrideS_Border_fp16

	KerParConvDWNxMStrideSxSy_fp16
		|------	KerConvNxMStrideSxSy_Body_fp16
		|------	KerConvNxMStrideSxSy_Border_fp16

	KerParConvDWNxMDxDyStrideSxSy_fp16
		|------	KerConvNxMDxDyStrideSxSy_Body_fp16
		|------	KerConvNxMDxDyStrideSxSy_Border_fp16

*/

void KerParConvDW1x1Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		KerConv1x1Stride1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Bias_fp16[of]);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x1Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		KerConv1x1Stride2_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Bias_fp16[of]);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x1StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		KerConv1x1StrideS_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Bias_fp16[of]);
	}
	gap_waitbarrier(0);
}

void KerParConvDW3x1Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=3, FSy=1, Sx=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B=Bias_fp16[of];
		KerConv3x1Stride1x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW3x1Stride2x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=3, FSy=1, Sx=2, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv3x1Stride2x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x3Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv1x3Stride1x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv1x3BorderStride1xN_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x3Stride1x2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv1x3Stride1x2_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv1x3BorderStride1xN_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW3x3Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv3x3Stride1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv3x3BorderStride1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW3x3Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv3x3Stride2_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv3x3BorderStride2_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW3x3StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv3x3StrideS_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B);
		if ((int)PadIn) KerConv3x3BorderStrideS_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW5x1Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=5, FSy=1, Sx=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv5x1Stride1x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW5x1Stride2x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=5, FSy=1, Sx=2, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv5x1Stride2x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x5Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv1x5Stride1x1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv1x5BorderStride1xN_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW1x5Stride1x2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv1x5Stride1x2_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv1x5BorderStride1xN_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW5x5Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv5x5Stride1_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv5x5BorderStride1_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW5x5Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv5x5Stride2_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B);
		if ((int)PadIn) KerConv5x5BorderStride2_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDW5x5StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConv5x5StrideS_Body_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B);
		if ((int)PadIn) KerConv5x5BorderStrideS_fp16(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDWNxNStrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=Arg->N, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FS*FS*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConvNxNStrideS_Body_fp16(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B);
		if ((int)PadIn) KerConvNxNStrideS_Border_fp16(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDWNxMStrideSxSy_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConvNxMStrideSxSy_Body_fp16(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, B);
		if ((int)PadIn) KerConvNxMStrideSxSy_Border_fp16(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

void KerParConvDWNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Bias_fp16 = (F16 * __restrict__) Arg->Bias;
	F16 * __restrict__ Out = Arg->Out;
	

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-(Dx*(FSx-1)+1)+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput((Dx*(FSx-1)+1), PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, (Dx*(FSx-1)+1), PadIn[0], Sx));
	int Ho = (Arg->UsedH-(Dy*(FSy-1)+1)+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput((Dy*(FSy-1)+1), PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, (Dy*(FSy-1)+1), PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		F16 *in = In+W*H*of, *filter = Filter+FSx*FSy*of, *out = Out+Wo*Ho*of;
		F16 B = Bias_fp16[of];
		KerConvNxMDxDyStrideSxSy_Body_fp16(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, B);
		if ((int)PadIn) KerConvNxMDxDyStrideSxSy_Border_fp16(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn, B);
	}
	gap_waitbarrier(0);
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are half words (_fp16) Dim=1,3,5,N, Stride=1,2,S

	A single feature map is evaluated in parallel on all cores

	Argument data type: KerConv_fp16_T

	KerConvDW1x1Stride1_fp16
	KerConvDW1x1Stride2_fp16
	KerConvDW1x1StrideS_fp16

	KerConvDW3x1Stride1x1_fp16
		|------	KerConv3x1Stride1x1_Body_fp16
		|------	KerConv3x1StrideNx1_Border_fp16
	KerConvDW3x1Stride2x1_fp16
		|------	KerConv3x1Stride2x1_Body_fp16
		|------	KerConv3x1StrideNx1_Border_fp16
	KerConvDW1x3Stride1x1_fp16
		|------	KerConv1x3Stride1x1_Body_fp16
		|------	KerConv1x3Stride1xN_Border_fp16
	KerConvDW1x3Stride1x2_fp16
		|------	KerConv1x3Stride1x2_Body_fp16
		|------	KerConv1x3Stride1xN_Border_fp16
	KerConvDW3x3Stride1_fp16
		|------	KerConv3x3Stride1_Body_fp16
		|------	KerConv3x3Stride1_Border_fp16
	KerConvDW3x3Stride2_fp16
		|------	KerConv3x3Stride2_Body_fp16
		|------	KerConv3x3Stride2_Border_fp16
	KerConvDW3x3StrideS_fp16
		|------	KerConv3x3StrideS_Body_fp16
		|------	KerConv3x3StrideS_Border_fp16

	KerConvDW5x1Stride1x1_fp16
		|------	KerConv5x1Stride1x1_Body_fp16
		|------	KerConv5x1StrideNx1_Border_fp16
	KerConvDW5x1Stride2x1_fp16
		|------	KerConv5x1Stride2x1_Body_fp16
		|------	KerConv5x1StrideNx1_Border_fp16
	KerConvDW1x5Stride1x1_fp16
		|------	KerConv1x5Stride1x1_Body_fp16
		|------	KerConv1x5Stride1xN_Border_fp16
	KerConvDW1x5Stride1x2_fp16
		|------	KerConv1x5Stride1x2_Body_fp16
		|------	KerConv1x5Stride1xN_Border_fp16
	KerConvDW5x5Stride1_fp16
		|------	KerConv5x5Stride1_Body_fp16
		|------	KerConv5x5Stride1_Border_fp16
	KerConvDW5x5Stride2_fp16
		|------	KerConv5x5Stride2_Body_fp16
		|------	KerConv5x5Stride2_Border_fp16
	KerConvDW5x5StrideS_fp16
		|------	KerConv5x5StrideS_Body_fp16
		|------	KerConv5x5StrideS_Border_fp16

	KerConvDWNxNStrideS_fp16
		|------	KerConvNxNStrideS_Body_fp16
		|------	KerConvNxNStrideS_Border_fp16

	KerConvDWNxMStrideSxSy_fp16
		|------	KerConvNxMStrideSxSy_Body_fp16
		|------	KerConvNxNStrideSxSy_Border_fp16

	KerConvDWNxMDxDyStrideSxSy_fp16
		|------	KerConvNxMDxDyStrideSxSy_Body_fp16
		|------	KerConvNxNDxDyStrideSxSy_Border_fp16

*/

void KerConvDW1x1Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1Stride1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
	gap_waitbarrier(0);
}

void KerConvDW1x1Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1Stride2_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
	gap_waitbarrier(0);
}

void KerConvDW1x1StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1StrideS_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B);
	gap_waitbarrier(0);
}

void KerConvDW3x1Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=3, Sx=1;
	unsigned int FSy=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x1Stride1x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW3x1Stride2x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=3, Sx=2;
	unsigned int FSy=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x1Stride2x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW1x3Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x3Stride1x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv1x3BorderStride1xN_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW1x3Stride1x2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x3Stride1x2_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv1x3BorderStride1xN_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW3x3Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3Stride1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv3x3BorderStride1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW3x3Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3Stride2_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv3x3BorderStride2_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW3x3StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3StrideS_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B);
		if ((int)PadIn) KerConv3x3BorderStrideS_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}


void KerConvDW5x1Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=5, Sx=1;
	unsigned int FSy=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x1Stride1x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW5x1Stride2x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=5, Sx=2;
	unsigned int FSy=1, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x1Stride2x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW1x5Stride1x1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x5Stride1x1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv1x5BorderStride1xN_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW1x5Stride1x2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x5Stride1x2_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv1x5BorderStride1xN_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW5x5Stride1_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=1;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5Stride1_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv5x5BorderStride1_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW5x5Stride2_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5Stride2_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B);
		if ((int)PadIn) KerConv5x5BorderStride2_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDW5x5StrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5StrideS_Body_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B);
		if ((int)PadIn) KerConv5x5BorderStrideS_fp16(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDWNxNStrideS_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FS=Arg->N, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxNStrideS_Body_fp16(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B);
		if ((int)PadIn) KerConvNxNStrideS_Border_fp16(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}


void KerConvDWNxMStrideSxSy_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxMStrideSxSy_Body_fp16(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg, B);
		if ((int)PadIn) KerConvNxMStrideSxSy_Border_fp16(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}

void KerConvDWNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	F16 * __restrict__ Filter = Arg->Filter;
	F16 * __restrict__ Out = Arg->Out;
	
	F16 B = Arg->Bias[0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-(Dx*(FSx-1)+1)+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput((Dx*(FSx-1)+1), PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, (Dx*(FSx-1)+1), PadIn[0], Sx));
	int Ho = (Arg->UsedH-(Dy*(FSy-1)+1)+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput((Dy*(FSy-1)+1), PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, (Dy*(FSy-1)+1), PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxMDxDyStrideSxSy_Body_fp16(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg, B);
		if ((int)PadIn) KerConvNxMDxDyStrideSxSy_Border_fp16(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg, B);
	}
	gap_waitbarrier(0);
}


#pragma GCC diagnostic pop