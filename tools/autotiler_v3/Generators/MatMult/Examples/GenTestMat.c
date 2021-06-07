#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "MatMultGenerator.h"

void Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory)


{
        SetSymbolDynamics();

        SetUsedFilesNames(0, 1, "MatMultBasicKernels.h");
        SetGeneratedFilesNames("Kernels.c", "Kernels.h");
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

        SetMemoryDeviceInfos(4,
                AT_MEM_L1, L1Memory, "L1_Memory", 0, 0,
                AT_MEM_L2, L2Memory, "L2_Memory", 0, 0,
                AT_MEM_L3_HRAM, L3Memory, "L3_Memory", 0, 0,
                AT_MEM_L3_HFLASH, 20*1024*1024, "0", "L3_Flash_Const.dat", 0
        );

        LoadMatMultLibrary();

	ParMatMultGenerator    ("ParMultfix8",         200, 100, 100, 150, 0);		/* Byte fixed point data */
        ParVectMatMultGenerator("ParMultVectfix8",     200, 100, 100, 150, 3);         /* Byte fixed point data, vectorized */
        ParVectMatMultTransGenerator("ParMultVect_Transfix8",  200, 100, 100, 150, 0);         /* Byte fixed point data, vectorized, the second input must be transposed, experimental */
	ParMatMultGenerator    ("ParMultfix16",        200, 100, 100, 150, 1);		/* Short fixed point data */
	ParVectMatMultGenerator("ParMultVectfix16",    200, 100, 100, 150, 1);		/* Short fixed point data, vectorized */
	ParMatMultGenerator    ("ParMultfix32",        200, 100, 100, 150, 2);		/* int fixed point data */
#ifdef WITH_FLOAT16
	ParMatMultGenerator    ("ParMultfloat16",      200, 100, 100, 150, 3);		/* Float 16 data */
	ParVectMatMultGenerator("ParMultVectfloat16",  200, 100, 100, 150, 2);		/* Float 16 data, vectorized */
#endif
	ParMatMultGenerator    ("ParMultfloat32",      200, 100, 100, 150, 4);		/* Float 32 data */
}

int main(int argc, char **argv)

{
        if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
        Model(54200, 360*1024, 8*1024*1024);
        GenerateTilingCode();
        return 0;
}


