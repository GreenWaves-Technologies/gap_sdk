#include "Gap.h"
#include "FFT_Library.h"

#ifndef AT_NORM
#define AT_NORM(x, n)	gap_norm_reg((x), (n))
#endif

typedef struct fbank_type_ {
	short int Start;
	short int Items;
	short int Base;
} fbank_type_t;

typedef struct {
        void *__restrict__ FramePower;
        void *__restrict__ MelSpectr;
        void *__restrict__ Mel_Coeffs;
        signed char *__restrict__ shift_buff;
        signed char *__restrict__ shift_fft;
        fbank_type_t *__restrict__ Mel_FilterBank;
        short int Mel_NBanks;
        short int Mel_Coeff_dyn;
        signed char IsMagSquared;
} MelFilterBank_T;

typedef struct {
        void * __restrict__ FrameIn;
        void * __restrict__ FrameOut;
        unsigned int FrameSize;
        unsigned short int Norm;
        short int ExtraQ;
        short int Q_FFT_Out;
        short int Mel_Coeff_Dyn;
        signed char IsMagSquared;
        signed char *__restrict__ shift_buff;
} MFCC_Log_T;

typedef struct {
        int * __restrict__ In;
        short int * __restrict__ Out;
        unsigned short int Norm;
        int N;
} Norm_Clip_args_T;

typedef struct {
        unsigned int * __restrict__ In;
        unsigned int * __restrict__ Out;
        unsigned int N;
        short int ExtraQ;
        short int Q_FFT_Out;
        short int Mel_Coeff_Dyn;
        signed char IsMagSquared;
        signed char *__restrict__ shift_buff;
} MFCC_Clip_32_T;

typedef struct {
        void * __restrict__ Data;     /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        void * __restrict__ DCTCoeff; /**< Pointer to DCT twiddles (I, Q) pairs, I and Q fixed point Q15 */
        void * __restrict__ FeatList; /**< Pointer to Feature list  */
        short int n_input;           /**< Number of inputs */
        short int n_dct;           /**< DCT dimension, doesn't has to be a power of 2, must be <= n_inputs */
} DCT_II_Arg_T;

typedef struct {
        void *__restrict__  FeatList;  /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        void * __restrict__ LiftCoeff;
        int FrameSize;                      /**< Frame dimension */
} Lifter_Arg_T;

void MelFilterBank_Fix32(MelFilterBank_T *Arg);
void MelFilterBank_Fix32_Scal(MelFilterBank_T *Arg);
void MelFilterBank_f32(MelFilterBank_T *Arg);

void MFCC_ComputeLog_Fix32(MFCC_Log_T *Arg);
void MFCC_ComputeLog_Fix32_Scal(MFCC_Log_T *Arg);
void MFCC_ComputeLog_f32(MFCC_Log_T *Arg);
void MFCC_ComputeDB_Fix32(MFCC_Log_T *Arg);
void MFCC_ComputeDB_Fix32_Scal(MFCC_Log_T *Arg);
void MFCC_ComputeDB_f32(MFCC_Log_T *Arg);

void norm_clip_16(Norm_Clip_args_T *Args);
void norm_clip_32_melspect(MFCC_Clip_32_T *Args);
void norm_clip_32_melspect_scal(MFCC_Clip_32_T *Args);

void MFCC_ComputeDCT_II_Fix16(DCT_II_Arg_T *Args);
void MFCC_ComputeDCT_II_f32(DCT_II_Arg_T *Args);

void MFCC_Lifter_Fix16(Lifter_Arg_T *Args);
void MFCC_Lifter_f32(Lifter_Arg_T *Args);

int MFCC_Logfp(unsigned int a);

#ifdef __gap9__
void MelFilterBank_f16(MelFilterBank_T *Arg);
void MFCC_ComputeLog_f16(MFCC_Log_T *Arg);
void MFCC_ComputeDB_f16(MFCC_Log_T *Arg);
void MFCC_ComputeDCT_II_f16(DCT_II_Arg_T *Args);
void MFCC_Lifter_f16(Lifter_Arg_T *Args);
#endif
