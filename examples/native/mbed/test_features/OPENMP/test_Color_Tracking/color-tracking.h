#ifndef __COLOR_TRACKING_H__
#define __COLOR_TRACKING_H__

#include <stdint.h>
#include "omp.h"

#define max(x,y)    ((x > y)? x : y)
#define min(x,y)    ((x < y)? x : y)

#define IMG_DATATYPE uint8_t

/**
 * Color Scale Coversion
 */
static inline void
colorScaleConversion(IMG_DATATYPE *input,
                     IMG_DATATYPE *output,
                     uint32_t size)
{
    uint32_t i;

    #pragma omp for
    for (i = 0; i < size; i+=3)
    {
        /* FIRST STEP: Convert to [0..1] range */
        int32_t red   = input[i];
        int32_t green = input[i+1];
        int32_t blue  = input[i+2];

        /* SECOND STEP: Determine H, S, V coefficients */
        int32_t mx = max (max (red, green), blue);
        int32_t mn = min (min (red, green), blue);
        int32_t mm = mx - mn;
        int32_t V, S, H;

        V = mx;
        if (V == 0)
            H = S = 0;
        else
        {
            S = 255*(mm)/V;
            if(S == 0)
                H = 0;
            else
            {
                if (mx == red)
                    H = 0 + 30*(green - blue)/mm;
                else if (mx == green)
                    H = 60 + 30*(blue - red)/mm;
                else
                    H = 120 + 30*(red - green)/mm;
            }
            if(H < 0)
                H += 180;
        }

        output[i]   = (IMG_DATATYPE) H;
        output[i+1] = (IMG_DATATYPE) S;
        output[i+2] = (IMG_DATATYPE) V;
    }

    return;
}

/**
 * cvThreshold - Color Based Threasholding
 */
static inline void
threshold (IMG_DATATYPE *input,
           IMG_DATATYPE *output,
           uint32_t size)
{
    //TUNE YELLOW RGB = 255, 255, 0 HSV 30 255 255
    int32_t lb1 = 25;
    int32_t ub1 = 45;
    int32_t lb2 = 100;
    int32_t ub2 = 255;
    int32_t lb3 = 100;
    int32_t ub3 = 255;

    uint32_t i;

    #pragma omp for
    for (i = 0U; i < size; ++i)
    {
        output[i] =  ((input[i*3] >= lb1) && (input[i*3] <= ub1) &&
        (input[i*3 + 1] >= lb2) && (input[i*3 + 1] <= ub2) &&
        (input[i*3 + 2] >= lb3) && (input[i*3 + 2] <= ub3)) ? 255 : 0;
    }

    return;
}

/**
 * cvMoments - Center of Gravity Computation based on Moments
 */
static inline void
computeMoments (IMG_DATATYPE *input,
                uint32_t *moments,
                uint32_t Y,
                uint32_t width,
                uint32_t size)
{
    uint32_t i;
    uint32_t moments00 = 0U, moments01 = 0U, moments10 = 0U;
    int32_t _x = 0, _y = Y;

    #pragma omp for
    for (i = 0; i < size; ++i)
    {
        if(input[i])
        {
            while(i > (_x + width))
            {
                _x += width;
                _y ++;
            }

            moments00++;            //M00
            moments01 += (_y);      //M01
            moments10 += (i - _x);  //M10
        }
    }

    #pragma omp critical
    {
        moments[0] += moments00;
        moments[1] += moments01;
        moments[2] += moments10;
    }

    return;
}

#if 0
/**
 c *vAdd - Image add based on channel-to-channel sum
 */
static inline void __cvAdd(IMG_DATATYPE *input_1, IMG_DATATYPE *input_2, IMG_DATATYPE *output, uint32_t size)
{
    int i;

    #pragma omp for
    for(i = 0; i < size; ++i)
    {
        output[i] = input_1[i] + input_2[i];

        if (output[i] >= 0xFF)
            output[i] = 0xFF;
    }
}
#endif

#endif
