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

#ifndef __DSP_FLOAT_TYPE_H__
#define __DSP_FLOAT_TYPE_H__

#ifdef __EMUL__
	#ifdef __FLOAT_EMUL__
		typedef float F16_DSP;
		typedef float F16V_DSP __attribute__((vector_size (8)));
		typedef int   V2S __attribute__((vector_size (8)));
	#else
		typedef short int F16_DSP;
		typedef short int F16V_DSP __attribute__((vector_size (4)));
		typedef v2s V2S;
	#endif
	typedef union {
	        F16_DSP     S;
	        F16V_DSP    V;
	} F16SV_DSP;
#else
	typedef v2s V2S;
	/* GAP9 */
	#ifdef __gap9__ 
		#ifdef F16_DSP_BFLOAT
			typedef float16alt F16_DSP;
			typedef v2ah F16V_DSP;
		#else
			typedef float16 F16_DSP;
			typedef v2h F16V_DSP;
		#endif
		typedef union {
		        F16_DSP     S;
		        F16V_DSP    V;
		} F16SV_DSP;
		#define TO_FLOAT(x) x
	/* GAP8 */
	#else 
		typedef short int F16_DSP;
		typedef short int F16V_DSP __attribute__((vector_size (4)));
		typedef union {
		        F16_DSP     S;
		        F16V_DSP    V;
		} F16SV_DSP;
	#endif // gap9
#endif // __EMUL__

#endif // __DSP_FLOAT_TYPE_H__