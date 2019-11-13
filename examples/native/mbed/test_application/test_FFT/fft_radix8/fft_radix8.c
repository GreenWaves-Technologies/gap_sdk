#include "core_gap_memory_define.h"
#include <stdio.h>
#include <stdio.h>

#ifndef MIXED_RADIX
#include "fft.h"
#endif

#if defined(MIXED_RADIX) && !defined(FULL_TWIDDLES)
extern Complex_type twiddle_factors[];
#else
#include "twiddle_factor.h"
#endif

#include "print_float.h"

#ifdef BITREV_LUT
#include "bit_reverse.h"
#endif


GAP_L1_DATA float ROT_CONST = 0.7071f;

static Complex_type __attribute__ ((always_inline)) complex_mul (Complex_type A, Complex_type B)
{
  Complex_type c_tmp;
  c_tmp.re = A.re * B.re - A.im * B.im;
  c_tmp.im = A.re * B.im + A.im * B.re;
  return c_tmp;
}

static Complex_type __attribute__ ((always_inline)) complex_mul_real (float A, Complex_type B)
{
  Complex_type c_tmp;
  c_tmp.re = A * B.re;
  c_tmp.im = A * B.im;
  return c_tmp;
}

#ifdef MIXED_RADIX
int __attribute__ ((always_inline)) bit_rev_2_8(int value)
{
    // unsigned int temp1 = (index_torevert & 0x1)   << 2 ;
    // unsigned int temp2 = (index_torevert & 0x2)        ;
    // unsigned int temp3 = (index_torevert & 0x4)   >> 2 ;
    // unsigned int temp4 = (index_torevert & 0x8)   << 2 ;
    // unsigned int temp5 = (index_torevert & 0x10)       ;
    // unsigned int temp6 = (index_torevert & 0x20)  >> 2 ;
    // unsigned int temp7 = (index_torevert & 0x40)  << 2 ;
    // unsigned int temp8 = (index_torevert & 0x80)       ;
    // unsigned int temp9 = (index_torevert & 0x100) >> 2 ;
    //
    // unsigned int res_index_torevert = temp1 | temp2 |  temp3 |  temp4 | temp5 | temp6 | temp7 | temp8 | temp9 ;
    //
    // return res_index_torevert;

    int i;
    unsigned int new_value = 0;
    unsigned int pos1 = 0x1;
    unsigned int pos2 = 0x2;
    unsigned int pos3 = 0x4;

    for (i = 0; i < LOG2_FFT_LEN_RADIX8/3; i++)
    {
        new_value |= (value & pos1) << 2;
        new_value |= (value & pos2);
        new_value |= (value & pos3) >> 2;

        pos1 <<= 3;
        pos2 <<= 3;
        pos3 <<= 3;

    }

    return new_value;
}
#endif

int __attribute__ ((always_inline)) bit_rev_radix8(int value) //digit reverse
{
    int i;
    unsigned int new_value = 0;
    for (i = 0; i < LOG2_FFT_LEN_RADIX8/3; i++)
    {
        new_value <<= 3;
        new_value |= (value & 0x7);
        value >>= 3;
    }

    return new_value;
}


// {
//     unsigned int temp_A = (index_torevert & 0x7)<<6;
//     unsigned int temp_B = ((unsigned int)index_torevert) >> 6;
//     unsigned int res_index_torevert = ((index_torevert & 0x38) | temp_A | temp_B);
//     return res_index_torevert;
// }

void __attribute__ ((always_inline)) process_butterfly_real_radix8 (Complex_type* input, int twiddle_index, int distance, Complex_type* twiddle_ptr)
{

    #if defined(MIXED_RADIX) && !defined(FULL_TWIDDLES)
    twiddle_index = twiddle_index * 4;
    #define MAX_TWIDDLES (FFT_LEN_RADIX2)
    #elif defined(FULL_TWIDDLES)
    #define MAX_TWIDDLES (FFT_LEN_RADIX8)
    #else
    #define MAX_TWIDDLES (FFT_LEN_RADIX8/2)
    #endif

    int index = 0;
    //int twiddle_index = 0;
    float d0         = input[index].re;
    float d1         = input[index+distance].re;
    float d2         = input[index+2*distance].re;
    float d3         = input[index+3*distance].re;
    float d4         = input[index+4*distance].re;
    float d5         = input[index+5*distance].re;
    float d6         = input[index+6*distance].re;
    float d7         = input[index+7*distance].re;


    Complex_type tw1 = twiddle_ptr[twiddle_index*1];
    Complex_type tw2 = twiddle_ptr[twiddle_index*2];
    Complex_type tw3 = twiddle_ptr[twiddle_index*3];
    Complex_type tw4 = twiddle_ptr[twiddle_index*4];
    Complex_type tw5;
    Complex_type tw6;
    Complex_type tw7;
    #ifdef FULL_TWIDDLES
    tw5 = twiddle_ptr[twiddle_index*5];
    tw6 = twiddle_ptr[twiddle_index*6];
    tw7 = twiddle_ptr[twiddle_index*7];
    #else
    if(twiddle_index*5 >= MAX_TWIDDLES)
    {
      tw5 = twiddle_ptr[(twiddle_index*5)%(MAX_TWIDDLES)];
      tw5.re = -tw5.re;
      tw5.im = -tw5.im;
    }
    else
    {
      tw5 = twiddle_ptr[twiddle_index*5];
    }
    if(twiddle_index*6 >= MAX_TWIDDLES)
    {
      tw6 = twiddle_ptr[(twiddle_index*6)%(MAX_TWIDDLES)];
      tw6.re = -tw6.re;
      tw6.im = -tw6.im;
    }
    else
    {
      tw6 = twiddle_ptr[twiddle_index*6];
    }
    if(twiddle_index*7 >= MAX_TWIDDLES)
    {
      tw7 = twiddle_ptr[(twiddle_index*7)%(MAX_TWIDDLES)];
      tw7.re = -tw7.re;
      tw7.im = -tw7.im;
    }
    else
    {
      tw7 = twiddle_ptr[twiddle_index*7];
    }
    #endif // FULL_TWIDDLES


    Complex_type r0, r1, r2, r3, r4, r5, r6, r7;


    // Basic buttefly rotation
    /*

r0   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i
r1   1.0000 + 0.0000i   0.7071 - 0.7071i   0.0000 - 1.0000i  -0.7071 - 0.7071i  -1.0000 - 0.0000i  -0.7071 + 0.7071i   0.0000 + 1.0000i   0.7071 + 0.7071i
r2   1.0000 + 0.0000i   0.0000 - 1.0000i  -1.0000 - 0.0000i   0.0000 + 1.0000i   1.0000 + 0.0000i   0.0000 - 1.0000i   -1.0000 - 0.0000i  0.0000 + 1.0000i
r3   1.0000 + 0.0000i  -0.7071 - 0.7071i   0.0000 + 1.0000i   0.7071 - 0.7071i  -1.0000 - 0.0000i   0.7071 + 0.7071i   0.0000 - 1.0000i  -0.7071 + 0.7071i

r4   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i
r5   1.0000 + 0.0000i  -0.7071 + 0.7071i   0.0000 - 1.0000i   0.7071 + 0.7071i  -1.0000 - 0.0000i   0.7071 - 0.7071i   0.0000 + 1.0000i  -0.7071 - 0.7071i
r6   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i
r7   1.0000 + 0.0000i   0.7071 + 0.7071i  -0.0000 + 1.0000i  -0.7071 + 0.7071i  -1.0000 - 0.0000i  -0.7071 - 0.7071i   0.0000 - 1.0000i   0.7071 - 0.7071i

    */


    float d13   = d1 + d3;
    float d1_3   = d1 - d3;
    float d26   = d2 + d6;
    float d57   = d5 + d7;
    float d_57   = d7 - d5;
    float temp1 = ROT_CONST*(d1_3+d_57);
    float temp2 = ROT_CONST*(d57-d13);
    float d04   = d0 + d4;
    float d0_4  = d0 - d4;
    float d0246 = d04 + d26;
    float d_26  = d6 - d2;

    //Re(c1*c2) = c1.re*c2.re - c1.im*c2.im, since c1 is real = c1.re*c2.re
    r0.re = d0246 + d13 + d57;
    //r1.re = d0 + ROT_CONST*d1 - ROT_CONST*d3 - d4 - ROT_CONST*d5 + ROT_CONST*d7;
    r1.re = d0_4 + temp1;
    r2.re = d04 - d26;
    //r3.re = d0 - ROT_CONST*d1 + ROT_CONST*d3 - d4 + ROT_CONST*d5 - ROT_CONST*d7;
    r3.re = d0_4 - temp1;
    r4.re = d0246 - d13 - d57;
    r5.re = r3.re;
    r6.re = r2.re;
    r7.re = r1.re;

    //Im(c1*c2) = c1.re*c2.im + c1.im*c2.re, since c1 is real = c1.re*c2.im
    r0.im = 0.0f;
    //r1.im = -ROT_CONST*d1 - d2 - ROT_CONST*d3 + ROT_CONST*d5 + d6 + ROT_CONST*d7;
    r1.im = d_26 + temp2;
    r2.im = d_57 - d1_3;
    //r3.im = -ROT_CONST*d1 + d2 - ROT_CONST*d3 + ROT_CONST*d5 - d6 + ROT_CONST*d7;
    r3.im = temp2 - d_26 ;
    r4.im = 0.0f;
    r5.im = -r3.im;
    r6.im = -r2.im;
    r7.im = -r1.im;

    input[index]            = r0;
    input[index+distance]   = complex_mul(tw1, r1);
    input[index+2*distance] = complex_mul(tw2, r2);
    input[index+3*distance] = complex_mul(tw3, r3);
    input[index+4*distance] = complex_mul_real(r4.re,tw4);
    input[index+5*distance] = complex_mul(tw5, r5);
    input[index+6*distance] = complex_mul(tw6, r6);
    input[index+7*distance] = complex_mul(tw7, r7);

}



void __attribute__ ((always_inline)) process_butterfly_radix8 (Complex_type* input, int twiddle_index, int index, int distance, Complex_type* twiddle_ptr)
{

    #if defined(MIXED_RADIX) && !defined(FULL_TWIDDLES)
    twiddle_index = twiddle_index * 4;
    #define MAX_TWIDDLES (FFT_LEN_RADIX2)
    #elif defined(FULL_TWIDDLES)
    #define MAX_TWIDDLES (FFT_LEN_RADIX8)
    #else
    #define MAX_TWIDDLES (FFT_LEN_RADIX8/2)
    #endif

    //int twiddle_index = 0;
    float d0         = input[index].re;
    float d1         = input[index+distance].re;
    float d2         = input[index+2*distance].re;
    float d3         = input[index+3*distance].re;
    float d4         = input[index+4*distance].re;
    float d5         = input[index+5*distance].re;
    float d6         = input[index+6*distance].re;
    float d7         = input[index+7*distance].re;

    float e0         = input[index].im;
    float e1         = input[index+distance].im;
    float e2         = input[index+2*distance].im;
    float e3         = input[index+3*distance].im;
    float e4         = input[index+4*distance].im;
    float e5         = input[index+5*distance].im;
    float e6         = input[index+6*distance].im;
    float e7         = input[index+7*distance].im;

    Complex_type tw1 = twiddle_ptr[twiddle_index*1];
    Complex_type tw2 = twiddle_ptr[twiddle_index*2];
    Complex_type tw3 = twiddle_ptr[twiddle_index*3];
    Complex_type tw4 = twiddle_ptr[twiddle_index*4];
    Complex_type tw5;
    Complex_type tw6;
    Complex_type tw7;

    #ifdef FULL_TWIDDLES
    tw5 = twiddle_ptr[twiddle_index*5];
    tw6 = twiddle_ptr[twiddle_index*6];
    tw7 = twiddle_ptr[twiddle_index*7];
    #else
    if(twiddle_index*5 >= MAX_TWIDDLES)
    {
      tw5 = twiddle_ptr[(twiddle_index*5)%(MAX_TWIDDLES)];
      tw5.re = -tw5.re;
      tw5.im = -tw5.im;
    }
    else
    {
      tw5 = twiddle_ptr[twiddle_index*5];
    }
    if(twiddle_index*6 >= MAX_TWIDDLES)
    {
      tw6 = twiddle_ptr[(twiddle_index*6)%(MAX_TWIDDLES)];
      tw6.re = -tw6.re;
      tw6.im = -tw6.im;
    }
    else
    {
      tw6 = twiddle_ptr[twiddle_index*6];
    }
    if(twiddle_index*7 >= MAX_TWIDDLES)
    {
      tw7 = twiddle_ptr[(twiddle_index*7)%(MAX_TWIDDLES)];
      tw7.re = -tw7.re;
      tw7.im = -tw7.im;
    }
    else
    {
      tw7 = twiddle_ptr[twiddle_index*7];
    }
    #endif // FULL_TWIDDLES


    Complex_type r0, r1, r2, r3, r4, r7, r6, r5 ;

    // Basic buttefly rotation
    /*

r0   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i
r1   1.0000 + 0.0000i   0.7071 - 0.7071i   0.0000 - 1.0000i  -0.7071 - 0.7071i  -1.0000 - 0.0000i  -0.7071 + 0.7071i   0.0000 + 1.0000i   0.7071 + 0.7071i
r2   1.0000 + 0.0000i   0.0000 - 1.0000i  -1.0000 - 0.0000i   0.0000 + 1.0000i   1.0000 + 0.0000i   0.0000 - 1.0000i   -1.0000 - 0.0000i  0.0000 + 1.0000i
r3   1.0000 + 0.0000i  -0.7071 - 0.7071i  -0.0000 + 1.0000i   0.7071 - 0.7071i  -1.0000 - 0.0000i   0.7071 + 0.7071i   0.0000 - 1.0000i  -0.7071 + 0.7071i
r4   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i
r5   1.0000 + 0.0000i  -0.7071 + 0.7071i   0.0000 - 1.0000i   0.7071 + 0.7071i  -1.0000 - 0.0000i   0.7071 - 0.7071i  -0.0000 + 1.0000i  -0.7071 - 0.7071i
r6   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i
r7   1.0000 + 0.0000i   0.7071 + 0.7071i  -0.0000 + 1.0000i  -0.7071 + 0.7071i  -1.0000 - 0.0000i  -0.7071 - 0.7071i   0.0000 - 1.0000i   0.7071 - 0.7071i
    */

    //Re(c1*c2) = c1.re*c2.re - c1.im*c2.im


    float d13 = d1 + d3;
    float d1_3 = d1 - d3;
    float e13 = e1 + e3;
    float e57 = e5 + e7;
    float e1_3 = e1 - e3;
    float e_57 = e7 - e5;
    float d57 = d5 + d7;
    float d_57 = d7 - d5;
    float temp1 = ROT_CONST*(d1_3+d_57);
    float temp1b = ROT_CONST*(e57 - e13);
    float temp2 = ROT_CONST*(d57-d13);
    float temp2b = ROT_CONST*(e1_3 + e_57);
    float d04   = d0 + d4;
    float d0_4  = d0 - d4;
    float d26  = d2 + d6;
    float d_26  = d6 - d2;
    float d0246 = d04 + d26;
    float d1357 = d13 + d57;
    float e0246 = e0 + e2 + e4 + e6;
    float e0_4  = e0 - e4;
    float e0_24_6 = e0 - e2 + e4 - e6;
    float e1357 = e13 + e57;
    float e_13_57 = e_57 - e1_3;
    float e2_6 = e2 - e6 ;
    float e_26 = e6 - e2 ;

    r0.re = d0246 + d1357;

    //r1.re = d0 + ROT_CONST*d1 - ROT_CONST*d3 - d4 - ROT_CONST*d5 + ROT_CONST*d7;
    r1.re = d0_4 + temp1;
    r7.re = r1.re + (e_26 + temp1b);
    r1.re = r1.re - (e_26 + temp1b);

    r2.re = d04 - d26;
    r6.re = r2.re + e_13_57;
    r2.re = r2.re - e_13_57;


    //r3.re = d0 - ROT_CONST*d1 + ROT_CONST*d3 - d4 + ROT_CONST*d5 - ROT_CONST*d7;
    r3.re = d0_4 - temp1;
    r5.re = r3.re + (e2_6 + temp1b);
    r3.re = r3.re - (e2_6 + temp1b);


    r4.re = d0246 - d1357;


    //Im(c1*c2) = c1.re*c2.im + c1.im*c2.re

    r0.im = e0246 + e1357;

    //r1.im = -ROT_CONST*d1 - d2 - ROT_CONST*d3 + ROT_CONST*d5 + d6 + ROT_CONST*d7;
    r1.im = d_26 + temp2;
    r7.im = -r1.im + ( e0_4 + temp2b);
    r1.im =  r1.im + ( e0_4 + temp2b);

    r2.im = d_57 - d1_3;
    r6.im = -r2.im + e0_24_6;
    r2.im =  r2.im + e0_24_6;

    //r3.im = -ROT_CONST*d1 + d2 - ROT_CONST*d3 + ROT_CONST*d5 - d6 + ROT_CONST*d7;
    r3.im =  temp2 - d_26;
    r5.im = -r3.im + (e0_4 - temp2b);
    r3.im =  r3.im + (e0_4 - temp2b);

    r4.im = e0246 - e1357;


    input[index]            = r0;
    input[index+distance]   = complex_mul(tw1, r1);
    input[index+2*distance] = complex_mul(tw2, r2);
    input[index+3*distance] = complex_mul(tw3, r3);
    input[index+4*distance] = complex_mul(tw4, r4);
    input[index+5*distance] = complex_mul(tw5, r5);
    input[index+6*distance] = complex_mul(tw6, r6);
    input[index+7*distance] = complex_mul(tw7, r7);

}

void __attribute__ ((always_inline)) process_butterfly_last_radix8 (Complex_type* input, Complex_type* output, int outindex )
{
    int index = 0;
    float d0         = input[index].re;
    float d1         = input[index+1].re;
    float d2         = input[index+2].re;
    float d3         = input[index+3].re;
    float d4         = input[index+4].re;
    float d5         = input[index+5].re;
    float d6         = input[index+6].re;
    float d7         = input[index+7].re;

    float e0         = input[index].im;
    float e1         = input[index+1].im;
    float e2         = input[index+2].im;
    float e3         = input[index+3].im;
    float e4         = input[index+4].im;
    float e5         = input[index+5].im;
    float e6         = input[index+6].im;
    float e7         = input[index+7].im;

    Complex_type r0, r1, r2, r3, r4, r7, r6, r5 ;

    /* twiddles are all 1s*/

    // Basic buttefly rotation
    /*

r0   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i   1.0000 + 0.0000i
r1   1.0000 + 0.0000i   0.7071 - 0.7071i   0.0000 - 1.0000i  -0.7071 - 0.7071i  -1.0000 - 0.0000i  -0.7071 + 0.7071i   0.0000 + 1.0000i   0.7071 + 0.7071i
r2   1.0000 + 0.0000i   0.0000 - 1.0000i  -1.0000 - 0.0000i   0.0000 + 1.0000i   1.0000 + 0.0000i   0.0000 - 1.0000i   -1.0000 - 0.0000i  0.0000 + 1.0000i
r3   1.0000 + 0.0000i  -0.7071 - 0.7071i  -0.0000 + 1.0000i   0.7071 - 0.7071i  -1.0000 - 0.0000i   0.7071 + 0.7071i   0.0000 - 1.0000i  -0.7071 + 0.7071i

r4   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i   1.0000 + 0.0000i  -1.0000 - 0.0000i
r5   1.0000 + 0.0000i  -0.7071 + 0.7071i   0.0000 - 1.0000i   0.7071 + 0.7071i  -1.0000 - 0.0000i   0.7071 - 0.7071i  -0.0000 + 1.0000i  -0.7071 - 0.7071i
r6   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i   1.0000 + 0.0000i   0.0000 + 1.0000i  -1.0000 - 0.0000i   0.0000 - 1.0000i
r7   1.0000 + 0.0000i   0.7071 + 0.7071i  -0.0000 + 1.0000i  -0.7071 + 0.7071i  -1.0000 - 0.0000i  -0.7071 - 0.7071i   0.0000 - 1.0000i   0.7071 - 0.7071i

    */

    //Re(c1*c2) = c1.re*c2.re - c1.im*c2.im

    float d13 = d1 + d3;
    float d1_3 = d1 - d3;
    float e13 = e1 + e3;
    float e57 = e5 + e7;
    float e1_3 = e1 - e3;
    float e_57 = e7 - e5;
    float d57 = d5 + d7;
    float d_57 = d7 - d5;
    float temp1 = ROT_CONST*(d1_3+d_57);
    float temp1b = ROT_CONST*(e57 - e13);
    float temp2 = ROT_CONST*(d57-d13);
    float temp2b = ROT_CONST*(e1_3 + e_57);
    float d04   = d0 + d4;
    float d0_4  = d0 - d4;
    float d26  = d2 + d6;
    float d_26  = d6 - d2;
    float d0246 = d04 + d26;
    float d1357 = d13 + d57;
    float e0246 = e0 + e2 + e4 + e6;
    float e0_4  = e0 - e4;
    float e0_24_6 = e0 - e2 + e4 - e6;
    float e1357 = e13 + e57;
    float e_13_57 = e_57 - e1_3;
    float e2_6 = e2 - e6 ;
    float e_26 = e6 - e2 ;

    r0.re = d0246 + d1357;

    //r1.re = d0 + ROT_CONST*d1 - ROT_CONST*d3 - d4 - ROT_CONST*d5 + ROT_CONST*d7;
    r1.re = d0_4 + temp1;
    r7.re = r1.re + (e_26 + temp1b);
    r1.re = r1.re - (e_26 + temp1b);

    r2.re = d04 - d26;
    r6.re = r2.re + e_13_57;
    r2.re = r2.re - e_13_57;


    //r3.re = d0 - ROT_CONST*d1 + ROT_CONST*d3 - d4 + ROT_CONST*d5 - ROT_CONST*d7;
    r3.re = d0_4 - temp1;
    r5.re = r3.re + (e2_6 + temp1b);
    r3.re = r3.re - (e2_6 + temp1b);


    r4.re = d0246 - d1357;


    //Im(c1*c2) = c1.re*c2.im + c1.im*c2.re

    r0.im = e0246 + e1357;

    //r1.im = -ROT_CONST*d1 - d2 - ROT_CONST*d3 + ROT_CONST*d5 + d6 + ROT_CONST*d7;
    r1.im = d_26 + temp2;
    r7.im = -r1.im + ( e0_4 + temp2b);
    r1.im =  r1.im + ( e0_4 + temp2b);

    r2.im = d_57 - d1_3;
    r6.im = -r2.im + e0_24_6;
    r2.im =  r2.im + e0_24_6;

    //r3.im = -ROT_CONST*d1 + d2 - ROT_CONST*d3 + ROT_CONST*d5 - d6 + ROT_CONST*d7;
    r3.im =  temp2 - d_26;
    r5.im = -r3.im + (e0_4 - temp2b);
    r3.im =  r3.im + (e0_4 - temp2b);

    r4.im = e0246 - e1357;

#ifndef SORT_OUTPUT
   /* In the Last step, twiddle factors are all 1 */
   #ifdef BITREV_LUT
   #ifdef MIXED_RADIX
   unsigned int index12 = *((unsigned int *)(&bit_rev_2_8_LUT[outindex]));
   unsigned int index34 = *((unsigned int *)(&bit_rev_2_8_LUT[outindex+2]));
   unsigned int index56 = *((unsigned int *)(&bit_rev_2_8_LUT[outindex+4]));
   unsigned int index78 = *((unsigned int *)(&bit_rev_2_8_LUT[outindex+6]));
   #else
   unsigned int index12 = *((unsigned int *)(&bit_rev_radix8_LUT[outindex]));
   unsigned int index34 = *((unsigned int *)(&bit_rev_radix8_LUT[outindex+2]));
   unsigned int index56 = *((unsigned int *)(&bit_rev_radix8_LUT[outindex+4]));
   unsigned int index78 = *((unsigned int *)(&bit_rev_radix8_LUT[outindex+6]));
   #endif
   unsigned int index1  = index12 & 0x0000FFFF;
   unsigned int index2  = index12 >> 16;
   unsigned int index3  = index34 & 0x0000FFFF;
   unsigned int index4  = index34 >> 16;
   unsigned int index5  = index56 & 0x0000FFFF;
   unsigned int index6  = index56 >> 16;
   unsigned int index7  = index78 & 0x0000FFFF;
   unsigned int index8  = index78 >> 16;
   output[index1] = r0;
   output[index2] = r1;
   output[index3] = r2;
   output[index4] = r3;
   output[index5] = r4;
   output[index6] = r5;
   output[index7] = r6;
   output[index8] = r7;
   #else
   #ifdef MIXED_RADIX
   output[bit_rev_2_8(outindex    )] = r0;
   output[bit_rev_2_8(outindex+1  )] = r1;
   output[bit_rev_2_8(outindex+2*1)] = r2;
   output[bit_rev_2_8(outindex+3*1)] = r3;
   output[bit_rev_2_8(outindex+4*1)] = r4;
   output[bit_rev_2_8(outindex+5*1)] = r5;
   output[bit_rev_2_8(outindex+6*1)] = r6;
   output[bit_rev_2_8(outindex+7*1)] = r7;
   #else
   output[bit_rev_radix8(outindex    )] = r0;
   output[bit_rev_radix8(outindex+1  )] = r1;
   output[bit_rev_radix8(outindex+2*1)] = r2;
   output[bit_rev_radix8(outindex+3*1)] = r3;
   output[bit_rev_radix8(outindex+4*1)] = r4;
   output[bit_rev_radix8(outindex+5*1)] = r5;
   output[bit_rev_radix8(outindex+6*1)] = r6;
   output[bit_rev_radix8(outindex+7*1)] = r7;
   #endif // MIXED_RADIX
   #endif // BITREV_LUT

#else
   output[outindex    ] = r0;
   output[outindex+1  ] = r1;
   output[outindex+2*1] = r2;
   output[outindex+3*1] = r3;
   output[outindex+4*1] = r4;
   output[outindex+5*1] = r5;
   output[outindex+6*1] = r6;
   output[outindex+7*1] = r7;
#endif

}

void __attribute__ ((always_inline)) fft_radix8 (Complex_type * Inp_signal, Complex_type * Out_signal)
{

  int k,j,stage, step, d, index;
  Complex_type * _in;
  Complex_type * _out;
  Complex_type  temp;
  int dist = FFT_LEN_RADIX8 >> 3;
  int nbutterfly = FFT_LEN_RADIX8 >> 3;
  int butt = 8;
  Complex_type * _in_ptr;
  Complex_type * _out_ptr;
  Complex_type * _tw_ptr;
  _in  = &(Inp_signal[0]);
  _out = &(Out_signal[0]);
  // first stage
  stage = 1;

  _in_ptr = _in;

  #ifdef FULL_TWIDDLES
  _tw_ptr = twiddle_factors8;
  #else
  _tw_ptr = twiddle_factors;
  #endif

  for(j=0;j<nbutterfly;j++)
  {
    #ifdef MIXED_RADIX
    process_butterfly_radix8(_in_ptr, j, 0, dist, _tw_ptr);
    #else
    process_butterfly_real_radix8(_in_ptr, j, dist, _tw_ptr);
    #endif
    //_tw_ptr+=8;
    _in_ptr++;
  } //j


  stage = stage + 1;
  dist  = dist >> 3;

  while(dist > 1)
  {
    step = dist << 3;
    for(j=0;j<butt;j++)
    {
        _in_ptr  = _in;
        for(d = 0; d < dist; d++){
            process_butterfly_radix8(_in_ptr,  d*butt, j*step, dist, _tw_ptr);
            _in_ptr++;
            //_tw_ptr+=8;
         } //d

    } //j
    stage = stage + 1;
    dist  = dist >> 3;
    butt = butt << 3;
  }

  _in_ptr  = _in;


  // last stage
  index=0;
  for(j=0;j<FFT_LEN_RADIX8/8; j++) //FFT_LEN_RADIX8/8 o FFT_LEN_RADIX8 >> 3
  {
    process_butterfly_last_radix8(_in_ptr, _out, index);
    _in_ptr +=8;
    index   +=8;
   } //j


   #ifdef SORT_OUTPUT
   //order values
   for(j=0; j<FFT_LEN_RADIX8; j+=8)
   {
     #ifdef BITREV_LUT
     #ifdef MIXED_RADIX
     unsigned int index12 = *((unsigned int *)(&bit_rev_2_8_LUT[j]));
     unsigned int index34 = *((unsigned int *)(&bit_rev_2_8_LUT[j+2]));
     unsigned int index56 = *((unsigned int *)(&bit_rev_2_8_LUT[j+4]));
     unsigned int index78 = *((unsigned int *)(&bit_rev_2_8_LUT[j+6]));
     #else
     unsigned int index12 = *((unsigned int *)(&bit_rev_radix8_LUT[j]));
     unsigned int index34 = *((unsigned int *)(&bit_rev_radix8_LUT[j+2]));
     unsigned int index56 = *((unsigned int *)(&bit_rev_radix8_LUT[j+4]));
     unsigned int index78 = *((unsigned int *)(&bit_rev_radix8_LUT[j+6]));
     #endif // MIXED_RADIX
     unsigned int index1  = index12 & 0x0000FFFF;
     unsigned int index2  = index12 >> 16;
     unsigned int index3  = index34 & 0x0000FFFF;
     unsigned int index4  = index34 >> 16;
     unsigned int index5  = index56 & 0x0000FFFF;
     unsigned int index6  = index56 >> 16;
     unsigned int index7  = index78 & 0x0000FFFF;
     unsigned int index8  = index78 >> 16;
     #else
     #ifdef MIXED_RADIX
     int index1 = bit_rev_2_8(j);
     int index2 = bit_rev_2_8(j+1);
     int index3 = bit_rev_2_8(j+2);
     int index4 = bit_rev_2_8(j+3);
     int index5 = bit_rev_2_8(j+4);
     int index6 = bit_rev_2_8(j+5);
     int index7 = bit_rev_2_8(j+6);
     int index8 = bit_rev_2_8(j+7);
     #else
     int index1 = bit_rev_radix8(j);
     int index2 = bit_rev_radix8(j+1);
     int index3 = bit_rev_radix8(j+2);
     int index4 = bit_rev_radix8(j+3);
     int index5 = bit_rev_radix8(j+4);
     int index6 = bit_rev_radix8(j+5);
     int index7 = bit_rev_radix8(j+6);
     int index8 = bit_rev_radix8(j+7);
     #endif // MIXED_RADIX
     #endif // BITREV_LUT

     if(index1 > j)
     {
         temp         = _out[j];
         _out[j]      = _out[index1];
         _out[index1] = temp;
     }
     if(index2 > j+1)
     {
         temp         = _out[j+1];
         _out[j+1]    = _out[index2];
         _out[index2] = temp;
     }
     if(index3 > j+2)
     {
         temp         = _out[j+2];
         _out[j+2]      = _out[index3];
         _out[index3] = temp;
     }
     if(index4 > j+3)
     {
         temp         = _out[j+3];
         _out[j+3]    = _out[index4];
         _out[index4] = temp;
     }
     if(index5 > j+4)
     {
         temp         = _out[j+4];
         _out[j+4]    = _out[index5];
         _out[index5] = temp;
     }
     if(index6 > j+5)
     {
         temp         = _out[j+5];
         _out[j+5]    = _out[index6];
         _out[index6] = temp;
     }
     if(index7 > j+6)
     {
         temp         = _out[j+6];
         _out[j+6]    = _out[index7];
         _out[index7] = temp;
     }
     if(index8 > j+7)
     {
         temp         = _out[j+7];
         _out[j+7]    = _out[index8];
         _out[index8] = temp;
     }
   }
   #endif

}

//parallel
void __attribute__ ((always_inline)) par_fft_radix8 (Complex_type * Inp_signal, Complex_type * Out_signal)
{

  int k,j,stage, step, d, index;
  Complex_type * _in;
  Complex_type * _out;
  Complex_type  temp;
  int dist = FFT_LEN_RADIX8 >> 3;
  int nbutterfly = FFT_LEN_RADIX8 >> 3;
  int butt = 8;
  int core_id = __core_ID();
  Complex_type * _in_ptr;
  Complex_type * _out_ptr;
  Complex_type * _tw_ptr;

  _in  = &(Inp_signal[0]);
  _out = &(Out_signal[0]);

  // first stage
  _in_ptr = &_in[core_id];
  #ifdef FULL_TWIDDLES
  _tw_ptr = twiddle_factors8;
  #else
  _tw_ptr = twiddle_factors;
  #endif


  stage = 1;

  for(j=0; j < nbutterfly/NUM_CORES; j++)
  {
    #ifdef MIXED_RADIX
    process_butterfly_radix8(_in_ptr, j*NUM_CORES+core_id, 0, dist, _tw_ptr);
    #else
    process_butterfly_real_radix8(_in_ptr,  j*NUM_CORES+core_id, dist, _tw_ptr);
    #endif
    //_tw_ptr+=8*NUM_CORES;
    _in_ptr+=NUM_CORES;
  } //j

  stage = stage + 1;
  dist  = dist >> 3;

  while(dist >= NUM_CORES)
  {
    CLUSTER_SynchBarrier();
    step = dist << 3;
    for(j=0;j<butt;j++)
    {
        _in_ptr = &_in[core_id];
        for(d = 0; d < dist/NUM_CORES; d++){
            //printf("DIST = %d BUTT = %d (%d, %d)\n", dist, butt, (d*NUM_CORES+core_id)*butt, j*step);
            process_butterfly_radix8(_in_ptr, (d*NUM_CORES+core_id)*butt, j*step, dist, _tw_ptr);
            //_tw_ptr+=8*NUM_CORES;
            _in_ptr+=NUM_CORES;

         } //d
    } //j
    stage = stage + 1;
    dist  = dist >> 3;
    butt = butt << 3;
  }

  while(dist > 1)
  {
    CLUSTER_SynchBarrier();
    step = dist << 3;
    for(j=0;j<butt/NUM_CORES;j++)
    {
        _in_ptr = _in;
        for(d = 0; d < dist; d++){
            process_butterfly_radix8(_in_ptr, d*butt, (j*NUM_CORES+core_id)*step, dist, _tw_ptr);
            _tw_ptr+=8*NUM_CORES;
            _in_ptr+=NUM_CORES;
         } //d
    } //j
    stage = stage + 1;
    dist  = dist >> 3;
    butt = butt << 3;
  }


  CLUSTER_SynchBarrier();

  // last stage
  _in_ptr  = &_in[8*core_id];
  index    = 8*core_id;
  //N must be at least 64 for multicore
  for(j=0;j<FFT_LEN_RADIX8/(8*NUM_CORES);j++)
  {
    process_butterfly_last_radix8(_in_ptr,_out,index);
    _in_ptr +=8*NUM_CORES;
    index   +=8*NUM_CORES;
   } //j


 #ifdef SORT_OUTPUT
 CLUSTER_SynchBarrier();

  //order values
  for(j = 8*core_id; j < FFT_LEN_RADIX8; j+=NUM_CORES*8)
  {

    #ifdef BITREV_LUT
    #ifdef MIXED_RADIX
    unsigned int index12 = *((unsigned int *)(&bit_rev_2_8_LUT[j]));
    unsigned int index34 = *((unsigned int *)(&bit_rev_2_8_LUT[j+2]));
    unsigned int index56 = *((unsigned int *)(&bit_rev_2_8_LUT[j+4]));
    unsigned int index78 = *((unsigned int *)(&bit_rev_2_8_LUT[j+6]));
    #else
    unsigned int index12 = *((unsigned int *)(&bit_rev_radix8_LUT[j]));
    unsigned int index34 = *((unsigned int *)(&bit_rev_radix8_LUT[j+2]));
    unsigned int index56 = *((unsigned int *)(&bit_rev_radix8_LUT[j+4]));
    unsigned int index78 = *((unsigned int *)(&bit_rev_radix8_LUT[j+6]));
    #endif // MIXED_RADIX
    unsigned int index1  = index12 & 0x0000FFFF;
    unsigned int index2  = index12 >> 16;
    unsigned int index3  = index34 & 0x0000FFFF;
    unsigned int index4  = index34 >> 16;
    unsigned int index5  = index56 & 0x0000FFFF;
    unsigned int index6  = index56 >> 16;
    unsigned int index7  = index78 & 0x0000FFFF;
    unsigned int index8  = index78 >> 16;
    #else
    #ifdef MIXED_RADIX
    int index1 = bit_rev_2_8(j);
    int index2 = bit_rev_2_8(j+1);
    int index3 = bit_rev_2_8(j+2);
    int index4 = bit_rev_2_8(j+3);
    int index5 = bit_rev_2_8(j+4);
    int index6 = bit_rev_2_8(j+5);
    int index7 = bit_rev_2_8(j+6);
    int index8 = bit_rev_2_8(j+7);
    #else
    int index1 = bit_rev_radix8(j);
    int index2 = bit_rev_radix8(j+1);
    int index3 = bit_rev_radix8(j+2);
    int index4 = bit_rev_radix8(j+3);
    int index5 = bit_rev_radix8(j+4);
    int index6 = bit_rev_radix8(j+5);
    int index7 = bit_rev_radix8(j+6);
    int index8 = bit_rev_radix8(j+7);
    #endif // MIXED_RADIX
    #endif // BITREV_LUT

    if(index1 > j)
    {
        temp         = _out[j];
        _out[j]      = _out[index1];
        _out[index1] = temp;
    }
    if(index2 > j+1)
    {
        temp         = _out[j+1];
        _out[j+1]    = _out[index2];
        _out[index2] = temp;
    }
    if(index3 > j+2)
    {
        temp         = _out[j+2];
        _out[j+2]      = _out[index3];
        _out[index3] = temp;
    }
    if(index4 > j+3)
    {
        temp         = _out[j+3];
        _out[j+3]    = _out[index4];
        _out[index4] = temp;
    }
    if(index5 > j+4)
    {
        temp         = _out[j+4];
        _out[j+4]    = _out[index5];
        _out[index5] = temp;
    }
    if(index6 > j+5)
    {
        temp         = _out[j+5];
        _out[j+5]    = _out[index6];
        _out[index6] = temp;
    }
    if(index7 > j+6)
    {
        temp         = _out[j+6];
        _out[j+6]    = _out[index7];
        _out[index7] = temp;
    }
    if(index8 > j+7)
    {
        temp         = _out[j+7];
        _out[j+7]    = _out[index8];
        _out[index8] = temp;
    }
  }
  #endif

  CLUSTER_SynchBarrier(); //forse va prima dell'#endif
}
