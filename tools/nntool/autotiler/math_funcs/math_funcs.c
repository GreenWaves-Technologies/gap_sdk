/*
 * Copyright 2021 GreenWaves Technologies, SAS
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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"

#include "math_funcs.h"

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef signed long long int64;
typedef signed int int32;
typedef signed short int16;
typedef signed char int8;

#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))

/**
 * ln(2) in 1.15 format
 */

#define	LN_2_1F15	0x000058B9


/* square root coefficients in 17.15 fixed point format */

int SqrtCoeffTable[] =
{
	0x00001A91, 0x0000BA3A, 0xFFFF53DA, 0x00008DAC,
	0xFFFFBB54, 0x00000E5A
};

/* square root of 2 powered to (index - 15) in 17.15 format */

int Sqrt2Powers[] =
{
	0x000000B5,	0x00000100, 0x0000016A, 0x00000200,
	0x000002D4,	0x00000400, 0x000005A8, 0x00000800,
	0x00000B50,	0x00001000, 0x000016A1, 0x00002000,
	0x00002D41,	0x00004000, 0x00005A82, 0x00008000,
	0x0000B505,	0x00010000, 0x00016A0A, 0x00020000,
	0x0002D414,	0x00040000, 0x0005A828, 0x00080000,
	0x000B504F,	0x00100000, 0x0016A09E, 0x00200000,
	0x002D413D,	0x00400000, 0x005A827A, 0x00800000
};

/* natural logarithm coefficients in 17.15 fixed point format */

int LognCoeffTable[] = // N
{
	0x00007FE3, 0xFFFFC149, 0x00002491, 0xFFFFEEF8,
	0x00000404
};

/* integer and fraction parts of exponent values */

static unsigned short int IntegerExpLUT[] =
{
        0x0001, 0x0002, 0x0007, 0x0014,
		0x0036, 0x0094, 0x0193, 0x0448,
		0x0BA4, 0x1FA7, 0x560A, 0xE9E2
};

static unsigned short int FractionExpLUT[] =
{
        0x0000, 0x5BF1, 0x31CD, 0x0AF3,
		0x4C90, 0x34E2, 0x36E3, 0x510B,
		0x7A9F, 0x0ABE, 0x3B9F, 0x1224
};

/* 17.15 fixed point format */
static unsigned short int ExpCoeffLUT[] = {
        0x7FFF, 0x7FFF, 0x4000, 0x1555,
		0x0555, 0x0111, 0x002E, 0x0007,
		0x0001
};

uint32 sqrt_17_15(uint32 x)

{
	unsigned int exponent, y, result, z;

	if (!x)
		return 0;
	if (x > 0x7FFFFFFF) return 0;	/* negative value */
	exponent = (unsigned int) gap_clb ((int) x);
	y = ((x << exponent) >> 16);

	/* sqrt(x) = 0.2075806 + 1.454895 * x - 1.34491 * x^2 + 1.106812 * x^3 - 0.536499 * x^4 + 0.1121216 * x^5 */
	z = y;
	result = 0;
	for (int i = 1; i < ARRAYSIZE (SqrtCoeffTable); i++) {
		result += z * SqrtCoeffTable[ i ];
		z = ((z * y) >> 15);
	}
	result >>= 15;
	result += SqrtCoeffTable[ 0 ];
	if (exponent != 16) {
		if (exponent < 12) 	result = (((result>>(12-exponent)) * Sqrt2Powers[ 31 - exponent ]) >> (15-(12-exponent)));
		else            	result = ((result * Sqrt2Powers[ 31 - exponent ]) >> 15);
	}

	return (uint32) result;
}

uint32 logn_17_15(uint32 x)
{
	register unsigned int   i, exponent;
	register int            result, y, z;

	if (!x)
		return 0x80000000;
	if (x > 0x7FFFFFFF)
		return 0x80000000;	/* negative value */

	// exponent = (unsigned int) norm ((int) x);
	exponent = (unsigned int) gap_clb ((int) x);
	y = ((x << exponent) >> 15) - 0x8000;

	/**
	 * ln(x) = 0.9991150 * (x -1) - 0.4899597 * (x -1) ^ 2
	 *       + 0.2856751 * (x -1) ^ 3 - 0.1330566 * (x -1) ^ 4
	 *       + 0.03137207 * (x -1) ^ 5
	 */
	z = ((y * y) >> 15);
	result = LognCoeffTable[ 0 ] * y;
	for (i = 1; i < ARRAYSIZE (LognCoeffTable); i++)
	{
		result += z * LognCoeffTable[ i ];
		z = ((z * y) >> 15);
	}
	result >>= 15;
	if (exponent != 15)
		result += LN_2_1F15 * (15 - exponent);

	return (uint32) result;
}

#define gap_mulshRN(x, y, n)     (short int) gap_mulsRN(x, y, n)
#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))

/* X : fixed point, format Q17.15, returns in Q17.15 */
unsigned int exp_17_15(unsigned int X)

{
        int  Y, Result, IntX, FractX, ScaledInt;
        short int Z_s, FractX_s;
        unsigned short int  ScaledFract;

        if (!X) return 0x8000;
        Y = gap_abs((int)X);
        IntX = (Y >> 15);
        if (IntX >= (int) ARRAYSIZE (IntegerExpLUT)) {
                if (Y==X) return 0x7FFFFFFF; else return 0;
        }
        FractX = (Y & 0x7FFF);
        if (gap_bitextractu(FractX, 1, 14)) {
                /* Taylor series converges quickly only when | FractX | < 0.5 */
                FractX -= 0x8000; IntX++;
        }
        ScaledInt = IntegerExpLUT[IntX]; ScaledFract = FractionExpLUT[IntX];
        /* Taylor's series: exp(x) = 1 + x + x ^ 2 / 2 + x ^ 3 / 3! + x ^ 4 / 4! + x ^ 5 / 5! + x ^ 6 / 6! + x ^ 7 / 7! + x ^ 8 / 8!  */
        FractX_s = FractX; Z_s = FractX; Result = 0;
        for (int i = 1; i < ARRAYSIZE (ExpCoeffLUT); i++) {
                Result += Z_s*ExpCoeffLUT[i]; // gap_macs(Result, Z, ExpCoeffLUT[ i ]);
                Z_s = gap_mulsRN(Z_s, FractX_s, 15);
        }
        Result = gap_roundnorm(Result, 15) + ExpCoeffLUT[0];
        unsigned short int U_Res = Result;
        Result = gap_muluRN(U_Res, ScaledFract, 15) + U_Res * ScaledInt;
        if (Result && (X > 0x7FFFFFFF))
                Result = ((0x7FFFFFFF / Result) >> 1);      /* negative value */
        return (unsigned int) Result;
}

uint32 pow_17_15(uint32 x, uint32 y)
{
	if (x==0) return (unsigned int)(y==0 ? 1<<15 : 0);
	int expo = gap_clipr((int)logn_17_15(x), (1 << gap_clb(y)) - 1);
    return exp_17_15((unsigned int)gap_roundnorm_reg((int)y * expo, 15));
}

int square_17_15(int x)
{
	return gap_roundnorm_reg(x * x, 15);
}

#define PI_Q17_15_DIV4 		25736		// int(math.floor(0.5 + PI_Q17_15 / 4))
#define PI_Q1_30_DIV4	 	843314857	// int(math.floor(0.5 + math.pi * math.pow(2, 30) / 4))
#define ARCTAN_FAC_Q17_15 	8946		// int(math.floor(0.5 + 0.273 * math.pow(2, 15)))
#define ONE_Q17_15 			32768		// 1 << 15

int arctan_17_15(int x)
{
    // Valid for 1 > x > -1
    // This can use p.mulsRN and p.adduRN on GAP so 4 cycles
    // p.mulsRN(x, p.adduRN(PI_Q1_30_DIV4, ARCTAN_FAC_Q17_15 * (ONE_Q17_15 - x)))
    return gap_roundnorm_reg(x * gap_addroundnorm_reg(PI_Q1_30_DIV4, ARCTAN_FAC_Q17_15 * (ONE_Q17_15 - x), 15), 15);
}

int16 fpsin(int16 i)
{
    /* Convert (signed) input to a value between 0 and 8192. (8192 is pi/2, which is the region of the curve fit). */
    /* ------------------------------------------------------------------- */
    i <<= 1;
    int c = i<0; //set carry for output pos/neg

    if(i == (i|0x4000)) // flip input value to corresponding value in range [0..8192)
        i = (1<<15) - i;
    i = (i & 0x7FFF) >> 1;
    /* ------------------------------------------------------------------- */

    /* The following section implements the formula:
     = y * 2^-n * ( A1 - 2^(q-p)* y * 2^-n * y * 2^-n * [B1 - 2^-r * y * 2^-n * C1 * y]) * 2^(a-q)
    Where the constants are defined as follows:
    */
    enum {A1=3370945099UL, B1=2746362156UL, C1=292421UL};
    enum {n=13, p=32, q=31, r=3, a=12};

    uint32 y = (C1*((uint32)i))>>n;
    y = B1 - (((uint32)i*y)>>r);
    y = (uint32)i * (y>>n);
    y = (uint32)i * (y>>n);
    y = A1 - (y>>(p-q));
    y = (uint32)i * (y>>n);
    y = gap_roundnormu(y, (q-a)); // Rounding

    return (c ? -((int16)y) : ((int16)y));
}

#pragma GCC diagnostic pop
