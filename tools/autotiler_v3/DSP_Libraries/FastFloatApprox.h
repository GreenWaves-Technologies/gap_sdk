/*=====================================================================*
 *                   Copyright (C) 2012 Paul Mineiro                   *
 * All rights reserved.                                                *
 *                                                                     *
 * Redistribution and use in source and binary forms, with             *
 * or without modification, are permitted provided that the            *
 * following conditions are met:                                       *
 *                                                                     *
 *     * Redistributions of source code must retain the                *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer.                                       *
 *                                                                     *
 *     * Redistributions in binary form must reproduce the             *
 *     above copyright notice, this list of conditions and             *
 *     the following disclaimer in the documentation and/or            *
 *     other materials provided with the distribution.                 *
 *                                                                     *
 *     * Neither the name of Paul Mineiro nor the names                *
 *     of other contributors may be used to endorse or promote         *
 *     products derived from this software without specific            *
 *     prior written permission.                                       *
 *                                                                     *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND              *
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,         *
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES               *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE             *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER               *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,                 *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES            *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE           *
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR                *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF          *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY              *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             *
 * POSSIBILITY OF SUCH DAMAGE.                                         *
 *                                                                     *
 * Contact: Paul Mineiro <paul@mineiro.com>                            *
 *=====================================================================*/

#ifndef __FASTFLOAT_H__
#define __FASTFLOAT_H__

#include "Gap.h"
#include "FloatDefines.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#define REAL_DIV

static inline float clip_f32(float x, float lower, float upper) {
  return (float) Minf32(Maxf32(x, lower), upper);
}

static inline float fastrsqrt(float x)
{
  const float threehalfs = 1.5F;

  float x2 = x * 0.5F;
  float y  = x;
  int i  = * ( int * ) &y;
  i  = 0x5f375a86 - ( i >> 1 );
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );

  return y;
}

static inline float fastreciprocal(float x)
{
  x = fastrsqrt(x);
  return x * x;
}

#define RDIV_F32(x, y) ((x)*fastreciprocal(y))

#ifdef REAL_DIV
#define DIV_F32(x, y) ((x) / (y))
#else
#define DIV_F32(x, y) RDIV_F32((x), (y))
#endif

static inline  float
fastpow2 (float p)
{
  float offset = (p < 0) ? 1.0f : 0.0f;
  float clipp = clip_f32(p, -126.0f, 126.0f);
  int w = clipp;
  float z = clipp - w + offset;
  union { unsigned int i; float f; } v = { (unsigned int) ( (1 << 23) * (clipp + 121.2740575f + DIV_F32(27.7280233f, (4.84252568f - z)) - 1.49012907f * z) ) };

  return v.f;
}

static inline  float
fasterpow2 (float p)
{
  float clipp = clip_f32(p, -126.0f, 126.0f);
  union { unsigned int i; float f; } v = { (unsigned int) ( (1 << 23) * (clipp + 126.94269504f) ) };
  return v.f;
}

#define F32_SHIFT (1 << 23) // 2^23
#define F32_SHIFT_DIV2 (1 << 22) // 2^23
#define F32_LN2 (0.6931472f)
#define F32_EXPBIAS 127
#define F32_FACA (float)(F32_SHIFT / F32_LN2)
#define F32_FACA_DIV2 (float)(F32_SHIFT_DIV2 / F32_LN2)
#define F32_FACB (float)(F32_SHIFT * F32_EXPBIAS)

static inline float
fastexp(float x)
{
	union {
		float bf;
		signed int bf_i;
	} res;
	res.bf_i = x * F32_FACA + F32_FACB;
	return res.bf;
}

static inline float
fastexp_div2(float x)
{
	union {
		float bf;
		signed int bf_i;
	} res;
	res.bf_i = x * F32_FACA_DIV2 + F32_FACB;
	return res.bf;
}

static inline float
fasterexp (float p)
{
  return fasterpow2 (1.442695040f * p);
}


static inline float
fastlog2 (float x)
{
  union { float f; unsigned int i; } vx = { x };
  union { unsigned int i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f
           - 1.498030302f * mx.f
           - DIV_F32(1.72587999f, (0.3520887068f + mx.f));
}

static inline float
fastlog (float x)
{
  return 0.69314718f * fastlog2 (x);
}

static inline float
fasterlog2 (float x)
{
  union { float f; unsigned int i; } vx = { x };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;
  return y - 126.94269504f;
}

static inline float
fasterlog (float x)
{
//  return 0.69314718f * fasterlog2 (x);

  union { float f; unsigned int i; } vx = { x };
  float y = vx.i;
  y *= 8.2629582881927490e-8f;
  return y - 87.989971088f;
}

static inline  float
fastpow (float x,
         float p)
{
  return ((x<0)&&(((int) p) & 0x1)?-1.0:1.0) * fastpow2 (p * fastlog2 (Absf32(x)));
}

static inline  float
fasterpow (float x,
				   float p)
{
  return ((x<0)&&(((int) p) & 0x1)?-1.0:1.0) * fasterpow2 (p * fasterlog2 (Absf32(x)));
}

static inline float
fastsinh (float p)
{
  return 0.5f * (fastexp (p) - fastexp (-p));
}

static inline float
fastersinh (float p)
{
  return 0.5f * (fasterexp (p) - fasterexp (-p));
}

static inline float
fastcosh (float p)
{
  return 0.5f * (fastexp (p) + fastexp (-p));
}

static inline float
fastercosh (float p)
{
  return 0.5f * (fasterexp (p) + fasterexp (-p));
}

static inline float
fasttanh(float x)
{
	f16 a = fastexp(x);
	f16 b = fastexp(-x);
	return (a - b)/(a + b);
}

static inline float
fastertanh (float p)
{
  return -1.0f + DIV_F32(2.0f, (1.0f + fasterexp (-2.0f * p)));
}

static inline float
fastsigmoid (float x)
{
	f16 a = fastexp_div2(x);
	f16 b = fastexp_div2(-x);
	return a/(a + b);
}

static inline float
fastersigmoid (float x)
{
  return DIV_F32(1.0f, (1.0f + fasterexp (-x)));
}

static inline float fastswish(float x, float beta) {
	return (x * fastsigmoid(beta * x));
}

// fasterfc: not actually faster than erfcf(3) on newer machines!
// ... although vectorized version is interesting
//     and fastererfc is very fast

static inline float
fasterfc (float x)
{
  static const float k = 3.3509633149424609f;
  static const float a = 0.07219054755431126f;
  static const float b = 15.418191568719577f;
  static const float c = 5.609846028328545f;

  union { float f; unsigned int i; } vc = { c * x };
  float xsq = x * x;
  float xquad = xsq * xsq;

  vc.i |= 0x80000000;

  return 2.0f / (1.0f + fastpow2 (k * x)) - a * x * (b * xquad - 1.0f) * fasterpow2 (vc.f);
}

static inline float
fastererfc (float x)
{
  static const float k = 3.3509633149424609f;

  return 2.0f / (1.0f + fasterpow2 (k * x));
}

// fasterf: not actually faster than erff(3) on newer machines! 
// ... although vectorized version is interesting
//     and fastererf is very fast

static inline float
fasterf (float x)
{
  return 1.0f - fasterfc (x);
}

static inline float
fastererf (float x)
{
  return 1.0f - fastererfc (x);
}

static inline float
fastinverseerf (float x)
{
  static const float invk = 0.30004578719350504f;
  static const float a = 0.020287853348211326f;
  static const float b = 0.07236892874789555f;
  static const float c = 0.9913030456864257f;
  static const float d = 0.8059775923760193f;

  float xsq = x * x;

  return invk * fastlog2 ((1.0f + x) / (1.0f - x)) 
       + x * (a - b * xsq) / (c - d * xsq);
}

static inline float
fasterinverseerf (float x)
{
  static const float invk = 0.30004578719350504f;

  return invk * fasterlog2 ((1.0f + x) / (1.0f - x));
}


// http://www.devmaster.net/forums/showthread.php?t=5784
// fast sine variants are for x \in [ -\pi, pi ]
// fast cosine variants are for x \in [ -\pi, pi ]
// fast tangent variants are for x \in [ -\pi / 2, pi / 2 ]
// "full" versions of functions handle the entire range of inputs
// although the range reduction technique used here will be hopelessly
// inaccurate for |x| >> 1000
//
// WARNING: fastsinfull, fastcosfull, and fasttanfull can be slower than
// libc calls on older machines (!) and on newer machines are only 
// slighly faster.  however:
//   * vectorized versions are competitive
//   * faster full versions are competitive

static inline float
fastsin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.78444488374548933f;
  union { float f; unsigned int i; } p = { 0.20363937680730309f };
  union { float f; unsigned int i; } r = { 0.015124940802184233f };
  union { float f; unsigned int i; } s = { -0.0032225901625579573f };

  union { float f; unsigned int i; } vx = { x };
  unsigned int sign = vx.i & 0x80000000;
  vx.i = vx.i & 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;
  float qpproxsq = qpprox * qpprox;

  p.i |= sign;
  r.i |= sign;
  s.i ^= sign;

  return q * qpprox + qpproxsq * (p.f + qpproxsq * (r.f + qpproxsq * s.f));
}

static inline float
fastersin (float x)
{
  static const float fouroverpi = 1.2732395447351627f;
  static const float fouroverpisq = 0.40528473456935109f;
  static const float q = 0.77633023248007499f;
  union { float f; unsigned int i; } p = { 0.22308510060189463f };

  union { float f; unsigned int i; } vx = { x };
  unsigned int sign = vx.i & 0x80000000;
  vx.i &= 0x7FFFFFFF;

  float qpprox = fouroverpi * x - fouroverpisq * x * vx.f;

  p.i |= sign;

  return qpprox * (q + p.f * qpprox);
}

static inline float
fastsinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastsin ((half + k) * twopi - x);
}

static inline float
fastersinfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  return fastersin ((half + k) * twopi - x);
}

static inline float
fastcos (float x)
{
  static const float halfpi = 1.5707963267948966f;
  static const float halfpiminustwopi = -4.7123889803846899f;
  float offset = (x > halfpi) ? halfpiminustwopi : halfpi;
  return fastsin (x + offset);
}

static inline float
fastercos (float x)
{
  static const float twooverpi = 0.63661977236758134f;
  static const float p = 0.54641335845679634f;

  union { float f; unsigned int i; } vx = { x };
  vx.i &= 0x7FFFFFFF;

  float qpprox = 1.0f - twooverpi * vx.f;

  return qpprox + p * qpprox * (1.0f - qpprox * qpprox);
}

static inline float
fastcosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsinfull (x + halfpi);
}

static inline float
fastercosfull (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastersinfull (x + halfpi);
}

static inline float
fasttan (float x)
{
  static const float halfpi = 1.5707963267948966f;
  return fastsin (x) / fastsin (x + halfpi);
}

static inline float
fastertan (float x)
{
  return fastersin (x) / fastercos (x);
}

static inline float
fasttanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastsin (xnew) / fastcos (xnew);
}

static inline float
fastertanfull (float x)
{
  static const float twopi = 6.2831853071795865f;
  static const float invtwopi = 0.15915494309189534f;

  int k = x * invtwopi;
  float half = (x < 0) ? -0.5f : 0.5f;
  float xnew = x - (half + k) * twopi;

  return fastersin (xnew) / fastercos (xnew);
}

#pragma GCC diagnostic pop
#endif
