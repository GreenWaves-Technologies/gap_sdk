#ifndef TWIDDLE_FACT_H
#define TWIDDLE_FACT_H

#define DATA_LOCATION __attribute__((section(".data_l1")))

#ifdef FULL_TWIDDLES
DATA_LOCATION Complex_type twiddle_factors8[FFT_LEN_RADIX8] = {
#else
DATA_LOCATION Complex_type twiddle_factors[FFT_LEN_RADIX8/2] = {
#endif
};
#endif
