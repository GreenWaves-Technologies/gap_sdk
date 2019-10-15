#ifndef _FIXEDPTC_H_
#define _FIXEDPTC_H_

#include <stdlib.h>     /* strtof */
#include "gap_common.h"
#include "gap_cluster.h"
#include "gap_dmamchan.h"


/*
 * fixedptc.h is a 32-bit or 64-bit fixed point numeric library.
 *
 * The symbol FIXEDPT_BITS, if defined before this library header file
 * is included, determines the number of bits in the data type (its "width").
 * The default width is 32-bit (FIXEDPT_BITS=32) and it can be used
 * on any recent C99 compiler. The 64-bit precision (FIXEDPT_BITS=64) is
 * available on compilers which implement 128-bit "long long" types. This
 * precision has been tested on GCC 4.2+.
 *
 * The FIXEDPT_WBITS symbols governs how many bits are dedicated to the
 * "whole" part of the number (to the left of the decimal point). The larger
 * this width is, the larger the numbers which can be stored in the fixedpt
 * number. The rest of the bits (available in the FIXEDPT_FBITS symbol) are
 * dedicated to the fraction part of the number (to the right of the decimal
 * point).
 *
 * Since the number of bits in both cases is relatively low, many complex
 * functions (more complex than div & mul) take a large hit on the precision
 * of the end result because errors in precision accumulate.
 * This loss of precision can be lessened by increasing the number of
 * bits dedicated to the fraction part, but at the loss of range.
 *
 * Adventurous users might utilize this library to build two data types:
 * one which has the range, and one which has the precision, and carefully
 * convert between them (including adding two number of each type to produce
 * a simulated type with a larger range and precision).
 *
 * The ideas and algorithms have been cherry-picked from a large number
 * of previous implementations available on the Internet.
 * Tim Hartrick has contributed cleanup and 64-bit support patches.
 *
 * == Special notes for the 32-bit precision ==
 * Signed 32-bit fixed point numeric library for the 24.8 format.
 * The specific limits are -8388608.999... to 8388607.999... and the
 * most precise number is 0.00390625. In practice, you should not count
 * on working with numbers larger than a million or to the precision
 * of more than 2 decimal places. Make peace with the fact that PI
 * is 3.14 here. :)
 */

/*-
 * Copyright (c) 2010-2012 Ivan Voras <ivoras@freebsd.org>
 * Copyright (c) 2012 Tim Hartrick <tim@edgecast.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/* Description:
 * This library has been extended to offer 3 types of Fixed Point representation.
 * 1. Standard Fixedpt
 * 2. High Range (HR) Fixedpt
 * 3. High Precision (HP) Fixedpt
 * */


#ifndef FIXEDPT_BITS
#define FIXEDPT_BITS	32
#endif

#if FIXEDPT_BITS == 16
typedef short fixedpt;
typedef	int fixedptd;
typedef	unsigned short fixedptu;
typedef	unsigned int fixedptud;
#elif FIXEDPT_BITS == 32
typedef int fixedpt;
typedef	long long fixedptd;
typedef	unsigned int fixedptu;
typedef	unsigned long long fixedptud;
#else
#error "FIXEDPT_BITS must be equal to 16 or 32"
#endif

#if FIXEDPT_BITS == 16
#define FIXEDPT_WBITS		10 	/* Default */
#define FIXEDPT_WBITS_HP	3	/* High Precision */
#define FIXEDPT_WBITS_HR	13	/* High Range */
#elif FIXEDPT_BITS == 32
#define FIXEDPT_WBITS		20 	/* Default */
#define FIXEDPT_WBITS_HP	15	/* High Precision */
#define FIXEDPT_WBITS_HR	29	/* High Range */
#endif


#if FIXEDPT_WBITS > FIXEDPT_BITS
#error "FIXEDPT_WBITS must be less than or equal to FIXEDPT_BITS"
#endif

#define FIXEDPT_VCSID "$Id$"

#define FIXEDPT_FBITS			(FIXEDPT_BITS - FIXEDPT_WBITS)
#define FIXEDPT_FBITS_HP		(FIXEDPT_BITS - FIXEDPT_WBITS_HP)
#define FIXEDPT_FBITS_HR		(FIXEDPT_BITS - FIXEDPT_WBITS_HR)
#define FIXEDPT_FMASK			(((fixedpt)1 << FIXEDPT_FBITS) - 1)
#define FIXEDPT_FMASK_HR		(((fixedpt)1 << FIXEDPT_FBITS_HR) - 1)
#define FIXEDPT_FMASK_HP		(((fixedpt)1 << FIXEDPT_FBITS_HP) - 1)

#define fixedpt_rconst(R) 		((fixedpt)((R) * FIXEDPT_ONE + ((R) >= 0 ? 0.5 : -0.5)))
#define fixedpt_rconst_HR(R) 	((fixedpt)((R) * FIXEDPT_ONE_HR + ((R) >= 0 ? 0.5 : -0.5)))
#define fixedpt_rconst_HP(R) 	((fixedpt)((R) * FIXEDPT_ONE_HP + ((R) >= 0 ? 0.5 : -0.5)))
#define fixedpt_fromint(I) 		((fixedptd)(I) << FIXEDPT_FBITS)
#define fixedpt_frominttoHR(I) 	((fixedptd)(I) << FIXEDPT_FBITS_HR)
#define fixedpt_frominttoHP(I) 	((fixedptd)(I) << FIXEDPT_FBITS_HP)
#define fixedpt_fromchar(C) 	((fixedptd)(C & 0x000000FF) << FIXEDPT_FBITS)
#define fixedpt_toint(F) 		((F) >> FIXEDPT_FBITS)
#define fixedpt_toint_HR(F) 	((F) >> FIXEDPT_FBITS_HR)
#define fixedpt_toint_HP(F) 	((F) >> FIXEDPT_FBITS_HP)
#define fixedpt_add(A,B) 		((A) + (B))
#define fixedpt_sub(A,B) 		((A) - (B))
#define fixedpt_xmul(A,B) 		((fixedpt)(((fixedptd)(A) * (fixedptd)(B)) >> FIXEDPT_FBITS))
#define fixedpt_xdiv(A,B)		((fixedpt)(((fixedptd)(A) << FIXEDPT_FBITS) / (fixedptd)(B)))
#define fixedpt_fracpart(A) 	((fixedpt)(A) & FIXEDPT_FMASK)
#define fixedpt_fracpart_HR(A) 	((fixedpt)(A) & FIXEDPT_FMASK_HR)
#define fixedpt_fracpart_HP(A) 	((fixedpt)(A) & FIXEDPT_FMASK_HP)
#define fixedpt_abs(A) 			((A) < 0 ? -(A) : (A))
#define MAX(x,y) 				(((x)>(y))?(x):(y))
#define MIN(x,y) 				(((x)<(y))?(x):(y))

#define FIXEDPT_ONE				((fixedpt)((fixedpt)1 << FIXEDPT_FBITS))
#define FIXEDPT_NEG_ONE			((fixedpt)((fixedpt)-1 << FIXEDPT_FBITS))
#define FIXEDPT_ONE_HP			((fixedpt)((fixedpt)1 << FIXEDPT_FBITS_HP))
#define FIXEDPT_ONE_HR			((fixedpt)((fixedpt)1 << FIXEDPT_FBITS_HR))
#define FIXEDPT_ONE_HALF 		(FIXEDPT_ONE >> 1)
#define FIXEDPT_TWO				(FIXEDPT_ONE + FIXEDPT_ONE)
#define FIXEDPT_PI				fixedpt_rconst(3.14159265358979323846)
#define FIXEDPT_TWO_PI			fixedpt_rconst(2 * 3.14159265358979323846)
#define FIXEDPT_HALF_PI			fixedpt_rconst(3.14159265358979323846 / 2)
#define FIXEDPT_E				fixedpt_rconst(2.7182818284590452354)


static inline fixedpt fixedpt_to_HP(fixedpt A) {

	return ((fixedptd)A << (FIXEDPT_WBITS - FIXEDPT_WBITS_HP));
}


static inline fixedpt fixedpt_to_HR(fixedpt A) {

	return ((fixedptd)A >> (FIXEDPT_WBITS_HR - FIXEDPT_WBITS));
}


static inline fixedpt
fixedpt_from_HR(fixedpt A)
{
	return ((fixedptd)A << (FIXEDPT_WBITS_HR - FIXEDPT_WBITS));
}


/* Multiplies two fixedpt numbers, returns the result. */
static inline fixedpt
fixedpt_mul(fixedpt A, fixedpt B)
{
//	fixedpt tmp_a = (fixedpt)A >> 6 ;
//	fixedpt tmp_b = (fixedpt)B >> 6 ;
//	return (((fixedpt)tmp_a * (fixedpt)tmp_b)) ;

return (((fixedptd)A * (fixedptd)B) >> FIXEDPT_FBITS);
}

static inline fixedpt
fixedpt_mul_HR(fixedpt A, fixedpt B)
{
	return (((fixedptd)A * (fixedptd)B) >> FIXEDPT_FBITS_HR);
}

static inline fixedpt
fixedpt_mul_HP(fixedpt A, fixedpt B)
{
	return (((fixedptd)A * (fixedptd)B) >> FIXEDPT_FBITS_HP);
}

/* Divides two fixedpt numbers, returns the result. */
static inline fixedpt
fixedpt_div(fixedpt A, fixedpt B)
{
//	return (((fixedpt)A << 4) / (fixedpt)B) << 8;

	return (((fixedptd)A << FIXEDPT_FBITS) / (fixedpt)B);
}

static inline fixedpt
fixedpt_div_HR(fixedpt A, fixedpt B)
{
//	return ((fixedpt)A / (fixedpt)B) << 3;

	return (((fixedptd)A << FIXEDPT_FBITS_HR) / (fixedpt)B);
}

static inline fixedpt
fixedpt_div_HP(fixedpt A, fixedpt B)
{
//	return (((fixedpt)A << 9) / (fixedpt)B) << 8;

	return (((fixedptd)A << FIXEDPT_FBITS_HP) / (fixedpt)B);
}

/*
 * Note: adding and substracting fixedpt numbers can be done by using
 * the regular integer operators + and -.
 */

/*  */
static inline fixedpt
fixedpt_mod(fixedpt A, fixedpt B)
{
	fixedpt a = fixedpt_div(A, B);
	fixedpt b = fixedpt_fracpart(a);
	return fixedpt_mul(b, B);
}

/*  */
static inline fixedpt
fixedpt_round(fixedpt A)
{
	fixedpt B = (A + ((A) >= 0 ? fixedpt_rconst(0.5) : fixedpt_rconst(-0.5)));
	return ((B >> FIXEDPT_FBITS) << FIXEDPT_FBITS);
}

static inline fixedpt
fixedpt_ceil(fixedpt A)
{
	if(fixedpt_fracpart(A) > 0)	A += FIXEDPT_ONE_HALF ;
	return fixedpt_round(A);

	/* Alternative */
//	fixedpt B = ((A << FIXEDPT_WBITS) >> FIXEDPT_WBITS);
//	if(B > 0) return ((A >> FIXEDPT_FBITS) << FIXEDPT_FBITS)+FIXEDPT_ONE;
//	else	return A;
}

/*  */
static inline fixedpt
fixedpt_floor(fixedpt A)
{
	return ((A >> FIXEDPT_FBITS) << FIXEDPT_FBITS);
}

static inline fixedpt
fixedpt_floor_HP(fixedpt A)
{
	return ((A >> FIXEDPT_FBITS_HP) << FIXEDPT_FBITS_HP);
}

static inline fixedpt
fixedpt_floor_HR(fixedpt A)
{
	return ((A >> FIXEDPT_FBITS_HR) << FIXEDPT_FBITS_HR);
}

/**
 * Convert the given fixedpt number to a decimal string.
 * The max_dec argument specifies how many decimal digits to the right
 * of the decimal point to generate. If set to -1, the "default" number
 * of decimal digits will be used (2 for 32-bit fixedpt width, 10 for
 * 64-bit fixedpt width); If set to -2, "all" of the digits will
 * be returned, meaning there will be invalid, bogus digits outside the
 * specified precisions.
 */
static inline void
fixedpt_str(fixedpt A, char *str, int max_dec)
{
	int ndec = 0, slen = 0;
	char tmp[12] = {0};
	fixedptud fr, ip;
	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
	const fixedptud mask = one - 1;

	if (max_dec == -1)
#if FIXEDPT_BITS <= 32
#if FIXEDPT_WBITS > 16
		max_dec = 2;
#else
		max_dec = 4;
#endif
#elif FIXEDPT_BITS == 64
		max_dec = 10;
#else
#error Invalid width
#endif
	else if (max_dec == -2)
		max_dec = 15;

	if (A < 0) {
		str[slen++] = '-';
		A *= -1;
	}

	ip = fixedpt_toint(A);
	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

	fr = (fixedpt_fracpart(A) << FIXEDPT_WBITS) & mask;
	do {
		fr = (fr & mask) * 10;

		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
		ndec++;
	} while (fr != 0 && ndec < max_dec);

	if (ndec > 1 && str[slen-1] == '0')
		str[slen-1] = '\0'; /* cut off trailing 0 */
	else
		str[slen] = '\0';
}


static inline void
fixedpt_str_HP(fixedpt A, char *str, int max_dec)
{
	int ndec = 0, slen = 0;
	char tmp[12] = {0};
	fixedptud fr, ip;
	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
	const fixedptud mask = one - 1;

	if (max_dec == -1)
#if FIXEDPT_BITS <= 32
#if FIXEDPT_WBITS_HP > 16
		max_dec = 2;
#else
		max_dec = 4;
#endif
#elif FIXEDPT_BITS == 64
		max_dec = 10;
#else
#error Invalid width
#endif
	else if (max_dec == -2)
		max_dec = 15;

	if (A < 0) {
		str[slen++] = '-';
		A *= -1;
	}

	ip = fixedpt_toint_HP(A);
	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

	fr = (fixedpt_fracpart_HP(A) << FIXEDPT_WBITS_HP) & mask;
	do {
		fr = (fr & mask) * 10;

		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
		ndec++;
	} while (fr != 0 && ndec < max_dec);

	if (ndec > 1 && str[slen-1] == '0')
		str[slen-1] = '\0'; /* cut off trailing 0 */
	else
		str[slen] = '\0';
}


static inline void
fixedpt_str_HR(fixedpt A, char *str, int max_dec)
{
	int ndec = 0, slen = 0;
	char tmp[12] = {0};
	fixedptud fr, ip;
	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
	const fixedptud mask = one - 1;

	if (max_dec == -1)
#if FIXEDPT_BITS <= 32
#if FIXEDPT_WBITS_HR > 16
		max_dec = 2;
#else
		max_dec = 4;
#endif
#elif FIXEDPT_BITS == 64
		max_dec = 10;
#else
#error Invalid width
#endif
	else if (max_dec == -2)
		max_dec = 15;

	if (A < 0) {
		str[slen++] = '-';
		A *= -1;
	}

	ip = fixedpt_toint_HR(A);
	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

	fr = (fixedpt_fracpart_HR(A) << FIXEDPT_WBITS_HR) & mask;
	do {
		fr = (fr & mask) * 10;

		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
		ndec++;
	} while (fr != 0 && ndec < max_dec);

	if (ndec > 1 && str[slen-1] == '0')
		str[slen-1] = '\0'; /* cut off trailing 0 */
	else
		str[slen] = '\0';
}

static inline void
fixedpt_str_special32(fixedpt A, char *str, int max_dec)
{
	int ndec = 0, slen = 0;
	char tmp[12] = {0};
	fixedptud fr, ip;
//	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
	const fixedptud one = (fixedptud)1 << 32;
	const fixedptud mask = one - 1;

	if (max_dec == -1)
		max_dec = 2;
	else
		if (max_dec == -2)
			max_dec = 15;

	if (A < 0) {
		str[slen++] = '-';
		A *= -1;
	}

//	ip = fixedpt_toint(A);
	ip = ((A) >> 12);

	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

//	fr = (fixedpt_fracpart(A) << FIXEDPT_WBITS_HP) & mask;
	fr = (((fixedpt)(A) & (((fixedpt)1 << 12) - 1)) << 20) & mask;


	do {
		fr = (fr & mask) * 10;

		str[slen++] = '0' + (fr >> 32) % 10;
		ndec++;
	} while (fr != 0 && ndec < max_dec);

	if (ndec > 1 && str[slen-1] == '0')
		str[slen-1] = '\0'; /* cut off trailing 0 */
	else
		str[slen] = '\0';
}

/* Converts the given fixedpt number into a string, using a static
 * (non-threadsafe) string buffer */
static inline char*
fixedpt_cstr(const fixedpt A, const int max_dec)
{
	static char str[25];

	fixedpt_str(A, str, max_dec);
	return (str);
}

/* Returns the square root of the given number, or -1 in case of error */
static inline fixedpt
fixedpt_sqrt(fixedpt A)
{
	int invert = 0;
	int iter = FIXEDPT_FBITS;
	int l, i;

	if (A < 0)
		return (-1);
	if (A == 0 || A == FIXEDPT_ONE)
		return (A);
	if (A < FIXEDPT_ONE && A > 6) {
		invert = 1;
		A = fixedpt_div(FIXEDPT_ONE, A);
	}
	if (A > FIXEDPT_ONE) {
		int s = A;

		iter = 0;
		while (s > 0) {
			s >>= 2;
			iter++;
		}
	}

	/* Newton's iterations */
	l = (A >> 1) + 1;
	for (i = 0; i < iter; i++)
		l = (l + fixedpt_div(A, l)) >> 1;
	if (invert)
		return (fixedpt_div(FIXEDPT_ONE, l));
	return (l);
}

/* Returns the square root of the given number, or -1 in case of error */
static inline fixedpt
fixedpt_sqrt_HR(fixedpt A)
{
	int invert = 0;
	int iter = FIXEDPT_FBITS_HR;
	int l, i;

	if (A < 0)
		return (-1);
	if (A == 0 || A == FIXEDPT_ONE_HR)
		return (A);
	if (A < FIXEDPT_ONE_HR && A > 6) {
		invert = 1;
		A = fixedpt_div_HR(FIXEDPT_ONE_HR, A);
	}
	if (A > FIXEDPT_ONE_HR) {
		int s = A;

		iter = 0;
		while (s > 0) {
			s >>= 2;
			iter++;
		}
	}

	/* Newton's iterations */
	l = (A >> 1) + 1;
	for (i = 0; i < iter; i++)
		l = (l + fixedpt_div_HR(A, l)) >> 1;
	if (invert)
		return (fixedpt_div_HR(FIXEDPT_ONE_HR, l));
	return (l);
}

static const fixedpt coef_fixedpt_sqrt_x0[] = {
   1,
   1,
   1,
   2,
   3,
   5,
   7,
   10,
   14,
   19,
   27,
   37,
   52,
   73,
   101,
   141,
   196,
   273,
   380,
   529,
   736,
   1024,
   1424,
   1981,
   2756,
   3834,
   5333,
   7419,
   10321,
   14357,
   19972,
   27782,
   38647,
   53761,
   74785,
   104031,
   144715,
   201308,
   280033,
   389544,
   541881,
   753793,
   1048576,
   1458638,
   2029061,
   2822557,
   3926362,
   5461827,
   7597760,
   10568983,
   14702149,
   20451655,
   28449594,
   39575253,
   55051773,
   76580629,
   106528681,
   148188386,
   206139768,
   286753945,
   398893554,
   554887109,
   771884380,
   1073741824
};

static const fixedpt coef_fixedpt_sqrt_y0[] = {
   64,
   75,
   89,
   105,
   123,
   146,
   172,
   203,
   239,
   282,
   333,
   393,
   463,
   546,
   645,
   760,
   897,
   1058,
   1248,
   1472,
   1736,
   2048,
   2415,
   2848,
   3360,
   3963,
   4674,
   5512,
   6501,
   7668,
   9044,
   10667,
   12581,
   14839,
   17502,
   20642,
   24346,
   28715,
   33867,
   39944,
   47112,
   55565,
   65536,
   77295,
   91164,
   107522,
   126816,
   149571,
   176409,
   208063,
   245397,
   289430,
   341364,
   402616,
   474860,
   560066,
   660561,
   779088,
   918884,
   1083763,
   1278228,
   1507586,
   1778099,
   2097152
};

static const fixedpt coef_fixedpt_sqrt_m[] = {
   120280,
   101981,
   86466,
   73311,
   62158,
   52701,
   44684,
   37886,
   32122,
   27235,
   23091,
   19578,
   16600,
   14074,
   11933,
   10117,
   8578,
   7273,
   6166,
   5228,
   4433,
   3758,
   3186,
   2702,
   2290,
   1942,
   1646,
   1396,
   1183,
   1003,
   851,
   721,
   611,
   518,
   439,
   372,
   316,
   268,
   227,
   192,
   163,
   138,
   117,
   99,
   84,
   71,
   60,
   51,
   43,
   36,
   31,
   26,
   22,
   19,
   16,
   13,
   11,
   9,
   8,
   7,
   6,
   5,
   4
};

static void fixedpt_sqrt_init(fixedpt **coef_x0, fixedpt **coef_y0, fixedpt **coef_m) {
   *coef_x0 = (fixedpt *) L1_Malloc(sizeof(fixedpt)*64);
   *coef_y0 = (fixedpt *) L1_Malloc(sizeof(fixedpt)*64);
   *coef_m  = (fixedpt *) L1_Malloc(sizeof(fixedpt)*64);

   dma_req_t req;

   DMAMCHAN_Memcpy_1D((uint32_t)coef_fixedpt_sqrt_x0, (uint32_t)*coef_x0, sizeof(fixedpt)*64, GAP_DMA_L22TCDM, &req);
   DMAMCHAN_Memcpy_1D((uint32_t)coef_fixedpt_sqrt_y0, (uint32_t)*coef_y0, sizeof(fixedpt)*64, GAP_DMA_L22TCDM, &req);
   DMAMCHAN_Memcpy_1D((uint32_t)coef_fixedpt_sqrt_m, (uint32_t)*coef_m, sizeof(fixedpt)*64, GAP_DMA_L22TCDM, &req);

   DMAMCHAN_WaitRequestEnd(&req);
}

static void fixedpt_sqrt_deinit(fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   L1_Free(coef_x0);
   L1_Free(coef_y0);
   L1_Free(coef_m);
}

static inline fixedpt fixedpt_sqrt_approx(fixedpt x, fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
   int i = 0;
   if(x < 1)
      return 64;
   if(x > 1073741824)
      return 2097152;
   while (x > coef_x0[i]) i++;
   i--;

   return coef_y0[i] + fixedpt_mul(coef_m[i], (x - coef_x0[i]));
}

/* Returns the sine of the given fixedpt number.
 * Note: the loss of precision is extraordinary! */
static inline fixedpt
fixedpt_sin(fixedpt fp)
{
	int sign = 1;
	fixedpt sqr, result;
	const fixedpt SK[2] = {
		fixedpt_rconst(7.61e-03),
		fixedpt_rconst(1.6605e-01)
	};

	fp %= 2 * FIXEDPT_PI;
	if (fp < 0)
		fp = FIXEDPT_PI * 2 + fp;
	if ((fp > FIXEDPT_HALF_PI) && (fp <= FIXEDPT_PI))
		fp = FIXEDPT_PI - fp;
	else if ((fp > FIXEDPT_PI) && (fp <= (FIXEDPT_PI + FIXEDPT_HALF_PI))) {
		fp = fp - FIXEDPT_PI;
		sign = -1;
	} else if (fp > (FIXEDPT_PI + FIXEDPT_HALF_PI)) {
		fp = (FIXEDPT_PI << 1) - fp;
		sign = -1;
	}
	sqr = fixedpt_mul(fp, fp);
	result = SK[0];
	result = fixedpt_mul(result, sqr);
	result -= SK[1];
	result = fixedpt_mul(result, sqr);
	result += FIXEDPT_ONE;
	result = fixedpt_mul(result, fp);
	return sign * result;
}


/* Returns the cosine of the given fixedpt number */
static inline fixedpt fixedpt_cos(fixedpt A)
{
	return (fixedpt_sin(FIXEDPT_HALF_PI - A));
}


/* Returns the tangens of the given fixedpt number */
static inline fixedpt fixedpt_tan(fixedpt A)
{
	return fixedpt_div(fixedpt_sin(A), fixedpt_cos(A));
}


/* Returns the value exp(x), i.e. e^x of the given fixedpt number. */
static inline fixedpt fixedpt_exp(fixedpt fp)
{
	fixedpt xabs, k, z, R, xp;
	const fixedpt LN2 = fixedpt_rconst(0.69314718055994530942);
	const fixedpt LN2_INV = fixedpt_rconst(1.4426950408889634074);
	const fixedpt EXP_P[5] = {
		fixedpt_rconst(1.66666666666666019037e-01),
		fixedpt_rconst(-2.77777777770155933842e-03),
		fixedpt_rconst(6.61375632143793436117e-05),
		fixedpt_rconst(-1.65339022054652515390e-06),
		fixedpt_rconst(4.13813679705723846039e-08),
	};

	if (fp == 0)
		return (FIXEDPT_ONE);
	xabs = fixedpt_abs(fp);
	k = fixedpt_mul(xabs, LN2_INV);
	k += FIXEDPT_ONE_HALF;
	k &= ~FIXEDPT_FMASK;
	if (fp < 0)
		k = -k;
	fp -= fixedpt_mul(k, LN2);
	z = fixedpt_mul(fp, fp);
	/* Taylor */
	R = FIXEDPT_TWO +
	    fixedpt_mul(z, EXP_P[0] + fixedpt_mul(z, EXP_P[1] +
	    fixedpt_mul(z, EXP_P[2] + fixedpt_mul(z, EXP_P[3] +
	    fixedpt_mul(z, EXP_P[4])))));
//	R = FIXEDPT_TWO +
//		fixedpt_mul(z, EXP_P[0] + fixedpt_mul(z, EXP_P[1] +
//		fixedpt_mul(z, EXP_P[2] )));
	/* Alternative Taylor implementation */
//	fixedptd sum = 0 ;
//	int i ;
//	for(i=0; i<5; i++)
//	{
//		sum = z * sum ;
//		sum += (fixedptd)z * (fixedptd)EXP_P[4-i] ;
//	}
//	sum = sum >> FIXEDPT_FBITS ;
//	R = FIXEDPT_TWO + sum ;

	xp = FIXEDPT_ONE + fixedpt_div(fixedpt_mul(fp, FIXEDPT_TWO), R - fp);
	if (k < 0)
		k = FIXEDPT_ONE >> (-k >> FIXEDPT_FBITS);
	else
		k = FIXEDPT_ONE << (k >> FIXEDPT_FBITS);
	return (fixedpt_mul(k, xp));
}

//static const fixedpt coef_fixedpt_exp_x0[] = {
//   -2048,
//   -1832,
//   -1616,
//   -1401,
//   -1185,
//   -970,
//   -754,
//   -538,
//   -323,
//   -107,
//   107,
//   323,
//   538,
//   754,
//   970,
//   1185,
//   1401,
//   1616,
//   1832,
//   2048
//};
//
//static const fixedpt coef_fixedpt_exp_y0[] = {
//   138,
//   171,
//   211,
//   260,
//   321,
//   397,
//   490,
//   604,
//   746,
//   921,
//   1137,
//   1404,
//   1733,
//   2139,
//   2640,
//   3259,
//   4023,
//   4966,
//   6129,
//   7566
//};
//
//static const fixedpt coef_fixedpt_exp_m[] = {
//   154,
//   190,
//   235,
//   290,
//   358,
//   441,
//   545,
//   673,
//   831,
//   1025,
//   1266,
//   1563,
//   1929,
//   2381,
//   2939,
//   3628,
//   4478,
//   5527,
//   6823
//};


//static void fixedpt_exp_init(fixedpt **coef_x0, fixedpt **coef_y0, fixedpt **coef_m) {
//   *coef_x0 = (fixedpt *) malloc(sizeof(fixedpt)*20);
//   *coef_y0 = (fixedpt *) malloc(sizeof(fixedpt)*20);
//   *coef_m  = (fixedpt *) malloc(sizeof(fixedpt)*20);
//   mchan_memcpy_async(*coef_x0, coef_fixedpt_exp_x0, sizeof(fixedpt)*20);
//   mchan_memcpy_async(*coef_y0, coef_fixedpt_exp_y0, sizeof(fixedpt)*20);
//   mchan_memcpy_async(*coef_m,  coef_fixedpt_exp_m,  sizeof(fixedpt)*20);
//   mchan_memcpy_wait();
//
//}
//
//static void fixedpt_exp_deinit(fixedpt *coef_x0, fixedpt *coef_y0, fixedpt *coef_m) {
//   free(coef_x0);
//   free(coef_y0);
//   free(coef_m);
//}

static inline fixedpt fixedpt_exp_approx(fixedpt x) {
	const fixedpt one		= FIXEDPT_ONE ;
	const fixedpt two		= FIXEDPT_ONE_HALF ;
	const fixedpt three   	= fixedpt_rconst(0.33333333) ;

	fixedpt x_2				= (x * x) >> FIXEDPT_FBITS ;
	fixedpt x_3				= (x_2 * x) >> FIXEDPT_FBITS ;

	return one + x + ((x_2 * two) >> FIXEDPT_FBITS) + ((x_3 * three) >> FIXEDPT_FBITS) ;
}

static inline fixedpt fixedpt_exp_linearized(fixedpt x) {
	fixedpt coef_x0[] = {
	   -2048,
	   -1616,
	   -1185,
	   -754,
	   -323,
	   107,
	   538,
	   970,
	   1401,
	   1832,
	   2048
	};

	fixedpt coef_y0[] = {
	   138,
	   211,
	   321,
	   490,
	   746,
	   1137,
	   1733,
	   2640,
	   4023,
	   6129
	};

	fixedpt coef_m[] = {
	   154,
	   235,
	   358,
	   545,
	   831,
	   1266,
	   1929,
	   2939,
	   4478,
	   6823
	};

	int i = 0;
	if(x < -2048)
		return 138;
	if(x > 2048)
		return 7566;
	while (x > coef_x0[i+1]) i++;
	i-- ;

   return coef_y0[i-1] + fixedpt_mul(coef_m[i-1], (x - coef_x0[i-1]));

}

/* Returns the natural logarithm of the given fixedpt number. */
static inline fixedpt
fixedpt_ln(fixedpt x)
{
	fixedpt log2, xi;
	fixedpt f, s, z, w, R;
	const fixedpt LN2 = fixedpt_rconst(0.69314718055994530942);
	const fixedpt LG[7] = {
		fixedpt_rconst(6.666666666666735130e-01),
		fixedpt_rconst(3.999999999940941908e-01),
		fixedpt_rconst(2.857142874366239149e-01),
		fixedpt_rconst(2.222219843214978396e-01),
		fixedpt_rconst(1.818357216161805012e-01),
		fixedpt_rconst(1.531383769920937332e-01),
		fixedpt_rconst(1.479819860511658591e-01)
	};

	if (x < 0)
		return (0);
	if (x == 0)
		return 0xffffffff;

	log2 = 0;
	xi = x;
	while (xi > FIXEDPT_TWO) {
		xi >>= 1;
		log2++;
	}
	f = xi - FIXEDPT_ONE;
	s = fixedpt_div(f, FIXEDPT_TWO + f);
	z = fixedpt_mul(s, s);
	w = fixedpt_mul(z, z);
	R = fixedpt_mul(w, LG[1] + fixedpt_mul(w, LG[3]
	    + fixedpt_mul(w, LG[5]))) + fixedpt_mul(z, LG[0]
	    + fixedpt_mul(w, LG[2] + fixedpt_mul(w, LG[4]
	    + fixedpt_mul(w, LG[6]))));
	return (fixedpt_mul(LN2, (log2 << FIXEDPT_FBITS)) + f
	    - fixedpt_mul(s, f - R));
}


/* Returns the logarithm of the given base of the given fixedpt number */
static inline fixedpt
fixedpt_log(fixedpt x, fixedpt base)
{
	return (fixedpt_div(fixedpt_ln(x), fixedpt_ln(base)));
}


/* Return the power value (n^exp) of the given fixedpt numbers */
static inline fixedpt
fixedpt_pow(fixedpt n, fixedpt exp)
{
	if (exp == 0)
		return (FIXEDPT_ONE);
	if (n < 0)
		return 0;
	return (fixedpt_exp(fixedpt_mul(fixedpt_ln(n), exp)));
}


static inline void fixedpt_print(fixedpt A) {
	char num[20];

	fixedpt_str(A, num, -2);
	printf("%s\n",num) ;
	//puts(num);
}

static inline void fixedpt_print_HP(fixedpt A) {
	char num[20];

	fixedpt_str_HP(A, num, -2);
	printf("%s\n",num) ;
	//puts(num);
}

static inline void fixedpt_print_HR(fixedpt A) {
	char num[20];

	fixedpt_str_HR(A, num, -2);
	printf("%s\n",num) ;
//	puts(num);
}

static inline void fixedpt_print_special32(fixedpt A) {
	char num[20];

	fixedpt_str_special32(A, num, -2);
	printf("%s\n",num) ;
	//puts(num);
}

/*
static inline void print32Bit(unsigned int *value) {

	// Float
//	if(type == 0)
//		printf("\n%d %d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\n",
//		(value[0] & 0x80000000) >> 31,
//		(value[0] & 0x40000000) >> 30,
//		(value[0] & 0x20000000) >> 29,
//		(value[0] & 0x10000000) >> 28,
//		(value[0] & 0x08000000) >> 27,
//		(value[0] & 0x04000000) >> 26,
//		(value[0] & 0x02000000) >> 25,
//		(value[0] & 0x01000000) >> 24,
//
//		(value[0] & 0x00800000) >> 23,
//		(value[0] & 0x00400000) >> 22,
//		(value[0] & 0x00200000) >> 21,
//		(value[0] & 0x00100000) >> 20,
//		(value[0] & 0x00080000) >> 19,
//		(value[0] & 0x00040000) >> 18,
//		(value[0] & 0x00020000) >> 17,
//		(value[0] & 0x00010000) >> 16,
//
//		(value[0] & 0x00008000) >> 15,
//		(value[0] & 0x00004000) >> 14,
//		(value[0] & 0x00002000) >> 13,
//		(value[0] & 0x00001000) >> 12,
//		(value[0] & 0x00000800) >> 11,
//		(value[0] & 0x00000400) >> 10,
//		(value[0] & 0x00000200) >> 9,
//		(value[0] & 0x00000100) >> 8,
//
//		(value[0] & 0x00000080) >> 7,
//		(value[0] & 0x00000040) >> 6,
//		(value[0] & 0x00000020) >> 5,
//		(value[0] & 0x00000010) >> 4,
//		(value[0] & 0x00000008) >> 3,
//		(value[0] & 0x00000004) >> 2,
//		(value[0] & 0x00000002) >> 1,
//		(value[0] & 0x00000001) >> 0);
//
//	// Iteger
//	else
		printf("%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d\n",
			(value[0] & 0x80000000) >> 31,
			(value[0] & 0x40000000) >> 30,
			(value[0] & 0x20000000) >> 29,
			(value[0] & 0x10000000) >> 28,
			(value[0] & 0x08000000) >> 27,
			(value[0] & 0x04000000) >> 26,
			(value[0] & 0x02000000) >> 25,
			(value[0] & 0x01000000) >> 24,

			(value[0] & 0x00800000) >> 23,
			(value[0] & 0x00400000) >> 22,
			(value[0] & 0x00200000) >> 21,
			(value[0] & 0x00100000) >> 20,
			(value[0] & 0x00080000) >> 19,
			(value[0] & 0x00040000) >> 18,
			(value[0] & 0x00020000) >> 17,
			(value[0] & 0x00010000) >> 16,

			(value[0] & 0x00008000) >> 15,
			(value[0] & 0x00004000) >> 14,
			(value[0] & 0x00002000) >> 13,
			(value[0] & 0x00001000) >> 12,
			(value[0] & 0x00000800) >> 11,
			(value[0] & 0x00000400) >> 10,
			(value[0] & 0x00000200) >> 9,
			(value[0] & 0x00000100) >> 8,

			(value[0] & 0x00000080) >> 7,
			(value[0] & 0x00000040) >> 6,
			(value[0] & 0x00000020) >> 5,
			(value[0] & 0x00000010) >> 4,
			(value[0] & 0x00000008) >> 3,
			(value[0] & 0x00000004) >> 2,
			(value[0] & 0x00000002) >> 1,
			(value[0] & 0x00000001) >> 0);
}
*/


#define FIXED_BITVALUE (1.0 / (1<<FIXEDPT_FBITS))
#define FIXED_BITVALUE_HP (1.0 / (1<<FIXEDPT_FBITS_HP))
#define FIXED_BITVALUE_HR (1.0 / (1<<FIXEDPT_FBITS_HR))

static inline float fixedpt_to_float(fixedpt A) {

	/* Old Expensive version */
//	char num[20];
//	fixedpt_str(A, num, -2);
//	return strtof(num, NULL);

	/* New Fast Version */
    if ((~A) < A)
        return -FIXED_BITVALUE * ((~A)+1) ;
    else
        return FIXED_BITVALUE * A ;
}

static inline float fixedpt_HP_to_float(fixedpt A) {

	/* Old Expensive version */
//	char num[20];
//	fixedpt_str_HP(A, num, -2);
//	return strtof(num, NULL);

	/* New Fast Version */
    if ((~A) < A)
        return -FIXED_BITVALUE_HP * ((~A)+1) ;
    else
        return FIXED_BITVALUE_HP * A ;

}

static inline float fixedpt_HR_to_float(fixedpt A) {

	/* Old Expensive version */
//	char num[20];
//	fixedpt_str_HR(A, num, -2);
//	return strtof(num, NULL);

	/* New Fast Version */
    if ((~A) < A)
        return -FIXED_BITVALUE_HR * ((~A)+1) ;
    else
        return FIXED_BITVALUE_HR * A ;
}

#endif
