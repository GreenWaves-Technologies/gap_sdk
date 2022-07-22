/* NOTE: retrieved from hardware testbench */

#include "ffc_test_utils.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define FIX2FP(Val, Precision)          ((double) (Val) / (double) (1<<(Precision)))
#define FP2FIXR(Val, Precision)         ((int) round((Val) * (long long int)((long long int)1<<(Precision))))
#define FP2FIX(Val, Precision)          ((int) ((Val) * (long long int)(1 << (Precision))))

#define gap_bitextract(x, size, off)    (((((x)>>(off))&((unsigned int)(1<<(size))-1))<<(32-(size)))>>(32-(size)))
#define gap_bitextractu(x, size, off)   (((x)>>(off))&((unsigned int)(1<<(size))-1))

#define gap_bitinsert(dst, src, size, off)      (((dst) & ~(((1<<(size))-1)<<(off))) | (((src) & ((1<<(size))-1))<<(off)))

#define gap_abs(x)                      (((x)<0)?-(x):(x))


/*

	Fractional: Q(Size-n).n, n bits for fractonal part, Size-n for integer part, Size = 8, 16, 24, 32

	To a fractional can be attached a scale factor  Qn * (2^Scale) where Scale can be >0 or <0.


	s: number of bits for sign, always 1
	e: number of bits for the exponent
	m: number of bits for the mantissa

	Actual values for sign, exponent and mantissa
	S, E, M

	E=0		-1^S  *  2^(e-1)-2          *  M=0.fraction	SubNormal
	E=[1..2^e-2]	-1^S  *  2^(E-(2^(e-1)-1))  *  M=1.fraction	Normal
	E=2^e-1		-1^S  *  Infinity				+/- Infinity

F32:
	s=1, e=8, m=23		IEEE754, float32
	E=0		-1^S  *  2^-126     *  M=0.fraction
	E=[1..254]	-1^S  *  2^(E-127)  *  M=1.fraction
	E=255		-1^S  *  Infinity

F16:
	s=1, e=5, m=10		IEEE754, float16
	E=0		-1^S  *  2^-14      *  M=0.fraction
	E=[1..30]	-1^S  *  2^(E-15)   *  M=1.fraction
	E=31		-1^S  *  Infinity

B16:
	s=1, e=8, m=7		bfloat16
	E=0		-1^S  *  2^-126     *  M=0.fraction
	E=[1..254]	-1^S  *  2^(E-127)  *  M=1.fraction
	E=255		-1^S  *  Infinity

*/

/*
	Convert In, a float [S, E, M] in this order, in a fixed point In  in Q(FixedSize-Precision).Precision scaled by 2^Scale.

	with S on SignSize bits
	with E on ExpSize bits
	with M on MantSize bits

	In practice we need only to deal with S,E,M for F32, F16 and B16

	FixedSize is 8, 16, 24 or 32 bits

	If lower part of Mantissa of In has to be eliminated (>>) then rounding to nearest integer is performed.
*/

unsigned int FloatToFixedFormat(unsigned int In, int FixedSize, int Precision, int Scale, unsigned int SignSize, unsigned int ExpSize, unsigned int MantSize)

{
	int S = gap_bitextractu(In,  SignSize, ExpSize+MantSize);
	int E = gap_bitextractu(In,  ExpSize,  MantSize);
	int M = gap_bitextractu(In,  MantSize, 0);
	int Out;

	if (E == ((1<<ExpSize) - 1)) {						/* Infinity: (1<<ExpSize) - 1)  F32: 0XFF, F16: 0x1F, B16: 0xFF */
		/* Infinity */
		if (S) Out = (0x1 << (FixedSize-1)); 				/* Negative saturation: 8: 0x80, 16: 0X8000, 24: 0X800000, 32: 0X80000000 */
		else Out = ~(0x1 << (FixedSize-1));				/* Positive saturation: 8: 0x7F, 16: 0x7FFF, 24: 0x7FFFFF, 32, 0X7FFFFFFF */
	} else {
		int Norm, NormMant;
		if (E==0) {
			/* Sub normal: 2^(-SubNormal) * M * 2^(-MantSize) */
			E =   - ((1<<(ExpSize-1))-2) + (Precision + Scale);	/* SubNormal: (((1<<(ExpSize-1))-2) => F32=126, F16=14, B16=126 */
			NormMant = 32 - __builtin_clz(M);
		} else {
			/* Normal: 2^(E-ExpBias) * (1 + M * 2^(-MantSize))  */
			E = E - ((1<<(ExpSize-1))-1) + (Precision + Scale);	/* ExpBias: ((1<<(ExpSize-1))-1)=> F32=127, F16=15, B16=127 */
			M = (1<<MantSize)|(M);					/* (1<<MantSize) => F32=0x800000, F16=0X400, B16=0X80 */
			NormMant = MantSize+1;
		}
		if (S)
        {
            M = -M;
        }
		Norm = E - (int) MantSize;
		if (Norm>=(FixedSize-NormMant))
        {
			if (M)
            {
				if (S)
                {
                    Out = (0x1 << (FixedSize-1)); 		/* Negative saturation: 8: 0x80, 16: 0X8000, 24: 0X800000, 32: 0X80000000 */
                }
				else
                {
                    Out = ~(0x1 << (FixedSize-1));		/* Positive saturation: 8: 0x7F, 16: 0x7FFF, 24: 0x7FFFFF, 32, 0X7FFFFFFF */
                }
			}
            else
            {
                Out = 0;
            }
		} else if ((-Norm)>=NormMant)
        {
            Out = 0;
        }
		else if (Norm >= 0)
        {
            Out = M<<Norm;
        }
		else
        {
            Out = ((M+(1<<(-Norm-1))))>>(-Norm);			/* Round to nearest value equiv to +0.5: + (1<<(-Norm-1))  */
        }
	}
	Out = Out & (unsigned int) (((long long int)0x1 << FixedSize)-1);	/* MaskOut            : 8: 0xFF, 16: 0xFFFF, 24: 0xFFFFFF, 32, 0xFFFFFFFF */
	return Out;
}

/*
	Convert fixed point In  in Q(FixedSize-Precision).Precision scaled by 2^Scale into a float [S, E, M] in this order

	with S on SignSize bits
	with E on ExpSize bits
	with M on MantSize bits

	In practice we need only to deal with S,E,M for F32, F16 and B16

	FixedSize is 8, 16, 24 or 32 bits

	If lower part of In has to be eliminated (>>) then rounding to nearest integer is performed.
*/

unsigned int FixedToFloatFormat(int In, int FixedSize, int Precision, int Scale, unsigned int SignSize, unsigned int ExpSize, unsigned int MantSize)

{
	int Sign, Exponent=0, Mantissa=0, Out=0;
	In = In & (unsigned int) (((long long int)0x1 << FixedSize)-1);
	Sign = gap_bitextractu(In, 1, FixedSize-1);
	In = ((FixedSize !=32) && (Sign == 1)) ? gap_bitinsert(In, 0xFFFFFFF, 32-FixedSize, FixedSize) : In;
	In = gap_abs(In);
	if (In!=0) {
		int Norm = 0;
		int Significand = 32 - __builtin_clz(In);					/* clz: Number of leading zeros for In, Significand: number of significand bits */
	       	Exponent = Significand - (Precision+Scale) - 1 + ((1<<(ExpSize-1)) - 1);	/* ExpBias: ((1<<(ExpSize-1)) - 1) => F32=127, F16=15, B16=127 */
		if (Exponent > ((1<<ExpSize)-2)) {						/* MaxExp:((1<<ExpSize)-2) => F32=254, F16=30, B16=254 */
			/* Infinity */
			Exponent = (1<<ExpSize) - 1;						/* Infinite: ((1<<ExpSize) - 1) => F32=255, F16=31, B16=255 */
		} else if (Exponent <= 0) {
			/* Sub Normal */
			Norm = (((1<<(ExpSize-1))-2) + MantSize) - (Precision+Scale);		/* SubNormal: (((1<<(ExpSize-1))-2) => F32=126, F16=14, B16=126 */
			Exponent = 0;
			Mantissa = In;
		} else {
			/* Normal */
			Norm = MantSize - Significand + 1;
			Mantissa = In;
		}
		if (Norm>=0) Mantissa = Mantissa<<Norm;
		else Mantissa = (Mantissa+(1<<(-Norm-1)))>>(-Norm);				/* Round to nearest value equiv to +0.5: + (1<<(-Norm-1))  */
		Mantissa &= ((1<<MantSize)-1);							/* MantissaMask: ((1<<MantSize)-1) => F32=0x7FFFFF, F16=0X3FF, B16=0x7F */
	}
	Out = gap_bitinsert(Out, Sign,     SignSize, ExpSize+MantSize);				/* Width in bits, Offset in bits */
	Out = gap_bitinsert(Out, Exponent,  ExpSize, MantSize);
	Out = gap_bitinsert(Out, Mantissa, MantSize, 0);

	return Out;
}
