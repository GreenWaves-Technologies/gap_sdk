/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include <stdio.h>

#include "CNN_BasicKernels.h"

//#define Min(a, b) 		(((a)<(b))?(a):(b))
#define Minu(a, b) 		(( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

#define __Ncore() __builtin_pulp_CoreCount()

static int CoreCountDynamic = 0;
static int ActiveCore = 8;

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

	if (CoreCountDynamic) NCore = ActiveCore; else NCore = __Ncore();
	Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;

}

static inline unsigned int __attribute__((always_inline)) ChunkSizeEven(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = __Ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return (Chunk+(Chunk&0x1));

}

/*
	Convolution related function. Set the convolution output to the bias value.
	Out:	short int * pointer
	W, H:	Size of the output
	Bias:	short int, bias value.
*/
void __attribute__ ((noinline)) KerSetInBias(KerSetInBiasT *Arg)

{
	Word16 * __restrict__ Out = Arg->Out;
	Word16 Bias = Arg->Bias;
	int W = Arg->W;
	int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	v2s * __restrict__ VectOut = (v2s *) (Out+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/2); i++) VectOut[i] = (v2s) {Bias, Bias};
	if (Iter&0x1) Out[Last-1] = Bias;

	CLUSTER_SynchBarrier(0);
}
void __attribute__ ((noinline)) KerSetInBias_int(KerSetInBiasT *Arg)

{
  int * __restrict__ Out = (int*) Arg->Out;
	Word16 Bias = Arg->Bias;
	int W = Arg->W;
	int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(W*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W*H);
	int *  __restrict__ VectOut = (int*) (Out+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter); i++) VectOut[i] = Bias<<8;
	if (Iter&0x1) Out[Last-1] = Bias<<8;

	CLUSTER_SynchBarrier(0);
}

/*
	Convolution related function. Set two convolution outputs to their respective bias values.
	Out0:	short int * pointer, first output
	Out1:	short int * pointer, second output
	W, H:	Size of the output, outputs dimension
	Bias0:	short int, bias value for Out0.
	Bias1:	short int, bias value for Out1.
*/

void __attribute__ ((noinline)) KerSetInBias2(KerSetInBias2T *Arg)

{
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	Word16 Bias0 = Arg->Bias0;
	Word16 Bias1 = Arg->Bias1;
	int W = Arg->W;
	int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W/2)*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W/2)*H);
	v2s * __restrict__ VectOut0 = (v2s *) (Out0+First);
	v2s * __restrict__ VectOut1 = (v2s *) (Out1+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/2); i++) {
		VectOut0[i] = (v2s) {Bias0, Bias0};
		VectOut1[i] = (v2s) {Bias1, Bias1};
	}
	if (Iter&0x1) {
		Out0[Last-1] = Bias0;
		Out1[Last-1] = Bias1;
	}

	CLUSTER_SynchBarrier(0);
}

/*
	Convolution related function. Set three convolution outputs to their respective bias values.
	Out0:	short int * pointer, first output
	Out1:	short int * pointer, second output
	Out2:	short int * pointer, third output
	W, H:	Size of the output, outputs dimension
	Bias0:	short int, bias value for Out0.
	Bias1:	short int, bias value for Out1.
	Bias2:	short int, bias value for Out2.
*/

void __attribute__ ((noinline)) KerSetInBias3(KerSetInBias3T *Arg)

{
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	Word16 * __restrict__ Out2 = Arg->Out2;
	Word16 Bias0 = Arg->Bias0;
	Word16 Bias1 = Arg->Bias1;
	Word16 Bias2 = Arg->Bias2;
	int W = Arg->W;
	int H = Arg->H;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W/2)*H);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W/2)*H);
	v2s * __restrict__ VectOut0 = (v2s *) (Out0+First);
	v2s * __restrict__ VectOut1 = (v2s *) (Out1+First);
	v2s * __restrict__ VectOut2 = (v2s *) (Out2+First);
	int Iter = (Last-First);
	int i;

	for (i=0; i<(Iter/2); i++) {
		VectOut0[i] = (v2s) {Bias0, Bias0};
		VectOut1[i] = (v2s) {Bias1, Bias1};
		VectOut2[i] = (v2s) {Bias2, Bias2};
	}
	if (Iter&0x1) {
		Out0[Last-1] = Bias0;
		Out1[Last-1] = Bias1;
		Out2[Last-1] = Bias2;
	}

	CLUSTER_SynchBarrier(0);
}

/*
	3x3 convolution, short int inputs and output
	The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the 9 convolution coefficients
	Norm:	Fixed point format
*/
void __attribute__ ((noinline)) KerConv3x3_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;
	v2s C0 = *((v2s *) &Filter[0]),            C1 = *((v2s *) &Filter[3]),
    	    C2 = gap8_pack2(Filter[2], Filter[5]), C3 = *((v2s *) &Filter[6]),
    	    C4 = gap8_pack2(Filter[8], 0);
	int Wo = W-3+1;
	int Ho = H-3+1;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);
	int i;
	unsigned j;
	v2s V0, V1, V2, V3, V4;
	v2s *Line;

	if (W%2) {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {

			Line = (v2s *) (&In[j]);
			V1 = *Line++; V2 = *Line; V2 = __builtin_shuffle(V2, (v2s) {1, 0});
			Line = (v2s *) (&In[W+j]);
			V3 = *Line++; V4 = *Line;
			for(i=0; i<Ho; i++) {
				int Acc = Out[i*Wo+j]<<Norm;
				Line = (v2s *) (&In[(i+2)*W+j]);
				V0 = V1; V1 = V3;
				V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
				V3 = *Line++; V4 = *Line;
				Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
				Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
				Acc = gap8_sumdotp2(V4, C4, Acc);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
		}
	} else {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			Line = (v2s *) (&In[j]);
			V1 = *Line++; V2 = *Line; Line += ((W/2)-1); V2 = __builtin_shuffle(V2, (v2s) {1, 0});
			V3 = *Line++; V4 = *Line; Line += ((W/2)-1);
			for(i=0; i<Ho; i++) {
				int Acc = Out[i*Wo+j]<<Norm;
				V0 = V1; V1 = V3;
				V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
				V3 = *Line++; V4 = *Line; Line += ((W/2)-1);
				Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
				Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
				Acc = gap8_sumdotp2(V4, C4, Acc);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
		}
	}
	CLUSTER_SynchBarrier(0);
}

/*
	3x3 convolution, short int inputs and output
	The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the 9 convolution coefficients
	Norm:	Fixed point format
*/
void __attribute__ ((noinline)) KerDirectConv3x3_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;
	v2s C0 = *((v2s *) &Filter[0]),            C1 = *((v2s *) &Filter[3]),
    	    C2 = gap8_pack2(Filter[2], Filter[5]), C3 = *((v2s *) &Filter[6]),
    	    C4 = gap8_pack2(Filter[8], 0);
	int Wo = W-3+1;
	int Ho = H-3+1;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);
	int i;
	unsigned j;
	v2s V0, V1, V2, V3, V4;
	v2s *Line;

	if (W%2) {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {

			Line = (v2s *) (&In[j]);
			V1 = *Line++; V2 = *Line; V2 = __builtin_shuffle(V2, (v2s) {1, 0});
			Line = (v2s *) (&In[W+j]);
			V3 = *Line++; V4 = *Line;
			for(i=0; i<Ho; i++) {
				int Acc;
				Line = (v2s *) (&In[(i+2)*W+j]);
				V0 = V1; V1 = V3;
				V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
				V3 = *Line++; V4 = *Line;
				Acc = gap8_dotp2(V0, C0);         Acc = gap8_sumdotp2(V1, C1, Acc);
				Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
				Acc = gap8_sumdotp2(V4, C4, Acc);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((Acc>>Norm), 15);
			}
		}
	} else {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			Line = (v2s *) (&In[j]);
			V1 = *Line++; V2 = *Line; Line += ((W/2)-1); V2 = __builtin_shuffle(V2, (v2s) {1, 0});
			V3 = *Line++; V4 = *Line; Line += ((W/2)-1);
			for(i=0; i<Ho; i++) {
				int Acc;
				V0 = V1; V1 = V3;
				V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
				V3 = *Line++; V4 = *Line; Line += ((W/2)-1);
				Acc = gap8_dotp2(V0, C0);         Acc = gap8_sumdotp2(V1, C1, Acc);
				Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
				Acc = gap8_sumdotp2(V4, C4, Acc);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((Acc>>Norm), 15);
			}
		}
	}
	CLUSTER_SynchBarrier(0);
}


/*
	5x5 convolution, short int inputs and output
	The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the 9 convolution coefficients
	Norm:	Fixed point format
*/

void __attribute__ ((noinline)) KerConv5x5_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;
	v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
	    C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
	    C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
	    C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
	    C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
    	    C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
    	    C12 = gap8_pack2(Filter[24], 0);

	int Wo = W-5+1;
	int Ho = H-5+1;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);
	int i;
	unsigned j;
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
	v2s Mask  = {1,2};
	v2s *Line;

	if (W%2) {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			V10 = (v2s) {0, In[j+4]}; V11 = (v2s) {In[j+W+4], In[j+2*W+4]}; 	Line = (v2s *) (In + j);
			V2 = Line[0]; V3 = Line[1]; 						Line = (v2s *) (In + W + j);
			V4 = Line[0]; V5 = Line[1]; 						Line = (v2s *) (In + 2*W + j);
			V6 = Line[0]; V7 = Line[1]; 						Line = (v2s *) (In + 3*W + j);
			V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
			for(i=0; i<Ho; i++) {
				int S = Out[i*Wo+j]<<Norm;
				V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
				V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
				Line = (v2s *) (&In[(i+4)*W+j]);
				V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
				S = gap8_sumdotp2(V0,  C0,  S);
				S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V2,  C2,  S);
				S = gap8_sumdotp2(V3,  C3,  S); S = gap8_sumdotp2(V4,  C4,  S);
				S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V6,  C6,  S);
				S = gap8_sumdotp2(V7,  C7,  S); S = gap8_sumdotp2(V8,  C8,  S);
				S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V10, C10, S);
				S = gap8_sumdotp2(V11, C11, S); S = gap8_sumdotp2(V12, C12, S);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
			}
		}
	} else {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			V10 = (v2s) {0, In[j+4]}; V11 = (v2s) {In[j+W+4], In[j+2*W+4]};
			Line = (v2s *) (In + j);
			V2 = Line[0]; V3 = Line[1];                Line += (W>>1);
			V4 = Line[0]; V5 = Line[1];                Line += (W>>1);
			V6 = Line[0]; V7 = Line[1];                Line += (W>>1);
			V8 = Line[0]; V9 = Line[1]; V12 = Line[2]; Line += (W>>1);
			for(i=0; i<Ho; i++) {
				int S = Out[i*Wo+j]<<Norm;
				V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
				V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
				V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
				Line += (W>>1);
				S = gap8_sumdotp2(V0,  C0,  S);
				S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V2,  C2,  S);
				S = gap8_sumdotp2(V3,  C3,  S); S = gap8_sumdotp2(V4,  C4,  S);
				S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V6,  C6,  S);
				S = gap8_sumdotp2(V7,  C7,  S); S = gap8_sumdotp2(V8,  C8,  S);
				S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V10, C10, S);
				S = gap8_sumdotp2(V11, C11, S); S = gap8_sumdotp2(V12, C12, S);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
			}
		}
	}
	CLUSTER_SynchBarrier(0);
}

/*
	5x5 convolution, short int inputs and output
	The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the 9 convolution coefficients
	Norm:	Fixed point format
*/
void __attribute__ ((noinline)) KerDirectConv5x5_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;
	v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
	    C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
	    C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
	    C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
	    C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
    	    C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
    	    C12 = gap8_pack2(Filter[24], 0);

	int Wo = W-5+1;
	int Ho = H-5+1;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);
	int i;
	unsigned j;
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
	v2s Mask  = {1,2};
	v2s *Line;

	if (W%2) {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			V10 = (v2s) {0, In[j+4]}; V11 = (v2s) {In[j+W+4], In[j+2*W+4]}; 	Line = (v2s *) (In + j);
			V2 = Line[0]; V3 = Line[1]; 						Line = (v2s *) (In + W + j);
			V4 = Line[0]; V5 = Line[1]; 						Line = (v2s *) (In + 2*W + j);
			V6 = Line[0]; V7 = Line[1]; 						Line = (v2s *) (In + 3*W + j);
			V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
			for(i=0; i<Ho; i++) {
				int S;
				V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
				V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
				Line = (v2s *) (&In[(i+4)*W+j]);
				V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
				S = gap8_dotp2   (V0,  C0);
				S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V2,  C2,  S);
				S = gap8_sumdotp2(V3,  C3,  S); S = gap8_sumdotp2(V4,  C4,  S);
				S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V6,  C6,  S);
				S = gap8_sumdotp2(V7,  C7,  S); S = gap8_sumdotp2(V8,  C8,  S);
				S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V10, C10, S);
				S = gap8_sumdotp2(V11, C11, S); S = gap8_sumdotp2(V12, C12, S);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
			}
		}
	} else {
		// for(j=0; j<Wo; j++) {
		for (j=First; j<Last; j++) {
			V10 = (v2s) {0, In[j+4]}; V11 = (v2s) {In[j+W+4], In[j+2*W+4]};
			Line = (v2s *) (In + j);
			V2 = Line[0]; V3 = Line[1];                Line += (W>>1);
			V4 = Line[0]; V5 = Line[1];                Line += (W>>1);
			V6 = Line[0]; V7 = Line[1];                Line += (W>>1);
			V8 = Line[0]; V9 = Line[1]; V12 = Line[2]; Line += (W>>1);
			for(i=0; i<Ho; i++) {
				int S;
				V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
				V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
				V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
				Line += (W>>1);
				S = gap8_dotp2   (V0,  C0);
				S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V2,  C2,  S);
				S = gap8_sumdotp2(V3,  C3,  S); S = gap8_sumdotp2(V4,  C4,  S);
				S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V6,  C6,  S);
				S = gap8_sumdotp2(V7,  C7,  S); S = gap8_sumdotp2(V8,  C8,  S);
				S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V10, C10, S);
				S = gap8_sumdotp2(V11, C11, S); S = gap8_sumdotp2(V12, C12, S);
				Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
				// Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
			}
		}
	}
	CLUSTER_SynchBarrier(0);
}

/*
	NxN convolution, short int inputs and output
	The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the NxN convolution coefficients
	Norm:	Fixed point format

*/

/*
	NxN convolution, short int inputs and output
	The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the NxN convolution coefficients
	Norm:	Fixed point format

*/
void __attribute__ ((noinline)) KerConvNxN_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;

        int Wo = W-N+1;
        int Ho = H-N+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned j;
        // for(j=0; j<Wo; j++) {
        for(i=0; i<Ho; i++) {
        	for(j=First; j<Last; j++) {
                        int ii, jj;
			int Acc = Out[i*Wo+j]<<Norm;
                        for (ii=0; ii<N; ii++) {
				v2s *Coeff = (v2s *) (Filter + ii*N);
				v2s *Line = (v2s *) (In + (i+ii)*W + j);
				for (jj=0; jj<(N/2); jj++) Acc = gap8_sumdotp2(Coeff[jj], Line[jj], Acc);
				Acc += Filter[ii*N+(N-1)]*In[(i+ii)*W + j + (N-1)];
                                // for (jj=0; jj<N; jj++) Acc += Filter[ii*N+jj]*In[(i+ii)*W + j + jj];
                        }
			Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                        // Out[i*Wo+j] =  gap8_clip((Acc>>Norm), 15);
                }
        }
	CLUSTER_SynchBarrier(0);
}

/*
	NxM convolution, short int inputs and output (N is line, M is column)
	The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the NxN convolution coefficients
	Norm:	Fixed point format
*/
void __attribute__ ((noinline)) KerDirectConvNxM_fp(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);
        // for(j=0; j<Wo; j++) {
        for(i=0; i<Ho; i++) {
        	for(j=First; j<Last; j++) {
                        int ii, jj;
                        int Acc =  Out[i*Wo+j]<<Norm;
                        for (ii=0; ii<M; ii++) {
				v2s *Coeff = (v2s *) (Filter + ii*N);
				v2s *Line = (v2s *) (In + (i+ii)*W + j);
				for (jj=0; jj<(N/2); jj++) {
				  Acc = gap8_sumdotp2(Coeff[jj], Line[jj], Acc);
#ifdef PRINTDEB_TILE
				  printf("nxm: core %d i %d j %d ii %d jj %d coeff %x %x in %x %x acc %x\n",CoreId,i,j,ii,jj,Coeff[jj][0],Coeff[jj][1], Line[jj][0],Line[jj][1], Acc);
#endif
				}
				// N and M are even =>  aligned with v2s: no last addition
				//Acc += Filter[ii*N+(N-1)]*In[(i+ii)*W + j + (N-1)];
                                // for (jj=0; jj<N; jj++) Acc += Filter[ii*N+jj]*In[(i+ii)*W + j + jj];
                        }
			//printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
			Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                        // Out[i*Wo+j] =  gap8_clip((Acc>>Norm), 15);
                }
        }
	CLUSTER_SynchBarrier(0);
}

void __attribute__ ((noinline)) KerDirectConvNxM_fp_model(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);
        // for(j=0; j<Wo; j++) {
        for(i=0; i<Ho; i++) {
        	for(j=First; j<Last; j++) {
                        int ii, jj;
                        int Acc =  Out[i*Wo+j]<<Norm;
#ifdef PRINTDEB_TILE
			printf("id %d i %d j %d Acc0 %x\n",CoreId,i,j,Acc);
#endif
                        for (ii=0; ii<M; ii++) {
				short int *Coeff =  (Filter + ii*N);
				short int *Line =  (In + (i+ii)*W + j);
				for (jj=0; jj<5; jj++) {
				  Acc = Coeff[jj]*Line[jj] + Acc;
#ifdef PRINTDEB_TILE
				  printf("nxm(0): core %d i %d j %d ii %d jj %d coeff %x  in %x  acc %x\n",CoreId,i,j,ii,jj,Coeff[jj], Line[jj], Acc);
#endif
				}

                        }
                        for (ii=0; ii<M; ii++) {
				short int *Coeff =  (Filter + ii*N + 5);
				short int *Line =  (In + (i+ii)*W + j + 5);
				for (jj=0; jj<3; jj++) {
				  Acc = Coeff[jj]*Line[jj] + Acc;
#ifdef PRINTDEB_TILE
				  printf("nxm(1): core %d i %d j %d ii %d jj %d coeff %x in %x acc %x\n",CoreId,i,j,ii,jj,Coeff[jj], Line[jj], Acc);
#endif
				}

                        }
	//printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
			Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                }
        }
	CLUSTER_SynchBarrier(0);
}

void __attribute__ ((noinline)) KerDirectConvNxM_fp_tiled(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	int * __restrict__ Out = (int*) Arg->Out;
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
	v2s Mask  = {1,2};
	v2s *Line;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);
#ifdef  PRINTIN
	if (CoreId==0){
	  for (i=0;i<98;i++)
	    for(j=0;j<40;j++) printf("sw in %d %d %x\n",i,j,In[j+40*i]);
	}
#endif
	// first pass of 4 5x5 kernels
	int tile,tile5;
	for (tile=0;tile<4;tile++){
	  //pointer on the 5x5 kernel
	  Word16 *Filterl = Filter + tile*40;
	  v2s C0  = *((v2s *) &Filterl[0]),           C1  = *((v2s *) &Filterl[2]),
	    C2  = *((v2s *) &Filterl[N]),             C3  = *((v2s *) &Filterl[2+N]),
	    C4  = *((v2s *) &Filterl[2*N]),           C5  = *((v2s *) &Filterl[2+2*N]),
	    C6  = *((v2s *) &Filterl[3*N]),           C7  = *((v2s *) &Filterl[2+3*N]),
	    C8  = *((v2s *) &Filterl[4*N]),           C9  = *((v2s *) &Filterl[2+4*N]),
    	    C10 = gap8_pack2(Filterl[4], Filterl[4+N]), C11 = gap8_pack2(Filterl[4+2*N], Filterl[4+3*N]),
    	    C12 = gap8_pack2(Filterl[4+4*N], 0);

#ifdef PRINTDEB_TILE0
	  printf("id %d tile %d\n",CoreId,tile);
	  printf("id %d C0 %x %x C1 %x %x C10 %x\n",CoreId,C0[0],C0[1],C1[0],C1[1],C10[0]);
	  printf("id %d C2 %x %x C3 %x %x C10 %x\n",CoreId,C2[0],C2[1],C3[0],C3[1],C10[1]);
	  printf("id %d C4 %x %x C5 %x %x C11 %x\n",CoreId,C4[0],C4[1],C5[0],C5[1],C11[0]);
	  printf("id %d C6 %x %x C7 %x %x C11 %x\n",CoreId,C6[0],C6[1],C7[0],C7[1],C11[1]);
	  printf("id %d C8 %x %x C9 %x %x C12 %x\n",CoreId,C8[0],C8[1],C9[0],C9[1],C12[0]);
#endif

	  for(j=First; j<Last; j++) {
	    int ii, jj;
	    int tile5 = 5*tile*W;
	    V10 = (v2s) {0, In[j + 4 + tile5 ]}; V11 = (v2s) {In[j + W+4 + tile5], In[j + 2*W+4 + tile5]};
	    Line = (v2s *) (In + j + tile5);
	    V2 = Line[0]; V3 = Line[1];                Line += (W>>1);
	    V4 = Line[0]; V5 = Line[1];                Line += (W>>1);
	    V6 = Line[0]; V7 = Line[1];                Line += (W>>1);
	    V8 = Line[0]; V9 = Line[1]; V12 = Line[2]; Line += (W>>1);

	    for(i=0; i<Ho; i++) {
	      int S;
	      if (tile==0) S=Out[i*Wo+j];
	      else S = Out[i*Wo+j];
	      //else S=0;
	      V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
	      V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
	      V8 = Line[0]; V9 = Line[1]; V12 = Line[2];
	      Line += (W>>1);
#ifdef PRINTDEB_TILE0
	      printf("id %d j %d i %d S %x\n",CoreId,j,i,S);
	      printf("id %d \tV0 %x %x V1 %x %x V10 %x \n",CoreId,V0[0],V0[1],V1[0],V1[1],V10[0]);
	      printf("id %d \tV2 %x %x V3 %x %x V10 %x \n",CoreId,V2[0],V2[1],V3[0],V3[1],V10[1]);
	      printf("id %d \tV4 %x %x V5 %x %x V11 %x \n",CoreId,V4[0],V4[1],V5[0],V5[1],V11[0]);
	      printf("id %d \tV6 %x %x V7 %x %x V11 %x \n",CoreId,V6[0],V6[1],V7[0],V7[1],V11[1]);
	      printf("id %d \tV8 %x %x V9 %x %x V12 %x \n",CoreId,V8[0],V8[1],V9[0],V9[1],V12[0]);
#endif
	      S = gap8_sumdotp2(V0,  C0,  S); //printf("\t\t id %d S0 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V1,  C1,  S); //printf("\t\t id %d S1 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V2,  C2,  S); //printf("\t\t id %d S2 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V3,  C3,  S); //printf("\t\t id %d S3 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V10, C10, S); //printf("\t\t id %d S10 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V4,  C4,  S); //printf("\t\t id %d S4 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V5,  C5,  S); //printf("\t\t id %d S5 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V6,  C6,  S); //printf("\t\t id %d S6 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V7,  C7,  S); //printf("\t\t id %d S7 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V11, C11, S); //printf("\t\t id %d S11 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V8,  C8,  S); //printf("\t\t id %d S8 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V9,  C9,  S); //printf("\t\t id %d S9 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V12, C12, S); //printf("\t\t id %d S12 %x\n",CoreId,S);
	      // store partial pixel as int
	      Out[i*Wo+j] =  S;
	      // Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
#ifdef PRINTDEB_TILE0
	      printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,gap8_clip((S>>Norm), 15));
#endif
	    }	      
	  }
#ifdef PRINTDEB_TILE0
	  printf("sw tile %d\n",tile);
	  CLUSTER_SynchBarrier(0);
	  if (CoreId==0) for(i=0;i<98-19;i++) for (j=0;j<33;j++) printf("sw1 ==> o %d %d %x\n",i,j,gap8_clip((Out[i*33+j]>>Norm),15));
	  CLUSTER_SynchBarrier(0);
#endif
	  
	}
	  CLUSTER_SynchBarrier(0);

	

	// second pass of 5 x 3x5 kernels
#ifdef PRINTDEB_TILE0
	printf("id %d second pass\n",CoreId);
#endif
	for (tile=0;tile<4;tile++){
	  tile5=40*tile+5;
	  v2s C0  = *((v2s *) &Filter[0 + tile5]),
	    C2  = *((v2s *) &Filter[N + tile5]),
	    C4  = *((v2s *) &Filter[2*N + tile5]),
	    C6  = *((v2s *) &Filter[3*N + tile5]),
	    C8  = *((v2s *) &Filter[4*N + tile5]),
    	    C10 = gap8_pack2(Filter[2+tile5], Filter[2+N+tile5]), C11 = gap8_pack2(Filter[2+2*N+tile5],Filter[2+3*N+tile5]), C12 = gap8_pack2(Filter[2+4*N+tile5],0);

#ifdef PRINTDEB_TILE0
	  printf("id %d tile %d\n",CoreId,tile);
	  printf("id %d C0 %x %x C10 %x\n",CoreId,C0[0],C0[1],C10[0]);
	  printf("id %d C2 %x %x C10 %x\n",CoreId,C2[0],C2[1],C10[1]);
	  printf("id %d C4 %x %x C11 %x\n",CoreId,C4[0],C4[1],C11[0]);
	  printf("id %d C6 %x %x C11 %x\n",CoreId,C6[0],C6[1],C11[1]);
	  printf("id %d C8 %x %x C12 %x\n",CoreId,C8[0],C8[1],C12[0]);
#endif
	  //
	    tile5 = 5 + tile*5*W;
	  for(j=First; j<Last; j++) {
	    int ii, jj;

	    V10 = (v2s) {0, In[j + 2 + tile5]}; V11 = (v2s) {In[j + W + 2 + tile5], In[j + 2*W + 2 + tile5]};
	    Line = (v2s *) (In + j + tile5);
	    V2 = Line[0];  Line += (W>>1);
	    V4 = Line[0];  Line += (W>>1);
	    V6 = Line[0];  Line += (W>>1);
	    V8 = Line[0];  V12 = Line[1]; Line += (W>>1);
	    for(i=0; i<Ho; i++) {
	      int S = Out[i*Wo+j];
	      //int S=0;
	      V0 = V2;  V2 = V4; V4 = V6; V6 = V8;
	      V10 = __builtin_shuffle(V10, V11, Mask);
	      V11 = __builtin_shuffle(V11, V12, Mask);
	      	      V8 = Line[0]; V12 = Line[1];
	      Line += (W>>1);
#ifdef PRINTDEB_TILE0
	      printf("id %d tile %d j %d i %d\n",CoreId,tile,j,i);
	      printf("id %d \tV0 %x  %x V10 %x \n",CoreId,V0[0],V0[1],V10[0]);
	      printf("id %d \tV2 %x  %x V10 %x \n",CoreId,V2[0],V2[1],V10[1]);
	      printf("id %d \tV4 %x  %x V11 %x \n",CoreId,V4[0],V4[1],V11[0]);
	      printf("id %d \tV6 %x  %x V11 %x \n",CoreId,V6[0],V6[1],V11[1]);
	      printf("id %d \tV8 %x  %x V12 %x \n",CoreId,V8[0],V8[1],V12[0]);
#endif
	      S = gap8_sumdotp2(V0,  C0,  S);//printf("\t\t id %d S0 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V2,  C2,  S);//printf("\t\t id %d S2 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V10, C10, S);//printf("\t\t id %d S10 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V4,  C4,  S);//printf("\t\t id %d S4 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V6,  C6,  S);//printf("\t\t id %d S6 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V11, C11, S);//printf("\t\t id %d S11 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V8,  C8,  S);//printf("\t\t id %d S8 %x\n",CoreId,S);
	      S = gap8_sumdotp2(V12, C12, S);//printf("\t\t id %d S12 %x\n",CoreId,S);
// store partial pixel as int
	      Out[i*Wo+j] =  S;
	      // Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
	    }	      //printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
	  }
#ifdef PRINTDEB_TILE0
	  printf("sw tile %d\n",tile);
	  CLUSTER_SynchBarrier(0);
	  if (CoreId==0) for(i=0;i<98-19;i++) for (j=0;j<33;j++) printf("sw1 ==> o %d %d %x\n",i,j,gap8_clip((Out[i*33+j]>>Norm),15));
	  CLUSTER_SynchBarrier(0);
#endif

	}
	CLUSTER_SynchBarrier(0);
	
#ifdef PRINTDEB_TILE
	if (CoreId==0) for(i=0;i<98-19;i++) for (j=0;j<33;j++) printf("sw1 ==> o %d %d %x\n",i,j,gap8_clip((Out[i*33+j]>>Norm),15));
	CLUSTER_SynchBarrier(0);
#endif

}


void __attribute__ ((noinline)) KerDirectConvNxM_tiled_2(KerConv_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	char *Filter = (char *) Arg->Filter;
	unsigned int Norm = Arg->Norm;
	int * __restrict__ Out = (int *) Arg->Out;
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s *Line;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);

	//  pass of 2 x 5x4 kernels
	int tile,tile4;
	for (tile=0;tile<2;tile++){
	  tile4 = tile*20;
	  v2s C0  = (*((v2s *) &Filter[0+tile4]))<< (v2s) {8,8};
	  C0 = C0 >> (v2s) {8,8};
	  v2s C1  = (*((v2s *) &Filter[1+tile4]))<< (v2s) {8,8};
	  C1 = C1 >> (v2s) {8,8};
	  v2s C2  = (*((v2s *) &Filter[4+tile4]))<< (v2s) {8,8};
	  C2 = C2 >> (v2s) {8,8};
	  v2s C3  = (*((v2s *) &Filter[5+tile4]))<< (v2s) {8,8};
	  C3 = C3 >> (v2s) {8,8};

	  v2s C4  = (*((v2s *) &Filter[8+tile4]))<< (v2s) {8,8};
	  C4 = C4 >> (v2s) {8,8};
	  v2s C5  = (*((v2s *) &Filter[9+tile4]))<< (v2s) {8,8};
	  C5 = C5 >> (v2s) {8,8};
	  v2s C6  = (*((v2s *) &Filter[12+tile4]))<< (v2s) {8,8};
	  C6 = C6 >> (v2s) {8,8};
	  v2s C7  = (*((v2s *) &Filter[13+tile4]))<< (v2s) {8,8};
	  C7 = C7 >> (v2s) {8,8};
	  v2s C8  = (*((v2s *) &Filter[16+tile4]))<< (v2s) {8,8};
	  C8 = C8 >> (v2s) {8,8};
	  v2s C9  = (*((v2s *) &Filter[17+tile4]))<< (v2s) {8,8};
	  C9 = C9 >> (v2s) {8,8};

#ifdef PRINTDEB_TILE20
	  printf("id %d tile %d\n",CoreId,tile);
	  printf("id %d C0 %x %x C1 %x %x \n",CoreId,C0[0],C0[1],C1[0],C1[1]);
	  printf("id %d C2 %x %x C3 %x %x \n",CoreId,C2[0],C2[1],C3[0],C3[1]);
	  printf("id %d C4 %x %x C5 %x %x \n",CoreId,C4[0],C4[1],C5[0],C5[1]);
	  printf("id %d C6 %x %x C7 %x %x \n",CoreId,C6[0],C6[1],C7[0],C7[1]);
	  printf("id %d C8 %x %x C9 %x %x \n",CoreId,C8[0],C8[1],C9[0],C9[1]);
#endif

	  for(j=First; j<Last; j++) {
	    int ii, jj;

	    Line = (v2s *) (In + j + 5*tile*W);
	    V2 = Line[0]; V3 = Line[1];  Line += (W>>1);
	    V4 = Line[0]; V5 = Line[1];  Line += (W>>1);
	    V6 = Line[0]; V7 = Line[1];  Line += (W>>1);
	    V8 = Line[0]; V9 = Line[1];  Line += (W>>1);

	    for(i=0; i<Ho; i++) {
	      int S;
	      if (tile==0) S=Out[i*Wo+j];
	      else S = Out[i*Wo+j];

	      V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
	      V8 = Line[0]; V9 = Line[1];
	      Line += (W>>1);

#ifdef PRINTDEB_TILE20
	      printf("id %d tile %d j %d i %d S %x\n",CoreId,tile,j,i,S);
	      printf("id %d \tV0 %x %x V1 %x %x\n",CoreId,V0[0],V0[1],V1[0],V1[1]);
	      printf("id %d \tV2 %x %x V3 %x %x\n",CoreId,V2[0],V2[1],V3[0],V3[1]);
	      printf("id %d \tV4 %x %x V5 %x %x\n",CoreId,V4[0],V4[1],V5[0],V5[1]);
	      printf("id %d \tV6 %x %x V7 %x %x\n",CoreId,V6[0],V6[1],V7[0],V7[1]);
	      printf("id %d \tV8 %x %x V9 %x %x\n",CoreId,V8[0],V8[1],V9[0],V9[1]);
#endif

	      S = gap8_sumdotp2(V0,  C0,  S);
	      //printf("id %d j %d i %d S0 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V1,  C1,  S);
	      //printf("id %d j %d i %d S1 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V2,  C2,  S);
	      //printf("id %d j %d i %d S2 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V3,  C3,  S);
	      //printf("id %d j %d i %d S3 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V4,  C4,  S);
	      //printf("id %d j %d i %d S4 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V5,  C5,  S);
	      //printf("id %d j %d i %d S5 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V6,  C6,  S);
	      //printf("id %d j %d i %d S6 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V7,  C7,  S);
	      //printf("id %d j %d i %d S7 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V8,  C8,  S);
	      //printf("id %d j %d i %d S8 %x\n",CoreId,j,i,S);
	      S = gap8_sumdotp2(V9,  C9,  S);
	      //printf("id %d j %d i %d S9 %x\n",CoreId,j,i,S);
	      // store partial pixel as int
	      Out[i*Wo+j] =  S;
	      //Out[i*Wo+j] =  gap8_clip((S>>Norm), 15);
	    }	      //printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
	  }
#ifdef PRINTDEB_TILE2
	  printf("sw tile %d\n",tile);
	  CLUSTER_SynchBarrier(0);
	  if (CoreId==0) for(i=0;i<39-9;i++) for (j=0;j<13;j++) printf("sw1 ==> o %d %d %x\n",i,j,gap8_clip((Out[i*13+j]>>Norm),15));
	  CLUSTER_SynchBarrier(0);
#endif
	}

	CLUSTER_SynchBarrier(0);

#ifdef PRINTDEB_TILE2
	if (CoreId==0) for(i=0;i<39-9;i++) for (j=0;j<13;j++) printf("sw1 ==> o %d %d %x\n",i,j,gap8_clip((Out[i*13+j]>>Norm),15));
	CLUSTER_SynchBarrier(0);
#endif

}



/*
	NxM convolution, short int inputs and output (N is line, M is column)
	The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
	In:	short int *, convolution input
	W, H:	Input dimension [W x H]
	Filter:	short int *, pointer to the NxN convolution coefficients
	Norm:	Fixed point format
*/


void __attribute__ ((noinline)) KerDirectConvNxM_char(KerConv_fpsT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	char *Filter = (char *)Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);
        // for(j=0; j<Wo; j++) {
        for(i=0; i<Ho; i++) {
        	for(j=First; j<Last; j++) {
                        int ii, jj;
                        int Acc =  Out[i*Wo+j]<<Norm;
                        for (ii=0; ii<M; ii++) {
				char *Coeff =  (Filter + ii*N);
				v2s *Line = (v2s *) (In + (i+ii)*W + j);
				for (jj=0; jj<(N/2); jj++) {
				  /// !! jj should be +3 every 2 steps : this work only for N=4
				  v2s Coeffv2s = (*((v2s*) (Coeff+jj))) << (v2s) {8,8};
				  Coeffv2s = Coeffv2s >>  (v2s) {8,8};
#ifdef PRINTDEB_TILE2
				  printf("lin: core %d i %d j %d ii %d jj %d coeff %x %x in %x %x acc %x\n",CoreId,i,j,ii,jj,Coeffv2s[0],Coeffv2s[1], Line[jj][0],Line[jj][1], Acc);
#endif
				  Acc = gap8_sumdotp2(Coeffv2s, Line[jj], Acc);
				  //if (i==0) printf("\t\t core %d i %d j %d ii %d jj %d coeff %x %x | %x %x in %x %x acc %x\n",CoreId,i,j,ii,jj,Coeff[jj],Coeff[jj+2],Coeffv2s[0],Coeffv2s[1], Line[jj][0],Line[jj][1], Acc);
				}
				// N and M are even =>  aligned with v2s: no last addition
				//Acc += Filter[ii*N+(N-1)]*In[(i+ii)*W + j + (N-1)];
                                // for (jj=0; jj<N; jj++) Acc += Filter[ii*N+jj]*In[(i+ii)*W + j + jj];
                        }
			//printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
			Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                }
        }
	CLUSTER_SynchBarrier(0);
}
void __attribute__ ((noinline)) KerDirectConvNxM_char_model(KerConv_fpsT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	char *Filter = (char *)Arg->Filter;
	unsigned int Norm = Arg->Norm;
	Word16 * __restrict__ Out = Arg->Out;

        int Wo = W-N+1;
        int Ho = H-M+1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(Wo);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, Wo);

        int i;
	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);
        // for(j=0; j<Wo; j++) {
        for(i=0; i<Ho; i++) {
        	for(j=First; j<Last; j++) {
                        int ii, jj;
                        int Acc =  Out[i*Wo+j]<<Norm;
#ifdef PRINTDEB_TILE
			printf("id %d i %d j %d Acc0 %x\n",CoreId,i,j,Acc);
#endif
                        for (ii=0; ii<M; ii++) {
				char *Coeff =  (Filter + ii*N);
				char coeffchar[4];
				coeffchar[0]= Coeff[0];
				coeffchar[1]= Coeff[2];
				coeffchar[2]= Coeff[1];
				coeffchar[3]= Coeff[3];
				short int *Line = (short int *) (In + (i+ii)*W + j);
				for (jj=0; jj<N; jj++) {
				  /// !! jj should be +3 every 2 steps : this work only for N=4


				  Acc = Line[jj]*coeffchar[jj] + Acc;

#ifdef PRINTDEB_TILE
				  printf("nxm(2): core %d i %d j %d ii %d jj %d coeff %x in %x acc %x\n",CoreId,i,j,ii,jj,coeffchar[jj], Line[jj], Acc);
#endif

				  //if (i==0) printf("\t\t core %d i %d j %d ii %d jj %d coeff %x %x | %x %x in %x %x acc %x\n",CoreId,i,j,ii,jj,Coeff[jj],Coeff[jj+2],Coeffv2s[0],Coeffv2s[1], Line[jj][0],Line[jj][1], Acc);
				}
				// N and M are even =>  aligned with v2s: no last addition
				//Acc += Filter[ii*N+(N-1)]*In[(i+ii)*W + j + (N-1)];
                                // for (jj=0; jj<N; jj++) Acc += Filter[ii*N+jj]*In[(i+ii)*W + j + jj];
                        }
			//printf("ACC: core %d i %d j %d acc %x\n",CoreId,i,j,Acc);
			Out[i*Wo+j] =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                }
        }
	CLUSTER_SynchBarrier(0);
}

/*
	Linear rectification
	In:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out:	short int *, Output data [W, H]
*/
void __attribute__ ((noinline)) KerReLU_fp(KerReLUMaxPool2x2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((W*H)/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W*H)/2);
	v2s * VectIn  = (v2s *) In;
	v2s * VectOut = (v2s *) Out;
	unsigned int i;
	unsigned int j;
	for (i=First; i<Last; i++) VectOut[i] = gap8_max2(VectIn[i], ((v2s) {0, 0}));
	if ((W*H)&0x1) Out[W*H-1] = Max(In[W*H-1], 0);
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	2x2 max pooling
	In:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerMaxPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned int j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
		v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M = gap8_max2(Line1[j], Line2[j]);
                        Out[(W/2)*i+j] = Max(M[0], M[1]);
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	2x2 Average pooling
	In:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerAvgPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned int j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
		v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			int S = gap8_dotp2(Line1[j], ((v2s) {1,1}));
                        Out[(W/2)*i+j] = gap8_sumdotp2(Line2[j], ((v2s) {1,1}), S)>>2;
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 max pooling, single output version
	In:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUMaxPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	Word16 * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
		v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M = gap8_max2(Line1[j], Line2[j]);
			M = gap8_max2(M, ((v2s) {0, 0}));
                        Out[(W/2)*i+j] = Max(M[0], M[1]);
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}



/*
	Linear rectification followed by a 2x2 max pooling, double output version
	In0:	short int *, Input data
	In1:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUMaxPool2x2_2_fp(KerReLUMaxPool2x2_2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M0 = gap8_max2(Line0_1[j], Line1_1[j]);
			v2s M1 = gap8_max2(Line0_2[j], Line1_2[j]);
			M0 = gap8_max2(M0, ((v2s) {0, 0}));
			M1 = gap8_max2(M1, ((v2s) {0, 0}));
                        Out0[(W/2)*i+j] = Max(M0[0], M0[1]);
                        Out1[(W/2)*i+j] = Max(M1[0], M1[1]);
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 average pooling, double output version
	In0:	short int *, Input data
	In1:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUAvgPool2x2_2_fp(KerReLUMaxPool2x2_2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			int S0 = gap8_dotp2(Line0_1[j], ((v2s) {1,1}));
			int S1 = gap8_dotp2(Line0_2[j], ((v2s) {1,1}));
                        Out0[(W/2)*i+j] = gap8_sumdotp2(Line1_1[j], ((v2s) {1,1}), S0)>>2;
                        Out1[(W/2)*i+j] = gap8_sumdotp2(Line1_2[j], ((v2s) {1,1}), S1)>>2;
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 max pooling, double output version, pad with an extra 0 at end of line in case W is odd
	In0:	short int *, Input data
	In1:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUMaxPool2x2_2_Even_fp(KerReLUMaxPool2x2_2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	int Wout = W/2 + (W&0x1);

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M0 = gap8_max2(Line0_1[j], Line1_1[j]);
			v2s M1 = gap8_max2(Line0_2[j], Line1_2[j]);
			M0 = gap8_max2(M0, ((v2s) {0, 0}));
			M1 = gap8_max2(M1, ((v2s) {0, 0}));
                        Out0[Wout*i+j] = Max(M0[0], M0[1]);
                        Out1[Wout*i+j] = Max(M1[0], M1[1]);
		}
		if (W&0x1) {
			Out0[Wout*i + Wout - 1] = 0;
			Out1[Wout*i + Wout - 1] = 0;
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 max pooling, double output version, drops one output at the end of each line, this is the counter
	part of KerReLUMaxPool2x2_2_Even_fp where we pad with one extra 0
	In0:	short int *, Input data
	In1:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUMaxPool2x2_2_Drop_fp(KerReLUMaxPool2x2_2_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	int Wout = (W/2)-1;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven((W/2)-1);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, (W/2)-1);
	int i;
	unsigned j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M0 = gap8_max2(Line0_1[j], Line1_1[j]);
			v2s M1 = gap8_max2(Line0_2[j], Line1_2[j]);
			M0 = gap8_max2(M0, ((v2s) {0, 0}));
			M1 = gap8_max2(M1, ((v2s) {0, 0}));
                        Out0[Wout*i+j] = Max(M0[0], M0[1]);
                        Out1[Wout*i+j] = Max(M1[0], M1[1]);
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 max pooling, triple output version
	In0:	short int *, Input data
	In1:	short int *, Input data
	In2:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
	Out2:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUMaxPool2x2_3_fp(KerReLUMaxPool2x2_3_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	Word16 * __restrict__ In2 = Arg->In2;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	Word16 * __restrict__ Out2 = Arg->Out2;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned int j;
	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line0_3 = (v2s *) (&In2[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		v2s * __restrict__ Line1_3 = (v2s *) (&In2[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			v2s M0 = gap8_max2(Line0_1[j], Line1_1[j]);
			v2s M1 = gap8_max2(Line0_2[j], Line1_2[j]);
			v2s M2 = gap8_max2(Line0_3[j], Line1_3[j]);
			M0 = gap8_max2(M0, ((v2s) {0, 0}));
			M1 = gap8_max2(M1, ((v2s) {0, 0}));
			M2 = gap8_max2(M2, ((v2s) {0, 0}));
                        Out0[(W/2)*i+j] = Max(M0[0], M0[1]);
                        Out1[(W/2)*i+j] = Max(M1[0], M1[1]);
                        Out2[(W/2)*i+j] = Max(M2[0], M2[1]);
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear rectification followed by a 2x2 averga pooling, triple output version
	In0:	short int *, Input data
	In1:	short int *, Input data
	In2:	short int *, Input data
	W, H:	Input data dimension [W x H]
	Out0:	short int *, Output data [W/2, H/2]
	Out1:	short int *, Output data [W/2, H/2]
	Out2:	short int *, Output data [W/2, H/2]
*/
void __attribute__ ((noinline)) KerReLUAvgPool2x2_3_fp(KerReLUMaxPool2x2_3_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In0 = Arg->In0;
	Word16 * __restrict__ In1 = Arg->In1;
	Word16 * __restrict__ In2 = Arg->In2;
	int W = Arg->W;
	int H = Arg->H/2;
	Word16 * __restrict__ Out0 = Arg->Out0;
	Word16 * __restrict__ Out1 = Arg->Out1;
	Word16 * __restrict__ Out2 = Arg->Out2;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(W/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, W/2);
	int i;
	unsigned int j;

	for (i=0; i<(H/2); i++) {
		v2s * __restrict__ Line0_1 = (v2s *) (&In0[(2*i  )*W]);
		v2s * __restrict__ Line0_2 = (v2s *) (&In1[(2*i  )*W]);
		v2s * __restrict__ Line0_3 = (v2s *) (&In2[(2*i  )*W]);
		v2s * __restrict__ Line1_1 = (v2s *) (&In0[(2*i+1)*W]);
		v2s * __restrict__ Line1_2 = (v2s *) (&In1[(2*i+1)*W]);
		v2s * __restrict__ Line1_3 = (v2s *) (&In2[(2*i+1)*W]);
		// for (j=0; j<(W/2); j++) {
		for (j=First; j<Last; j++) {
			int S0 = gap8_dotp2(Line0_1[j], ((v2s) {1,1}));
			int S1 = gap8_dotp2(Line0_2[j], ((v2s) {1,1}));
			int S2 = gap8_dotp2(Line0_3[j], ((v2s) {1,1}));
                        Out0[(W/2)*i+j] = gap8_sumdotp2(Line1_1[j], ((v2s) {1,1}), S0)>>2;
                        Out1[(W/2)*i+j] = gap8_sumdotp2(Line1_2[j], ((v2s) {1,1}), S1)>>2;
                        Out2[(W/2)*i+j] = gap8_sumdotp2(Line1_3[j], ((v2s) {1,1}), S2)>>2;
		}
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear layer:	Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
	Outputs are evaluated in sequence, a given output is evaluated in parallel

	In	short int, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	short int, output vector
	OutSize	output dimension
*/

void __attribute__ ((noinline)) KerLinearLayer_fp(KerLinearLayer_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word16 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word16 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned int j;

	v2s * __restrict__ VectIn = (v2s *) In;

       	// Linear combination
	for (i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize]);
		int Acc = 0;
		// for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(In[j], Filt[j], Acc); j++;
		for (j = First; j<Last; j++) {
		  Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
#ifdef DUMP_DENSE
		  printf("lin: core %d j %d %4x * (%4x,%4x) Acc %x\n",CoreId,j,VectIn[j][0], Filt[j][0], Filt[j][1],Acc);
#endif
		}
		j++;
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			printf("bias %x\n",Acc);
			if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++) {
			  Acc += Reduct[j];
#ifdef DUMP_DENSE
			  printf("reduc: %d %x acc %x\n",j,Reduct[j],Acc);
#endif
			}
			Out[i] = gap8_clip(gap8_roundnorm_reg(Acc, NormFilter), 15);
			//printf("reducOut %x\n",Out[i]);
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
}

/*
	Linear layer:	Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
	Outputs are evaluated in parallel, a given output is evaluated in sequence

	In	short int, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	short int, output vector
	OutSize	output dimension
*/

void __attribute__ ((noinline)) KerLinearLayerParOut_fp(KerLinearLayer_fpT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word16 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word16 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(OutSize);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, OutSize);
        unsigned int i;
	int j;
	v2s * __restrict__ VectIn = (v2s *) In;

       	// Linear combination
	// for (i=0; i<OutSize; i++) {
	for (i=First; i<Last; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize]);
		int Acc = 0;
		for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
		if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
		Acc += (Bias[i]<<NormBias);
		Out[i] = gap8_clip(gap8_roundnorm_reg(Acc, NormFilter), 15);
	}
	CLUSTER_SynchBarrier(0);
}

/*
	Linear layer:	Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
	Outputs are evaluated in sequence, a given output is evaluated in parallel

	In	short int, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	int, output vector
	OutSize	output dimension
*/
void __attribute__ ((noinline)) KerLinearLayer_fpd(KerLinearLayer_fpdT *Arg)

{
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word16 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word32 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned int j;

	v2s * __restrict__ VectIn = (v2s *) In;

       	// Linear combination
	for (i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize]);
		int Acc = 0;
		// for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(In[j], Filt[j], Acc); j++;
		for (j = First; j<Last; j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
    j++;
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++) Acc += Reduct[j];
			Out[i] = gap8_roundnorm_reg(Acc, NormFilter);
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
}

/*
	Linear layer:	Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
	Outputs are evaluated in sequence, a given output is evaluated in parallel

	In	Byte, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	short int, output vector
	OutSize	output dimension
*/
void __attribute__ ((noinline)) KerLinearLayer_fps(KerLinearLayer_fpsT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word8 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word16 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned int j;

	v2s * __restrict__ VectIn = (v2s *) In;
       	// Linear combination
	if (0) for (i=0;i<12480;i++) {
	  if (!(i%30)) printf("%d ",i);
	  printf(" %d", In[i]);
	  if (!((i+1)%30)) printf("\n");
	}

	for (i=0; i<OutSize; i++) {
		const Word8 * __restrict__ Filt1 = &Filter[InSize*i+2*First];
		const Word8 * __restrict__ Filt2 = Filt1+1; // &Filter[InSize*i+1+2*First];
		int Acc = 0;
		// for (j=0; j<(InSize/2); j++) {
		for (j=First; j<Last; j++) {
			int F1 = *Filt1; Filt1 += 2;
			int F2 = *Filt2; Filt2 += 2;
			v2s F = gap8_pack2(F1, F2);
			// Acc = gap8_sumdotp2(In[j], (F<<(v2s){8, 8}), Acc);
			Acc = gap8_sumdotp2(VectIn[j], F, Acc);
#ifdef DUMP_DENSE
			printf("lin1: core %d i %d j %d coeff %x %x in %x %x acc %x\n",CoreId,i,j,  F1,F2, VectIn[j][0],VectIn[j][1], Acc);
#endif
		}
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			// if (InSize%2) Acc += In[InSize-1]* (*Filt1);
			if (InSize%2) Acc += In[InSize-1]* Filter[InSize*i + InSize-1]; // [InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++){
			  Acc += Reduct[j];
#ifdef DUMP_REDUC
			  printf("reduc: %d %x acc %x\n",j,Reduct[j],Acc);
#endif
			}
			Out[i] = gap8_clip(gap8_roundnorm_reg(Acc, NormFilter), 15);
#ifdef DUMP_REDUC
			printf("out: %d %x out %x\n",i,Acc,Out[i]);
#endif
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear layer:	Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
	Outputs are evaluated in sCnnequence, a given output is evaluated in parallel

	In	Byte, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	short int, output vector
	OutSize	output dimension
*/
void __attribute__ ((noinline)) KerLinearLayer_halfchar(KerLinearLayer_fpsT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word8 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word16 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned int j,k;

	v2s * __restrict__ VectIn =  (v2s*) In;
	
	//printf("chuncksize %d\n",ChunkCell);
	if (0) {
	  if (CoreId==0){
	    for (i=0;i<12480;i++) {
	      if (!(i%13)) printf("%d ",i);
	      printf(" %x", In[i]);
	      if (!((i+1)%13)) printf("\n");
	    }
	  }
	  CLUSTER_SynchBarrier(0);
	}
	
	//coeff are v2s {(0,2,4,6), (1,3,5,7)}
       	// Linear combination
	for (i=0; i<OutSize; i++) {
	  // for one output, filter size is input size (scalar product)
	  const Word8 * __restrict__ Filt1 = &Filter[(i*InSize)>>1];
		int Acc = 0;
		// for (j=0; j<(InSize/2); j++) {
		for (j=First; j<Last; j++) {
		  signed char F1 = *(Filt1+j);
		  F1= ((signed char) (F1<<4))>>4;
		  signed char F2 = ((*(Filt1+j))>>4) ;
		  v2s F = gap8_pack2((int)F1, (int)F2);
		  Acc = gap8_sumdotp2(VectIn[j], F , Acc);

		  
#ifdef DUMP_DENSE
		  printf("lin1: core %d i %d j %d coeff %x %x in %x %x acc %x\n",CoreId,i,j, F1,F2, VectIn[j][0],VectIn[j][1], Acc);
#endif
		}
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			//printf("bias %x\n",Acc);
			// if (InSize%2) Acc += In[InSize-1]* (*Filt1);
			if (InSize%2) Acc += In[InSize-1]* Filter[InSize*i + InSize-1]; // [InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++) {
			  Acc += Reduct[j];
#ifdef DUMP_REDUC
			  printf("reduc: %d %x acc %x\n",j,Reduct[j],Acc);
#endif
			}
			Out[i] = gap8_clip(gap8_roundnorm_reg(Acc, NormFilter), 15);
#ifdef DUMP_REDUC
			printf("out: %d %x out %x\n",i,Acc,Out[i]);
#endif
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}


/*
	Linear layer:	Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
	Outputs are evaluated in sequence, a given output is evaluated in parallel

	In	short int, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	short int, output vector
	OutSize	output dimension
*/

void __attribute__ ((noinline)) KerLinearLayerReLU_fp(KerLinearLayer_fpT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word16 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word16 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned j;

	v2s * __restrict__ VectIn = (v2s *) In;

       	// Linear combination
	for (i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize]);
		int Acc = 0;
		// for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(In[j], Filt[j], Acc); j++;
		for (j = First; j<Last; j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
    j++;
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++) Acc += Reduct[j];
			Out[i] = Max(gap8_clip(gap8_roundnorm_reg(Acc, NormFilter), 15), 0);
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

/*
	Linear layer:	Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
	Outputs are evaluated in sequence, a given output is evaluated in parallel

	In	short int, input vector
	InSize	Number of inputs
	Filter	short int, Linear filter coefficients. Dimension: InSize*OutSize
	Out	int, output vector
	OutSize	output dimension
*/
void __attribute__ ((noinline)) KerLinearLayerReLU_fpd(KerLinearLayer_fpdT *Arg)

{
#define Max(a, b) (((a)>(b))?(a):(b))
	Word16 * __restrict__ In = Arg->In;
	int InSize = Arg->InSize;
	const Word16 * __restrict__ Filter = Arg->Filter;
	unsigned int NormFilter = Arg->NormFilter;
	const Word16 * __restrict__ Bias = Arg->Bias;
	unsigned int NormBias = Arg->NormBias;
	Word32 * __restrict__ Out = Arg->Out;
	int OutSize = Arg->OutSize;
	static L1_CL_MEM int Reduct[8];
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSizeEven(InSize/2);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Minu(First+ChunkCell, InSize/2);
        int i;
	unsigned int j;

	v2s * __restrict__ VectIn = (v2s *) In;

       	// Linear combination
	for (i=0; i<OutSize; i++) {
		v2s * __restrict__ Filt = (v2s *) (&Filter[i*InSize]);
		int Acc = 0;
		// for (j = 0; j<(InSize/2); j++) Acc = gap8_sumdotp2(In[j], Filt[j], Acc); j++;
		for (j = First; j<Last; j++) Acc = gap8_sumdotp2(VectIn[j], Filt[j], Acc);
    j++;
		Reduct[CoreId] = Acc;
		CLUSTER_SynchBarrier(0);
		if (CoreId==0) {
			Acc = (Bias[i]<<NormBias);
			if (InSize%2) Acc += In[InSize-1]*Filter[i*InSize+InSize-1];
			for (j=0;j<(unsigned int)__Ncore();j++) Acc += Reduct[j];
			Out[i] = gap8_roundnorm_reg(Acc, NormFilter);
			Out[i] = Max(gap8_roundnorm_reg(Acc, NormFilter), 0);
		}
		CLUSTER_SynchBarrier(0);
	}
	CLUSTER_SynchBarrier(0);
#undef Max
}

