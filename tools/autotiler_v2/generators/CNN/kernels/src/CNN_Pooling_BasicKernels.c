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
        unsigned short int PadR,
        unsigned short int PadB,
	unsigned short int DoReLU
        )

{
	if (DoReLU) {
        	for (unsigned int i=Ho_F; i<Ho_L; i++) {
                	v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
                	v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
                        	v2s M = gap8_max2(Line1[j], Line2[j]);
                        	Out[Wo*i+j] = Max(0, Max(M[0], M[1]));
                	}
        	}
	} else {
        	for (unsigned int i=Ho_F; i<Ho_L; i++) {
                	v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
                	v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
                        	v2s M = gap8_max2(Line1[j], Line2[j]);
                        	Out[Wo*i+j] = Max(M[0], M[1]);
                	}
        	}
	}
       	if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(Max(0, In[(2*i)*W+W-1]), In[(2*i+1)*W+W-1]);
       	if (PadB) {
               	v2s * __restrict__ Line1 = (v2s *) (&In[(H-1)*W]);
               	for (unsigned int j=Wo_F; j<Wo_L; j++) {
                       	v2s M = gap8_max2(Line1[j], ((v2s){0,0}));
                       	Out[Wo*(Ho-1)+j] = Max(M[0], M[1]);
               	}
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
        unsigned short int PadR,
        unsigned short int PadB,
	unsigned short int DoReLU
        )

{
	if (DoReLU) {
        	for (unsigned int i=Ho_F; i<Ho_L; i++) {
                	v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
                	v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp2(Line1[j], ((v2s) {1,1}));
				Out[Wo*i+j] = Max(0, gap8_sumdotp2(Line2[j], ((v2s) {1,1}), S)>>2);

                	}
        	}
       		if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(0, (In[(2*i)*W+W-1]+In[(2*i+1)*W+W-1])>>2);
		if (PadB) {
			v2s * __restrict__ Line1 = (v2s *) (&In[(H-1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp2(Line1[j], ((v2s) {1,1}));
				Out[Wo*(Ho-1)+j] = Max(0, S>>2);
			}
			if (PadR) Out[Wo*Ho-1] = Max(0, In[W*H-1]>>2);
        	}
	} else {
        	for (unsigned int i=Ho_F; i<Ho_L; i++) {
                	v2s * __restrict__ Line1 = (v2s *) (&In[(2*i  )*W]);
                	v2s * __restrict__ Line2 = (v2s *) (&In[(2*i+1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp2(Line1[j], ((v2s) {1,1}));
				Out[Wo*i+j] = gap8_sumdotp2(Line2[j], ((v2s) {1,1}), S)>>2;
                	}
        	}
       		if (PadR) for (unsigned int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = (In[(2*i)*W+W-1]+In[(2*i+1)*W+W-1])>>2;
		if (PadB) {
			v2s * __restrict__ Line1 = (v2s *) (&In[(H-1)*W]);
                	for (unsigned int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp2(Line1[j], ((v2s) {1,1}));
				Out[Wo*(Ho-1)+j] = S>>2;
			}
			if (PadR) Out[Wo*Ho-1] = In[W*H-1]>>2;
        	}
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
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
                	for ( int h=0; h<Ho_F; h++) {
				int Acc = 0x80000000;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				ht -= Stride; hb -= Stride;
                	}
		}
	if (PadB)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
                	for ( int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0x80000000;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				hb -= Stride;
                	}
		}
	if (PadL)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                	for ( int w=0; w<Wo_F; w++) {
				int Acc = 0x80000000;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wl -= Stride; wr -= Stride;
                	}
		}
	if (PadR)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                	for ( int w=Wo_L; w<Wo; w++) {
				int Acc = 0x80000000;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wr -= Stride;
                	}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
					int Acc = 0x80000000;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= Stride;
				}
				hb -= Stride;
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
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F


	if (PadT)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
                	for ( int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
				ht -= Stride; hb -= Stride;
                	}
		}
	if (PadB)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
                	for ( int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = Min(Fh, hb); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
				hb -= Stride;
                	}
		}
	if (PadL)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                	for ( int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
				wl -= Stride; wr -= Stride;
                	}
		}
	if (PadR)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                	for ( int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = Min(Fw, wr); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
				wr -= Stride;
                	}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = Min(Fw, wr), Fh_min = ht, Fh_max = Min(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 16), 15));
					wr -= Stride;
				}
				hb -= Stride;
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
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				v2s *Line0, *Line1;
				for ( int i=0; i<(Fh/2); i++) {
					Line0 = (v2s*) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					Line1 = (v2s*) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) {
						v2s M0 = gap8_max2(Line0[j], Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
					Acc = Max(Acc, In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL) + Fw - 1]);
					Acc = Max(Acc, In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL) + Fw - 1]);
				}
				for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+Fh-1)*W + (w*Stride-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for ( int i=0; i<(Fh/2); i++) {
					v2s *Line0 = (v2s *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					v2s *Line1 = (v2s *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) {
						v2s M0 = gap8_max2(Line0[j], Line1[j]);
						Acc = Max(M0[0], M0[1]);
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for ( int i=0; i<Fh; i++) {
					for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
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
		int ReVal)

{
	unsigned int PoolFactor = (1<<16)/(Fw*Fh);

	if (Fw&0x1) {
       	for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
            	int S = 0;
				for ( int i=0; i<Fh; i++) {
					v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
                    	for ( int j=0; j<(Fw/2); j++) S = gap8_sumdotp2(Line[j], ((v2s) {1,1}), S);
					S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
                }
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(S*PoolFactor, 16), 15));
			}
       	}
	} else {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for ( int i=0; i<Fh; i++) {
					v2s *Line = (v2s *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) S = gap8_sumdotp2(Line[j], ((v2s) {1,1}), S);
				}
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(S*PoolFactor, 16), 15));
			}
		}
	}
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
        int PadR,
        int PadB,
	int DoReLU
        )

{
	if (DoReLU) {
        	for (int i=Ho_F; i<Ho_L; i++) {
                	v4s * __restrict__ Line1 = (v4s *) (&In[(2*i  )*W + Wo_F]);
                	v4s * __restrict__ Line2 = (v4s *) (&In[(2*i+1)*W + Wo_F]);
			 int Iter=(Wo_L-Wo_F);
			for ( int j=0; j<(Iter/2); j++) {
                        	v4s M = gap8_max4(Line1[j], Line2[j]);
                        	Out[Wo*i+2*j  ] = Max(0, Max(M[0], M[1]));
                        	Out[Wo*i+2*j+1] = Max(0, Max(M[2], M[3]));
			}
			if (Iter&0x1) {
                        	v4s M = gap8_max4(Line1[Iter/2], Line2[Iter/2]);
                        	Out[Wo*i+2*(Iter/2)] = Max(0, Max(M[0], M[1]));
			}
        	}
	} else {
        	for ( int i=Ho_F; i<Ho_L; i++) {
                	v4s * __restrict__ Line1 = (v4s *) (&In[(2*i  )*W + Wo_F]);
                	v4s * __restrict__ Line2 = (v4s *) (&In[(2*i+1)*W + Wo_F]);
			 int Iter=(Wo_L-Wo_F);
			for ( int j=0; j<(Iter/2); j++) {
                        	v4s M = gap8_max4(Line1[j], Line2[j]);
                        	Out[Wo*i+2*j  ] = Max(M[0], M[1]);
                        	Out[Wo*i+2*j+1] = Max(M[2], M[3]);
			}
			if (Iter&0x1) {
                        	v4s M = gap8_max4(Line1[Iter/2], Line2[Iter/2]);
                        	Out[Wo*i+2*(Iter/2)] = Max(M[0], M[1]);
			}
        	}
	}
       	if (PadR) for ( int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(Max(0, In[(2*i)*W+W-1]), In[(2*i+1)*W+W-1]);
       	if (PadB) {
               	v4s * __restrict__ Line1 = (v4s *) (&In[(H-1)*W]);
		 int Iter=(Wo_L-Wo_F);
		for ( int j=0; j<(Iter/2); j++) {
                       	v4s M = gap8_max4(Line1[j], ((v4s){0,0,0,0}));
                       	Out[Wo*(Ho-1)+2*j]   = Max(M[0], M[1]);
                       	Out[Wo*(Ho-1)+2*j+1] = Max(M[2], M[3]);
               	}
		if (Iter&0x1) {
                       	v4s M = gap8_max4(Line1[Iter/2], ((v4s){0,0,0,0}));
                       	Out[Wo*(Ho-1)+2*(Iter/2)] = Max(M[0], M[1]);
		}
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
        int PadR,
        int PadB,
	int DoReLU
        )

{
	if (DoReLU) {
        	for ( int i=Ho_F; i<Ho_L; i++) {
                	short int * __restrict__ Line1 = (short int *) (&In[(2*i  )*W]);
                	short int * __restrict__ Line2 = (short int *) (&In[(2*i+1)*W]);
                	for ( int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
				Out[Wo*i+j] = Max(0, gap8_sumdotp4((v4s)(int)Line2[j], ((v4s) {1,1,0,0}), S)>>2);

                	}
        	}
       		if (PadR) for ( int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = Max(0, (In[(2*i)*W+W-1]+In[(2*i+1)*W+W-1])>>2);
		if (PadB) {
			short int * __restrict__ Line1 = (short int *) (&In[(H-1)*W]);
                	for ( int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
				Out[Wo*(Ho-1)+j] = Max(0, S>>2);
			}
			if (PadR) Out[Wo*Ho-1] = Max(0, In[W*H-1]>>2);
        	}
	} else {
        	for ( int i=Ho_F; i<Ho_L; i++) {
                	short int * __restrict__ Line1 = (short int *) (&In[(2*i  )*W]);
                	short int * __restrict__ Line2 = (short int *) (&In[(2*i+1)*W]);
                	for ( int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
				Out[Wo*i+j] = gap8_sumdotp4((v4s)(int)Line2[j], ((v4s) {1,1,0,0}), S)>>2;
                	}
        	}
       		if (PadR) for ( int i=Ho_F; i<Ho_L; i++) Out[Wo*i+Wo-1] = (In[(2*i)*W+W-1]+In[(2*i+1)*W+W-1])>>2;
		if (PadB) {
			short int * __restrict__ Line1 = (short int *) (&In[(H-1)*W]);
                	for ( int j=Wo_F; j<Wo_L; j++) {
				int S = gap8_dotp4((v4s)(int)Line1[j], ((v4s) {1,1,0,0}));
				Out[Wo*(Ho-1)+j] = S>>2;
			}
			if (PadR) Out[Wo*Ho-1] = In[W*H-1]>>2;
        	}
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
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
                	for ( int h=0; h<Ho_F; h++) {
				int Acc = 0x80000000;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				ht -= Stride; hb -= Stride;
                	}
		}
	if (PadB)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
                	for ( int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0x80000000;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				hb -= Stride;
                	}
		}
	if (PadL)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                	for ( int w=0; w<Wo_F; w++) {
				int Acc = 0x80000000;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wl -= Stride; wr -= Stride;
                	}
		}
	if (PadR)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                	for ( int w=Wo_L; w<Wo; w++) {
				int Acc = 0x80000000;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
				Out[Wo*h+w] = Max(ReVal, Acc);
				wr -= Stride;
                	}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
					int Acc = 0x80000000;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
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
					int Acc = 0x80000000;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc = Max(Acc, In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]);
					Out[Wo*h+w] = Max(ReVal, Acc);
					wr -= Stride;
				}
				hb -= Stride;
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
	unsigned int PoolFactor = (1<<8)/(Fw*Fh);
	int Hi_F = (Fh-1)/2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = (Fw-1)/2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F


	if (PadT)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
			/* Top stripe: 0 .. Min(Ho_F, Ho), Along W all points are defined since we are in [Wo_F..Wo_L[  */
                	for ( int h=0; h<Ho_F; h++) {
				int Acc = 0;
				int Fh_min = ht, Fh_max = MinCond(Fh, hb); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
				ht -= Stride; hb -= Stride;
                	}
		}
	if (PadB)
        	for ( int w=Wo_F; w<Wo_L; w++) {
			int ht = 0, hb = H - (Hi_L+Stride) + Fh/2;
			/* Bottom stripe.  Exists only if Ho_L>Ho_F, then in this case Fh_min is = 0 by construction */
                	for ( int h=Ho_L; h<Ho; h++) {
				int Fh_min = ht, Fh_max = MinCond(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
				int Acc = 0;
                        	for ( int i=Fh_min; i<Fh_max; i++)
                                	for ( int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
				hb -= Stride;
                	}
		}
	if (PadL)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                	for ( int w=0; w<Wo_F; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(Fw, wr); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
				wl -= Stride; wr -= Stride;
                	}
		}
	if (PadR)
        	for ( int h=Ho_F; h<Ho_L; h++) {
			int wl = 0, wr = W - (Wi_L+Stride) + Fw/2;
                	for ( int w=Wo_L; w<Wo; w++) {
				int Acc = 0;
				int Wh_min = wl, Wh_max = MinCond(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
                        	for ( int i=0; i<Fh; i++)
                               		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
				wr -= Stride;
                	}
		}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh/2;
        		for ( int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw/2;
                		for ( int w=0; w<Wo_F; w++) {
					int Acc = 0;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
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
					int Acc = 0;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = MinCond(Fh, hb);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(Fw, wr), Fh_min = ht, Fh_max = MinCond(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
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
					int Acc = 0;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = MinCond(wr, Fw), Fh_min = ht, Fh_max = Min(hb, Fh);
                        		for ( int i=Fh_min; i<Fh_max; i++)
                                		for ( int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)];
					Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, 8), 7));
					wr -= Stride;
				}
				hb -= Stride;
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
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for ( int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					short int *Line1 = (short int *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) {
						v4s M0 = gap8_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(Acc, Max(M0[0], M0[1]));
					}
					Acc = Max(Acc, In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL) + Fw - 1]);
					Acc = Max(Acc, In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL) + Fw - 1]);
				}
				for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+Fh-1)*W + (w*Stride-PadL+j)]);
				Out[Wo*h+w] = Acc;
			}
		}
	} else if ((W_Odd==0)&&(H_Odd==0)) {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for ( int i=0; i<(Fh/2); i++) {
					short int *Line0 = (short int *) &In[(h*Stride-PadT+2*i  )*W + (w*Stride-PadL)];
					short int *Line1 = (short int *) &In[(h*Stride-PadT+2*i+1)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) {
						v4s M0 = gap8_max4((v4s)(int)Line0[j], (v4s)(int)Line1[j]);
						Acc = Max(M0[0], M0[1]);
					}
				}
				Out[Wo*h+w] = Acc;
			}
		}
	} else {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int Acc = ReVal;
				for ( int i=0; i<Fh; i++) {
					for ( int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
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
		int ReVal)

{
	unsigned int PoolFactor = (1<<8)/(Fw*Fh);
	
	if (Fw&0x1) {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for ( int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) S = gap8_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
					S += In[(h*Stride-PadT+i)*W + (w*Stride-PadL) + Fw - 1];
				}
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(S*PoolFactor, 8), 7));
			}
		}
	} else {
		for ( int w=Wo_F; w<Wo_L; w++) {
			for ( int h=Ho_F; h<Ho_L; h++) {
				int S = 0;
				for ( int i=0; i<Fh; i++) {
					short int *Line = (short int *) &In[(h*Stride-PadT+i)*W + (w*Stride-PadL)];
					for ( int j=0; j<(Fw/2); j++) S = gap8_sumdotp4((v4s)(int)Line[j], ((v4s) {1,1,0,0}), S);
				}
				Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(S*PoolFactor, 8), 7));
			}
		}
	}
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). Several output feature maps are evaluated in parallel, one output map per core

	Zero padding is optional (Arg->Pad)

	Arg->DoReLU == 0	Max Pooling, No ReLU
	Arg->DoReLU == 1	Max Pooling then ReLU
	Arg->DoReLU == 2	Average Pooling, No ReLU
	Arg->DoReLU == 3	Average Pooling then ReLU

	Input and output feature maps are bytes:
		KerParPool2x2Stride2_fps		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_fps
			|------	KerAvgPool2x2Stride2_fps
		KerParPoolNxNStrideS_fps		General case. Padding management: KerAvgPoolNxNStrideS_Border_fps or KerMaxPoolNxNStrideS_Border_fps
			|------	KerMaxPoolNxNStrideS_Body_fps
			|------	KerMaxPoolNxNStrideS_Border_fps
			|------	KerAvgPoolNxNStrideS_Body_fps
			|------	KerAvgPoolNxNStrideS_Border_fps


	Input and output feature maps are short words:
		KerPool2x2Stride2_fp		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------ KerMaxPool2x2Stride2_fp
			|------	KerAvgPool2x2Stride2_fp
		KerPoolNxNStrideS_fp		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp or KerMaxPoolNxNStrideS_Border_fp
			|------	KerMaxPoolNxNStrideS_Body_fp
			|------	KerMaxPoolNxNStrideS_Border_fp
			|------	KerAvgPoolNxNStrideS_Body_fp
			|------	KerAvgPoolNxNStrideS_Border_fp

*/

void KerParPool2x2Stride2_fps(KerParReLUPool_fps_T *Arg)

{
        unsigned int FS=2,S=2;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W, H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        signed char * __restrict__ Out = Arg->Out;
        int PoolMax = ((Arg->DoReLU&0x2)==0);

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
        int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
        int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

	if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_fps(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
	else         for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_fps(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
        gap8_waitbarrier(0);

}

void KerParPoolNxNStrideS_fps(KerParReLUPool_fps_T *Arg)

{
        unsigned int FS=Arg->M, S=Arg->S;
        signed char * __restrict__ In = Arg->In;
        unsigned int W = Arg->W, H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        signed char * __restrict__ Out = Arg->Out;
        int PoolMax = ((Arg->DoReLU&0x2)==0);

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
        int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
        int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F,Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo,  Wo_F,  Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In+of*W*H, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo,  Wo_F,  Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	}
	gap8_waitbarrier(0);
}

void KerParPool2x2Stride2_fp(KerParReLUPool_fp_T *Arg)

{
        unsigned int FS=2,S=2;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W, H = Arg->H;
        unsigned int OutFeatures = Arg->OutFeatures;
        short int * __restrict__ Out = Arg->Out;
        int PoolMax = ((Arg->DoReLU&0x2)==0);

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
        int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
        int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

        if (PoolMax) for (unsigned int of=First; of<Last; of++) KerMaxPool2x2Stride2_fp(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
        else         for (unsigned int of=First; of<Last; of++) KerAvgPool2x2Stride2_fp(In+of*W*H, W, H, Out+of*Wo*Ho, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
        gap8_waitbarrier(0);

}

void KerParPoolNxNStrideS_fp(KerParReLUPool_fp_T *Arg)

{
        unsigned int FS=Arg->M, S=Arg->S;
        short int * __restrict__ In = Arg->In;
        unsigned int W = Arg->W, H = Arg->H;
        unsigned int Wref = W, Href = H;
        unsigned int OutFeatures = Arg->OutFeatures;
        short int * __restrict__ Out = Arg->Out;
	int PoolMax = ((Arg->DoReLU&0x2)==0);

        unsigned int CoreId = gap8_coreid();
        unsigned int Chunk = ChunkSize(OutFeatures);
        unsigned int First = Chunk*CoreId;
        unsigned int Last = Min(First+Chunk, OutFeatures);
        v4s PadIn = Arg->Pad;
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));

	if (PoolMax) {
		for (unsigned int of=First; of<Last; of++) {
			KerMaxPoolNxNStrideS_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo,  Wo_F,  Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		for (unsigned int of=First; of<Last; of++) {
			KerAvgPoolNxNStrideS_Body_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn[0], PadIn[2], W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In+of*Wref*Href, Out+of*Wo*Ho, FS, FS, PadIn, PadIn, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, ReVal);
		}
	}
        gap8_waitbarrier(0);
}

/* Pooling group.
	Performs Max or Average pooling followed by an optional linear rectification (ReLU). One output feature map is evaluated in parallel on all cores

	Zero padding is optional (Arg->Pad)

	Arg->DoReLU == 0	Max Pooling, No ReLU
	Arg->DoReLU == 1	Max Pooling then ReLU
	Arg->DoReLU == 2	Average Pooling, No ReLU
	Arg->DoReLU == 3	Average Pooling then ReLU

	Input and output feature maps are bytes:
		KerPool2x2Stride2_fps		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_fps
			|------	KerAvgPool2x2Stride2_fps
		KerPoolNxNStrideS_fps		General case. Padding management: KerAvgPoolNxNStrideS_Border_fps or KerMaxPoolNxNStrideS_Border_fps
			|------	KerMaxPoolNxNStrideS_Body_fps
			|------	KerMaxPoolNxNStrideS_Border_fps
			|------	KerAvgPoolNxNStrideS_Body_fps
			|------	KerAvgPoolNxNStrideS_Border_fps


	Input and output feature maps are short words:
		KerPool2x2Stride2_fp		Special case: Stride=2 and PoolSize=2, Padding management is embedded
			|------	KerMaxPool2x2Stride2_fp
			|------	KerAvgPool2x2Stride2_fp
		KerPoolNxNStrideS_fp		General case. Padding management: KerAvgPoolNxNStrideS_Border_fp or KerMaxPoolNxNStrideS_Border_fp
			|------	KerMaxPoolNxNStrideS_Body_fp
			|------	KerMaxPoolNxNStrideS_Border_fp
			|------	KerAvgPoolNxNStrideS_Body_fp
			|------	KerAvgPoolNxNStrideS_Border_fp

*/

void KerPool2x2Stride2_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
        v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->DoReLU&0x2)==0);

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        unsigned int CoreId = gap8_coreid();

	if (Arg->Orientation) { // Horizontal
		int Chunk = ChunkSize(Wo);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_fps(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
		else         KerAvgPool2x2Stride2_fps(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
	} else {
		int Chunk = ChunkSize(Ho);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_fps(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
		else         KerAvgPool2x2Stride2_fps(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
	}
        gap8_waitbarrier(0);
}

void KerPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->M, S = Arg->S;
        v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->DoReLU&0x2)==0);

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        unsigned int CoreId = gap8_coreid();

	if (Arg->Orientation) { // Horizontal
		int Chunk = ChunkSize(Wo);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		int Chunk = ChunkSize(Ho);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fps(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fps(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		}
        }
        gap8_waitbarrier(0);
}

void KerPool2x2Stride2_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = 2, S = 2;
        v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->DoReLU&0x2)==0);

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        unsigned int CoreId = gap8_coreid();

	if (Arg->Orientation) { // Horizontal
		int Chunk = ChunkSize(Wo);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) KerMaxPool2x2Stride2_fp(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
		else         KerAvgPool2x2Stride2_fp(In, W, H, Out, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
	} else {
		int Chunk = ChunkSize(Ho);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) KerMaxPool2x2Stride2_fp(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
		else         KerAvgPool2x2Stride2_fp(In, W, H, Out, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), PadIn[1]&W&0x1, PadIn[3]&H&0x1, Arg->DoReLU&0x1);
	}
        gap8_waitbarrier(0);
}

void KerPoolNxNStrideS_fp(KerReLUPool_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	unsigned int W = Arg->W, H = Arg->H;
	unsigned int FS = Arg->M, S = Arg->S;
        v4s PadIn = Arg->Pad;
	int PoolMax = ((Arg->DoReLU&0x2)==0);

        int Wo = (W-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(W, FS, PadIn[0], S));
        int Ho = (H-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(H, FS, PadIn[2], S));
        int ReVal = (Arg->DoReLU&0x1)?0:0x80000000;

        unsigned int CoreId = gap8_coreid();

	if (Arg->Orientation) { // Horizontal
		int Chunk = ChunkSize(Wo);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Wo);
		v4s PadOrg = PadIn;
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Max(First, Wo_F), Min(Last, Wo_L), Ho, Ho_F, Ho_L, S, ReVal);
		}
	} else {
		int Chunk = ChunkSize(Ho);
		int First = Chunk*CoreId;
		int Last = Min(First+Chunk, Ho);
		v4s PadOrg = PadIn;
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		if (PoolMax) {
			KerMaxPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerMaxPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		} else {
			KerAvgPoolNxNStrideS_Body_fp(In, Out, FS, FS, PadOrg[0], PadOrg[2], W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
			if ((int) PadIn) KerAvgPoolNxNStrideS_Border_fp(In, Out, FS, FS, PadIn, PadOrg, W, H, Wo, Wo_F, Wo_L, Ho, Max(First, Ho_F), Min(Last, Ho_L), S, ReVal);
		}
        }
        gap8_waitbarrier(0);
}
