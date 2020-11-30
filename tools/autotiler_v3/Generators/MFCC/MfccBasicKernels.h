#include "Gap.h"
#include "FFTLib.h"

#ifndef AT_NORM
#define AT_NORM(x, n)	gap_norm_reg((x), (n))
#endif

typedef struct mfcc_params {
  short int in_freq;
  short int min_freq;
  short int max_freq;
  short int frame;
  short int frame_step;
  short int n_frame;
  short int n_fft;
  short int numcep;
  short int nfeats;
  short int ndct;
  short int l_lifter;
  short int mfcc_bank_cnt;
} mfcc_params_t;

typedef struct fbank_type_ {
    short int Start;
    short int Stop;
    short int Base;
    short int Norm;
} fbank_type_t;

typedef struct {
  v2s *__restrict__ FrameIn;
  int *__restrict__ FrameOut;
  signed char *__restrict__ shift_fft;
  int nfft;
} MFCC_EP_T;

typedef struct {
  v2s *__restrict__ FrameIn;
  int *__restrict__ FrameOut;
  signed char *__restrict__ shift_fft;
  short int *Shift;
  int nfft;
} MFCC_EP_BFF_T;

typedef struct {
  v2s *__restrict__ FrameIn;
  int *__restrict__ FrameOut;
  int nfft;
} MFCC_Abs_T;

typedef struct {
  short int * __restrict__ Frame;
  short int * __restrict__ Out;
  short int Prev;
  short int PreempFactor; 
  short int *Shift;
  int FrameSize;
  int maxin[8];
} MFCC_PreEmphasis_T;

typedef struct {
  short int *__restrict__ Frame;
  short int *__restrict__ OutFrame;
  short int *__restrict__ Window;
  int FFT_Input_Dyn;
  int FrameSize;
  int FFT_Dim;
} MFCC_WF_T;

typedef struct {
  short int *__restrict__ Frame;
  int       *__restrict__ OutFrame;
  short int *__restrict__ Window;
  int FrameSize;
  int FFT_Dim;
} MFCC_WFINT_T;

typedef struct {
  unsigned int *__restrict__ FramePower;
  unsigned int *__restrict__ MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank;
  short int *__restrict__ MFCC_Coeffs;
  signed char *__restrict__ shift_pow;
  signed char *__restrict__ shift_BF;
  short int MFCC_bank_cnt;
} MFCC_MF_New_T;

typedef struct {
  unsigned int * __restrict__ FrameIn;
  unsigned int FrameSize;
  short int *Shift;
  short int fft_bits;
  short int MFCC_Coeff_dyn;
  signed char *__restrict__ shift_BF;
} MFCC_Log_T;

typedef struct {
  unsigned int *__restrict__ FramePower;
  unsigned int *__restrict__ MFCC;
  fbank_type_t *__restrict__ MFCC_FilterBank;
  short int *__restrict__ MFCC_Coeffs;
  short int MFCC_bank_cnt;
  short int MFCC_Coeff_dyn;
} MFCC_MF_T;

typedef struct {
  short int * __restrict__ Out;
  unsigned short int Norm;
  int N;
} Norm_Clip_args_T;

typedef struct {
  void * __restrict__ Data;	/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  void * __restrict__ Twiddles;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  void * __restrict__ Twidend;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15: DCT last stage */
  short int * __restrict__ SwapLuts;	/**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  short int * __restrict__ FeatList;	/**< Pointer to Feature list  */
  unsigned int n_dct; /**< FFT dimension, has to be a power of 2 */
  int numcep;
} DCT_Arg_T;

typedef struct {
  void * __restrict__ Data; /**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  short int * __restrict__ DCTCoeff;  /**< Pointer to fft twiddles (I, Q) pairs, I and Q fixed point Q15 */
  short int * __restrict__ FeatList;  /**< Pointer to Feature list  */
  unsigned int n_dct; /**< DCT dimension, doesn't has to be a power of 2 */
  int numcep;
} DCT_II_Arg_T;

typedef struct {
        int *__restrict__ Frame;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        int FrameSize;					/**< FFT dimension */
        int shift;					/**< scale dimension */
} preproc_dct_args_T;

typedef struct {
        int *__restrict__ Frame;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
        int FrameSize;					/**< frame dimension */
        int shift;					/**< scale dimension */
} postproc_dct_args_T;

typedef struct {
  short int *__restrict__  FeatList;			/**< Pointer to input data (I, Q) pairs, I and Q fixed point format */
  short int * __restrict__ LiftCoeff;
  int FrameSize;					/**< Frame dimension */
} Lifter_Arg_T;

void MFCC_PreEmphasis(MFCC_PreEmphasis_T *Arg);
void MFCC_WindowedFrame(MFCC_WF_T *Arg);
void MFCC_WindowedFrame_int(MFCC_WFINT_T *Arg);
void MFCC_PowerV2S(MFCC_EP_T *Arg);
void MFCC_Power(MFCC_EP_T *Arg);
void MFCC_Abs_BFF(MFCC_EP_BFF_T *Arg);
void MFCC_Abs(MFCC_EP_T *Arg);
void MFCC_ComputeMFCC(MFCC_MF_T *Arg);
void MFCC_ComputeMFCC_BFF(MFCC_MF_New_T *Arg);
void MFCC_ComputeDCT(DCT_Arg_T *Args) ;
void norm_clip_16(Norm_Clip_args_T *Args);
int MFCC_Logfp(unsigned int a);
void MFCC_ComputeLog( MFCC_Log_T *Arg) ;
void MFCC_ComputeDCT_II(DCT_II_Arg_T *Args);
