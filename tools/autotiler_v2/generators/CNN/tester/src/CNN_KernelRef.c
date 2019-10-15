#include <stdio.h>
#include "Gap8.h"

#define ALIGN(Value, Size)      (((Value)&((1<<(Size))-1))?((((Value)>>(Size))+1)<<(Size)):(Value))

#ifdef __pulp__
#define Abs(a)                  __builtin_pulp_abs((a))
#define Min(a, b)               __builtin_pulp_minsi((a), (b))
#define Max(a, b)               __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)                  (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)               (((a)<(b))?(a):(b))
#define Max(a, b)               (((a)>(b))?(a):(b))
#endif

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

/**************************************************************************************************************************
 * Data Size = 2
 * Convolution, Pooling (Max, Average), Liner Rectification
 * Tensor Traversal
***************************************************************************************************************************/

static void __attribute__ ((noinline)) DoReLU_fp(short int *__restrict__ In, short int *__restrict__ Out, unsigned short int W, unsigned short int H)

{
        v2s * LineIn = (v2s *) In;
        v2s * LineOut = (v2s *) Out;

        for (unsigned int i=0; i<((W*H)/4); i++) {
                v2s X = LineIn[2*i], Y = LineIn[2*i+1];
                LineOut[2*i]   = gap8_max2(X, ((v2s){0,0}));
                LineOut[2*i+1] = gap8_max2(Y, ((v2s){0,0}));
        }
        for (unsigned int i=4*((W*H)/4); i<(W*H); i++) Out[i] = Max(In[i], 0);
}

static void DoMaxPool_fp(short int *In, unsigned int W, unsigned int H, short int *Out, unsigned int Wo, unsigned int Ho, unsigned int Fw, unsigned int Fh, unsigned int Stride, int Norm, v4s Pad, int ReLU)

{
	int PadL = 0, PadR = 0, PadT = 0, PadB = 0;
	if ((int) Pad) {
		PadL = Pad[0]; PadR = Pad[1]; PadT = Pad[2]; PadB = Pad[3];
	}
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0x80000000;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
 			}
			Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}

        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}
}

static void DoAvgPool_fp(short int *In, unsigned int W, unsigned int H, short int *Out, unsigned int Wo, unsigned int Ho, unsigned int Fw, unsigned int Fh, unsigned int Stride, int Norm, v4s Pad, int ReLU)

{
	unsigned int PoolFactor = (1<<Norm)/(Fw*Fh);
	int PadL = 0, PadR = 0, PadT = 0, PadB = 0;

	if ((int) Pad) {
		PadL = Pad[0]; PadR = Pad[1]; PadT = Pad[2]; PadB = Pad[3];
	}
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)];
 			}
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
	}

        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
	}
	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)];
			}
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 15));
		}
	}
}

static void DoConv_fp(short int *In, int W, int H, short int *Filter, short int *Out, int Fw, int Fh, int Stride, unsigned int Norm, v4s Pad, int ReLU)

{
	int PadL = Pad[0]?((Fw-1)/2):0, PadR = Pad[1]?(Fw/2):0;
	int PadT = Pad[2]?((Fh-1)/2):0, PadB = Pad[3]?(Fh/2):0;

        int Wo = (W-Fw+PadL+PadR)/Stride + 1;
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho = (H-Fh+PadT+PadB)/Stride + 1;
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

	// printf("W: %d, H: %d, Wo: %d, Ho: %d. W:[%d..%d], H:[%d..%d], Pad: %d,%d  %d,%d\n", W, H, Wo, Ho, Wo_F, Wo_L, Ho_F, Ho_L, PadL, PadR, PadT, PadB);
        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
		}
	}
        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=Ho_F; h<Ho_L; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fh*i+j];
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
                }
        }
}

#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))

static unsigned short int IntegerExpLUT[] =
{
        0x0001, 0x0002, 0x0007, 0x0014, 0x0036, 0x0094, 0x0193, 0x0448, 0x0BA4, 0x1FA7, 0x560A, 0xE9E2
};

static unsigned short int FractionExpLUT[] =
{
        0x0000, 0x5BF1, 0x31CD, 0x0AF3, 0x4C90, 0x34E2, 0x36E3, 0x510B, 0x7A9F, 0x0ABE, 0x3B9F, 0x1224
};

/* 17.15 fixed point format */
static unsigned short int ExpCoeffLUT[] = {
        0x7FFF, 0x7FFF, 0x4000, 0x1555, 0x0555, 0x0111, 0x002E, 0x0007, 0x0001
};


/* X : fixed point, format Q17.15, returns in Q17.15 */
static unsigned int Exp_fp_17_15(unsigned int X)

{
        int  Y, Result, IntX, FractX, ScaledInt;
        short int Z_s, FractX_s;
        unsigned short int  ScaledFract;

        if (!X) return 0x8000;
        Y = Abs(X);
        IntX = (Y >> 15);
        if (IntX >= (int) ARRAYSIZE (IntegerExpLUT)) {
                if (Y==X) return 0x7FFFFFFF; else return 0;
        }
        FractX = (Y & 0x7FFF);
        if (gap8_bitextractu(FractX, 1, 14)) {
                /* Taylor series converges quickly only when | FractX | < 0.5 */
                FractX -= 0x8000; IntX++;
        }
        ScaledInt = IntegerExpLUT[IntX]; ScaledFract = FractionExpLUT[IntX];
        /* Taylor's series: exp(x) = 1 + x + x ^ 2 / 2 + x ^ 3 / 3! + x ^ 4 / 4! + x ^ 5 / 5! + x ^ 6 / 6! + x ^ 7 / 7! + x ^ 8 / 8!  */
        FractX_s = FractX; Z_s = FractX; Result = 0;
        for (int i = 1; i < ARRAYSIZE (ExpCoeffLUT); i++) {
                Result += Z_s*ExpCoeffLUT[i]; // gap8_macs(Result, Z, ExpCoeffLUT[ i ]);
                Z_s = gap8_mulsRN(Z_s, FractX_s, 15);
        }
        Result = gap8_roundnorm(Result, 15) + ExpCoeffLUT[0];
        unsigned short int U_Res = Result;
        Result = gap8_muluRN(U_Res, ScaledFract, 15) + U_Res * ScaledInt;
        if (Result && (X > 0x7FFFFFFF))
                Result = ((0x7FFFFFFF / Result) >> 1);      /* negative value */
        return (unsigned int) Result;
}

static void __attribute__ ((noinline)) DoSoftMax_fp(short int *__restrict__ In, short int *__restrict__ Out, unsigned int Dim, unsigned int Norm)

{
	int M, Sum, InvSum;

	M = 0x80000000;
	for (int i=0; i<Dim; i++) M = Max(M, In[i]);
        Sum = 0;
        for (int i=0; i<Dim; i++) {
                unsigned int Exp = Exp_fp_17_15((In[i]-M)<<(15-Norm));
                Out[i] = Exp; Sum += Exp;
        }
        InvSum = (FP2FIX(1.0, 15)<<15)/Sum;
        for (int i=0; i<Dim; i++) Out[i] = Abs(gap8_roundnorm_reg(Out[i]*InvSum, 15));
}

/**************************************************************************************************************************
 * Data Size = 1
 * Convolution, Pooling (Max, Average), Liner Rectification
 * Tensor Traversal
***************************************************************************************************************************/

static void __attribute__ ((noinline)) DoReLU_fps(signed char *__restrict__ In, signed char *__restrict__ Out, unsigned int W, unsigned int H)

{
        v4s * LineIn = (v4s *) In;
        v4s * LineOut = (v4s *) Out;

        for (unsigned int i=0; i<((W*H)/8); i++) {
                v4s X = LineIn[2*i], Y = LineIn[2*i+1];
                LineOut[2*i]   = gap8_max4(X, ((v4s){0,0,0,0}));
                LineOut[2*i+1] = gap8_max4(Y, ((v4s){0,0,0,0}));
        }
        for (unsigned int i=8*((W*H)/8); i<(W*H); i++) Out[i] = Max(In[i], 0);
}

static void DoMaxPool_fps(signed char *In, unsigned int W, unsigned int H, signed char *Out, unsigned int Wo, unsigned int Ho, unsigned int Fw, unsigned int Fh, unsigned int Stride, int Norm, v4s Pad, int ReLU)

{
	int PadL = 0, PadR = 0, PadT = 0, PadB = 0;
	if ((int) Pad) {
		PadL = Pad[0]; PadR = Pad[1]; PadT = Pad[2]; PadB = Pad[3];
	}
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0x80000000;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc = Max(Acc, In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]);
 			}
			Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}

        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Acc = 0x80000000;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc = Max(Acc, In[Y*W + X]);
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, Acc);
		}
	}
}

static void DoAvgPool_fps(signed char *In, unsigned int W, unsigned int H, signed char *Out, unsigned int Wo, unsigned int Ho, unsigned int Fw, unsigned int Fh, unsigned int Stride, int Norm, v4s Pad, int ReLU)

{
	unsigned int PoolFactor = (1<<Norm)/(Fw*Fh);
	int PadL = 0, PadR = 0, PadT = 0, PadB = 0;

	if ((int) Pad) {
		PadL = Pad[0]; PadR = Pad[1]; PadT = Pad[2]; PadB = Pad[3];
	}
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)];
 			}
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
	}

        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Acc = 0;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X];
				}
                        }
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
	}
	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = 0;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)];
			}
			Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc*PoolFactor, Norm), 7));
		}
	}
}

static void DoConv_fps(signed char *In, int W, int H, signed char *Filter, signed char *Out, int Fw, int Fh, int Stride, unsigned int Norm, v4s Pad, int ReLU)

{
	int PadL = Pad[0]?((Fw-1)/2):0, PadR = Pad[1]?(Fw/2):0;
	int PadT = Pad[2]?((Fh-1)/2):0, PadB = Pad[3]?(Fh/2):0;

        int Wo = (W-Fw+PadL+PadR)/Stride + 1;
        int Wo_F = FirstDefinedOutput(Fw, PadL, Stride), Wo_L = Max(Wo_F, LastDefinedOutput(W, Fw, PadL, Stride));
        int Ho = (H-Fh+PadT+PadB)/Stride + 1;
        int Ho_F = FirstDefinedOutput(Fh, PadT, Stride), Ho_L = Max(Ho_F, LastDefinedOutput(H, Fh, PadT, Stride));
	int ReVal = ReLU?0:0x80000000;

	// printf("W: %d, H: %d, Wo: %d, Ho: %d. W:[%d..%d], H:[%d..%d], Pad: %d,%d  %d,%d\n", W, H, Wo, Ho, Wo_F, Wo_L, Ho_F, Ho_L, PadL, PadR, PadT, PadB);
        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=0; h<Ho_F; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
		}
                for (unsigned int h=Ho_L; h<Ho; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (Y>=0&&Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
		}
	}
        for (unsigned int h=0; h<Ho; h++) {
        	for (unsigned int w=0; w<Wo_F; w++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
		}
        	for (unsigned int w=Wo_L; w<Wo; w++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) {
					int X = (w*Stride-PadL+j), Y = h*Stride-PadT+i;
					if (X>=0 && X<W && Y>=0 && Y<H) Acc += In[Y*W + X]*Filter[Fh*i+j];
				}
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
		}
	}
        for (unsigned int w=Wo_F; w<Wo_L; w++) {
                for (unsigned int h=Ho_F; h<Ho_L; h++) {
                        int Acc = Out[Wo*h+w]<<Norm;
                        for (unsigned int i=0; i<Fh; i++) {
                                for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fh*i+j];
                        }
                        Out[Wo*h+w] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
                }
        }
}

static void __attribute__ ((noinline)) DoSoftMax_fps(signed char *__restrict__ In, signed char *__restrict__ Out, unsigned int Dim, unsigned int Norm)

{
	int M, Sum, InvSum;

	M = 0x80000000;
	for (int i=0; i<Dim; i++) M = Max(M, In[i]);
        Sum = 0;
        for (int i=0; i<Dim; i++) {
                unsigned int Exp = Exp_fp_17_15((In[i]-M)<<(15-Norm));
                Out[i] = Exp; Sum += Exp;
        }
        InvSum = (FP2FIX(1.0, 7)<<7)/Sum;
        for (int i=0; i<Dim; i++) Out[i] = Abs(gap8_roundnorm_reg(Out[i]*InvSum, 7));
}

/**************************************************************************************************************************
 * Tensor initialization
 * Data Size = 4,2,1
***************************************************************************************************************************/

static void InitIn_fpd(int *In, unsigned int W, unsigned int H, int Offset, int VStep, int ResetOnly)

{
        int V = 0;

	if (ResetOnly) for (int i=0; i<(W*H); i++) In[i] = 0;
	else {
        	for (int Line=0; Line<H; Line++)
                	for (int Col=0; Col<W; Col++) {
                        	In[W*Line+Col] = Offset + V;
				V += VStep;
			}
	}
}

static void InitIn_fp(short int *In, unsigned int W, unsigned int H, int Offset, int VStep, int ResetOnly)

{
        int V = 0;

	if (ResetOnly) for (int i=0; i<(W*H); i++) In[i] = 0;
	else {
        	for (int Line=0; Line<H; Line++)
                	for (int Col=0; Col<W; Col++) {
                        	In[W*Line+Col] = Offset + V;
				V += VStep;
			}
	}
}

static void InitFilter_fp(short int *Filter, unsigned int FSw, unsigned int FSh, int P, int FVal, int VStep, int FStep)

{
        for (int p=0; p<P; p++) {
		int V = FVal;
                for (int f=0; f<(FSw*FSh); f++) {
			Filter[p*FSw*FSh + f] = V+p*FStep; V += VStep;
		}
	}
}

static void InitIn_fps(signed char *In, unsigned int W, unsigned int H, int Offset, int VStep, int ResetOnly)

{
        int V = 0;

        if (ResetOnly) for (int i=0; i<(W*H); i++) In[i] = 0;
	else {
        	for (int Line=0; Line<H; Line++)
                	for (int Col=0; Col<W; Col++) {
                        	In[W*Line+Col] = Offset + V;
				V += VStep;
			}
	}
}

static void InitFilter_fps(signed char *Filter, unsigned int FSw, unsigned int FSh, int P, int FVal, int VStep, int FStep)

{
        for (int p=0; p<P; p++) {
		int V = FVal;
                for (int f=0; f<(FSw*FSh); f++) {
			Filter[p*FSw*FSh + f] = V+p*FStep; V += VStep;
		}
	}
}

/************************************************************************
 Generic Interfaces 
	InitTensor
	EvaluateLayer
	CheckSum

	DumpPlane
	DumpFeaturePlanes
	Dump2DFeaturePlanes
************************************************************************/

int InitTensor(v4s DataSize, unsigned int InP, unsigned int OutP, unsigned int W, unsigned int H, unsigned int Wo, unsigned int Ho,
		unsigned int FSw, unsigned int FSh,
		void *Gen_In, void *Gen_Filter, void *Gen_Bias, void *Gen_Out, void *Gen_BufferOut,
		int IStep, int OStep, int FStep, int FVal, int VStep, int BVal, int BStep)


{
	int DS;
	int InDS, FilterDS, BiasDS, OutDS;

	if (DataSize[0]==DataSize[1] && DataSize[1]==DataSize[2] && DataSize[2]==DataSize[3]) DS = DataSize[0];
	else {
		DS = 0;
		InDS = DataSize[0];
		FilterDS = DataSize[1];
		BiasDS = DataSize[2];
		OutDS = DataSize[3];
	}
	if (DS==2) {
		short int *In = (short int *) Gen_In;
		short int *Filter = (short int *) Gen_Filter;
		short int *Bias = (short int *) Gen_Bias;
		short int *Out = (short int *) Gen_Out;
		short int *BufferOut = (short int *) Gen_BufferOut;

        	for (int p=0; p<InP; p++) InitIn_fp(In + p*W*H, W, H, p*IStep, IStep, 0);
        	for (int p=0; p<OutP; p++) InitIn_fp(Out + p*(Wo)*(Ho), Wo, Ho, 0, 0, 1);
		if (BufferOut) InitIn_fp(BufferOut + (Wo)*(Ho), Wo, Ho, 0, 0, 1);
        	for (int op=0; op<OutP; op++) {
               		if (Filter) InitFilter_fp(Filter + op*InP*FSw*FSh, FSw, FSh, InP, FVal+2*op, VStep, (op+1)*FStep);
               		if (Bias) Bias[op] = BVal + op*BStep;
        	}
	} else if (DS==1) {
		signed char *In = (signed char *) Gen_In;
		signed char *Filter = (signed char *) Gen_Filter;
		signed char *Bias = (signed char *) Gen_Bias;
		signed char *Out = (signed char *) Gen_Out;
		signed char *BufferOut = (signed char *) Gen_BufferOut;

        	for (int p=0; p<InP; p++) InitIn_fps(In + p*W*H, W, H, p*IStep, IStep, 0);
        	for (int p=0; p<OutP; p++) InitIn_fps(Out + p*(Wo)*(Ho), Wo, Ho, 0, 0, 1);
		if (BufferOut) InitIn_fps(BufferOut + (Wo)*(Ho), Wo, Ho, 0, 0, 1);
        	for (int op=0; op<OutP; op++) {
               		if (Filter) InitFilter_fps(Filter + op*InP*FSw*FSh, FSw, FSh, InP, FVal+2*op, VStep, (op+1)*FStep);
               		if (Bias) Bias[op] = BVal + op*BStep;
        	}
	} else if (DS==0) {
		if (InDS==2&&FilterDS==1&&BiasDS==2&&OutDS==2) {
			short int *In = (short int *) Gen_In;
			signed char *Filter = (signed char *) Gen_Filter;
			short int *Bias = (short int *) Gen_Bias;
			short int *Out = (short int *) Gen_Out;
			short int *BufferOut = (short int *) Gen_BufferOut;

        		for (int p=0; p<InP; p++) InitIn_fp(In + p*W*H, W, H, p*IStep, IStep, 0);
        		for (int p=0; p<OutP; p++) InitIn_fp(Out + p*(Wo)*(Ho), Wo, Ho, 0, 0, 1);
			if (BufferOut) InitIn_fp(BufferOut + (Wo)*(Ho), Wo, Ho, 0, 0, 1);
        		for (int op=0; op<OutP; op++) {
               			if (Filter) InitFilter_fps(Filter + op*InP*FSw*FSh, FSw, FSh, InP, FVal+2*op, VStep, (op+1)*FStep);
               			if (Bias) Bias[op] = BVal + op*BStep;
        		}
		} else if (InDS==2&&FilterDS==2&&BiasDS==2&&OutDS==4) {
			short int *In = (short int *) Gen_In;
			short int *Filter = (short int *) Gen_Filter;
			short int *Bias = (short int *) Gen_Bias;
			int *Out = (int *) Gen_Out;
			int *BufferOut = (int *) Gen_BufferOut;

        		for (int p=0; p<InP; p++) InitIn_fp(In + p*W*H, W, H, p*IStep, IStep, 0);
        		for (int p=0; p<OutP; p++) InitIn_fpd(Out + p*(Wo)*(Ho), Wo, Ho, 0, 0, 1);
			if (BufferOut) InitIn_fpd(BufferOut + (Wo)*(Ho), Wo, Ho, 0, 0, 1);
        		for (int op=0; op<OutP; op++) {
               			if (Filter) InitFilter_fp(Filter + op*InP*FSw*FSh, FSw, FSh, InP, FVal+2*op, VStep, (op+1)*FStep);
               			if (Bias) Bias[op] = BVal + op*BStep;
        		}
		} else if (InDS==2&&FilterDS==0&&BiasDS==0&&OutDS==2) {
			short int *In = (short int *) Gen_In;
        		for (int p=0; p<InP; p++) InitIn_fp(In + p*W*H, W, H, p*IStep, IStep, 0);
		} else if (InDS==1&&FilterDS==0&&BiasDS==0&&OutDS==1) {
			signed char *In = (signed char *) Gen_In;
        		for (int p=0; p<InP; p++) InitIn_fps(In + p*W*H, W, H, p*IStep, IStep, 0);
		} else return 0;
	} else return 0;
	return 1;
}

int EvaluateLayer(
		v4s DataSize,
		unsigned int InFeat, unsigned int OutFeat, unsigned int W, unsigned int H,
		void *Gen_In, void *Gen_Filter, void *Gen_Bias, void *Gen_Out, void *Gen_Buffer,
		int ConvStride, int Fwc, int Fhc, v4s DoPadc, int ReLUc, int DoConv,
		int PoolStride, int Fwp, int Fhp, v4s DoPadp, int ReLUp, int DoPool,
		int DoReLU, int DoLinear, int DoSoftMax,
		unsigned int Norm, unsigned int NormBias)

{
	int PadLc = DoPadc[0]?((Fwc-1)/2):0, PadRc = DoPadc[1]?(Fwc/2):0;
	int PadTc = DoPadc[2]?((Fhc-1)/2):0, PadBc = DoPadc[3]?(Fhc/2):0;

        int Woc = DoConv?((W-Fwc+PadLc+PadRc)/ConvStride + 1):W;
        int Hoc = DoConv?((H-Fhc+PadTc+PadBc)/ConvStride + 1):H;
	v4s Padc = (v4s) {PadLc,PadRc,PadTc,PadBc};

	int PadLp = DoPadp[0]?((Fwp-1)/2):0, PadRp = DoPadp[1]?(Fwp/2):0;
	int PadTp = DoPadp[2]?((Fhp-1)/2):0, PadBp = DoPadp[3]?(Fhp/2):0;

        int Wop = DoPool?((Woc-Fwp+PadLp+PadRp)/PoolStride + 1):Woc;
        int Hop = DoPool?((Hoc-Fhp+PadTp+PadBp)/PoolStride + 1):Hoc;
	v4s Padp = (v4s) {PadLp,PadRp,PadTp,PadBp};

	int DS;
	int InDS, FilterDS, BiasDS, OutDS;

	if (DoLinear) {
		if (DataSize[0]==DataSize[1] && DataSize[1]==DataSize[2] && DataSize[2]==DataSize[3]) DS = DataSize[0];
		else {
			DS = 0;
			InDS = DataSize[0];
			FilterDS = DataSize[1];
			BiasDS = DataSize[2];
			OutDS = DataSize[3];
		}
	} else {
		if (DoConv) {
			if (DataSize[0]==DataSize[1] && DataSize[1]==DataSize[2] && DataSize[2]==DataSize[3]) DS = DataSize[0]; else return 0;
		} else if (DoPool || DoReLU || DoSoftMax) {
			if (DataSize[0]==DataSize[3]) DS = DataSize[0]; else return 0;
		} else return 0;
	}
	if (DoConv) {
		/* Convolution followed by optional Pooling followed by optional ReLU */
		if (DS==2) {
			short int *In = (short int *) Gen_In;
			short int *Filter = (short int *) Gen_Filter;
			short int *Bias = (short int *) Gen_Bias;
			short int *Out = (short int *) Gen_Out;
			short int *Buffer = (short int *) Gen_Buffer;
			switch (DoPool) {
				case 0: // No Pooling
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Out[Woc*Hoc*of + i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fp(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Out+Woc*Hoc*of, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
					}
					break;
				case 1: // MaxPool
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Buffer[i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fp(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Buffer, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
						DoMaxPool_fp(Buffer, Woc, Hoc, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					}
					break;
				case 2: // AvgPool
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Buffer[i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fp(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Buffer, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
						DoAvgPool_fp(Buffer, Woc, Hoc, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					}
					break;
				default: return 0;
			}
		} else if (DS==1) {
			signed char *In = (signed char *) Gen_In;
			signed char *Filter = (signed char *) Gen_Filter;
			signed char *Bias = (signed char *) Gen_Bias;
			signed char *Out = (signed char *) Gen_Out;
			signed char *Buffer = (signed char *) Gen_Buffer;
			switch (DoPool) {
				case 0: // No Pooling
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Out[Woc*Hoc*of + i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fps(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Out+Woc*Hoc*of, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
					}
					break;
				case 1: // MaxPool
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Buffer[i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fps(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Buffer, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
						DoMaxPool_fps(Buffer, Woc, Hoc, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					}
					break;
				case 2: // AvgPool
					for (unsigned int of=0; of<OutFeat; of++) {
						for (unsigned int i=0; i<(Woc*Hoc); i++) Buffer[i] = Bias[of];
						for (unsigned int If=0; If<InFeat; If++) {
							DoConv_fps(In+W*H*If, W, H, Filter+Fwc*Fhc*(InFeat*of + If), Buffer, Fwc, Fhc, ConvStride, Norm, Padc, ReLUc && (If==(InFeat-1)));
						}
						DoAvgPool_fps(Buffer, Woc, Hoc, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					}
					break;
				default: return 0;
			}
		} else return 0;
	} else if (DoPool) {
		if (DataSize[0]==DataSize[3]) DS = DataSize[0]; else DS = 0;
		/* Pooling followed by optional ReLU */
		if (DS==2) {
			short int *In = (short int *) Gen_In;
			short int *Out = (short int *) Gen_Out;
			switch (DoPool) {
				case 1: // MaxPool
					for (unsigned int of=0; of<OutFeat; of++) DoMaxPool_fp(In+W*H*of, W, H, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					break;
				case 2: // AvgPool
					for (unsigned int of=0; of<OutFeat; of++) DoAvgPool_fp(In+W*H*of, W, H, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					break;
				default: ;
			}
		} else if (DS==1) {
			signed char *In = (signed char *) Gen_In;
			signed char *Out = (signed char *) Gen_Out;
			switch (DoPool) {
				case 1: // MaxPool
					for (unsigned int of=0; of<OutFeat; of++) DoMaxPool_fps(In+W*H*of, W, H, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					break;
				case 2: // AvgPool
					for (unsigned int of=0; of<OutFeat; of++) DoAvgPool_fps(In+W*H*of, W, H, Out+Wop*Hop*of, Wop, Hop, Fwp, Fhp, PoolStride, Norm, Padp, ReLUp);
					break;
				default: return 0;
			}
		} else return 0;
	} else if (DoLinear) {
		/* Linear layer followed by optional ReLU */
		int ReVal = DoReLU?0:0x80000000;
		int InDim=InFeat, OutDim=OutFeat;
		if (DS==2) {
			short int *In = (short int *) Gen_In;
			short int *Filter = (short int *) Gen_Filter;
			short int *Bias = (short int *) Gen_Bias;
			short int *Out = (short int *) Gen_Out;
			for (unsigned int of=0; of<OutDim; of++) {
				int Acc = Bias[of]<<Norm;
				for (unsigned int If=0; If<InDim; If++) Acc += In[If]*Filter[InDim*of+If];
                        	Out[of] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
			}
		} else if (DS==1) {
			signed char *In = (signed char *) Gen_In;
			signed char *Filter = (signed char *) Gen_Filter;
			signed char *Bias = (signed char *) Gen_Bias;
			signed char *Out = (signed char *) Gen_Out;
			for (unsigned int of=0; of<OutDim; of++) {
				int Acc = Bias[of]<<Norm;
				for (unsigned int If=0; If<InDim; If++) Acc += In[If]*Filter[InDim*of+If];
                        	Out[of] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 7));
			}
		} else if (DS==0) {
			if (InDS==2&&FilterDS==1&&BiasDS==2&&OutDS==2) {
				short int *In = (short int *) Gen_In;
				signed char *Filter = (signed char *) Gen_Filter;
				short int *Bias = (short int *) Gen_Bias;
				short int *Out = (short int *) Gen_Out;
				for (unsigned int of=0; of<OutDim; of++) {
					int Acc = Bias[of]<<NormBias;
					for (unsigned int If=0; If<InDim; If++) Acc += In[If]*Filter[InDim*of+If];
                        		Out[of] = Max(ReVal, gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
				}
			} else if (InDS==2&&FilterDS==2&&BiasDS==2&&OutDS==4) {
				short int *In = (short int *) Gen_In;
				short int *Filter = (short int *) Gen_Filter;
				short int *Bias = (short int *) Gen_Bias;
				int *Out = (int *) Gen_Out;
				for (unsigned int of=0; of<OutDim; of++) {
					int Acc = Bias[of]<<Norm;
					for (unsigned int If=0; If<InDim; If++) Acc += In[If]*Filter[InDim*of+If];
                        		Out[of] = Max(ReVal, gap8_roundnorm_reg(Acc, Norm));
				}
			} else return 0;
		} else return 0;
	} else if (DoReLU) {
		if (DataSize[0]==DataSize[3]) DS = DataSize[0]; else DS = 0;
		/* ReLU alone */
		if (DS==2) {
			short int *In = (short int *) Gen_In;
			short int *Out = (short int *) Gen_Out;
			for (unsigned int If=0; If<InFeat; If++) DoReLU_fp(In+W*H*If, Out+W*H*If, W, H);
		} else if (DS==1) {
			signed char *In = (signed char *) Gen_In;
			signed char *Out = (signed char *) Gen_Out;
			for (unsigned int If=0; If<InFeat; If++) DoReLU_fps(In+W*H*If, Out+W*H*If, W, H);
		} else return 0;
	} else if (DoSoftMax) {
		if (DataSize[0]==DataSize[3]) DS = DataSize[0]; else DS = 0;
		if (DS==2) {
			short int *In = (short int *) Gen_In;
			short int *Out = (short int *) Gen_Out;
			DoSoftMax_fp(In, Out, H, Norm);
		} else if (DS==1) {
			signed char *In = (signed char *) Gen_In;
			signed char *Out = (signed char *) Gen_Out;
			DoSoftMax_fps(In, Out, H, Norm);
		} else return 0;
	}
	return 1;
}

unsigned int CheckSum(int DataSize, void *Gen_In, int Size)

{
        int i;
        unsigned S=0;

	if (DataSize==1) {
		signed char *In = (signed char *) Gen_In;
        	for (i=0; i<Size; i++) S += In[i];
	} else if (DataSize==2) {
		short int *In = (short int *) Gen_In;
        	for (i=0; i<Size; i++) S += In[i];
	} else if (DataSize==4) {
		int *In = (int *) Gen_In;
        	for (i=0; i<Size; i++) S += In[i];
	}
        return S;
}

void DumpPlane(char *Mess, int DataSize, void *Plane, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax)

{
        int i, j;
	int CPT, MaxW=20;

        printf("----------------- %s ------------------------\n", Mess);
	printf("  : "); CPT=0; for (j=0; j<Min(W, Wmax); j++) {printf("%8d ", j); CPT++; if (CPT>MaxW) {printf("\n  : "); CPT=0;}} printf("\n");
        for (i=0; i<Min(H, Hmax); i++) {
                printf("%2d: ", i); CPT = 0;
                for (j=0; j<Min(W, Wmax); j++) {
			if      (DataSize==1) printf("%8d ", (int) ((signed char *) Plane)[i*W+j]);
			else if (DataSize==2) printf("%8d ", (int) ((short int *) Plane)[i*W+j]);
			else if (DataSize==4) printf("%8d ", (int) ((int *) Plane)[i*W+j]);
			CPT++;
			if (CPT>MaxW) {
				printf("\n    "); CPT = 0;
			}
                }
                printf("\n");
        }
        printf("-----------------------------------------\n");
}

void DumpFeaturePlanes(char *Mess, int DataSize, void *Plane, unsigned int NPlanes, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax)

{
	char Str[50];

	for (unsigned int i=0; i<NPlanes; i++) {
		sprintf(Str, "%s %d", Mess, i);
		DumpPlane(Str, DataSize, (void *) ((char *) Plane + DataSize*i*W*H), W, Wmax, H, Hmax);
	}
}


void Dump2DFeaturePlanes(char *Mess, int DataSize, void *Plane, unsigned int InP, unsigned int OutP, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax)

{
	char Str[50];

	for (unsigned int i=0; i<OutP; i++) {
		sprintf(Str, "%s OutP%d", Mess, i);
		DumpFeaturePlanes(Str, DataSize, (void *) ((char *) Plane + DataSize*i*W*H*InP), InP, W, Wmax, H, Hmax);
	}
}
