#ifndef _FIXEDPTC_H_
#define _FIXEDPTC_H_

//#include <stdlib.h>     /* strtof */


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

//#include <stdint.h>
//#include <unistd.h>
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
#elif FIXEDPT_BITS == 64
//typedef 	int64_t fixedpt;
//typedef	__int128_t fixedptd;
//typedef	uint64_t fixedptu;
//typedef	__uint128_t fixedptud;
#else
#error "FIXEDPT_BITS must be equal to 32 or 64"
#endif

#if FIXEDPT_BITS == 16
#define FIXEDPT_WBITS		10 	/* Default */
#define FIXEDPT_WBITS_HP	8	/* High Precision */
#define FIXEDPT_WBITS_HR	14	/* High Range */
#elif FIXEDPT_BITS == 32
#define FIXEDPT_WBITS		20 	/* Default */
#define FIXEDPT_WBITS_HP	15	/* High Precision */
#define FIXEDPT_WBITS_HR	29	/* High Range */
#endif


#if FIXEDPT_WBITS >= FIXEDPT_BITS
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

/*  */
static inline fixedpt
fixedpt_floor(fixedpt A)
{
	return ((A >> FIXEDPT_FBITS) << FIXEDPT_FBITS);
}

static inline fixedpt
fixedpt_ceil(fixedpt A)
{
	if(fixedpt_fracpart(A) > 0)	A += FIXEDPT_ONE_HALF ;
	return fixedpt_round(A);
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
//	int ndec = 0, slen = 0;
//	char tmp[12] = {0};
//	fixedptud fr, ip;
//	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
//	const fixedptud mask = one - 1;
//
//	if (max_dec == -1)
//#if FIXEDPT_BITS == 32
//#if FIXEDPT_WBITS > 16
//		max_dec = 2;
//#else
//		max_dec = 4;
//#endif
//#elif FIXEDPT_BITS == 64
//		max_dec = 10;
//#else
//#error Invalid width
//#endif
//	else if (max_dec == -2)
//		max_dec = 15;
//
//	if (A < 0) {
//		str[slen++] = '-';
//		A *= -1;
//	}
//
//	ip = fixedpt_toint(A);
//	do {
//		tmp[ndec++] = '0' + ip % 10;
//		ip /= 10;
//	} while (ip != 0);
//
//	while (ndec > 0)
//		str[slen++] = tmp[--ndec];
//	str[slen++] = '.';
//
//	fr = (fixedpt_fracpart(A) << FIXEDPT_WBITS) & mask;
//	do {
//		fr = (fr & mask) * 10;
//
//		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
//		ndec++;
//	} while (fr != 0 && ndec < max_dec);
//
//	if (ndec > 1 && str[slen-1] == '0')
//		str[slen-1] = '\0'; /* cut off trailing 0 */
//	else
//		str[slen] = '\0';
}


static inline void
fixedpt_str_HP(fixedpt A, char *str, int max_dec)
{
//	int ndec = 0, slen = 0;
//	char tmp[12] = {0};
//	fixedptud fr, ip;
//	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
//	const fixedptud mask = one - 1;
//
//	if (max_dec == -1)
//#if FIXEDPT_BITS == 32
//#if FIXEDPT_WBITS_HP > 16
//		max_dec = 2;
//#else
//		max_dec = 4;
//#endif
//#elif FIXEDPT_BITS == 64
//		max_dec = 10;
//#else
//#error Invalid width
//#endif
//	else if (max_dec == -2)
//		max_dec = 15;
//
//	if (A < 0) {
//		str[slen++] = '-';
//		A *= -1;
//	}
//
//	ip = fixedpt_toint_HP(A);
//	do {
//		tmp[ndec++] = '0' + ip % 10;
//		ip /= 10;
//	} while (ip != 0);
//
//	while (ndec > 0)
//		str[slen++] = tmp[--ndec];
//	str[slen++] = '.';
//
//	fr = (fixedpt_fracpart_HP(A) << FIXEDPT_WBITS_HP) & mask;
//	do {
//		fr = (fr & mask) * 10;
//
//		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
//		ndec++;
//	} while (fr != 0 && ndec < max_dec);
//
//	if (ndec > 1 && str[slen-1] == '0')
//		str[slen-1] = '\0'; /* cut off trailing 0 */
//	else
//		str[slen] = '\0';
}


static inline void
fixedpt_str_HR(fixedpt A, char *str, int max_dec)
{
//	int ndec = 0, slen = 0;
//	char tmp[12] = {0};
//	fixedptud fr, ip;
//	const fixedptud one = (fixedptud)1 << FIXEDPT_BITS;
//	const fixedptud mask = one - 1;
//
//	if (max_dec == -1)
//#if FIXEDPT_BITS == 32
//#if FIXEDPT_WBITS_HR > 16
//		max_dec = 2;
//#else
//		max_dec = 4;
//#endif
//#elif FIXEDPT_BITS == 64
//		max_dec = 10;
//#else
//#error Invalid width
//#endif
//	else if (max_dec == -2)
//		max_dec = 15;
//
//	if (A < 0) {
//		str[slen++] = '-';
//		A *= -1;
//	}
//
//	ip = fixedpt_toint_HR(A);
//	do {
//		tmp[ndec++] = '0' + ip % 10;
//		ip /= 10;
//	} while (ip != 0);
//
//	while (ndec > 0)
//		str[slen++] = tmp[--ndec];
//	str[slen++] = '.';
//
//	fr = (fixedpt_fracpart_HR(A) << FIXEDPT_WBITS_HR) & mask;
//	do {
//		fr = (fr & mask) * 10;
//
//		str[slen++] = '0' + (fr >> FIXEDPT_BITS) % 10;
//		ndec++;
//	} while (fr != 0 && ndec < max_dec);
//
//	if (ndec > 1 && str[slen-1] == '0')
//		str[slen-1] = '\0'; /* cut off trailing 0 */
//	else
//		str[slen] = '\0';
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

#if 1
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
#endif

#if 0
static inline fixedpt fixedpt_sqrt_HR(fixedpt x) {
   fixedpt y = 0;
   if(x < 1)
      y = 2;
   else if(x < 1)
      y = 2 + (fixedpt_mul(77283, (x - (1))) >> 10);
   else if(x < 3)
      y = 3 + (fixedpt_mul(55262, (x - (1))) >> 10);
   else if(x < 7)
      y = 5 + (fixedpt_mul(39515, (x - (3))) >> 10);
   else if(x < 14)
      y = 7 + (fixedpt_mul(28255, (x - (7))) >> 10);
   else if(x < 28)
      y = 10 + (fixedpt_mul(20204, (x - (14))) >> 10);
   else if(x < 55)
      y = 15 + (fixedpt_mul(14447, (x - (28))) >> 10);
   else if(x < 109)
      y = 21 + (fixedpt_mul(10330, (x - (55))) >> 10);
   else if(x < 214)
      y = 29 + (fixedpt_mul(7387, (x - (109))) >> 10);
   else if(x < 418)
      y = 41 + (fixedpt_mul(5282, (x - (214))) >> 10);
   else if(x < 818)
      y = 57 + (fixedpt_mul(3777, (x - (418))) >> 10);
   else if(x < 1601)
      y = 80 + (fixedpt_mul(2700, (x - (818))) >> 10);
   else if(x < 3132)
      y = 113 + (fixedpt_mul(1931, (x - (1601))) >> 10);
   else if(x < 6125)
      y = 158 + (fixedpt_mul(1380, (x - (3132))) >> 10);
   else if(x < 11980)
      y = 221 + (fixedpt_mul(987, (x - (6125))) >> 10);
   else if(x < 23430)
      y = 309 + (fixedpt_mul(706, (x - (11980))) >> 10);
   else if(x < 45825)
      y = 432 + (fixedpt_mul(504, (x - (23430))) >> 10);
   else if(x < 89624)
      y = 605 + (fixedpt_mul(361, (x - (45825))) >> 10);
   else if(x < 175286)
      y = 846 + (fixedpt_mul(258, (x - (89624))) >> 10);
   else if(x < 342821)
      y = 1184 + (fixedpt_mul(184, (x - (175286))) >> 10);
   else if(x < 670481)
      y = 1656 + (fixedpt_mul(131, (x - (342821))) >> 10);
   else if(x < 1311312)
      y = 2315 + (fixedpt_mul(94, (x - (670481))) >> 10);
   else if(x < 2564635)
      y = 3238 + (fixedpt_mul(67, (x - (1311312))) >> 10);
   else if(x < 5015856)
      y = 4529 + (fixedpt_mul(48, (x - (2564635))) >> 10);
   else if(x < 9809896)
      y = 6334 + (fixedpt_mul(34, (x - (5015856))) >> 10);
   else if(x < 19185970)
      y = 8858 + (fixedpt_mul(24, (x - (9809896))) >> 10);
   else if(x < 37523480)
      y = 12389 + (fixedpt_mul(17, (x - (19185970))) >> 10);
   else if(x < 73387563)
      y = 17325 + (fixedpt_mul(12, (x - (37523480))) >> 10);
   else if(x < 143529713)
      y = 24230 + (fixedpt_mul(9, (x - (73387563))) >> 10);
   else if(x < 280712122)
      y = 33885 + (fixedpt_mul(6, (x - (143529713))) >> 10);
   else if(x < 549010334)
      y = 47388 + (fixedpt_mul(4, (x - (280712122))) >> 10);
   else if(x < 1073741824)
      y = 66272 + (fixedpt_mul(3, (x - (549010334))) >> 10);
   else
      y = 92681;
   return y;
}

static inline fixedpt fixedpt_sqrt(fixedpt x) {
   fixedpt y = 0;
   if(x < 1)
      y = 64;
   else if(x < 1)
      y = 64 + fixedpt_mul(109295, (x - (1)));
   else if(x < 3)
      y = 89 + fixedpt_mul(78152, (x - (1)));
   else if(x < 7)
      y = 125 + fixedpt_mul(55883, (x - (3)));
   else if(x < 14)
      y = 175 + fixedpt_mul(39959, (x - (7)));
   else if(x < 28)
      y = 244 + fixedpt_mul(28573, (x - (14)));
   else if(x < 55)
      y = 342 + fixedpt_mul(20431, (x - (28)));
   else if(x < 109)
      y = 478 + fixedpt_mul(14609, (x - (55)));
   else if(x < 214)
      y = 669 + fixedpt_mul(10446, (x - (109)));
   else if(x < 418)
      y = 936 + fixedpt_mul(7470, (x - (214)));
   else if(x < 818)
      y = 1309 + fixedpt_mul(5341, (x - (418)));
   else if(x < 1601)
      y = 1831 + fixedpt_mul(3819, (x - (818)));
   else if(x < 3132)
      y = 2561 + fixedpt_mul(2731, (x - (1601)));
   else if(x < 6125)
      y = 3581 + fixedpt_mul(1952, (x - (3132)));
   else if(x < 11980)
      y = 5009 + fixedpt_mul(1396, (x - (6125)));
   else if(x < 23430)
      y = 7005 + fixedpt_mul(998, (x - (11980)));
   else if(x < 45825)
      y = 9796 + fixedpt_mul(714, (x - (23430)));
   else if(x < 89624)
      y = 13700 + fixedpt_mul(510, (x - (45825)));
   else if(x < 175286)
      y = 19159 + fixedpt_mul(365, (x - (89624)));
   else if(x < 342821)
      y = 26795 + fixedpt_mul(261, (x - (175286)));
   else if(x < 670481)
      y = 37472 + fixedpt_mul(186, (x - (342821)));
   else if(x < 1311312)
      y = 52405 + fixedpt_mul(133, (x - (670481)));
   else if(x < 2564635)
      y = 73288 + fixedpt_mul(95, (x - (1311312)));
   else if(x < 5015856)
      y = 102492 + fixedpt_mul(68, (x - (2564635)));
   else if(x < 9809896)
      y = 143335 + fixedpt_mul(48, (x - (5015856)));
   else if(x < 19185970)
      y = 200452 + fixedpt_mul(34, (x - (9809896)));
   else if(x < 37523480)
      y = 280331 + fixedpt_mul(24, (x - (19185970)));
   else if(x < 73387563)
      y = 392041 + fixedpt_mul(17, (x - (37523480)));
   else if(x < 143529713)
      y = 548265 + fixedpt_mul(12, (x - (73387563)));
   else if(x < 280712122)
      y = 766744 + fixedpt_mul(9, (x - (143529713)));
   else if(x < 549010334)
      y = 1072285 + fixedpt_mul(6, (x - (280712122)));
   else if(x < 1073741824)
      y = 1499582 + fixedpt_mul(4, (x - (549010334)));
   else
      y = 2097152;
   return y;
}

static inline fixedpt fixedpt_sqrt_HP(fixedpt x) {
   fixedpt y = 0;
   if(x < 1)
      y = 362;
   else if(x < 1)
      y = 362 + (fixedpt_mul(156585, (x - (1))) << 24);
   else if(x < 3)
      y = 495 + (fixedpt_mul(114499, (x - (1))) << 24);
   else if(x < 6)
      y = 677 + (fixedpt_mul(83724, (x - (3))) << 24);
   else if(x < 12)
      y = 925 + (fixedpt_mul(61221, (x - (6))) << 24);
   else if(x < 22)
      y = 1266 + (fixedpt_mul(44766, (x - (12))) << 24);
   else if(x < 42)
      y = 1731 + (fixedpt_mul(32734, (x - (22))) << 24);
   else if(x < 80)
      y = 2368 + (fixedpt_mul(23936, (x - (42))) << 24);
   else if(x < 149)
      y = 3238 + (fixedpt_mul(17502, (x - (80))) << 24);
   else if(x < 279)
      y = 4429 + (fixedpt_mul(12798, (x - (149))) << 24);
   else if(x < 523)
      y = 6057 + (fixedpt_mul(9358, (x - (279))) << 24);
   else if(x < 979)
      y = 8284 + (fixedpt_mul(6843, (x - (523))) << 24);
   else if(x < 1831)
      y = 11329 + (fixedpt_mul(5003, (x - (979))) << 24);
   else if(x < 3425)
      y = 15493 + (fixedpt_mul(3659, (x - (1831))) << 24);
   else if(x < 6405)
      y = 21188 + (fixedpt_mul(2675, (x - (3425))) << 24);
   else if(x < 11980)
      y = 28976 + (fixedpt_mul(1956, (x - (6405))) << 24);
   else if(x < 22406)
      y = 39626 + (fixedpt_mul(1430, (x - (11980))) << 24);
   else if(x < 41905)
      y = 54192 + (fixedpt_mul(1046, (x - (22406))) << 24);
   else if(x < 78372)
      y = 74111 + (fixedpt_mul(764, (x - (41905))) << 24);
   else if(x < 146576)
      y = 101353 + (fixedpt_mul(559, (x - (78372))) << 24);
   else if(x < 274132)
      y = 138607 + (fixedpt_mul(408, (x - (146576))) << 24);
   else if(x < 512695)
      y = 189555 + (fixedpt_mul(299, (x - (274132))) << 24);
   else if(x < 958864)
      y = 259229 + (fixedpt_mul(218, (x - (512695))) << 24);
   else if(x < 1793310)
      y = 354514 + (fixedpt_mul(159, (x - (958864))) << 24);
   else if(x < 3353925)
      y = 484822 + (fixedpt_mul(116, (x - (1793310))) << 24);
   else if(x < 6272656)
      y = 663027 + (fixedpt_mul(85, (x - (3353925))) << 24);
   else if(x < 11731392)
      y = 906735 + (fixedpt_mul(62, (x - (6272656))) << 24);
   else if(x < 21940556)
      y = 1240023 + (fixedpt_mul(45, (x - (11731392))) << 24);
   else if(x < 41034174)
      y = 1695816 + (fixedpt_mul(33, (x - (21940556))) << 24);
   else if(x < 76743880)
      y = 2319144 + (fixedpt_mul(24, (x - (41034174))) << 24);
   else if(x < 143529713)
      y = 3171588 + (fixedpt_mul(17, (x - (76743880))) << 24);
   else if(x < 268435456)
      y = 4337364 + (fixedpt_mul(13, (x - (143529713))) << 24);
   else
      y = 5931641;
   return y;
}
#endif


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
	/* Alternative Taylor implementation */
//	fixedptd sum = 0 ;
//	int i ;
//	for(i=0; i<5; i++)
//	{
//		sum = (fixedptd)z * sum ;
//		sum += (fixedptd)z * (fixedptd)EXP_P[4-i] ;
//	}
//
//	sum = sum >> FIXEDPT_FBITS ;
//	R = FIXEDPT_TWO + sum ;

	xp = FIXEDPT_ONE + fixedpt_div(fixedpt_mul(fp, FIXEDPT_TWO), R - fp);
	if (k < 0)
		k = FIXEDPT_ONE >> (-k >> FIXEDPT_FBITS);
	else
		k = FIXEDPT_ONE << (k >> FIXEDPT_FBITS);
	return (fixedpt_mul(k, xp));
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

static inline void fixedpt_print_HR(fixedpt A) {
	char num[20];

	fixedpt_str_HR(A, num, -2);
	printf("%s\n",num) ;
//	puts(num);
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

static inline float
fixedpt_from_HP_to_float(fixedpt A)
{
	//char num[20];
	//fixedpt_str_HP(A, num, -2);
	//return strtof(num, NULL);
printf("Not implemented yet!\n");
return 0.0 ;
}

#endif
