#ifndef __CNN_INFOS_SQ8__
#define __CNN_INFOS_SQ8__

/*
 * Copyright (C) 2022 GreenWaves Technologies
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


#define AT_INF_BIASL_SM         0 // 0
#define AT_INF_ACTSCALE		0
#define AT_INF_ACTSCALE_LEN	1

#define AT_INF_ACTSCALEN	(AT_INF_ACTSCALE+AT_INF_ACTSCALE_LEN) // 1
#define AT_INF_ACTSCALEN_LEN	1

#define AT_INF_A0		(AT_INF_ACTSCALEN+AT_INF_ACTSCALEN_LEN) // 2
#define AT_INF_A0_LEN           2

#define AT_INF_B0		(AT_INF_A0+AT_INF_A0_LEN) // 4
#define AT_INF_B0_LEN		2

#define AT_INF_C0		(AT_INF_B0+AT_INF_B0_LEN) // 6
#define AT_INF_C0_LEN		2

#define AT_INF_BIASN		(AT_INF_C0+AT_INF_C0_LEN) // 8
#define AT_INF_BIASN_LEN	1
#define AT_INF_IN1SCALE		AT_INF_BIASN
#define AT_INF_SCALE		AT_INF_BIASN
#define AT_INF_GLOBAL_SUM_SCALE	AT_INF_BIASN

#define AT_INF_SCALEN		(AT_INF_BIASN+AT_INF_BIASN_LEN) // 9
#define AT_INF_SCALEN_LEN	1
#define AT_INF_IN1SCALEN	AT_INF_SCALEN
#define AT_INF_GLOBAL_SUM_SCALEN AT_INF_SCALEN

#define AT_INF_OUTZEROPOINT     (AT_INF_SCALEN+AT_INF_SCALEN_LEN) // 10
#define AT_INF_OUTZEROPOINT_LEN 1     
#define AT_INF_OUTSCALE		AT_INF_OUTZEROPOINT

#define AT_INF_OUTSCALEN	(AT_INF_OUTZEROPOINT+AT_INF_OUTZEROPOINT_LEN) // 11
#define AT_INF_OUTSCALEN_LEN	1

#define AT_INF_DIM		(AT_INF_OUTSCALEN+AT_INF_OUTSCALEN_LEN) // 12

#define AT_INF_PRENORM          (AT_INF_OUTSCALEN+AT_INF_OUTSCALEN_LEN) // 12
#define AT_INF_PRENORM_LEN      1

#define AT_INF_SQ16_DIM         (AT_INF_PRENORM+AT_INF_PRENORM_LEN) // 13

#define AT_INF_ADD_BIAS		AT_INF_PRENORM // 13
#define AT_INF_ADD_BIAS_LEN     2
#define AT_INF_ASYM_ADD_DIM	(AT_INF_ADD_BIAS+AT_INF_ADD_BIAS_LEN)


#define AT_INF_NE16_PADVAL      AT_INF_SQ16_DIM + 1 // 14
#define AT_INF_NE16_PADVAL_LEN  2

#define AT_INF_NE16_WOFFSET     (AT_INF_NE16_PADVAL+AT_INF_NE16_PADVAL_LEN) // 16 (Must be on word boundary)
#define AT_INF_NE16_WOFFSET_LEN 4

#define AT_INF_NE16_DIM         (AT_INF_NE16_WOFFSET+AT_INF_NE16_WOFFSET_LEN) // 20


#endif