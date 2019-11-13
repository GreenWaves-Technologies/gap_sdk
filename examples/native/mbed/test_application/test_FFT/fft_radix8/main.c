#include <stdio.h>
#include "print_float.h"
#include "fft.h"
#include "data_signal.h"

#ifdef FULL_TWIDDLES
extern Complex_type twiddle_factors8[FFT_LEN_RADIX8];
#else
extern Complex_type twiddle_factors[FFT_LEN_RADIX8/2];
#endif
extern short bit_rev_radix8_LUT[FFT_LEN_RADIX8];
int bit_rev_radix8(int);
void compute_twiddles();


void main_fn()
{
  int i;
  //INIT_STATS();

  if(__core_ID()==0) {
    #ifdef FULL_TWIDDLES
    compute_full_twiddles();
    #else
    compute_twiddles();
    #endif

    #ifdef BITREV_LUT
    for (i=0; i<FFT_LEN_RADIX8;i++)
      bit_rev_radix8_LUT[i] = bit_rev_radix8(i);
    #endif

    #ifndef SORT_OUTPUT
    for(i=0;i<FFT_LEN_RADIX8;i++)
      Buffer_Signal_Out[i] = (Complex_type){0.0f, 0.0f};
    #endif
  }

  #ifdef SORT_OUTPUT
  Complex_type * Buffer_Signal_Out = Input_Signal;
  #endif

  CLUSTER_SynchBarrier();

  /* ENTER_LOOP_STATS(); */
  /* START_STATS(); */
  #ifdef PARALLEL
  par_fft_radix8(Input_Signal, Buffer_Signal_Out);
  #else
  fft_radix8(Input_Signal, Buffer_Signal_Out);
  #endif
  /* STOP_STATS(); */
  /* EXIT_LOOP_STATS(); */

  CLUSTER_SynchBarrier();

  #ifdef PRINT_RESULTS
  if(__core_ID()==0) {
    float real_acc = 0;
    float imag_acc = 0;
    for(i=0;i<FFT_LEN_RADIX8;i++)
    {
      printf("Output_Signal[%d] = (",i);
      printFloat(Buffer_Signal_Out[i].re);
      printf(", ");
      printFloat(Buffer_Signal_Out[i].im);
      printf(")\n");
      real_acc+=Buffer_Signal_Out[i].re;
      imag_acc+=Buffer_Signal_Out[i].im;
    }
    printf("Real Acc = ");
    printFloat(real_acc);
    printf("\n");
    printf("Imag Acc = ");
    printFloat(imag_acc);
    printf("\n");
  }
  CLUSTER_SynchBarrier();
  #endif

}

static void cluster_entry(void *arg)
{
    CLUSTER_CoresFork(main_fn, arg);
}

int main()
{
    /* Cluster Start - power on */
    CLUSTER_Start(0, NUM_CORES, 0);

    /* FC send a task to cluster */
    CLUSTER_SendTask(0, cluster_entry, 0, 0);

    /* Cluster Stop - power down */
    CLUSTER_Stop(0);

    return 0;
}
