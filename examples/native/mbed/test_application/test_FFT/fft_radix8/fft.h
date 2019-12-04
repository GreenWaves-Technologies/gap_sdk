#ifndef FFT_H
#define FFT_H

#include "gap_common.h"
// FEATURE_CLUSTER
#include "gap_cluster.h"
#include "gap_dmamchan.h"
#include <stdlib.h>

#define LOG2_FFT_LEN_RADIX8   9

#if (LOG2_FFT_LEN_RADIX8 % 3 != 0)
#pragma GCC error "For this input size you cannot use the radix-8 algorithm"
#endif

#define FFT_LEN_RADIX8        (1 << LOG2_FFT_LEN_RADIX8)

#define FULL_TWIDDLES

#define SORT_OUTPUT
#define BITREV_LUT

#define PARALLEL

#define STATS
// #define BOARD

// #define PRINT_RESULTS


struct Complex_type_str {
  float re;
  float im;
};
typedef struct Complex_type_str Complex_type;

void fft_radix8     (Complex_type * Inp_signal, Complex_type * Out_signal);
void par_fft_radix8 (Complex_type * Inp_signal, Complex_type * Out_signal);

#endif
