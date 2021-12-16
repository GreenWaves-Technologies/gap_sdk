/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __FLOAT_DEFINES_H__
#define __FLOAT_DEFINES_H__


#ifdef __gap9__
	#define Absf16(a)	__builtin_pulp_f16abs((a))
	#define Maxf16(a, b)	__builtin_pulp_f16max((a), (b))
	#define Minf16(a, b)	__builtin_pulp_f16min((a), (b))

	#define Absv2h(a)	__builtin_pulp_f16abs2((a))
	#define Maxv2h(a, b)	__builtin_pulp_f16max2((a), (b))
	#define Minv2h(a, b)	__builtin_pulp_f16min2((a), (b))
	
	#define Absf16a(a)	__builtin_pulp_f16altabs((a))
	#define Maxf16a(a, b)	__builtin_pulp_f16altmax((a), (b))
	#define Minf16a(a, b)	__builtin_pulp_f16altmin((a), (b))

	#define Absv2ah(a)	__builtin_pulp_f16altabs2((a))
	#define Maxv2ah(a, b)	__builtin_pulp_f16altmax2((a), (b))
	#define Minv2ah(a, b)	__builtin_pulp_f16altmin2((a), (b))

	#define MIN_f16		((float16)(6.10e-5f))
	#define MAX_f16		((float16)(65504))
	#define MIN_f16a	((float16alt)(1.1754943508e-38f))
	#define MAX_f16a	((float16alt)(3.4028234664e38f))

	#define Absf32(a)	__builtin_pulp_f32abs((a))
	#define Maxf32(a, b)	__builtin_pulp_f32max((a), (b))
	#define Minf32(a, b)	__builtin_pulp_f32min((a), (b))

	#define Sqrtf16(a) 	__builtin_pulp_f16sqrt(a)
	#define Sqrtf16a(a)	__builtin_pulp_f16altsqrt(a)
	#define Sqrtf32(a) 	__builtin_pulp_f32sqrt(a)

	#define Cvt_v2h_v2s(a) 	((v2s) __builtin_pulp_v2hftov2hi(a))
	#define Cvt_v2h_v2u(a) 	((v2u)__builtin_pulp_v2hftov2hi_u(a))
	#define Cvt_v2ah_v2s(a) ((v2s)__builtin_pulp_v2ohftov2hi(a))
	#define Cvt_v2ah_v2u(a) ((v2u)__builtin_pulp_v2ohftov2hi_u(a))

	#define Cvt_v2s_v2h(a) 	((v2h)__builtin_pulp_v2hitov2hf(a))
	#define Cvt_v2u_v2h(a) 	((v2h)__builtin_pulp_v2hitov2hf_u(a))
	#define Cvt_v2s_v2ah(a) ((v2ah)__builtin_pulp_v2hitov2ohf(a))
	#define Cvt_v2u_v2ah(a) ((v2ah)__builtin_pulp_v2hitov2ohf_u(a))

	#define Cvt_v2h_v2ah(a) ((v2ah)__builtin_pulp_v2hftov2ohf(a))
	#define Cvt_v2ah_v2h(a) ((v2h)__builtin_pulp_v2ohftov2hf(a))
#else
	#define AbsF(a)         (((a)<0.0f)?(-(a)):(a))
	#define Absf16 			AbsF
	#define Absf16a 		AbsF
	#define MinF(a, b)      (((a)<(b))?(a):(b))
	#define Minf16			MinF
	#define Minf16a 		MinF
	#define MaxF(a, b)      (((a)>(b))?(a):(b))
	#define Maxf16 			MaxF
	#define Maxf16a 		MaxF
	#define Absf32          AbsF
	#define Minf32			MinF
	#define Maxf32			MaxF
	#define AbsF2(a)	((v2h) {((float)(a)[0]<0.0)?(-(float)(a)[0]):((float)(a)[0]), \
					 ((float)(a)[1]<0.0)?(-(float)(a)[1]):((float)(a)[1])})
	#define MinF2(a, b)	((v2h) {((float)(a)[0]<(float)(b)[0])?((float)(a)[0]):((float)(b)[0]), \
					 ((float)(a)[1]<(float)(b)[1])?((float)(a)[1]):((float)(b)[1])})
	#define MaxF2(a, b)	((v2h) {((float)(a)[0]>(float)(b)[0])?((float)(a)[0]):((float)(b)[0]), \
					 ((float)(a)[1]>(float)(b)[1])?((float)(a)[1]):((float)(b)[1])})
	#define Absv2h			AbsF2
	#define Absv2ah 		AbsF2
	#define Minv2h 			MinF2
	#define Minv2ah 		MinF2
	#define Maxv2h 			MaxF2
	#define Maxv2ah 		MaxF2

	#define MIN_FLT16	((float)(1.1754943508e-38f))
	#define MAX_FLT16	((float)(3.4028234664e38f))
	#define MIN_f16		((float)(1.1754943508e-38f))
	#define MAX_f16		((float)(3.4028234664e38f))
	#define MIN_f16a	((float)(1.1754943508e-38f))
	#define MAX_f16a	((float)(3.4028234664e38f))

	#define Sqrtf16(a) 	sqrt(a)
	#define Sqrtf16a(a) 	sqrt(a)
	#define Sqrtf32(a) 	sqrt(a)

	#define Cvt_v2h_v2s(a) (v2s) (a)
	#define Cvt_v2h_v2u(a) (v2u) (a)
	#define Cvt_v2ah_v2s(a) (v2s) (a)
	#define Cvt_v2ah_v2u(a) (v2u) (a)

	#define Cvt_v2s_v2h(a) (v2h) (a)
	#define Cvt_v2u_v2h(a) (v2h)(a)
	#define Cvt_v2s_v2ah(a) (v2ah) (a)
	#define Cvt_v2u_v2ah(a) (v2ah) (a)

	#define Cvt_v2h_v2ah(a) (v2ah) (a)
	#define Cvt_v2ah_v2h(a) (v2h) (a)
#endif //__gap9__
#define Clipf32(a, upper, lower) ((float) Maxf32(Minf32((a), (upper)), (lower)))

#endif
