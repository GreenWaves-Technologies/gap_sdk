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

#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels_fp16.h"
#include "FastFloatApprox16.h"

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

/*
static F16 Fast_Tanh_fp16(F16 x)

{
        return (((F16)2.0 * x) / ((F16) 1.0f + AbsF((F16)2.0 * x)));
}
*/
void KerParSoftMax_fp16(KerSoftMax_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	F16 * __restrict__ Out = Arg->Out;
	int N = Arg->N;
	int Feat = Arg->Feat;
	static L1_CL_MEM F16 Reduct[8];
	F16 M, Sum, InvSum;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(N);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, N);
	F16 *Red = &Reduct[CoreId];

	for (int f=0; f<Feat; f++) {
		/* Turns In into distribution */
		/* Find max */
		M = -MIN_FLT16;
		for (int i=First; i<Last; i++) M = MaxF(M, In[i]);
		Reduct[CoreId] = M;
		gap_waitbarrier(0);
		if (CoreId==0) {
			M = Reduct[0];
			for (int i=1; i<gap_ncore(); i++) M = MaxF(M, Reduct[i]);
			Reduct[0] = M;

		}
		gap_waitbarrier(0);
		/* Computes Exp(In[i]-M) for all in and sum results.
		   Since we substract max from In[i] we always have exp(X) with X<=0 thus exp(X)<=1.0
		   By definition of softmax Sum is <= 1
		*/
		M = Reduct[0];
		Sum = 0.0;
		for (int i=First; i<Last; i++) {
			F16 Exp = FastExpF16(In[i] - M);
			Out[i] = Exp; Sum += Exp;
		}
		Reduct[CoreId] = Sum;
		gap_waitbarrier(0);
		if (CoreId==0) {
			Sum = 0.0;
			for (int i=0; i<gap_ncore(); i++) Sum += Reduct[i];
			Reduct[0] = Sum;
		}
		gap_waitbarrier(0);
		Sum = Reduct[0];
		InvSum = (F16) 1.0/Sum;
		for (int i=First; i<Last; i++) Out[i] = Out[i]*InvSum;
		gap_waitbarrier(0);
	}

}
/*
void KerParTanh_fp16(KerSoftMax_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	F16 * __restrict__ Out = Arg->Out;
	int N = Arg->N;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(N);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, N);

	for (int i=First; i<Last; i++) {
		Out[i] = Fast_Tanh_fp16(In[i]);
	}
}
*/
