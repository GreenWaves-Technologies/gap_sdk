/*
 * Copyright 2021 GreenWaves Technologies, SAS
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

#include "nntool_extra_kernels.h"

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

void CNN_NormRGB565_offset_fps(KerNormRGB565_fps_T *Arg)

{
	unsigned short *__restrict__ In = Arg->In;
	signed char *__restrict__ Out0 = Arg->Out0;
	signed char *__restrict__ Out1 = Arg->Out1;
	signed char *__restrict__ Out2 = Arg->Out2;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H), First = Chunk*CoreId, Last = Min(First+Chunk, H);
	for (int h=First; h<Last; h++) {
		for (int w=0; w<((W/4)*4); w+=4) {
			int idx = h*W + w;
			int A0 = In[idx], A1 = In[idx+1], A2 = In[idx+2], A3 = In[idx+3];
			v4s *V0 = (v4s *) &Out0[idx], *V1 = (v4s *) &Out1[idx], *V2 = (v4s *) &Out2[idx];
			*V0 = (v4s) gap_pack4(gap_bitextractu(A0, 5, 11), gap_bitextractu(A1, 5, 11), gap_bitextractu(A2, 5, 11), gap_bitextractu(A3, 5, 11));
			*V1 = (v4s) gap_pack4(gap_bitextractu(A0, 6, 5), gap_bitextractu(A1, 6, 5), gap_bitextractu(A2, 6, 5), gap_bitextractu(A3, 6, 5));
			*V2 = (v4s) gap_pack4(gap_bitextractu(A0, 5, 0), gap_bitextractu(A1, 5, 0), gap_bitextractu(A2, 5, 0), gap_bitextractu(A3, 5, 0));
			*V0 -= 16; *V1 -= 32; *V2 -= 16;
			*V0 = *V0 << 3; *V1 = *V1 << 2; *V2 = *V2 << 3;
		}
		int Left = W&0x3;
		for (int i=W-Left; i<W; i++) {
			int V = In[h*W + i];
			Out0[h*W + i] = (((int)gap_bitextractu(V, 5, 11)) - 16) << 3;
			Out1[h*W + i] = (((int)gap_bitextractu(V, 6, 5)) - 32) << 2;
			Out2[h*W + i] = (((int)gap_bitextractu(V, 5, 0)) - 16) << 3;
		}
	}
	gap_waitbarrier(0);
}

void CNN_NormRGB565_shift_fps(KerNormRGB565_fps_T *Arg)

{
	unsigned short *__restrict__ In = Arg->In;
	signed char *__restrict__ Out0 = Arg->Out0;
	signed char *__restrict__ Out1 = Arg->Out1;
	signed char *__restrict__ Out2 = Arg->Out2;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(H), First = Chunk*CoreId, Last = Min(First+Chunk, H);

	for (int h=First; h<Last; h++) {
		for (int w=0; w<((W/4)*4); w+=4) {
			int idx = h*W + w;
			int A0 = In[idx], A1 = In[idx+1], A2 = In[idx+2], A3 = In[idx+3];
			v4s *V0 = (v4s *) &Out0[idx], *V1 = (v4s *) &Out1[idx], *V2 = (v4s *) &Out2[idx];
			*V0 = (v4s) gap_pack4(gap_bitextractu(A0, 5, 11), gap_bitextractu(A1, 5, 11), gap_bitextractu(A2, 5, 11), gap_bitextractu(A3, 5, 11));
			*V1 = (v4s) gap_pack4(gap_bitextractu(A0, 6, 5), gap_bitextractu(A1, 6, 5), gap_bitextractu(A2, 6, 5), gap_bitextractu(A3, 6, 5));
			*V2 = (v4s) gap_pack4(gap_bitextractu(A0, 5, 0), gap_bitextractu(A1, 5, 0), gap_bitextractu(A2, 5, 0), gap_bitextractu(A3, 5, 0));
			*V0 = *V0 << 2; *V1 = *V1 << 1; *V2 = *V2 << 2;
		}
		int Left = W&0x3;
		for (int i=W-Left; i<W; i++) {
			int V = In[h*W + i];
			Out0[h*W + i] = gap_bitextractu(V, 5, 11) << 2;
			Out1[h*W + i] = gap_bitextractu(V, 6, 5) << 1;
			Out2[h*W + i] = gap_bitextractu(V, 5, 0) << 2;
		}
	}
	gap_waitbarrier(0);
}

void CNN_NormRGB888_shift_fps(KerNormRGB888_fps_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out0 = Arg->Out0;
	signed char *__restrict__ Out1 = Arg->Out1;
	signed char *__restrict__ Out2 = Arg->Out2;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	unsigned int Diff = Last-First;
	for (int OutIdx=First; OutIdx<(First+((Diff*4)/4)); OutIdx+=4) {
		int InIdx0 = OutIdx * 3, InIdx1 = InIdx0 + 6;
		*((v4u *)&Out0[OutIdx]) = __builtin_shuffle(*((v4u *) &In[InIdx0++]), *((v4u *) &In[InIdx1++]), (v4u) {0, 3, 4, 7}) >> 1;
		*((v4u *)&Out1[OutIdx]) = __builtin_shuffle(*((v4u *) &In[InIdx0++]), *((v4u *) &In[InIdx1++]), (v4u) {0, 3, 4, 7}) >> 1;
		*((v4u *)&Out2[OutIdx]) = __builtin_shuffle(*((v4u *) &In[InIdx0++]), *((v4u *) &In[InIdx1++]), (v4u) {0, 3, 4, 7}) >> 1;
	}
	int Left = Diff&0x3;
	for (int i=Last-Left, InIdx=i*3; i<Last; i++) {
		Out0[i] = In[InIdx++] >> 1;
		Out1[i] = In[InIdx++] >> 1;
		Out2[i] = In[InIdx++] >> 1;
	}
	gap_waitbarrier(0);
}

void CNN_NormRGB888_offset_fps(KerNormRGB888_fps_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out0 = Arg->Out0;
	signed char *__restrict__ Out1 = Arg->Out1;
	signed char *__restrict__ Out2 = Arg->Out2;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	unsigned int InIdx = First * 3;
	for (int OutIdx=First; OutIdx<Last; OutIdx++) {
		Out0[OutIdx] = In[InIdx++] - 128;
		Out1[OutIdx] = In[InIdx++] - 128;
		Out2[OutIdx] = In[InIdx++] - 128;
	}
	gap_waitbarrier(0);
}

void CNN_NormRGB16_fp(KerNormRGB16_fp_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed short int *__restrict__ Out0 = Arg->Out0;
	signed short int *__restrict__ Out1 = Arg->Out1;
	signed short int *__restrict__ Out2 = Arg->Out2;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	unsigned int InIdx = First * 3;
	for (int OutIdx=First; OutIdx<Last; OutIdx++) {
		Out0[OutIdx] = In[InIdx++] << 7;
		Out1[OutIdx] = In[InIdx++] << 7;
		Out2[OutIdx] = In[InIdx++] << 7;
	}
	gap_waitbarrier(0);
}

void CNN_NormBW_shift_fps(KerNormBW_fps_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	unsigned int Diff = Last-First;
	for (int Idx=First; Idx<First+(Diff*4)/4; Idx+=4) {
		*((v4u *) &Out[Idx]) = *((v4u *)&In[Idx]) >> 1;
	}
	int Left = Diff&0x3;
	for (int i=Last-Left; i<Last; i++) {
		Out[i]  = In[i] >> 1;
	}
	gap_waitbarrier(0);
}

void CNN_NormBW_offset_fps(KerNormBW_fps_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	for (int Idx=First; Idx<Last; Idx++) {
		Out[Idx] = In[Idx] - 128;
	}
	gap_waitbarrier(0);
}

void CNN_NormBW_fp(KerNormBW_fp_T *Arg)
{
	unsigned char *__restrict__ In = Arg->In;
	signed short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Sz = W * H;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Sz), First = Chunk*CoreId, Last = Min(First+Chunk, Sz);

	unsigned int Diff = Last-First;
	for (int Idx=First; Idx<First+Diff; Idx++){
		Out[Idx] = In[Idx] << 7;
	}
	gap_waitbarrier(0);
}

static void Int8toUint8(signed char *__restrict__ In, signed char *__restrict__ Out, unsigned char Offset, unsigned int N)
{
	v4u *pIn = (v4u *) In, *pOut = (v4u *) Out;
	v4u Off  = (v4u) {Offset, Offset, Offset, Offset};
	v4u Off1 = (v4u) {Offset, Offset, 0, 0};

	for (int i=0; i<N/8; i++) {
		v4u V0 = pIn[2*i], V1 = pIn[2*i+1];
		V0 = V0 + Off; V1 = V1 + Off;
		pOut[2*i] = V0; pOut[2*i+1] = V1;
	}
	for (int i=(N-N%8); i<=N; i++) {
		((unsigned char *)Out)[i] = ((unsigned char *)In)[i] + Offset;
	}
	gap_waitbarrier(0);
}

void CNN_FpsFpu(CNN_FpsFpu_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	Int8toUint8((Arg->In + First), (signed char *)(Arg->Out + First), (unsigned char) Arg->Infos[0], Iter);
}

void CNN_FpuFps(CNN_FpuFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char Offset = (unsigned char) Arg->Infos[0];
	Int8toUint8((signed char *)(Arg->In + First), (Arg->Out + First), (unsigned char) Arg->Infos[0], Iter);
}


#pragma GCC diagnostic pop
