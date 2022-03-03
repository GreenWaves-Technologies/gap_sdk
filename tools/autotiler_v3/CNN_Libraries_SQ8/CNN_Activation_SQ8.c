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

#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

unsigned short int SIGMOID_LUT_uint16[256] = {
    32768, 33451, 34133, 34813, 35493, 36169, 36843, 37513, 38180, 38841, 39498,
    40149, 40794, 41432, 42064, 42688, 43304, 43912, 44511, 45102, 45683, 46255,
    46817, 47369, 47911, 48443, 48964, 49475, 49975, 50464, 50942, 51409, 51865,
    52311, 52745, 53169, 53581, 53983, 54374, 54755, 55125, 55485, 55834, 56174,
    56503, 56823, 57133, 57433, 57724, 58007, 58280, 58544, 58800, 59048, 59288,
    59519, 59743, 59959, 60168, 60370, 60565, 60753, 60935, 61110, 61279, 61441,
    61599, 61750, 61896, 62036, 62172, 62302, 62428, 62549, 62666, 62778, 62886,
    62990, 63090, 63186, 63279, 63368, 63454, 63536, 63615, 63691, 63765, 63835,
    63903, 63968, 64030, 64090, 64148, 64204, 64257, 64308, 64357, 64405, 64450,
    64494, 64536, 64576, 64614, 64652, 64687, 64721, 64754, 64786, 64816, 64845,
    64873, 64900, 64926, 64950, 64974, 64997, 65019, 65039, 65060, 65079, 65097,
    65115, 65132, 65149, 65164, 65179, 65194, 65208, 65221, 65234, 65246, 65258,
    65269, 65280, 65291, 65301, 65310, 65319, 65328, 65337, 65345, 65352, 65360,
    65367, 65374, 65381, 65387, 65393, 65399, 65404, 65410, 65415, 65420, 65425,
    65429, 65433, 65438, 65442, 65445, 65449, 65453, 65456, 65459, 65462, 65465,
    65468, 65471, 65474, 65476, 65479, 65481, 65483, 65485, 65488, 65489, 65491,
    65493, 65495, 65497, 65498, 65500, 65501, 65503, 65504, 65505, 65507, 65508,
    65509, 65510, 65511, 65512, 65513, 65514, 65515, 65516, 65517, 65517, 65518,
    65519, 65520, 65520, 65521, 65522, 65522, 65523, 65523, 65524, 65524, 65525,
    65525, 65526, 65526, 65526, 65527, 65527, 65528, 65528, 65528, 65529, 65529,
    65529, 65529, 65530, 65530, 65530, 65530, 65531, 65531, 65531, 65531, 65531,
    65532, 65532, 65532, 65532, 65532, 65532, 65533, 65533, 65533, 65533, 65533,
    65533, 65533, 65533, 65534, 65534, 65534, 65534, 65534, 65534, 65534, 65534,
    65534, 65534, 65535};

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X) {
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
        Log2Core = gap_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

#define NEAREST //Use nearest LUT element instead of linearly interpolate
static inline int __attribute__((always_inline)) SigmoidTableInt(int x, unsigned short int * table){
	/* Input x: Q12 [-8:8] range

	   Output y = sig(x) -> Q15
	*/
#ifndef NEAREST
	int result, ua, ub, ut;
	int abs_x = (Abs(x) * 3) >> 9; // * 3/4 (*3 >>2) and clip it to [0:255] (>>7) ot be an index of the LUT
	if (abs_x > 255) {
		result = 0x1FFFC00; // result = 1 in Q25
	} else {
		ua = table[abs_x];
		ub = table[abs_x+1];
		ut = abs_x & 0xFF;
		result = (ua << 9) + ut * (ub-ua); // LUT in Q16 * ut in Q9 = Q25
	}
	if (x>0) result = result;
	else     result = (1<<25) - result;
	return result >> 10;
#else
	int result;
	int abs_x = (Abs(x) * 3) >> 9; // * 3/4 (*3 >>2) and clip it to [0:255] (>>7) ot be an index of the LUT
	if (abs_x > 255) {
		result = 0xFFFF; // result = 1 in Q16
	} else {
		result = table[abs_x]; // LUT in Q16
	}
	if (x>0) result = result;
	else     result = (1<<16) - result;
	return result;
#endif
}

int SigmoidTable(int x, unsigned short int * table){
	return SigmoidTableInt(x, table) >> 1;
}

int SigmoidTableUnsigned(int x, unsigned short int * table){
	return SigmoidTableInt(x, table);
}

int TanhTable(int x, unsigned short * table){
#ifndef NEAREST
	int result, ua, ub, ut;
	int abs_x = (Abs(x) * 3) >> 8; // 2*x
	if (abs_x > 255) {
		result = 0xFFFF00;
	} else {
		ua = table[abs_x];
		ub = table[abs_x+1];
		ut = abs_x & 0xFF;
		result = (ua << 8) + ut * (ub-ua);
	}
	if (x>0) result =  result - (1 << 23);
	else     result = -result + (1 << 23);
	return result >> 8; // back to 16 bits
#else
	int result;
	int abs_x = (Abs(x) * 3) >> 8; // 2*x
	if (abs_x > 255) {
		result = 0xFFFF;
	} else {
		result = table[abs_x];
	}
	if (x>0) result =  result - (1 << 15);
	else     result = -result + (1 << 15);
	return result; // back to 16 bits
#endif
}

#define KER_ACT(Activation, in_d_type, out_d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	decl(in_d_type * __restrict__, In) = decl((in_d_type *__restrict__), Arg->In); \
	decl(out_d_type * __restrict__, Out) = decl((out_d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Max(0, Last-First); \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
\
	for (unsigned int i=First; i<Last; i++) { \
		int Acc0 = In[i]; \
		ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
		Out[i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_ACT_IO(Activation, in_d_type, out_d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int S = Arg->W*Arg->H*Arg->Feat, CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ InOut = (int *__restrict__) Arg->In; \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Max(0, Last-First); \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
\
	for (unsigned int i=0; i<Size; i++) { \
		int *In = (int *) (InOut + First); \
		out_d_type *Out = (out_d_type *) (InOut + First); \
		int Acc0 = In[i]; \
		ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
		Out[i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
	} \
	gap_waitbarrier(0); \
	KerReductIO_Compact_SQ8_1((signed char *__restrict__)InOut, (signed char *__restrict__)InOut, Size, S); \
} while(0)

#define KER_PAR_REDUCT_ACT_CHW(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	int S = Arg->Feat; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Arg->W*Arg->H; \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int c=First; c<Last; c++) { \
		for (unsigned int i=0; i<Size; i++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Size*c + i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[Size*c + i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_REDUCT_ACT_CHW(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int Feat = Arg->Feat; \
	unsigned S = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Max(0, Last-First); \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int c=0; c<Feat; c++) { \
		for (unsigned int i=First; i<Last; i++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Size*c + i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[Size*c + i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_PAR_REDUCT_IO_ACT_CHW(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	int S = Arg->Feat; \
	unsigned int Size = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	signed char *__restrict__ Out = (signed char *__restrict__)(In+First*Size); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int c=First; c<Last; c++) { \
		for (unsigned int i=0; i<Size; i++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Size*c + i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
		Out += Size; \
	} \
	gap_waitbarrier(0); \
	KerReductIO_Compact_SQ8_1((signed char *__restrict__)In, (signed char *__restrict__)In, Size*ChunkCell, Size * Arg->Feat); \
} while(0);

#define KER_REDUCT_IO_ACT_CHW(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int Feat = Arg->Feat; \
	unsigned int S = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ InOut = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Max(0, Last-First); \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
 \
	for (unsigned int c=0; c<Feat; c++) { \
		int *In = (int *) (InOut+S*c+First); \
		d_type *Out = (d_type *) (InOut+S*c+First); \
		for (unsigned int i=0; i<Size; i++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[i] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
		gap_waitbarrier(0); \
		KerReductIO_Compact_SQ8_1((signed char *__restrict__)InOut+S*c, (signed char *__restrict__)(InOut+S*c), ChunkCell, S); \
	} \
} while(0)

#define KER_PAR_REDUCT_ACT_CHW2HWC(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	int Feat = Arg->Feat; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(Feat), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, Feat); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Arg->W*Arg->H; \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int c=First; c<Last; c++) { \
		for (unsigned int i=0; i<Size; i++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Size*c + i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[Feat*i + c] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_REDUCT_ACT_CHW2HWC(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int Feat = Arg->Feat; \
	unsigned S = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int Size = Max(0, Last-First); \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int c=0; c<Feat; c++) { \
		for (unsigned int i=First; i<Last; i++) { \
	                int Acc0 = AT_SCALE(AT_NORM(In[Size*c + i], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
	                Out[Feat*i + c] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
	        } \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_PAR_REDUCT_ACT_HWC(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int Feat = Arg->Feat; \
	unsigned S = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int i=First; i<Last; i++) { \
		for (unsigned int c=0; c<Feat; c++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Feat*i + c], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[Feat*i + c] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
	} \
	gap_waitbarrier(0); \
} while(0)

#define KER_REDUCT_ACT_HWC(Activation, d_type, p_type, in_n_bits, out_n_bits, is_unsigned) \
do { \
	unsigned int Feat = Arg->Feat; \
	unsigned S = Arg->W*Arg->H; \
	unsigned int CoreId = gap_coreid(), ChunkCell = ChunkSize(S), First = CoreId*ChunkCell, Last  = Min(First+ChunkCell, S); \
	int * __restrict__ In = (int *__restrict__) Arg->In; \
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale; \
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN; \
	decl(d_type * __restrict__, Out) = decl((d_type *__restrict__), Arg->Out); \
	signed char * __restrict__ Infos = (signed char *__restrict__) Arg->Infos; \
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN]; \
	int A0 = arr_at_as(Infos, AT_INF_A0, p_type); int B0 = arr_at_as(Infos, AT_INF_B0, p_type); int C0 = arr_at_as(Infos, AT_INF_C0, p_type); \
	int Prenorm = arr_at_as(Infos, AT_INF_PRENORM, p_type); \
\
	for (unsigned int i=First; i<Last; i++) { \
		for (unsigned int c=0; c<Feat; c++) { \
			int Acc0 = AT_SCALE(AT_NORM(In[Feat*i + c], Prenorm), Scale[c], ScaleN[c]); \
			ACT_SWITCH(Acc0, Activation, ActScale, ActScaleN, A0, B0, C0, in_n_bits, is_unsigned); \
			Out[Feat*i + c] = OUT_CLIP(Acc0, is_unsigned, out_n_bits); \
		} \
	} \
	gap_waitbarrier(0); \
} while(0)

#define B_CLR(x, bits)  ((x)&(~((1<<(bits))-1)))
static void KerReductIO_Compact_SQ8_1(signed char *__restrict__ To, signed char *__restrict__ From, int Size, int TotalSize) {
        unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
        unsigned int Iter = Max(0, Last-First);

	for (int i=Size; i<TotalSize; i+=Size) {
		From += Size*4; To += Size;

        	int *pFrom = (int *) (From+First), *pTo = (int *) (To+First);
        	for (unsigned int j=0; j<Iter/8; j++) {
                	int V0 = pFrom[2*j], V1 = pFrom[2*j+1];
                	pTo[2*j] = V0; pTo[2*j+1] = V1;
        	}
        	if (Iter & 0x4) *((int *) (To + First + B_CLR(Iter, 3))) = *((int *) (From + First + B_CLR(Iter, 3)));
        	if (Iter & 0x2) *((short int *) (To + First + B_CLR(Iter, 2))) = *((short int *) (From + First + B_CLR(Iter, 2)));
        	if (Iter & 0x1) *((signed char *) (To + First + Iter - 1)) = *((signed char *) (From + First + Iter - 1));
		gap_waitbarrier(0);
	}
}

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated in parallel
*/
void KerParReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}


void KerParReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated in parallel. In: CHW Out: HWC
*/
void KerParReduct_CC_CHW2HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}


void KerParReduct_CC_CHW2HWC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_CHW2HWC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_CHW2HWC(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated in parallel
*/
void KerParReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReductIO_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_IO_ACT_CHW(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}


/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated one after the other in parallel
*/
void KerReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_ACT_CHW(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}

/* 
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated one after the other in parallel 
*/
void KerReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_REDUCT_IO_ACT_CHW(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}

/*
 * Standalone Scaled Activation, No reduction with Scale[c] ScaleN[c] - All the elements can be evaluated in parallel
*/

void Ker_ActNone_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_NONE, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_ReLU_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_RELU, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_ReLUN_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_RELUN, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_ReLUM_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_RELUM, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_ReLUMN_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_RELUMN, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_HSigmoid_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_HSIGMOID, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_HSwish_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_HSWISH, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_LEAKYRELU, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_Sigmoid_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_SIGMOID, signed char, signed char, unsigned char, 8, 8, 0);
}

void Ker_Tanh_SQ8(KerActivation_SQ8_T *Arg) {
	KER_ACT(ACT_TANH, signed char, signed char, unsigned char, 8, 8, 0);
}

/* 
 * from int32 to 8/16bits + optional Activation - Reduction with Scale[c] ScaleN[c] - All the elements can be evaluated in parallel
*/

/* ------------------------------------------------------ Signed 8 bits ------------------------------------------------------ */
void KerReduct_CC_NoScale_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_NONE, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELU, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUN, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUM, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUMN, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSIGMOID, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSWISH, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_LEAKYRELU, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_SIGMOID, int, signed char, unsigned char, 32, 8, 0);
}

void KerReduct_CC_NoScale_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_TANH, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_NONE, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELU, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUN, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUM, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUMN, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSIGMOID, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSWISH, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_LEAKYRELU, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_SIGMOID, int, signed char, unsigned char, 32, 8, 0);
}

void KerReductIO_CC_NoScale_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_TANH, int, signed char, unsigned char, 32, 8, 0);
}


/* ------------------------------------------------------ Signed 16 bits ------------------------------------------------------ */
void KerReduct_CC_NoScale_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_NONE, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_ReLU_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELU, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_ReLUN_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUN, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_ReLUM_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUM, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_ReLUMN_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUMN, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_HSigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSIGMOID, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_HSwish_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSWISH, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_LeakyReLU_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_LEAKYRELU, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_Sigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_SIGMOID, int, signed short, unsigned short, 32, 16, 0);
}

void KerReduct_CC_NoScale_Tanh_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_TANH, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_NONE, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_ReLU_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELU, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_ReLUN_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUN, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_ReLUM_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUM, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_ReLUMN_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUMN, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_HSigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSIGMOID, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_HSwish_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSWISH, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_LeakyReLU_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_LEAKYRELU, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_Sigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_SIGMOID, int, signed short, unsigned short, 32, 16, 0);
}

void KerReductIO_CC_NoScale_Tanh_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_TANH, int, signed short, unsigned short, 32, 16, 0);
}

/* ---------------------------------------------------- Unsigned 8 bits ----------------------------------------------------- */
void KerReduct_CC_NoScale_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_NONE, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_ReLU_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELU, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_ReLUN_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUN, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_ReLUM_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUM, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_ReLUMN_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUMN, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_HSigmoid_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSIGMOID, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_HSwish_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSWISH, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_LeakyReLU_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_LEAKYRELU, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_Sigmoid_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_SIGMOID, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReduct_CC_NoScale_Tanh_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_TANH, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_NONE, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_ReLU_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELU, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_ReLUN_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUN, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_ReLUM_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUM, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_ReLUMN_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUMN, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_HSigmoid_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSIGMOID, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_HSwish_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSWISH, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_LeakyReLU_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_LEAKYRELU, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_Sigmoid_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_SIGMOID, int, unsigned char, unsigned char, 32, 8, 1);
}

void KerReductIO_CC_NoScale_Tanh_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_TANH, int, unsigned char, unsigned char, 32, 8, 1);
}

/* ---------------------------------------------------- UnSigned 16 bits ----------------------------------------------------- */
void KerReduct_CC_NoScale_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_NONE, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_ReLU_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELU, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_ReLUN_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUN, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_ReLUM_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUM, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_ReLUMN_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_RELUMN, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_HSigmoid_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSIGMOID, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_HSwish_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_HSWISH, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_LeakyReLU_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_LEAKYRELU, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_Sigmoid_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_SIGMOID, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReduct_CC_NoScale_Tanh_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT(ACT_TANH, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_NONE, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_ReLU_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELU, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_ReLUN_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUN, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_ReLUM_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUM, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_ReLUMN_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_RELUMN, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_HSigmoid_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSIGMOID, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_HSwish_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_HSWISH, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_LeakyReLU_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_LEAKYRELU, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_Sigmoid_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_SIGMOID, int, unsigned short, unsigned short, 32, 16, 1);
}

void KerReductIO_CC_NoScale_Tanh_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_ACT_IO(ACT_TANH, int, unsigned short, unsigned short, 32, 16, 1);
}

/* 
	HWC Activations 
*/

/* ------------------------------------------------------ Signed 8 bits ------------------------------------------------------ */
void KerParReduct_CC_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_NONE, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUM_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUM, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_ReLUMN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUMN, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_HSigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_HSwish_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSWISH, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_LeakyReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_LEAKYRELU, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_Sigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_SIGMOID, signed char, unsigned char, 32, 8, 0);
}

void KerParReduct_CC_Tanh_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_TANH, signed char, unsigned char, 32, 8, 0);
}

/* ----------------------------------------------------- UnSigned 8 bits ----------------------------------------------------- */
void KerParReduct_CC_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_NONE, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_ReLU_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELU, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_ReLUN_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUN, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_ReLUM_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUM, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_ReLUMN_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUMN, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_HSigmoid_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSIGMOID, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_HSwish_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSWISH, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_LeakyReLU_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_LEAKYRELU, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_Sigmoid_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_SIGMOID, unsigned char, unsigned char, 32, 8, 1);
}

void KerParReduct_CC_Tanh_HWC_USQ8(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_TANH, unsigned char, unsigned char, 32, 8, 1);
}

/* ----------------------------------------------------- Signed 16 bits ---------------------------------------------------- */
void KerParReduct_CC_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_NONE, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_ReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELU, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_ReLUN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUN, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_ReLUM_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUM, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_ReLUMN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUMN, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_HSigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSIGMOID, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_HSwish_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSWISH, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_LeakyReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_LEAKYRELU, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_Sigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_SIGMOID, signed short, unsigned short, 32, 16, 0);
}

void KerParReduct_CC_Tanh_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_TANH, signed short, unsigned short, 32, 16, 0);
}


/* ----------------------------------------------------- UnSigned 16 bits ---------------------------------------------------- */
void KerParReduct_CC_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_NONE, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_ReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELU, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_ReLUN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUN, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_ReLUM_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUM, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_ReLUMN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_RELUMN, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_HSigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSIGMOID, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_HSwish_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_HSWISH, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_LeakyReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_LEAKYRELU, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_Sigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_SIGMOID, unsigned short, unsigned short, 32, 16, 1);
}

void KerParReduct_CC_Tanh_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg) {
	KER_PAR_REDUCT_ACT_HWC(ACT_TANH, unsigned short, unsigned short, 32, 16, 1);
}



#pragma GCC diagnostic pop
