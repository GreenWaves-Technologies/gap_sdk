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
 */

#include "jpeg_constants.h"

#if 1
// TODO should be moved to pmsis
#define FIX2FP(Val, Precision)                      ((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)                     ((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)                      ((int)((Val)*((1 << (Precision))-1)))

#define DCTSIZE  8

void Dct8x8(short int *__restrict__ data, const short int *__restrict__ ftab)
{
    int tmp0,  tmp1,  tmp2,  tmp3;
    int tmp4,  tmp5,  tmp6,  tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5, z11, z13;
    short *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr = 0; ctr < DCTSIZE; ctr++)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = __builtin_pulp_trunch(tmp10 + tmp11);  /* phase 3 */
        dataptr[4] = __builtin_pulp_trunch(tmp10 - tmp11);

        z1 = __builtin_pulp_mulfsN(tmp12 + tmp13, FP2FIX(0.707106781f, Q11), Q11);
        dataptr[2] = __builtin_pulp_trunch(tmp13 + z1);  /* phase 5 */
        dataptr[6] = __builtin_pulp_trunch(tmp13 - z1);

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = __builtin_pulp_mulfsN(tmp10 - tmp12, FP2FIX(0.382683433f, Q11), Q11);       /* c6 */
        z2 = __builtin_pulp_mulfsN(tmp10,       FP2FIX(0.541196100f, Q11), Q11) + z5;   /* c2-c6 */
        z4 = __builtin_pulp_mulfsN(tmp12,       FP2FIX(1.306562965f, Q11), Q11) + z5;   /* c2+c6 */
        z3 = __builtin_pulp_mulfsN(tmp11,       FP2FIX(0.707106781f, Q11), Q11);            /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = __builtin_pulp_trunch(z13 + z2);  /* phase 6 */
        dataptr[3] = __builtin_pulp_trunch(z13 - z2);
        dataptr[1] = __builtin_pulp_trunch(z11 + z4);
        dataptr[7] = __builtin_pulp_trunch(z11 - z4);

        dataptr += DCTSIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr = 0; ctr < DCTSIZE; ctr++)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = __builtin_pulp_trunch(tmp10 + tmp11);  /* phase 3 */
        dataptr[DCTSIZE * 4] = __builtin_pulp_trunch(tmp10 - tmp11);

        z1 = __builtin_pulp_mulfsN(tmp12 + tmp13, FP2FIX(0.707106781f, Q11), Q11);       /* c4 */
        dataptr[DCTSIZE * 2] = __builtin_pulp_trunch(tmp13 + z1);  /* phase 5 */
        dataptr[DCTSIZE * 6] = __builtin_pulp_trunch(tmp13 - z1);

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = __builtin_pulp_mulfsN(tmp10 - tmp12, FP2FIX(0.382683433f, Q11), Q11);       /* c6 */
        z2 = __builtin_pulp_mulfsN(tmp10,       FP2FIX(0.541196100f, Q11), Q11) + z5;   /* c2-c6 */
        z4 = __builtin_pulp_mulfsN(tmp12,       FP2FIX(1.306562965f, Q11), Q11) + z5;   /* c2+c6 */
        z3 = __builtin_pulp_mulfsN(tmp11,       FP2FIX(0.707106781f, Q11), Q11);            /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCTSIZE * 5] = __builtin_pulp_trunch(z13 + z2); /* phase 6 */
        dataptr[DCTSIZE * 3] = __builtin_pulp_trunch(z13 - z2);
        dataptr[DCTSIZE * 1] = __builtin_pulp_trunch(z11 + z4);
        dataptr[DCTSIZE * 7] = __builtin_pulp_trunch(z11 - z4);

        dataptr++;  /* advance pointer to next column */
    }

    for (ctr = 0; ctr < 64; ctr++) data[ctr]  = __builtin_pulp_mulfsN(data[ctr], ftab[ctr], Q11 + 2);
}

#else
#define DCTSIZE  8

#define FLOAT2FIX(f)  ((int)((f) * (1 << 11)))
#define FIXQ          11

/* ÄÚ²¿È«¾Ö±äÁ¿¶¨Òå */
static const float AAN_DCT_FACTOR[DCTSIZE] = {
    1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
    1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
};

static int g_inited         =  0;
static int g_fdctfactor[64] = {0};
static int g_idctfactor[64] = {0};

void init_dct_module(void)
{
    int   i, j;
    float factor[64];

    // check inited
    if (g_inited) return;
    else g_inited = 1;

    // fdct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        g_fdctfactor[i] = FLOAT2FIX(factor[i]);
    }

    // idct factor
    for (i=0; i<DCTSIZE; i++) {
        for (j=0; j<DCTSIZE; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        g_idctfactor[i] = FLOAT2FIX(factor[i]);
    }
}

void init_fdct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] * 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] / qtab[i]);
    }
}

void init_idct_ftab(int *ftab, int *qtab)
{
    int   i, j;
    float factor[64];
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            factor[i * 8 + j] = 1.0f * (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[j] / 8);
        }
    }
    for (i=0; i<64; i++) {
        ftab[i] = FLOAT2FIX(factor[i] * qtab[i]);
    }
}

/* º¯ÊýÊµÏÖ */

void Dct8x8(int *__restrict__ data, const short int *__restrict__ ftab)
{
    int tmp0,  tmp1,  tmp2,  tmp3;
    int tmp4,  tmp5,  tmp6,  tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5, z11, z13;
    int *dataptr;
    int ctr;

    static int init = 0;
    if (!init)
    {
        init = 1;
        init_dct_module();
    }

    /* Pass 1: process rows. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * FLOAT2FIX(0.707106781f) >> FIXQ; /* c4 */
        dataptr[2] = tmp13 + z1;  /* phase 5 */
        dataptr[6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * FLOAT2FIX(0.382683433f) >> FIXQ;  /* c6 */
        z2 = (FLOAT2FIX(0.541196100f) * tmp10 >> FIXQ) + z5;     /* c2-c6 */
        z4 = (FLOAT2FIX(1.306562965f) * tmp12 >> FIXQ) + z5;     /* c2+c6 */
        z3 = tmp11 * FLOAT2FIX(0.707106781f) >> FIXQ;            /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += DCTSIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data;
    for (ctr=0; ctr<DCTSIZE; ctr++)
    {
        tmp0 = dataptr[DCTSIZE * 0] + dataptr[DCTSIZE * 7];
        tmp7 = dataptr[DCTSIZE * 0] - dataptr[DCTSIZE * 7];
        tmp1 = dataptr[DCTSIZE * 1] + dataptr[DCTSIZE * 6];
        tmp6 = dataptr[DCTSIZE * 1] - dataptr[DCTSIZE * 6];
        tmp2 = dataptr[DCTSIZE * 2] + dataptr[DCTSIZE * 5];
        tmp5 = dataptr[DCTSIZE * 2] - dataptr[DCTSIZE * 5];
        tmp3 = dataptr[DCTSIZE * 3] + dataptr[DCTSIZE * 4];
        tmp4 = dataptr[DCTSIZE * 3] - dataptr[DCTSIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCTSIZE * 0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[DCTSIZE * 4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * FLOAT2FIX(0.707106781f) >> FIXQ; /* c4 */
        dataptr[DCTSIZE * 2] = tmp13 + z1;  /* phase 5 */
        dataptr[DCTSIZE * 6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * FLOAT2FIX(0.382683433f) >> FIXQ;  /* c6 */
        z2 = (FLOAT2FIX(0.541196100f) * tmp10 >> FIXQ) + z5;     /* c2-c6 */
        z4 = (FLOAT2FIX(1.306562965f) * tmp12 >> FIXQ) + z5;     /* c2+c6 */
        z3 = tmp11 * FLOAT2FIX(0.707106781f) >> FIXQ;            /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCTSIZE * 5] = z13 + z2; /* phase 6 */
        dataptr[DCTSIZE * 3] = z13 - z2;
        dataptr[DCTSIZE * 1] = z11 + z4;
        dataptr[DCTSIZE * 7] = z11 - z4;

        dataptr++;  /* advance pointer to next column */
    }

    //ftab = ftab ? ftab : g_fdctfactor;
    for (ctr=0; ctr<64; ctr++) {
        data[ctr]  *= g_fdctfactor[ctr];
        data[ctr] >>= FIXQ + 2;
    }
}

#endif