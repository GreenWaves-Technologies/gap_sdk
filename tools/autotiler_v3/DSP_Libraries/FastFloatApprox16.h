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

#ifdef REAL_DIV
#define DIV_F16(x, y) ((x) / (y))
#define DIV_V2H(x, y) ((x) / (y))
#else
#define DIV_F16(x, y) ((x)*reciprocal_f16(y))
#define DIV_V2H(x, y) ((x)*reciprocal_v2h(y))
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
	return (v2ah){ (f16a) fastlog2((float)x[0]), (f16a) fastlog2((float)x[1]) };
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
	return (f16a) fastlog2((float)x);
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
	return (v2ah){ (f16a) fastlog((float)x[0]), (f16a) fastlog((float)x[1]) };
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
	return (f16a) fastlog((float)x);
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

/*   _____  ___ __  
//  / _ \ \/ / '_ \
// |  __/>  <| |_) |
//  \___/_/\_\ .__/ 
//           |_|    
*/

static inline v2h fastexp_v2h(v2h x)
{
	return fastpow2_v2h(x * (f16)1.442695040f);
}

static inline v2h fastexp_unclipped_v2h(v2h x)
{
	return fastpow2_unclipped_v2h(x * (f16)1.442695040f);
}

static inline f16 fastexp_f16(f16 x)
{
	return fastpow2_f16(x * (f16)1.442695040f);
}

static inline f16 fastexp_unclipped_f16(f16 x)
{
	return fastpow2_unclipped_f16(x * (f16)1.442695040f);
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastexp_v2ah(v2ah x)
{
	return (v2ah){ (f16a) fastexp((float)x[0]), (f16a) fastexp((float)x[1]) };
}

static inline f16a fastexp_f16a(f16a x)
{
	return (f16a)fastexp((float)x);
}
#else
static inline v2ah fastexp_v2ah(v2ah x)
{
	v2h res = fastpow2_v2h(Cvt_v2ah_v2h(x) * (f16)1.442695040f);
	return Cvt_v2h_v2ah(res);
}

static inline f16a fastexp_f16a(f16a x)
{
	return (f16a)fastpow2_f16((f16)x * (f16)1.442695040f);
}
#endif
/*  _ __   _____      __
// | '_ \ / _ \ \ /\ / /
// | |_) | (_) \ V  V / 
// | .__/ \___/ \_/\_/  
// |_|                  
*/

static inline v2h fastpow_v2h(v2h x, v2h p)
{
	return fastpow2_v2h(p * fastlog2_v2h(x));
}

static inline f16 fastpow_f16(f16 x, f16 p)
{
	return fastpow2_f16(p * fastlog2_f16(x));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastpow_v2ah(v2ah x, v2ah p)
{
	return (v2ah){ (f16a) fastpow((float)x[0], (float)p[0]), (f16a) fastpow((float)x[1], (float)p[1]) };
}

static inline f16a fastpow_f16a(f16a x, f16a p)
{
	return (f16a)fastpow((float)x, (float)p);
}
#else
static inline v2ah fastpow_v2ah(v2ah x, v2ah p)
{
	v2h res = fastpow2_v2h(Cvt_v2ah_v2h(p) * fastlog2_v2ah_v2h(x));
	return Cvt_v2h_v2ah(res);
}

static inline f16a fastpow_f16a(f16a x, f16a p)
{
	return (f16a)fastpow2_f16((f16)p * fastlog2_f16a_f16(x));
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
	x = clip_v2h(x, (f16)-5.19f, (f16)5.19f);
	return (f16)-1.0f + DIV_V2H((f16)2.0f, ((f16)1.0f + fastexp_unclipped_v2h((f16)-2.0f * x)));
}

static inline f16 fasttanh_f16(f16 x)
{
	x = clip_f16(x, (f16)-5.19f, (f16)5.19f);
	return (f16)-1.0f + DIV_F16((f16)2.0f, ((f16)1.0f + fastexp_unclipped_f16((f16)-2.0f * x)));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fasttanh_v2ah(v2ah x)
{
	return (v2ah){ (f16a) fasttanh((float)x[0]), (f16a) fasttanh((float)x[1]) };
}

static inline f16a fasttanh_f16a(f16a x)
{
	return (f16a)fasttanh((float)x);
}
#else
static inline v2ah fasttanh_v2ah(v2ah x)
{
	x = clip_v2ah(x, (f16a)-5.19f, (f16a)5.19f);
	v2h res = (f16)-1.0f + DIV_V2H((f16)2.0f, ((f16)1.0f + fastexp_unclipped_v2h((f16)-2.0f * Cvt_v2ah_v2h(x))));
	return Cvt_v2h_v2ah(res);
}

static inline f16a fasttanh_f16a(f16a x)
{
	x = clip_f16a(x, (f16a)-5.19f, (f16a)5.19f);
	return (f16a)((f16)-1.0f + DIV_F16((f16)2.0f, ((f16)1.0f + fastexp_unclipped_f16((f16)-2.0f * (f16)x))));
}
#endif

/*      _                       _     _ 
//  ___(_) __ _ _ __ ___   ___ (_) __| |
// / __| |/ _` | '_ ` _ \ / _ \| |/ _` |
// \__ \ | (_| | | | | | | (_) | | (_| |
// |___/_|\__, |_| |_| |_|\___/|_|\__,_|
//        |___/                         
*/

static inline v2h fastsigmoid_v2h(v2h x)
{
	x = clip_v2h(x, (f16)-6.0f, (f16)6.0f);
	return DIV_V2H((f16)1.0f, ((f16)1.0f + fastexp_unclipped_v2h(-x)));
}

static inline f16 fastsigmoid_f16(f16 x)
{
	x = clip_f16(x, (f16)-6.0f, (f16)6.0f);
	return DIV_F16((f16)1.0f, ((f16)1.0f + fastexp_unclipped_f16(-x)));
}

#ifdef BFLOAT16_FLOAT32
static inline v2ah fastsigmoid_v2ah(v2ah x)
{
	return (v2ah){ (f16a) fastsigmoid((float)x[0]), (f16a) fastsigmoid((float)x[1]) };
}

static inline f16a fastsigmoid_f16a(f16a x)
{
	return (f16a)fastsigmoid((float)x);
}
#else
static inline v2ah fastsigmoid_v2ah(v2ah x)
{
	x = clip_v2ah(x, (f16a)-6.0f, (f16a)6.0f);
	v2h res = DIV_V2H((f16)1.0f, ((f16)1.0f + fastexp_unclipped_v2h(Cvt_v2ah_v2h(-x))));
	return Cvt_v2h_v2ah(res);
}

static inline f16a fastsigmoid_f16a(f16a x)
{
	x = clip_f16a(x, (f16a)-6.0f, (f16a)6.0f);
	return (f16a)DIV_F16((f16)1.0f, ((f16)1.0f + fastexp_unclipped_f16((f16)-x)));
}
#endif

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
	return (v2ah){ (f16a) fastswish((float)x[0], (float)beta), (f16a) fastswish((float)x[1], (float)beta) };
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

#endif // #if defined(__EMUL__) && defined(__FLOAT_EMUL__)

#pragma GCC diagnostic pop
#endif // __FASTFLOATAPPROX16_H__
