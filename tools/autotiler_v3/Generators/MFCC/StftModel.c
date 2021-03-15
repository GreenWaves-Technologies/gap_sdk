#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "MfccGenerator.h"
#include "STFT_params.h"

void STFTConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "MfccBasicKernels.h");
  SetGeneratedFilesNames("STFTKernels.c", "STFTKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
  	if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    STFTConfiguration(51200);
    // Load FIR basic kernels
    LoadMFCCLibrary();
    // Generate code for MFCC applied to N_FRAME of size FRAME_SIZE with FRAME_STEP as stride
    STFT_Generator("STFT",        0, N_FRAME, FRAME_SIZE, FRAME_STEP, N_FFT, PREEMP_FACTOR, USE_RADIX_4, OUT_FFT, USE_POWER, DATA_TYPE);
    // Generate code for MFCC applied to a single frame
    STFT_Generator("STFT_single", 0, 1,       FRAME_SIZE, FRAME_STEP, N_FFT, PREEMP_FACTOR, USE_RADIX_4, OUT_FFT, USE_POWER, DATA_TYPE);
    GenerateTilingCode();
}
