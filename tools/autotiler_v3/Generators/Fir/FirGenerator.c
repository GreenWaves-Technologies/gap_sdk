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


extern Object_T *KerArgPad(
	char 	     *KerArgName,	/**< Kernel argument name */
	KernelArgDimDescrT *KerArgSpace,/**< Kernel argument space descriptor */
	Object_Type_T ObjType,		/**< Kernel argument type: logical OR of types (O_xxx) or pre defined types */
	unsigned int W,			/**< Kernel argument Data plane width */
	unsigned int H,			/**< Kernel argument Data plane height */
	unsigned int BottomBuffer,	/**< Amount of buffer to be added before this kernel argument, unit is ItemSize */
	unsigned int TopBuffer,		/**< Amount of buffer to be added after this kernel argument, unit is ItemSize */
	unsigned int ItemSize,		/**< Data plane basic data type size in bytes */
	int TileOverlap,		/**< Amount of overlap between 2 adjacent tiles */
	KernelArgConstraints_T Constraint, /**< Kernel argument constraints */
	unsigned int PreferedTileSize,  /**< Tile variable dimension is prefered to a multiple of PreferedTileSize if not 0 */
	char *CArgName			/**< To which user kernel C argument this kernel argument is related to */
	);


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
		"KerFirParallel_ArgT",NULL
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
		"KerFirParallel_ArgT",NULL
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
		"KerFirParallel_ArgT",NULL
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
		"KerFirParallel_ArgT",NULL
	);
	LibKernel("KerFirNTaps_f16", CALL_PARALLEL,
		CArgs(6,
			TCArg("F16_DSP * __restrict__", "In"),
			TCArg("F16_DSP * __restrict__", "NextIn"),
			TCArg("F16_DSP * __restrict__", "Coeffs"),
			TCArg("F16_DSP * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs")
		),
		"KerFirNTaps_f16_ArgT",NULL
	);

	LibKernel("KerFirNTaps_f32", CALL_PARALLEL,
		CArgs(6,
			TCArg("float * __restrict__", "In"),
			TCArg("float * __restrict__", "NextIn"),
			TCArg("float * __restrict__", "Coeffs"),
			TCArg("float * __restrict__", "Out"),
			TCArg("unsigned int", "NSamples"),
			TCArg("unsigned int", "NCoeffs")
		),
		"KerFirNTaps_f32_ArgT",NULL
	);

	LibKernel("KerFirParallelInit", CALL_PARALLEL,
		CArgs(2,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned int", "NCoeffs")
		),
		"KerFirParallelInit_ArgT",NULL
	);	

	LibKernel("KerFirNTapsInit_f16", CALL_PARALLEL,
		CArgs(2,
			TCArg("F16_DSP * __restrict__", "In"),
			TCArg("unsigned int", "NCoeffs")
		),
		"KerFirNTapsInit_f16_ArgT",NULL
	);

	LibKernel("KerFirNTapsInit_f32", CALL_PARALLEL,
		CArgs(2,
			TCArg("float * __restrict__", "In"),
			TCArg("unsigned int", "NCoeffs")
		),
		"KerFirNTapsInit_f32_ArgT",NULL
	);

	
}

void GeneratorFIR(char *Name, unsigned int NSamples, unsigned int NCoeffs, unsigned int Generic, unsigned int Scalar)

{
	char *KerFirName;
	char *KerFirInitName = "KerFirParallelInit";
	Kernel_T *Kernel;
	
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

	//Try not Tiled solution
	//This is done to avoid KER_ARG_TILE_NEXTDB to throw an error in case no tiling is needed
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Kernel=UserKernel(Name,
		   //KernelDimensions(0, 1, 1, 0),
		   //KernelIterationOrder(1, KER_TILE),
		   KernelIterSpace(1, IterTiledSpace(KER_TILE)),
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
		Calls(2,
			Call(KerFirInitName,LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(7,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(0),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					Imm(NSamples),
					Imm(NCoeffs),
					C_Arg("Norm")
				)
			)
		),
		KerArgs(3,
			KerArgPad("KerIn",     KerArgSpace(1,KER_TILE),O_IN|O_NTILED,        1, NSamples, NCoeffs, 0, sizeof(short int), 0, 0, 0, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE),O_IN|O_NTILED|O_BUFF, 1, NCoeffs ,             sizeof(short int), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut",    KerArgSpace(1,KER_TILE),O_OUT|O_NTILED,       1, NSamples,             sizeof(short int), 0, 0, 0, "Out")
			)
	  
	);
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	//Try tiled solution
	if (Kernel == 0) {

			Kernel=UserKernel(Name,
		   //KernelDimensions(0, 1, 1, 0),
		   //KernelIterationOrder(1, KER_TILE),
		   KernelIterSpace(1, IterTiledSpace(KER_TILE)),
                TILE_HOR,
                CArgs(4,
                        TCArg("short int *  __restrict__", "In"),
                        TCArg("short int *  __restrict__", "Coeffs"),
                        TCArg("short int *  __restrict__", "Out"),
                        TCArg("unsigned int", "Norm")
                ),
		Calls(2,
			Call(KerFirInitName,LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(7,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_NEXTDB),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(NCoeffs),
					C_Arg("Norm")
				)
			)
		),
		KerArgs(3,
			KerArgPad("KerIn",  KerArgSpace(1,KER_TILE),   OBJ_IN_DB,            1, NSamples, NCoeffs, 0, sizeof(short int), 0, 0, NCoeffs, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE),O_IN|O_NTILED|O_BUFF, 1, NCoeffs ,             sizeof(short int), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut", KerArgSpace(1,KER_TILE),   OBJ_OUT_DB,           1, NSamples,             sizeof(short int), 0, 0, 0, "Out")
			)
	  
	);

	}
}


void GeneratorFIR_fp16(char *Name, unsigned int NSamples, unsigned int NCoeffs)
{
	char *KerFirName;
	Kernel_T *Kernel;

	KerFirName = "KerFirNTaps_f16";

	if(NCoeffs>NSamples)
		GenTilingError("Error Ncoeffs (%d) can't be bigger that NSamples (%d)\n", NCoeffs,NSamples);
	//printf("Generating Fir Float 16 NSamples: %d - NCoeffs: %d\n",NSamples,NCoeffs);

	//Try not Tiled solution
	//This is done to avoid KER_ARG_TILE_NEXTDB to throw an error in case no tiling is needed
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Kernel= UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(KER_TILE)),
                TILE_HOR,
                CArgs(3,
                        TCArg("F16_DSP *  __restrict__", "In"),
                        TCArg("F16_DSP *  __restrict__", "Coeffs"),
                        TCArg("F16_DSP *  __restrict__", "Out")
                ),

		Calls(2,
			Call("KerFirNTapsInit_f16",LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(0),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					Imm(NSamples),
					Imm(NCoeffs)
				)
			)
		),
		KerArgs(3,
			KerArgPad("KerIn",     KerArgSpace(1,KER_TILE), O_IN|O_NTILED,          1, NSamples, NCoeffs, 0, sizeof(short int), 0, 0, 0, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE), O_IN|O_NTILED|O_BUFF,   1, NCoeffs ,             sizeof(short int), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut",    KerArgSpace(1,KER_TILE), O_OUT|O_NTILED,         1, NSamples,             sizeof(short int), 0, 0, 0, "Out")
			)	  
	);

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	if (Kernel == 0) {
		//Try tiled solution
		UserKernel(Name,
				KernelIterSpace(1, IterTiledSpace(KER_TILE)),
                TILE_HOR,
                CArgs(3,
                        TCArg("F16_DSP *  __restrict__", "In"),
                        TCArg("F16_DSP *  __restrict__", "Coeffs"),
                        TCArg("F16_DSP *  __restrict__", "Out")
                ),

		Calls(2,
			Call("KerFirNTapsInit_f16",LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_NEXTDB),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(NCoeffs)
				)
			)
		),
		KerArgs(3,/*F16_DSP does not exist on X86 compiler so use sizeof(short int)*/
			KerArgPad("KerIn",     KerArgSpace(1,KER_TILE),    OBJ_IN_DB,           1, NSamples, NCoeffs, 0, sizeof(short int), 0, 0, NCoeffs, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE), O_IN|O_NTILED|O_BUFF,   1, NCoeffs ,             sizeof(short int), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut",    KerArgSpace(1,KER_TILE),    OBJ_OUT_DB,          1, NSamples,             sizeof(short int), 0, 0, 0, "Out")
			)	  
		);
	}

}


void GeneratorFIR_fp32(char *Name, unsigned int NSamples, unsigned int NCoeffs)
{
	char *KerFirName;
	Kernel_T *Kernel;
	KerFirName = "KerFirNTaps_f32";
	if(NCoeffs>NSamples)
		GenTilingError("Error Ncoeffs (%d) can't be bigger that NSamples (%d)\n", NCoeffs,NSamples);
	//Try not Tiled solution
	//This is done to avoid KER_ARG_TILE_NEXTDB to throw an error in case no tiling is needed
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Kernel=UserKernel(Name,
		 KernelIterSpace(1, IterTiledSpace(KER_TILE)),
                TILE_HOR,
                CArgs(3,
                        TCArg("float *  __restrict__", "In"),
                        TCArg("float *  __restrict__", "Coeffs"),
                        TCArg("float *  __restrict__", "Out")
                ),

		Calls(2,
			Call("KerFirNTapsInit_f32",LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(0),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					Imm(NSamples),
					Imm(NCoeffs)
				)
			)
		),
		KerArgs(3,
			KerArgPad("KerIn",     KerArgSpace(1,KER_TILE), O_IN|O_NTILED,          1, NSamples, NCoeffs, 0, sizeof(float), 0, 0, 0, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE), O_IN|O_NTILED|O_BUFF,   1, NCoeffs ,             sizeof(float), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut",    KerArgSpace(1,KER_TILE), O_OUT|O_NTILED,         1, NSamples,             sizeof(float), 0, 0, 0, "Out")
			)	  
	);

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	if (Kernel == 0) {
		//Try tiled solution
		UserKernel(Name,
		 KernelIterSpace(1, IterTiledSpace(KER_TILE)),
                TILE_HOR,
                CArgs(3,
                        TCArg("float *  __restrict__", "In"),
                        TCArg("float *  __restrict__", "Coeffs"),
                        TCArg("float *  __restrict__", "Out")
                ),

		Calls(2,
			Call("KerFirNTapsInit_f32",LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerIn", KER_ARG_TILE),
					Imm(NCoeffs)
					)
				),
			Call(KerFirName, LOC_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_NEXTDB),
					K_Arg("KerCoeffs", KER_ARG_TILE),
					K_Arg("KerOut", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(NCoeffs)
				)
			)
		),
		KerArgs(3,
			KerArgPad("KerIn",     KerArgSpace(1,KER_TILE), OBJ_IN_DB,            1, NSamples, NCoeffs, 0, sizeof(float), 0, 0, NCoeffs, "In"),
			KerArg   ("KerCoeffs", KerArgSpace(1,KER_TILE), O_IN|O_NTILED|O_BUFF, 1, NCoeffs ,             sizeof(float), 0, 0, 0, "Coeffs"),
			KerArg   ("KerOut",    KerArgSpace(1,KER_TILE), OBJ_OUT_DB,           1, NSamples,             sizeof(float), 0, 0, 0, "Out")
			)	  
		);
	}

}