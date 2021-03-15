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
//#define LN_2   ((int) floor(log(2)*(float)(1<<QN)+0.5))
//#define c1_3      ((int) floor((1.0/3.0) * (float)(1<<QN) +0.5))
#define QNN                 16
#define QN_LOC              34
#define LN_2                (0xb172)
#define c1_3                (0x5555)
#define LN_10_INV_Q10       (0x1bd)
#define LOG10_2             (0x4d10)

//#define LOG_OFFSET    (0x28f)

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
  
    shift = max?gap_fl1(max):0;
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

#ifdef __gap9__
void MFCC_PreEmphasis_f16(MFCC_PreEmphasis_T *Arg)
{
  f16 * __restrict__ Frame = Arg->Frame;
  f16 * __restrict__ Out = Arg->Out;
  int FrameSize = Arg->FrameSize;
  f16 PreempFactor = Arg->PreempFactor;
  f16 S;
  f16 Sprev;
  
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  // Y[n]=X[n]−0.97⋅X[n−1]
  S = CoreId? Frame[First-1] : Arg->Prev;

  for (int i=First; i<Last; i++) {
    Sprev = Frame[i];
    Out[i] = Sprev - PreempFactor * S;
    S = Sprev;
  }

  gap_waitbarrier(0);

  if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nShift = %d\n",*Arg->Shift);
    printf("out_preemph_c = np.array([\n\t\t");
    for(int i=0; i<FrameSize ; i++) printf("%f, ", Out[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}
#endif

void MFCC_PreEmphasis_f32(MFCC_PreEmphasis_T *Arg)
{
  float * __restrict__ Frame = Arg->Frame;
  float * __restrict__ Out = Arg->Out;
  int FrameSize = Arg->FrameSize;
  float PreempFactor = Arg->PreempFactor;
  float S;
  float Sprev;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  // Y[n]=X[n]−0.97⋅X[n−1]
  S = CoreId? Frame[First-1] : Arg->Prev;

  for (int i=First; i<Last; i++) {
    Sprev = Frame[i];
    Out[i] = Sprev - PreempFactor * S;
    S = Sprev;
  }

  gap_waitbarrier(0);

  if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nShift = %d\n",*Arg->Shift);
    printf("out_preemph_c = np.array([\n\t\t");
    for(int i=0; i<FrameSize ; i++) printf("%f, ", Out[i]);
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
  Chunk = ChunkSize(FFT_Dim-FrameSize);
  First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
  for (i=First; i<Last;i++) OutFrame[i] = (v2s) {0,0};
  
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
  Chunk = ChunkSize(FFT_Dim-FrameSize);
  First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
  for (i=First; i<Last;i++) {
    OutFrame[2*i]   = 0;
    OutFrame[2*i+1] = 0;
  }
  //if (CoreId==0) for (i=FrameSize; i<(unsigned int)FFT_Dim;i++) {
  //  OutFrame[2*i]   = 0;
  //  OutFrame[2*i+1] = 0;
  //} 
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

#ifdef __gap9__
void MFCC_WindowedFrame_f16(MFCC_WF_T *Arg)
{
  f16 *__restrict__ Frame     = (f16*) Arg->Frame;
  v2h *__restrict__ OutFrame  = (v2h*) Arg->OutFrame;
  f16 *__restrict__ Window    = (f16*) Arg->Window;
  int                     FrameSize = Arg->FrameSize;
  int                     FFT_Dim   = Arg->FFT_Dim;

  int i;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  for (i=First; i<Last; i++) {
    /* Frame is Q15+shift and Window is Q15 */
    OutFrame[i] = (v2h) {Frame[i] * Window[i], 0};
  }
  // 0 padding
  Chunk = ChunkSize(FFT_Dim-FrameSize);
  First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
  for (i=First; i<Last;i++) OutFrame[i] = (v2h) {0,0};
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nN_FFT = %d\n",FFT_Dim);
    printf("out_window_c = np.array([\n\t\t");
    for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i][0]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}
#endif

void MFCC_WindowedFrame_f32(MFCC_WF_T *Arg)
{
  float     *__restrict__ Frame     = (float *) Arg->Frame;
  float     *__restrict__ OutFrame  = (float *) Arg->OutFrame;
  float     *__restrict__ Window    = (float *) Arg->Window;
  int                     FrameSize = Arg->FrameSize;
  int                     FFT_Dim   = Arg->FFT_Dim;

  int i;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(FrameSize);
  First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

  for (i=First; i<Last; i++) {
    /* Frame is Q15+shift and Window is Q15 */
    OutFrame[2*i] = (float) (Frame[i] * Window[i]);
    OutFrame[2*i+1] = 0;
  }
  // 0 padding
  Chunk = ChunkSize(FFT_Dim-FrameSize);
  First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
  for (i=First; i<Last;i++) {
    OutFrame[2*i] = 0;
    OutFrame[2*i+1] = 0;
  }
  gap_waitbarrier(0);

#ifdef PRINTDEB
  if (CoreId==0) {
    printf("\nN_FFT = %d\n",FFT_Dim);
    printf("out_window_c = np.array([\n\t\t");
    for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[2*i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void MFCC_PowerV2S(MFCC_EP_BFF_T *Arg)
{
  int          *__restrict__ FrameIn   = (int *) Arg->FrameIn;
  unsigned int *__restrict__ Power_int = (unsigned int *) Arg->FrameOut;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  unsigned int P;
  for (i=First; i<(unsigned int)Last; i++) {
    P = (unsigned int) gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]);
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

void MFCC_Power(MFCC_EP_BFF_T *Arg)
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

#ifdef __gap9__
void MFCC_Power_f16(MFCC_EP_BFF_T *Arg)
{
  v2h *__restrict__ FrameIn   = (v2h *) Arg->FrameIn;
  f16 *__restrict__ Power     = (f16 *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;
  

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    f16 P = (FrameIn[i][0] * FrameIn[i][0]) + (FrameIn[i][1] * FrameIn[i][1]);
    Power[i] = P; // __MULSRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_power = np.array([\n\t");
      for (i=0; i<N; i++) printf("%f, ", Power[i]);
      printf("])\n");
    }
  #endif
}
#endif

void MFCC_Power_f32(MFCC_EP_BFF_T *Arg)
{
  float *__restrict__ FrameIn   = (float *) Arg->FrameIn;
  float *__restrict__ Power     = (float *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;
  

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    float P = (FrameIn[2*i] * FrameIn[2*i]) + (FrameIn[2*i+1] * FrameIn[2*i+1]);
    Power[i] = P; // __MULSRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_power = np.array([\n\t");
      for (i=0; i<N; i++) printf("%f, ", Power[i]);
      printf("])\n");
    }
  #endif
}

#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))
/* square root coefficients in 17.15 fixed point format */
int SqrtCoeffTable[] =
{
  0x00001A91, 0x0000BA3A, 0xFFFF53DA, 0x00008DAC,
  0xFFFFBB54, 0x00000E5A
};

/* square root of 2 powered to (index - 15) in 17.15 format */
int Sqrt2Powers[] =
{
  0x000000B5, 0x00000100, 0x0000016A, 0x00000200,
  0x000002D4, 0x00000400, 0x000005A8, 0x00000800,
  0x00000B50, 0x00001000, 0x000016A1, 0x00002000,
  0x00002D41, 0x00004000, 0x00005A82, 0x00008000,
  0x0000B505, 0x00010000, 0x00016A0A, 0x00020000,
  0x0002D414, 0x00040000, 0x0005A828, 0x00080000,
  0x000B504F, 0x00100000, 0x0016A09E, 0x00200000,
  0x002D413D, 0x00400000, 0x005A827A, 0x00800000
};

unsigned int sqrt_17_15(unsigned int x) {
  unsigned int   exponent;
  int            result, y, z;

  if (!x)
    return 0;
  if (x > 0x7FFFFFFF) return 0; /* negative value */
  exponent = (unsigned int) gap_clb ((int) x);
  y = ((x << exponent) >> 16);

  /* sqrt(x) = 0.2075806 + 1.454895 * x - 1.34491 * x^2 + 1.106812 * x^3 - 0.536499 * x^4 + 0.1121216 * x^5 */
  z = y;
  result = 0;
  for (int i = 1; i < ARRAYSIZE (SqrtCoeffTable); i++) {
    result += z * SqrtCoeffTable[ i ];
    z = ((z * y) >> 15);
  }
  result >>= 15;
  result += SqrtCoeffTable[ 0 ];
  if (exponent != 16) result = ((result * Sqrt2Powers[ 31 - exponent ]) >> 15);

  return (unsigned int) result;
}

void MFCC_Abs(MFCC_EP_BFF_T *Arg)
{
  int          *__restrict__ FrameIn     = (int *) Arg->FrameIn;
  unsigned int *__restrict__ Spectrogram = (unsigned int *) Arg->FrameOut;
  signed char *     shift_fft = (signed char *) Arg->shift_fft;
  signed char       Shift = *Arg->Shift;
  int N = (Arg->nfft)/2 + 1;
  // int Qpow = 2*(16-gap_fl1(Arg->nfft)+Shift);
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    Spectrogram[i] = ((unsigned int) round(sqrt(gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]))));
    //unsigned int P = (unsigned int) gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]) ;
    //int pre_shift = gap_fl1(P) - 15;
    //if (pre_shift > 0) {
    //  P >>= pre_shift; 
    //} else {
    //  pre_shift = 0;
    //}
    //Spectrogram[i] = sqrt_17_15((Qpow+pre_shift-15)>0?(P >>(Qpow+pre_shift-15)):(P <<(15-Qpow-pre_shift))) << pre_shift;
  }

  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_spectrogram = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Spectrogram[i]);
      printf("])");
      printf("\nshift_fft = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", shift_fft[i]);
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
    //int Qpow = shift_fft[i] + Shift + 1;
    //unsigned int P = (unsigned int) ((FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16));
    //int pre_shift = gap_fl1(P) - 15;
    //if (pre_shift > 0) {
    //  P >>= pre_shift; 
    //} else {
    //  pre_shift = 0;
    //}
    //P = (Qpow+pre_shift-15)>0?(P >>(Qpow+pre_shift-15)):(P <<(15-Qpow-pre_shift));
    //Spectrogram[i] = sqrt_17_15(P) << pre_shift;
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

#ifdef __gap9__
void MFCC_Abs_f16(MFCC_EP_BFF_T *Arg)
{
  f16 *__restrict__ FrameIn     = (f16 *) Arg->FrameIn;
  f16 *__restrict__ Spectrogram = (f16 *) Arg->FrameOut;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    Spectrogram[i] = (f16) sqrt(FrameIn[2*i]*FrameIn[2*i] + FrameIn[2*i+1]*FrameIn[2*i+1]);
  }

  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_spectrogram = np.array([\n\t");
      for (i=0; i<N; i++) printf("%f, ", Spectrogram[i]);
      printf("])");
    }
  #endif
}
#endif

void MFCC_Abs_f32(MFCC_EP_BFF_T *Arg)
{
  float *__restrict__ FrameIn     = (float *) Arg->FrameIn;
  float *__restrict__ Spectrogram = (float *) Arg->FrameOut;
  int N = (Arg->nfft)/2 + 1;
  unsigned int i;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  Chunk = ChunkSize(N);
  First = CoreId*Chunk; Last = Min(First + Chunk, N);

  for (i=First; i<(unsigned int)Last; i++) {
    Spectrogram[i] = (float) sqrt(FrameIn[2*i]*FrameIn[2*i] + FrameIn[2*i+1]*FrameIn[2*i+1]);
  }

  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_spectrogram = np.array([\n\t");
      for (i=0; i<N; i++) printf("%f, ", Spectrogram[i]);
      printf("])");
    }
  #endif
}

void MFCC_ComputeMFCC(MFCC_MF_New_T *Arg)
{
  unsigned int i, j, k;

  unsigned int *__restrict__ FramePower      = Arg->FramePower;
  unsigned int *__restrict__ MFCC            = Arg->MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank = Arg->MFCC_FilterBank;
  short int    *__restrict__ MFCC_Coeffs     = Arg->MFCC_Coeffs;
  short int                  mfcc_bank_cnt   = Arg->MFCC_bank_cnt;
  short int                  MFCC_Coeff_Dyn  = Arg->MFCC_Coeff_dyn;
  signed char  *__restrict__ shift_BF        = Arg->shift_BF;
  signed char  *             shift_pow       = Arg->shift_pow;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  
  Chunk = ChunkSize(mfcc_bank_cnt);
  First = CoreId*Chunk; Last = Min(First + Chunk, mfcc_bank_cnt);
  
  /*
    MFCC_Coeffs is in MFCC_Coeff_Dyn
  */

  for (i=First; i<(unsigned int)Last; i++) {
    unsigned int Coeff = 0;
    int Base = MFCC_FilterBank[i].Base;
    int NonZeroItems = MFCC_FilterBank[i].Items;

    // Before computing the coeff multiplication check that max(FramePower) + 10 (QCoeff)
    // do not exceed 31, if so right shift (lose precision) the coeff
    int maxin=0;
    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      if (FramePower[j]>(unsigned int)maxin) maxin =FramePower[j];
    }

    int shift;
    short int shift0 = maxin?gap_fl1(maxin):0;
    // check multiply overflow condition FramePower * Q6.10 <= Q31
    if ((shift0+MFCC_Coeff_Dyn+1)>31) shift = shift0+MFCC_Coeff_Dyn+1-31; else shift = 0;
    shift_BF[i] = shift + shift_pow[i];

    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      // very easy to overflow here - maybe decrease the coeff dyn
      Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>shift);
    }
    MFCC[i] = Coeff?Coeff:1; // Avoid 0s for log
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_melfilterbank = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%u, ", MFCC[i]);
      printf("])\n");
      printf("\nshift_BF = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%d, ", shift_BF[i]);
      printf("])\n");
      //printf("Shift = %d\n", *(Arg->Shift));
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
  short int                  MFCC_Coeff_Dyn  = Arg->MFCC_Coeff_dyn;
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
    int NonZeroItems = MFCC_FilterBank[i].Items;

    int maxin=0;
    min_shift = 0x7fff;

    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      if (shift_pow[j] < min_shift) min_shift = shift_pow[j];
    }

    // align the block scaling on the min , compute the max value in the block
    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++) {
      int TMP = FramePower[j] >> ( MUL_EXP * (shift_pow[j] - min_shift));
      if (TMP > (unsigned int) maxin) maxin = TMP;
    }

    // compute shift right to apply on the block to prevent overflow
    shift  = 0 ;
    short int shift0 = maxin?gap_fl1(maxin):0;

    // check multiply overflow conditionQ18.14 * Q6.10 => Q8.24
    if ((shift0+MFCC_Coeff_Dyn+2) > 31) shift = shift0+MFCC_Coeff_Dyn+2-31; else shift = 0;

    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>(shift + MUL_EXP * (shift_pow[j] - min_shift)));
      //Coeff = Coeff + MFCC_Coeffs[Base+k]*(FramePower[j]>>(shift));
    }
    // update the shift left  value on the block (to compensate)
    shift_BF[i] = shift - MUL_EXP * min_shift;
    MFCC[i] = Coeff?Coeff:1; // Avoid 0s for log
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("# MUL_EXP = %d\n", MUL_EXP);
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

#ifdef __gap9__
void MFCC_ComputeMFCC_f16(MFCC_MF_New_T *Arg)
{
  unsigned int i, j, k;

  f16 *__restrict__ FramePower = (f16*) Arg->FramePower;
  f16 *__restrict__ MFCC       = (f16*) Arg->MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank = Arg->MFCC_FilterBank;
  f16 *__restrict__ MFCC_Coeffs = (f16*) Arg->MFCC_Coeffs;
  short int                  mfcc_bank_cnt   = Arg->MFCC_bank_cnt;
  short int                  MFCC_Coeff_Dyn  = Arg->MFCC_Coeff_dyn;
  signed char  *__restrict__ shift_BF        = Arg->shift_BF;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  
  Chunk = ChunkSize(mfcc_bank_cnt);
  First = CoreId*Chunk; Last = Min(First + Chunk, mfcc_bank_cnt);
  
  /*
    MFCC_Coeffs is in MFCC_Coeff_Dyn
  */

  for (i=First; i<(unsigned int)Last; i++) {
    f16 Coeff = 0;
    int Base = MFCC_FilterBank[i].Base;
    int NonZeroItems = MFCC_FilterBank[i].Items;
    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      Coeff = Coeff + MFCC_Coeffs[Base+k]*FramePower[j];
    }
    MFCC[i] = Coeff; // Avoid 0s for log
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_melfilterbank = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%e, ", MFCC[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}
#endif
void MFCC_ComputeMFCC_f32(MFCC_MF_New_T *Arg)
{
  unsigned int i, j, k;

  float *__restrict__ FramePower = (float*) Arg->FramePower;
  float *__restrict__ MFCC       = (float*) Arg->MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank = Arg->MFCC_FilterBank;
  float *__restrict__ MFCC_Coeffs = (float*) Arg->MFCC_Coeffs;
  short int                  mfcc_bank_cnt   = Arg->MFCC_bank_cnt;
  short int                  MFCC_Coeff_Dyn  = Arg->MFCC_Coeff_dyn;
  signed char  *__restrict__ shift_BF        = Arg->shift_BF;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();
  
  Chunk = ChunkSize(mfcc_bank_cnt);
  First = CoreId*Chunk; Last = Min(First + Chunk, mfcc_bank_cnt);
  
  /*
    MFCC_Coeffs is in MFCC_Coeff_Dyn
  */

  for (i=First; i<(unsigned int)Last; i++) {
    float Coeff = 0;
    int Base = MFCC_FilterBank[i].Base;
    int NonZeroItems = MFCC_FilterBank[i].Items;
    for (k=0, j=MFCC_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
      Coeff = Coeff + MFCC_Coeffs[Base+k]*FramePower[j];
    }
    MFCC[i] = Coeff; // Avoid 0s for log
  }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_melfilterbank = np.array([\n\t");
      for (i=0; i<mfcc_bank_cnt; i++) printf("%e, ", MFCC[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}

#if 0
#define LOG_C_1   (0x1bde6)
#define LOG_C_2   (0x2d23a)
#define LOG_C_3   (0x1784f)
#define LOG_C_4   (0x727a)
#define LOG_C_5   (0xe7b)

int MFCC_Logfp(unsigned int a)
{
    int iLog2N  = a?gap_fl1(a):0;
    int valint = iLog2N - QNN;

    if (valint>=0) {
      if ((a>>valint)&(1<<(QNN-1))) valint++;
      a  = a >> valint;
    }
    else {
        a = a << (-valint);
    }
    int loga = (-LOG_C_1 + (LOG_C_2 + (((-LOG_C_3 + (((LOG_C_4 - ((LOG_C_5 * a) >> QNN)) * a) >> QNN)) * a) >> QNN) * a)) >> QNN;
    loga += (valint*LN_2);
    //int loga =  (valint*LN_2) - ar + ar2 - ar3;
    return (int) loga;
}
#endif

int MFCC_Logfp(unsigned int a)
{
    int iLog2N  = a?gap_fl1(a):0;
    int valint = iLog2N - QNN;

    // We ensure that a is in the range where the log approximation is valid.
    if (valint>=0) {
      if ((a>>valint)&(1<<(QNN-1))) valint++;
      a  = a >> valint;
    }
    else {
        a = a << (-valint);
    }
	
    #ifdef USE_FLOAT_LOG
      return (int) round(logf((float) a / (1<<QNN)) * (1<<QNN)) + (valint*LN_2);
    #else
      int ar = (1<<QNN) - a; // (1-ar = a):  log(1-ar) ~ -ar + ar*ar/2 - ar*ar*ar/3
      int ar2 = ((ar>>(QNN>>1)) * (ar>>(QNN>>1))) >> (1);
      int ar3 = ((ar2) * (ar)) >> (QNN-1);
      ar3 = ((ar3) * (c1_3)) >> QNN;

      int loga =  (valint*LN_2) - ar + ar2 - ar3;
      return (int) loga;
    #endif
}


void MFCC_ComputeLog( MFCC_Log_T *Arg) {

  int i;
  int         size      = Arg->FrameSize;
  unsigned int *frameIn  = (unsigned int *) Arg->FrameIn;
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
    #ifndef USE_ABS
    #ifdef HIGH_PREC_FFT
      int Qformat =  (mfcc_coeff_dyn-2-shift_BF[i]+2*Shift); //POWER HIGH_PREC
    #else
      int Qformat =  (2*(16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn); //POWER
    #endif
    #else
    #ifdef HIGH_PREC_FFT
      int Qformat = (mfcc_coeff_dyn-1-shift_BF[i]+Shift); //Abs HIGH_PREC
    #else
      //int Qformat = (15-shift_BF[i]+mfcc_coeff_dyn); //Abs
      int Qformat = ((16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn); //Abs
    #endif
    #endif


    #ifdef LOG_OFFSET
      int Log_offset = ((QNN-Qformat)<=0)?LOG_OFFSET << (-QNN+Qformat):LOG_OFFSET >> (QNN-Qformat);
      TMP = MFCC_Logfp(frameIn[i] + Log_offset);
    #else
      TMP = MFCC_Logfp(frameIn[i]);
    #endif

    frameIn[i] =  TMP - (Qformat-QNN) * LN_2;
  }
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    #ifdef HIGH_PREC_FFT
      printf("#HIGH_PREC\n");
    #endif
    printf("\nout_log = np.array([\n");
    for(i=0;i<size;i++) printf("%d, ", frameIn[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDB( MFCC_Log_T *Arg) {

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
    #ifndef USE_ABS
    #ifdef HIGH_PREC_FFT
      int Qformat =  (mfcc_coeff_dyn-2-shift_BF[i]+2*Shift); //POWER HIGH_PREC
    #else
      int Qformat =  (2*(16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn); //POWER
    #endif
    #else
    #ifdef HIGH_PREC_FFT
      int Qformat = (mfcc_coeff_dyn-1-shift_BF[i]+Shift); //Abs HIGH_PREC
    #else
      int Qformat = ((16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn); //Abs
      //int Qformat = (15-shift_BF[i]+mfcc_coeff_dyn); //Abs
    #endif
    #endif

    #ifdef LOG_OFFSET
      int Log_offset = ((QNN-Qformat)<=0)?LOG_OFFSET << (-QNN+Qformat):LOG_OFFSET >> (QNN-Qformat);
      TMP = MFCC_Logfp(frameIn[i] + Log_offset);
    #else
      TMP = MFCC_Logfp(frameIn[i]);
    #endif

    frameIn[i] = 10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2);
  }
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    #ifdef HIGH_PREC_FFT
      printf("#HIGH_PREC\n");
    #endif
    printf("\nout_db = np.array([\n");
    for(i=0;i<size;i++) printf("%d, ", frameIn[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void norm_clip_16(Norm_Clip_args_T *Args) {

  int       *In  = (int *__restrict__) Args->In;
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

void norm_clip_32_melspect(MFCC_Clip_32_T *Args) {
  // Output is 32 bit integer in Q(QNN) fixed point
  unsigned int *In  = (unsigned int *__restrict__) Args->In;
  unsigned int *Out = (unsigned int *__restrict__) Args->Out;
  int          Shift     = *Args->Shift;
  int          fft_bits  = Args->fft_bits;
  int          mfcc_coeff_dyn = Args->MFCC_Coeff_dyn;
  signed char *shift_BF = Args->shift_BF;
  int N = Args->N;
  
  int i, Norm;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  if (CoreId==0)
    for (i=0; i<(unsigned int)N; i++) {
      #ifndef USE_ABS
      #ifdef HIGH_PREC_FFT
        Norm = mfcc_coeff_dyn-2-shift_BF[i]+2*Shift - 16; //POWER HIGH_PREC
      #else
        Norm = 2*(16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn - 16; //POWER
      #endif
      #else
      #ifdef HIGH_PREC_FFT
        Norm = mfcc_coeff_dyn-1-shift_BF[i]+Shift - 16; //Abs HIGH_PREC
      #else
        Norm = (16-fft_bits+Shift)-shift_BF[i]+mfcc_coeff_dyn - 16; //Abs
      #endif
      #endif
      Out[i] = (Norm<32)?(In[i] >> Norm):0;
    }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      printf("\nout_norm_clip = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Out[i]);
      printf("])\n");
      printf("Shift %d\n", Shift);
    }
  #endif
}

void norm_clip_32_spect(MFCC_Clip_32_T *Args) {
  // Output is 32 bit integer in Q(QNN) fixed point
  unsigned int *In  = (unsigned int *__restrict__) Args->In;
  unsigned int *Out = (unsigned int *__restrict__) Args->Out;
  int          Shift     = *Args->Shift;
  int          fft_bits  = Args->fft_bits;
  signed char *shift_pow = Args->shift_BF;
  int N = Args->N / 2 + 1;
  
  int i, Norm;
  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  if (CoreId==0)
    for (i=0; i<(unsigned int)N; i++) {
      #ifndef USE_ABS
      #ifdef HIGH_PREC_FFT
        Norm = -2+2*shift_pow[i]+2*Shift - 13; //POWER HIGH_PREC
      #else
        Norm = 2*(16-fft_bits+Shift) - 13; //POWER
      #endif
      #else
      #ifdef HIGH_PREC_FFT
        Norm = -1+shift_pow[i]+Shift - 13; //Abs HIGH_PREC
      #else
        Norm = (16-fft_bits+Shift) - 13; //Abs
      #endif
      #endif
      Out[i] = (Norm<32)?(In[i] >> Norm):0;
    }
  gap_waitbarrier(0);
  #ifdef PRINTDEB
    if (CoreId==0) {
      #ifdef USE_ABS
      printf("#USE_ABS\n");
      #endif
      #ifdef HIGH_PREC_FFT
      printf("#HIGH_PREC\n");
      #endif
      printf("\nout_norm_clip = np.array([\n\t");
      for (i=0; i<N; i++) printf("%d, ", Out[i]);
      printf("])\n");
      printf("Shift %d\n", Shift);
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
    
    Radix2FFT_DIF_Par_Fix16(&fft_arg);
  }
  gap_waitbarrier(0);

  FFT_SwapSamples_T Swap_arg;
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

  // last dct stage
  Chunk = ChunkSize(NDCT);
  First = CoreId*Chunk; Last = Min(First + Chunk, NDCT);

  int TMP;  
  for (k=First; k<Last; k++) {
    TMP = 0;
    for (i=0; i<NDCT; i++){
      TMP += (in_dct[i] * DCTCoeff[i+k*NDCT]) >> (QNN+4);
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

#ifdef __gap9__
void MFCC_ComputeLog_f16( MFCC_Log_T *Arg) {

  int i;
  int         size      = Arg->FrameSize;
  f16         *frameIn  = (f16 *) Arg->FrameIn;
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
    #ifdef LOG_OFFSET
      frameIn[i] = (f16) logf((float) frameIn[i] + LOG_OFFSET);
    #else
      frameIn[i] = (f16) logf((float) frameIn[i] + 1e-6);
    #endif
  }
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    #ifdef HIGH_PREC_FFT
      printf("#HIGH_PREC\n");
    #endif
    printf("\nout_log = np.array([\n");
    for(i=0;i<size;i++) printf("%f, ", frameIn[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDCT_II_f16(DCT_II_Arg_T *Args)
{
  unsigned int i,k;
  f16 * in_dct   = (f16 * __restrict__ ) Args->Data;
  f16 * FeatList = (f16 * __restrict__ ) Args->FeatList;
  f16 * DCTCoeff = (f16 * __restrict__) Args->DCTCoeff;
  short int NDCT       = Args->n_dct;
  short int numcep     = Args->numcep;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  // last dct stage
  Chunk = ChunkSize(NDCT);
  First = CoreId*Chunk; Last = Min(First + Chunk, NDCT);

  f16 TMP;  
  for (k=First; k<Last; k++) {
    TMP = 0;
    for (i=0; i<NDCT; i++){
      TMP += in_dct[i] * DCTCoeff[i+k*NDCT];
    }
    FeatList[k] = TMP;
  }
  gap_waitbarrier(0);
  #if PRINTDEB
    if (CoreId==0) {
      printf("NDCT = %d\nout_dct_ii = np.array([\n", NDCT);
      for (int i=0; i<NDCT; i++) printf("%f, ", FeatList[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}
#endif

void MFCC_ComputeLog_f32( MFCC_Log_T *Arg) {

  int i;
  int         size      = Arg->FrameSize;
  float       *frameIn  = (float *) Arg->FrameIn;
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
    #ifdef LOG_OFFSET
      frameIn[i] = (float) logf((float) frameIn[i] + LOG_OFFSET);
    #else
      frameIn[i] = (float) logf((float) frameIn[i] + 1e-6);
    #endif
  }
  
  gap_waitbarrier(0);
#ifdef PRINTDEB
  if (CoreId==0) {
    #ifdef HIGH_PREC_FFT
      printf("#HIGH_PREC\n");
    #endif
    printf("\nout_log = np.array([\n");
    for(i=0;i<size;i++) printf("%f, ", frameIn[i]);
    printf("])\n");
  }
  gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDCT_II_f32(DCT_II_Arg_T *Args)
{
  unsigned int i,k;
  float * in_dct   = (float * __restrict__ ) Args->Data;
  float * FeatList = (float * __restrict__ ) Args->FeatList;
  float * DCTCoeff = (float * __restrict__) Args->DCTCoeff;
  short int NDCT       = Args->n_dct;
  short int numcep     = Args->numcep;

  unsigned int Chunk, First, Last, CoreId=gap_coreid();

  // last dct stage
  Chunk = ChunkSize(NDCT);
  First = CoreId*Chunk; Last = Min(First + Chunk, NDCT);

  float TMP;  
  for (k=First; k<Last; k++) {
    TMP = 0;
    for (i=0; i<NDCT; i++){
      TMP += in_dct[i] * DCTCoeff[i+k*NDCT];
    }
    FeatList[k] = TMP;
  }
  gap_waitbarrier(0);
  #if PRINTDEB
    if (CoreId==0) {
      printf("NDCT = %d\nout_dct_ii = np.array([\n", NDCT);
      for (int i=0; i<NDCT; i++) printf("%f, ", FeatList[i]);
      printf("])\n");
    }
    gap_waitbarrier(0);
  #endif
}