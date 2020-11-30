#include "Gap.h"
#include "MfccBasicKernels.h"
#include "FFTLib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef abs
#define abs(a)  (((a)<0) ? (-(a)) : (a))
#endif
#ifndef Min
#define Min(x, y)       (((x)<(y))?(x):(y))
#endif

// used in computelog -> assume output of log in Q16
//#define GAPLOG2   ((int) floor(log(2)*(float)(1<<QN)+0.5))
//#define c1_3      ((int) floor((1.0/3.0) * (float)(1<<QN) +0.5))
//#define LOG_NORM  ((int) floor(log((float)(1<<(QN-QN_LOC))+0.5))
#define QNN         16
#define QN_LOC      34
#define GAPLOG2     (0xb172)
#define c1_3        (0x5555)
#define LOG_NORM    (0xc7a06)


static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)
{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        NCore = gap_ncore();
        Log2Core = gap_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

void get_max(MFCC_PreEmphasis_T *Arg)
{
  short int * __restrict__  Frame =   Arg->Frame;
  int FrameSize = Arg->FrameSize;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);
  int *maxin = Arg->maxin;
  int shift;

  maxin[CoreId]=0;
  if (CoreId==0) maxin[0] = abs(Arg->Prev);
  
  for (int j=First;j<Last;j++) 
    if(abs((int)Frame[j])>maxin[CoreId])
      maxin[CoreId]=abs((int)Frame[j]);
  gap_waitbarrier(0);

  if (CoreId==0) {
    int max=0;
    for(int j=0; j<gap_ncore();j++)
      if (maxin[j]>max) max=maxin[j];
  
    shift = gap_fl1(max);
    // set to Q13 for FFT 
    if (shift<=13) shift = (13-shift); else shift=0;
    *(Arg->Shift)=shift;
  }
  gap_waitbarrier(0);
}

void MFCC_PreEmphasis(MFCC_PreEmphasis_T *Arg)
{
  short int * __restrict__ Frame = Arg->Frame;
  short int * __restrict__ Out = Arg->Out;
  int FrameSize = Arg->FrameSize;
  short int PreempFactor = Arg->PreempFactor;
  short int S;
  short int Sprev;
  
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);


  // Y[n]=X[n]−0.97⋅X[n−1]
  // To gain in precision:
  //    Y[n] = X[n]<<shift - Q15(0.97)*X[n-1]<<shift
  // X[n] -> Q15
  // Y[n] -> Q15+shift
  get_max(Arg);
  int shift = *(Arg->Shift);
  S = CoreId? Frame[First-1]<<shift : Arg->Prev<<shift;

  for (int i=First; i<Last; i++) {
    Sprev = Frame[i]<<shift;
    Out[i] = Sprev - gap_mulsRN(PreempFactor, S, 15);
    S = Sprev;
  }

  gap_waitbarrier(0);

  if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nShift = %d\n",*Arg->Shift);
    printf("out_preemph_c = np.array([\n\t\t");
    for(int i=0; i<FrameSize ; i++) printf("%d, ", Out[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void MFCC_WindowedFrame(MFCC_WF_T *Arg)
{
  short int *__restrict__ Frame     = Arg->Frame;
  v2s *__restrict__ OutFrame  = (v2s*) Arg->OutFrame;
  short int *__restrict__ Window    = Arg->Window;
  //int                     FFT_Dyn   = Arg->FFT_Input_Dyn;
  int                     FrameSize = Arg->FrameSize;
  int                     FFT_Dim   = Arg->FFT_Dim;

  int i;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  for (i=First; i<Last; i++) {
    /* Frame is Q15+shift and Window is Q15 */
    OutFrame[i] = (v2s) {gap_mulsRN(Frame[i], Window[i], 15), 0};
  }
  // 0 padding
  if (CoreId==0) for (i=FrameSize; i<FFT_Dim;i++) OutFrame[i] = (v2s) {0,0};
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nN_FFT = %d\n",FFT_Dim);
    printf("out_window_c = np.array([\n\t\t");
    for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i][0]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

// the output is an complex int table
void MFCC_WindowedFrame_int(MFCC_WFINT_T *Arg)
{
  short int *__restrict__ Frame    = Arg->Frame;
  int       *__restrict__ OutFrame = (int *) Arg->OutFrame;
  short int *__restrict__ Window   = Arg->Window;
  int FrameSize = Arg->FrameSize;
  int FFT_Dim   = Arg->FFT_Dim;
  unsigned int i;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  for (i=First; i<Last; i++) {
    /* Frame and Window are Q15, we want OutFrame with precision FFT2_SAMPLE_DYN */
    OutFrame[2*i]   = (int) gap_mulsRN(Frame[i], Window[i], 15);
    OutFrame[2*i+1] = (int) 0;
  }

  // 0 padding
  if (CoreId==0) for (i=FrameSize; i<(unsigned int)FFT_Dim;i++) {
    OutFrame[2*i]   = 0;
    OutFrame[2*i+1] = 0;
  } 
  gap_waitbarrier(0);

  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_window_c = np.array([\n\t");
      for(int i=0; i<FFT_Dim ; i++) {
        if (OutFrame[2*i+1]>=0) printf("%d+%dj, ", OutFrame[2*i],OutFrame[2*i+1]);
        else                    printf("%d%dj, ",  OutFrame[2*i],OutFrame[2*i+1]);
      }
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

void MFCC_PowerV2S(MFCC_EP_T *Arg)
{
  int *__restrict__ FrameIn   = (int *) Arg->FrameIn;
  int *__restrict__ Power_int = (int *) Arg->FrameOut;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    int P = gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]);
    Power_int[i] = P;
  }

  gap_waitbarrier(0);

#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nout_power = np.array([\n\t");
    for (i=0; i<N; i++) printf("%d, ", Power_int[i]);
    printf("])\n");
  }
#endif
}

void MFCC_Power(MFCC_EP_T *Arg)
{
  int *__restrict__ FrameIn   = (int *) Arg->FrameIn;
  int *__restrict__ Power_int = (int *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;
  

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    int P = (FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16);
    Power_int[i] = P; // __MULSRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_power = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Power_int[i]);
      printf("])\nshift_power = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", shift_fft[i]);
      printf("])\n");
    }
  #endif
}

int SQRT(int x, int Qbits){
  int ONE = 1<<Qbits;
  int a_over_2 = (x - ONE) >> 1;
  int a2_over_8 = (a_over_2 * a_over_2) >> (Qbits + 2);
  int a3_over_16 = (a2_over_8 * a_over_2) >> (Qbits);
  return ONE + a_over_2 - a2_over_8 + a3_over_16;
}

void MFCC_Abs(MFCC_EP_T *Arg)
{
  int *__restrict__ FrameIn     = (int *) Arg->FrameIn;
  int *__restrict__ Spectrogram = (int *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    int S = ((int) round(sqrt(gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]))));
    Spectrogram[i] = S;
  }

  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_spectrogram = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Spectrogram[i]);
      printf("])");
    }
  #endif
}


void MFCC_Abs_BFF(MFCC_EP_BFF_T *Arg)
{
  int *__restrict__ FrameIn     = (int *) Arg->FrameIn;
  int *__restrict__ Spectrogram = (int *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  int               Shift = *Arg->Shift;

  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    int S = ( (int) round(sqrt(( double) ((FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16)))) );
    Spectrogram[i] = S;
  }

  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_spectrogram = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Spectrogram[i]);
      printf("])\nshift_abs = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", shift_fft[i]);
      printf("])\n");
    }
  #endif
}


void MFCC_ComputeMFCC(MFCC_MF_T *Arg)
{
  unsigned int i, j, k;

  unsigned int *__restrict__ FramePower      = Arg->FramePower;
  unsigned int *__restrict__ MFCC            = Arg->MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank = Arg->MFCC_FilterBank;
  short int    *__restrict__ MFCC_Coeffs     = Arg->MFCC_Coeffs;
  short int                  mfcc_bank_cnt   = Arg->MFCC_bank_cnt;
  short int                  MFCC_Coeff_Dyn  = Arg->MFCC_Coeff_dyn;
  
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(mfcc_bank_cnt);
  First = CoreId*Chunk; Last = Min(First + Chunk, mfcc_bank_cnt);
  
  /*
    MFCC_Coeffs is in MFCC_Coeff_Dyn
  */

  for (i=First; i<(unsigned int)Last; i++) {
    unsigned int Coeff = 0;
    int Base = MFCC_FilterBank[i].Base;
    int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);

    // Before computing the coeff multiplication check that max(FramePower) + 10 (QCoeff)
    // do not exceed 31, if so right shift (lose precision) the coeff
    int maxin=0;
    for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
      if (FramePower[j]>(unsigned int)maxin) maxin =FramePower[j];
    }

    short int shift ;
    short int shift0 = gap_fl1(maxin);
    // check multiply overflow condition FramePower * Q6.10 <= Q31
    if ((shift0+MFCC_Coeff_Dyn)>31) shift = shift0-(31-MFCC_Coeff_Dyn); else shift = 0;

    for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++){
      // very easy to overflow here - maybe decrease the coeff dyn
      Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>shift);
    }
    Coeff = Coeff << shift;
    MFCC[i] = Coeff;
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_melfilterbank = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%u, ", MFCC[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

/* compatible with block floating point fft */
void MFCC_ComputeMFCC_BFF(MFCC_MF_New_T *Arg)
{
  unsigned int i, j, k;
  unsigned int *__restrict__ FramePower      = Arg->FramePower;
  unsigned int *__restrict__ MFCC            = Arg->MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank = Arg->MFCC_FilterBank;
  short int    *__restrict__ MFCC_Coeffs     = Arg->MFCC_Coeffs;
  short int                  mfcc_bank_cnt   = Arg->MFCC_bank_cnt;
  signed char  *             shift_BF        = Arg->shift_BF;
  signed char  *             shift_pow       = Arg->shift_pow;
  
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(mfcc_bank_cnt);
  First = CoreId*Chunk; Last = Min(First + Chunk, mfcc_bank_cnt);

  #ifdef USE_ABS
    #define MUL_EXP 1
  #else
    #define MUL_EXP 2
  #endif
  
  int min_shift;    
  short int shift;     
  for (i=First; i<Last; i++) {
    unsigned int Coeff = 0;
    int Base = MFCC_FilterBank[i].Base;
    int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);
    int maxin=0;
    min_shift = 0x7fff;

    for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
      if (shift_pow[j] < min_shift) min_shift = shift_pow[j];
    }

    // align the block scaling on the min , compute the max value in the block
    for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
      int TMP = FramePower[j] >> ( MUL_EXP * (shift_pow[j] - min_shift));
      if (TMP > (unsigned int) maxin) maxin = TMP;
    }

    // compute shift right to apply on the block to prevent overflow
    shift  = 0 ;
    short int shift0 = gap_fl1(maxin);

    // check multiply overflow conditionQ18.14 * Q6.10 => Q8.24
    if ((shift0 + 14) > 31) shift = shift0 - 17; else shift = 0;

    for (k=0, j=MFCC_FilterBank[i].Start; j<(unsigned int)(MFCC_FilterBank[i].Start+Count); j++, k++) {
      Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>(shift + MUL_EXP * (shift_pow[j] - min_shift)));
      //Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>(shift));
    }
    // update the shift left  value on the block (to compensate)
    shift_BF[i] = shift - MUL_EXP * min_shift;
    MFCC[i] = Coeff;
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_melfilterbank = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%u, ", MFCC[i]);
      printf("])\nshift_BF = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%d, ", shift_BF[i]);
      printf("])\nshift_pow = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%u, ", shift_pow[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

int MFCC_Logfp(unsigned int a)
{
    int iLog2N  = gap_fl1(a);
    int valint = iLog2N - QNN;

    if (valint>=0) {
      if ((a>>valint)&(1<<(QNN-1))) valint++;
      a  = a >> valint;
    }
    else {
        a = a << (-valint);
    }
	
    int ar = (1<<QNN) - a; // (1-ar = a):  log(1-ar) ~ -ar + ar*ar/2 - ar*ar*ar/3
    int ar2 = ((ar>>(QNN>>1)) * (ar>>(QNN>>1))) >> (1);
    int ar3 = ((ar2) * (ar)) >> (QNN-1);
    ar3 = ((ar3) * (c1_3)) >> QNN;
  
    int loga =  (valint*GAPLOG2) - ar + ar2 - ar3;
    return (int) loga;
}


void MFCC_ComputeLog( MFCC_Log_T *Arg) {

  int i;
  int         size      = Arg->FrameSize;
  int         *frameIn  = (int *) Arg->FrameIn;
  int         Shift     = *Arg->Shift;
  int         fft_bits  = Arg->fft_bits;
  int         mfcc_coeff_dyn = Arg->MFCC_Coeff_dyn;
  signed char *shift_BF = Arg->shift_BF;

  // log is natural log in C
  int TMP;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(size);
  First = CoreId*Chunk; Last = Min(First + Chunk, size);

  for (i=First;i<Last;i++){
    TMP = MFCC_Logfp(frameIn[i]);

    #ifndef USE_ABS
    #ifdef HIGH_PREC_FFT
      frameIn[i] =  TMP - (mfcc_coeff_dyn-2-shift_BF[i]+2*Shift - QNN) * GAPLOG2; //POWER HIGH_PREC
    #else
      frameIn[i] =  TMP - (2*(16-fft_bits+Shift)+mfcc_coeff_dyn - QNN) * GAPLOG2; //POWER
    #endif
    #else
    #ifdef HIGH_PREC_FFT
      frameIn[i] =  TMP - (mfcc_coeff_dyn-1-shift_BF[i]+Shift - QNN) * GAPLOG2; //Abs HIGH_PREC
    #else
      frameIn[i] =  TMP - ((16-fft_bits+Shift)+mfcc_coeff_dyn - QNN) * GAPLOG2; //Abs
    #endif
    #endif
  }
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nout_log = np.array([\n");
    for(i=0;i<size;i++) printf("%d, ", frameIn[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void norm_clip_16(Norm_Clip_args_T *Args) {

  int       *In  = (int *__restrict__) Args->Out;
  short int *Out = (short int *__restrict__) Args->Out;
  unsigned short int Norm = Args->Norm;
  int N = Args->N;
  
  int i;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  if (CoreId==0)
    for (i=0; i<(unsigned int)N; i++) {
      Out[i] = (short int) gap_clip(AT_NORM(In[i], Norm), 15);
    }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_norm_clip = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Out[i]);
      printf("])\nNorm = %d\n\n", Norm);
    }
  #endif
}

void MFCC_ComputeDCT(DCT_Arg_T *Args) {

  unsigned int i,j;
  v2s       * in_dct   = (v2s * __restrict__ ) Args->Data;
  short int * FeatList = (short int * __restrict__ ) Args->FeatList;
  short int * twiddles = (short int * __restrict__) Args->Twiddles;
  short int * twiddct  = (short int * __restrict__) Args->Twidend;
  short int * SwapLUT  = (short int * __restrict__) Args->SwapLuts;
  short int NDCT       = Args->n_dct;
  short int numcep     = Args->numcep;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  
  Chunk = ChunkSize(numcep);
  First = CoreId*Chunk; Last = Min(First + Chunk, numcep);

  // convert int to v2s
  int shift=0; // scaling factor
  for(i=First; i<Last; i++) {
    in_dct[i] = (v2s) {(short int) (((FeatList)[i]) >> shift) , 0};
  }

  // 0 padding
  if (CoreId==0) 
  {
    memset(in_dct+numcep,0,(2*NDCT-numcep)*sizeof(v2s));

    // double stage fft
    FFT_Arg_T fft_arg;
    fft_arg.Data = in_dct;
    fft_arg.Twiddles = twiddles;
    fft_arg.N_fft = 2*NDCT;
    fft_arg.Inverse = 0;
    
    Radix2FFT_DIF(&fft_arg);
  }
  gap_waitbarrier(0);

  SwapSamples_Arg_T Swap_arg;
  Swap_arg.Data = in_dct;
  Swap_arg.SwapTable = SwapLUT;
  Swap_arg.Ni = 2*NDCT;

  //bit_reverse_reorder_int((short int *) in_dct, 2*NDCT, (int) 2);
  SwapSamples_Par(&Swap_arg);

  // last dct stage

  Chunk = ChunkSize(NDCT);
  First = CoreId*Chunk; Last = Min(First + Chunk, NDCT);

  for (i=First; i<Last; i++) {
    in_dct[i] =  (v2s) gap_cplxmuls(in_dct[i],((v2s*) twiddct)[i]);
    FeatList[i] = in_dct[i][0]; //take real part
  }

  gap_waitbarrier(0);
  #if PRINTDEB
    if (CoreId==0) {
      printf("\nout_dct = np.array([\n");
      for (int i=0; i<NDCT; i++){
        if (in_dct[i][1]>=0) printf("%d+%dj, ", in_dct[i][0], in_dct[i][1]);
        else                 printf("%d%dj, ", in_dct[i][0], in_dct[i][1]);
      }
      printf("])\nout_mfcc = np.array([\n");
      for (int i=0; i<NDCT; i++) printf("%d, ", FeatList[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

void MFCC_ComputeDCT_II(DCT_II_Arg_T *Args)
{
  unsigned int i,k;
  short int * in_dct   = (short int * __restrict__ ) Args->Data;
  short int * FeatList = (short int * __restrict__ ) Args->FeatList;
  short int * DCTCoeff = (short int * __restrict__) Args->DCTCoeff;
  short int NDCT       = Args->n_dct;
  short int numcep     = Args->numcep;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  // 0 padding
  if (CoreId==0) 
  {
    memset(in_dct+numcep, 0, (NDCT-numcep)*sizeof(short int));
  }

  // last dct stage
  Chunk = ChunkSize(NDCT);
  First = CoreId*Chunk; Last = Min(First + Chunk, NDCT);

  int TMP;  
  for (k=First; k<Last; k++) {
    TMP = 0;
    for (i=0; i<NDCT; i++){
      TMP += (in_dct[i] * DCTCoeff[i+k*NDCT]) >> (QNN+5);
    }
    FeatList[k] = (short int) gap_clip(TMP, 15);;
  }
  gap_waitbarrier(0);
  #if PRINTDEB
    if (CoreId==0) {
      printf("out_dct_ii = np.array([\n");
      for (int i=0; i<NDCT; i++) printf("%d, ", FeatList[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

void MFCC_Lifter(Lifter_Arg_T *Args)
{
  short int * FeatList = (short int * __restrict__ ) Args->FeatList;
  short int * LiftCoeff = (short int * __restrict__ ) Args->LiftCoeff;
  int FrameSize        = Args->FrameSize;
  // LIFTER LiftCoeff in Q11
  unsigned int j, Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);
  
  for (j=First; j<Last; j++) FeatList[j] = gap_mulsRN(FeatList[j], LiftCoeff[j], 11);
  gap_waitbarrier(0);
}

