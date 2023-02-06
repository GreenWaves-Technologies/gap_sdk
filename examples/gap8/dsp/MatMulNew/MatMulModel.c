#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "DSP_Generators.h"

void MatMulConfiguration(unsigned int L1Memory)
{
  SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

  SetUsedFilesNames(0, 1, "DSP_Lib.h");
  SetGeneratedFilesNames("MatMulKernels.c", "MatMulKernels.h");

  SetL1MemorySize(L1Memory);
}

int main(int argc, char **argv)
{
  	if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");

    // Set Auto Tiler configuration, given shared L1 memory is 51200
    MatMulConfiguration(112*1024);
    // Load FIR basic kernels
    LoadMFCCLibrary();

    // Generate code for MatMul applied to 49 of size FRAME_SIZE with FRAME_STEP as stride
    DSP_MatMul_Generator("MatMul_Fix16", 0, W_M1, H_M1, W_M2, W_M1, 0, FIX16);
    DSP_MatMul_Generator("MatMul_fp16" , 0, W_M1, H_M1, W_M2, W_M1, 0, FLOAT16);
    DSP_MatMul_Generator("MatMul_fp32" , 0, W_M1, H_M1, W_M2, W_M1, 0, FLOAT32);

    GenerateTilingCode();
}
