
#include <stdint.h>
#include "Gap.h"
#include "pulp_dsp/plp_math_extract.h"
#include "math_funcs.h"
#include "DSP_FloatType.h"
#include "FloatDefines.h"

#ifndef DSP_LIB_H
#define DSP_LIB_H

#define FFT_TWIDDLE_DYN 15
#define FFT4_SCALEDOWN 2
#define FFT2_SCALEDOWN 1

#ifndef AT_NORM
#define AT_NORM(x, n)	gap_norm_reg((x), (n))
#endif

#ifdef __pulp__
#define Abs(a)          __builtin_pulp_abs((a))
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)          (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif

typedef struct {
        void *__restrict__ FrameIn;
        void *__restrict__ FrameOut;
        int Nfft;
        short int ExtraQ;
        short int Input_QFormat;
        signed char *__restrict__ shift_fft;
} CmplxMag_T;

typedef struct {
  float r;
  float i;
} complex_fp;

typedef int cmplx __attribute__((vector_size (8)));

/** @brief Template for FFT/IFFT 1D on a  complex input
****
Template for FFT/IFFT 1D on a complex input
*/

typedef struct {
	void * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
	void * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
	unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
	unsigned int Inverse;         /**< FFT direction, direct or inverse */
} FFT_Arg_T;

typedef struct {
	void  * __restrict__ Data; /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
	void  * __restrict__ RFFT_Out; /**< Pointer to output */
	void  * __restrict__ Twiddles; /**< Pointer to fft twiddles */
	void  * __restrict__ RTwiddles; /**< Pointer to rfft twiddles */
	short * __restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
	unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
	unsigned int Inverse;         /**< FFT direction, direct or inverse */
} RFFT_Arg_T;

typedef struct {
	void * __restrict__ Data;	    /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
	void * __restrict__ Twiddles;	    /**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
	unsigned int N_fft;               /**< FFT dimension, has to be a power of 2 */
	unsigned int Inverse;             /**< FFT direction, direct or inverse */
	signed char *__restrict__ shift_fft;
} FFT_scal_Arg_T;

typedef struct {
	void  * __restrict__ Data; /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
	void  * __restrict__ RFFT_Out; /**< Pointer to output */
	void  * __restrict__ Twiddles; /**< Pointer to fft twiddles */
	void  * __restrict__ RTwiddles; /**< Pointer to rfft twiddles */
	short * __restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
	unsigned int N_fft;           /**< FFT dimension, has to be a power of 2 */
	unsigned int Inverse;         /**< FFT direction, direct or inverse */
	signed char *__restrict__ shift_fft;
	signed char *__restrict__ shift_rfft;
} RFFT_scal_Arg_T;

/** @brief Template for I,Q samples reordering (bit reverse to/from natural order)

Template for I,Q samples reordering (bit reverse to/from natural order)
*/
typedef struct {
        void *__restrict__ Data;        /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        short *__restrict__ SwapTable;  /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;                         /**< FFT dimension */
} SwapSamples_Arg_T;

typedef struct {
        void        *__restrict__ Data;         /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        signed char *__restrict__ shift_fft;
        short       *__restrict__ SwapTable;    /**< Pointer to lookup table for bit reverse to/from natural order reordering of Data */
        int Ni;                                 /**< FFT dimension */
} SwapSamples_scal_Arg_T;

/** @brief Template for twiddles and swap coefficients copy from L2 to shared L1.

Template for twiddles and swap coefficients copy from L2 to shared L1.

Should always be called before using one of the FFT routines when using it on the cluster, it will speedup operations.
All FFTs routines will also work safely if Twiddles and swap LUT are accessed directly in L2 memory.
*/
typedef struct {
        short int Nfft;                         /**< Size of the fft */
        short int Radix;                        /**< Radix of the fft, 2 or 4 */
        void      * __restrict__ Twiddles;      /**< Pointer to Twiddles in L2 */
        void      * __restrict__ RTwiddles;     /**< Pointer to RTwiddles in L2 */
        short int * __restrict__ SwapLUT;       /**< Pointer to Swap LUT table in L2 */
        void      * __restrict__ L1_Twiddles;   /**< Pointer to Twiddles copy in shared L1 */
        void      * __restrict__ L1_RTwiddles;  /**< Pointer to Twiddles copy in shared L1 */
        short int * __restrict__ L1_SwapLUT;    /**< Pointer to Swap LUT table copy in shared L1 */
} FFT_InstallArg_T;

typedef struct fbank_type_ {
	unsigned short int Start;
	unsigned short int Items;
	unsigned short int Base;
} fbank_type_t;

typedef struct {
        void *__restrict__ FramePower;
        void *__restrict__ MelSpectr;
        void *__restrict__ Mel_Coeffs;
        signed char *__restrict__ shift_buff;
        signed char *__restrict__ shift_fft;
        short int *__restrict__ Mel_FilterBank;
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
        int LogOffset;
} MFCC_Log_T;

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
        float LogOffset;
} MFCC_LogF_T;

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

typedef struct {
	void * __restrict__ Frame;
	void * __restrict__ Out;
	short int Prev;
	short int PreempFactor; 
	short int *Shift;
        short int QIn_FFT;
	int FrameSize;
	int maxin[8];
} PreEmphasis_T;

typedef struct {
        void * __restrict__ Frame;
        void * __restrict__ Out;
        F16_DSP Prev;
        F16_DSP PreempFactor;
        int FrameSize;
} PreEmphasis_f16_T;

typedef struct {
        void * __restrict__ Frame;
        void * __restrict__ Out;
        float Prev;
        float PreempFactor;
        int FrameSize;
} PreEmphasis_f32_T;

typedef struct {
	void *__restrict__ Frame;
	void *__restrict__ OutFrame;
	void *__restrict__ Window;
	int FrameSize;
	int FFT_Dim;
} Windowing_T;

typedef struct {
        void * __restrict__ In1;
        void * __restrict__ In2;
        void * __restrict__ Out;
        void *BufferColIn2;
        unsigned int W_In1;
        unsigned int H_In1;
        unsigned int W_In2;
        unsigned int W_Out;
        unsigned int OutFirstCol;
        int ColFirst;
        int Norm;
} MatMul_DSP_T;

/********************************************************************************************************************************************************************/
/****************** FFT Library  ************************************************************************************************************************************/
/********************************************************************************************************************************************************************/

/* Utils to Copy LUT from L2 to L1 */
extern void FFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format);
extern void RFFT_InstallTwiddlesAndSwapLUT(FFT_InstallArg_T *Arg, int format);

/* Sequential functions */
extern void Radix4FFT_DIF_Seq_Fix16(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft, int Inverse);
extern void Radix4FFT_DIF_Seq_f16  (F16_DSP *__restrict__ Data, F16_DSP *__restrict__ Twiddles, int N_fft, int Inverse);
extern void Radix4FFT_DIF_Seq_f32  (float *__restrict__ Data, float *__restrict__ Twiddles, int N_fft, int Inverse);
extern void Radix2FFT_DIF_Seq_Fix16(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_fft, int Inverse);
extern void Radix2FFT_DIF_Seq_f16  (F16_DSP *__restrict__ Data, F16_DSP *__restrict__ Twiddles, int N_fft, int Inverse);
extern void Radix2FFT_DIF_Seq_f32  (float *__restrict__ Data, float *__restrict__ Twiddles, int N_fft, int Inverse);

extern void SwapSamples_Seq_Fix16(short int *Data, short int *SwapTable, int Ni);
extern void SwapSamples_Seq_f16  (F16_DSP *Data, short int *SwapTable, int Ni);
extern void SwapSamples_Seq_f32  (float *Data, short int *SwapTable, int Ni);

extern void RFFT_DIF_Seq_Fix16(short int *Data, short int *RFFT_Out, short int *Twiddles, short int *RTwiddles, short int *SwapTable, int N_fft);
extern void RFFT_DIF_Seq_f16  (F16_DSP *Data, F16_DSP *RFFT_Out, F16_DSP *Twiddles, F16_DSP *RTwiddles, short int *SwapTable, int N_fft);
extern void RFFT_DIF_Seq_f32  (float *Data, float *RFFT_Out, float *Twiddles, float *RTwiddles, short int *SwapTable, int N_fft);

extern void IRFFT_DIF_Seq_Fix16(short int *Data, short int *RFFT_Out, short int *Twiddles, short int *RTwiddles, short int *SwapTable, int N_fft);
extern void IRFFT_DIF_Seq_f16  (F16_DSP *Data, F16_DSP *RFFT_Out, F16_DSP *Twiddles, F16_DSP *RTwiddles, short int *SwapTable, int N_fft);
extern void IRFFT_DIF_Seq_f32  (float *Data, float *RFFT_Out, float *Twiddles, float *RTwiddles, short int *SwapTable, int N_fft);

/* Parallel functions */
extern void Radix4FFT_DIF_Par_Fix16(FFT_Arg_T *Arg);
extern void Radix4FFT_DIF_Par_Fix32(FFT_Arg_T *Arg);
extern void Radix4FFT_DIF_Par_f16(FFT_Arg_T *Arg);
extern void Radix4FFT_DIF_Par_f32(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix16_Fast(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix16(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix32(FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_Fix32_Scal(FFT_scal_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_f32  (FFT_Arg_T *Arg);
extern void Radix2FFT_DIF_Par_f16(FFT_Arg_T *Arg);

extern void SwapSamples_Par(SwapSamples_Arg_T *Arg);
extern void SwapSamples_Par_f16(SwapSamples_Arg_T *Arg);
extern void SwapSamples_Par_f32(SwapSamples_Arg_T *Arg);
extern void SwapSamples_Par_Fix32(SwapSamples_Arg_T *Arg);
extern void SwapSamples_scal(SwapSamples_scal_Arg_T *Arg);

extern void RFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg);
extern void RFFT_DIF_Par_Fix32_Scal(RFFT_scal_Arg_T *Arg);
extern void RFFT_DIF_Par_f16(RFFT_Arg_T *Arg);
extern void RFFT_DIF_Par_f32(RFFT_Arg_T *Arg);

extern void IRFFT_DIF_Par_Fix16(RFFT_Arg_T *Arg);
extern void IRFFT_DIF_Par_f16(RFFT_Arg_T *Arg);
extern void IRFFT_DIF_Par_f32(RFFT_Arg_T *Arg);


/********************************************************************************************************************************************************************/
/****************** Cmplx Functions *********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

extern void CmplxMagSquared_Fix16(CmplxMag_T *Arg);
extern void CmplxMagSquared_Fix32_scal(CmplxMag_T *Arg);
extern void CmplxMagSquared_f32(CmplxMag_T *Arg);
extern void CmplxMag_Fix32(CmplxMag_T *Arg);
extern void CmplxMag_Fix32_scal(CmplxMag_T *Arg);
extern void CmplxMag_f32(CmplxMag_T *Arg);
extern void CmplxMagSquared_f16(CmplxMag_T *Arg);
extern void CmplxMag_f16(CmplxMag_T *Arg);

/********************************************************************************************************************************************************************/
/****************** Fast Float math *********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

float32_t ffast_cos(float32_t val);
float32_t ffast_sin(float32_t val);
float32_t ffast_sigmoid(float32_t val, float32_t alpha);
float32_t fsigmoid(float32_t val);

/********************************************************************************************************************************************************************/
/****************** Mfcc Basic Kern *********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

extern void MelFilterBank_Fix32(MelFilterBank_T *Arg);
extern void MelFilterBank_Fix32_Scal(MelFilterBank_T *Arg);
extern void MelFilterBank_f32(MelFilterBank_T *Arg);

extern void MFCC_ComputeLog_Fix32(MFCC_Log_T *Arg);
extern void MFCC_ComputeLog_Fix32_Scal(MFCC_Log_T *Arg);
extern void MFCC_ComputeLog_f32(MFCC_LogF_T *Arg);
extern void MFCC_ComputeDB_Fix32(MFCC_Log_T *Arg);
extern void MFCC_ComputeDB_Fix32_Scal(MFCC_Log_T *Arg);
extern void MFCC_ComputeDB_f32(MFCC_LogF_T *Arg);

extern void norm_clip_16(Norm_Clip_args_T *Args);
extern void norm_clip_32_melspect(MFCC_Clip_32_T *Args);
extern void norm_clip_32_melspect_scal(MFCC_Clip_32_T *Args);

extern void MFCC_ComputeDCT_II_Fix16(DCT_II_Arg_T *Args);
extern void MFCC_ComputeDCT_II_f32(DCT_II_Arg_T *Args);

extern void MFCC_Lifter_Fix16(Lifter_Arg_T *Args);
extern void MFCC_Lifter_f32(Lifter_Arg_T *Args);

int MFCC_Logfp(unsigned int a);

extern void MelFilterBank_f16(MelFilterBank_T *Arg);
extern void MFCC_ComputeLog_f16(MFCC_LogF_T *Arg);
extern void MFCC_ComputeDB_f16(MFCC_LogF_T *Arg);
extern void MFCC_ComputeDCT_II_f16(DCT_II_Arg_T *Args);
extern void MFCC_Lifter_f16(Lifter_Arg_T *Args);

extern void PreEmphasis(PreEmphasis_T *Arg);
extern void PreEmphasis_f32(PreEmphasis_f32_T *Arg);

extern void ZeroPad_Fix16(Windowing_T *Arg);
extern void ZeroPad_Fix32(Windowing_T *Arg);
extern void ZeroPad_f16(Windowing_T *Arg);
extern void ZeroPad_f32(Windowing_T *Arg);
extern void ZeroPadCenter_Fix16(Windowing_T *Arg);
extern void ZeroPadCenter_Fix32(Windowing_T *Arg);
extern void ZeroPadCenter_f16(Windowing_T *Arg);
extern void ZeroPadCenter_f32(Windowing_T *Arg);
extern void InvWindowing_Fix16(Windowing_T *Arg);
extern void WindowingReal2Cmplx_Fix16(Windowing_T *Arg);
extern void WindowingReal2Cmplx_Fix32(Windowing_T *Arg);
extern void WindowingReal2Cmplx_f32(Windowing_T *Arg);
extern void WindowingReal2Cmplx_PadCenter_Fix16(Windowing_T *Arg);
extern void WindowingReal2Cmplx_PadCenter_Fix32(Windowing_T *Arg);
extern void WindowingReal2Cmplx_PadCenter_f32(Windowing_T *Arg);
extern void WindowingReal2Real_Fix16(Windowing_T *Arg);
extern void WindowingReal2Real_Fix32(Windowing_T *Arg);
extern void WindowingReal2Real_f32(Windowing_T *Arg);
extern void WindowingReal2Real_PadCenter_Fix16(Windowing_T *Arg);
extern void WindowingReal2Real_PadCenter_Fix32(Windowing_T *Arg);
extern void WindowingReal2Real_PadCenter_f32(Windowing_T *Arg);
extern void PreEmphasis_f16(PreEmphasis_f16_T *Arg);
extern void WindowingReal2Cmplx_f16(Windowing_T *Arg);
extern void WindowingReal2Cmplx_PadCenter_f16(Windowing_T *Arg);
extern void WindowingReal2Real_f16(Windowing_T *Arg);
extern void WindowingReal2Real_PadCenter_f16(Windowing_T *Arg);

extern void KerParMatMulDSP_Fix16(MatMul_DSP_T *Arg);
extern void KerParMatMulDSPT_Fix16(MatMul_DSP_T *Arg);
extern void KerParMatMulDSP_fp16(MatMul_DSP_T *Arg);
extern void KerParMatMulDSPT_fp16(MatMul_DSP_T *Arg);
extern void KerParMatMulDSP_fp32(MatMul_DSP_T *Arg);
extern void KerParMatMulDSPT_fp32(MatMul_DSP_T *Arg);

#endif //DSP_LIB_H