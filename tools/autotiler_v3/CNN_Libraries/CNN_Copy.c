/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#include "Gap.h"
#include <stdio.h>
#include <math.h>
#include "CNN_Copy.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

#ifdef __EMUL__
#include <math.h>
float to_float(unsigned short v) {
	float res = 0;
	unsigned short * q = ((unsigned short *)&res);
	q[1] = v;
	return res;
}
#define MaxF2(a, b)	gap_pack2f16(fmax((a)[0], (b)[0]), fmax((a)[1], (b)[1]))
#define MinF2(a, b)	gap_pack2f16(fmin((a)[0], (b)[0]), fmin((a)[1], (b)[1]))
#define ClipF2(a, upper, lower) gap_pack2f16(fmax(fmin((a)[0], (upper)[0]), (lower)[0]), fmax(fmin((a)[1], (upper)[1]), (lower)[1]))
#define ClipF(a, upper, lower) fmax(fmin((a), (upper)), (lower))
#ifndef TO_FLOAT
#define TO_FLOAT(x) to_float(*((unsigned short *)&x))
#endif
#else
#ifndef TO_FLOAT
#define TO_FLOAT(x) *((F16 *)&x)
#endif
#define AbsF2(a)	__builtin_pulp_f16altabs2((a))
#define MaxF2(a, b)	__builtin_pulp_f16altmax2((a), (b))
#define MinF2(a, b)	__builtin_pulp_f16altmin2((a), (b))
#define MaxF(a, b)	__builtin_pulp_f16altmax((a), (b))
#define MinF(a, b)	__builtin_pulp_f16altmin((a), (b))
#define ClipF2(a, upper, lower) ((F16V)MaxF2(MinF2((a), (upper)), (lower)))
#define ClipF(a, upper, lower) ((F16) MaxF(MinF((a), (upper)), (lower)))
#endif

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

#define B_CLR(x, bits)  ((x)&(~((1<<(bits))-1)))
void CNN_Copy_void(KerCopy_void_T *Arg)

{
	signed char *__restrict__ From = Arg->In;
	signed char *__restrict__ To = Arg->Out;
	unsigned int Size = Arg->N;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);

	int *pFrom = (int *) (From+First), *pTo = (int *) (To+First);
	for (int i=0; i<Iter/8; i++) {
		int V0 = pFrom[2*i], V1 = pFrom[2*i+1];
		pTo[2*i] = V0; pTo[2*i+1] = V1;
	}
	if (Iter & 0x4) *((int *) (To + First + B_CLR(Iter, 3))) = *((int *) (From + First + B_CLR(Iter, 3)));
	if (Iter & 0x2) *((short int *) (To + First + B_CLR(Iter, 2))) = *((short int *) (From + First + B_CLR(Iter, 2)));
	if (Iter & 0x1) *((signed char *) (To + First + Iter - 1)) = *((signed char *) (From + First + Iter - 1));
        gap_waitbarrier(0);
}

/* Matrix transposition */

static void CNN_Transpose_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int H,
	unsigned int Ho_F,
	unsigned int Ho_L
	)
       
{
	int IterL = Ho_L-Ho_F;
	int IterW = Wo_L-Wo_F;
	for (int l=0; l<IterL/4; l++) {
		v4s *pV0 = (v4s *) (In + (Ho_F+4*l+0)*W + Wo_F);
		v4s *pV1 = (v4s *) (In + (Ho_F+4*l+1)*W + Wo_F);
		v4s *pV2 = (v4s *) (In + (Ho_F+4*l+2)*W + Wo_F);
		v4s *pV3 = (v4s *) (In + (Ho_F+4*l+3)*W + Wo_F);
		signed char *pO  = (Out + Ho_F + 4*l + Wo_F*H);
		for (int c=0; c<IterW/4; c++) {
			v4s X, Y;
			v4s A = pV0[c], B = pV1[c], C = pV2[c], D = pV3[c];
			v4s rA, rB, rC, rD;

			X  = __builtin_shuffle(A, B, (v4s) {0,4,1,5});
			Y  = __builtin_shuffle(C, D, (v4s) {0,4,1,5});
			rA = __builtin_shuffle(X, Y, (v4s) {0,1,4,5});
			rB = __builtin_shuffle(X, Y, (v4s) {2,3,6,7});

			X  = __builtin_shuffle(A, B, (v4s) {2,6,3,7});
			Y  = __builtin_shuffle(C, D, (v4s) {2,6,3,7});
			rC = __builtin_shuffle(X, Y, (v4s) {0,1,4,5});
			rD = __builtin_shuffle(X, Y, (v4s) {2,3,6,7});

			*((v4s *) (pO+(4*c+0)*H)) = rA;
			*((v4s *) (pO+(4*c+1)*H)) = rB;
			*((v4s *) (pO+(4*c+2)*H)) = rC;
			*((v4s *) (pO+(4*c+3)*H)) = rD;
		}
		for (int c=4*(IterW/4); c<IterW; c++) {
			int X0 = In[(Ho_F+4*l+0)*W + (Wo_F+c)], X1 = In[(Ho_F+4*l+1)*W + (Wo_F+c)], X2 = In[(Ho_F+4*l+2)*W + (Wo_F+c)], X3 = In[(Ho_F+4*l+3)*W + (Wo_F+c)];
			Out[Ho_F+4*l+0 + (Wo_F+c)*H] = X0; Out[Ho_F+4*l+1 + (Wo_F+c)*H] = X1; Out[Ho_F+4*l+2 + (Wo_F+c)*H] = X2; Out[Ho_F+4*l+3 + (Wo_F+c)*H] = X3;
		}
	}
	for (int l=4*(IterL/4); l<IterL; l++) {
		for (int c=0; c<IterW/2; c++) {
			signed char X0 = In[(Ho_F+l)*W + Wo_F+2*c+0], X1 = In[(Ho_F+l)*W + Wo_F+2*c+1];
			Out[Ho_F+l + (Wo_F+2*c+0)*H] = X0;
			Out[Ho_F+l + (Wo_F+2*c+1)*H] = X1;
		}
		if (IterW&0x1) Out[Ho_F+l+ (Wo_L-1)*H] = In[(Ho_F+l)*W+Wo_L-1];
	}
}

static void CNN_Transpose_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int H,
	unsigned int Ho_F,
	unsigned int Ho_L
	)
       
{
	int IterL = Ho_L-Ho_F;
	int IterW = Wo_L-Wo_F;
	for (int l=0; l<IterL/2; l++) {
		v2s *pV0 = (v2s *) (In + (Ho_F+2*l+0)*W + Wo_F);
		v2s *pV1 = (v2s *) (In + (Ho_F+2*l+1)*W + Wo_F);
		short int *pO  = (Out + Ho_F + 2*l + Wo_F*H);
		for (int c=0; c<IterW/2; c++) {
			v2s A = pV0[c], B = pV1[c];
			v2s rA, rB;
			rA = __builtin_shuffle(A, B, (v2s) {0,2});
			rB = __builtin_shuffle(A, B, (v2s) {1,3});
			*((v2s *) (pO+(2*c+0)*H)) = rA;
			*((v2s *) (pO+(2*c+1)*H)) = rB;
		}
		if (IterW&0x1) {
			int c = IterW-1;
			int X0 = In[(Ho_F+2*l+0)*W + (Wo_F+c)], X1 = In[(Ho_F+2*l+1)*W + (Wo_F+c)];
			Out[Ho_F+2*l+0 + (Wo_F+c)*H] = X0; Out[Ho_F+2*l+1 + (Wo_F+c)*H] = X1;
		}
	}
	if (IterL&0x1) {
		int l = IterL-1;
		for (int c=0; c<IterW/2; c++) {
			int X0 = In[(Ho_F+l)*W + Wo_F+2*c+0], X1 = In[(Ho_F+l)*W + Wo_F+2*c+1];
			Out[Ho_F+l + (Wo_F+2*c+0)*H] = X0;
			Out[Ho_F+l + (Wo_F+2*c+1)*H] = X1;
		}
		if (IterW&0x1) Out[Ho_F+l+ (Wo_L-1)*H] = In[(Ho_F+l)*W+Wo_L-1];
	}
}

static void CNN_TransposeSxSy_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int Ho_F,
	unsigned int Ho_L,
	unsigned int Ho,
	unsigned int Sx,
	unsigned int Sy
	)
{
	for (int l=Ho_F; l<Ho_L; l++) {
		int IterW = Wo_L-Wo_F;
		for (int c=0; c<IterW/2; c++) {
			short int X0 = In[Sy*l*W + Sx*(Wo_F+2*c+0)], X1 = In[Sy*l*W + Sx*(Wo_F+2*c+1)];
			Out[l + (Wo_F+2*c+0)*Ho] = X0;
			Out[l + (Wo_F+2*c+1)*Ho] = X1;
		}
		if (IterW&0x1) Out[l+ (Wo_L-1)*Ho] = In[Sy*l*W+Sx*(Wo_L-1)];
	}
}

static void CNN_Transpose2_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int H,
	unsigned int Ho_F,
	unsigned int Ho_L
	)
{
	for (int l=Ho_F; l<Ho_L; l++) {
		int IterW = Wo_L-Wo_F;
		for (int c=0; c<IterW/2; c++) {
			signed char X0 = In[l*W + Wo_F+2*c+0], X1 = In[l*W + Wo_F+2*c+1];
			Out[l + (Wo_F+2*c+0)*H] = X0;
			Out[l + (Wo_F+2*c+1)*H] = X1;
		}
		if (IterW&0x1) Out[l+ (Wo_L-1)*H] = In[l*W+Wo_L-1];
	}
}

static void CNN_TransposeSxSy_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	unsigned int W,
	unsigned int Wo_F,
	unsigned int Wo_L,
	unsigned int Ho,
	unsigned int Ho_F,
	unsigned int Ho_L,
	unsigned int Sx,
	unsigned int Sy
	)
{
	for (int l=Ho_F; l<Ho_L; l++) {
		int IterW = Wo_L-Wo_F;
		for (int c=0; c<IterW/2; c++) {
			signed char X0 = In[Sy*l*W + Sx*(Wo_F+2*c+0)], X1 = In[Sy*l*W + Sx*(Wo_F+2*c+1)];
			Out[l + (Wo_F+2*c+0)*Ho] = X0;
			Out[l + (Wo_F+2*c+1)*Ho] = X1;
		}
		if (IterW&0x1) Out[l+ (Wo_L-1)*Ho] = In[Sy*l*W+Sx*(Wo_L-1)];
	}
}

void CNN_ParTranspose_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_Transpose_Body_fp(In+W*H*f, Out+W*H*f, W, 0, W, H, 0, H);
	gap_waitbarrier(0);
}

void CNN_ParTransposeSxSy_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_TransposeSxSy_Body_fp(In+W*H*f, Out+Wo*Ho*f, W, 0, Wo, Ho, 0, Ho, Sx, Sy);
	gap_waitbarrier(0);
}

void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_Transpose_Body_fps(In+W*H*f, Out+W*H*f, W, 0, W, H, 0, H);
	gap_waitbarrier(0);
}

void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
        unsigned int CoreId = gap_coreid();
        unsigned int Chunk = ChunkSize(Feat);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, Feat);

	for (int f=First; f<Last; f++) CNN_TransposeSxSy_Body_fps(In+W*H*f, Out+Wo*Ho*f, W, 0, Wo, Ho, 0, Ho, Sx, Sy);
	gap_waitbarrier(0);
}

void CNN_Transpose_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (H>W) {
		/* Tile horizontally */
		Chunk = ChunkSize(H); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, H); Wo_F = 0; Wo_L = W;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(W); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, W); Ho_F = 0; Ho_L = H;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_Transpose_Body_fp(In+W*H*f, Out+W*H*f, W, Wo_F, Wo_L, H, Ho_F, Ho_L); 
	}
	gap_waitbarrier(0);
}

void CNN_TransposeSxSy_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (Ho>Wo) {
		/* Tile horizontally */
		Chunk = ChunkSize(Ho); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, Ho); Wo_F = 0; Wo_L = Wo;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(Wo); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, Wo); Ho_F = 0; Ho_L = Ho;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_TransposeSxSy_Body_fp(In+W*H*f, Out+Wo*Ho*f, W, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy); 
	}
	gap_waitbarrier(0);
}

void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (H>W) {
		/* Tile horizontally */
		Chunk = ChunkSize(H); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, H); Wo_F = 0; Wo_L = W;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(W); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, W); Ho_F = 0; Ho_L = H;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_Transpose_Body_fps(In+W*H*f, Out+W*H*f, W, Wo_F, Wo_L, H, Ho_F, Ho_L); 
	}
	gap_waitbarrier(0);
}


void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Wo_F, Wo_L, Ho_F, Ho_L;
	unsigned int Sx = Arg->Sx;
	unsigned int Sy = Arg->Sy;
	unsigned int Wo = (W+Sx-1)/Sx;
	unsigned int Ho = (H+Sy-1)/Sy;
	unsigned int Feat = Arg->Feat;
        unsigned int Chunk, CoreId = gap_coreid();

	if (Ho>Wo) {
		/* Tile horizontally */
		Chunk = ChunkSize(Ho); Ho_F = Chunk*CoreId; Ho_L = Min(Ho_F+Chunk, Ho); Wo_F = 0; Wo_L = Wo;
	} else {
		/* Tile vertically */
		Chunk = ChunkSize(Wo); Wo_F = Chunk*CoreId; Wo_L = Min(Wo_F+Chunk, Wo); Ho_F = 0; Ho_L = Ho;
	}
	if (Wo_F<Wo_L && Ho_F<Ho_L) {
		for (int f=0; f<Feat; f++) CNN_TransposeSxSy_Body_fps(In+W*H*f, Out+Wo*Ho*f, W, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy); 
	}
	gap_waitbarrier(0);
}

/* 3D Tensor permutations */

void CNN_MatPermCHW2CWH_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[c*H*W + (w+0)*H + h] = V0;
				Out[c*H*W + (w+1)*H + h] = V1;
			}
			if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2CWH_fp(KerMatTranspose_fp_T *Arg)

{
        short int *__restrict__ In = Arg->In;
        short int *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[c*H*W + (w+0)*H + h] = V0;
                                Out[c*H*W + (w+1)*H + h] = V1;
                        }
                        if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2HWC_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*W*C + (w+0)*C + c] = V0;
				Out[h*W*C + (w+1)*C + c] = V1;
			}
			if (W&0x1) Out[h*W*C + (W-1)*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2CHW_fp(KerMatTranspose_fp_T *Arg)

{
        short int *__restrict__ In = Arg->In;
        short int *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[c*W*H + h*W + (w+0)] = V0;
                                Out[c*W*H + h*W + (w+1)] = V1;
                        }
                        if (W&0x1) Out[c*W*H + h*W + W-1] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2WHC_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*H*C + h*C + c] = V0;
				Out[(w+1)*H*C + h*C + c] = V1;
			}
			if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2WHC_fp(KerMatTranspose_fp_T *Arg)

{
        short int *__restrict__ In = Arg->In;
        short int *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[(w+0)*H*C + h*C + c] = V0;
                                Out[(w+1)*H*C + h*C + c] = V1;
                        }
                        if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2WCH_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*C*H + c*H + h] = V0;
				Out[(w+1)*C*H + c*H + h] = V1;
			}
			if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2WCH_fp(KerMatTranspose_fp_T *Arg)

{
        short int *__restrict__ In = Arg->In;
        short int *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[(w+0)*C*H + c*H + h] = V0;
                                Out[(w+1)*C*H + c*H + h] = V1;
                        }
                        if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2HCW_fp(KerMatTranspose_fp_T *Arg)

{
	short int *__restrict__ In = Arg->In;
	short int *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*C*W + c*W + (w+0)] = V0;
				Out[h*C*W + c*W + (w+1)] = V1;
			}
			if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2HCW_fp(KerMatTranspose_fp_T *Arg)

{
        short int *__restrict__ In = Arg->In;
        short int *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[h*C*W + c*W + (w+0)] = V0;
                                Out[h*C*W + c*W + (w+1)] = V1;
                        }
                        if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}


void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W + h*W + (w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[c*H*W + (w+0)*H + h] = V0;
				Out[c*H*W + (w+1)*H + h] = V1;
			}
			if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2CWH_fps(KerMatTranspose_fps_T *Arg)

{
        signed char *__restrict__ In = Arg->In;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[c*H*W + (w+0)*H + h] = V0;
                                Out[c*H*W + (w+1)*H + h] = V1;
                        }
                        if (W&0x1) Out[c*H*W + (W-1)*H + h] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*W*C + (w+0)*C + c] = V0;
				Out[h*W*C + (w+1)*C + c] = V1;
			}
			if (W&0x1) Out[h*W*C + (W-1)*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2CHW_fps(KerMatTranspose_fps_T *Arg)

{
        signed char *__restrict__ In = Arg->In;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[c*W*H + h*W + (w+0)] = V0;
                                Out[c*W*H + h*W + (w+1)] = V1;
                        }
                        if (W&0x1) Out[c*W*H + h*W + W-1] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*H*C + h*C + c] = V0;
				Out[(w+1)*H*C + h*C + c] = V1;
			}
			if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2WHC_fps(KerMatTranspose_fps_T *Arg)

{
        signed char *__restrict__ In = Arg->In;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[(w+0)*H*C + h*C + c] = V0;
                                Out[(w+1)*H*C + h*C + c] = V1;
                        }
                        if (W&0x1) Out[(W-1)*H*C + h*C + c] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[(w+0)*C*H + c*H + h] = V0;
				Out[(w+1)*C*H + c*H + h] = V1;
			}
			if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2WCH_fps(KerMatTranspose_fps_T *Arg)

{
        signed char *__restrict__ In = Arg->In;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[(w+0)*C*H + c*H + h] = V0;
                                Out[(w+1)*C*H + c*H + h] = V1;
                        }
                        if (W&0x1) Out[(W-1)*C*H + c*H + h] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
}

void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg)

{
	signed char *__restrict__ In = Arg->In;
	signed char *__restrict__ Out = Arg->Out;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (int c=First; c<Last; c++) {
		for (int h=0; h<H; h++) {
			for (int w=0; w<((W/2)*2); w+=2) {
				int V0 = In[c*H*W+h*W+(w+0)], V1 = In[c*H*W+h*W+(w+1)];
				Out[h*C*W + c*W + (w+0)] = V0;
				Out[h*C*W + c*W + (w+1)] = V1;
			}
			if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[c*H*W+h*W+W-1];
		}
	}
	gap_waitbarrier(0);
}

void CNN_MatPermHWC2HCW_fps(KerMatTranspose_fps_T *Arg)

{
        signed char *__restrict__ In = Arg->In;
        signed char *__restrict__ Out = Arg->Out;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int Feat = Arg->Feat, C = Feat;
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);
        for (int c=First; c<Last; c++) {
                for (int h=0; h<H; h++) {
                        for (int w=0; w<((W/2)*2); w+=2) {
                                int V0 = In[h*W*C + w*Feat + c], V1 = In[h*W*C+(w+1)*Feat+c];
                                Out[h*C*W + c*W + (w+0)] = V0;
                                Out[h*C*W + c*W + (w+1)] = V1;
                        }
                        if (W&0x1) Out[h*C*W + c*W + (W-1)] = In[h*W*C+(W-1)*Feat+c];
                }
        }
        gap_waitbarrier(0);
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

	for (int i=0; i<N/8; i++) {
		v4u V0 = pIn[2*i], V1 = pIn[2*i+1];
		V0 = V0 + Off; V1 = V1 + Off;
		pOut[2*i] = V0; pOut[2*i+1] = V1;
	}
	for (int i=(N-N%8); i<N; i++) {
		((unsigned char *)Out)[i] = ((unsigned char *)In)[i] + Offset;
	}
	gap_waitbarrier(0);
}

void CNN_UFpsFpsScaled(CNN_UFpsFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = Arg->In + First;
	v4s *pOut = (v4s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 7)
		);
	}
	signed char * pcOut = ((signed char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7);
	}
	gap_waitbarrier(0);
}

void CNN_FpsUFpsScaled(CNN_FpsUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = Arg->In + First;
	v4u *pOut = (v4u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 8)
		);
	}
	unsigned char * pcOut = ((unsigned char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8);
	}
	gap_waitbarrier(0);
}

void CNN_UFpsUFpsScaled(CNN_UFpsUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = Arg->In + First;
	v4u *pOut = (v4u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 8)
		);
	}
	unsigned char * pcOut = ((unsigned char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8);
	}
	gap_waitbarrier(0);
}

void CNN_FpsFpsScaled(CNN_FpsFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = Arg->In + First;
	v4s *pOut = (v4s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 7)
		);
	}
	signed char * pcOut = ((signed char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7);
	}
	gap_waitbarrier(0);
}

void CNN_FpsUFps(CNN_FpsUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	Int8toUint8((Arg->In + First), (signed char *)(Arg->Out + First), (unsigned char) Arg->Infos[0], Iter);
}

void CNN_UFpsFps(CNN_UFpsFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	Int8toUint8((signed char *)(Arg->In + First), (Arg->Out + First), (unsigned char) Arg->Infos[0], Iter);
}

static void Int16toUint16(signed short *__restrict__ In, signed short *__restrict__ Out, unsigned short Offset, unsigned int N)
{
	v2u *pIn = (v2u *) In, *pOut = (v2u *) Out;
	v2u Off  = (v2u) {Offset, Offset};

	for (int i=0; i<N/4; i++) {
		v2u V0 = pIn[2*i], V1 = pIn[2*i+1];
		V0 = V0 + Off; V1 = V1 + Off;
		pOut[2*i] = V0; pOut[2*i+1] = V1;
	}
	for (int i=(N-N%4); i<N; i++) {
		((unsigned short *)Out)[i] = ((unsigned short *)In)[i] + Offset;
	}
	gap_waitbarrier(0);
}

void CNN_FpUFp(CNN_FpUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	Int16toUint16((Arg->In + First), (signed short *)(Arg->Out + First), *((unsigned short *)&Arg->Infos[0]), Iter);
}

void CNN_UFpFp(CNN_UFpFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	Int16toUint16((signed short *)(Arg->In + First), (Arg->Out + First), *((unsigned short *)&Arg->Infos[0]), Iter);
}

void CNN_UFpFpsScaled(CNN_UFpFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short int *pIn = Arg->In + First;
	v4s *pOut = (v4s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 7)
		);
	}
	signed char * pcOut = ((signed char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7);
	}
	gap_waitbarrier(0);
}

void CNN_UFpUFpsScaled(CNN_UFpUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short int *pIn = Arg->In + First;
	v4u *pOut = (v4u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 8)
		);
	}
	unsigned char * pcOut = ((unsigned char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (unsigned char) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 8);
	}
	gap_waitbarrier(0);
}

void CNN_FpFpsScaled(CNN_FpFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short int *pIn = Arg->In + First;
	v4s *pOut = (v4s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v2 + zero_diff) * scale, norm), 7),
			(signed char) gap_clip(gap_roundnorm_reg((v3 + zero_diff) * scale, norm), 7)
		);
	}
	signed char * pcOut = ((signed char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*(pcOut++) = (signed char) gap_clip(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 7);
	}
	gap_waitbarrier(0);
}

void CNN_FpUFpsScaled(CNN_FpUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short int *pIn = Arg->In + First;
	v4u *pOut = (v4u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = (unsigned char)Arg->Infos[AT_INF_QUANT_SCALE];
	unsigned char norm = (unsigned char)Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) gap_clipu(gap_roundnormu_reg((v0 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnormu_reg((v1 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnormu_reg((v2 + zero_diff) * scale, norm), 8),
			(unsigned char) gap_clipu(gap_roundnormu_reg((v3 + zero_diff) * scale, norm), 8)
		);
	}
	unsigned char * pcOut = ((unsigned char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*pcOut++ = (unsigned char) gap_clipu(gap_roundnormu_reg((v0 + zero_diff) * scale, norm), 8);
	}
	gap_waitbarrier(0);
}

void CNN_FpFps(CNN_FpFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short int *pIn = Arg->In + First;
	v4s *pOut = (v4s *) (Arg->Out + First);

	for (int i=0; i<Iter/4; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++); int v2 = (int)(*pIn++); int v3 = (int)(*pIn++);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) gap_clip(gap_roundnorm(v0, 8), 7),
			(signed char) gap_clip(gap_roundnorm(v1, 8), 7),
			(signed char) gap_clip(gap_roundnorm(v2, 8), 7),
			(signed char) gap_clip(gap_roundnorm(v3, 8), 7)
		);
	}
	signed char * pcOut = ((signed char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		int v0 = (int)(*pIn++);
		*pcOut++ = (signed char) gap_clip(gap_roundnorm(v0, 8), 7);
	}
	gap_waitbarrier(0);
}

void CNN_UFpUFps(CNN_UFpUFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short int *pIn = Arg->In + First;
	v4u *pOut = (v4u *) (Arg->Out + First);

	for (int i=0; i<Iter/4; i++) {
		unsigned int v0 = (int)(*pIn++); unsigned int v1 = (int)(*pIn++);
		unsigned int v2 = (int)(*pIn++); unsigned int v3 = (int)(*pIn++);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) gap_clipu(gap_roundnormu(v0, 8), 8),
			(unsigned char) gap_clipu(gap_roundnormu(v1, 8), 8),
			(unsigned char) gap_clipu(gap_roundnormu(v2, 8), 8),
			(unsigned char) gap_clipu(gap_roundnormu(v3, 8), 8)
		);
	}
	unsigned char * pcOut = ((unsigned char *) pOut);
	for (int i=0; i<Iter%4; i++) {
		unsigned int v0 = (unsigned int)(*pIn++);
		*pcOut++ = (unsigned char) gap_clipu(gap_roundnormu(v0, 8), 8);
	}
	gap_waitbarrier(0);
}

void CNN_FpFpScaled(CNN_FpFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short *pIn = Arg->In + First;
	v2s *pOut = (v2s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2s) gap_pack2(
			(signed short int) gap_clip(gap_roundnorm_reg(((v0 + zero_diff) * scale), norm), 15),
			(signed short int) gap_clip(gap_roundnorm_reg(((v1 + zero_diff) * scale), norm), 15)
		);
	}

	if (Iter % 2) {
		*((signed short int *)pOut) = (signed short int) gap_clip(gap_roundnorm_reg(((((int)*pIn) + zero_diff) * scale), norm), 15);
	}
	gap_waitbarrier(0);
}

void CNN_UFpFpScaled(CNN_UFpFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short *pIn = Arg->In + First;
	v2s *pOut = (v2s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2s) gap_pack2(
			(signed short int) gap_clip(gap_roundnorm_reg(((v0 + zero_diff) * scale), norm), 15),
			(signed short int) gap_clip(gap_roundnorm_reg(((v1 + zero_diff) * scale), norm), 15)
		);
	}

	if (Iter % 2) {
		*((signed short int *)pOut) = (signed short int) gap_clip(gap_roundnorm_reg(((((int)*pIn) + zero_diff) * scale), norm), 15);
	}
	gap_waitbarrier(0);
}

void CNN_FpUFpScaled(CNN_FpUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short *pIn = Arg->In + First;
	v2u *pOut = (v2u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2u) gap_pack2(
			((unsigned short int) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 16)),
			((unsigned short int) gap_clipu(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 16))
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = ((unsigned short int) gap_clipu(gap_roundnorm_reg((((int)*pIn) + zero_diff) * scale, norm), 16));
	}
	gap_waitbarrier(0);
}

void CNN_UFpUFpScaled(CNN_UFpUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short *pIn = Arg->In + First;
	v2u *pOut = (v2u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned char scale = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2u) gap_pack2(
			((unsigned short int) gap_clipu(gap_roundnorm_reg((v0 + zero_diff) * scale, norm), 16)),
			((unsigned short int) gap_clipu(gap_roundnorm_reg((v1 + zero_diff) * scale, norm), 16))
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = ((unsigned short int) gap_clipu(gap_roundnorm_reg((((int)*pIn) + zero_diff) * scale, norm), 16));
	}
	gap_waitbarrier(0);
}

void CNN_FpsFpScaled(CNN_FpsFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = Arg->In + First;
	v2s *pOut = (v2s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned short scale = *((unsigned short *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2s) gap_pack2(
			(signed short int) gap_clip(gap_roundnorm_reg((v0 * scale), norm) + zero_diff, 15),
			(signed short int) gap_clip(gap_roundnorm_reg((v1 * scale), norm) + zero_diff, 15)
		);
	}

	if (Iter % 2) {
		*((signed short int *)pOut) = (signed short int) gap_clip(gap_roundnorm_reg((((int)*pIn) * scale), norm) + zero_diff, 15);
	}
	gap_waitbarrier(0);
}

void CNN_FpsUFpScaled(CNN_FpsUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = Arg->In + First;
	v2u *pOut = (v2u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned short scale = *((unsigned short *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = *((unsigned char *)&Arg->Infos[AT_INF_QUANT_NORM]);


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2u) gap_pack2(
			(unsigned short int) gap_clipu(gap_roundnorm_reg(v0 * scale, norm) + zero_diff, 16),
			(unsigned short int) gap_clipu(gap_roundnorm_reg(v1 * scale, norm) + zero_diff, 16)
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = (unsigned short int) gap_clipu(gap_roundnorm_reg(((int)*pIn) * scale, norm) + zero_diff, 16);
	}
	gap_waitbarrier(0);
}

void CNN_UFpsFpScaled(CNN_UFpsFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = Arg->In + First;
	v2s *pOut = (v2s *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned short scale = *((unsigned short *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = (unsigned char) Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2s) gap_pack2(
			(signed short int) gap_clip(gap_roundnorm_reg(v0 * scale, norm) + zero_diff, 15),
			(signed short int) gap_clip(gap_roundnorm_reg(v1 * scale, norm) + zero_diff, 15)
		);
	}

	if (Iter % 2) {
		int v0 = (int)(*pIn++);
		*((signed short int *)pOut) = (signed short int) gap_clip(gap_roundnorm_reg(v0 * scale, norm) + zero_diff, 15);
	}
	gap_waitbarrier(0);
}

void CNN_UFpsUFp(CNN_UFpsUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = Arg->In + First;
	v2u *pOut = (v2u *) (Arg->Out + First);

	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2u) gap_pack2(
			(unsigned short int) v0 << 8,
			(unsigned short int) v1 << 8
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = (unsigned short int) ((int)(*pIn)) << 8;
	}
	gap_waitbarrier(0);
}

void CNN_UFpsUFpScaled(CNN_UFpsUFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = Arg->In + First;
	v2u *pOut = (v2u *) (Arg->Out + First);
	signed int zero_diff = *((signed int  *)&Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	unsigned short scale = *((unsigned short *)&Arg->Infos[AT_INF_QUANT_SCALE]);
	unsigned char norm = (unsigned char) Arg->Infos[AT_INF_QUANT_NORM];


	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*(pOut++) = (v2u) gap_pack2(
			(unsigned short int) gap_clipu(gap_roundnormu_reg(v0 * scale, norm) + zero_diff, 16),
			(unsigned short int) gap_clipu(gap_roundnormu_reg(v1 * scale, norm) + zero_diff, 16)
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = (signed short int) gap_clipu(gap_roundnormu_reg(((int)*pIn) * scale, norm) + zero_diff, 16);
	}
	gap_waitbarrier(0);
}

void CNN_FpsFp(CNN_FpsFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = Arg->In + First;
	v2s *pOut = (v2s *) (Arg->Out + First);

	for (int i=0; i<Iter/2; i++) {
		int v0 = (int)(*pIn++); int v1 = (int)(*pIn++);
		*pOut = (v2s) gap_pack2(
			(signed short int) v0,
			(signed short int) v1
		);
		*(pOut++) <<= 8;
	}

	if (Iter % 2) {
		*((signed short int *)pOut) = (signed short int) *pIn << 8;
	}
	gap_waitbarrier(0);
}

void CNN_FpFloat16(CNN_FpFloat16_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed short *pIn = (signed short *) (Arg->In + First);
	F16V *pOut = (F16V *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = gap_pack2f16((F16) *(pIn++), (F16) *(pIn++));
		*pOut++ = (v_in + zero_v) * scale_v;
	}


	if (Iter % 2) {
		*((F16 *)pOut) = ((F16) *pIn + zero_diff) * scale;
	}
	gap_waitbarrier(0);
}

void CNN_UFpFloat16(CNN_UFpFloat16_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned short *pIn = (unsigned short *) (Arg->In + First);
	F16V *pOut = (F16V *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = gap_pack2f16((F16) *(pIn++), (F16) *(pIn++));
		*pOut++ = (v_in + zero_v) * scale_v;
	}


	if (Iter % 2) {
		*((F16 *)pOut) = ((F16) *pIn + zero_diff) * scale;
	}
	gap_waitbarrier(0);
}

void CNN_FpsFloat16(CNN_FpsFloat16_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	signed char *pIn = (signed char *) (Arg->In + First);
	F16V *pOut = (F16V *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = gap_pack2f16((F16) *(pIn++), (F16) *(pIn++));
		*pOut++ = (v_in + zero_v) * scale_v;
	}


	if (Iter % 2) {
		*((F16 *)pOut) = ((F16) *pIn + zero_diff) * scale;
	}
	gap_waitbarrier(0);
}

void CNN_UFpsFloat16(CNN_UFpsFloat16_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	unsigned char *pIn = (signed char *) (Arg->In + First);
	F16V *pOut = (F16V *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = gap_pack2f16((F16) *(pIn++), (F16) *(pIn++));
		*pOut++ = (v_in + zero_v) * scale_v;
	}


	if (Iter % 2) {
		*((F16 *)pOut) = ((F16) *pIn + zero_diff) * scale;
	}
	gap_waitbarrier(0);
}

void CNN_Float16Fp(CNN_Float16Fp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	F16V *pIn = (F16V *) (Arg->In + First);
	v2s *pOut = (v2s *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);
	F16V max_v = gap_pack2f16(32767.0F, 32767.0F);
	F16V min_v = gap_pack2f16(-32768.0F, -32768.0F);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = (*(pIn++) + zero_v) * scale_v;
		v_in = ClipF2(v_in, max_v, min_v);
		*(pOut++) = (v2s) gap_pack2(
			(signed short int) v_in[0],
			(signed short int) v_in[1]
		);
	}

	if (Iter % 2) {
		*((signed short int *)pOut) = (signed short int) ClipF((*((F16 *) pIn) + zero_diff) * scale, 32767.0F, -32768.0F);
	}
	gap_waitbarrier(0);
}

void CNN_Float16UFp(CNN_Float16UFp_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	F16V *pIn = (F16V *) (Arg->In + First);
	v2u *pOut = (v2u *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);
	F16V max_v = gap_pack2f16(65535.0F, 65535.0F);
	F16V min_v = gap_pack2f16(0.0F, 0.0F);

	for (int i=0; i<Iter/2; i++) {
		F16V v_in = (*(pIn++) + zero_v) * scale_v;
		v_in = ClipF2(v_in, max_v, min_v);
		*(pOut++) = (v2u) gap_pack2(
			(unsigned short int) v_in[0],
			(unsigned short int) v_in[1]
		);
	}

	if (Iter % 2) {
		*((unsigned short int *)pOut) = (unsigned short int) ClipF((*((F16 *) pIn) + zero_diff) * scale, 65535.0F, 0.0F);
	}
	gap_waitbarrier(0);
}

void CNN_Float16Fps(CNN_Float16Fps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	F16V *pIn = (F16V *) (Arg->In + First);
	v4s *pOut = (v4s *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);
	F16V max_v = gap_pack2f16(127.0F, 127.0F);
	F16V min_v = gap_pack2f16(-128.0F, -128.0F);

	for (int i=0; i<Iter/4; i++) {
		F16V v_in0 = (*(pIn++) + zero_v) * scale_v;
		v_in0 = ClipF2(v_in0, max_v, min_v);
		F16V v_in1 = (*(pIn++) + zero_v) * scale_v;
		v_in1 = ClipF2(v_in1, max_v, min_v);
		*(pOut++) = (v4s) gap_pack4(
			(signed char) v_in0[0],
			(signed char) v_in0[1],
			(signed char) v_in1[0],
			(signed char) v_in1[1]
		);
	}

	signed char * pcOut = ((signed char *) pOut);
	F16 * pcIn = (F16 *)(pIn);
	for (int i=0; i<Iter%4; i++) {
		*pcOut++ = (signed char) ClipF((*pcIn++ + zero_diff) * scale, 127.0F, -128.0F);
	}
	gap_waitbarrier(0);
}

void CNN_Float16UFps(CNN_Float16UFps_T * Arg)
{
	unsigned int Size = Arg->W * Arg->H;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
	unsigned int Iter = Max(0, Last-First);
	F16V *pIn = (F16V *) (Arg->In + First);
	v4u *pOut = (v4u *) (Arg->Out + First);
	F16 zero_diff = TO_FLOAT(Arg->Infos[AT_INF_QUANT_ZERO_DIFF]);
	F16 scale = TO_FLOAT(Arg->Infos[AT_INF_QUANT_SCALE]);
	F16V scale_v = gap_pack2f16(scale, scale);
	F16V zero_v = gap_pack2f16(zero_diff, zero_diff);
	F16V max_v = gap_pack2f16(255.0F, 255.0F);
	F16V min_v = gap_pack2f16(0.0F, 0.0F);

	for (int i=0; i<Iter/4; i++) {
		F16V v_in0 = (*(pIn++) + zero_v) * scale_v;
		v_in0 = ClipF2(v_in0, max_v, min_v);
		F16V v_in1 = (*(pIn++) + zero_v) * scale_v;
		v_in1 = ClipF2(v_in1, max_v, min_v);
		*(pOut++) = (v4u) gap_pack4(
			(unsigned char) v_in0[0],
			(unsigned char) v_in0[1],
			(unsigned char) v_in1[0],
			(unsigned char) v_in1[1]
		);
	}

	unsigned char * pcOut = ((unsigned char *) pOut);
	F16 * pcIn = (F16 *)(pIn);
	for (int i=0; i<Iter%4; i++) {
		*pcOut++ = (unsigned char) ClipF((*pcIn++ + zero_diff) * scale, 255.0F, 0.0F);
	}
	gap_waitbarrier(0);
}

#pragma GCC diagnostic pop
