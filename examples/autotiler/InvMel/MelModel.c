#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "DSP_Generators.h"
#include "MEL_PARAMS.h"

void MelConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "DSP_Lib.h");
  SetGeneratedFilesNames("MelKernels.c", "MelKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
  	if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MelConfiguration(224*1024);
    // Load FIR basic kernels
    LoadMFCCLibrary();

    // Generate code for MFCC applied to 49 of size FRAME_SIZE with FRAME_STEP as stride
    MFCC_Generator("Librosa_Mel",  0, 49, FRAME_SIZE, FRAME_STEP, N_FFT, N_MELS, MEL_COEFF_CNT, 0, 0, 0, 0, 0, DATA_TYPE, 0, 0);
    DSP_MatMul_Generator("Librosa_IMel", 0, N_MELS, 49, N_FFT/2+1, N_MELS, 0, DATA_TYPE);

    GenerateTilingCode();
}
