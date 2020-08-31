#ifndef __MFCC_H__
#define __MFCC_H__

#include "Gap8.h"

/* Input frequency */
#define IN_FREQ			16000 

#define MIN_FREQ		300

#define MAX_FREQ		8000

/* Size of the frame, duration=FRAME*1/IN_FREQ, for 400: 25ms */
#define FRAME 			400

/* Step in samples between two overlapping frames, in our case 10ms */
#define FRAME_STEP		160

/* Fixed point precision of audio samples, in bits */
#define PRECISION		12

/* FFT dimension */
#define N_FFT			512
#define HALF_FFT		(N_FFT/2 + 1)

// #define MFCC_BANK_CNT		64
#define MFCC_BANK_CNT		40
#define MFCC_COEFF_DYN		10
#define MFCC_DERIV_COEFF_DYN	(12)
#define MFCC_DERIV_COEFF	((short int) (FP2FIX(0.1, MFCC_DERIV_COEFF_DYN)))

typedef struct {
        short int Start;
        short int Stop;
        short int Base;
        short int Norm;
} FbankType;

typedef struct {
        short int *FFT_Twiddles;
        short int *FFT_Swap;
        short int *Hamming;
        FbankType *FilterBank;
        short int *MFCC_Coeffs;
        short int *MFCC;
        v2s       *W_Frame;
        short int *FramePower;
        short int *Frame;  
        short int *InSignal;
} MFCC_Config_T;

#endif

