/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#ifndef __MFCC_H__
//#define __MFCC_H__
//#include "Gap8.h"


/* Input frequency */
#define IN_FREQ			16000 // 625us
#define MIN_FREQ		0
#define MAX_FREQ		8000

/* Size of the frame, duration=FRAME*1/IN_FREQ,  25ms */
#define FRAME 			480
#define N_FRAME 		98

/* Step in samples between two overlapping frames, in our case 10ms */
#define FRAME_STEP		160

/* Fixed point precision of audio samples, in bits */
#define PRECISION		12

/* FFT dimension */
#define N_FFT			512
#define HALF_FFT		(N_FFT/2 + 1)
#define NUMCEP                  40
#define NDCT                    64
#define L_LIFTER                22

// #define MFCC_BANK_CNT		64
#define MFCC_BANK_CNT		40
#define MFCC_COEFF_DYN		10

// fractional definition for log
#define QN 24
#define gaplog2  ((int) floor(log(2)*(float)(1<<QN)+0.5))


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

int logfp(unsigned int a);
void Compute_log( unsigned int *MFCC);
void SwapSamples_args(        v2s *__restrict__ Data,
			 short int *__restrict__ SwapTable ,
			      int Ni);
void Radix2FFT_DIF_args(short int *__restrict__ Data,short int *__restrict__ Twiddles, int N_FFT2);
void PreEmphasis(short int * __restrict__ Frame, short int * __restrict__ Out, int FrameSize, short int S, short int  shift);
void WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame,
		          short int *__restrict__ Window, int FrameSize, int FFT_Dim);
void EstimatePower(v2s *__restrict__ FrameIn,  int N, int N_fft, int SHIFT_RIGHT);
void EstimatePower_int(v2s *__restrict__ FrameIn,  int N, int N_fft);
void ComputeMFCC(short int *__restrict__ FramePower,  int *__restrict__ MFCC);
void ComputeMFCC_int(unsigned int *__restrict__ FramePower,  unsigned int *__restrict__ MFCC);
void Compute_dct(v2s *in_dct, v2s *twiddct, v2s *twiddles, short int *SwapLUT) ;

//#endif

