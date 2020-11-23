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

unsigned short int IntegerExpTable[] = // N
{
	0x00000001, 0x00000002, 0x00000007, 0x00000014,
	0x00000036, 0x00000094, 0x00000193, 0x00000448,
	0x00000BA4, 0x00001FA7, 0x0000560A, 0x0000E9E2

};

unsigned short int FractionExpTable[] = // N
{
	0x00000000, 0x00005BF1, 0x000031CD, 0x00000AF3,
	0x00004C90, 0x000034E2, 0x000036E3, 0x0000510B,
	0x00007A9F, 0x00000ABE, 0x00003B9F, 0x00001224

};

/* exponent coefficients in 17.15 fixed point format */

unsigned short int ExpCoeffTable[] = // N
{
	// 0x00008000, 0x00008000, 0x00004000, 0x00001555,
	0x00007FFF, 0x00007FFF, 0x00004000, 0x00001555,
	0x00000555, 0x00000111, 0x0000002E, 0x00000007,
	0x00000001
};

uint32 sqrt_17_15(uint32 x)

{
	unsigned int   exponent;
	int            result, y, z;

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
	if (exponent != 16) result = ((result * Sqrt2Powers[ 31 - exponent ]) >> 15);

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

uint32 exp_17_15(uint32 x)
{
	register unsigned int   i;
	register int            z, y, result, intx, fractx, scaleint;
	short int Z, Fractx;
	unsigned short int      scalefract;

	if (!x) return 0x8000;
	y = gap_abs(x);

	intx = (y >> 15);
	fractx = (y & 0x7FFF);
	if (gap_bitextractu(fractx, 1, 14)) {
		/* Taylor series converges quickly only when | fractx | < 0.5 */
		fractx -= 0x8000;
		++intx;
	}
	if (intx >= (int) ARRAYSIZE (IntegerExpTable)) {
		if (y==x) return 0x7FFFFFFF; else return 0;
	}

	scaleint = IntegerExpTable[ intx ];
	scalefract = FractionExpTable[ intx ];

	/** Taylor's series: * exp(x) = 1 + x + x ^ 2 / 2 + x ^ 3 / 3! + x ^ 4 / 4! + x ^ 5 / 5! + x ^ 6 / 6! + x ^ 7 / 7! + x ^ 8 / 8! */
	Fractx = fractx;
	Z = fractx;
	result = 0;
	for (i = 1; i < ARRAYSIZE (ExpCoeffTable); i++) {
		result = gap_macs(result, Z, ExpCoeffTable[ i ]);
		Z = gap_mulshRN(Z, Fractx, 15);
	}
	result = gap_roundnorm(result, 15) + ExpCoeffTable[0];

	unsigned short int uz = result;
	result = gap_muluRN(uz, scalefract, 15) + uz * scaleint;

	if (x > 0x7FFFFFFF) result = ((0x7FFFFFFF / result) >> 1);	/* negative value */
	return (uint32) result;
}

uint32 pow_17_15(uint32 x, uint32 y)
{
    return exp_17_15(gap_roundnorm(y * logn_17_15(x), 15));
}

#pragma GCC diagnostic pop
