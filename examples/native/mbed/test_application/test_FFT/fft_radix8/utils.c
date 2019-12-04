#include <math.h>
#include "fft.h"

#ifdef FULL_TWIDDLES
extern Complex_type twiddle_factors8[FFT_LEN_RADIX8];
#elif !defined(MIXED_RADIX)
extern Complex_type twiddle_factors[FFT_LEN_RADIX8/2];
#endif

#ifdef FULL_TWIDDLES

void compute_full_twiddles()
{
    int i;
    float Theta = -4*(2*M_PI)/(FFT_LEN_RADIX8*4); //solo i pari cioè FFT_LEN_RADIX2/4 => *4
    for (i=0; i<FFT_LEN_RADIX8; i++)
    {
            float Phi = Theta*i;
            twiddle_factors8[i].re = cosf(Phi);
            twiddle_factors8[i].im = sinf(Phi);
    }
}

#elif !defined(MIXED_RADIX)

void compute_twiddles()
{
    int i;
    float Theta = -4*(2*M_PI)/(FFT_LEN_RADIX8*4); //solo i pari cioè FFT_LEN_RADIX2/4 => *4
    for (i=0; i<FFT_LEN_RADIX8/2; i++)
    {
            float Phi = Theta*i;
            twiddle_factors[i].re = cosf(Phi);
            twiddle_factors[i].im = sinf(Phi);
    }
}

#endif
