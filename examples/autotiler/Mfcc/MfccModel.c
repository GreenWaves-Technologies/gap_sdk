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
    CNN_GenControl_T Librosa_Settings;
    CNN_InitGenCtrl(&Librosa_Settings);
    CNN_SetGenCtrl(&Librosa_Settings, "PADTYPE", AT_OPT_VAL(3)); // Pad Balanced Right == Center pad of librosa

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MFCCConfiguration(112*1024);
    // Load FIR basic kernels
    LoadMFCCLibrary();

    // Generate code for MFCC applied to 49 of size FRAME_SIZE with FRAME_STEP as stride
    MFCC_Generator("Librosa_MFCC",                    &Librosa_Settings, 49, FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, DATA_TYPE, 2, 0);

    // Generate code for MFCC applied to a single frame just for code generation testing
    MFCC_Generator("Librosa_MFCC_single_Fix16",       &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 0, 2, 0);
    MFCC_Generator("Librosa_MFCC_single_Fix16_FFT",   &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 0, 2, 1);
    MFCC_Generator("Librosa_LogMel_single_Fix16",     &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 0, 2, 0);
    MFCC_Generator("Librosa_LogMel_single_Fix16_FFT", &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 0, 2, 1);

    MFCC_Generator("Librosa_MFCC_single_Fix32",       &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 1, 2, 0);
    MFCC_Generator("Librosa_MFCC_single_Fix32_FFT",   &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 1, 2, 1);
    MFCC_Generator("Librosa_LogMel_single_Fix32",     &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 1, 2, 0);
    MFCC_Generator("Librosa_LogMel_single_Fix32_FFT", &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 1, 2, 1);

    MFCC_Generator("Librosa_MFCC_single_f16",         &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 2, 2, 0);
    MFCC_Generator("Librosa_MFCC_single_f16_FFT",     &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 2, 2, 1);
    MFCC_Generator("Librosa_LogMel_single_f16",       &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 2, 2, 0);
    MFCC_Generator("Librosa_LogMel_single_f16_FFT",   &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 2, 2, 1);

    MFCC_Generator("Librosa_MFCC_single_f32",         &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 3, 2, 0);
    MFCC_Generator("Librosa_MFCC_single_f32_FFT",     &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, N_DCT, 0, 0, 0, USE_POWER, 3, 2, 1);
    MFCC_Generator("Librosa_LogMel_single_f32",       &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 3, 2, 0);
    MFCC_Generator("Librosa_LogMel_single_f32_FFT",   &Librosa_Settings, 1,  FRAME_SIZE, FRAME_STEP, N_FFT, MFCC_BANK_CNT, MFCC_COEFF_CNT, 0,     0, 0, 0, USE_POWER, 3, 2, 1);
    GenerateTilingCode();
}
