#include <stdio.h>
#include <math.h>
#include <cmsis.h>

/* Input frequency */
#define IN_FREQ         16000 // 625us
#define MIN_FREQ        0
#define MAX_FREQ        8000

/* Size of the frame, duration=FRAME*1/IN_FREQ,  25ms */
#define FRAME           480
#define N_FRAME         98

/* Step in samples between two overlapping frames, in our case 10ms */
#define FRAME_STEP      160

/* Fixed point precision of audio samples, in bits */
#define PRECISION        12

/* FFT dimension */
#define N_FFT            512
#define HALF_FFT         (N_FFT/2 + 1)
#define NUMCEP           40
#define NDCT             64
#define L_LIFTER         22

// #define MFCC_BANK_CNT 64
#define MFCC_BANK_CNT    40
#define MFCC_COEFF_DYN   10

// fractional definition for log
#define QN               24

//#define NORMDCT                              ((short int) floor(1.0/sqrt(2)*(1<<15)))
#define NORMDCT                                (0x5a82)
//#define GAPLOG2                              ((int) floor(log(2)*(float)(1<<QN)+0.5))
#define GAPLOG2                                (0xb17218)
#define FIX2FP(Val, Precision)                 ((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)                ((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)                 ((int)((Val)*((1 << (Precision))-1)))


typedef struct fbank_type_ {
    short int Start;
    short int Stop;
    short int Base;
    short int Norm;
} fbank_type_t;

typedef struct mfcc_config_ {
    short int *FFT_Twiddles;
    short int *FFT_Swap;
    short int *Hamming;
    fbank_type_t *FilterBank;
    short int *MFCC_Coeffs;
    short int *MFCC;
    v2s       *W_Frame;
    short int *FramePower;
    short int *Frame;
    short int *InSignal;
} mfcc_config_t;

int  MFCC_Logfp(unsigned int a);
void MFCC_ComputeLog( unsigned int *MFCC);
void SwapSamples_args(v2s *__restrict__ Data,
                      short int *__restrict__ SwapTable ,
                      int Ni);
void Radix2FFT_DIF_args(short int *__restrict__ Data,short int *__restrict__ Twiddles, int N_FFT2);
void MFCC_PreEmphasis(short int * __restrict__ Frame, short int * __restrict__ Out, int FrameSize, short int S, short int  shift);
void MFCC_WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame,
                   short int *__restrict__ Window, int FrameSize, int FFT_Dim);
void MFCC_EstimatePower(v2s *__restrict__ FrameIn,  int N, int N_fft, int SHIFT_RIGHT);
void MFCC_EstimatePowerInt(v2s *__restrict__ FrameIn,  int N, int N_fft);
void MFCC_Compute(short int *__restrict__ FramePower,  int *__restrict__ MFCC);
void MFCC_ComputeMFCCInt(unsigned int *__restrict__ FramePower,  unsigned int *__restrict__ MFCC);
void MFCC_ComputeDCT(v2s *in_dct, v2s *twiddct, v2s *twiddles, short int *SwapLUT) ;
