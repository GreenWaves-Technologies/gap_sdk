#include "Gap.h"

enum DataType{
		FIX16,
		FIX32,
		FLOAT16,
		FLOAT32
};

void LoadMFCCLibrary();
void PieceWiseGenerator(char *Name, CNN_GenControl_T *Ctrl, char *FunName, int Dim, int DataType, int Inplace);

int MFCC_Generator(
	char *Name,		/* Name of the generated function */
	CNN_GenControl_T *Ctrl, /* Ctrl to override some feature, e.g. MfccLogOffset and PadType (Center pad) */
	int NFrames,		/* Number of windows */
	int FrameSize,		/* Size (in samples) of the windows */
	int FrameStride,	/* Stride (in samples) between two windows */
	int Nfft,		/* Number of fft bins */
	int NMelBanks,		/* Number of mel banks */
	int SizeMelCoeff,	/* Size of melbanks after sparsity (calculated by the filter generator python script) */
	int Ndct,		/* Number of DCT coeff: <= NMels */
	float PreempFactor,	/* Preemph factor */
	int NoWindow,		/* Not use any windowing function */
	float LifterCoeff,	/* Lifter coeff */
	int MagSquared,		/* 0: Compute MelBanks(|STFT|),  1: compute MelBanks(|STFT|^2) */
	int DataType, 		/* 0: Fixed point 16 bits, 1: Fixed point 32 (old version), 2: float16 (only for gap9), 3: float32 */
	int LogType, 		/* 0: Output melspect without applying log and dct, 1: Natural log, 2: 10log10 */
	int OutFFT 		/* If output FFT beside mel spect */
	);

int DSP_MatMul_Generator(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int ColM1,
	int LineM1,
	int ColM2,
	int LineM2,

	int TransposedIn2,
	int DataType
);

int IMel_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int Nfft,
	int NMelBanks,
	int SizeMelCoeff,
	int DataType
	);

int RFFT_2D_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	float PreempFactor,
	int SkipPreemp,
	int NoWindow,
	int OutFFT,
	int MagSquared,
	int DataType
	);

int IRFFT_2D_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int Nfft,
	int InvertWindow,
	int DataType
	);

void FFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int Nfft,
	int ForceRadix2,
	int DataType,
	int IntScal
	);

void IFFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int Nfft,
	int ForceRadix2,
	int DataType,
	int IntScal
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
