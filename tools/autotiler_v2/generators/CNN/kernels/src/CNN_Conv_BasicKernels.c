#include "Gap8.h"
#include "CNN_BasicKernels.h"

#ifdef __pulp__
#define Min(a, b)	__builtin_pulp_minsi((a), (b))
#define Max(a, b)	__builtin_pulp_maxsi((a), (b))
#else
#define Min(a, b)	(((a)<(b))?(a):(b))
#define Max(a, b)	(((a)>(b))?(a):(b))
#endif

#define VOL volatile

static int CoreCountDynamic = 1;
static int ActiveCore = gap8_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

static int FirstDefinedOutput(unsigned int F, unsigned int Pad, unsigned int Stride)

{
        // k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

        return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(unsigned int DimIn, unsigned int F, unsigned int PadL, unsigned int Stride)

{
        // k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

        return ((DimIn - ((F-1)/2 - PadL + (F/2)) + Stride-1)/Stride);
}

static inline int MinCond(int a, int b)

{
#ifdef DIM_ALWAYS_GREATER_THAN_FILTER
	return a;
#else
	return Min(a, b);
#endif
}

/* Padded Convolution Border processing

	Zero padding support. Implementation is based on partial convolutions derived from the original filter

	Input feature maps, Output feature maps and filter on bytes

		KerConv3x3BorderStride1_fps
			|------	KerConv2x3from3x3Stride1_V_fps	3x3 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride1_H_fps	3x3 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStride2_fps
			|------	KerConv2x3from3x3Stride2_V_fps	3x3 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride2_H_fps	3x3 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStrideS_fps
			|------	KerConv2x3from3x3StrideS_V_fps	3x3 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3StrideS_H_fps	3x3 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride1_fps
			|------	KerConv4x5from5x5Stride1_V_fps	5x5 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride1_H_fps	5x5 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride2_fps
			|------	KerConv4x5from5x5Stride2_V_fps	5x5 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride2_H_fps	5x5 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStrideS_fps
			|------	KerConv4x5from5x5StrideS_V_fps	5x5 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5StrideS_H_fps	5x5 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConvNxNStrideS_Border_fp	NxN convolution, stride S, Left, Right, Top and Bottom borders




	Input feature maps, Output feature maps and filter on half words

		KerConv3x3BorderStride1_fp
			|------	KerConv2x3from3x3Stride1_V_fp	3x3 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride1_H_fp	3x3 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStride2_fp
			|------	KerConv2x3from3x3Stride2_V_fp	3x3 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride2_H_fp	3x3 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStrideS_fp
			|------	KerConv2x3from3x3StrideS_V_fp	3x3 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3StrideS_H_fp	3x3 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride1_fp
			|------	KerConv4x5from5x5Stride1_V_fp	5x5 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride1_H_fp	5x5 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride2_fp
			|------	KerConv4x5from5x5Stride2_V_fp	5x5 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride2_H_fp	5x5 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStrideS_fp
			|------	KerConv4x5from5x5StrideS_V_fp	5x5 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5StrideS_H_fp	5x5 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConvNxNStrideS_Border_fps	NxN convolution, stride S, Left, Right, Top and Bottom borders
*/

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride1_V_fps(
        signed char * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
		V1 = *((v4s *) PtIn); PtIn += W;
	} else  { // == 0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride2_V_fps(
        signed char * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn, *PtOut = Out;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
	} else  { // == 0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut+=Wo;
		V0 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3StrideS_V_fps(
        signed char * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
                PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
	} else  { // == 0
                PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += (Stride-2)*W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
	}
}


static void __attribute__ ((noinline)) KerConv3x2from3x3Stride1_H_fps(
        signed char * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut<<Norm;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn++;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride2_H_fps(
        signed char * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*2-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut<<Norm;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=2;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3StrideS_H_fps(
        signed char * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
	int Stride,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*Stride-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut<<Norm;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=Stride;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride1_V_fps(
        signed char * __restrict__ In,
        int W, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
        signed char *PtIn, *PtOut;
        int Bottom, PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 2: // [0..4 x 0..4] => [0..3 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 3: // [0..4 x 0..4] => [0..2 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+0)); C0 = (v4s)(((int)C0)<<8);
			C1 = *((v4s*) (Filter + 1*5+0)); C1 = (v4s)(((int)C1)<<8);
			C2 = *((v4s*) (Filter + 2*5+0)); C2 = (v4s)(((int)C2)<<8);
			C3 = *((v4s*) (Filter + 3*5+0)); C3 = (v4s)(((int)C3)<<8);
			C4 = *((v4s*) (Filter + 4*5+0)); C4 = (v4s)(((int)C4)<<8);
			break;
	}
	if (PadT) {
		 PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else { // Ho_F==0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	V2 = *((v4s *) PtIn); PtIn += W;
	V3 = *((v4s *) PtIn); PtIn += W;
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc = gap8_sumdotp4(V4, C4, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc; PtOut+=Wo;
		V0 = V1; V1 = V2; V2 = V3; V3 = V4;
	}
	if (PadB && (Bottom=(Ho-Ho_L))) {
		int Acc = *PtOut<<Norm;
		PtIn -= 4*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc; PtOut+=Wo;
		if (Bottom>1) {
			Acc = *PtOut<<Norm;
			V0 = V1; V1 = V2; V2 = V3;
			Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
			*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		}
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride2_V_fps(
        signed char * __restrict__ In,
        int W, int H, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
        signed char *PtIn, *PtOut;
        int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [0..3 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			if (((W+PadL)&0x1)!=0) {
				C0 = (v4s)(((int)C0)<<8); C1 = (v4s)(((int)C1)<<8); C2 = (v4s)(((int)C2)<<8);
				C3 = (v4s)(((int)C3)<<8); C4 = (v4s)(((int)C4)<<8);
			}
			break;
	}
	if (PadT) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else { // Ho_F==0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	V2 = *((v4s *) PtIn); PtIn += W;
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc = gap8_sumdotp4(V4, C4, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc; PtOut+=Wo;
		V0 = V2; V1 = V3; V2 = V4;
	}
	if (PadB && ((Ho-Ho_L)==1)) {
		int Acc = *PtOut<<Norm;
		PtIn -= 3*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
		if (((H+PadTOrg)&0x1)==0) {
			V3 = *((v4s *) PtIn);
			Acc = gap8_sumdotp4(V3, C3, Acc);
		}
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc;
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5StrideS_V_fps(
        signed char * __restrict__ In,
        int W, int H, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
        signed char *PtIn, *PtOut;
        int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1:
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			if (((W+PadL)&0x1)!=0) {
				C0 = (v4s)(((int)C0)<<8); C1 = (v4s)(((int)C1)<<8); C2 = (v4s)(((int)C2)<<8);
				C3 = (v4s)(((int)C3)<<8); C4 = (v4s)(((int)C4)<<8);
			}
			break;
	}
	if (Ho_F) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else { // Ho_F==0
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += (Stride-4)*W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc = gap8_sumdotp4(V4, C4, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc; PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W;
	}
	if (PadB && ((Ho-Ho_L)==1)) {
		int Acc = *PtOut<<Norm;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
		if (((H+PadTOrg)&0x1)==0) {
			V3 = *((v4s *) PtIn);
			Acc = gap8_sumdotp4(V3, C3, Acc);
		}
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
		*PtOut = Acc;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride1_H_fps(
        signed char * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 0:
			C0 = *((v4s *) &Filter[2*5+0]);
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1:
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 2:
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 3:
			C0 = (v4s){0,0,0,0};
			C1 = *((v4s *) &Filter[0*5+0]);
			C2 = *((v4s *) &Filter[1*5+0]);
			C3 = *((v4s *) &Filter[2*5+0]); C4 = (v4s){0, Filter[0*5+4], Filter[1*5+4], Filter[2*5+4]};
			break;
	}
	V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
	for (int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut<<Norm;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn++;
		V4 = gap8_pack4(x0,x1,x2,x3);
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc = gap8_sumdotp4(V4, C4, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
		V0 = __builtin_shuffle(V0, (v4s)(int)V4, (v4s){1,2,3,4});
		V1 = __builtin_shuffle(V1, (v4s)(int)V4, (v4s){1,2,3,5});
		V2 = __builtin_shuffle(V2, (v4s)(int)V4, (v4s){1,2,3,6});
		V3 = __builtin_shuffle(V3, (v4s)(int)V4, (v4s){1,2,3,7});
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride2_H_fps(
        signed char * __restrict__ In,
        int W, int H, int PadL, int PadT,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
        int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*2-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 0:
			C0 = *((v4s *) &Filter[2*5+0]);
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1:
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 2:
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 3:
			C0 = (v4s){0,0,0,0};
			C1 = *((v4s *) &Filter[0*5+0]);
			C2 = *((v4s *) &Filter[1*5+0]);
			C3 = *((v4s *) &Filter[2*5+0]); C4 = (v4s){0, Filter[0*5+4], Filter[1*5+4], Filter[2*5+4]};
			break;
	}
	for (int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut<<Norm;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=(2-4);
		V4 = gap8_pack4(x0,x1,x2,x3);
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc += gap8_sumdotp4(V4, C4, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5StrideS_H_fps(
        signed char * __restrict__ In,
        int W, int H, int PadL, int PadT,
        int Wo, int Wo_F, int Wo_L,
	unsigned int Stride,
        unsigned int Norm,
        signed char * __restrict__ Out,
        signed char * __restrict__ Filter,
        int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*Stride-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 0:
			C0 = *((v4s *) &Filter[2*5+0]);
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1:
			if (((H+PadT)&0x1)==0) {
				C0 = *((v4s *) &Filter[0*5+0]);
				C1 = *((v4s *) &Filter[1*5+0]);
				C2 = *((v4s *) &Filter[2*5+0]);
				C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			} else {
				C0 = (v4s){0,0,0,0};
				C1 = *((v4s *) &Filter[0*5+0]);
				C2 = *((v4s *) &Filter[1*5+0]);
				C3 = *((v4s *) &Filter[2*5+0]); C4 = (v4s){0, Filter[0*5+4], Filter[1*5+4], Filter[2*5+4]};
			}
			break;
	}
	for (int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut<<Norm;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=(Stride-4);
		V4 = gap8_pack4(x0,x1,x2,x3);
		Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc); Acc = gap8_sumdotp4(V3, C3, Acc); Acc += gap8_sumdotp4(V4, C4, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtOut++;
	}
}

void __attribute__ ((noinline)) KerConvNxNStrideS_Border_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	int Norm
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh/2;
               	for (int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
        		for (int w=Wo_F; w<Wo_L; w++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for (int i=Fh_min; i<Fh_max; i++)
                                	for (int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
			}
			ht -= Stride; hb -= Stride;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
               	for (int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
        		for ( int w=Wo_F; w<Wo_L; w++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
			}
			hb -= Stride;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw/2;
               	for ( int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
        		for ( int h=Ho_F; h<Ho_L; h++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=0; i<Fh; i++)
                                	for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
			}
			wl -= Stride; wr -= Stride;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
               	for ( int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
        		for ( int h=Ho_F; h<Ho_L; h++) {
                       		int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=0; i<Fh; i++)
                                	for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
			}
			wr -= Stride;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
					wl -= Stride; wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                		for ( int w=Wo_L; w<Wo; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
					wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
        		for ( int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
					wl -= Stride; wr -= Stride;
				}
				hb -= Stride;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
        		for ( int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                		for ( int w=Wo_L; w<Wo; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride1_V_fp(
        short int * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short int * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2;
	v2s C0, C1, C2;
	short int *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((v2s*) (Filter + 0*3+0)); C1 = *((v2s*) (Filter + 1*3+0)); C2 = *((v2s*) (Filter + 2*3+0));
	} else {
		C0 = *((v2s*) (Filter + 0*3+1)); C1 = *((v2s*) (Filter + 1*3+1)); C2 = *((v2s*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0};
		V1 = *((v2s *) PtIn); PtIn += W;
	} else  { // == 0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v2s *) PtIn); PtIn += W;
		V1 = *((v2s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V2 = *((v2s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= 2*W;
		V0 = *((v2s *) PtIn); PtIn += W;
		V1 = *((v2s *) PtIn);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride2_V_fp(
        short int * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2;
	v2s C0, C1, C2;
	short int *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((v2s*) (Filter + 0*3+0)); C1 = *((v2s*) (Filter + 1*3+0)); C2 = *((v2s*) (Filter + 2*3+0));
	} else {
		C0 = *((v2s*) (Filter + 0*3+1)); C1 = *((v2s*) (Filter + 1*3+1)); C2 = *((v2s*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0};
	} else  { // == 0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v2s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V1 = *((v2s *) PtIn); PtIn += W; V2 = *((v2s *) PtIn); PtIn += W;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut+=Wo;
		V0 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= W;
		V0 = *((v2s *) PtIn); PtIn += W;
		V1 = *((v2s *) PtIn);;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3StrideS_V_fp(
        short int * __restrict__ In,
        int W, int PadTOrg,
        int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2;
	v2s C0, C1, C2;
	short int *PtIn, *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = *((v2s*) (Filter + 0*3+0)); C1 = *((v2s*) (Filter + 1*3+0)); C2 = *((v2s*) (Filter + 2*3+0));
	} else {
		C0 = *((v2s*) (Filter + 0*3+1)); C1 = *((v2s*) (Filter + 1*3+1)); C2 = *((v2s*) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0};
	} else  { // == 0
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v2s *) PtIn); PtIn += W;
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V1 = *((v2s *) PtIn); PtIn += W; V2 = *((v2s *) PtIn); PtIn += (Stride-2)*W;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut+=Wo;
		V0 = *((v2s *) PtIn); PtIn += W;
	}
	if (Bottom) {
		int Acc = *PtOut<<Norm;
		PtIn -= W;
		V0 = *((v2s *) PtIn); PtIn += W;
		V1 = *((v2s *) PtIn);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride1_H_fp(
        short int * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	v2s X, Y, V0, V1, V2;
	v2s C0, C1, C2;
	short int *PtIn = In+Wo_F*1-PadL;
	short int *PtOut = Out;

	if (FilterConf) {
		X = *((v2s *) &Filter[0*3+0]); Y = *((v2s *) &Filter[1*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap8_pack2(Filter[2], Filter[5]);
	} else {
		X = *((v2s *) &Filter[1*3+0]); Y = *((v2s *) &Filter[2*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap8_pack2(Filter[5], Filter[8]);
	}
	X = *((v2s *) (PtIn+0*W+0)); Y = *((v2s *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	PtIn += 2;
	for (int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, Acc = *PtOut<<Norm;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; PtIn++;
		V2 = gap8_pack2(x0,x1);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
		V0=V1; V1=V2;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride2_H_fp(
        short int * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	v2s X, Y, V0, V1, V2;
	v2s C0, C1, C2;
	short int *PtIn = In+Wo_F*2-PadL;
	short int *PtOut = Out;

	if (FilterConf) {
		X = *((v2s *) &Filter[0*3+0]); Y = *((v2s *) &Filter[1*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap8_pack2(Filter[2], Filter[5]);
	} else {
		X = *((v2s *) &Filter[1*3+0]); Y = *((v2s *) &Filter[2*3+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3}); C2 = gap8_pack2(Filter[5], Filter[8]);
	}
	X = *((v2s *) (PtIn+0*W+0)); Y = *((v2s *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	V0 = gap8_pack2(PtIn[0], PtIn[W]); PtIn++;
	for (int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut<<Norm;
		X = *((v2s *) (PtIn+0*W)); Y = *((v2s *) (PtIn+1*W)); V1 = __builtin_shuffle(X,Y,(v2s){0,2}); V2 = __builtin_shuffle(X,Y,(v2s){1,3}); PtIn+=2;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
		V0=V2;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3StrideS_H_fp(
        short int * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
	int Stride,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	int Fw = 3;
	v2s Cv0, Cv1;
	int C0, C1;
	short int *PtOut = Out;
	In += Wo_F*Stride-PadL;

	if (FilterConf) {
		Cv0 =  *((v2s *) &Filter[0*3+0]); C0 = Filter[0*3+2];
		Cv1 =  *((v2s *) &Filter[1*3+0]); C1 = Filter[1*3+2];
	} else {
		Cv0 =  *((v2s *) &Filter[1*3+0]); C0 = Filter[1*3+2];
		Cv1 =  *((v2s *) &Filter[2*3+0]); C1 = Filter[2*3+2];
	}
	for (int w=Wo_F; w<Wo_L; w++) {
                        short int *PtI = In + w*Stride;
                        v2s Iv0;
                        int I;
                        int Acc = *PtOut<<Norm;
#ifdef ASM_OPT
                        asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
                        Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc += I*C0;
                        asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
                        Acc = gap8_sumdotp2(Iv0, Cv1, Acc); Acc += I*C1;
                        Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                        asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtOut) : "r" (Acc), "i" (2) : "memory");
#else
                        Iv0 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
                        Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc += I*C0;
                        Iv0 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
                        Acc = gap8_sumdotp2(Iv0, Cv1, Acc); Acc += I*C1;
                        *PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
#endif
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride1_V_fp(
        short int * __restrict__ In,
        int W, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	short int *PtIn, *PtOut;
	int Bottom, PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			C0 = *((v2s*) (Filter + 0*5+2)); C1 = gap8_pack2(Filter[0*5+4], 0);
			C2 = *((v2s*) (Filter + 1*5+2)); C3 = gap8_pack2(Filter[1*5+4], 0);
			C4 = *((v2s*) (Filter + 2*5+2)); C5 = gap8_pack2(Filter[2*5+4], 0);
			C6 = *((v2s*) (Filter + 3*5+2)); C7 = gap8_pack2(Filter[3*5+4], 0);
			C8 = *((v2s*) (Filter + 4*5+2)); C9 = gap8_pack2(Filter[4*5+4], 0);
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4]
			C0 = *((v2s*) (Filter + 0*5+1)); C1 = *((v2s*) (Filter + 0*5+3));
			C2 = *((v2s*) (Filter + 1*5+1)); C3 = *((v2s*) (Filter + 1*5+3));
			C4 = *((v2s*) (Filter + 2*5+1)); C5 = *((v2s*) (Filter + 2*5+3));
			C6 = *((v2s*) (Filter + 3*5+1)); C7 = *((v2s*) (Filter + 3*5+3));
			C8 = *((v2s*) (Filter + 4*5+1)); C9 = *((v2s*) (Filter + 4*5+3));
			break;
		case 2: // [0..4 x 0..4] => [0..3 x 0..4]
			C0 = *((v2s*) (Filter + 0*5+0)); C1 = *((v2s*) (Filter + 0*5+2));
			C2 = *((v2s*) (Filter + 1*5+0)); C3 = *((v2s*) (Filter + 1*5+2));
			C4 = *((v2s*) (Filter + 2*5+0)); C5 = *((v2s*) (Filter + 2*5+2));
			C6 = *((v2s*) (Filter + 3*5+0)); C7 = *((v2s*) (Filter + 3*5+2));
			C8 = *((v2s*) (Filter + 4*5+0)); C9 = *((v2s*) (Filter + 4*5+2));
			break;
		case 3: // [0..4 x 0..4] => [0..2 x 0..4]
			C0 = gap8_pack2(0, Filter[0*5+0]); C1 = *((v2s*) (Filter + 0*5+1));
			C2 = gap8_pack2(0, Filter[1*5+0]); C3 = *((v2s*) (Filter + 1*5+1));
			C4 = gap8_pack2(0, Filter[2*5+0]); C5 = *((v2s*) (Filter + 2*5+1));
			C6 = gap8_pack2(0, Filter[3*5+0]); C7 = *((v2s*) (Filter + 3*5+1));
			C8 = gap8_pack2(0, Filter[4*5+0]); C9 = *((v2s*) (Filter + 4*5+1));
			break;
	}
	int Off0 = 2*(W-2), Off1 = 2*Wo;
	if (PadT) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0}; V1 = (v2s){0,0};
		V2 = (v2s){0,0}; V3 = (v2s){0,0};
	} else {
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
#ifdef ASM_OPT
		asm VOL("p.lw %0,%2(%1!)":"=r"(V0),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V1),"+r"(PtIn):"r"(Off0):"memory");
		asm VOL("p.lw %0,%2(%1!)":"=r"(V2),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V3),"+r"(PtIn):"r"(Off0):"memory");
#else
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
#endif
	}
#ifdef ASM_OPT
	asm VOL("p.lw %0,%2(%1!)":"=r"(V4),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V5),"+r"(PtIn):"r"(Off0):"memory");
	asm VOL("p.lw %0,%2(%1!)":"=r"(V6),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V7),"+r"(PtIn):"r"(Off0):"memory");
#else
	V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
	V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn); PtIn += (W-2);
#endif
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
#ifdef ASM_OPT1
		asm VOL("p.lw %0,%2(%1!)":"=r"(V8),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V9),"+r"(PtIn):"r"(Off0):"memory");
#else
		V8 = *((v2s *) PtIn); PtIn += 2; V9 = *((v2s *) PtIn); PtIn += (W-2);
#endif
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		Acc = gap8_sumdotp2(V8, C8, Acc); Acc = gap8_sumdotp2(V9, C9, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
#ifdef ASM_OPT
		asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtOut) : "r" (Acc), "r" (Off1) : "memory");
#else
		*PtOut = Acc; PtOut+=Wo;
#endif
		V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
	}
	if (PadB && (Bottom=(Ho-Ho_L))) {
		int Acc = *PtOut<<Norm;
		PtIn -= 4*W;
#ifdef ASM_OPT1
		asm VOL("p.lw %0,%2(%1!)":"=r"(V0),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V1),"+r"(PtIn):"r"(Off0):"memory");
		asm VOL("p.lw %0,%2(%1!)":"=r"(V2),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V3),"+r"(PtIn):"r"(Off0):"memory");
		asm VOL("p.lw %0,%2(%1!)":"=r"(V4),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V5),"+r"(PtIn):"r"(Off0):"memory");
		asm VOL("p.lw %0,%2(%1!)":"=r"(V6),"+r"(PtIn):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V7),"+r"(PtIn):"r"(Off0):"memory");
#else
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
		V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
		V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn); PtIn += (W-2);
#endif
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
#ifdef ASM_OPT
		asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtOut) : "r" (Acc), "r" (Off1) : "memory");
#else
		*PtOut = Acc; PtOut+=Wo;
#endif
		if (Bottom>1) {
			Acc = *PtOut<<Norm;
			V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7;
			Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
			Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
			Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
			*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
		}
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride2_V_fp(
        short int * __restrict__ In,
        int W, int H, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	short int *PtIn, *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			// PadL Should always be = 2 in this case
			C0 = *((v2s*) (Filter + 0*5+2)); C1 = gap8_pack2(Filter[0*5+4], 0);
			C2 = *((v2s*) (Filter + 1*5+2)); C3 = gap8_pack2(Filter[1*5+4], 0);
			C4 = *((v2s*) (Filter + 2*5+2)); C5 = gap8_pack2(Filter[2*5+4], 0);
			C6 = *((v2s*) (Filter + 3*5+2)); C7 = gap8_pack2(Filter[3*5+4], 0);
			C8 = *((v2s*) (Filter + 4*5+2)); C9 = gap8_pack2(Filter[4*5+4], 0);
			break;
		case 1:
			if (((W+PadL)&0x1)==0) {
				C0 = *((v2s*) (Filter + 0*5+0)); C1 = *((v2s*) (Filter + 0*5+2));
				C2 = *((v2s*) (Filter + 1*5+0)); C3 = *((v2s*) (Filter + 1*5+2));
				C4 = *((v2s*) (Filter + 2*5+0)); C5 = *((v2s*) (Filter + 2*5+2));
				C6 = *((v2s*) (Filter + 3*5+0)); C7 = *((v2s*) (Filter + 3*5+2));
				C8 = *((v2s*) (Filter + 4*5+0)); C9 = *((v2s*) (Filter + 4*5+2));
			} else {
				C0 = gap8_pack2(0, Filter[0*5+0]); C1 = *((v2s*) (Filter + 0*5+1));
				C2 = gap8_pack2(0, Filter[1*5+0]); C3 = *((v2s*) (Filter + 1*5+1));
				C4 = gap8_pack2(0, Filter[2*5+0]); C5 = *((v2s*) (Filter + 2*5+1));
				C6 = gap8_pack2(0, Filter[3*5+0]); C7 = *((v2s*) (Filter + 3*5+1));
				C8 = gap8_pack2(0, Filter[4*5+0]); C9 = *((v2s*) (Filter + 4*5+1));
			}
			break;
	}

	if (PadT) { // = 2 by construction and in this case Ho_F==1
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0}; V1 = (v2s){0,0};
		V2 = (v2s){0,0}; V3 = (v2s){0,0};
	} else { // PadT==0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
	}
	V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn); PtIn += (W-2);
		V8 = *((v2s *) PtIn); PtIn += 2; V9 = *((v2s *) PtIn); PtIn += (W-2);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		Acc = gap8_sumdotp2(V8, C8, Acc); Acc = gap8_sumdotp2(V9, C9, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut+=Wo;
                V0 = V4; V1 = V5; V2 = V6; V3 = V7; V4 = V8; V5 = V9;
	}
	if (PadB && ((Ho-Ho_L)==1)) {
		int PadTOrg = PadOrg[2];
		int Acc = *PtOut<<Norm;
		PtIn -= 3*W;
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
		V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		if (((H+PadTOrg)&0x1)==0) {
			V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn);
			Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		}
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5StrideS_V_fp(
        short int * __restrict__ In,
        int W, int H, v4s PadOrg, v4s Pad,
        int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)
{
	v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	short int *PtIn, *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 0: // [0..4 x 0..4] => [2..4 x 0..4]
			C0 = *((v2s*) (Filter + 0*5+2)); C1 = gap8_pack2(Filter[0*5+4], 0);
			C2 = *((v2s*) (Filter + 1*5+2)); C3 = gap8_pack2(Filter[1*5+4], 0);
			C4 = *((v2s*) (Filter + 2*5+2)); C5 = gap8_pack2(Filter[2*5+4], 0);
			C6 = *((v2s*) (Filter + 3*5+2)); C7 = gap8_pack2(Filter[3*5+4], 0);
			C8 = *((v2s*) (Filter + 4*5+2)); C9 = gap8_pack2(Filter[4*5+4], 0);
			break;
		case 1:
			if (((W+PadL)&0x1)==0) {
				C0 = *((v2s*) (Filter + 0*5+0)); C1 = *((v2s*) (Filter + 0*5+2));
				C2 = *((v2s*) (Filter + 1*5+0)); C3 = *((v2s*) (Filter + 1*5+2));
				C4 = *((v2s*) (Filter + 2*5+0)); C5 = *((v2s*) (Filter + 2*5+2));
				C6 = *((v2s*) (Filter + 3*5+0)); C7 = *((v2s*) (Filter + 3*5+2));
				C8 = *((v2s*) (Filter + 4*5+0)); C9 = *((v2s*) (Filter + 4*5+2));
			} else {
				C0 = gap8_pack2(0, Filter[0*5+0]); C1 = *((v2s*) (Filter + 0*5+1));
				C2 = gap8_pack2(0, Filter[1*5+0]); C3 = *((v2s*) (Filter + 1*5+1));
				C4 = gap8_pack2(0, Filter[2*5+0]); C5 = *((v2s*) (Filter + 2*5+1));
				C6 = gap8_pack2(0, Filter[3*5+0]); C7 = *((v2s*) (Filter + 3*5+1));
				C8 = gap8_pack2(0, Filter[4*5+0]); C9 = *((v2s*) (Filter + 4*5+1));
			}
			break;
	}

	if (PadT) { // = 2 by construction and in this case Ho_F==1
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v2s){0,0}; V1 = (v2s){0,0};
		V2 = (v2s){0,0}; V3 = (v2s){0,0};
	} else { // PadT==0
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
	}
	for (int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut<<Norm;
		V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
		V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn); PtIn += (W-2);
		V8 = *((v2s *) PtIn); PtIn += 2; V9 = *((v2s *) PtIn); PtIn += (Stride-5)*(W-2);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		Acc = gap8_sumdotp2(V8, C8, Acc); Acc = gap8_sumdotp2(V9, C9, Acc);
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut+=Wo;
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
	}
	if (PadB && ((Ho-Ho_L)==1)) {
		int PadTOrg = PadOrg[2];
		int Acc = *PtOut<<Norm;
		PtIn -= (Stride+2)*W;
		V0 = *((v2s *) PtIn); PtIn += 2; V1 = *((v2s *) PtIn); PtIn += (W-2);
		V2 = *((v2s *) PtIn); PtIn += 2; V3 = *((v2s *) PtIn); PtIn += (W-2);
		V4 = *((v2s *) PtIn); PtIn += 2; V5 = *((v2s *) PtIn); PtIn += (W-2);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
		Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
		Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc);
		if (((H+PadTOrg)&0x1)==0) {
			V6 = *((v2s *) PtIn); PtIn += 2; V7 = *((v2s *) PtIn);
			Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc);
		}
		*PtOut =  gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride1_H_fp(
        short int * __restrict__ In,
        int W, int PadL,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	v2s X, Y, V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	short int *PtIn = In+Wo_F*1-PadL;
	short int *PtOut = Out;

	switch (FilterConf) {
		case 0:
			X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap8_pack2(Filter[14], Filter[19]);
			C5 = gap8_pack2(Filter[20],0); C6 = gap8_pack2(Filter[21],0); C7 = gap8_pack2(Filter[22],0); C8 = gap8_pack2(Filter[23],0); C9 = gap8_pack2(Filter[24],0);
			break;
		case 1:
			X = *((v2s *) &Filter[1*5+0]); Y = *((v2s *) &Filter[2*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[1*5+2]); Y = *((v2s *) &Filter[2*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap8_pack2(Filter[9], Filter[14]);
			X = *((v2s *) &Filter[3*5+0]); Y = *((v2s *) &Filter[4*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[3*5+2]); Y = *((v2s *) &Filter[4*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap8_pack2(Filter[19], Filter[24]);
			break;
		case 2:
			X = *((v2s *) &Filter[0*5+0]); Y = *((v2s *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[0*5+2]); Y = *((v2s *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap8_pack2(Filter[4], Filter[9]);
			X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap8_pack2(Filter[14], Filter[19]);
			break;
		case 3:
			C0 = gap8_pack2(0, Filter[0]); C1 = gap8_pack2(0, Filter[1]); C2 = gap8_pack2(0, Filter[2]); C3 = gap8_pack2(0, Filter[3]); C4 = gap8_pack2(0, Filter[4]);
			X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
			C9 = gap8_pack2(Filter[9], Filter[14]);
			break;
	}
	X = *((v2s *) (PtIn+0*W+0)); Y = *((v2s *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((v2s *) (PtIn+0*W+2)); Y = *((v2s *) (PtIn+1*W+2)); V2 = __builtin_shuffle(X,Y,(v2s){0,2}); V3 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((v2s *) (PtIn+2*W+0)); Y = *((v2s *) (PtIn+3*W+0)); V5 = __builtin_shuffle(X,Y,(v2s){0,2}); V6 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((v2s *) (PtIn+2*W+2)); Y = *((v2s *) (PtIn+3*W+2)); V7 = __builtin_shuffle(X,Y,(v2s){0,2}); V8 = __builtin_shuffle(X,Y,(v2s){1,3});
	PtIn += 4;
	int Off0 = 2*W, Off1 = -6*W+2;
	for (int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3, Acc = *PtOut<<Norm;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn++;
		V4 = gap8_pack2(x0,x1); V9 = gap8_pack2(x2,x3);
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc); Acc = gap8_sumdotp2(V4, C4, Acc);
		Acc = gap8_sumdotp2(V5, C5, Acc); Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc); Acc = gap8_sumdotp2(V8, C8, Acc); Acc = gap8_sumdotp2(V9, C9, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
		V0=V1; V1=V2; V2=V3; V3=V4; V5=V6; V6=V7; V7=V8; V8=V9;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride2_H_fp(
        short int * __restrict__ In,
        int W, int H, int PadL, int PadT,
        int Wo, int Wo_F, int Wo_L,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	v2s X, Y, V0, V1, V2, V3, V4, V5, V6, V7, V8, V9;
	v2s C0, C1, C2, C3, C4, C5, C6, C7, C8, C9;
	short int *PtIn = In+Wo_F*2-PadL;
	short int *PtOut = Out;

	switch (FilterConf) {
		case 0: // PadT == 2 by construction
			X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
			X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
			C4 = gap8_pack2(Filter[14], Filter[19]);
			C5 = gap8_pack2(Filter[20],0); C6 = gap8_pack2(Filter[21],0); C7 = gap8_pack2(Filter[22],0); C8 = gap8_pack2(Filter[23],0); C9 = gap8_pack2(Filter[24],0);
			break;
		case 1:
			if (((H+PadT)&0x1)==0) {
				X = *((v2s *) &Filter[0*5+0]); Y = *((v2s *) &Filter[1*5+0]); C0 = __builtin_shuffle(X,Y,(v2s){0,2}); C1 = __builtin_shuffle(X,Y,(v2s){1,3});
				X = *((v2s *) &Filter[0*5+2]); Y = *((v2s *) &Filter[1*5+2]); C2 = __builtin_shuffle(X,Y,(v2s){0,2}); C3 = __builtin_shuffle(X,Y,(v2s){1,3});
				C4 = gap8_pack2(Filter[4], Filter[9]);
				X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
				X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
				C9 = gap8_pack2(Filter[14], Filter[19]);
			} else {
				C0 = gap8_pack2(0, Filter[0]); C1 = gap8_pack2(0, Filter[1]); C2 = gap8_pack2(0, Filter[2]); C3 = gap8_pack2(0, Filter[3]); C4 = gap8_pack2(0, Filter[4]);
				X = *((v2s *) &Filter[2*5+0]); Y = *((v2s *) &Filter[3*5+0]); C5 = __builtin_shuffle(X,Y,(v2s){0,2}); C6 = __builtin_shuffle(X,Y,(v2s){1,3});
				X = *((v2s *) &Filter[2*5+2]); Y = *((v2s *) &Filter[3*5+2]); C7 = __builtin_shuffle(X,Y,(v2s){0,2}); C8 = __builtin_shuffle(X,Y,(v2s){1,3});
				C9 = gap8_pack2(Filter[9], Filter[14]);
			}
			break;
	}
	X = *((v2s *) (PtIn+0*W+0)); Y = *((v2s *) (PtIn+1*W+0)); V0 = __builtin_shuffle(X,Y,(v2s){0,2}); V1 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((v2s *) (PtIn+0*W+2)); Y = *((v2s *) (PtIn+1*W+2)); V2 = __builtin_shuffle(X,Y,(v2s){0,2});
	X = *((v2s *) (PtIn+2*W+0)); Y = *((v2s *) (PtIn+3*W+0)); V5 = __builtin_shuffle(X,Y,(v2s){0,2}); V6 = __builtin_shuffle(X,Y,(v2s){1,3});
	X = *((v2s *) (PtIn+2*W+2)); Y = *((v2s *) (PtIn+3*W+2)); V7 = __builtin_shuffle(X,Y,(v2s){0,2});
	PtIn += 3;
	for (int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut<<Norm;
		X = *((v2s *) (PtIn+0*W+0)); Y = *((v2s *) (PtIn+1*W+0)); V3 = __builtin_shuffle(X,Y,(v2s){0,2}); V4 = __builtin_shuffle(X,Y,(v2s){1,3});
		X = *((v2s *) (PtIn+2*W+0)); Y = *((v2s *) (PtIn+3*W+0)); V8 = __builtin_shuffle(X,Y,(v2s){0,2}); V9 = __builtin_shuffle(X,Y,(v2s){1,3}); PtIn+=2;
		Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc); Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc); Acc = gap8_sumdotp2(V4, C4, Acc);
		Acc = gap8_sumdotp2(V5, C5, Acc); Acc = gap8_sumdotp2(V6, C6, Acc); Acc = gap8_sumdotp2(V7, C7, Acc); Acc = gap8_sumdotp2(V8, C8, Acc); Acc = gap8_sumdotp2(V9, C9, Acc);
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
		V0=V2; V1=V3; V2=V4; V5=V7; V6=V8; V7=V9;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5StrideS_H_fp(
        short int * __restrict__ In,
        int W, int H, int PadL, int PadT,
        int Wo, int Wo_F, int Wo_L,
	int Stride,
        unsigned int Norm,
        short int * __restrict__ Out,
        short * __restrict__ Filter,
	int FilterConf
	)

{
	int Fw=5;
	v2s Cv0, Cv1, Cv2, Cv3, Cv4, Cv5, Cv6, Cv7;
	int C0,C1,C2,C3;
	short int *PtOut = Out;

	In += Wo_F*Stride-PadL;
	switch (FilterConf) {
		case 0:
			Cv0 = *((v2s *)(Filter+ 2*5+0)); Cv1 = *((v2s *)(Filter+ 2*5+2)); C0 = Filter[2*5+4];
			Cv2 = *((v2s *)(Filter+ 3*5+0)); Cv3 = *((v2s *)(Filter+ 3*5+2)); C1 = Filter[3*5+4];
			Cv4 = *((v2s *)(Filter+ 4*5+0)); Cv5 = *((v2s *)(Filter+ 4*5+2)); C2 = Filter[4*5+4];
			Cv6 = (v2s) {0,0};               Cv7 = (v2s) {0,0};		  C3 = 0;
			break;
		case 1:
			if (((H+PadT)&0x1)==0) {
				Cv0 = *((v2s *)(Filter+ 0*5+0)); Cv1 = *((v2s *)(Filter+ 0*5+2)); C0 = Filter[0*5+4];
				Cv2 = *((v2s *)(Filter+ 1*5+0)); Cv3 = *((v2s *)(Filter+ 1*5+2)); C1 = Filter[1*5+4];
				Cv4 = *((v2s *)(Filter+ 2*5+0)); Cv5 = *((v2s *)(Filter+ 2*5+2)); C2 = Filter[2*5+4];
				Cv6 = *((v2s *)(Filter+ 3*5+0)); Cv7 = *((v2s *)(Filter+ 3*5+2)); C3 = Filter[3*5+4];
			} else {
				Cv0 = (v2s) {0,0};               Cv1 = (v2s) {0,0};		  C0 = 0;
				Cv2 = *((v2s *)(Filter+ 0*5+0)); Cv3 = *((v2s *)(Filter+ 0*5+2)); C1 = Filter[0*5+4];
				Cv4 = *((v2s *)(Filter+ 1*5+0)); Cv5 = *((v2s *)(Filter+ 1*5+2)); C2 = Filter[1*5+4];
				Cv6 = *((v2s *)(Filter+ 2*5+0)); Cv7 = *((v2s *)(Filter+ 2*5+2)); C3 = Filter[2*5+4];
			}
			break;
	}
	for (int w=Wo_F; w<Wo_L; w++) {
		short int *PtI = In + w*Stride;
		v2s Iv0, Iv1;
		int I;
		int Acc = *PtOut<<Norm;
#ifdef ASM_OPT
		asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
		Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc = gap8_sumdotp2(Iv1, Cv1, Acc); Acc += I*C0;
		asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
		Acc = gap8_sumdotp2(Iv0, Cv2, Acc); Acc = gap8_sumdotp2(Iv1, Cv3, Acc); Acc += I*C1;
		asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
		Acc = gap8_sumdotp2(Iv0, Cv4, Acc); Acc = gap8_sumdotp2(Iv1, Cv5, Acc); Acc += I*C2;
		asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
		Acc = gap8_sumdotp2(Iv0, Cv6, Acc); Acc = gap8_sumdotp2(Iv1, Cv7, Acc); Acc += I*C3;
		Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
		asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtOut) : "r" (Acc), "i" (2) : "memory");
#else
		Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc = gap8_sumdotp2(Iv1, Cv1, Acc); Acc += I*C0;
		Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		Acc = gap8_sumdotp2(Iv0, Cv2, Acc); Acc = gap8_sumdotp2(Iv1, Cv3, Acc); Acc += I*C1;
		Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		Acc = gap8_sumdotp2(Iv0, Cv4, Acc); Acc = gap8_sumdotp2(Iv1, Cv5, Acc); Acc += I*C2;
		Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
		Acc = gap8_sumdotp2(Iv0, Cv6, Acc); Acc = gap8_sumdotp2(Iv1, Cv7, Acc); Acc += I*C3;
		*PtOut = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtOut++;
#endif

	}
}



void __attribute__ ((noinline)) KerConvNxNStrideS_Border_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
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
	int Norm
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh/2;
               	for ( int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
        		for ( int w=Wo_F; w<Wo_L; w++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
			ht -= Stride; hb -= Stride;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
               	for ( int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
        		for ( int w=Wo_F; w<Wo_L; w++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
			hb -= Stride;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw/2;
               	for ( int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
        		for ( int h=Ho_F; h<Ho_L; h++) {
                        	int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=0; i<Fh; i++)
                                	for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
			wl -= Stride; wr -= Stride;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
               	for ( int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
        		for ( int h=Ho_F; h<Ho_L; h++) {
                       		int Acc = Out[Wo*h+w]<<Norm;
                        	for ( int i=0; i<Fh; i++)
                                	for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        	Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			}
			wr -= Stride;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
					wl -= Stride; wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                		for ( int w=Wo_L; w<Wo; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
					wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
        		for ( int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
					wl -= Stride; wr -= Stride;
				}
				hb -= Stride;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
        		for ( int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                		for ( int w=Wo_L; w<Wo; w++) {
                        		int Acc = Out[Wo*h+w]<<Norm;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fh*i+j];
                        		Out[Wo*h+w] = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

/* Wrapper functions */

static void __attribute__ ((noinline)) KerConv3x3BorderStride1_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3, Stride=1;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3Stride1_V_fp(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3Stride1_V_fp(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3Stride1_H_fp(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3Stride1_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride2_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3, Stride=2;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3Stride2_V_fp(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3Stride2_V_fp(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3Stride2_H_fp(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3Stride2_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStrideS_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3StrideS_V_fp(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3StrideS_V_fp(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3StrideS_H_fp(In, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3StrideS_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride1_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5, Stride=1;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==2)) {
		KerConv4x5from5x5Stride1_V_fp(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
		KerConv4x5from5x5Stride1_V_fp(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+1, Filter, 1);
	}
	if (PadR && ((Wo-Wo_L)==2)) {
		KerConv4x5from5x5Stride1_V_fp(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-2, Filter, 2);
		KerConv4x5from5x5Stride1_V_fp(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 3);
	}
	if (PadT && (Ho_F==2)) {
		KerConv5x4from5x5Stride1_H_fp(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
		KerConv5x4from5x5Stride1_H_fp(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F+Wo, Filter, 1);
	}
	if (PadB && ((Ho-Ho_L)==2)) {
		KerConv5x4from5x5Stride1_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 2);
		KerConv5x4from5x5Stride1_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+(Ho_L+1)*Wo+Wo_F, Filter, 3);
	}
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride2_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5, Stride=2;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv4x5from5x5Stride2_V_fp(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv4x5from5x5Stride2_V_fp(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);

	if (PadT && (Ho_F==1)) KerConv5x4from5x5Stride2_H_fp(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv5x4from5x5Stride2_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStrideS_fp(
        short int *__restrict__ In,
        short int *__restrict__ Out,
        short int *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv4x5from5x5StrideS_V_fp(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv4x5from5x5StrideS_V_fp(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out+Wo-1, Filter, 1);

	if (PadT && (Ho_F==1)) KerConv5x4from5x5StrideS_H_fp(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv5x4from5x5StrideS_H_fp(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride1_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3, Stride=1;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3Stride1_V_fps(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3Stride1_V_fps(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3Stride1_H_fps(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3Stride1_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride2_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3, Stride=2;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3Stride2_V_fps(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3Stride2_V_fps(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3Stride2_H_fps(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3Stride2_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStrideS_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=3, Fw=3;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv2x3from3x3StrideS_V_fps(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv2x3from3x3StrideS_V_fps(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out+Wo-1, Filter, 1);
	if (PadT && (Ho_F==1)) KerConv3x2from3x3StrideS_H_fps(In, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv3x2from3x3StrideS_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride1_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5, Stride=1;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==2)) {
		KerConv4x5from5x5Stride1_V_fps(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
		KerConv4x5from5x5Stride1_V_fps(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+1, Filter, 1);
	}
	if (PadR && ((Wo-Wo_L)==2)) {
		KerConv4x5from5x5Stride1_V_fps(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-2, Filter, 2);
		KerConv4x5from5x5Stride1_V_fps(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 3);
	}
	if (PadT && (Ho_F==2)) {
		KerConv5x4from5x5Stride1_H_fps(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
		KerConv5x4from5x5Stride1_H_fps(In, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F+Wo, Filter, 1);
	}
	if (PadB && ((Ho-Ho_L)==2)) {
		KerConv5x4from5x5Stride1_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 2);
		KerConv5x4from5x5Stride1_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Norm, Out+(Ho_L+1)*Wo+Wo_F, Filter, 3);
	}
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride2_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5, Stride=2;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv4x5from5x5Stride2_V_fps(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv4x5from5x5Stride2_V_fps(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Norm, Out+Wo-1, Filter, 1);

	if (PadT && (Ho_F==1)) KerConv5x4from5x5Stride2_H_fps(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv5x4from5x5Stride2_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStrideS_fps(
        signed char *__restrict__ In,
        signed char *__restrict__ Out,
        signed char *__restrict__ Filter,
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
        int Norm)

{
	int Fh=5, Fw=5;
        int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
        int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL && (Wo_F==1)) KerConv4x5from5x5StrideS_V_fps(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out, Filter, 0);
	if (PadR && ((Wo-Wo_L)==1)) KerConv4x5from5x5StrideS_V_fps(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Norm, Out+Wo-1, Filter, 1);

	if (PadT && (Ho_F==1)) KerConv5x4from5x5StrideS_H_fps(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Wo_F, Filter, 0);
	if (PadB && ((Ho-Ho_L)==1)) KerConv5x4from5x5StrideS_H_fps(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Norm, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

/* Convolution, body processing (covers both padded and non padded variants)

	Input feature maps, Output feature maps and filter on bytes

		KerConv1x1Stride1_Body_fps	1x1 convolution, stride 1
		KerConv1x1Stride2_Body_fps	1x1 convolution, stride 2
		KerConv1x1StrideS_Body_fps	1x1 convolution, stride S

		KerConv3x3Stride1_Body_fps	3x3 convolution, stride 1
		KerConv3x3Stride2_Body_fps	3x3 convolution, stride 2
		KerConv3x3StrideS_Body_fps	3x3 convolution, stride S

		KerConv5x5Stride1_Body_fps	5x5 convolution, stride 1
		KerConv5x5Stride2_Body_fps	5x5 convolution, stride 2
		KerConv5x5StrideS_Body_fps	5x5 convolution, stride S

		KerConvNxNStrideS_Body_fps	NxN convolution, stride S


	Input feature maps, Output feature maps and filter on half words

		KerConv1x1Stride1_Body_fp	1x1 convolution, stride 1
		KerConv1x1Stride2_Body_fp	1x1 convolution, stride 2
		KerConv1x1StrideS_Body_fp	1x1 convolution, stride S

		KerConv3x3Stride1_Body_fp	3x3 convolution, stride 1
		KerConv3x3Stride2_Body_fp	3x3 convolution, stride 2
		KerConv3x3StrideS_Body_fp	3x3 convolution, stride S

		KerConv5x5Stride1_Body_fp	5x5 convolution, stride 1
		KerConv5x5Stride2_Body_fp	5x5 convolution, stride 2
		KerConv5x5StrideS_Body_fp	5x5 convolution, stride S

		KerConvNxNStrideS_Body_fp	NxN convolution, stride S
*/

static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int C0 = Filter[0];
	int IterW = Wo_L-Wo_F;
	switch (IterW&0x3) {
		case 0: for ( int h=Ho_F; h<Ho_L; h++) {
				v4s *LineOut = (v4s *) (&Out[Wo*h+Wo_F]);
				v4s *LineIn = (v4s *) (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
				for ( int w=0; w<(IterW/4); w++) {
					v4s O = LineOut[w];
					v4s I = LineIn[w];
                                	int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm, Acc2 = O[2]<<Norm, Acc3 = O[3]<<Norm;
                                	Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, I[0], C0), Norm), 7);
                                	Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, I[1], C0), Norm), 7);
                                	Acc2 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc2, I[2], C0), Norm), 7);
                                	Acc3 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc3, I[3], C0), Norm), 7);
                                	LineOut[w] =  gap8_pack4(Acc0, Acc1, Acc2, Acc3);
				}
			}
			break;
		case 1: for ( int h=Ho_F; h<Ho_L; h++) {
				v4s O,I;
				v4s *LineOut = (v4s *) (&Out[Wo*h+Wo_F]);
				v4s *LineIn = (v4s *) (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
				for ( int w=0; w<(IterW/4); w++) {
					O = LineOut[w]; I = LineIn[w];
                                	int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm, Acc2 = O[2]<<Norm, Acc3 = O[3]<<Norm;
                                	Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, I[0], C0), Norm), 7);
                                	Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, I[1], C0), Norm), 7);
                                	Acc2 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc2, I[2], C0), Norm), 7);
                                	Acc3 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc3, I[3], C0), Norm), 7);
                                	LineOut[w] =  gap8_pack4(Acc0, Acc1, Acc2, Acc3);
				}
				O = LineOut[IterW/4]; I = LineIn[IterW/4];
                               	O[0] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[0]<<Norm, I[0], C0), Norm), 7);
				LineOut[IterW/4] = O;
			}
			break;
		case 2: for ( int h=Ho_F; h<Ho_L; h++) {
				v4s O,I;
				v4s *LineOut = (v4s *) (&Out[Wo*h+Wo_F]);
				v4s *LineIn = (v4s *) (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
				for ( int w=0; w<(IterW/4); w++) {
					O = LineOut[w]; I = LineIn[w];
                                	int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm, Acc2 = O[2]<<Norm, Acc3 = O[3]<<Norm;
                                	Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, I[0], C0), Norm), 7);
                                	Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, I[1], C0), Norm), 7);
                                	Acc2 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc2, I[2], C0), Norm), 7);
                                	Acc3 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc3, I[3], C0), Norm), 7);
                                	LineOut[w] =  gap8_pack4(Acc0, Acc1, Acc2, Acc3);
				}
				O = LineOut[IterW/4]; I = LineIn[IterW/4];
                               	O[0] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[0]<<Norm, I[0], C0), Norm), 7);
                               	O[1] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[1]<<Norm, I[1], C0), Norm), 7);
				LineOut[IterW/4] = O;
			}
			break;
		case 3: for ( int h=Ho_F; h<Ho_L; h++) {
				v4s O,I;
				v4s *LineOut = (v4s *) (&Out[Wo*h+Wo_F]);
				v4s *LineIn = (v4s *) (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
				for ( int w=0; w<(IterW/4); w++) {
					O = LineOut[w]; I = LineIn[w];
                                	int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm, Acc2 = O[2]<<Norm, Acc3 = O[3]<<Norm;
                                	Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, I[0], C0), Norm), 7);
                                	Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, I[1], C0), Norm), 7);
                                	Acc2 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc2, I[2], C0), Norm), 7);
                                	Acc3 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc3, I[3], C0), Norm), 7);
                                	LineOut[w] =  gap8_pack4(Acc0, Acc1, Acc2, Acc3);
				}
				O = LineOut[IterW/4]; I = LineIn[IterW/4];
                               	O[0] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[0]<<Norm, I[0], C0), Norm), 7);
                               	O[1] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[1]<<Norm, I[1], C0), Norm), 7);
                               	O[2] = gap8_clip(gap8_roundnorm_reg(gap8_macs(O[2]<<Norm, I[2], C0), Norm), 7);
				LineOut[IterW/4] = O;
			}
			break;
	}
}

static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int Stride = 2;
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO<<Norm;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			*PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtO++;
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}

static void __attribute__ ((noinline)) KerConv1x1StrideS_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	int Norm
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO<<Norm;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			*PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtO++;
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}


static void __attribute__ ((noinline)) KerConv3x3Stride1_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
        v4s V0, V1, V2;
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO<<Norm;
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
                        Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
                        Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
                        V0 = V1; V1 = V2;
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride2_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
        v4s V0, V1, V2;
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO<<Norm;
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
                        Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
                        Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
                        V0 = V2;
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3StrideS_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	int Norm
	)

{
        v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
        v4s V0, V1, V2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO<<Norm;
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+(Stride-2)*W);
                        Acc = gap8_sumdotp4(V0, C0, Acc); Acc = gap8_sumdotp4(V1, C1, Acc); Acc = gap8_sumdotp4(V2, C2, Acc);
                        Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride1_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v4s C0  = *((v4s *) &Filter[0]),
            C1  = *((v4s *) &Filter[5]),
            C2  = *((v4s *) &Filter[10]),
            C3  = *((v4s *) &Filter[15]),
            C4  = *((v4s *) &Filter[20]),
            C5 = gap8_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
            C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

        v4s V0, V1, V2, V3, V4, V5, V6;
        v4s Mask  = {1,2,3,4};
	register int N = Norm;

	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
                int x0,x1,x2,x3;
#ifdef ASM_OPT
                asm VOL("p.lw %0,%2(%1!)":"=r"(V0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lb %0,%2(%1!)":"=r"(x0),"+r"(PtI):"r"(W-4):"memory");
                asm VOL("p.lw %0,%2(%1!)":"=r"(V1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lb %0,%2(%1!)":"=r"(x1),"+r"(PtI):"r"(W-4):"memory");
                asm VOL("p.lw %0,%2(%1!)":"=r"(V2),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lb %0,%2(%1!)":"=r"(x2),"+r"(PtI):"r"(W-4):"memory");
                asm VOL("p.lw %0,%2(%1!)":"=r"(V3),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lb %0,%2(%1!)":"=r"(x3),"+r"(PtI):"r"(W-4):"memory");
#else
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
#endif
		V5 = gap8_pack4(x0,x1,x2,x3);

        	for ( int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO<<N;
#ifdef ASM_OPT
                	asm VOL("p.lw %0,%2(%1!)":"=r"(V4),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lb %0,%2(%1!)":"=r"(V6),"+r"(PtI):"r"(W-4):"memory");
#else
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
#endif
                        S = gap8_sumdotp4(V0,  C0,  S); S = gap8_sumdotp4(V1,  C1,  S);
                        S = gap8_sumdotp4(V2,  C2,  S); S = gap8_sumdotp4(V3,  C3,  S);
                        S = gap8_sumdotp4(V4,  C4,  S); S = gap8_sumdotp4(V5,  C5,  S); S = gap8_sumdotp4(V6,  C6,  S);
                        S =  gap8_clip(gap8_roundnorm_reg(S, N), 7);
                        V0 = V1; V1 = V2; V2 = V3; V3 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
#ifdef ASM_OPT
			asm VOL("p.sb %1,%2(%0!)"  : "+r" (PtO) : "r" (S), "r" (Wo) : "memory");
#else
			*PtO = S; PtO+=Wo;
#endif
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride2_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v4s C0  = *((v4s *) &Filter[0]),
            C1  = *((v4s *) &Filter[5]),
            C2  = *((v4s *) &Filter[10]),
            C3  = *((v4s *) &Filter[15]),
            C4  = *((v4s *) &Filter[20]),
            C5 = gap8_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
            C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

        v4s V0, V1, V2, V3, V4, V5, V6;
        v4s Mask  = {2,3,4,4};

	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
                int x0,x1,x2,x3;
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V5 = gap8_pack4(x0,x1,x2,0);
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO<<Norm;
			V3 = *PtI++; V5[3] = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
                        S = gap8_sumdotp4(V0,  C0,  S); S = gap8_sumdotp4(V1,  C1,  S);
                        S = gap8_sumdotp4(V2,  C2,  S); S = gap8_sumdotp4(V3,  C3,  S);
                        S = gap8_sumdotp4(V4,  C4,  S); S = gap8_sumdotp4(V5,  C5,  S); S = gap8_sumdotp4(V6,  C6,  S);
                        S =  gap8_clip(gap8_roundnorm_reg(S, Norm), 7);
                        V0 = V2; V1 = V3; V2 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5StrideS_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	int Norm
	)

{
        v4s C0  = *((v4s *) &Filter[0]),
            C1  = *((v4s *) &Filter[5]),
            C2  = *((v4s *) &Filter[10]),
            C3  = *((v4s *) &Filter[15]),
            C4  = *((v4s *) &Filter[20]),
            C5 = gap8_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
            C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

        v4s V0, V1, V2, V3, V4, V5, V6;
        v4s Mask  = {2,3,4,4};

	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
                int x0,x1,x2,x3;
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO<<Norm;
			V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4); V5 = gap8_pack4(x0,x1,x2,0);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+(Stride-4)*W-4);
                        S = gap8_sumdotp4(V0,  C0,  S); S = gap8_sumdotp4(V1,  C1,  S);
                        S = gap8_sumdotp4(V2,  C2,  S); S = gap8_sumdotp4(V3,  C3,  S);
                        S = gap8_sumdotp4(V4,  C4,  S); S = gap8_sumdotp4(V5,  C5,  S); S = gap8_sumdotp4(V6,  C6,  S);
                        S =  gap8_clip(gap8_roundnorm_reg(S, Norm), 7);
			V5 = __builtin_shuffle(V5, V6, Mask);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConvNxNStrideS_Body_fps(
	signed char *__restrict__ In,
	signed char *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	int Norm
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO<<Norm;
                        for ( int i=0; i<Fh; i++) {
                                for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fh*i+j];
                        }
                        *PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7); PtO++;
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}

static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int C0 = Filter[0];
	int IterW = Wo_L-Wo_F;
	if (IterW&0x1) {
        	for ( int h=Ho_F; h<Ho_L; h++) {
			v2s *LineOut = (v2s *) (&Out[Wo*h+Wo_F]);
			short int *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for ( int w=0; w<(IterW/2); w++) {
				v2s O = LineOut[w];
                                int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm;
                                Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, PtI[2*w  ], C0), Norm), 15);
                                Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, PtI[2*w+1], C0), Norm), 15);
                                LineOut[w] =  gap8_pack2(Acc0, Acc1);
			}
			Out[Wo*h+Wo_L-1] = gap8_clip(gap8_roundnorm_reg(gap8_macs((Out[Wo*h+Wo_L-1]<<Norm), PtI[IterW-1], C0), Norm), 15);
		}
	} else {
        	for ( int h=Ho_F; h<Ho_L; h++) {
			v2s *LineOut = (v2s *) (&Out[Wo*h+Wo_F]);
			short int *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for ( int w=0; w<(IterW/2); w++) {
				v2s O = LineOut[w];
                                int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm;
                                Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, PtI[2*w  ], C0), Norm), 15);
                                Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, PtI[2*w+1], C0), Norm), 15);
                                LineOut[w] =  gap8_pack2(Acc0, Acc1);
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int C0 = Filter[0];
	int IterW = Wo_L-Wo_F;
	if (IterW&0x1) {
        	for ( int h=Ho_F; h<Ho_L; h++) {
			v2s *LineOut = (v2s *) (&Out[Wo*h+Wo_F]);
			short int *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for ( int w=0; w<(IterW/2); w++) {
				v2s O = LineOut[w];
                                int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm;
                                Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, PtI[4*w  ], C0), Norm), 15);
                                Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, PtI[4*w+2], C0), Norm), 15);
                                LineOut[w] =  gap8_pack2(Acc0, Acc1);
			}
			Out[Wo*h+Wo_L-1] = gap8_clip(gap8_roundnorm_reg(gap8_macs((Out[Wo*h+Wo_L-1]<<Norm), PtI[2*(IterW-1)], C0), Norm), 15);
		}
	} else {
        	for ( int h=Ho_F; h<Ho_L; h++) {
			v2s *LineOut = (v2s *) (&Out[Wo*h+Wo_F]);
			short int *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
			for ( int w=0; w<(IterW/2); w++) {
				v2s O = LineOut[w];
                                int Acc0 = O[0]<<Norm, Acc1 = O[1]<<Norm;
                                Acc0 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc0, PtI[4*w  ], C0), Norm), 15);
                                Acc1 = gap8_clip(gap8_roundnorm_reg(gap8_macs(Acc1, PtI[4*w+2], C0), Norm), 15);
                                LineOut[w] =  gap8_pack2(Acc0, Acc1);
			}
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1StrideS_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
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
	int Norm
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	short int *PtO = Out+Wo*Ho_F+Wo_F;
	short int *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
		short int *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int I;
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
			asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*Stride):"memory");
			Acc += I*C0;
			Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "i" (2) : "memory");
#else
			I = *PtI; PtI+=Stride;
			Acc += I*C0;
			*PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtO++;
#endif
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}

static void __attribute__ ((noinline)) KerConv3x3Stride1_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v2s C0 = *((v2s *) &Filter[0]),            C1 = *((v2s *) &Filter[3]),
            C2 = gap8_pack2(Filter[2], Filter[5]), C3 = *((v2s *) &Filter[6]),
            C4 = gap8_pack2(Filter[8], 0);
        v2s V0, V1, V2, V3, V4;
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	short int *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-4;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v2s *PtI = (v2s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		short int *PtO = PtO1;
#ifdef ASM_OPT
                asm VOL("p.lw %0,%2(%1!)":"=r"(V0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(V2),"+r"(PtI):"r"(Off0):"memory");
                asm VOL("p.lw %0,%2(%1!)":"=r"(V1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(V3),"+r"(PtI):"r"(Off0):"memory"); V2 = __builtin_shuffle(V2, V3, (v2s){0,2});

#else
		V0 = *PtI++; V2 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2);
		V1 = *PtI++; V3 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2); V2 = __builtin_shuffle(V2, V3, (v2s){0,2});
#endif
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
                        asm VOL ("p.lw %0,%2(%1!)" : "=r" (V3), "+r" (PtI) : "i" (4) : "memory"); asm VOL ("p.lh %0,%2(%1!)" : "=r" (V4), "+r" (PtI) : "r" (Off0) : "memory");
#else
			V3 = *PtI++; V4 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2);
#endif
                        Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
                        Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
                        Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                        V0 = V1; V1 = V3;
                        V2 = __builtin_shuffle(V2, V4, (v2s) {1, 2});
#ifdef ASM_OPT
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "r" (2*Wo) : "memory");
#else
			*PtO = Acc; PtO+=Wo;
#endif
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride2_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v2s C0 = *((v2s *) &Filter[0]), C1 = gap8_pack2(Filter[2], 0),
            C2 = *((v2s *) &Filter[3]), C3 = gap8_pack2(Filter[5], 0),
            C4 = *((v2s *) &Filter[6]), C5 = gap8_pack2(Filter[8], 0);
        v2s V0, V1, V2, V3, V4, V5;
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	short int *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-4;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v2s *PtI = (v2s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		short int *PtO = PtO1;
#ifdef ASM_OPT
                asm VOL("p.lw %0,%2(%1!)":"=r"(V0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(V1),"+r"(PtI):"r"(Off0):"memory");
#else
		V0 = *PtI++; V1 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2);
#endif
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
                        asm VOL ("p.lw %0,%2(%1!)" : "=r" (V2), "+r" (PtI) : "i" (4) : "memory"); asm VOL ("p.lh %0,%2(%1!)" : "=r" (V3), "+r" (PtI) : "r" (Off0) : "memory");
                        asm VOL ("p.lw %0,%2(%1!)" : "=r" (V4), "+r" (PtI) : "i" (4) : "memory"); asm VOL ("p.lh %0,%2(%1!)" : "=r" (V5), "+r" (PtI) : "r" (Off0) : "memory");
#else
			V2 = *PtI++; V3 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2);
			V4 = *PtI++; V5 = *PtI; PtI = (v2s*) ((short int *)PtI+W-2);
#endif
                        Acc = gap8_sumdotp2(V0, C0, Acc); Acc = gap8_sumdotp2(V1, C1, Acc);
                        Acc = gap8_sumdotp2(V2, C2, Acc); Acc = gap8_sumdotp2(V3, C3, Acc);
                        Acc = gap8_sumdotp2(V4, C4, Acc); Acc = gap8_sumdotp2(V5, C5, Acc); Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
                        V0 = V4; V1 = V5;
#ifdef ASM_OPT
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "r" (2*Wo) : "memory");
#else
			*PtO = Acc; PtO+=Wo;
#endif
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3StrideS_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
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
	int Norm
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	v2s Cv0 = *((v2s *)(Filter+0)),
	    Cv1 = *((v2s *)(Filter+3)),
	    Cv2 = *((v2s *)(Filter+6));
	int C0 = Filter[2],
	    C1 = Filter[5],
	    C2 = Filter[8];
	int Fw = 3, Fh = 3;
	short int *PtO = Out+Wo*Ho_F+Wo_F;
	short int *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
        	for ( int w=Wo_F; w<Wo_L; w++) {
			short int *PtI = In + (h*Stride-PadT)*W + (w*Stride-PadL);
			v2s Iv0;
			int I;
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc += I*C0;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv1, Acc); Acc += I*C1;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv2, Acc); Acc += I*C2;
			Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "i" (2) : "memory");
#else
			Iv0 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc += I*C0;
			Iv0 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv1, Acc); Acc += I*C1;
			Iv0 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv2, Acc);Acc += I*C2;
			*PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtO++;
#endif
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}

static void __attribute__ ((noinline)) KerConv5x5Stride1_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
            C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
            C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
            C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
            C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
            C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
            C12 = gap8_pack2(Filter[24], 0);

        v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;
        v2s Mask  = {1,2};

	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	short int *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W - 8;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v2s *PtI = (v2s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		short int *PtO = PtO1;
                int X, Y;
#ifdef ASM_OPT
                asm("p.lw %0,%2(%1!)" : "=r" (V0), "+r" (PtI) : "i" (4) : ); asm("p.lw %0,%2(%1!)" : "=r" (V1), "+r" (PtI) : "i" (4) : ); asm("p.lh %0,%2(%1!)" : "=r" (X), "+r" (PtI) : "r" (Off0) : );
                asm("p.lw %0,%2(%1!)" : "=r" (V2), "+r" (PtI) : "i" (4) : ); asm("p.lw %0,%2(%1!)" : "=r" (V3), "+r" (PtI) : "i" (4) : ); asm("p.lh %0,%2(%1!)" : "=r" (Y), "+r" (PtI) : "r" (Off0) : ); V10 = gap8_pack2(X, Y);
                asm("p.lw %0,%2(%1!)" : "=r" (V4), "+r" (PtI) : "i" (4) : ); asm("p.lw %0,%2(%1!)" : "=r" (V5), "+r" (PtI) : "i" (4) : ); asm("p.lh %0,%2(%1!)" : "=r" (X), "+r" (PtI) : "r" (Off0) : );
                asm("p.lw %0,%2(%1!)" : "=r" (V6), "+r" (PtI) : "i" (4) : ); asm("p.lw %0,%2(%1!)" : "=r" (V7), "+r" (PtI) : "i" (4) : ); asm("p.lh %0,%2(%1!)" : "=r" (Y), "+r" (PtI) : "r" (Off0) : ); V11 = gap8_pack2(X, Y);
#else
		V0 = *PtI++; V1 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
		V2 = *PtI++; V3 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V10 = gap8_pack2(X, Y);
		V4 = *PtI++; V5 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
		V6 = *PtI++; V7 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V11 = gap8_pack2(X, Y);
#endif
        	for ( int h=Ho_F; h<Ho_L; h++) {
#ifdef ASM_OPT1
                        asm VOL("p.lw %0,%2(%1!)":"=r" (V8),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r" (V9),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r" (V12),"+r" (PtI):"r" (Off0):"memory");
#else
			V8 = *PtI++; V9 = *PtI++; V12 = *PtI; PtI = (v2s*) ((short int *)PtI+W-4);
#endif
			int S = *PtO<<Norm;
                        S = gap8_sumdotp2(V0,  C0,  S); S = gap8_sumdotp2(V1,  C1,  S); S = gap8_sumdotp2(V10, C10, S);
                        S = gap8_sumdotp2(V2,  C2,  S); S = gap8_sumdotp2(V3,  C3,  S);
                        S = gap8_sumdotp2(V4,  C4,  S); S = gap8_sumdotp2(V5,  C5,  S); S = gap8_sumdotp2(V11, C11, S);
                        S = gap8_sumdotp2(V6,  C6,  S); S = gap8_sumdotp2(V7,  C7,  S);
                        S = gap8_sumdotp2(V8,  C8,  S); S = gap8_sumdotp2(V9,  C9,  S); S = gap8_sumdotp2(V12, C12, S);
                        S =  gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
                        V0 = V2; V1 = V3; V2 = V4; V3 = V5; V4 = V6; V5 = V7; V6 = V8; V7 = V9;
                        V10 = __builtin_shuffle(V10, V11, Mask); V11 = __builtin_shuffle(V11, V12, Mask);
#ifdef ASM_OPT
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (S), "r" (2*Wo) : "memory");
#else
			*PtO = S; PtO+=Wo;
#endif
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride2_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Norm
	)

{
        v2s C0  = *((v2s *) &Filter[0]),            C1  = *((v2s *) &Filter[2]),
            C2  = *((v2s *) &Filter[5]),            C3  = *((v2s *) &Filter[7]),
            C4  = *((v2s *) &Filter[10]),           C5  = *((v2s *) &Filter[12]),
            C6  = *((v2s *) &Filter[15]),           C7  = *((v2s *) &Filter[17]),
            C8  = *((v2s *) &Filter[20]),           C9  = *((v2s *) &Filter[22]),
            C10 = gap8_pack2(Filter[4], Filter[9]), C11 = gap8_pack2(Filter[14], Filter[19]),
            C12 = gap8_pack2(Filter[24], 0);
        v2s V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12;

	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	short int *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off0 = 2*W-8;

       	for ( int w=Wo_F; w<Wo_L; w++) {
		v2s *PtI = (v2s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		short int *PtO = PtO1;
                int X, Y;
#ifdef ASM_OPT
                asm VOL("p.lw %0,%2(%1!)":"=r" (V0),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r" (V1),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lh %0,%2(%1!)" :"=r" (X),  "+r" (PtI):"r" (Off0):"memory");
                asm VOL("p.lw %0,%2(%1!)":"=r" (V2),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r" (V3),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lh %0,%2(%1!)" :"=r" (Y),  "+r" (PtI):"r" (Off0):"memory"); V10 = gap8_pack2(X, Y);
                asm VOL("p.lw %0,%2(%1!)":"=r" (V4),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r" (V5),"+r" (PtI):"i" (4):"memory"); asm VOL("p.lhu %0,%2(%1!)":"=r" (V11),"+r" (PtI):"r" (Off0):"memory");
#else
		V0 = *PtI++; V1 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4);
		V2 = *PtI++; V3 = *PtI++; Y = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V10 = gap8_pack2(X, Y);
		V4 = *PtI++; V5 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V11 = gap8_pack2(X, 0);
#endif
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO<<Norm;
#ifdef ASM_OPT1
                        asm VOL("p.lw %0,%2(%1!)":"=r"(V6),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V7),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(X),  "+r"(PtI):"r"(Off0):"memory"); V11 = gap8_pack2((int)V11, X);
                        asm VOL("p.lw %0,%2(%1!)":"=r"(V8),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(V9),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(V12),"+r"(PtI):"r"(Off0):"memory");
#else
			V6 = *PtI++; V7 = *PtI++; X = *((short int *) PtI); PtI = (v2s*) ((short int *)PtI+W-4); V11 = gap8_pack2((int)V11, X);
			V8 = *PtI++; V9 = *PtI++; V12 = *PtI;               PtI = (v2s*) ((short int *)PtI+W-4);
#endif
                        S = gap8_sumdotp2(V0, C0, S); S = gap8_sumdotp2(V1, C1, S); S = gap8_sumdotp2(V10, C10, S);
                        S = gap8_sumdotp2(V2, C2, S); S = gap8_sumdotp2(V3, C3, S);
                        S = gap8_sumdotp2(V4, C4, S); S = gap8_sumdotp2(V5, C5, S); S = gap8_sumdotp2(V11, C11, S);
                        S = gap8_sumdotp2(V6, C6, S); S = gap8_sumdotp2(V7, C7, S);
                        S = gap8_sumdotp2(V8, C8, S); S = gap8_sumdotp2(V9, C9, S); S = gap8_sumdotp2(V12, C12, S); S = gap8_clip(gap8_roundnorm_reg(S, Norm), 15);
			V10 = V11; V11 = V12; V0 = V4; V1 = V5; V2 = V6; V3 = V7; V4 = V8; V5 = V9;
#ifdef ASM_OPT
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (S), "r" (2*Wo) : "memory");
#else
			*PtO = S; PtO+=Wo;
#endif
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5StrideS_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
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
	int Norm
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	v2s Cv0 = *((v2s *)(Filter+ 0)), Cv1 = *((v2s *)(Filter+ 2)),
	    Cv2 = *((v2s *)(Filter+ 5)), Cv3 = *((v2s *)(Filter+ 7)),
	    Cv4 = *((v2s *)(Filter+10)), Cv5 = *((v2s *)(Filter+12)),
	    Cv6 = *((v2s *)(Filter+15)), Cv7 = *((v2s *)(Filter+17)),
	    Cv8 = *((v2s *)(Filter+20)), Cv9 = *((v2s *)(Filter+22));
	int C0 = Filter[ 4],
	    C1 = Filter[ 9],
	    C2 = Filter[14],
	    C3 = Filter[19],
	    C4 = Filter[24];
	int Fw = 5, Fh = 5;
	short int *PtO = Out+Wo*Ho_F+Wo_F;
	short int *PtC = Filter;

        for ( int h=Ho_F; h<Ho_L; h++) {
        	for ( int w=Wo_F; w<Wo_L; w++) {
			short int *PtI = In + (h*Stride-PadT)*W + (w*Stride-PadL);
			v2s Iv0, Iv1;
			int I;
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc = gap8_sumdotp2(Iv1, Cv1, Acc); Acc += I*C0;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv2, Acc); Acc = gap8_sumdotp2(Iv1, Cv3, Acc); Acc += I*C1;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv4, Acc); Acc = gap8_sumdotp2(Iv1, Cv5, Acc); Acc += I*C2;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv6, Acc); Acc = gap8_sumdotp2(Iv1, Cv7, Acc); Acc += I*C3;
			asm VOL("p.lw %0,%2(%1!)":"=r"(Iv0),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lw %0,%2(%1!)":"=r"(Iv1),"+r"(PtI):"i"(4):"memory"); asm VOL("p.lh %0,%2(%1!)":"=r"(I),"+r"(PtI):"r"(2*W-2*(Fw-1)):"memory");
			Acc = gap8_sumdotp2(Iv0, Cv8, Acc); Acc = gap8_sumdotp2(Iv1, Cv9, Acc); Acc += I*C4;
			Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "i" (2) : "memory");
#else
			Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv0, Acc); Acc = gap8_sumdotp2(Iv1, Cv1, Acc); Acc += I*C0;
			Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv2, Acc); Acc = gap8_sumdotp2(Iv1, Cv3, Acc); Acc += I*C1;
			Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv4, Acc); Acc = gap8_sumdotp2(Iv1, Cv5, Acc); Acc += I*C2;
			Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv6, Acc); Acc = gap8_sumdotp2(Iv1, Cv7, Acc); Acc += I*C3;
			Iv0 = *((v2s *) PtI); PtI+=2; Iv1 = *((v2s *) PtI); PtI+=2; I = *PtI; PtI+=W-(Fw-1);
			Acc = gap8_sumdotp2(Iv0, Cv8, Acc); Acc = gap8_sumdotp2(Iv1, Cv9, Acc); Acc += I*C4;
			*PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtO++;
#endif
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}

static void __attribute__ ((noinline)) KerConvNxNStrideS_Body_fp(
	short int *__restrict__ In,
	short int *__restrict__ Out,
	short int *__restrict__ Filter,
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
	int Norm
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	short int *PtO = Out+Wo*Ho_F+Wo_F;
	short int *PtC = Filter;
        for ( int h=Ho_F; h<Ho_L; h++) {
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO<<Norm;
#ifdef ASM_OPT
			short int *PtI = In + (h*Stride-PadT)*W +  (w*Stride-PadL);
                        for ( int i=0; i<Fh; i++) {
				int C, I;
                                for ( int j=0; j<(Fw/2); j++) {
					v2s Cv, Iv;
					asm VOL("p.lw %0,%2(%1!)" : "=r" (Cv), "+r" (PtC) : "i"   (4) : "memory"); asm VOL("p.lw %0,%2(%1!)" : "=r" (Iv), "+r" (PtI) : "i"   (4) : "memory");
					Acc = gap8_sumdotp2(Iv, Cv, Acc);
				}
				asm VOL("p.lh %0,%2(%1!)" : "=r" (C), "+r" (PtC) : "i"   (2) : "memory"); asm VOL("p.lh %0,%2(%1!)" : "=r" (I), "+r" (PtI) : "r"   (2*W-2*(Fw-1)) : "memory");
				Acc += I*C;
			}
			Acc = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15);
			asm VOL("p.sh %1,%2(%0!)"  : "+r" (PtO) : "r" (Acc), "i" (2) : "memory");
			PtC -= Fw*Fh;
#else
                        for ( int i=0; i<Fh; i++) {
                                for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fh*i+j];
                        }
                        *PtO = gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15); PtO++;
#endif
                }
		PtO = PtO + (Wo-Wo_L)+Wo_F;
        }
}


/*
	Optionally 0 padded convolutions.

	Input, output features and filters are half words (_fp) Dim=1,3,5,N, Stride=1,2,S

	Output feature maps are evaluated in parallel, one per core

	Argument data type: KerParConv_fp_T

	KerParConv1x1Stride1_fp
	KerParConv1x1Stride2_fp
	KerParConv1x1StrideS_fp

	KerParConv3x3Stride1_fp
		|------	KerConv3x3Stride1_Body_fp
		|------	KerConv3x3Stride1_Border_fp
	KerParConv3x3Stride2_fp
		|------	KerConv3x3Stride2_Body_fp
		|------	KerConv3x3Stride2_Border_fp
	KerParConv3x3StrideS_fp
		|------	KerConv3x3StrideS_Body_fp
		|------	KerConv3x3StrideS_Border_fp

	KerParConv5x5Stride1_fp
		|------	KerConv5x5Stride1_Body_fp
		|------	KerConv5x5Stride1_Border_fp
	KerParConv5x5Stride2_fp
		|------	KerConv5x5Stride2_Body_fp
		|------	KerConv5x5Stride2_Border_fp
	KerParConv5x5StrideS_fp
		|------	KerConv5x5StrideS_Body_fp
		|------	KerConv5x5StrideS_Border_fp

	KerParConvNxNStrideS_fp
		|------	KerConvNxNStrideS_Body_fp
		|------	KerConvNxNStrideS_Border_fp

*/
void KerParConv1x1Stride1_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=1, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1Stride1_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv1x1Stride2_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=1, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1Stride2_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv1x1StrideS_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1StrideS_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3Stride1_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=3, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3Stride1_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStride1_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3Stride2_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=3, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3Stride2_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStride2_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3StrideS_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3StrideS_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStrideS_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv5x5Stride1_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=5, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5Stride1_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStride1_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv5x5Stride2_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=5, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5Stride2_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStride2_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv5x5StrideS_fp(KerParConv_fp_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5StrideS_Body_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStrideS_fp(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConvNxNStrideS_fp(KerParConv_fp_T *Arg)

{
        unsigned int FS=Arg->N, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			short int *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConvNxNStrideS_Body_fp(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConvNxNStrideS_Border_fp(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are bytes (_fps) Dim=1,3,5,N, Stride=1,2,S

	Output feature maps are evaluated in parallel, one per core

	Argument data type: KerParConv_fps_T

	KerParConv1x1Stride1_fps
	KerParConv1x1Stride2_fps
	KerParConv1x1StrideS_fps

	KerParConv3x3Stride1_fps
		|------	KerConv3x3Stride1_Body_fps
		|------	KerConv3x3Stride1_Border_fps
	KerParConv3x3Stride2_fps
		|------	KerConv3x3Stride2_Body_fps
		|------	KerConv3x3Stride2_Border_fps
	KerParConv3x3StrideS_fps
		|------	KerConv3x3StrideS_Body_fps
		|------	KerConv3x3StrideS_Border_fps

	KerParConv5x5Stride1_fps
		|------	KerConv5x5Stride1_Body_fps
		|------	KerConv5x5Stride1_Border_fps
	KerParConv5x5Stride2_fps
		|------	KerConv5x5Stride2_Body_fps
		|------	KerConv5x5Stride2_Border_fps
	KerParConv5x5StrideS_fps
		|------	KerConv5x5StrideS_Body_fps
		|------	KerConv5x5StrideS_Border_fps

	KerParConvNxNStrideS_fps
		|------	KerConvNxNStrideS_Body_fps
		|------	KerConvNxNStrideS_Border_fps
*/

void KerParConv1x1Stride1_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=1, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1Stride1_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv1x1Stride2_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=1, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1Stride2_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv1x1StrideS_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv1x1StrideS_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3Stride1_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=3, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3Stride1_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStride1_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3Stride2_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=3, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3Stride2_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStride2_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv3x3StrideS_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv3x3StrideS_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConv3x3BorderStrideS_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}
void KerParConv5x5Stride1_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=5, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5Stride1_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStride1_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv5x5Stride2_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=5, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5Stride2_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStride2_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConv5x5StrideS_fps(KerParConv_fps_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConv5x5StrideS_Body_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConv5x5BorderStrideS_fps(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}

void KerParConvNxNStrideS_fps(KerParConv_fps_T *Arg)

{
        unsigned int FS=Arg->N, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        unsigned int TotalInFeatures, InFeatures = Arg->InFeatures;
        unsigned int OutFeatures = Arg->OutFeatures;
        unsigned int BaseOutFeature = 0, BaseInFeature = 0;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

	BaseInFeature = Arg->BaseInFeature; BaseOutFeature = Arg->BaseOutFeature; TotalInFeatures = Arg->TotalInFeatures;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        for (unsigned int of=First; of<Last; of++)
                for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + BaseInFeature + If), *out = Out+Wo*Ho*(BaseOutFeature + of);
			KerConvNxNStrideS_Body_fps(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, Norm);
			if ((int)PadIn) KerConvNxNStrideS_Border_fps(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, Norm);
                }
        gap8_waitbarrier(0);
}


/*
	Optionally 0 padded convolutions.

	Input, output features and filters are half words (_fp) Dim=1,3,5,N, Stride=1,2,S

	A single feature map is evaluated in parallel on all cores

	Argument data type: KerConv_fp_T

	KerConv1x1Stride1_fp
	KerConv1x1Stride2_fp
	KerConv1x1StrideS_fp

	KerConv3x3Stride1_fp
		|------	KerConv3x3Stride1_Body_fp
		|------	KerConv3x3Stride1_Border_fp
	KerConv3x3Stride2_fp
		|------	KerConv3x3Stride2_Body_fp
		|------	KerConv3x3Stride2_Border_fp
	KerConv3x3StrideS_fp
		|------	KerConv3x3StrideS_Body_fp
		|------	KerConv3x3StrideS_Border_fp

	KerConv5x5Stride1_fp
		|------	KerConv5x5Stride1_Body_fp
		|------	KerConv5x5Stride1_Border_fp
	KerConv5x5Stride2_fp
		|------	KerConv5x5Stride2_Body_fp
		|------	KerConv5x5Stride2_Border_fp
	KerConv5x5StrideS_fp
		|------	KerConv5x5StrideS_Body_fp
		|------	KerConv5x5StrideS_Border_fp

	KerConvNxNStrideS_fp
		|------	KerConvNxNStrideS_Body_fp
		|------	KerConvNxNStrideS_Border_fp

*/

void KerConv1x1Stride1_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=1, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1Stride1_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
        gap8_waitbarrier(0);
}

void KerConv1x1Stride2_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=1, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1Stride2_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
        gap8_waitbarrier(0);
}

void KerConv1x1StrideS_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=1, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1StrideS_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
        gap8_waitbarrier(0);
}


void KerConv3x3Stride1_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=3, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3Stride1_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStride1_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv3x3Stride2_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=3, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3Stride2_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStride2_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv3x3StrideS_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=3, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3StrideS_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStrideS_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5Stride1_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=5, S=1;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5Stride1_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStride1_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5Stride2_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=5, S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5Stride2_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStride2_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5StrideS_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=5, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5StrideS_Body_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStrideS_fp(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConvNxNStrideS_fp(KerConv_fp_T *Arg)

{
        unsigned int FS=Arg->N, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        short int * __restrict__ Filter = Arg->Filter;
        short int * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConvNxNStrideS_Body_fp(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConvNxNStrideS_Border_fp(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}




/*
	Optionally 0 padded convolutions.

	Input, output features and filters are bytes (_fps) Dim=1,3,5,N, Stride=1,2,S

	A single feature map is evaluated in parallel on all cores

	Argument data type: KerConv_fps_T

	KerConv1x1Stride1_fps
	KerConv1x1Stride2_fps
	KerConv1x1StrideS_fps

	KerConv3x3Stride1_fps
		|------	KerConv3x3Stride1_Body_fps
		|------	KerConv3x3Stride1_Border_fps
	KerConv3x3Stride2_fps
		|------	KerConv3x3Stride2_Body_fps
		|------	KerConv3x3Stride2_Border_fps
	KerConv3x3StrideS_fps
		|------	KerConv3x3StrideS_Body_fps
		|------	KerConv3x3StrideS_Border_fps

	KerConv5x5Stride1_fps
		|------	KerConv5x5Stride1_Body_fps
		|------	KerConv5x5Stride1_Border_fps
	KerConv5x5Stride2_fps
		|------	KerConv5x5Stride2_Body_fps
		|------	KerConv5x5Stride2_Border_fps
	KerConv5x5StrideS_fps
		|------	KerConv5x5StrideS_Body_fps
		|------	KerConv5x5StrideS_Border_fps

	KerConvNxNStrideS_fps
		|------	KerConvNxNStrideS_Body_fps
		|------	KerConvNxNStrideS_Border_fps
*/

void KerConv1x1Stride1_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=1, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1Stride1_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
        gap8_waitbarrier(0);
}

void KerConv1x1Stride2_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=1, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1Stride2_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
        gap8_waitbarrier(0);
}

void KerConv1x1StrideS_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=1, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
        }
	if (First<Last) KerConv1x1StrideS_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
        gap8_waitbarrier(0);
}


void KerConv3x3Stride1_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=3, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3Stride1_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStride1_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv3x3Stride2_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=3, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3Stride2_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStride2_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv3x3StrideS_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=3, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv3x3StrideS_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConv3x3BorderStrideS_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5Stride1_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=5, S=1;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5Stride1_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStride1_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5Stride2_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=5, S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5Stride2_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStride2_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConv5x5StrideS_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=5, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConv5x5StrideS_Body_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConv5x5BorderStrideS_fps(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}

void KerConvNxNStrideS_fps(KerConv_fps_T *Arg)

{
        unsigned int FS=Arg->N, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W;
        unsigned int H = Arg->H;
        signed char * __restrict__ Filter = Arg->Filter;
        signed char * __restrict__ Out = Arg->Out;
        unsigned int Norm = Arg->Norm;
        v4s PadIn = Arg->Pad;
        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        unsigned int CoreId = gap8_coreid();
	v4s PadOrg = PadIn;
	int Chunk, First, Last;

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
		KerConvNxNStrideS_Body_fps(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, Norm);
		if ((int)PadIn) KerConvNxNStrideS_Border_fps(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, Norm);
	}
        gap8_waitbarrier(0);
}



