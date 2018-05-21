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


void LoadFFT2DLibrary()

{
        LibKernel("Image2Complex", CALL_PARALLEL,
		  CArgs(4,
			TCArg("Wordu8 * __restrict__", "In"),
			TCArg("Word32 * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
		       ),
		  "Image2Complex_Arg_T"
	);
        LibKernel("Radix4FFT_DIF_2D_Horizontal", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix4FFT_DIF_2D_Vertical", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix4FFT_DIF_2D_Scalar_Horizontal", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix4FFT_DIF_2D_Scalar_Vertical", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix2FFT_DIF_2D_Horizontal", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix2FFT_DIF_2D_Vertical", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix2FFT_DIF_2D_Scalar_Horizontal", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("Radix2FFT_DIF_2D_Scalar_Vertical", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("Word16 * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "Twiddles"),
                         TCArg("unsigned short int", "N_fft"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "FFT_2D_Arg_T"
        );
        LibKernel("SwapSamples_2D_Horizontal_Par", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("v2s * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "SwapTable"),
                         TCArg("unsigned short int", "Ni"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "SwapSamples_2D_Arg_T"
        );
        LibKernel("SwapSamples_2D_Vertical_Par", CALL_PARALLEL,
                   CArgs(4,
                         TCArg("v2s * __restrict__", "Data"),
                         TCArg("Word16 * __restrict__", "SwapTable"),
                         TCArg("unsigned short int", "Ni"),
                         TCArg("unsigned short int", "Nb")
                        ),
                   "SwapSamples_2D_Arg_T"
        );
}

void FFT2D(char *Name, unsigned int Dim, int ForceRadix2, int Scalar)

{
	char *KerHorizontal, *KerVertical;

	if (__builtin_popcount(Dim) != 1) GenTilingError("FFT2D: %s, Incorrect Dim: %d, it has to be a a power of 2", Name, Dim);
	else if (!ForceRadix2 && ((__builtin_ctz(Dim)%2)==0)) {
		if (Scalar) {
			KerHorizontal = "Radix4FFT_DIF_2D_Scalar_Horizontal";
			KerVertical = "Radix4FFT_DIF_2D_Scalar_Vertical";
		} else {
			KerHorizontal = "Radix4FFT_DIF_2D_Horizontal";
			KerVertical = "Radix4FFT_DIF_2D_Vertical";
		}
	} else {
		if (Scalar) {
			KerHorizontal = "Radix2FFT_DIF_2D_Scalar_Horizontal";
			KerVertical = "Radix2FFT_DIF_2D_Scalar_Vertical";
		} else {
			KerHorizontal = "Radix2FFT_DIF_2D_Horizontal";
			KerVertical = "Radix2FFT_DIF_2D_Vertical";
		}
	}

	OpenKernelGroup(Name);
	UserKernel(AppendNames(Name, "Horizontal"),
		KernelDimensions(1, Dim, Dim, 1),
		KernelIterationOrder(KER_DIM2, KER_TILE),
                TILE_HOR,
		CArgs(4,
			TCArg("Wordu8 * __restrict__", "In"),
			TCArg("v2s * __restrict__", "Out"),
			TCArg("Word16 * __restrict__", "Twiddles"),
			TCArg("Word16 * __restrict__", "SwapTable")
		),
		Calls(3,
			Call("Image2Complex", LOC_INNER_LOOP,
				Bindings(4, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H))),
			Call(KerHorizontal, LOC_INNER_LOOP,
				Bindings(4, K_Arg("Out", KER_ARG_TILE), C_Arg("Twiddles"), K_Arg("Out", KER_ARG_TILE_W), K_Arg("Out", KER_ARG_TILE_H))),
			Call("SwapSamples_2D_Horizontal_Par", LOC_INNER_LOOP,
				Bindings(4, K_Arg("Out", KER_ARG_TILE), C_Arg("SwapTable"), K_Arg("Out", KER_ARG_TILE_W), K_Arg("Out", KER_ARG_TILE_H)))
		),
		KerArgs(2,
			KerArg("In",  OBJ_IN_DB,  Dim, Dim, sizeof(Wordu8),  0, 0, 0, "In", 0),
			KerArg("Out", OBJ_OUT_DB, Dim, Dim, sizeof(Word32),  0, 0, 8, "Out", 0)
		)

	);
	UserKernel(AppendNames(Name, "Vertical"),
		KernelDimensions(1, Dim, Dim, 1),
		KernelIterationOrder(KER_DIM2, KER_TILE),
                TILE_VER,
		CArgs(3,
			TCArg("Word16 * __restrict__", "InOut"),
			TCArg("Word16 * __restrict__", "Twiddles"),
			TCArg("Word16 * __restrict__", "SwapTable")
		),
		Calls(2,
			Call(KerVertical, LOC_INNER_LOOP,
				Bindings(4, K_Arg("InOut", KER_ARG_TILE), C_Arg("Twiddles"), K_Arg("InOut", KER_ARG_TILE_H), K_Arg("InOut", KER_ARG_TILE_W))),
			Call("SwapSamples_2D_Vertical_Par", LOC_INNER_LOOP,
				Bindings(4, K_Arg("InOut", KER_ARG_TILE), C_Arg("SwapTable"), K_Arg("InOut", KER_ARG_TILE_H), K_Arg("InOut", KER_ARG_TILE_W)))
		),
		KerArgs(1,
			KerArg("InOut", OBJ_IN_OUT_DB, Dim, Dim, sizeof(Word32), 0, 0, 8, "InOut", 0)
		)

	);
	CloseKernelGroup();

	UserKernelGroup(Name,
		CArgs(4,
			TCArg("Wordu8 * __restrict__", "In"),
			TCArg("Word32 * __restrict__", "Out"),
			TCArg("Word16 * __restrict__", "Twiddles"),
			TCArg("Word16 * __restrict__", "SwapTable")
		),
		Calls(2,
			UserKernelCall(AppendNames(Name, "Horizontal"), LOC_GROUP,
				       Bindings(4, C_Arg("In"), C_Arg("Out"), C_Arg("Twiddles"), C_Arg("SwapTable"))),
			UserKernelCall(AppendNames(Name, "Vertical"),   LOC_GROUP,
				       Bindings(3, C_Arg("Out"), C_Arg("Twiddles"), C_Arg("SwapTable")))
		)
	);
}

void FFT2DModel(unsigned int L1Memory)

{

        // SetInlineMode(SINGLE_INLINE);
        // SetInlineMode(NEVER_INLINE);
        SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

        SetUsedFilesNames("KernelLibStdTypes.h", 1, "FFT_Lib.h");
        SetGeneratedFilesNames("FFT2D_Init.c", "FFT2D_Init.h", "FFT2D.c", "FFT2D.h");

        SetL1MemorySize(L1Memory);
	LoadFFT2DLibrary();

	printf("\n");

	FFT2D("FFT2D_256", 256, 0, 0);
	FFT2D("FFT2D_128", 128, 0, 0);
}

int main(int argc, char **argv)

{
        if (TilerParseOptions(argc, argv)) {
                printf("Failed to initialize or incorrect output directory.\n"); return 0;
        }
        FFT2DModel(51200);
        GenerateTilingCode();
        return 0;
}


