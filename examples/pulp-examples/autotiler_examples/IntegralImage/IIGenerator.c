/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "StdTypes.h"

void LoadIntegralImageLibrary()
{
	LibKernel("KerPrime", CALL_PARALLEL,
            CArgs(2,
            TCArg("Wordu32 * __restrict__", "KerBuffer"),
			TCArg("Wordu32", "W")
		),
		"KerPrimeImage_ArgT"
	);
	LibKernel("KerProcess", CALL_PARALLEL,
               CArgs(5,
			TCArg("Wordu8 * __restrict__", "In"),
			TCArg("Wordu32", "W"),
			TCArg("Wordu32", "H"),
			TCArg("Wordu32 * __restrict__", "IntegralImage"),
			TCArg("Wordu32 * __restrict__", "KerBuffer")

		),
		"KerProcessImage_ArgT"
	);

}


void GenerateIntegralImage(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H 		/* Image Height */
	)
{

	
	UserKernel(AppendNames("Body", Name),
		KernelDimensions(0, W, H, 0),
		KernelIterationOrder(1, KER_TILE),
		TILE_HOR,
		CArgs(2,
			TCArg("Wordu8 *  __restrict__",  "ImageIn"),
			TCArg("Wordu32 *  __restrict__", "IntegralImage")
		),
		Calls(2,
			Call("KerPrime", LOC_INNER_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerBuffer",KER_ARG),
					K_Arg("KerIn", KER_ARG_TILE_W)
				)
			),
			Call("KerProcess", LOC_INNER_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					K_Arg("KerIn", KER_ARG_TILE_H),
					K_Arg("KerOut",KER_ARG_TILE),
					K_Arg("KerBuffer",KER_ARG)
				)
			)
		),
			KerArgs(3,
                KerArg("KerIn",     OBJ_IN_DB,           W,  H, sizeof(char), 0, 0, 0, "ImageIn", 0),
                KerArg("KerBuffer", O_BUFF | O_NDB | O_NOUT | O_NIN | O_NTILED ,   W,  1, sizeof(int),  0, 0, 0, 0, 0),
                KerArg("KerOut",    OBJ_OUT_DB,          W,  H, sizeof(int),  0, 0, 0, "IntegralImage", 0) 
            )
	);
}

void IntegralImageConfiguration(unsigned int L1Memory)

{
    SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
	SetSymbolNames("II_L1_Memory", "II_L2_Memory", "II_KernelDescr", "II_KernelArgs");
    SetSymbolDynamics();
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

    SetUsedFilesNames("KernelLibStdTypes.h", 1, "IIBasicKernels.h");
    SetGeneratedFilesNames("IIKernelsInit.c", "IIKernelsInit.h", "IIKernels.c", "IIKernels.h");

    SetL1MemorySize(L1Memory);
}
