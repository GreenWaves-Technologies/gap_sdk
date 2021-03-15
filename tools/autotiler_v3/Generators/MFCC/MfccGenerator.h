#include "Gap.h"
#include "MfccBasicKernels.h"

enum DataType{
        FIX16,
        FIX32,
        FLOAT16,
        FLOAT32
};

void LoadMFCCLibrary();
//void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt);
void MFCC_Generator(
	char *Name,				 /* Node Name */
	CNN_GenControl_T *Ctrl,	 /* Ctrl to override some arg */
	int NFrames,			 /* Number of frames in the input */
	int FrameSize,			 /* Size (in samples) of each single frame */
	int FrameStride,		 /* Step (in samples) between two frames */
	int Nfft,				 /* Number of FFT bins (must be 2^k and greater than FrameSize) */
	int NMFCCCoeff,			 /* Number of MFCC coefficients (MFCC_COEFF_CNT output by the GenLut.py script) */
	int Ndct,				 /* Number of DCT bins and MFCC filters, if 0 will be output the LogMelSpectrogram with MfccBankCnt MFCC filters */
	float PreempFactor,		 /* PreEmphasis factor */
	int LifterCoeff,		 /* Lifter coefficient */
	int use_radix_4_fft,	 /* If 1, using fft mixed radix (Not fully tested) */
	int use_power,			 /* If 1, the MelFilterBank will be applied to |STFT|^2, otherwise |STFT| */
	int DataType,			 /* If 1, uses fake floatng point arithmetic for FFT and MFCC */
	int MfccBankCnt,		 /* If !=0, the number of MFCC filters */
	int OutMelspectrogram	 /* If 1, the output of the function will be the output of the MelFilter Bank */
	);
void STFT_Generator(
	char *Name,				 /* Node Name */
	CNN_GenControl_T *Ctrl,	 /* Ctrl to override some arg */
	int NFrames,			 /* Number of frames in the input */
	int FrameSize,			 /* Size (in samples) of each single frame */
	int FrameStride,		 /* Step (in samples) between two frames */
	int Nfft,				 /* Number of FFT bins (must be 2^k and greater than FrameSize) */
	float PreempFactor,		 /* PreEmphasis factor */
	int use_radix_4_fft,	 /* If 1, using fft mixed radix (Not fully tested) */
	int out_fft,			 /* If 1, do not apply any operation to FFT complex output */
	int use_power,			 /* If 1, the MelFilterBank will be applied to |STFT|^2, otherwise |STFT| */
	int DataType			 /* If 1, uses fake floatng point arithmetic for FFT and MFCC */
	);
void ISTFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	int use_radix_4_fft,
	int DataType
	);
