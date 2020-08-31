#include <stdio.h>
#include "Gap.h"
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

static void KerMaxPool2x2Stride2_fp(
	short int * __restrict__ In,
	unsigned short int W,
	unsigned short int H,
	short int * __restrict__ Out,
	unsigned short int Wo,
	unsigned short int Wo_F,
	unsigned short int Wo_L,
	unsigned short int Ho,
	unsigned short int Ho_F,
	unsigned short int Ho_L,
	v4s PadIn,
	v4s PadOrg,
	unsigned short int DoReLU
	)

{
	int PadL = PadIn[0], PadR = PadIn[1], PadT = PadIn[2], PadB = PadIn[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = Max(Max(0, In[(2*i-PadTOrg)*W+0]), In[(2*i+1-PadTOrg)*W+0]);
	if (PadT) {
	       	v2s * __restrict__ Line1 = (v2s *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
		       	v2s M = gap_max2(Line1[j], ((v2s){0,0}));
		       	Out[j+Wo_F] = Max(M[0], M[1]);
	       	}
		if (PadL) Out[0] = Max(In[0], 0);
	       	if (PadR) Out[Wo-1] = Max(In[W-1], 0);
	}
	if (DoReLU) {
		for (unsigned int i=Ho_F; i<Ho_L; i++) {
			v2s *__restrict__ Line1 = (v2s *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
			v2s *__restrict__ Line2 = (v2s *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
			unsigned int Iter=(Wo_L-Wo_F);
			for (unsigned int j=0; j<Iter; j++) {
				v2s M = gap_max2(Line1[j], Line2[j]);
				Out[Wo*i+j+Wo_F] = Max(0, Max(M[0], M[1]));
			}
		}
	} else {
		for (unsigned int i=Ho_F; i<Ho_L; i++) {
			v2s *__restrict__ Line1 = (v2s *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
			v2s *__restrict__ Line2 = (v2s *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
			unsigned int Iter=(Wo_L-Wo_F);
			for (unsigned int j=0; j<Iter; j++) {
				v2s M = gap_max2(Line1[j], Line2[j]);
				Out[Wo*i+j+Wo_F] = Max(M[0], M[1]);
			}
		}
	}
       	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(Max(0, In[(2*i-PadTOrg)*W+W-1]), In[(2*i+1-PadTOrg)*W+W-1]);
       	if (PadB) {
	       	v2s * __restrict__ Line1 = (v2s *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
		       	v2s M = gap_max2(Line1[j], ((v2s){0,0}));
		       	Out[Wo*(Ho-1)+j+Wo_F] = Max(M[0], M[1]);
	       	}
		if (PadL) Out[Wo*(Ho-1)] = Max(In[W*(H-1)], 0);
	       	if (PadR) Out[Wo*Ho-1] = Max(In[W*H-1], 0);
       	}
}

static void KerAvgPool2x2Stride2_fp(
	short int * __restrict__ In,
	unsigned short int W,
	unsigned short int H,
	short int * __restrict__ Out,
	unsigned short int Wo,
	unsigned short int Wo_F,
	unsigned short int Wo_L,
	unsigned short int Ho,
	unsigned short int Ho_F,
	unsigned short int Ho_L,
	v4s PadIn,
	v4s PadOrg,
	unsigned short int DoReLU
	)

{
	int PadL = PadIn[0], PadR = PadIn[1], PadT = PadIn[2], PadB = PadIn[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int ReVal = ((int)DoReLU)?0:0x80000000;

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = Max(ReVal, (In[(2*i-PadTOrg)*W+0]+In[(2*i+1-PadTOrg)*W+0])>>2);
	if (PadT) {
		v2s * __restrict__ Line1 = (v2s *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp2(Line1[j], ((v2s) {1,1}));
			Out[j+Wo_F] = Max(ReVal, S>>2);
		}
		if (PadL) Out[0] = Max(In[0]>>2, ReVal);
		if (PadR) Out[Wo-1] = Max(In[W-1]>>2, ReVal);
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		v2s *__restrict__ Line1 = (v2s *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
		v2s *__restrict__ Line2 = (v2s *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp2(Line1[j], ((v2s) {1,1}));
			Out[Wo*i+j+Wo_F] = Max(ReVal, gap_sumdotp2(Line2[j], ((v2s) {1,1}), S)>>2);
	       	}
	}
       	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(ReVal, (In[(2*i-PadTOrg)*W+W-1]+In[(2*i+1-PadTOrg)*W+W-1])>>2);
	if (PadB) {
		v2s * __restrict__ Line1 = (v2s *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp2(Line1[j], ((v2s) {1,1}));
			Out[Wo*(Ho-1)+j+Wo_F] = Max(ReVal, S>>2);
		}
		if (PadL) Out[Wo*(Ho-1)] = Max(ReVal, In[W*(H-1)]>>2);
		if (PadR) Out[Wo*Ho-1] = Max(ReVal, In[W*H-1]>>2);
       	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Border_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int Stride,
	int ReVal)

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
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Border_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int StrideY,
	int ReVal)

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
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Border_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int Stride,
	int ReVal)

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
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}


static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Border_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int StrideY,
	int ReVal)

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
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 15));
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Body_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int Stride,
	int ReVal)

{
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				v2s *Line0, *Line1;
				for (unsigned int i=0; i<(Fh/2); i++) {
					Line0 = (v2s*) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					Line1 = (v2s*) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v2s M0 = gap_max2(Line0[j], Line1[j]);
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
					v2s *Line0 = (v2s *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					v2s *Line1 = (v2s *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v2s M0 = gap_max2(Line0[j], Line1[j]);
						Acc = Max(M0[0], M0[1]);
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

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Body_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int StrideY,
	int ReVal)

{
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				v2s *Line0, *Line1;
				for (unsigned int i=0; i<(Fh/2); i++) {
					Line0 = (v2s*) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					Line1 = (v2s*) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v2s M0 = gap_max2(Line0[j], Line1[j]);
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
					v2s *Line0 = (v2s *) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					v2s *Line1 = (v2s *) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						v2s M0 = gap_max2(Line0[j], Line1[j]);
						Acc = Max(M0[0], M0[1]);
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

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Body_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int Stride,
	int DoReLU)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (DoReLU) {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
						S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15));
				}
			}
		} else {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15));
				}
			}
		}
	} else {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
						S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15);
				}
			}
		} else {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15);
				}
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Body_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
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
	int StrideY,
	int DoReLU)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (DoReLU) {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
						S += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15));
				}
			}
		} else {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15));
				}
			}
		}
	} else {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
						S += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15);
				}
			}
		} else {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						v2s *Line = (v2s *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp2(Line[j], ((v2s) {1,1}), S);
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 15);
				}
			}
		}
	}
}

static void KerGlobalMaxPoolDP_fp(
	short int * __restrict__ In,
	int * __restrict__ Out,
	int W,
	int H,
	int Init)

{
	int m = Init?(-32768):(*Out);
	v2s M = (v2s) {m,m};
	v2s *Vi = (v2s *) In;

	for (int i=0; i<((W*H)/2); i++) M = gap_max2(Vi[i], M);
	m = Max(M[0], M[1]);
	if ((W*H)&0x1) m = Max(In[W*H-1], m);
	*Out = m;
}

static void KerGlobalAvgPoolDP_fp(
	short int * __restrict__ In,
	int * __restrict__ Out,
	int W,
	int H,
	int Init)

{
	v2s M = (v2s) {1,1};
	v2s *Vi = (v2s *) In;
	int Sum = Init?0:(*Out);

	for (int i=0; i<((W*H)/2); i++) Sum = gap_sumdotp2(Vi[i], M, Sum);
	if ((W*H)&0x1) Sum += In[W*H-1];
	*Out = Sum;
}

static void KerGlobalMaxPoolFullFeat_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
	int W,
	int H)

{
	int m = -32768;
	v2s M = (v2s) {m,m};
	v2s *Vi = (v2s *) In;

	for (int i=0; i<((W*H)/(2*2)); i++) {
		M = gap_max2(Vi[2*i],   M);
		M = gap_max2(Vi[2*i+1], M);
	}
	if ((W*H)&0x2) M = gap_max2(Vi[(W*H)/2], M);
	m = Max(M[0], M[1]);
	m = Max(In[W*H-1], m);
	*Out = m;
}

static void KerGlobalAvgPoolFullFeat_fp(
	short int * __restrict__ In,
	short int * __restrict__ Out,
	int W,
	int H,
	int N)

{
	v2s M = (v2s) {1,1};
	v2s *Vi = (v2s *) In;
	int Sum = 0;

	for (int i=0; i<((W*H)/(2*2)); i++) {
		Sum = gap_sumdotp2(Vi[2*i], M, Sum);
		Sum = gap_sumdotp2(Vi[2*i+1], M, Sum);
	}
	if ((W*H)&0x2) Sum = gap_sumdotp2(Vi[(W*H)/2], M, Sum);
	if ((W*H)&0x1) Sum += In[W*H-1];

	*Out = gap_roundnorm_reg((Sum<<N)/(W*H), N);
}

static void KerMaxPool2x2Stride2_fps(
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
	v4s PadOrg,
	int DoReLU
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

	if (DoReLU) {
		for (unsigned int i=Ho_F; i<Ho_L; i++) {
			v4s *__restrict__ Line1 = (v4s *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
			v4s *__restrict__ Line2 = (v4s *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
			unsigned int Iter=(Wo_L-Wo_F);
			for (unsigned int j=0; j<(Iter/2); j++) {
				v4s M = gap_max4(Line1[j], Line2[j]);
				Out[Wo*i+2*j  +Wo_F] = Max(0, Max(M[0], M[1]));
				Out[Wo*i+2*j+1+Wo_F] = Max(0, Max(M[2], M[3]));
			}
			if (Iter&0x1) {
				v4s M = gap_max4(Line1[Iter/2], Line2[Iter/2]);
				Out[Wo*i+2*(Iter/2)+Wo_F] = Max(0, Max(M[0], M[1]));
			}
		}
	} else {
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

static void KerAvgPool2x2Stride2_fps(
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
	v4s PadOrg,
	int DoReLU
	)

{
	int PadL = PadIn[0], PadR = PadIn[1], PadT = PadIn[2], PadB = PadIn[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int ReVal = ((int)DoReLU)?0:0x80000000;

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = Max(ReVal, (In[(2*i-PadTOrg)*W+0]+In[(2*i+1-PadTOrg)*W+0])>>2);
	if (PadT) {
		v4s * __restrict__ Line1 = (v4s *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[j+Wo_F] = Max(ReVal, S>>2);
		}
		if (PadL) Out[0] = Max(In[0]>>2, 0);
		if (PadR) Out[Wo-1] = Max(In[W-1]>>2, 0);
	}
       	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		short int *__restrict__ Line1 = (short int *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
		short int *__restrict__ Line2 = (short int *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[Wo*i+j] = Max(ReVal, gap_sumdotp4((v4s)(int)Line2[j], ((v4s) {1,1,0,0}), S)>>2);

	       	}
       	}
	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(ReVal, (In[(2*i-PadTOrg)*W+W-1]+In[(2*i+1-PadTOrg)*W+W-1])>>2);
	if (PadB) {
		short int * __restrict__ Line1 = (short int *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			int S = gap_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
			Out[Wo*(Ho-1)+j+Wo_F] = Max(ReVal, S>>2);
		}
		if (PadL) Out[Wo*(Ho-1)] = Max(In[W*(H-1)]>>2, 0);
		if (PadR) Out[Wo*Ho-1] = Max(ReVal, In[W*H-1]>>2);
       	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Border_fps(
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
	int Stride,
	int ReVal)

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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Border_fps(
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
	int StrideY,
	int ReVal)

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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
				Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Border_fps(
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
	int Stride,
	int ReVal)

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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Border_fps(
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
	int StrideY,
	int ReVal)

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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
				Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(ReVal, gap_clip(gap_roundnorm_reg(Acc*PoolFactor, 16), 7));
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Body_fps(
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
	int Stride,
	int ReVal)

{
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

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Body_fps(
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
	int StrideY,
	int ReVal)

{
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

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Body_fps(
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
	int Stride,
	int DoReLU)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (DoReLU) {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
						S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7));
				}
			}
		}
	} else {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
						S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7);
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
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7);
				}
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Body_fps(
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
	int StrideY,
	int DoReLU)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);
	if (DoReLU) {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						short int *Line = (short int *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
						S += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7));
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
					Out[Wo*h+w] = Max(0, gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7));
				}
			}
		}
	} else {
		if (Fw&0x1) {
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				for (unsigned int h=Ho_F; h<Ho_L; h++) {
					int S = 0;
					for (unsigned int i=0; i<Fh; i++) {
						short int *Line = (short int *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
						for (unsigned int j=0; j<(Fw/2); j++) S = gap_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
						S += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL) + Fw - 1];
					}
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7);
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
					Out[Wo*h+w] = gap_clip(gap_roundnorm_reg(S*PoolFactor, 16), 7);
				}
			}
		}
	}
}

static void KerGlobalMaxPoolDP_fps(
	signed char * __restrict__ In,
	int * __restrict__ Out,
	int W,
	int H,
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
	for (int i=((W*H)/4); i<(W*H); i++) m = Max(In[i], m);
	*Out = m;
}

static void KerGlobalMaxPoolFullFeat_fps(
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
	for (int i=((W*H)/4); i<(W*H); i++) m = Max(In[i], m);
	*Out = m;
}

static void KerGlobalAvgPoolDP_fps(
	signed char * __restrict__ In,
	int * __restrict__ Out,
	int W,
	int H,
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
	for (int i=((W*H)/4); i<(W*H); i++) Sum += In[i];
	*Out = Sum;
}

static void KerGlobalAvgPoolFullFeat_fps(
	signed char * __restrict__ In,
	signed char * __restrict__ Out,
	int W,
	int H,
	int N)

{
	v4s M = (v4s) {1,1,1,1};
	v4s *Vi = (v4s *) In;
	int Sum = 0;

	for (int i=0; i<((W*H)/(2*4)); i++) {
		Sum = gap_sumdotp4(Vi[2*i], M, Sum);
		Sum = gap_sumdotp4(Vi[2*i+1], M, Sum);
	}
	if ((W*H)&0x4) Sum = gap_sumdotp4(Vi[(W*H)/4-1], M, Sum);
	for (int i=((W*H)/4); i<(W*H); i++) Sum += In[i];

	*Out = gap_roundnorm_reg((Sum<<N)/(W*H), N);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). Several output feature maps are evaluated in parallel, one output map per core

	Zero padding is optional (Arg->Pad)

	Arg->Oper == 0	Max Pooling, No ReLU
	Arg->Oper == 1	Max Pooling then ReLU
	Arg->Oper == 2	Average Pooling, No ReLU
	Arg->Oper == 3	Average Pooling then ReLU

	Input and output feature maps are bytes:
		KerParPool2x2Stride2_fps		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_fps
			|------	KerAvgPool2x2Stride2_fps
		KerParPoolNxNStrideS_fps		General case. Padding management: KerAvgPoolNxNStrideS_Border_fps or KerMaxPoolNxNStrideS_Border_fps
			|------	KerMaxPoolNxNStrideS_Body_fps
			|------	KerMaxPoolNxNStrideS_Border_fps
			|------	KerAvgPoolNxNStrideS_Body_fps
			|------	KerAvgPoolNxNStrideS_Border_fps
		KerParPoolNxMStrideSxSy_fps		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fps or KerMaxPoolNxMStrideSxSy_Border_fps
			|------	KerMaxPoolNxMStrideSxSy_Body_fps
			|------	KerMaxPoolNxMStrideSxSy_Border_fps
			|------	KerAvgPoolNxMStrideSxSy_Body_fps
			|------	KerAvgPoolNxMStrideSxSy_Border_fps
		KerParGlobalMaxPoolDP_fps
		KerParGlobalMaxPoolDPReduct_fps
		KerParGlobalAvgPoolDP_fps
		KerParGlobalAvgPoolDPReduct_fps
		KerParGlobalMaxPoolFullFeat_fps
		KerParGlobalAvgPoolFullFeat_fps
		

	Input and output feature maps are short words:
		KerPool2x2Stride2_fp		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_fp
			|------	KerAvgPool2x2Stride2_fp
		KerPoolNxNStrideS_fp		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp or KerMaxPoolNxNStrideS_Border_fp
			|------	KerMaxPoolNxNStrideS_Body_fp
			|------	KerMaxPoolNxNStrideS_Border_fp
			|------	KerAvgPoolNxNStrideS_Body_fp
			|------	KerAvgPoolNxNStrideS_Border_fp
		KerPoolNxMStrideSxSy_fp		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fp or KerMaxPoolNxMStrideSxSy_Border_fp
			|------	KerMaxPoolNxMStrideSxSy_Body_fp
			|------	KerMaxPoolNxMStrideSxSy_Border_fp
			|------	KerAvgPoolNxMStrideSxSy_Body_fp
			|------	KerAvgPoolNxMStrideSxSy_Border_fp
		KerParGlobalMaxPoolDP_fp
		KerParGlobalMaxPoolDPReduct_fp
		KerParGlobalAvgPoolDP_fp
		KerParGlobalAvgPoolDPReduct_fp
		KerParGlobalMaxPoolFullFeat_fp
		KerParGlobalAvgPoolFullFeat_fp
	
*/

void KerParPool2x2Stride2_fps(KerReLUPool_fps_T *Arg)

{
	unsigned int FS=2,S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = ((Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_fps(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_fps(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	gap_waitbarrier(0);

}

void KerParPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg)

{
	unsigned int FS=Arg->M, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	int ReVal = ((int)ReLU)?0:0x80000000;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_fps(KerReLUPool_fps_T *Arg)

{
	unsigned int FSx=Arg->M, Sx=Arg->S;
	unsigned int FSy=Arg->My, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	int ReVal = ((int)ReLU)?0:0x80000000;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_fps(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fps(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_fps(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fps(In+of*W*H, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolDP_fps(KerGlobalPoolDP_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	int * __restrict__ Out = Arg->Out;
	int FirstTile = (Arg->TileIndex==0);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolDP_fps(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolDP_fps(KerGlobalPoolDP_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	int * __restrict__ Out = Arg->Out;
	int FirstTile = (Arg->TileIndex==0);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolDP_fps(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolDPReduct_fps(KerGlobalPoolDPReduct_fps_T *Arg)

{
	int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);


	for (unsigned int of=First; of<Last; of+=2) {
		int I0 = In[of], I1 = In[of+1];
		Out[of] = I0; Out[of+1] = I1;
	}
	Out[Last-1] = In[Last-1];
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolDPReduct_fps(KerGlobalPoolDPReduct_fps_T *Arg)

{
	int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int N = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);


	for (unsigned int of=First; of<Last; of++) Out[of] = gap_roundnorm_reg((In[of]<<N)/(W*H), N);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_fps(KerGlobalPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_fps(In+of*W*H, Out+of, W, H);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_fps(KerGlobalPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Out = Arg->Out;
	int N = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_fps(In+of*W*H, Out+of, W, H, N);
	gap_waitbarrier(0);
}

void KerParPool2x2Stride2_fp(KerReLUPool_fp_T *Arg)

{
	unsigned int FS=2,S=2;
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int PoolMax = ((Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_fp(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_fp(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	gap_waitbarrier(0);

}

void KerParPoolNxNStrideS_fp(KerReLUPool_fp_T *Arg)

{
	unsigned int FS=Arg->M, S=Arg->S;
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Wref = W, Href = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	int ReVal = ((int)ReLU)?0:0x80000000;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_fp(KerReLUPool_fp_T *Arg)

{
	unsigned int FSx=Arg->M, Sx=Arg->S;
	unsigned int FSy=Arg->My, Sy=Arg->Sy;
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Wref = W, Href = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	int ReVal = ((int)ReLU)?0:0x80000000;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolDP_fp(KerGlobalPoolDP_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	int * __restrict__ Out = Arg->Out;
	int FirstTile = (Arg->TileIndex==0);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolDP_fp(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolDP_fp(KerGlobalPoolDP_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	int * __restrict__ Out = Arg->Out;
	int FirstTile = (Arg->TileIndex==0);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolDP_fp(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPooDPReduct_fp(KerGlobalPoolDPReduct_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);


	for (unsigned int of=First; of<Last; of+=2) {
		int I0 = In[of], I1 = In[of+1];
		Out[of] = I0; Out[of+1] = I1;
	}
	Out[Last-1] = In[Last-1];
	gap_waitbarrier(0);
}

void KerParGlobalAvgPooDPReduct_fp(KerGlobalPoolDPReduct_fp_T *Arg)

{
	int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int N = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);


	for (unsigned int of=First; of<Last; of++) Out[of] = gap_roundnorm_reg((In[of]<<N)/(W*H), N);

	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_fp(KerGlobalPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_fp(In+of*W*H, Out+of, W, H);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_fp(KerGlobalPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	short int * __restrict__ Out = Arg->Out;
	int N = Arg->Norm;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_fp(In+of*W*H, Out+of, W, H, N);
	gap_waitbarrier(0);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). One output feature map is evaluated in parallel on all cores

	Zero padding is optional (Arg->Pad)

	Arg->Oper == 0	Max Pooling, No ReLU
	Arg->Oper == 1	Max Pooling then ReLU
	Arg->Oper == 2	Average Pooling, No ReLU
	Arg->Oper == 3	Average Pooling then ReLU

	Input and output feature maps are bytes:
		KerPool2x2Stride2_fps		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_fps
			|------	KerAvgPool2x2Stride2_fps
		KerPoolNxNStrideS_fps		General case. Padding management: KerAvgPoolNxNStrideS_Border_fps or KerMaxPoolNxNStrideS_Border_fps
			|------	KerMaxPoolNxNStrideS_Body_fps
			|------	KerMaxPoolNxNStrideS_Border_fps
			|------	KerAvgPoolNxNStrideS_Body_fps
			|------	KerAvgPoolNxNStrideS_Border_fps
		KerPoolNxMStrideSxSy_fps	General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fps or KerMaxPoolNxMStrideSxSy_Border_fps
			|------	KerMaxPoolNxMStrideSxSy_Body_fps
			|------	KerMaxPoolNxMStrideSxSy_Border_fps
			|------	KerAvgPoolNxMStrideSxSy_Body_fps
			|------	KerAvgPoolNxMStrideSxSy_Border_fps
		

	Input and output feature maps are short words:
		KerPool2x2Stride2_fp		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_fp
			|------	KerAvgPool2x2Stride2_fp
		KerPoolNxNStrideS_fp		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp or KerMaxPoolNxNStrideS_Border_fp
			|------	KerMaxPoolNxNStrideS_Body_fp
			|------	KerMaxPoolNxNStrideS_Border_fp
			|------	KerAvgPoolNxNStrideS_Body_fp
			|------	KerAvgPoolNxNStrideS_Border_fp
		KerPoolNxMStrideSxSy_fp		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fp or KerMaxPoolNxMStrideSxSy_Border_fp
			|------	KerMaxPoolNxMStrideSxSy_Body_fp
			|------	KerMaxPoolNxMStrideSxSy_Border_fp
			|------	KerAvgPoolNxMStrideSxSy_Body_fp
			|------	KerAvgPoolNxMStrideSxSy_Border_fp
	
*/

void KerPool2x2Stride2_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_fps(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fps(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_fps(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fps(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->M, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FSx = Arg->M, Sx = Arg->S;
	unsigned int FSy = Arg->My, Sy = Arg->Sy;
	v4s PadIn = Arg->Pad;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fps(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fps(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fps(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fps(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fps(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fps(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fps(In, Out, FSx, FSx, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fps(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerPool2x2Stride2_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int PoolMax = ((int)(Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_fp(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fp(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_fp(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fp(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->M, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FSx = Arg->M, Sx = Arg->S;
	unsigned int FSy = Arg->My, Sy = Arg->Sy;
	v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	int ReVal = ((int)ReLU)?0:0x80000000;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fp(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fp(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fp(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fp(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}
