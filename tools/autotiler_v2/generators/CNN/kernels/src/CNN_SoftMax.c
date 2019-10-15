#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels.h"

#define Minu(a, b)              (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

#ifdef __pulp__
#define Abs(a)			__builtin_pulp_abs((a))
#define Min(a, b)               __builtin_pulp_minsi((a), (b))
#define Max(a, b)               __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)			(((int)(a)<0)?(-(a)):(a))
#define Min(a, b)               (((a)<(b))?(a):(b))
#define Max(a, b)               (((a)>(b))?(a):(b))
#endif

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


#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))

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

void KerParSoftMax_fp(KerParSoftMax_fp_T *Arg)

{
	short int * __restrict__ In = Arg->In;
	short int * __restrict__ Out = Arg->Out;
	int N = Arg->N;
	unsigned Norm = Arg->Norm;
	static L1_CL_MEM int Reduct[8];
	int M, Sum, InvSum;
	unsigned int CoreId = gap8_coreid();
	unsigned int ChunkCell = ChunkSize(N);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, N);
	unsigned int *Red = &Reduct[CoreId];

	/* Turns In into distribution
	/* Find max */
	M = 0x80000000;
	for (int i=First; i<Last; i++) M = Max(M, In[i]);
	Reduct[CoreId] = M;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		M = Reduct[0];
		// for (int i=1; i<gap8_ncore(); i++) M = Max(M, Reduct[i]);
		for (int i=1; i<8; i++) M = Max(M, Reduct[i]);
		Reduct[0] = M;

	}
	gap8_waitbarrier(0);
	/* Computes Exp(In[i]-M) for all in and sum results.
	   Since we substract max from In[i] we always have exp(X) with X<=0 thus exp(X)<=1.0
	   By definition of softmax Sum is <= 1
	*/
	M = Reduct[0];
	Sum = 0;
	for (int i=First; i<Last; i++) {
		unsigned int Exp = Exp_fp_17_15((In[i]-M)<<(15-Norm));
		Out[i] = Exp; Sum += Exp;
	}
	Reduct[CoreId] = Sum;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		Sum = 0;
		// for (int i=0; i<gap8_ncore(); i++) Sum += Reduct[i];
		for (int i=0; i<8; i++) Sum += Reduct[i];
		Reduct[0] = Sum;
	}
	gap8_waitbarrier(0);
	Sum = Reduct[0];
	InvSum = (FP2FIX(1.0, 15)<<15)/Sum;
	for (int i=First; i<Last; i++) Out[i] = Abs(gap8_roundnorm_reg(Out[i]*InvSum, 15));
	gap8_waitbarrier(0);

}


void KerParSoftMax_fps(KerParSoftMax_fps_T *Arg)

{
	signed char * __restrict__ In = Arg->In;
	signed char * __restrict__ Out = Arg->Out;
	int N = Arg->N;
	unsigned Norm = Arg->Norm;
	static L1_CL_MEM int Reduct[8];
	int M, Sum, InvSum;
	unsigned int CoreId = gap8_coreid();
	unsigned int ChunkCell = ChunkSize(N);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, N);
	unsigned int *Red = &Reduct[CoreId];

	/* Turns In into distribution
	/* Find max */
	M = 0x80000000;
	for (int i=First; i<Last; i++) M = Max(M, In[i]);
	Reduct[CoreId] = M;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		M = Reduct[0];
		// for (int i=1; i<gap8_ncore(); i++) M = Max(M, Reduct[i]);
		for (int i=1; i<8; i++) M = Max(M, Reduct[i]);
		Reduct[0] = M;

	}
	gap8_waitbarrier(0);
	/* Computes Exp(In[i]-M) for all in and sum results.
	   Since we substract max from In[i] we always have exp(X) with X<=0 thus exp(X)<=1.0
	   By definition of softmax Sum is <= 1
	*/
	M = Reduct[0];
	Sum = 0;
	for (int i=First; i<Last; i++) {
		unsigned int Exp = Exp_fp_17_15((In[i]-M)<<(15-Norm));
		Out[i] = Exp>>8; Sum += Exp;
	}
	Reduct[CoreId] = Sum;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		Sum = 0;
		// for (int i=0; i<gap8_ncore(); i++) Sum += Reduct[i];
		for (int i=0; i<8; i++) Sum += Reduct[i];
		Reduct[0] = Sum;
	}
	Sum = Reduct[0];
	InvSum = (FP2FIX(1.0, 7)<<7)/Sum;
	gap8_waitbarrier(0);
	for (int i=First; i<Last; i++) Out[i] = Abs(gap8_roundnorm_reg(Out[i]*InvSum, 7));
	gap8_waitbarrier(0);

}
