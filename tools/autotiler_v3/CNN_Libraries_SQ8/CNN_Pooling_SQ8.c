#include <stdio.h>
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

static inline int __attribute__ ((always_inline)) MinCond(int a, int b)

{
#ifdef DIM_ALWAYS_GREATER_THAN_FILTER
	return a;
#else
	return Max(0, Min(a, b));
#endif
}

/*
 * Standalone activation, assuming contiguous tile (horizontal)
*/
static void Ker_Activation_SQ8(
	signed char * __restrict__ InOut,
	unsigned int N,
	CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
	)

{
	for (unsigned int i=0; i<N/2; i++) {
		int Acc0 = InOut[2*i], Acc1 = InOut[2*i+1];
		switch (Activation) {
			case ACT_NONE:     Acc0 = AT_SCALE(Acc0, ActScale, ActScaleN); Acc1 = AT_SCALE(Acc1, ActScale, ActScaleN); break;
			case ACT_RELU:     Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Acc1), ActScale, ActScaleN); break;
			case ACT_RELUN:    Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN); Acc1 = AT_SCALE(Min(A0, Max(0, Acc1)), ActScale, ActScaleN); break;
			case ACT_HSIGMOID: Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0, ActScale, ActScaleN); break;
			case ACT_HSWISH:   Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN); Acc1 = AT_SCALE(Max(0, Min(A0, Acc1 + B0)) * C0 * Acc1, ActScale, ActScaleN); break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
					int Neg1 = gap_bitextractu(Acc1, 1, 31), Pos1 = !Neg1;
					int Acc1N = AT_NORM(Acc1 * A0, 7);
					Acc1 = AT_SCALE((Neg1*Acc1N+Pos1*Acc1), ActScale, ActScaleN);
				//      Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM((Acc0 * A0), 7):Acc0), ActScale, ActScaleN);
				//      Acc1 = AT_SCALE(((Acc1<0) ? AT_NORM((Acc1 * A0), 7):Acc1), ActScale, ActScaleN);
				}
				break;
		}
		InOut[2*i] = gap_clip(Acc0, 7), InOut[2*i+1] = gap_clip(Acc1, 7);
	}
	if (N&0x1) {
		unsigned int i=N-1;
		int Acc0 = InOut[i];
		switch (Activation) {
			case ACT_NONE:     Acc0 = AT_SCALE(Acc0, ActScale, ActScaleN); break;
			case ACT_RELU:     Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN); break;
			case ACT_RELUN:    Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN); break;
			case ACT_HSIGMOID: Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN); break;
			case ACT_HSWISH:   Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN); break;
			case ACT_LEAKYRELU:
				{
					int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
					int Acc0N = AT_NORM(Acc0 * A0, 7);
					Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
				//      Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM((Acc0 * A0), 7):Acc0), ActScale, ActScaleN);
				}
				break;
		}
		InOut[i] = gap_clip(Acc0, 7);
	}
}

/*
 * Standalone activation, assuming contiguous tile (horizontal), special case when ActScale=1.0
*/
static void Ker_ActivationScale1_SQ8(
	signed char * __restrict__ InOut,
	unsigned int N,
	CNN_ActivationOper_T Activation,
	int A0
	)

{
	for (unsigned int i=0; i<N/2; i++) {
		int Acc0 = InOut[2*i], Acc1 = InOut[2*i+1];
		switch (Activation) {
			case ACT_RELU: Acc0 = Max(0, Acc0); Acc1 = Max(0, Acc1); break;
			case ACT_RELUN: Acc0 = Min(A0, Max(0, Acc0)); Acc1 = Min(A0, Max(0, Acc1)); break;
		}
		InOut[2*i] = Acc0; InOut[2*i+1] = Acc1;
	}
	if (N&0x1) {
		unsigned int i=N-1;
		int Acc0 = InOut[i];
		switch (Activation) {
			case ACT_RELU: Acc0 = Max(0, Acc0); break;
			case ACT_RELUN: Acc0 = Min(A0, Max(0, Acc0)); break;
		}
		InOut[i] = Acc0;
	}
}

/*
 * Standalone activation, assuming vertical tile
*/
static void Ker_Activation_Ver_SQ8(
	signed char * __restrict__ InOut,
	unsigned int W, unsigned int H, unsigned int C_First, unsigned int C_Last,
	CNN_ActivationOper_T Activation,
	unsigned int ActScale, unsigned int ActScaleN, int A0, int B0, int C0
	)

{
	for (unsigned int l=0; l<H; l++) {
		for (unsigned int c=C_First; c<C_Last; c++) {
			int Acc0 = InOut[l*W+c];
			switch (Activation) {
				case ACT_NONE:     Acc0 = AT_SCALE(Acc0, ActScale, ActScaleN); break;
				case ACT_RELU:     Acc0 = AT_SCALE(Max(0, Acc0), ActScale, ActScaleN); break;
				case ACT_RELUN:    Acc0 = AT_SCALE(Min(A0, Max(0, Acc0)), ActScale, ActScaleN); break;
				case ACT_HSIGMOID: Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0, ActScale, ActScaleN); break;
				case ACT_HSWISH:   Acc0 = AT_SCALE(Max(0, Min(A0, Acc0 + B0)) * C0 * Acc0, ActScale, ActScaleN); break;
				case ACT_LEAKYRELU:
					{
						int Neg0 = gap_bitextractu(Acc0, 1, 31), Pos0 = !Neg0;
						int Acc0N = AT_NORM(Acc0 * A0, 7);
						Acc0 = AT_SCALE((Neg0*Acc0N+Pos0*Acc0), ActScale, ActScaleN);
					//      Acc0 = AT_SCALE(((Acc0<0) ? AT_NORM((Acc0 * A0), 7):Acc0), ActScale, ActScaleN);
					}
					break;
			}
			InOut[l*W+c] = gap_clip(Acc0, 7);
		}
	}
}

/*
 * Standalone activation, assuming contiguous tile (horizontal), special case when ActScale=1.0
*/
static void Ker_ActivationScale1_Ver_SQ8(
	signed char * __restrict__ InOut,
	unsigned int W, unsigned int H, unsigned int C_First, unsigned int C_Last,
	CNN_ActivationOper_T Activation,
	int A0
	)

{
	for (unsigned int l=0; l<H; l++) {
		for (unsigned int c=C_First; c<C_Last; c++) {
			int Acc0 = InOut[l*W+c];
			switch (Activation) {
				case ACT_RELU: Acc0 = Max(0, Acc0); break;
				case ACT_RELUN: Acc0 = Min(A0, Max(0, Acc0)); break;
			}
			InOut[l*W+c] = Acc0;
		}
	}
}

static void KerMaxPool2x2Stride2_SQ8(
	signed char * __restrict__ In,
	int W,
	int H,
	signed char * __restrict__ Out,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s PadIn,
	v4s PadOrg
	)

{
	int PadL = PadIn[0], PadR = PadIn[1], PadT = PadIn[2], PadB = PadIn[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = Max(Max(0, In[(2*i-PadTOrg)*W+0]), In[(2*i+1-PadTOrg)*W+0]);
	if (PadT) {
		v4s * __restrict__ Line1 = (v4s *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<(Iter/2); j++) {
		       	v4s M = gap_max4(Line1[j], ((v4s){0,0,0,0}));
		       	Out[2*j  +Wo_F] = Max(M[0], M[1]);
		       	Out[2*j+1+Wo_F] = Max(M[2], M[3]);
		}
		if (Iter&0x1) {
		       	v4s M = gap_max4(Line1[Iter/2], ((v4s){0,0,0,0}));
		       	Out[2*(Iter/2)+Wo_F] = Max(M[0], M[1]);
		}
		if (PadL) Out[0] = Max(In[0], 0);
		if (PadR) Out[Wo-1] = Max(In[W-1], 0);
	}

	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		v4s *__restrict__ Line1 = (v4s *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
		v4s *__restrict__ Line2 = (v4s *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<(Iter/2); j++) {
			v4s M = gap_max4(Line1[j], Line2[j]);
			Out[Wo*i+2*j  +Wo_F] = Max(M[0], M[1]);
			Out[Wo*i+2*j+1+Wo_F] = Max(M[2], M[3]);
		}
		if (Iter&0x1) {
			v4s M = gap_max4(Line1[Iter/2], Line2[Iter/2]);
			Out[Wo*i+2*(Iter/2)+Wo_F] = Max(M[0], M[1]);
		}
	}
	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(Max(0, In[(2*i-PadTOrg)*W+W-1]), In[(2*i+1-PadTOrg)*W+W-1]);
       	if (PadB) {
		v4s * __restrict__ Line1 = (v4s *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<(Iter/2); j++) {
		       	v4s M = gap_max4(Line1[j], ((v4s){0,0,0,0}));
		       	Out[Wo*(Ho-1)+2*j+Wo_F]   = Max(M[0], M[1]);
		       	Out[Wo*(Ho-1)+2*j+1+Wo_F] = Max(M[2], M[3]);
	       	}
		if (Iter&0x1) {
		       	v4s M = gap_max4(Line1[Iter/2], ((v4s){0,0,0,0}));
		       	Out[Wo*(Ho-1)+2*(Iter/2)+Wo_F] = Max(M[0], M[1]);
		}
		if (PadL) Out[Wo*(Ho-1)] = Max(In[W*(H-1)], 0);
	       	if (PadR) Out[Wo*Ho-1] = Max(In[W*H-1], 0);
       	}
}

static void KerAvgPool2x2Stride2_SQ8(
	signed char * __restrict__ In,
	int W,
	int H,
	signed char * __restrict__ Out,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s PadIn,
	v4s PadOrg
	)

{
	int PadL = PadIn[0], PadR = PadIn[1], PadT = PadIn[2], PadB = PadIn[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = gap_roundnorm(In[(2*i-PadTOrg)*W+0]+In[(2*i+1-PadTOrg)*W+0], 2);
	if (PadT) {
		v4s * __restrict__ Line1 = (v4s *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[j+Wo_F] = gap_roundnorm(S, 2);
		}
		if (PadL) Out[0] = gap_roundnorm(In[0], 2);
		if (PadR) Out[Wo-1] = gap_roundnorm(In[W-1], 2);
	}
       	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		short int *__restrict__ Line1 = (short int *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
		short int *__restrict__ Line2 = (short int *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[Wo*i+j] = gap_roundnorm(gap_sumdotp4((v4s)(int)Line2[j], ((v4s) {1,1,0,0}), S), 2);

	       	}
       	}
	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = gap_roundnorm(In[(2*i-PadTOrg)*W+W-1]+In[(2*i+1-PadTOrg)*W+W-1], 2);
	if (PadB) {
		short int * __restrict__ Line1 = (short int *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[Wo*(Ho-1)+j+Wo_F] = gap_roundnorm(S, 2);
		}
		if (PadL) Out[Wo*(Ho-1)] = gap_roundnorm(In[W*(H-1)], 2);
		if (PadR) Out[Wo*Ho-1] = gap_roundnorm(In[W*H-1], 2);
       	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Border_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	v4s Pad,
	v4s PadOrg,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride
	)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT)
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
			for (unsigned int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				wr -= Stride;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Acc;
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Border_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	v4s Pad,
	v4s PadOrg,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY
	)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;	// iff Wi_L>Wi_F

	if (PadT)
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
			for (unsigned int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Acc;
				wr -= StrideX;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] =Acc;
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = Acc;
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Border_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	v4s Pad,
	v4s PadOrg,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride
	)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F


	if (PadT)
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
			for (unsigned int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				wr -= Stride;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = Min(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Border_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	v4s Pad,
	v4s PadOrg,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY
	)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;	// iff Wi_L>Wi_F


	if (PadT)
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
			for (unsigned int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
				wr -= StrideX;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = Min(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = gap_clip(gap_roundnorm(Acc*PoolFactor, 16), 7);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Body_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	int PadL,
	int PadT,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride
	)

{
	int ReVal = 0x80000000;
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					short int *Line1 = (short int *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v4s M0 = gap_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
					Acc = Max(Acc, In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL) + Fw - 1]);
					Acc = Max(Acc, In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL) + Fw - 1]);
				}
				for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+Fh-1)*W + (w*Stride-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					short int *Line1 = (short int *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v4s M0 = gap_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
				}
				Out[Wo*h+w] = Acc;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Body_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	int PadL,
	int PadT,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY
	)

{
	int ReVal = 0x80000000;
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					short int *Line1 = (short int *) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v4s M0 = gap_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
					Acc = Max(Acc, In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL) + Fw - 1]);
					Acc = Max(Acc, In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL) + Fw - 1]);
				}
				for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadT+Fh-1)*W + (w*StrideX-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					short int *Line1 = (short int *) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v4s M0 = gap_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL+j)]);
				}
				Out[Wo*h+w] = Acc;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Body_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	int PadL,
	int PadT,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride
	)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (Fw&0x1) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
					S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
				}
				Out[Wo*h+w] = gap_roundnorm(S*PoolFactor, 16);
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
				}
				Out[Wo*h+w] = gap_roundnorm(S*PoolFactor, 16);
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Body_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int Fw,
	int Fh,
	int PadL,
	int PadT,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY
	)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (Fw&0x1) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
					S += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL) + Fw - 1];
				}
				Out[Wo*h+w] = gap_roundnorm(S*PoolFactor, 16);
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
				}
				Out[Wo*h+w] = gap_roundnorm(S*PoolFactor, 16);
			}
		}
	}
}

static void KerGlobalMaxPool_SQ8(
	signed char * __restrict__ In,
	int * __restrict__ Out,
	unsigned int W,
	unsigned int H,
	int Init)

{
	int m = Init?(-128):(*Out);
	v4s M = (v4s) {m,m,m,m};
	v4s *Vi = (v4s *) In;

	for (int i=0; i<((W*H)/(2*4)); i++) {
		M = gap_max4(Vi[2*i], M);
		M = gap_max4(Vi[2*i+1], M);
	}
	if ((W*H)&0x4) M = gap_max4(Vi[(W*H)/4-1], M);
	m = Max(Max(M[0], M[1]), Max(M[2], M[3]));
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) m = Max(In[i], m);
	*Out = m;
}

static void KerGlobalMaxPoolFullFeat_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int W,
	int H)

{
	int m = -128;
	v4s M = (v4s) {m,m,m,m};
	v4s *Vi = (v4s *) In;

	for (int i=0; i<((W*H)/(4*2)); i++) {
		M = gap_max4(Vi[2*i],   M);
		M = gap_max4(Vi[2*i+1], M);
	}
	if ((W*H)&0x4) M = gap_max4(Vi[(W*H)/4-1], M);
	m = Max(Max(M[0], M[1]), Max(M[2], M[3]));
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) m = Max(In[i], m);
	*Out = m;
}

static void KerGlobalAvgPool_SQ8(
	signed char * __restrict__ In,
	int * __restrict__ Out,
	unsigned int W,
	unsigned int H,
	int Init)

{
	v4s M = (v4s) {1,1,1,1};
	v4s *Vi = (v4s *) In;
	int Sum = Init?0:(*Out);

	for (int i=0; i<((W*H)/(2*4)); i++) {
		Sum = gap_sumdotp4(Vi[2*i], M, Sum);
		Sum = gap_sumdotp4(Vi[2*i+1], M, Sum);
	}
	if ((W*H)&0x4) Sum = gap_sumdotp4(Vi[(W*H)/4-1], M, Sum);
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Sum += In[i];
	*Out = Sum;
}

static void KerGlobalAvgPoolFullFeat_SQ8(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	unsigned int W,
	unsigned int H
	)

{
	v4s M = (v4s) {1,1,1,1};
	v4s *Vi = (v4s *) In;
	int Sum = 0;

	for (int i=0; i<((W*H)/(2*4)); i++) {
		Sum = gap_sumdotp4(Vi[2*i], M, Sum);
		Sum = gap_sumdotp4(Vi[2*i+1], M, Sum);
	}
	if ((W*H)&0x4) Sum = gap_sumdotp4(Vi[(W*H)/4-1], M, Sum);
	for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Sum += In[i];

	*Out = gap_roundnorm_reg((Sum<<7)/((int)(W*H)), 7);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). Several output feature maps are evaluated in parallel, one output map per core

	Zero padding is optional (Arg->Pad)

	Arg->MaxPool 	Max Pooling, otherwise Average Pooling

	Input and output feature maps are bytes:
		KerParPool2x2Stride2_SQ8		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_SQ8
			|------	KerAvgPool2x2Stride2_SQ8
		KerParPoolNxNStrideS_SQ8		General case. Padding management: KerAvgPoolNxNStrideS_Border_SQ8 or KerMaxPoolNxNStrideS_Border_SQ8
			|------	KerMaxPoolNxNStrideS_Body_SQ8
			|------	KerMaxPoolNxNStrideS_Border_SQ8
			|------	KerAvgPoolNxNStrideS_Body_SQ8
			|------	KerAvgPoolNxNStrideS_Border_SQ8
		KerParPoolNxMStrideSxSy_SQ8		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_SQ8 or KerMaxPoolNxMStrideSxSy_Border_SQ8
			|------	KerMaxPoolNxMStrideSxSy_Body_SQ8
			|------	KerMaxPoolNxMStrideSxSy_Border_SQ8
			|------	KerAvgPoolNxMStrideSxSy_Body_SQ8
			|------	KerAvgPoolNxMStrideSxSy_Border_SQ8
		KerParGlobalMaxPool_SQ8
		KerParGlobalAvgPool_SQ8
		KerParGlobalMaxPoolFullFeat_SQ8
		KerParGlobalAvgPoolFullFeat_SQ8
*/


static void KerParPoolActivation(signed char *__restrict__ InOut, int W, int H, int FirstFeat, int LastFeat,
				 signed char *__restrict__ Infos, CNN_ActivationOper_T Activation)

{
	if (Infos[AT_INF_ACTSCALE]) {
		unsigned int Off = W*H*FirstFeat, Size = W*H*(LastFeat-FirstFeat);
		unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
		int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];

		switch (Activation) {
			case ACT_NONE:      Ker_Activation_SQ8(InOut+Off, Size, ACT_NONE,      ActScale, ActScaleN, A0, B0, C0); break;
			case ACT_RELU:      Ker_Activation_SQ8(InOut+Off, Size, ACT_RELU,      ActScale, ActScaleN, A0, B0, C0); break;
			case ACT_RELUN:     Ker_Activation_SQ8(InOut+Off, Size, ACT_RELUN,     ActScale, ActScaleN, A0, B0, C0); break;
			case ACT_HSIGMOID:  Ker_Activation_SQ8(InOut+Off, Size, ACT_HSIGMOID,  ActScale, ActScaleN, A0, B0, C0); break;
			case ACT_HSWISH:    Ker_Activation_SQ8(InOut+Off, Size, ACT_HSWISH,    ActScale, ActScaleN, A0, B0, C0); break;
			case ACT_LEAKYRELU: Ker_Activation_SQ8(InOut+Off, Size, ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0); break;
		}
	} else if (Activation == ACT_RELU) {
		unsigned int Off = W*H*FirstFeat, Size = W*H*(LastFeat-FirstFeat);
		Ker_ActivationScale1_SQ8(InOut+Off, Size, ACT_RELU, 0);
	} else if (Activation == ACT_RELUN) {
		unsigned int Off = W*H*FirstFeat, Size = W*H*(LastFeat-FirstFeat);
		int A0 = Infos[AT_INF_A0];
		Ker_ActivationScale1_SQ8(InOut+Off, Size, ACT_RELUN, A0);
	}
}

void KerParPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=2,S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int PoolMax = Arg->PoolMax;
	int DoScale = Arg->DoScale;
	v4s PadIn = Arg->Pad;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);

	if (DoScale) KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE);
	gap_waitbarrier(0);
}

void KerParPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=2,S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int PoolMax = Arg->PoolMax;
	v4s PadIn = Arg->Pad;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);

	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU);
	gap_waitbarrier(0);
}

void KerParPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=2,S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int PoolMax = Arg->PoolMax;
	v4s PadIn = Arg->Pad;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_SQ8(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn);

	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN);
	gap_waitbarrier(0);
}

void KerParPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=Arg->FS, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = Arg->PoolMax;
	signed char * __restrict__ Infos = Arg->Infos;
	v4s PadIn = Arg->Pad;
	int DoScale = Arg->DoScale;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	}
	if (DoScale) KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE);
	gap_waitbarrier(0);
}

void KerParPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=Arg->FS, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = Arg->PoolMax;
	signed char * __restrict__ Infos = Arg->Infos;
	v4s PadIn = Arg->Pad;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	}
	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU);
	gap_waitbarrier(0);
}

void KerParPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FS=Arg->FS, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = Arg->PoolMax;
	signed char * __restrict__ Infos = Arg->Infos;
	v4s PadIn = Arg->Pad;
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S);
		}
	}
	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN);
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FSx=Arg->FS, Sx=Arg->S;
	unsigned int FSy=Arg->FSy, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int DoScale = Arg->DoScale;
	int PoolMax = Arg->PoolMax;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	}
	if (DoScale) KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE);
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FSx=Arg->FS, Sx=Arg->S;
	unsigned int FSy=Arg->FSy, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int PoolMax = Arg->PoolMax;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	}
	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU);
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	unsigned int FSx=Arg->FS, Sx=Arg->S;
	unsigned int FSy=Arg->FSy, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	int PoolMax = Arg->PoolMax;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy);
		}
	}
	KerParPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPool_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	int * __restrict__ Out = (int *__restrict) Arg->Out;
	int FirstTile = Arg->FirstTile;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPool_SQ8(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	int DoScale = Arg->DoScale;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (DoScale && ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = AT_SCALE(In[of], ActScale, ActScaleN);
	else for (unsigned int of=First; of<Last; of++) Out[of] = In[of];
	gap_waitbarrier(0);
}

void KerParGlobalMaxPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, AT_SCALE(In[of], ActScale, ActScaleN));
	else for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, In[of]);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, Min(A0, AT_SCALE(In[of], ActScale, ActScaleN)));
	else for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, Min(A0, In[of]));
	gap_waitbarrier(0);
}

void KerParGlobalAvgPool_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	int * __restrict__ Out = (int *__restrict__) Arg->Out;
	int FirstTile = Arg->FirstTile;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPool_SQ8(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	int DoScale = Arg->DoScale;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (DoScale && ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = AT_SCALE(gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7), ActScale, ActScaleN);
	else for (unsigned int of=First; of<Last; of++) Out[of] = gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, AT_SCALE(gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7), ActScale, ActScaleN));
	else for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7));
	gap_waitbarrier(0);
}

void KerParGlobalAvgPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	int * __restrict__ In = (int *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char *__restrict__ Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = Infos[AT_INF_A0];
	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	if (ActScale) for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, Min(A0, AT_SCALE(gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7), ActScale, ActScaleN)));
	else for (unsigned int of=First; of<Last; of++) Out[of] = Max(0, Min(A0, gap_roundnorm_reg((In[of]<<7)/((int)(W*H)), 7)));
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	int DoScale = Arg->DoScale;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	if (DoScale) KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_NONE);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_RELU);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_RELUN);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	int DoScale = Arg->DoScale;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	if (DoScale) KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_NONE);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_RELU);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = (signed char *__restrict__) Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Feat = Arg->Feat;
	signed char * __restrict__ Out = (signed char *__restrict__) Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;

	unsigned int CoreId = gap_coreid(), Chunk = ChunkSize(Feat), First = Chunk*CoreId, Last = Min(First+Chunk, Feat);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_SQ8(In+of*W*H, Out+of, W, H);
	KerParPoolActivation(Out, 1, 1, First, Last, Infos, ACT_RELUN);
	gap_waitbarrier(0);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). One output feature map is evaluated in parallel on all cores

	Zero padding is optional (Arg->Pad)

	Arg->MaxPool 	Max Pooling, otherwise Average Pool

	Input and output feature maps are bytes:
		KerPool2x2Stride2_SQ8		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_SQ8
			|------	KerAvgPool2x2Stride2_SQ8
		KerPoolNxNStrideS_SQ8		General case. Padding management: KerAvgPoolNxNStrideS_Border_SQ8 or KerMaxPoolNxNStrideS_Border_SQ8
			|------	KerMaxPoolNxNStrideS_Body_SQ8
			|------	KerMaxPoolNxNStrideS_Border_SQ8
			|------	KerAvgPoolNxNStrideS_Body_SQ8
			|------	KerAvgPoolNxNStrideS_Border_SQ8
		KerPoolNxMStrideSxSy_SQ8	General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_SQ8 or KerMaxPoolNxMStrideSxSy_Border_SQ8
			|------	KerMaxPoolNxMStrideSxSy_Body_SQ8
			|------	KerMaxPoolNxMStrideSxSy_Border_SQ8
			|------	KerAvgPoolNxMStrideSxSy_Body_SQ8
			|------	KerAvgPoolNxMStrideSxSy_Border_SQ8
*/

static void KerPoolActivation(signed char *__restrict__ InOut, int W, int H, int First, int Last,
			      signed char *__restrict__ Infos, CNN_ActivationOper_T Activation, int Orientation)

{
	if (Orientation) { // Horizontal
		if (Infos[AT_INF_ACTSCALE]) {
			unsigned int Off = W*First, Size = W*(Last-First);
			unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
			int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
	
			switch (Activation) {
				case ACT_NONE:      Ker_Activation_SQ8(InOut+Off, Size, ACT_NONE,      ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_RELU:      Ker_Activation_SQ8(InOut+Off, Size, ACT_RELU,      ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_RELUN:     Ker_Activation_SQ8(InOut+Off, Size, ACT_RELUN,     ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_HSIGMOID:  Ker_Activation_SQ8(InOut+Off, Size, ACT_HSIGMOID,  ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_HSWISH:    Ker_Activation_SQ8(InOut+Off, Size, ACT_HSWISH,    ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_LEAKYRELU: Ker_Activation_SQ8(InOut+Off, Size, ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0); break;
			}
		} else if (Activation == ACT_RELU) {
			unsigned int Off = W*First, Size = W*(Last-First);
			Ker_ActivationScale1_SQ8(InOut+Off, Size, ACT_RELU, 0);
		} else if (Activation == ACT_RELUN) {
			unsigned int Off = W*First, Size = W*(Last-First);
			int A0 = Infos[AT_INF_A0];
			Ker_ActivationScale1_SQ8(InOut+Off, Size, ACT_RELUN, A0);
		}
	} else {
		if (Infos[AT_INF_ACTSCALE]) {
			unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
			int A0 = Infos[AT_INF_A0], B0 = Infos[AT_INF_B0], C0 = Infos[AT_INF_C0];
			switch (Activation) {
				case ACT_NONE:      Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_NONE,      ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_RELU:      Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_RELU,      ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_RELUN:     Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_RELUN,     ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_HSIGMOID:  Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_HSIGMOID,  ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_HSWISH:    Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_HSWISH,    ActScale, ActScaleN, A0, B0, C0); break;
				case ACT_LEAKYRELU: Ker_Activation_Ver_SQ8(InOut+First, W, H, First, Last, ACT_LEAKYRELU, ActScale, ActScaleN, A0, B0, C0); break;
			}
		} else if (Activation == ACT_RELU) {
			Ker_ActivationScale1_Ver_SQ8(InOut+First, W, H, First, Last, ACT_RELU, 0);
		} else if (Activation == ACT_RELUN) {
			int A0 = Infos[AT_INF_A0];
			Ker_ActivationScale1_Ver_SQ8(InOut+First, W, H, First, Last, ACT_RELUN, A0);
		}
	}
}

void KerPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int DoScale = Arg->DoScale;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 0);
	}
	gap_waitbarrier(0);
}

void KerPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 0);
	}
	gap_waitbarrier(0);
}

void KerPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg);
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		else	 KerAvgPool2x2Stride2_SQ8(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg);
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->FS, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int DoScale = Arg->DoScale;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		}
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		}
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->FS, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->FS, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		} else {
			KerAvgPoolNxNStrideS_Body_SQ8(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_SQ8(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FSx = Arg->FS, Sx = Arg->S;
	unsigned int FSy = Arg->FSy, Sy = Arg->Sy;
	v4s PadIn = Arg->Pad;
	int DoScale = Arg->DoScale;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		}
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSx, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		}
		if (DoScale) KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_NONE, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FSx = Arg->FS, Sx = Arg->S;
	unsigned int FSy = Arg->FSy, Sy = Arg->Sy;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSx, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELU, 0);
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	signed char * __restrict__ Infos = Arg->Infos;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FSx = Arg->FS, Sx = Arg->S;
	unsigned int FSy = Arg->FSy, Sy = Arg->Sy;
	v4s PadIn = Arg->Pad;
	int PoolMax = Arg->PoolMax;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 1);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_SQ8(In, Out, FSx, FSx, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_SQ8(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy);
		}
		KerPoolActivation(Out, Wo, Ho, First, Last, Infos, ACT_RELUN, 0);
	}
	gap_waitbarrier(0);
}
