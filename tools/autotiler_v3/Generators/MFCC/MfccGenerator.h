#include "Gap.h"
#include "MfccBasicKernels.h"

void LoadMFCCLibrary();
//void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt);
void MFCC_Generator(char *Name, CNN_GenControl_T *Ctrl,	int NFrames, int FrameSize,	int FrameStride, int Nfft, int NMFCCCoeff, int Ndct, float PreempFactor, int do_lifter, int use_radix_4_fft, int use_power, int high_prec_fft);
