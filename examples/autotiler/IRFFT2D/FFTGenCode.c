#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "DSP_Generators.h"

void FFTConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "DSP_Lib.h");
  SetGeneratedFilesNames("RFFTKernels.c", "RFFTKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
  	if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    FFTConfiguration(51200);
    // Load FIR basic kernels
    LoadMFCCLibrary();
    // Generate code for MFCC applied to N_FRAME of size FRAME_SIZE with FRAME_STEP as stride
    RFFT_2D_Generator("RFFT",   0, 49, N_FFT, 160, N_FFT, 0, 0, 0, 1, 0, DATA_TYPE);
    IRFFT_2D_Generator("IRFFT", 0, 49, N_FFT, 1, DATA_TYPE);
    GenerateTilingCode();
}
