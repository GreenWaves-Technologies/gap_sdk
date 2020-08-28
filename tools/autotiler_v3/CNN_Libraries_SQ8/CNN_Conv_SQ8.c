#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

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

static int __attribute__ ((always_inline)) MinCond(int a, int b)

{
#ifdef DIM_ALWAYS_GREATER_THAN_FILTER
	return a;
#else
	return Max(0, Min(a, b));
#endif
}

/* Padded Convolution Border processing

	Zero padding support. Implementation is based on partial convolutions derived from the original filter

	Input feature maps, Output feature maps and filter on bytes

		KerConv3x1BorderStrideNx1_SQ8
			|------ KerConv2x1from3x1StrideNx1_V_SQ8 3x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x3BorderStride1xN_SQ8
			|------ KerConv1x2from1x3Stride1xN_H_SQ8 1x3 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv3x3BorderStride1_SQ8
			|------	KerConv2x3from3x3Stride1_V_SQ8	3x3 convolution, stride 1, Left and right 0 padded stripes processing. 
			|------	KerConv3x2from3x3Stride1_H_SQ8	3x3 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStride2_SQ8
			|------	KerConv2x3from3x3Stride2_V_SQ8	3x3 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride2_H_SQ8	3x3 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStrideS_SQ8
			|------	KerConv2x3from3x3StrideS_V_SQ8	3x3 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3StrideS_H_SQ8	3x3 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConv5x1BorderStrideNx1_SQ8
			|------ KerConv4x1from5x1StrideNx1_V_SQ8 5x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x5BorderStride1xN_SQ8
			|------ KerConv1x4from1x5Stride1xN_H_SQ8 1x5 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv5x5BorderStride1_SQ8
			|------	KerConv4x5from5x5Stride1_V_SQ8	5x5 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride1_H_SQ8	5x5 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride2_SQ8
			|------	KerConv4x5from5x5Stride2_V_SQ8	5x5 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride2_H_SQ8	5x5 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStrideS_SQ8
			|------	KerConv4x5from5x5StrideS_V_SQ8	5x5 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5StrideS_H_SQ8	5x5 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConvNxNStrideS_Border_fp		NxN convolution, stride S, Left, Right, Top and Bottom borders

		KerConvNxMStrideSxSy_Border_fp		NxM convolution, stride Sx,Sy, Left, Right, Top and Bottom borders

		KerConvNxMDxDyStrideSxSy_Border_fp	NxM convolution, dilation Dx,Dy, stride Sx,Sy, Left, Right, Top and Bottom borders




	Input feature maps, Output feature maps and filter on half words

		KerConv3x1BorderStrideNx1_fp
			|------ KerConv2x1from3x1StrideNx1_V_fp 3x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x3BorderStride1xN_fp
			|------ KerConv1x2from1x3Stride1xN_H_fp 1x3 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv3x3BorderStride1_fp
			|------	KerConv2x3from3x3Stride1_V_fp	3x3 convolution, stride 1, Left and right 0 padded stripes processing. 
			|------	KerConv3x2from3x3Stride1_H_fp	3x3 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStride2_fp
			|------	KerConv2x3from3x3Stride2_V_fp	3x3 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3Stride2_H_fp	3x3 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv3x3BorderStrideS_fp
			|------	KerConv2x3from3x3StrideS_V_fp	3x3 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv3x2from3x3StrideS_H_fp	3x3 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConv5x1BorderStrideNx1_fp
			|------ KerConv4x1from5x1StrideNx1_V_fp 5x1 convolution, stride Nx1, Left and right 0 padded stripes processing.

		KerConv1x5BorderStride1xN_fp
			|------ KerConv1x4from1x5Stride1xN_H_fp 1x5 convolution, stride 1xN, Left and right 0 padded stripes processing.

		KerConv5x5BorderStride1_fp
			|------	KerConv4x5from5x5Stride1_V_fp	5x5 convolution, stride 1, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride1_H_fp	5x5 convolution, stride 1, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStride2_fp
			|------	KerConv4x5from5x5Stride2_V_fp	5x5 convolution, stride 2, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5Stride2_H_fp	5x5 convolution, stride 2, Top and bottom 0 padded stripes processing.

		KerConv5x5BorderStrideS_fp
			|------	KerConv4x5from5x5StrideS_V_fp	5x5 convolution, stride S, Left and right 0 padded stripes processing.
			|------	KerConv5x4from5x5StrideS_H_fp	5x5 convolution, stride S, Top and bottom 0 padded stripes processing.

		KerConvNxNStrideS_Border_SQ8		NxN convolution, stride S, Left, Right, Top and Bottom borders

		KerConvNxMStrideSxSy_Border_SQ8	NxM convolution, stride Sx,Sy, Left, Right, Top and Bottom borders

		KerConvNxMDxDyStrideSxSy_Border_SQ8	NxM convolution, dilation Dx,Dy, stride Sx,Sy, Left, Right, Top and Bottom borders
*/

static void __attribute__ ((noinline)) KerConv2x1from3x1StrideNx1_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	int V0,V1;
	int C0,C1;
	signed char *PtIn;
	int *PtOut;

	if (FilterConf) { /* Right Side */
		C0 = Filter[0]; C1 = Filter[1];
	} else { /* Left Side */
		C0 = Filter[1]; C1 = Filter[2];
	}
	PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V0 = *PtIn; V1 = *(PtIn+1); PtIn += W;
		Acc += V0*C0; Acc += V1*C1;
		*PtOut =  Acc; PtOut+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x2from1x3Stride1xN_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	int V0,V1;
	int C0,C1;
	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut = Out;

	if (FilterConf) { /* Bottom Side */
		C0 = Filter[0]; C1 = Filter[1];
	} else { /* Top Side */
		C0 = Filter[1]; C1 = Filter[2];
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		V0 = *(PtIn+0*W+0); V1 = *(PtIn+1*W+0); PtIn++;
		Acc += V0*C0; Acc += V1*C1;
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride1_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	int *PtOut;
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
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		*PtOut =  Acc; PtOut+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut =  Acc;
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride1_V_2Out_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	unsigned int NextFilter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C00, C01, C02, C10, C11, C12;
	signed char *PtIn;
	int *PtOut0, *PtOut1;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C00 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C01 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C02 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
		Filter += NextFilter;
		C10 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C11 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C12 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C00 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C01 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C02 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
		Filter += NextFilter;
		C10 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C11 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C12 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut0 = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
		V1 = *((v4s *) PtIn); PtIn += W;
	} else  { // == 0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut0 = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	PtOut1 = PtOut0+Wo*Ho;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc0 = *PtOut0, Acc1 = *PtOut1;
		V2 = *((v4s *) PtIn); PtIn += W;
		Acc0 = gap_sumdotp4(V0, C00, Acc0); Acc0 = gap_sumdotp4(V1, C01, Acc0); Acc0 = gap_sumdotp4(V2, C02, Acc0);
		Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1); Acc1 = gap_sumdotp4(V2, C12, Acc1);
		*PtOut0 =  Acc0; PtOut0+=Wo;
		*PtOut1 =  Acc1; PtOut1+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		int Acc0 = *PtOut0, Acc1 = *PtOut1;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);
		Acc0 = gap_sumdotp4(V0, C00, Acc0); Acc0 = gap_sumdotp4(V1, C01, Acc0);
		Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1);
		*PtOut0 =  Acc0;
		*PtOut1 =  Acc1;
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3Stride2_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	int *PtOut = Out;
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
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		*PtOut =  Acc; PtOut+=Wo;
		V0 = V2;
	}
	if (Bottom) {
		int Acc = *PtOut;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut =  Acc;
	}
}

static void __attribute__ ((noinline)) KerConv2x3from3x3StrideS_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	int *PtOut;
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
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += (Stride-2)*W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		*PtOut =  Acc; PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	if (Bottom) {
		int Acc = *PtOut;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut =  Acc;
	}
}


static void __attribute__ ((noinline)) KerConv3x2from3x3Stride1_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn++;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride1_H_2Out_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Ho, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	unsigned int NextFilter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C00, C01, C10, C11;
	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut0 = Out, *PtOut1 = PtOut0 + Wo*Ho;

	if (FilterConf) {
		C00 = *((v4s *) &Filter[0*3+0]); C01 = *((v4s *) &Filter[1*3+0]); C00[3] = 0; C01[3] = 0;
		Filter += NextFilter;
		C10 = *((v4s *) &Filter[0*3+0]); C11 = *((v4s *) &Filter[1*3+0]); C10[3] = 0; C11[3] = 0;
	} else {
		C00 = *((v4s *) &Filter[1*3+0]); C01 = *((v4s *) &Filter[2*3+0]); C00[3] = 0; C01[3] = 0;
		Filter += NextFilter;
		C10 = *((v4s *) &Filter[1*3+0]); C11 = *((v4s *) &Filter[2*3+0]); C10[3] = 0; C11[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc0 = *PtOut0, Acc1 = *PtOut1;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn++;
		Acc0 = gap_sumdotp4(V0, C00, Acc0); Acc0 = gap_sumdotp4(V1, C01, Acc0);
		Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1);
		*PtOut0 = Acc0; PtOut0++;
		*PtOut1 = Acc1; PtOut1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3Stride2_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*2-PadL;
	int *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=2;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv3x2from3x3StrideS_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Stride,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*Stride-PadL;
	int *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=Stride;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv3x1from4x1StrideNx1_V_SQ8(
	signed char * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0;
	v4s C0;
	signed char *PtIn;
	int *PtOut;

	switch (FilterConf) {
		case 1: // [0..3 x 0] => [1..3 x 0] => PadL==1
			C0 = *((v4s*) (Filter + 0*4+1)); C0[3] = 0;
			break;
		case 2: // [0..3 x 0] => [2..3 x 0] => PadL==2
			C0 = *((v4s*) (Filter + 0*4+2)); C0[2] = 0; C0[3] = 0;
			break;
		case 3: // [0..3 x 0] => [0..2 x 0] => PadR==1
			C0 = *((v4s*) (Filter + 0*4+0)); C0[3] = 0;
			break;
		case 4: // [0..3 x 0] => [0..1 x 0] => PadR==2
			C0 = *((v4s*) (Filter + 0*4+0)); C0[2] = 0; C0[3] = 0;
			break;
	}
	PtIn = In + (Ho_F*1-PadOrg[2])*W; PtOut = Out+Ho_F*Wo;
	V0 = * (v4s *) PtIn; PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		Acc = gap_sumdotp4(V0, C0, Acc);
		V0 = * (v4s *) PtIn; PtIn += W;
		*PtOut =  Acc; PtOut+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x3from1x4Stride1xN_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0;
	v4s C0;
	int x0,x1,x2,x3;
	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut = Out;

	switch (FilterConf) {
		case 1: // PadT==1
			C0 = *((v4s *) &Filter[1]); C0[3] = 0;
			break;
		case 2: // PadT==2
			C0 = *((v4s *) &Filter[2]); C0[2] = 0; C0[3] = 0;
			break;
		case 3: // PadB == 1
			C0 = *((v4s *) &Filter[0]); C0[3] = 0;
			break;
		case 4: // PadB == 2
			C0 = *((v4s *) &Filter[0]); C0[2] = 0; C0[3] = 0;
			break;
	}
	x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn++;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		Acc = gap_sumdotp4(V0, C0, Acc);
		x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn++;
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv4x1from5x1StrideNx1_V_SQ8(
	signed char * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0;
	v4s C0;
	signed char *PtIn;
	int *PtOut;

	switch (FilterConf) {
		case 2: // [0..4 x 0] => [2..4 x 0] => PadL==2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			break;
		case 1: // [0..4 x 0] => [1..4 x 0] => PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			break;
		case 3: // [0..4 x 0] => [0..3 x 0] => PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			break;
		case 4: // [0..4 x 0] => [0..2 x 0] => PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0 = (v4s)(((int)C0)<<8);
			break;
                case 5: // [0..4 x 0] => [0..2 x 0] PadR==2, Wo==1
                        C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
                        break;
	}
	PtIn = In + (Ho_F*1-PadOrg[2])*W; PtOut = Out+Ho_F*Wo;
	V0 = * (v4s *) PtIn; PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		Acc = gap_sumdotp4(V0, C0, Acc);
		V0 = * (v4s *) PtIn; PtIn += W;
		*PtOut =  Acc; PtOut+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x4from1x5Stride1xN_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0;
	v4s C0;
	int x0,x1,x2,x3;
	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT==2
			C0 = *((v4s *) &Filter[2]);  C0[3] = 0;
			break;
		case 1: // PadT==1
			C0 = *((v4s *) &Filter[1]);
			break;
		case 3: // PadB == 1
			C0 = *((v4s *) &Filter[0]);
			break;
		case 4: // PadB == 2
			C0 = *((v4s *) &Filter[0]); C0 = (v4s)((int)C0<<8);
			break;
                case 5: // PadB == 2, Ho == 1
                        C0 = *((v4s *) &Filter[0]);  C0[3] = 0;
                        break;
	}
	x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn++;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = *PtOut;
		Acc = gap_sumdotp4(V0, C0, Acc);
		x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn++;
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride1_V_SQ8(
	signed char * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	int *PtOut;
	int Bottom, PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL == 2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL == 1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR == 1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR == 2
			C0 = *((v4s*) (Filter + 0*5+0)); C0 = (v4s)(((int)C0)<<8);
			C1 = *((v4s*) (Filter + 1*5+0)); C1 = (v4s)(((int)C1)<<8);
			C2 = *((v4s*) (Filter + 2*5+0)); C2 = (v4s)(((int)C2)<<8);
			C3 = *((v4s*) (Filter + 3*5+0)); C3 = (v4s)(((int)C3)<<8);
			C4 = *((v4s*) (Filter + 4*5+0)); C4 = (v4s)(((int)C4)<<8);
			break;
                case 5: // [0..4 x 0..4] => [0..2 x 0..4] PadR == 2, Wo==1
                        C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
                        C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
                        C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
                        C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
                        C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
                        break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
		V1 = *((v4s *) PtIn); PtIn += W;
	} else { // Ho_F==0
		PtIn = In + (Ho_F*1-PadTOrg)*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	V2 = *((v4s *) PtIn); PtIn += W;
        if (Ho==1) {
                int Acc = *PtOut; Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); *PtOut = Acc;
                return;
        }
	V3 = *((v4s *) PtIn); PtIn += W;
	PtOut = Out+Ho_F*Wo;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut+=Wo;
		V0 = V1; V1 = V2; V2 = V3; V3 = V4;
	}
	if (PadB) {
		int Acc = *PtOut;
		PtIn -= 4*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc);
		*PtOut = Acc; PtOut+=Wo;
		if (PadB==2) {
			Acc = *PtOut;
			V0 = V1; V1 = V2; V2 = V3;
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			*PtOut =  Acc;
		}
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5Stride2_V_SQ8(
	signed char * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	int *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];
	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL == 2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = *((v4s *) PtIn); PtIn += W;
	} else {
		PtIn = In + (Ho_F*2-PadTOrg)*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	PtOut = Out+Ho_F*Wo;
	V2 = *((v4s *) PtIn); PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut+=Wo;
		V0 = V2; V1 = V3; V2 = V4;
	}
	if (PadB) {
		int Acc = *PtOut;
		PtIn -= 3*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		if (PadB==1) {
			V3 = *((v4s *) PtIn); Acc = gap_sumdotp4(V3, C3, Acc);
		}
		*PtOut = Acc;
	}
}

static void __attribute__ ((noinline)) KerConv4x5from5x5StrideS_V_SQ8(
	signed char * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride, 
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	int *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL==2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = *((v4s *) PtIn); PtIn += W;
	} else {
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	PtOut = Out+Ho_F*Wo;

	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = *PtOut;
		V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += (Stride-4)*W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W;
	}
	if (PadB) {
		int Acc = *PtOut;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		if (PadB==1) {
			V3 = *((v4s *) PtIn); Acc = gap_sumdotp4(V3, C3, Acc);
		}
		*PtOut = Acc;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride1_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*1-PadL;
	int *PtOut = Out;

	switch (FilterConf) {
		case 2:
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
		case 3:
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4:
			C0 = (v4s){0,0,0,0};
			C1 = *((v4s *) &Filter[0*5+0]);
			C2 = *((v4s *) &Filter[1*5+0]);
			C3 = *((v4s *) &Filter[2*5+0]); C4 = (v4s){0, Filter[0*5+4], Filter[1*5+4], Filter[2*5+4]};
			break;
                case 5: // PadB == 2, Ho == 1
                        C0 = *((v4s *) &Filter[0*5+0]);
                        C1 = *((v4s *) &Filter[1*5+0]);
                        C2 = *((v4s *) &Filter[2*5+0]);
                        C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
                        break;
	}
	V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn++;
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut++;
		V0 = __builtin_shuffle(V0, (v4s)(int)V4, (v4s){1,2,3,4});
		V1 = __builtin_shuffle(V1, (v4s)(int)V4, (v4s){1,2,3,5});
		V2 = __builtin_shuffle(V2, (v4s)(int)V4, (v4s){1,2,3,6});
		V3 = __builtin_shuffle(V3, (v4s)(int)V4, (v4s){1,2,3,7});
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5Stride2_H_SQ8(
	signed char * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*2-PadL;
	int *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT==2
			C0 = *((v4s *) &Filter[2*5+0]);
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1: // PadT==1
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 3: // PadB==1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4: // PadB==2
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
			break;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=(2-4);
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut++;
	}
}

static void __attribute__ ((noinline)) KerConv5x4from5x5StrideS_H_SQ8(
	signed char * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	unsigned int Stride,
	int * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*Stride-PadL;
	int *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT==2
			C0 = *((v4s *) &Filter[2*5+0]);
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1: // PadT==1
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 3: // PadB==1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4: // PadB==2
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = (v4s){0,0,0,0};
			C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
			break;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = *PtOut;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=((int)Stride-4);
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		*PtOut = Acc; PtOut++;
	}
}

void __attribute__ ((noinline)) KerConvNxNStrideS_Border_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
	       	for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Out[Wo*h+w];
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
			int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Out[Wo*h+w];
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
			int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Out[Wo*h+w];
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
			int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
		       		int Acc = Out[Wo*h+w];
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
					int Acc = Out[Wo*h+w];
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
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
					int Acc = Out[Wo*h+w];
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
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
					int Acc = Out[Wo*h+w];
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
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
					int Acc = Out[Wo*h+w];
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
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

void __attribute__ ((noinline)) KerConvNxMStrideSxSy_Border_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	int StrideX,
	int StrideY,
	v4s Pad,
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
	       	for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Out[Wo*h+w];
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
			int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Out[Wo*h+w];
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
			int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Out[Wo*h+w];
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
			int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
		       		int Acc = Out[Wo*h+w];
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
					int Acc = Out[Wo*h+w];
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
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
					int Acc = Out[Wo*h+w];
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
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
					int Acc = Out[Wo*h+w];
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
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
					int Acc = Out[Wo*h+w];
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
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

void __attribute__ ((noinline)) KerConvNxMDxDyStrideSxSy_Border_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	v4s PadOrg
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
	int Prec=10;
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
				int Acc = Out[Wo*h+w];
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
			int Fh_max = MinCond(hba, Fh);
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Out[Wo*h+w];
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
				int Acc = Out[Wo*h+w];
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
			int Wr_max = MinCond(wra, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Out[Wo*h+w];
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
					int Acc = Out[Wo*h+w];
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
					int Acc = Out[Wo*h+w];
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = MinCond(wra, Fw), Fh_min = hta;
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
					int Acc = Out[Wo*h+w];
					int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
					int Wl_min = wla, Fh_max = MinCond(hba, Fh);
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
					int Acc = Out[Wo*h+w];
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = MinCond(wra, Fw), Fh_max = MinCond(hba, Fh);
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

static void __attribute__ ((noinline)) KerConv3x1BorderStrideNx1_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x1from3x1StrideNx1_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out, Filter, 0);
	if (PadR) KerConv2x1from3x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv1x3BorderStride1xN_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) KerConv1x2from1x3Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 0);
	if (PadB) KerConv1x2from1x3Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride1_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=3, Fw=3, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride1_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride1_2Out_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	unsigned int NextFilter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg
	)

{
	int Fh=3, Fw=3, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride1_V_2Out_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out, Filter, NextFilter, 0);
	if (PadR) KerConv2x3from3x3Stride1_V_2Out_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, NextFilter, 1);
	if (PadT) KerConv3x2from3x3Stride1_H_2Out_SQ8(In, W, PadLOrg, Wo, Ho, Wo_F, Wo_L, Out+Wo_F, Filter, NextFilter, 0);
	if (PadB) KerConv3x2from3x3Stride1_H_2Out_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Ho, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, NextFilter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStride2_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=3, Fw=3, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride2_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3Stride2_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3Stride2_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3Stride2_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv3x3BorderStrideS_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=3, Fw=3;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3StrideS_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Out, Filter, 0);
	if (PadR) KerConv2x3from3x3StrideS_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Out+Wo-1, Filter, 1);
	if (PadT) KerConv3x2from3x3StrideS_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Out+Wo_F, Filter, 0);
	if (PadB) KerConv3x2from3x3StrideS_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Out+Ho_L*Wo+Wo_F, Filter, 1);
}

static void __attribute__ ((noinline)) KerConv4x1BorderStrideNx1_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	if (PadL) {
		if (Wo_F==2) {
			KerConv3x1from4x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, 2);
			KerConv3x1from4x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+1, Filter, 1);
		} else KerConv3x1from4x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, PadL);
	}
	if (PadR) {
		if ((Wo-Wo_L)==2) {
			KerConv3x1from4x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-2, Filter, 3);
			KerConv3x1from4x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 4);
		} else KerConv3x1from4x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, PadR+2);
	}
}

static void __attribute__ ((noinline)) KerConv1x4BorderStride1xN_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) {
		if (Ho_F==2) { // Happens only if stride = 1
			KerConv1x3from1x4Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 2);
			KerConv1x3from1x4Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F+Wo, Filter, 1);
		} else KerConv1x3from1x4Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, PadT);
	}
	if (PadB) {
		if ((Ho-Ho_L)==2) { // Happens only if stride == 1
			KerConv1x3from1x4Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 3);
			KerConv1x3from1x4Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4);
		} else KerConv1x3from1x4Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
	}
}

static void __attribute__ ((noinline)) KerConv5x1BorderStrideNx1_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	if (PadL) {
		if (Wo_F==2) {
			KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, 2);
			KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+1, Filter, 1);
		} else KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, PadL);
	}
	if (PadR) {
		if ((Wo-Wo_L)==2) {
			KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-2, Filter, 3);
			KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 4);
		} else if (Wo==1) KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 5);
		else KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, PadR+2);
	}
}

static void __attribute__ ((noinline)) KerConv1x5BorderStride1xN_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) {
		if (Ho_F==2) { // Happens only if stride = 1
			KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 2);
			KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F+Wo, Filter, 1);
		} else KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, PadT);
	}
	if (PadB) {
		if ((Ho-Ho_L)==2) { // Happens only if stride == 1
			KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 3);
			KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4);
		} else if (Ho==1) KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 5);
		else KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
	}
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride1_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=5, Fw=5, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL==2) {
		KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, 2);
		KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+1, Filter, 1);
	} else if (PadL==1) KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, 1);
	if (PadR==2) {
		if (Wo==1) KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 5);
		else {
			KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-2, Filter, 3);
			KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 4);
		}
	} else if (PadR==1) KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, 3);
	if (PadT==2) {
		KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 2);
		KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F+Wo, Filter, 1);
	} else if (PadT==1) KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, 1);
	if (PadB==2) {
		if (Ho==1) KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+(Ho_L)*Wo+Wo_F, Filter, 5);
		else {
			KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 3);
			KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4);
		}
	} else if (PadB==1) KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, 3);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStride2_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=5, Fw=5, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5Stride2_V_SQ8(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out, Filter, PadL);
	if (PadR) KerConv4x5from5x5Stride2_V_SQ8(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Out+Wo-1, Filter, PadR+2);

	if (PadT) KerConv5x4from5x5Stride2_H_SQ8(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Out+Wo_F, Filter, PadT);
	if (PadB) KerConv5x4from5x5Stride2_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
}

static void __attribute__ ((noinline)) KerConv5x5BorderStrideS_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s PadOrg
	)

{
	int Fh=5, Fw=5;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5StrideS_V_SQ8(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Out, Filter, PadL);
	if (PadR) KerConv4x5from5x5StrideS_V_SQ8(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Out+Wo-1, Filter, PadR+2);

	if (PadT) KerConv5x4from5x5StrideS_H_SQ8(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Out+Wo_F, Filter, PadT);
	if (PadB) KerConv5x4from5x5StrideS_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Out+Ho_L*Wo+Wo_F, Filter, PadB+2);
}

/* Convolution, body processing (covers both padded and non padded variants)

	Input feature maps, Output feature maps and filter on bytes

		KerConv1x1Stride1_Body_SQ8		1x1 convolution, stride 1
		KerConv1x1Stride2_Body_SQ8		1x1 convolution, stride 2
		KerConv1x1StrideS_Body_SQ8		1x1 convolution, stride S

		KerConv3x1Stride1x1_Body_SQ8		3x1 convolution, stride 1x1
		KerConv3x1Stride2x1_Body_SQ8		3x1 convolution, stride 2x1
		KerConv1x3Stride1x1_Body_SQ8		1x3 convolution, stride 1x1
		KerConv1x3Stride1x2_Body_SQ8		1x3 convolution, stride 1x2
		KerConv3x3Stride1_Body_SQ8		3x3 convolution, stride 1
		KerConv3x3Stride2_Body_SQ8		3x3 convolution, stride 2
		KerConv3x3StrideS_Body_SQ8		3x3 convolution, stride S

		KerConv5x1Stride1x1_Body_SQ8		5x1 convolution, stride 1x1
		KerConv5x1Stride2x1_Body_SQ8		5x1 convolution, stride 2x1
		KerConv1x5Stride1x1_Body_SQ8		1x5 convolution, stride 1x1
		KerConv1x5Stride1x2_Body_SQ8		1x5 convolution, stride 1x2
		KerConv5x5Stride1_Body_SQ8		5x5 convolution, stride 1
		KerConv5x5Stride2_Body_SQ8		5x5 convolution, stride 2
		KerConv5x5StrideS_Body_SQ8		5x5 convolution, stride S
		KerConv7x7StrideS_Body_SQ8		7x7 convolution, stride S

		KerConvNxNStrideS_Body_SQ8		NxN convolution, stride S
		KerConvNxMStrideSxSy_Body_SQ8	NxM convolution, stride Sx, Sy

		KerConvNxMDxDyStrideSxSy_Body_SQ8	NxM convolution, dilation Dx,Dy, stride Sx, Sy
*/

static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_4In_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	v4s C = *((v4s *) Filter);
	int IterW = Wo_L-Wo_F;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		int *LineOut = (&Out[Wo*h+Wo_F]);
		signed char *LineIn = (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
		for (unsigned int w=0; w<IterW; w++) {
			int O = LineOut[w];
			v4s V = gap_pack4(LineIn[w*Stride], LineIn[w*Stride+1*W*H], LineIn[w*Stride+2*W*H], LineIn[w*Stride+3*W*H]);
			O = gap_sumdotp4(V, C, O);
			LineOut[w] = O;
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_4In_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	v4s C = *((v4s *) Filter);
	int IterW = Wo_L-Wo_F;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		int *LineOut = (&Out[Wo*h+Wo_F]);
		signed char *LineIn = (In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL));
		for (unsigned int w=0; w<IterW; w++) {
			int O = LineOut[w];
			v4s V = gap_pack4(LineIn[w*Stride], LineIn[w*Stride+1*W*H], LineIn[w*Stride+2*W*H], LineIn[w*Stride+3*W*H]);
			O = gap_sumdotp4(V, C, O);
			LineOut[w] = O;
		}
	}
}

static void __attribute__ ((noinline)) KerConv1x1Stride1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
        int Stride = 1;
        int PadL = Pad[0], PadT = Pad[2];

        int C0 = Filter[0];
        int IterW = Wo_L-Wo_F;
        for (unsigned int h=Ho_F; h<Ho_L; h++) {
                int *LineOut = (int *) (&Out[Wo*h+Wo_F]);
                signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
                for (unsigned int w=0; w<(IterW/2); w++) {
                        int O0 = LineOut[2*w], O1 = LineOut[2*w+1];
                        O0 += PtI[2*w]*C0; O1 += PtI[2*w+1]*C0;
                        LineOut[2*w] = O0; LineOut[2*w+1] = O1;
                }
                if (IterW&0x1) Out[Wo*h+Wo_L-1] = Out[Wo*h+Wo_L-1] + PtI[IterW-1]*C0;
        }
}

static void __attribute__ ((noinline)) KerConv1x1Stride2_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int Stride = 2;
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	int *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv1x1StrideS_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s Pad
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	int *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConv3x1Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			Acc = gap_sumdotp4(V0, C0, Acc);
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x1Stride2x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	unsigned short int StrideX = 2;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			Acc = gap_sumdotp4(V0, C0, Acc);
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x3Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	v4s Mask = (v4s) {1,2,4,0};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0[1] = *PtI; PtI = ((signed char *)PtI+W);
		V0[2] = *PtI; PtI = ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			int X0 = *PtI; PtI = ((signed char *)PtI+W);
			V0 = __builtin_shuffle(V0, (v4s) X0, Mask);
			Acc = gap_sumdotp4(V0, C0, Acc);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x3Stride1x2_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	v4s Mask = (v4s) {2,4,5,0};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0[2] = *PtI; PtI = ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			unsigned int X0 = *(unsigned char *) PtI; PtI = ((signed char *)PtI+W);
			unsigned int X1 = *(unsigned char *) PtI; PtI = ((signed char *)PtI+W);
			X0 = X0 | (X1<<8);
			V0 = __builtin_shuffle(V0, (v4s) X0, Mask);
			Acc = gap_sumdotp4(V0, C0, Acc);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			V0 = V1; V1 = V2;
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride1_Body_2Out_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	unsigned int NextFilter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C00 = *((v4s *) &Filter[0]), C01 = *((v4s *) &Filter[3]), C02 = *((v4s *) &Filter[6]);
	v4s C10 = *((v4s *) &Filter[NextFilter+0]), C11 = *((v4s *) &Filter[NextFilter+3]), C12 = *((v4s *) &Filter[NextFilter+6]);
	v4s V0, V1, V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO0 = Out+Wo*Ho_F+Wo_F, *PtO1 = PtO0 + Wo*Ho;
	C00[3]=0; C01[3]=0; C02[3]=0;
	C10[3]=0; C11[3]=0; C12[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO_0 = PtO0, *PtO_1 = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc0 = *PtO_0, Acc1 = *PtO_1;
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc0 = gap_sumdotp4(V0, C00, Acc0); Acc0 = gap_sumdotp4(V1, C01, Acc0); Acc0 = gap_sumdotp4(V2, C02, Acc0);
			Acc1 = gap_sumdotp4(V0, C10, Acc1); Acc1 = gap_sumdotp4(V1, C11, Acc1); Acc1 = gap_sumdotp4(V2, C12, Acc1);
			V0 = V1; V1 = V2;
			*PtO_0 = Acc0; PtO_0+=Wo;
			*PtO_1 = Acc1; PtO_1+=Wo;
		}
		PtO0++; PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3Stride2_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			V0 = V2;
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv3x3StrideS_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s Pad
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *PtO = PtO1;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+(Stride-2)*W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			*PtO = Acc; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv4x1Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	v4s V0;
	int StrideX = 1;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	v4s Mask = (v4s) {1,2,3,4};

	for (int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		int *PtO = PtO1;
		v4s V0 = ((v4s *)PtI)[0];
		int x0 = PtI[4];
		PtI += 5;
		for (int w=Wo_F; w<Wo_L; w++) {
			int S = *PtO;
			S = gap_sumdotp4(V0,  C0,  S);
			V0 = __builtin_shuffle(V0, (v4s) x0, Mask); x0 = *PtI; PtI++;
			*PtO = S; PtO++;
		}
		PtO1+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv1x4Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	v4s Mask = (v4s) {1,2,3,4};
	v4s V0;
	int X0;

	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

    for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		V0[1] = *PtI; PtI = PtI+W;
		V0[2] = *PtI; PtI = PtI+W;
		V0[3] = *PtI; PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = *PtO;
			X0 = (signed char)(*PtI); PtI = PtI+W;
			V0 = __builtin_shuffle(V0, (v4s) X0, Mask);
			Acc = gap_sumdotp4(V0,  C0,  Acc);
			*PtO = Acc; PtO += Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x1Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s V0;
	int StrideX = 1;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	v4s Mask = (v4s) {1,2,3,4};

	for (int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		int *PtO = PtO1;
		v4s V0 = ((v4s *)PtI)[0];
		int x0 = PtI[4];
		PtI += 5;
		for (int w=Wo_F; w<Wo_L; w++) {
			int S = *PtO;
			S = gap_sumdotp4(V0,  C0,  S); S += x0*C1;
			V0 = __builtin_shuffle(V0, (v4s) x0, Mask); x0 = *PtI; PtI++;
			*PtO = S; PtO++;
		}
		PtO1+=Wo;
	}
}

static void __attribute__ ((noinline)) KerConv5x1Stride2x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s V0;
	int StrideX = 2;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		int x0;
		for (int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S += x0*C1;
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x5Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s Mask = (v4s) {1,2,3,4};
	v4s V0;
	signed char V1;

	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		int x0,x1,x2;
		x0 = *PtI; PtI = PtI+W;
		x1 = *PtI; PtI = PtI+W;
		x2 = *PtI; PtI = PtI+W;
		V0 = gap_pack4(0,x0,x1,x2);
		V1 = *PtI; PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V0 = __builtin_shuffle(V0, (v4s)((int) V1), Mask);
			V1 = (signed char)(*PtI); PtI = PtI+W;
			S = gap_sumdotp4(V0,  C0,  S); S += V1*C1;
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv1x5Stride1x2_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s Mask = (v4s) {2,3,4,5};
	v4s V0, V1;

	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		int *PtO = PtO1;
		int x0,x1;
		x0 = *PtI; PtI = PtI+W;
		x1 = *PtI; PtI = PtI+W;
		V0 = gap_pack4(0,0,x0,x1);
		V1 = (v4s) ((int) (*PtI)); PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			x0 = *PtI; PtI = PtI+W;
			V1[1] = x0; V0 = __builtin_shuffle(V0, (v4s)((int) V1), Mask);
			x1 = (*PtI); PtI = PtI+W; V1 = (v4s)((int)x1);
			S = gap_sumdotp4(V0,  C0,  S); S += x1*C1;
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {1,2,3,4};

	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *PtO = PtO1;
		int x0,x1,x2,x3;
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V5 = gap_pack4(x0,x1,x2,x3);

		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			V0 = V1; V1 = V2; V2 = V3; V3 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5Stride2_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {2,3,4,4};

	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *PtO = PtO1;
		int x0,x1,x2,x3;
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V5 = gap_pack4(x0,x1,x2,0);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V3 = *PtI++; V5[3] = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			V0 = V2; V1 = V3; V2 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv5x5StrideS_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s Pad
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {2,3,4,4};

	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *PtO = PtO1;
		int x0,x1,x2,x3;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4); V5 = gap_pack4(x0,x1,x2,x3);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+(Stride-4)*W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv7x7StrideS_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s Pad
	)

{
	v4s C0, C1;
	v4s V0, V1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off = (6 - Stride)*W;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *__restrict__ PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		int *__restrict__ PtO = PtO1;
		v4s *__restrict__ PtC = (v4s *) Filter;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = *PtO;
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI-Off-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3-49); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			*PtO = S; PtO+=Wo;
		}
		PtO1++;
	}
}

static void __attribute__ ((noinline)) KerConv13x1Stride1x1_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad
	)
{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s C1 = *((v4s *) &Filter[4]);
	v4s C2 = *((v4s *) &Filter[8]);
	signed char C3 = Filter[12];
	v4s V0;
	int StrideX = 1;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	int *PtO1 = Out+Wo*Ho_F+Wo_F;
	v4s Mask = (v4s) {1,2,3,4};

	for (int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		int *PtO = PtO1;
		v4s V0 = ((v4s *) PtI)[0];
		v4s V1 = ((v4s *) PtI)[4];
		v4s V2 = ((v4s *) PtI)[8];
		int V3 = PtI[12];
		PtI += 13;
		for (int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
			Acc = gap_sumdotp4(V0, C0, Acc);
			Acc = gap_sumdotp4(V1, C1, Acc);
			Acc = gap_sumdotp4(V2, C2, Acc);
			Acc += V3*C3;
			V0 = __builtin_shuffle(V0, V1, Mask);
			V1 = __builtin_shuffle(V1, V2, Mask);
			V2 = __builtin_shuffle(V2, (v4s) V3, Mask);
			V3 = *PtI; PtI++;
			*PtO = Acc; PtO++;
		}
		PtO1+=Wo;
	}
}
static void __attribute__ ((noinline)) KerConvNxNStrideS_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	v4s Pad
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fw*i+j];
			}
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConvNxMStrideSxSy_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
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
	int StrideX,
	int StrideY,
	v4s Pad
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL+j)]*Filter[Fw*i+j];
			}
			*PtO = Acc; PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static void __attribute__ ((noinline)) KerConvNxMDxDyStrideSxSy_Body_SQ8(
	signed char *__restrict__ In,
	int *__restrict__ Out,
	signed char *__restrict__ Filter,
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
	v4s Pad
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = *PtO;
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

	Input, output features and filters are bytes (_SQ8) Dim=1,3,5,N, Stride=1,2,S

	Output feature maps are evaluated in parallel, one per core

	Argument data type: KerConv_SQ8

	KerParConv1x1Stride1_SQ8
	KerParConv1x1Stride2_SQ8
	KerParConv1x1StrideS_SQ8

	KerParConv3x1Stride1x1_SQ8
		|------	KerConv3x1Stride1x1_Body_SQ8
		|------	KerConv3x1StrideNx1_Border_SQ8
	KerParConv3x1Stride2x1_SQ8
		|------	KerConv3x1Stride2x1_Body_SQ8
		|------	KerConv3x1StrideNx1_Border_SQ8
	KerParConv1x3Stride1x1_SQ8
		|------	KerConv1x3Stride1x1_Body_SQ8
		|------	KerConv1x3Stride1xN_Border_SQ8
	KerParConv1x3Stride1x2_SQ8
		|------	KerConv1x3Stride1x2_Body_SQ8
		|------	KerConv1x3Stride1xN_Border_SQ8
	KerParConv3x3Stride1_SQ8
		|------	KerConv3x3Stride1_Body_SQ8
		|------	KerConv3x3Stride1_Border_SQ8
	KerParConv3x3Stride2_SQ8
		|------	KerConv3x3Stride2_Body_SQ8
		|------	KerConv3x3Stride2_Border_SQ8
	KerParConv3x3StrideS_SQ8
		|------	KerConv3x3StrideS_Body_SQ8
		|------	KerConv3x3StrideS_Border_SQ8

	KerParConv4x1Stride1x1_SQ8
		|------ KerConv4x1Stride1x1_Body_SQ8
		|------ KerConv4x1BorderStrideNx1_SQ8
	KerParConv1x4Stride1x1_SQ8
		|------ KerConv1x4Stride1x1_Body_SQ8
		|------ KerConv1x4BorderStrideNx1_SQ8
	KerParConv5x1Stride1x1_SQ8
		|------	KerConv5x1Stride1x1_Body_SQ8
		|------	KerConv5x1StrideNx1_Border_SQ8
	KerParConv5x1Stride2x1_SQ8
		|------	KerConv5x1Stride2x1_Body_SQ8
		|------	KerConv5x1StrideNx1_Border_SQ8
	KerParConv1x5Stride1x1_SQ8
		|------	KerConv1x5Stride1x1_Body_SQ8
		|------	KerConv1x5Stride1xN_Border_SQ8
	KerParConv1x5Stride1x2_SQ8
		|------	KerConv1x5Stride1x2_Body_SQ8
		|------	KerConv1x5Stride1xN_Border_SQ8
	KerParConv5x5Stride1_SQ8
		|------	KerConv5x5Stride1_Body_SQ8
		|------	KerConv5x5Stride1_Border_SQ8
	KerParConv5x5Stride2_SQ8
		|------	KerConv5x5Stride2_Body_SQ8
		|------	KerConv5x5Stride2_Border_SQ8
	KerParConv5x5StrideS_SQ8
		|------	KerConv5x5StrideS_Body_SQ8
		|------	KerConv5x5StrideS_Border_SQ8
	KerParConv7x7StrideS_SQ8
		|------	KerConv7x7StrideS_Body_SQ8
		|------	KerConvNxNStrideS_Border_SQ8

	KerParConvNxNStrideS_SQ8
		|------	KerConvNxNStrideS_Body_SQ8
		|------	KerConvNxNStrideS_Border_SQ8

	KerParConvNxMStrideSxSy_SQ8
		|------	KerConvNxMStrideSxSy_Body_SQ8
		|------	KerConvNxMStrideSxSy_Border_SQ8

	KerParConvNxMDxDyStrideSxSy_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Body_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Border_SQ8
*/

void KerParConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) {
		for (unsigned int If=0; If<(InFeatures/4); If++) {
			signed char *in = In+W*H*4*If, *filter = Filter+FS*FS*(TotalInFeatures*of + 4*If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x1Stride1_Body_4In_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
		}
		if (InFeatures&0x3) {
			for (unsigned int If=4*(InFeatures/4); If<InFeatures; If++) {
				signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
				int *out = Out+Wo*Ho*(of);
				KerConv1x1Stride1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			}
		}
	}
	gap_waitbarrier(0);
}

void KerParConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) {
		for (unsigned int If=0; If<(InFeatures/4); If++) {
			signed char *in = In+W*H*4*If, *filter = Filter+FS*FS*(TotalInFeatures*of + 4*If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x1Stride2_Body_4In_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
		}
		if (InFeatures&0x3) {
			for (unsigned int If=4*(InFeatures/4); If<InFeatures; If++) {
				signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
				int *out = Out+Wo*Ho*(of);
				KerConv1x1Stride2_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			}
		}
	}
	gap_waitbarrier(0);
}

void KerParConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x1StrideS_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=3, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x1Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv3x1BorderStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=3, FSy=1, Sx=2, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x1Stride2x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv3x1BorderStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x3Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv1x3BorderStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x3Stride1x2_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv1x3BorderStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
	unsigned int Iter = Max(0, Last-First);
	for (unsigned int i=0; i<Iter/2; i++) {
		unsigned int of = First + 2*i;
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x3Stride1_Body_2Out_SQ8(in, out, filter, TotalInFeatures*FS*FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv3x3BorderStride1_2Out_SQ8(in, out, filter, TotalInFeatures*FS*FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
		}
	}
	if (Iter&0x1) {
		unsigned int of = Last-1;
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x3Stride1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv3x3BorderStride1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
		}
	}
	gap_waitbarrier(0);
}

void KerParConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x3Stride2_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv3x3BorderStride2_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv3x3StrideS_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn);
			if ((int)PadIn) KerConv3x3BorderStrideS_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv4x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=4, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv4x1Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv4x1BorderStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv1x4Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, FSy=4, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x4Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv1x4BorderStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=5, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv5x1Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv5x1BorderStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=5, FSy=1, Sx=2, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv5x1Stride2x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv5x1BorderStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x5Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv1x5BorderStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv1x5Stride1x2_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv1x5BorderStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv5x5Stride1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv5x5BorderStride1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv5x5Stride2_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConv5x5BorderStride2_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv5x5StrideS_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn);
			if ((int)PadIn) KerConv5x5BorderStrideS_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=7, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv7x7StrideS_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn);
			if ((int)PadIn) KerConvNxNStrideS_Border_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConv13x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)
{
	unsigned int FSx=13, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConv13x1Stride1x1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn);
			if ((int)PadIn) KerConvNxMStrideSxSy_Border_SQ8(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=Arg->N, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConvNxNStrideS_Body_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn);
			if ((int)PadIn) KerConvNxNStrideS_Border_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;
		
	for (unsigned int of=First; of<Last; of++) 
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of + If);
			int *out = Out+Wo*Ho*(of);
			KerConvNxMStrideSxSy_Body_SQ8(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn);
			if ((int)PadIn) KerConvNxMStrideSxSy_Border_SQ8(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

void KerParConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	int TotalInFeatures = Arg->TotalInFeatures;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-(Dx*(FSx-1)+1)+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput((Dx*(FSx-1)+1), PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, (Dx*(FSx-1)+1), PadIn[0], Sx));
	int Ho = (Arg->UsedH-(Dy*(FSy-1)+1)+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput((Dy*(FSy-1)+1), PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, (Dy*(FSy-1)+1), PadIn[2], Sy));

	unsigned int InFeatures = Arg->InFeatures;

	for (unsigned int of=First; of<Last; of++)
		for (unsigned int If=0; If<InFeatures; If++) {
			signed char *in = In+W*H*If, *filter = Filter+FSx*FSy*(TotalInFeatures*of  + If);
			int *out = Out+Wo*Ho*(of);
			KerConvNxMDxDyStrideSxSy_Body_SQ8(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn);
			if ((int)PadIn) KerConvNxMDxDyStrideSxSy_Border_SQ8(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn);
		}
	gap_waitbarrier(0);
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are bytes (_SQ8) Dim=1,3,5,N, Stride=1,2,S

	A single feature map is evaluated in parallel on all cores

	Argument data type: KerConv_SQ8

	KerConv1x1Stride1_SQ8
	KerConv1x1Stride2_SQ8
	KerConv1x1StrideS_SQ8

	KerConv3x1Stride1x1_SQ8
		|------	KerConv3x1Stride1x1_Body_SQ8
		|------	KerConv3x1StrideNx1_Border_SQ8
	KerConv3x1Stride2x1_SQ8
		|------	KerConv3x1Stride2x1_Body_SQ8
		|------	KerConv3x1StrideNx1_Border_SQ8
	KerConv1x3Stride1x1_SQ8
		|------	KerConv1x3Stride1x1_Body_SQ8
		|------	KerConv1x3Stride1xN_Border_SQ8
	KerConv1x3Stride1x2_SQ8
		|------	KerConv1x3Stride1x2_Body_SQ8
		|------	KerConv1x3Stride1xN_Border_SQ8
	KerConv3x3Stride1_SQ8
		|------	KerConv3x3Stride1_Body_SQ8
		|------	KerConv3x3Stride1_Border_SQ8
	KerConv3x3Stride2_SQ8
		|------	KerConv3x3Stride2_Body_SQ8
		|------	KerConv3x3Stride2_Border_SQ8
	KerConv3x3StrideS_SQ8
		|------	KerConv3x3StrideS_Body_SQ8
		|------	KerConv3x3StrideS_Border_SQ8

	KerConv5x1Stride1x1_SQ8
		|------	KerConv5x1Stride1x1_Body_SQ8
		|------	KerConv5x1StrideNx1_Border_SQ8
	KerConv5x1Stride2x1_SQ8
		|------	KerConv5x1Stride2x1_Body_SQ8
		|------	KerConv5x1StrideNx1_Border_SQ8
	KerConv1x5Stride1x1_SQ8
		|------	KerConv1x5Stride1x1_Body_SQ8
		|------	KerConv1x5Stride1xN_Border_SQ8
	KerConv1x5Stride1x2_SQ8
		|------	KerConv1x5Stride1x2_Body_SQ8
		|------	KerConv1x5Stride1xN_Border_SQ8
	KerConv5x5Stride1_SQ8
		|------	KerConv5x5Stride1_Body_SQ8
		|------	KerConv5x5Stride1_Border_SQ8
	KerConv5x5Stride2_SQ8
		|------	KerConv5x5Stride2_Body_SQ8
		|------	KerConv5x5Stride2_Border_SQ8
	KerConv5x5StrideS_SQ8
		|------	KerConv5x5StrideS_Body_SQ8
		|------	KerConv5x5StrideS_Border_SQ8
	KerConv7x7StrideS_SQ8
		|------	KerConv7x7StrideS_Body_SQ8
		|------	KerConvNxNStrideS_Border_SQ8

	KerConvNxNStrideS_SQ8
		|------	KerConvNxNStrideS_Body_SQ8
		|------	KerConvNxNStrideS_Border_SQ8

	KerConvNxMStrideSxSy_SQ8
		|------	KerConvNxMStrideSxSy_Body_SQ8
		|------	KerConvNxMStrideSxSy_Border_SQ8

	KerConvNxMDxDyStrideSxSy_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Body_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Border_SQ8
*/

void KerConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
	if (First<Last) KerConv1x1Stride1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
	gap_waitbarrier(0);
}

void KerConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
	if (First<Last) KerConv1x1Stride2_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
	gap_waitbarrier(0);
}

void KerConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=1, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
	if (First<Last) KerConv1x1StrideS_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg);
	gap_waitbarrier(0);
}

void KerConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=3, Sx=1;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv3x1Stride1x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=3, Sx=2;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv3x1Stride2x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv3x1BorderStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv1x3Stride1x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv1x3BorderStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv1x3Stride1x2_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv1x3BorderStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
#ifdef ALT
		unsigned int TotalInFeatures = Arg->TotalInFeatures, InFeatures = Arg->InFeatures, OutFeatures = Arg->OutFeatures;
		for (unsigned int of=0; of<OutFeatures; of++)
			for (unsigned int If=0; If<InFeatures; If++) {
				signed char *in = In+W*H*If, *filter = Filter+FS*FS*(TotalInFeatures*of  + If);
				int *out = Out+Wo*Ho*(of);
				KerConv3x3Stride1_Body_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
				if ((int)PadIn) KerConv3x3BorderStride1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg);
			}
#else
		KerConv3x3Stride1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv3x3BorderStride1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg);
#endif
	}
	gap_waitbarrier(0);
}

void KerConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv3x3Stride2_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv3x3BorderStride2_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=3, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv3x3StrideS_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg);
		if ((int)PadIn) KerConv3x3BorderStrideS_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=5, Sx=1;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv5x1Stride1x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=5, Sx=2;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv5x1Stride2x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv5x1BorderStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv1x5Stride1x1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv1x5BorderStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv1x5Stride1x2_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv1x5BorderStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv5x5Stride1_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv5x5BorderStride1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv5x5Stride2_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg);
		if ((int)PadIn) KerConv5x5BorderStride2_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=5, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv5x5StrideS_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg);
		if ((int)PadIn) KerConv5x5BorderStrideS_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=7, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConv7x7StrideS_Body_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg);
		if ((int)PadIn) KerConvNxNStrideS_Border_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FS=Arg->N, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConvNxNStrideS_Body_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg);
		if ((int)PadIn) KerConvNxNStrideS_Border_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConvNxMStrideSxSy_Body_SQ8(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg);
		if ((int)PadIn) KerConvNxMStrideSxSy_Border_SQ8(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

void KerConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Out = Arg->Out;
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
		KerConvNxMDxDyStrideSxSy_Body_SQ8(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg);
		if ((int)PadIn) KerConvNxMDxDyStrideSxSy_Border_SQ8(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg);
	}
	gap_waitbarrier(0);
}

