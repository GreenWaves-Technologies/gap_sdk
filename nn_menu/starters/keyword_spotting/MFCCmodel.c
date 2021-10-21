#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "DSP_Generators.h"
#include "MFCC_params.h"

void MFCCConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
  SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "DSP_Lib.h");
  SetGeneratedFilesNames("MFCCKernels.c", "MFCCKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
    if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MFCCConfiguration(64*1024);
    // Load FIR basic kernels
    LoadMFCCLibrary();
    MFCC_Generator("MFCC", 0, 49, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, DATA_TYPE, 1, 0);
    GenerateTilingCode();
}
