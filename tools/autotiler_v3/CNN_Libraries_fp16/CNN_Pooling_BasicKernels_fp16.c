#include <stdio.h>
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

static void KerMaxPool2x2Stride2_fp16(
	F16 * __restrict__ In,
	unsigned short int W,
	unsigned short int H,
	F16 * __restrict__ Out,
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

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = MaxF(MaxF((F16) 0.0, In[(2*i-PadTOrg)*W+0]), In[(2*i+1-PadTOrg)*W+0]);
	if (PadT) {
	       	F16V * __restrict__ Line1 = (F16V *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
		       	F16V M = MaxF2(Line1[j], ((F16V){0.0,0.0}));
		       	Out[j+Wo_F] = MaxF(M[0], M[1]);
	       	}
		if (PadL) Out[0] = MaxF(In[0], 0.0);
	       	if (PadR) Out[Wo-1] = MaxF(In[W-1], 0.0);
	}
	if (DoReLU) {
		for (unsigned int i=Ho_F; i<Ho_L; i++) {
			F16V *__restrict__ Line1 = (F16V *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
			F16V *__restrict__ Line2 = (F16V *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
			unsigned int Iter=(Wo_L-Wo_F);
			for (unsigned int j=0; j<Iter; j++) {
				F16V M = MaxF2(Line1[j], Line2[j]);
				Out[Wo*i+j+Wo_F] = MaxF(0.0, MaxF(M[0], M[1]));
			}
		}
	} else {
		for (unsigned int i=Ho_F; i<Ho_L; i++) {
			F16V *__restrict__ Line1 = (F16V *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
			F16V *__restrict__ Line2 = (F16V *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
			unsigned int Iter=(Wo_L-Wo_F);
			for (unsigned int j=0; j<Iter; j++) {
				F16V M = MaxF2(Line1[j], Line2[j]);
				Out[Wo*i+j+Wo_F] = MaxF(M[0], M[1]);
			}
		}
	}
       	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = MaxF(MaxF(0.0, In[(2*i-PadTOrg)*W+W-1]), In[(2*i+1-PadTOrg)*W+W-1]);
       	if (PadB) {
	       	F16V * __restrict__ Line1 = (F16V *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
		       	F16V M = MaxF2(Line1[j], ((F16V){0.0,0.0}));
		       	Out[Wo*(Ho-1)+j+Wo_F] = MaxF(M[0], M[1]);
	       	}
		if (PadL) Out[Wo*(Ho-1)] = MaxF(In[W*(H-1)], 0.0);
	       	if (PadR) Out[Wo*Ho-1] = MaxF(In[W*H-1], 0.0);
       	}
}

static void KerAvgPool2x2Stride2_fp16(
	F16 * __restrict__ In,
	unsigned short int W,
	unsigned short int H,
	F16 * __restrict__ Out,
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
	F16 ReVal = ((int)DoReLU)?((F16)0.0):-MIN_FLT16;

	if (PadL) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+0] = MaxF(ReVal, (In[(2*i-PadTOrg)*W+0]+In[(2*i+1-PadTOrg)*W+0])*((F16)0.25));
	if (PadT) {
		F16 * __restrict__ Line1 = (F16 *) (&In[Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			Out[j+Wo_F] = MaxF(ReVal, (Line1[2*j]+Line1[2*j+1])*((F16)0.25));
		}
		if (PadL) Out[0] = MaxF(In[0]*((F16)0.25), ReVal);
		if (PadR) Out[Wo-1] = MaxF(In[W-1]*((F16)0.25), ReVal);
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		F16V *__restrict__ Line1 = (F16V *) (In + (i*2  -PadTOrg)*W + Wo_F*2-PadLOrg);
		F16V *__restrict__ Line2 = (F16V *) (In + (i*2+1-PadTOrg)*W + Wo_F*2-PadLOrg);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			F16V S = Line1[j] + Line2[j];
			Out[Wo*i+j+Wo_F] = MaxF(ReVal, (S[0]+S[1])*((F16)0.25));
	       	}
	}
       	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = MaxF(ReVal, (In[(2*i-PadTOrg)*W+W-1]+In[(2*i+1-PadTOrg)*W+W-1])*((F16)0.25));
	if (PadB) {
		F16 * __restrict__ Line1 = (F16 *) (&In[(H-1)*W + Wo_F*2-PadLOrg]);
		unsigned int Iter=(Wo_L-Wo_F);
		for (unsigned int j=0; j<Iter; j++) {
			Out[Wo*(Ho-1)+j+Wo_F] =  MaxF(ReVal, (Line1[2*j]+Line1[2*j+1])*((F16)0.25));
		}
		if (PadL) Out[Wo*(Ho-1)] = MaxF(ReVal, In[W*(H-1)]*((F16)0.25));
		if (PadR) Out[Wo*Ho-1] = MaxF(ReVal, In[W*H-1]*((F16)0.25));
       	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Border_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

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
				F16 Acc = (F16) 0.0;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				F16 Acc = (F16) 0.0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				wr -= Stride;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = (F16) 0.0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Border_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

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
				F16 Acc = (F16) 0.0;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				F16 Acc = (F16) 0.0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
				Out[Wo*h+w] = MaxF(ReVal, Acc);
				wr -= StrideX;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = (F16) 0.0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc = MaxF(Acc, In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]);
					Out[Wo*h+w] = MaxF(ReVal, Acc);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Border_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	F16 PoolFactor = ((F16)1.0)/((F16)(Fw*Fh));
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
				F16 Acc = (F16) 0.0;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				ht -= Stride; hb -= Stride;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				F16 Acc = (F16) 0.0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				hb -= Stride;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				wl -= Stride; wr -= Stride;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				wr -= Stride;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = (F16) 0.0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}


static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Border_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

{
	unsigned int PadL=Pad[0], PadR=Pad[1], PadT=Pad[2], PadB=Pad[3];
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	F16 PoolFactor = ((F16)1.0)/((F16)(Fw*Fh));
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
				F16 Acc = (F16) 0.0;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				ht -= StrideY; hb -= StrideY;
			}
		}
	if (PadB) 
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				F16 Acc = (F16) 0.0;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				hb -= StrideY;
			}
		}
	if (PadL)
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw2;
			for (unsigned int w=0; w<Wo_F; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				wl -= StrideX; wr -= StrideX;
			}
		}
	if (PadR) 
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
			for (unsigned int w=Wo_L; w<Wo; w++) {
				F16 Acc = (F16) 0.0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				for (unsigned int i=0; i<Fh; i++) 
			       		for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
				Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
				wr -= StrideX;
			}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					F16 Acc = (F16) 0.0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
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
					F16 Acc = (F16) 0.0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)];
					Out[Wo*h+w] = MaxF(ReVal, Acc*PoolFactor);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxNStrideS_Body_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

{
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				F16V *Line0, *Line1;
				for (unsigned int i=0; i<(Fh/2); i++) {
					Line0 = (F16V*) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					Line1 = (F16V*) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						F16V M0 = MaxF2(Line0[j], Line1[j]);
						Acc = MaxF(Acc, MaxF(M0[0], M0[1]));
					}
					Acc = MaxF(Acc, In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL) + Fw - 1]);
					Acc = MaxF(Acc, In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL) + Fw - 1]);
				}
				for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*Stride-PadT+Fh-1)*W + (w*Stride-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					F16V *Line0 = (F16V *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					F16V *Line1 = (F16V *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						F16V M0 = MaxF2(Line0[j], Line1[j]);
						Acc = MaxF(M0[0], M0[1]);
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
				}
				Out[Wo*h+w] = Acc;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerMaxPoolNxMStrideSxSy_Body_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 ReVal)

{
	int W_Odd = (Fw&0x1), H_Odd = (Fh&0x1);
	if (W_Odd && H_Odd) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				F16V *Line0, *Line1;
				for (unsigned int i=0; i<(Fh/2); i++) {
					Line0 = (F16V*) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					Line1 = (F16V*) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						F16V M0 = MaxF2(Line0[j], Line1[j]);
						Acc = MaxF(Acc, MaxF(M0[0], M0[1]));
					}
					Acc = MaxF(Acc, In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL) + Fw - 1]);
					Acc = MaxF(Acc, In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL) + Fw - 1]);
				}
				for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*StrideY-PadT+Fh-1)*W + (w*StrideX-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				for (unsigned int i=0; i<(Fh/2); i++) {
					F16V *Line0 = (F16V *) &In[(h*StrideY-PadT+2*i  )*W + (w*StrideX-PadL)];
					F16V *Line1 = (F16V *) &In[(h*StrideY-PadT+2*i+1)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<(Fw/2); j++) {
						F16V M0 = MaxF2(Line0[j], Line1[j]);
						Acc = MaxF(M0[0], M0[1]);
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 Acc = ReVal;
				for (unsigned int i=0; i<Fh; i++) {
					for (unsigned int j=0; j<Fw; j++) Acc = MaxF(Acc, In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL+j)]);
				}
				Out[Wo*h+w] = Acc;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxNStrideS_Body_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 PoolFactor = ((F16)1.0)/((F16)(Fw*Fh));
	if (DoReLU) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					F16 *Line = (F16 *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<Fw; j++) S += Line[j];
				}
				Out[Wo*h+w] = MaxF((F16)0.0, S*PoolFactor);
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					F16 *Line = (F16 *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for (unsigned int j=0; j<Fw; j++) S += Line[j];
				}
				Out[Wo*h+w] = S*PoolFactor;
			}
		}
	}
}

static void __attribute__ ((noinline)) KerAvgPoolNxMStrideSxSy_Body_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
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
	F16 PoolFactor = ((F16)1.0)/((F16)(Fw*Fh));
	if (DoReLU) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					F16 *Line = (F16 *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<Fw; j++) S += Line[j];
				}
				Out[Wo*h+w] = MaxF((F16)0.0, S*PoolFactor);
			}
		}
	} else {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				F16 S = 0;
				for (unsigned int i=0; i<Fh; i++) {
					F16 *Line = (F16 *) &In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL)];
					for (unsigned int j=0; j<Fw; j++) S += Line[j];
				}
				Out[Wo*h+w] = S*PoolFactor;
			}
		}
	}
}

static void KerGlobalMaxPool_fp16(
        F16 * __restrict__ In,
        F16 * __restrict__ Out,
        unsigned int W,
        unsigned int H,
        int Init)

{
	F16 m = Init?(-MIN_FLT16):*Out;
	F16V M = (F16V) {m,m};
	F16V *Vi = (F16V *) In;

        for (int i=0; i<((W*H)/(2*2)); i++) {
		M = MaxF2(Vi[2*i],   M);
		M = MaxF2(Vi[2*i+1], M);
        }
	if ((W*H)&0x2) M = MaxF2(Vi[(W*H)/2], M);
	m = MaxF(M[0], M[1]);
	m = MaxF(In[W*H-1], m);
	*Out = m;
}

static void KerGlobalMaxPoolFullFeat_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
	int W,
	int H)

{
	F16 m = -MIN_FLT16;
	F16V M = (F16V) {m,m};
	F16V *Vi = (F16V *) In;

	for (int i=0; i<((W*H)/(2*2)); i++) {
		M = MaxF2(Vi[2*i],   M);
		M = MaxF2(Vi[2*i+1], M);
	}
	if ((W*H)&0x2) M = MaxF2(Vi[(W*H)/2], M);
	m = MaxF(M[0], M[1]);
	m = MaxF(In[W*H-1], m);
	*Out = m;
}

static void KerGlobalAvgPool_fp16(
        F16 * __restrict__ In,
        F16 * __restrict__ Out,
        int W,
        int H,
	int Init)

{
	F16V Sum = Init ? ((F16V) {(F16) 0, (F16) 0}):((F16V){*Out, (F16) 0});
        F16V *Vi = (F16V *) In;

        for (int i=0; i<((W*H)/(2*2)); i++) {
                Sum += Vi[2*i] + Vi[2*i+1];
        }
        if ((W*H)&0x2) Sum += Vi[(W*H)/2];
        F16 S = Sum[0]+Sum[1];
        if ((W*H)&0x1) S += In[W*H-1];

        *Out = S/(W*H);
}


static void KerGlobalAvgPoolFullFeat_fp16(
	F16 * __restrict__ In,
	F16 * __restrict__ Out,
	int W,
	int H)

{
	F16V Sum = (F16V) {(F16) 0, (F16) 0};
	F16V *Vi = (F16V *) In;

	for (int i=0; i<((W*H)/(2*2)); i++) {
		Sum += Vi[2*i] + Vi[2*i+1];
	}
	if ((W*H)&0x2) Sum += Vi[(W*H)/2];
	F16 S = Sum[0]+Sum[1];
	if ((W*H)&0x1) S += In[W*H-1];

	*Out = S/(W*H);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). Several output feature maps are evaluated in parallel, one output map per core

	Zero padding is optional (Arg->Pad)

	Arg->Oper == 0	Max Pooling, No ReLU
	Arg->Oper == 1	Max Pooling then ReLU
	Arg->Oper == 2	Average Pooling, No ReLU
	Arg->Oper == 3	Average Pooling then ReLU

	Input and output feature maps are short floats:
		KerPool2x2Stride2_fp16		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_fp16
			|------	KerAvgPool2x2Stride2_fp16
		KerPoolNxNStrideS_fp16		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp16 or KerMaxPoolNxNStrideS_Border_fp16
			|------	KerMaxPoolNxNStrideS_Body_fp16
			|------	KerMaxPoolNxNStrideS_Border_fp16
			|------	KerAvgPoolNxNStrideS_Body_fp16
			|------	KerAvgPoolNxNStrideS_Border_fp16
		KerPoolNxMStrideSxSy_fp16		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fp16 or KerMaxPoolNxMStrideSxSy_Border_fp16
			|------	KerMaxPoolNxMStrideSxSy_Body_fp16
			|------	KerMaxPoolNxMStrideSxSy_Border_fp16
			|------	KerAvgPoolNxMStrideSxSy_Body_fp16
			|------	KerAvgPoolNxMStrideSxSy_Border_fp16
		KerParGlobalMaxPoolFullFeat_fp16
		KerParGlobalAvgPoolFullFeat_fp16
	
*/

void KerParPool2x2Stride2_fp16(KerPool_fp16_T *Arg)

{
	unsigned int FS=2,S=2;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;
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

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_fp16(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	else	 for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_fp16(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, ReLU);
	gap_waitbarrier(0);

}

void KerParPoolNxNStrideS_fp16(KerPool_fp16_T *Arg)

{
	unsigned int FS=Arg->M, S=Arg->S;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Wref = W, Href = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	F16 ReVal = ((int)ReLU)?((F16)0.0):MIN_FLT16;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParPoolNxMStrideSxSy_fp16(KerPool_fp16_T *Arg)

{
	unsigned int FSx=Arg->M, Sx=Arg->S;
	unsigned int FSy=Arg->My, Sy=Arg->Sy;
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int Wref = W, Href = H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;
	int PoolMax = (((int)Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;
	F16 ReVal = ((int)ReLU)?(F16)0.0:-MIN_FLT16;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxMStrideSxSy_Body_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxMStrideSxSy_Body_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp16(In+of*Wref*Href, Out+of*Wo*Ho, FSx, FSy, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerParGlobalMaxPool_fp16(KerGlobalPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;
	int FirstTile = Arg->TileIndex==0;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPool_fp16(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

void KerParGlobalMaxPoolFullFeat_fp16(KerGlobalPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalMaxPoolFullFeat_fp16(In+of*W*H, Out+of, W, H);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPoolFullFeat_fp16(KerGlobalPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPoolFullFeat_fp16(In+of*W*H, Out+of, W, H);
	gap_waitbarrier(0);
}

void KerParGlobalAvgPool_fp16(KerGlobalPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int OutFeatures = Arg->OutFeatures;
	F16 * __restrict__ Out = Arg->Out;
	int FirstTile = Arg->TileIndex==0;

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);

	for (unsigned int of=First; of<Last; of++) KerGlobalAvgPool_fp16(In+of*W*H, Out+of, W, H, FirstTile);
	gap_waitbarrier(0);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). One output feature map is evaluated in parallel on all cores

	Zero padding is optional (Arg->Pad)

	Arg->Oper == 0	Max Pooling, No ReLU
	Arg->Oper == 1	Max Pooling then ReLU
	Arg->Oper == 2	Average Pooling, No ReLU
	Arg->Oper == 3	Average Pooling then ReLU

	Input and output feature maps are short floats:
		KerPool2x2Stride2_fp16		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_fp16
			|------	KerAvgPool2x2Stride2_fp16
		KerPoolNxNStrideS_fp16		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp16 or KerMaxPoolNxNStrideS_Border_fp16
			|------	KerMaxPoolNxNStrideS_Body_fp16
			|------	KerMaxPoolNxNStrideS_Border_fp16
			|------	KerAvgPoolNxNStrideS_Body_fp16
			|------	KerAvgPoolNxNStrideS_Border_fp16
		KerPoolNxMStrideSxSy_fp16		General case. Padding management: KerAvgPoolNxMStrideSxSy_Border_fp16 or KerMaxPoolNxMStrideSxSy_Border_fp16
			|------	KerMaxPoolNxMStrideSxSy_Body_fp16
			|------	KerMaxPoolNxMStrideSxSy_Border_fp16
			|------	KerAvgPoolNxMStrideSxSy_Body_fp16
			|------	KerAvgPoolNxMStrideSxSy_Border_fp16
	
*/

void KerPool2x2Stride2_fp16(KerPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
	v4s PadIn = Arg->Pad;
	int PoolMax = ((int)(Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	F16 ReVal = ((int)ReLU)?(F16)0.0:-MIN_FLT16;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_fp16(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fp16(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn, PadOrg, ReLU);
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_fp16(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
		else	 KerAvgPool2x2Stride2_fp16(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn, PadOrg, ReLU);
	}
	gap_waitbarrier(0);
}

void KerPoolNxNStrideS_fp16(KerPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	F16 * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->M, S = Arg->S;
	v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->Oper&0x2)==0);
	int ReLU = ((int)Arg->Oper&0x1);

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	F16 ReVal = ((int)ReLU)?(F16)0.0:-MIN_FLT16;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp16(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp16(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp16(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp16(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp16(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp16(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp16(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReLU);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp16(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		}
	}
	gap_waitbarrier(0);
}

void KerPoolNxMStrideSxSy_fp16(KerPool_fp16_T *Arg)

{
	F16 * __restrict__ In = Arg->In;
	F16 * __restrict__ Out = Arg->Out;
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
	F16 ReVal = ((int)ReLU)?(F16)0.0:-MIN_FLT16;

	unsigned int CoreId = gap_coreid();

	if (Arg->Orientation) { // Horizontal
		unsigned int Chunk = ChunkSize(Wo);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fp16(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp16(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fp16(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp16(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, Sx, Sy, ReVal);
		}
	} else {
		unsigned int Chunk = ChunkSize(Ho);
		unsigned int First = Chunk*CoreId;
		unsigned int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxMStrideSxSy_Body_fp16(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerMaxPoolNxMStrideSxSy_Border_fp16(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		} else {
			KerAvgPoolNxMStrideSxSy_Body_fp16(In, Out, FSx, FSy, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
			if ((int) PadIn) KerAvgPoolNxMStrideSxSy_Border_fp16(In, Out, FSx, FSy, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), Sx, Sy, ReVal);
		}
	}
	gap_waitbarrier(0);
}
