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

void LoadFIRLibrary()

{
	LibKernel("KerFirParallelScalarNTaps", CALL_PARALLEL,
		CArgs(7,
			TCArg("short int * __restrict__", "In"),
			TCArg("short int * __restrict__", "NextIn"),
			TCArg("short int * __restrict__", "Coeffs"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs"),
			TCArg("unsigned int", "Norm")
		),
		"KerFirParallel_ArgT"
	);
	LibKernel("KerFirParallelNTaps", CALL_PARALLEL,
		CArgs(7,
			TCArg("short int * __restrict__", "In"),
			TCArg("short int * __restrict__", "NextIn"),
			TCArg("short int * __restrict__", "Coeffs"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs"),
			TCArg("unsigned int", "Norm")
		),
		"KerFirParallel_ArgT"
	);
	LibKernel("KerFirParallel20Taps", CALL_PARALLEL,
		CArgs(7,
			TCArg("short int * __restrict__", "In"),
			TCArg("short int * __restrict__", "NextIn"),
			TCArg("short int * __restrict__", "Coeffs"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs"),
			TCArg("unsigned int", "Norm")
		),
		"KerFirParallel_ArgT"
	);
	LibKernel("KerFirParallel10Taps", CALL_PARALLEL,
		CArgs(7,
			TCArg("short int * __restrict__", "In"),
			TCArg("short int * __restrict__", "NextIn"),
			TCArg("short int * __restrict__", "Coeffs"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs"),
			TCArg("unsigned int", "Norm")
		),
		"KerFirParallel_ArgT"
	);
}

void GeneratorFIR(char *Name, unsigned int NSamples, unsigned int NCoeffs, unsigned int Generic, unsigned int Scalar)

{
	char *KerFirName;

	if (!Generic) {
		switch (NCoeffs) {
/*
			// In case other specilized variants are needed for small number of Coefficients
			case 4: KerFirName = "KerFirParallel4Taps"; break;
			case 6: KerFirName = "KerFirParallel6Taps"; break;
			case 8: KerFirName = "KerFirParallel8Taps"; break;
*/
			case 10: KerFirName = "KerFirParallel10Taps"; break;
			case 20: KerFirName = "KerFirParallel20Taps"; break;
			default: KerFirName = "KerFirParallelNTaps";
		}
	} else {
		if (Scalar) KerFirName = "KerFirParallelScalarNTaps";
		else KerFirName = "KerFirParallelNTaps";
	}

	UserKernel(Name,
                KernelDimensions(0, 1, 1, 0),
                KernelIterationOrder(1, KER_TILE),
                TILE_HOR,
                CArgs(4,
                        TCArg("short int *  __restrict__", "In"),
                        TCArg("short int *  __restrict__", "Coeffs"),
                        TCArg("short int *  __restrict__", "Out"),
                        TCArg("unsigned int", "Norm")
                ),
/** @brief
	Based on actual number of Coefficients for the FIR filter we select which basic kernel to be used, for
small number of coefficients we have specially optimized basic kernels. To force generic the usage of generic
implementations set Generic argument to 1. If Generic is selected you can also use Scalar to force the usage
of a scalar implementation instead of a vectorial one.

	Inputs and outputs are double buffered to make sur that the computing kernels
are always kept buzzy. Input and output are objects of width 1 and height NSamples, tiling is performed horizontally.

	We need a delay line capability for the input, this delay line should be able to store NCoeffs value. The
basic kernel assumes that this delay line should be placed right before the input, alltogether a single chunck of
memory of size NSample+NCoeffs of short int. To achieve this we use the KerArgPad() construct instead of the usual
KerArg() one where extra fixed size padding before or after the user kernel argument can be delcared, in this case
we need extra padding before the input hence the NCoeefs, 0 part.

	The second parameter of the FIR kernel, NextIn, should point to the next double buffer, this is why we use
the KER_ARG_TILE_NEXTDB property in the call sequence.
*/
		Calls(1,
			Call(KerFirName, LOC_INNER_LOOP,
				Bindings(7,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_NEXTDB),
					C_Arg("Coeffs"),
					K_Arg("KerOut", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(NCoeffs),
					C_Arg("Norm")
				)
			)
		),
		KerArgs(2,
			KerArgPad("KerIn",      OBJ_IN_DB,          1, NSamples, NCoeffs, 0, sizeof(short int), 0, 0, 0, "In",  0),
			KerArg   ("KerOut",     OBJ_OUT_DB,         1, NSamples,             sizeof(short int), 0, 0, 0, "Out", 0)
		)
	);
}


void FIRConfiguration(unsigned int L1Memory)

{
        SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
		SetSymbolNames("FIR_L1_Memory", "FIR_L2_Memory", "FIR_KernelDescr", "FIR_KernelArgs");
        SetSymbolDynamics();

        SetUsedFilesNames(0, 1, "FirBasicKernels.h");
        SetGeneratedFilesNames("FirKernelsInit.c", "FirKernelsInit.h", "FirKernels.c", "FirKernels.h");

        SetL1MemorySize(L1Memory);
}
