#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "nntool_extra_generators.h"

#ifdef CONSTINIT
#define Cinit(a, b, c, d, e) ConstInfo((a), (b), (c), (d), (e))
#else
#define Cinit(a, b, c, d, e) 0
#endif


void testModel(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    // SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);
    SetSymbolDynamics();
    SetUsedFilesNames(0, 1, "nntool_extra_kernels.h");
    SetGeneratedFilesNames("testKernels.c", "testKernels.h");

    SetMemoryDeviceInfos(4,
        AT_MEM_L1, L1Memory, "vww_L1_Memory", 0, 0,
        AT_MEM_L2, L2Memory, "vww_L2_Memory", 0, 0,
        AT_MEM_L3_HRAM, L3Memory, "vww_L3_Memory", 0, 0,
        AT_MEM_L3_HFLASH, L3Flash, "0", "vww_L3_Flash_Const.dat", 0
    );

    LoadNNTools_Extra_Library();
    CNN_Norm("testfunc1", 320, 240, 1, NNTOOL_KOP_RGB565);
    CNN_Norm("testfunc2", 320, 240, 0, NNTOOL_KOP_RGB565);
    CNN_Norm("testfunc3", 320, 240, 1, NNTOOL_KOP_RGB888);
    CNN_Norm("testfunc4", 320, 240, 0, NNTOOL_KOP_RGB888);
    CNN_Norm("testfunc5", 320, 240, 1, NNTOOL_KOP_BW);
    CNN_Norm("testfunc6", 320, 240, 0, NNTOOL_KOP_BW);
    CNN_Norm("testfunc7", 320, 240, 0, NNTOOL_KOP_BW16);
    CNN_Norm("testfunc8", 320, 240, 0, NNTOOL_KOP_RGB16);
}

int main(int argc, char **argv)

{
    if (TilerParseOptions(argc, argv)) {
            printf("Failed to initialize or incorrect output arguments directory.\\n"); return 1;
    }
    testModel(52000, 300*1024, 8*1024*1024, 20*1024*1024);
    GenerateTilingCode();
    return 0;
}

