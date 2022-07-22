/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#ifndef __FASTFLOATAPPROX16_H__
#define __FASTFLOATAPPROX16_H__

#define REAL_DIV

// Define for disable ieee16 workaround
// #define BFLOAT16_FLOAT32

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include "Gap.h"
#include "FastFloatApprox.h"

// Clipping functions

static inline f16 clip_f16(f16 x, f16 lower, f16 upper)
{
	x = (f16)Maxf16(x, lower);
	return (f16)Minf16(x, upper);
}

static inline f16a clip_f16a(f16a x, f16a lower, f16a upper)
{
	x = (f16a)Maxf16a(x, lower);
	return (f16a)Minf16a(x, upper);
}

static inline v2h clip_v2h(v2h x, f16 lower, f16 upper)
{
	v2h lower_v = (v2h){lower, lower};
	v2h upper_v = (v2h){upper, upper};
	x = (v2h)Maxv2h(x, lower_v);
	return (v2h)Minv2h(x, upper_v);
}

static inline v2ah clip_v2ah(v2ah x, f16a lower, f16a upper)
{
	v2ah lower_v = (v2ah){lower, lower};
	v2ah upper_v = (v2ah){upper, upper};
	x = (v2ah)Maxv2ah(x, lower_v);
	return (v2ah)Minv2ah(x, upper_v);
}

static inline f16 fastsquare_f16(f16 x)
{
	return x * x;
}

static inline f16a fastsquare_f16a(f16a x)
{
	return x * x;
}

static inline v2h fastsquare_v2h(v2h x)
{
	return x * x;
}

static inline v2ah fastsquare_v2ah(v2ah x)
{
	return x * x;
}

// Alias to float versions if in emul and float emulation on
#if defined(__EMUL__) && defined(__FLOAT_EMUL__)
#include "FastFloatApprox.h"
#define fastsigmoid_f16(__x) fastsigmoid(__x)
#define fastsigmoid_f16a(__x) fastsigmoid(__x)
#define fastsigmoid_v2h(__x) ((v2h){fastsigmoid((__x)[0]), fastsigmoid((__x)[1])})
#define fastsigmoid_v2ah(__x) ((v2ah){fastsigmoid((__x)[0]), fastsigmoid((__x)[1])})
#define fasttanh_f16(__x) fasttanh(__x)
#define fasttanh_f16a(__x) fasttanh(__x)
#define fasttanh_v2h(__x) ((v2h){fasttanh((__x)[0]), fasttanh((__x)[0])})
#define fasttanh_v2ah(__x) ((v2ah){fasttanh((__x)[0]), fasttanh((__x)[0])})
#define fastswish_f16(__x, __beta) fastswish(__x, (float)(__beta))
#define fastswish_f16a(__x, __beta) fastswish(__x, (float)(__beta))
#define fastswish_v2h(__x, __beta) ((v2h){fastswish((__x)[0], (float)(__beta)), fastswish((__x)[1], (float)(__beta))})
#define fastswish_v2ah(__x, __beta) ((v2ah){fastswish((__x)[0], (float)(__beta)), fastswish((__x)[1], (float)(__beta))})
#define fastexp_f16(__x) fastexp(__x)
#define fastexp_f16a(__x) fastexp(__x)
#define fastexp_v2h(__x) ((v2h){fastexp((__x)[0]), fastexp((__x)[1])})
#define fastexp_v2ah(__x) ((v2ah){fastexp((__x)[0]), fastexp((__x)[1])})
#else // defined(__EMUL__) && defined(__FLOAT_EMUL__)

/* ____           _                           _
/ |  _ \ ___  ___(_)_ __  _ __ ___   ___ __ _| |
/ | |_) / _ \/ __| | '_ \| '__/ _ \ / __/ _` | |
/ |  _ <  __/ (__| | |_) | | | (_) | (_| (_| | |
/ |_| \_\___|\___|_| .__/|_|  \___/ \___\__,_|_|
/                  |_|
*/

static inline f16 rsqrt_f16(f16 x)
{
	f16 xhalf = (f16)-0.5f * x;
	int i = *(short *)&x;
	i = (unsigned short)0x59b8 - (i >> 1);
	x = *(f16 *)&i;
	x *= (f16)1.5f + (xhalf * x * x);
	return x;
}

static inline v2h rsqrt_v2h(v2h x)
{
	v2h xhalf = (f16)-0.5f * x;
	v2s i = *(v2s *)&x;
	i = (unsigned short)0x59b8 - (i >> 1);
	x = *(v2h *)&i;
	x *= (f16)1.5f + (xhalf * x * x);
	return x;
}

static inline f16 reciprocal_f16(f16 x)
{
	x = rsqrt_f16(x);
	return x * x;
}

static inline v2h reciprocal_v2h(v2h x)
{
	x = rsqrt_v2h(x);
	return x * x;
}

#define RDIV_F16(x, y) ((x)*reciprocal_f16(y))
#define RDIV_V2H(x, y) ((x)*reciprocal_v2h(y))

#ifdef REAL_DIV
#define DIV_F16(x, y) ((x) / (y))
#define DIV_V2H(x, y) ((x) / (y))
#else
#define DIV_F16(x, y) RDIV_F16((x), (y))
#define DIV_V2H(x, y) RDIV_V2H((x), (y))
#endif

/*  _             ____
// | | ___   __ _|___ \
// | |/ _ \ / _` | __) |
// | | (_) | (_| |/ __/
// |_|\___/ \__, |_____|
//           |___/
*/

static inline v2h fastlog2_v2h(v2h x)
{
	v2u vx = *((v2u *)&x);
	v2u mx = (vx & 0x3ff) | 0x3800;
	v2h mxf = *((v2h *)&mx);
	v2h vxf = Cvt_v2u_v2h(vx);
	v2h y = vxf / (f16)1024.0f;
	return y - (f16)12.22544637f - (f16)1.498030302f * mxf - DIV_V2H((f16)1.72587999f, ((f16)0.3520887068f + mxf));
}

static inline v2h fastlog2_v2ah_v2h(v2ah x)
{
	v2u vx = *((v2u *)&x);
	v2u mx = (vx & 0x7f) | 0x3f00;
	v2ah mxf = *((v2ah *)&mx);
	// v2h mxf_h = Cvt_v2ah_v2h(mxf);
	v2h mxf_h = (v2h){(f16)mxf[0], (f16)mxf[1]};
	v2h vxf_h = Cvt_v2u_v2h(vx);
	v2h y = vxf_h / (f16)(128.0f);
	return y - (f16)124.22544637f - (f16)1.498030302f * mxf_h - DIV_V2H((f16)1.72587999f, ((f16)0.3520887068f + mxf_h));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastlog2_v2ah(v2ah x)
{
	return (v2ah){(f16a)fastlog2((float)x[0]), (f16a)fastlog2((float)x[1])};
}
#else
static inline v2ah fastlog2_v2ah(v2ah x)
{
	v2h y = fastlog2_v2ah_v2h(x);
	return Cvt_v2h_v2ah(y);
}
#endif

static inline f16 fastlog2_f16(f16 x)
{
	unsigned short vx = *((unsigned short *)&x);
	unsigned short mx = (vx & 0x3ff) | 0x3800;
	f16 mxf = *((f16 *)&mx);
	f16 vxf = (f16)vx;
	f16 y = vxf / (f16)1024.0f;
	return y - (f16)12.22544637f - (f16)1.498030302f * mxf - DIV_F16((f16)1.72587999f, ((f16)0.3520887068f + mxf));
}

static inline f16 fastlog2_f16a_f16(f16a x)
{
	unsigned short vx = *((unsigned short *)&x);
	unsigned short mx = (vx & 0x7f) | 0x3f00;
	f16a mxf = *((f16a *)&mx);
	f16 mxf_h = (f16)mxf;
	f16 vxf_h = (f16)vx;
	f16 y = vxf_h / (f16)128.0f;
	return y - (f16)124.22544637f - (f16)1.498030302f * mxf - DIV_F16((f16)1.72587999f, ((f16)0.3520887068f + mxf));
}

#ifdef BFLOAT16_FLOAT32
static inline f16a fastlog2_f16a(f16a x)
{
	return (f16a)fastlog2((float)x);
}
#else
static inline f16a fastlog2_f16a(f16a x)
{
	return (f16a)fastlog2_f16a_f16(x);
}
#endif
/*  _
// | | ___   __ _
// | |/ _ \ / _` |
// | | (_) | (_| |
// |_|\___/ \__, |
//          |___/
*/

static inline v2h fastlog_v2h(v2h x)
{
	return (f16)0.69314718f * fastlog2_v2h(x);
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastlog_v2ah(v2ah x)
{
	return (v2ah){(f16a)fastlog((float)x[0]), (f16a)fastlog((float)x[1])};
}
#else
static inline v2ah fastlog_v2ah(v2ah x)
{
	return Cvt_v2h_v2ah((f16)0.69314718f * fastlog2_v2ah_v2h(x));
}
#endif
static inline f16 fastlog_f16(f16 x)
{
	return (f16)0.69314718f * fastlog2_f16(x);
}

#ifdef BFLOAT16_FLOAT32
static inline f16a fastlog_f16a(f16a x)
{
	return (f16a)fastlog((float)x);
}
#else
static inline f16a fastlog_f16a(f16a x)
{
	return (f16a)((f16)0.69314718f * fastlog2_f16a_f16(x));
}
#endif

/*                      ____
//  _ __   _____      _|___ \
// | '_ \ / _ \ \ /\ / / __) |
// | |_) | (_) \ V  V / / __/
// | .__/ \___/ \_/\_/ |_____|
// |_|
*/

static inline v2h fastpow2_unclipped_v2h(v2h x)
{
	v2u iv_f = *((v2u *)&x);
	v2u offset_u = iv_f >> 15;
	v2h offset = Cvt_v2u_v2h(offset_u);
	v2h v_f_floor = Cvt_v2s_v2h(Cvt_v2h_v2s(x));
	v_f_floor = x - v_f_floor + offset;
	v2h a = x + (f16)9.2740838f;
	v2h b = DIV_V2H((f16)27.7280233f, ((f16)4.84252568f - v_f_floor));
	v2h c = (f16)-1.49012907f * v_f_floor;
	v2h res = (f16)(1 << 10) * (a + b + c);
	v2u res_vs = Cvt_v2h_v2u(res); // (v2u) {(unsigned short) res[0], (unsigned short) res[1]};
	return *((v2h *)&res_vs);
}

static inline v2h fastpow2_v2h(v2h x)
{
	return fastpow2_unclipped_v2h(clip_v2h(x, (f16)-15.0f, (f16)15.0f));
}

static inline f16 fastpow2_unclipped_f16(f16 x)
{
	unsigned short iv_f = *((unsigned short *)&x);
	f16 offset = (f16)(iv_f >> 15);
	signed short sv_f = (signed short)x;
	f16 v_f_floor = x - (f16)sv_f + offset;
	f16 a = x + (f16)9.2740838f;
	f16 b = DIV_F16((f16)27.7280233f, ((f16)4.84252568f - v_f_floor));
	f16 c = (f16)-1.49012907f * v_f_floor;
	f16 res = (f16)(1 << 10) * (a + b + c);
	unsigned short res_us = (unsigned short)res;
	return *((f16 *)&res_us);
}

static inline f16 fastpow2_f16(f16 x)
{
	return fastpow2_unclipped_f16(clip_f16(x, (f16)-15.0f, (f16)15.0f));
}

static inline f16 fastpow2_unclipped_f16a_f16(f16a x)
{
	unsigned short iv_f = *((unsigned short *)&x);
	f16a offset = (f16a)(iv_f >> 15);
	signed short sv_f = (signed short)x;
	f16 v_f_floor = (f16)(x - (f16a)sv_f + offset);
	f16 a = x + (f16)9.2740838f;
	f16 b = DIV_F16((f16)27.7280233f, ((f16)4.84252568f - v_f_floor));
	f16 c = (f16)-1.49012907f * v_f_floor;
	f16 res = (f16)(1 << 10) * (a + b + c);
	unsigned short res_us = (unsigned short)res;
	return *((f16 *)&res_us);
}

static inline f16 fastpow2_f16a(f16a x)
{
	return fastpow2_unclipped_f16a_f16(clip_f16(x, (f16)-15.0f, (f16)15.0f));
}

/*   _____  ___ __
//  / _ \ \/ / '_ \
// |  __/>  <| |_) |
//  \___/_/\_\ .__/
//           |_|
*/

#define F16_SHIFT (1<<10)
#define F16_SHIFT_DIV2 (1<<9)
#define F16_LN2 (0.6931472f)
#define F16_EXPBIAS 15
#define F16_FACA (f16)(F16_SHIFT / F16_LN2)
#define F16_FACA_DIV2 (f16)(F16_SHIFT_DIV2 / F16_LN2)
#define F16_FACB (f16)(F16_SHIFT * F16_EXPBIAS)

typedef union
{
	f16 bf;
	signed short bf_i;
} fastfloat_f16_short_t;

static inline f16 fastexp_unclipped_f16(f16 x)
{
	fastfloat_f16_short_t res;
	res.bf_i = x * F16_FACA + F16_FACB;
	return res.bf;
}

static inline f16 fastexp_div2_unclipped_f16(f16 x)
{
	fastfloat_f16_short_t res;
	res.bf_i = x * F16_FACA_DIV2 + F16_FACB;
	return res.bf;
}

static inline v2h fastexp_unclipped_v2h(v2h x)
{
	union
	{
		v2h bf;
		v2s bf_i;
	} res;
	v2h res_v2h = x * ((v2h){F16_FACA, F16_FACA}) + ((v2h){F16_FACB, F16_FACB});
	res.bf_i = Cvt_v2h_v2s(res_v2h);
	return res.bf;
}

static inline v2h fastexp_div2_unclipped_v2h(v2h x)
{
	union
	{
		v2h bf;
		v2s bf_i;
	} res;
	v2h res_v2h = x * ((v2h){F16_FACA_DIV2, F16_FACA_DIV2}) + ((v2h){F16_FACB, F16_FACB});
	res.bf_i = Cvt_v2h_v2s(res_v2h);
	return res.bf;
}

#define BF16_SHIFT 128 // 2^7
#define BF16_EXPBIAS 127
#define BF16_FACA (f16)(BF16_SHIFT / F16_LN2)
#define BF16_FACA_DIV2 (f16)(BF16_SHIFT / (F16_LN2 * 2.0f))
#define BF16_FACB (f16)(BF16_SHIFT * BF16_EXPBIAS)

static inline f16a fastexp_unclipped_f16a(f16a x)
{
	union
	{
		f16a bf;
		signed short bf_i;
	} res;
	res.bf_i = (f16)x * BF16_FACA + BF16_FACB;
	return res.bf;
}

static inline v2ah fastexp_unclipped_v2ah(v2ah x)
{
	union
	{
		v2ah bf;
		v2s bf_i;
	} res;
	// without the direct call to builtin the compiler does not insert a convert
	v2h res_v2h = ((v2h)__builtin_pulp_v2ohftov2hf(x)) * ((v2h){BF16_FACA, BF16_FACA}) + ((v2h){BF16_FACB, BF16_FACB});
	res.bf_i = Cvt_v2h_v2s(res_v2h);
	return res.bf;
}

static inline f16 fastexp_unclipped_f16a_f16(f16a x)
{
	union
	{
		f16 bf;
		signed short bf_i;
	} res;
	res.bf_i = (f16)x * F16_FACA + F16_FACB;
	return res.bf;
}

static inline f16 fastexp_div2_unclipped_f16a_f16(f16a x)
{
	union
	{
		f16 bf;
		signed short bf_i;
	} res;
	res.bf_i = (f16)x * F16_FACA_DIV2 + F16_FACB;
	return res.bf;
}

static inline v2h fastexp_unclipped_v2ah_v2h(v2ah x)
{
	union
	{
		v2h bf;
		v2s bf_i;
	} res;
	// without the direct call to builtin the compiler does not insert a convert
	v2h res_v2h = ((v2h)__builtin_pulp_v2ohftov2hf(x)) * ((v2h){F16_FACA, F16_FACA}) + ((v2h){F16_FACB, F16_FACB});
	res.bf_i = Cvt_v2h_v2s(res_v2h);
	return res.bf;
}

static inline v2h fastexp_div2_unclipped_v2ah_v2h(v2ah x)
{
	union
	{
		v2h bf;
		v2s bf_i;
	} res;
	// without the direct call to builtin the compiler does not insert a convert
	v2h res_v2h = ((v2h)__builtin_pulp_v2ohftov2hf(x)) * ((v2h){F16_FACA_DIV2, F16_FACA_DIV2}) + ((v2h){F16_FACB, F16_FACB});
	res.bf_i = Cvt_v2h_v2s(res_v2h);
	return res.bf;
}

static inline v2h fastexp_v2h(v2h x)
{
	x = clip_v2h(x, (f16)-10.0f, (f16)10.0f);
	return fastexp_unclipped_v2h(x);
}

static inline f16 fastexp_f16(f16 x)
{
	x = clip_f16(x, (f16)-10.0f, (f16)10.0f);
	return fastexp_unclipped_f16(x);
}

static inline v2ah fastexp_v2ah(v2ah x)
{
	return fastexp_unclipped_v2ah(x);
}

static inline f16a fastexp_f16a(f16a x)
{
	return fastexp_unclipped_f16a(x);
}

/*  _ __   _____      __
// | '_ \ / _ \ \ /\ / /
// | |_) | (_) \ V  V /
// | .__/ \___/ \_/\_/
// |_|
*/

static inline v2h fastpow_v2h(v2h x, v2h p)
{
	return ((v2h) {(x[0]<0)&&(((int) p[0]) & 0x1)?-1.0:1.0, (x[1]<0)&&(((int) p[1]) & 0x1)?-1.0:1.0}) * fastpow2_v2h(p * fastlog2_v2h(Absv2h(x)));
}

static inline f16 fastpow_f16(f16 x, f16 p)
{
	return ((x<0)&&(((int) p) & 0x1)?-1.0:1.0) * fastpow2_f16(p * fastlog2_f16(Absf16(x)));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastpow_v2ah(v2ah x, v2ah p)
{
	return (v2ah){(f16a)fastpow((float)x[0], (float)p[0]), (f16a)fastpow((float)x[1], (float)p[1])};
}

static inline f16a fastpow_f16a(f16a x, f16a p)
{
	return (f16a)fastpow((float)x, (float)p);
}
#else
static inline v2ah fastpow_v2ah(v2ah x, v2ah p)
{
	v2h res = ((v2h) {(x[0]<0)&&(((int) p[0]) & 0x1)?-1.0:1.0, (x[1]<0)&&(((int) p[1]) & 0x1)?-1.0:1.0}) * fastpow2_v2h(Cvt_v2ah_v2h(p) * fastlog2_v2ah_v2h(Absv2ah(x)));
	return Cvt_v2h_v2ah(res);
}

static inline f16a fastpow_f16a(f16a x, f16a p)
{
	return ((x<0)&&(((int) p) & 0x1)?-1.0:1.0) * (f16a)fastpow2_f16((f16)p * fastlog2_f16a_f16(Absf16a(x)));
}
#endif

/*  _              _
// | |_ __ _ _ __ | |__
// | __/ _` | '_ \| '_ \
// | || (_| | | | | | | |
//  \__\__,_|_| |_|_| |_|
*/

static inline v2h fasttanh_v2h(v2h x)
{
	x = clip_v2h(x, (f16)-6.0f, (f16)6.0f);
	v2h a = fastexp_unclipped_v2h(x);
	v2h b = fastexp_unclipped_v2h(-x);
	return (a - b)/(a + b);
}

static inline f16 fasttanh_f16(f16 x)
{
	x = clip_f16(x, (f16)-6.0f, (f16)6.0f);
	f16 a = fastexp_unclipped_f16(x);
	f16 b = fastexp_unclipped_f16(-x);
	return (a - b)/(a + b);
}

static inline v2ah fasttanh_v2ah(v2ah x)
{
	x = clip_v2ah(x, (f16a)-6.0f, (f16a)6.0f);
	v2h a = fastexp_unclipped_v2ah_v2h(x);
	v2h b = fastexp_unclipped_v2ah_v2h(x * (f16a)-1.0f);
	return Cvt_v2h_v2ah((a - b)/(a + b));
}

static inline f16a fasttanh_f16a(f16a x)
{
	x = clip_f16a(x, (f16a)-6.0f, (f16a)6.0f);
	f16 a = fastexp_unclipped_f16a_f16(x);
	f16 b = fastexp_unclipped_f16a_f16(-x);
	return (f16a)((a - b)/(a + b));
}

/*      _                       _     _
//  ___(_) __ _ _ __ ___   ___ (_) __| |
// / __| |/ _` | '_ ` _ \ / _ \| |/ _` |
// \__ \ | (_| | | | | | | (_) | | (_| |
// |___/_|\__, |_| |_| |_|\___/|_|\__,_|
//        |___/
*/

// static inline v2h fastsigmoid_v2h(v2h x)
// {
// 	x = clip_v2h(x, (f16)-6.0f, (f16)6.0f);
// 	return RDIV_V2H((f16)1.0f, ((f16)1.0f + fastexp_unclipped_v2h(-x)));
// }

static inline v2h fastsigmoid_v2h(v2h x)
{
	x = clip_v2h(x, (f16)-6.0f, (f16)6.0f);
	v2h a = fastexp_div2_unclipped_v2h(x);
	v2h b = fastexp_div2_unclipped_v2h(-x);
	return a/(a+b);
}

// static inline f16 fastsigmoid_f16(f16 x)
// {
// 	x = clip_f16(x, (f16)-6.0f, (f16)6.0f);
// 	return RDIV_F16((f16)1.0f, ((f16)1.0f + fastexp_unclipped_f16(-x)));
// }

static inline f16 fastsigmoid_f16(f16 x)
{
	x = clip_f16(x, (f16)-6.0f, (f16)6.0f);
	f16 a = fastexp_div2_unclipped_f16(x);
	f16 b = fastexp_div2_unclipped_f16(-x);
	return a/(a+b);
	// return DIV_F16((f16)1.0f, ((f16)1.0f + fastexp_unclipped_f16(-x)));
}

// static inline v2ah fastsigmoid_v2ah(v2ah x)
// {
// 	x = clip_v2ah(x, (f16a)-6.0f, (f16a)6.0f);
// 	v2h res = RDIV_V2H((f16)1.0f, ((f16)1.0f + fastexp_unclipped_v2ah_v2h(-x)));
// 	return Cvt_v2h_v2ah(res);
// }

static inline v2ah fastsigmoid_v2ah(v2ah x)
{
	x = clip_v2ah(x, (f16a)-6.0f, (f16a)6.0f);
	v2h a = fastexp_div2_unclipped_v2ah_v2h(x);
	v2h b = fastexp_div2_unclipped_v2ah_v2h(-x);

	return Cvt_v2h_v2ah(a/(a+b));
}


// static inline f16a fastsigmoid_f16a(f16a x)
// {
// 	x = clip_f16a(x, (f16a)-6.0f, (f16a)6.0f);
// 	return (f16a)RDIV_F16((f16)1.0f, ((f16)1.0f + fastexp_unclipped_f16a_f16(-x)));
// }

static inline f16a fastsigmoid_f16a(f16a x)
{
	x = clip_f16a(x, (f16a)-6.0f, (f16a)6.0f);
	f16 a = fastexp_div2_unclipped_f16a_f16(x);
	f16 b = fastexp_div2_unclipped_f16a_f16(-x);
	return (f16a)(a/(a+b));
}

/*                _
//   ___ ___  ___| |__
//  / __/ _ \/ __| '_ \
// | (_| (_) \__ \ | | |
//  \___\___/|___/_| |_|
*/

static inline v2h fastcosh_v2h(v2h x)
{
	return (f16)0.5f * (fastexp_v2h(x) + fastexp_v2h(-x));
}

static inline f16 fastcosh_f16(f16 x)
{
	return (f16)0.5f * (fastexp_f16(x) + fastexp_f16(-x));
}

static inline v2ah fastcosh_v2ah(v2ah x)
{
	return Cvt_v2h_v2ah((f16)0.5f * (fastexp_unclipped_v2ah_v2h(x) + fastexp_unclipped_v2ah_v2h(-x)));
}

static inline f16a fastcosh_f16a(f16a x)
{
	return (f16a)((f16)0.5f * (fastexp_unclipped_f16a_f16(x) + fastexp_unclipped_f16a_f16(-x)));
}

/*               _     _
//  _____      _(_)___| |__
// / __\ \ /\ / / / __| '_ \
// \__ \\ V  V /| \__ \ | | |
// |___/ \_/\_/ |_|___/_| |_|
*/

static inline v2h fastswish_v2h(v2h x, f16 beta)
{
	return (x * fastsigmoid_v2h(beta * x));
}

static inline f16 fastswish_f16(f16 x, f16 beta)
{
	return (x * fastsigmoid_f16(beta * x));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastswish_v2ah(v2ah x, f16a beta)
{
	return (v2ah){(f16a)fastswish((float)x[0], (float)beta), (f16a)fastswish((float)x[1], (float)beta)};
}

static inline f16a fastswish_f16a(f16a x, f16a beta)
{
	return (f16a)fastswish((float)x, (float)beta);
}
#else
static inline v2ah fastswish_v2ah(v2ah x, f16a beta)
{
	return (x * fastsigmoid_v2ah(beta * x));
}

static inline f16a fastswish_f16a(f16a x, f16a beta)
{
	return (x * fastsigmoid_f16a(beta * x));
}
#endif

#ifndef M_PI_4
#define M_PI_4 (3.1415926535897932384626433832795f/4.0f)
#endif

#define __FF_FAC_A 0.0776509570923569f
#define __FF_FAC_B -0.287434475393028f
#define __FF_FAC_C (M_PI_4 - __FF_FAC_A - __FF_FAC_B)

static inline v2h fastarctan_v2h(v2h x) {
	v2h xx = x * x;
	return (((f16)__FF_FAC_A*xx + (f16)__FF_FAC_B)*xx + (f16)__FF_FAC_C)*x;
}

static inline v2ah fastarctan_v2ah(v2ah x) {
	v2ah xx = x * x;
	return (((f16a)__FF_FAC_A*xx + (f16a)__FF_FAC_B)*xx + (f16a)__FF_FAC_C)*x;
}

static inline f16 fastarctan_f16a(f16 x) {
	f16 xx = x * x;
	return (((f16)__FF_FAC_A*xx + (f16)__FF_FAC_B)*xx + (f16)__FF_FAC_C)*x;
}

static inline f16a fastarctan_f16(f16a x) {
	f16a xx = x * x;
	return (((f16a)__FF_FAC_A*xx + (f16a)__FF_FAC_B)*xx + (f16a)__FF_FAC_C)*x;
}

#endif // #if defined(__EMUL__) && defined(__FLOAT_EMUL__)

#pragma GCC diagnostic pop
#endif // __FASTFLOATAPPROX16_H__
