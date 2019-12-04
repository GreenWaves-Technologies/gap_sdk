/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/* 
 * Authors: Eric Flamand, GreenWaves Technologies (eric.flamand@greenwaves-technologies.com)
 *          Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef _JPEG_CONSTANTS_H_
#define _JPEG_CONSTANTS_H_

#define Q8      8
#define Q11     11

typedef struct
{
    const short int FDCT_FACTORS[64];
    const unsigned char ZIGZAG_LUT[64];

    const unsigned char QUANT_TAB_LUMIN[64];
    const unsigned char QUANT_TAB_CHROM[64];

    const unsigned short int HUFTAB_LUMIN_AC_Code[256];  /* Fliped bits representation */
    const unsigned char HUFTAB_LUMIN_AC_Size[256];

    const unsigned short int HUFTAB_CHROM_AC_Code[250]; /* Fliped bits representation */
    const unsigned char HUFTAB_CHROM_AC_Size[250];

    const unsigned short int HUFTAB_LUMIN_DC_Code[12]; /* Fliped bits representation */
    const unsigned char HUFTAB_LUMIN_DC_Size[12];

    const unsigned short int HUFTAB_CHROM_DC_Code[11]; /* Fliped bits representation */
    const unsigned char HUFTAB_CHROM_DC_Size[11];
} JPG_CST;

extern JPG_CST JpegConstants;

#endif
