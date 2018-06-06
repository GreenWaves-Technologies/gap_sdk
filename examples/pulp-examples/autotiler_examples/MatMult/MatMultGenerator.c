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

void LoadLibrary()

{
	LibKernel("KerMatMultParallel", CALL_PARALLEL,
		CArgs(9,
			TCArg("short int * __restrict__", "In1"),
			TCArg("unsigned int", "W_In1"),
			TCArg("unsigned int", "H_In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("unsigned int", "W_In2"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "W_Out"),
			TCArg("unsigned int", "OutFirstCol"),
			TCArg("short int * __restrict__", "BufferColIn2")
		),
		"KerMatMultParallel_ArgT"
	);

	LibKernel("KerMatMultParallelVectorial", CALL_PARALLEL,
		CArgs(9,
			TCArg("short int * __restrict__", "In1"),
			TCArg("unsigned int", "W_In1"),
			TCArg("unsigned int", "H_In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("unsigned int", "W_In2"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned int", "W_Out"),
			TCArg("unsigned int", "OutFirstCol"),
			TCArg("short int * __restrict__", "BufferColIn2")
		),
		"KerMatMultParallel_ArgT"
	);
}

void ParMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2)
{
	unsigned int LineO = LineM1, ColO = ColM2;

	if (ColM1 != LineM2) GenTilingError("Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", LineM1, ColM1, LineM2, ColM2);
	UserKernel(Name,
                0,
                KernelIterationOrder(2, KER_TILE1, KER_TILE),
                TILE_HOR,
                CArgs(3,
                        TCArg("Word16 *  __restrict__", "M1"),
                        TCArg("Word16 *  __restrict__", "M2"),
                        TCArg("Word16 *  __restrict__", "Out")
                ),
		Calls(1,
			Call("KerMatMultParallel", LOC_INNER_LOOP,
				Bindings(9,
					K_Arg("KerM1",  KER_ARG_TILE),    K_Arg("KerM1",  KER_ARG_TILE_W), K_Arg("KerM1",  KER_ARG_TILE_H),
					K_Arg("KerM2",  KER_ARG_TILE),    K_Arg("KerM2",  KER_ARG_TILE_W),
					K_Arg("KerOut", KER_ARG_TILE),    K_Arg("KerOut", KER_ARG_TILE_W), K_Arg("KerM2",  KER_ARG_TILE_BASE),
					Imm(0)
				)
			)
		),
		KerArgs(3,
			KerArg("KerM1",  OBJ_IN_DB|O_TILE1,  ColM1, LineM1, sizeof(Word16), 0, 0,                        0, "M1",  0),
			KerArg("KerM2",  OBJ_IN_DB,  ColM2, LineM2, sizeof(Word16), 0, OBJ_CONSTRAINTS_TILE_VER, 0, "M2",  0),
			KerArg("KerOut", OBJ_OUT_DB|O_TILE1, ColO,  LineO,  sizeof(Word16), 0, 0,                        0, "Out", 0)
		)
	);
}

void ParVectMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2)

{
	unsigned int LineO = LineM1, ColO = ColM2;

	if (ColM1 != LineM2) GenTilingError("Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", LineM1, ColM1, LineM2, ColM2);
	UserKernel(Name,
                0,
                KernelIterationOrder(2, KER_TILE1, KER_TILE),
                TILE_HOR,
                CArgs(3,
                        TCArg("Word16 *  __restrict__", "M1"),
                        TCArg("Word16 *  __restrict__", "M2"),
                        TCArg("Word16 *  __restrict__", "Out")
                ),
		Calls(1,
			Call("KerMatMultParallelVectorial", LOC_INNER_LOOP,
				Bindings(9,
					K_Arg("KerM1",  KER_ARG_TILE),    K_Arg("KerM1",  KER_ARG_TILE_W), K_Arg("KerM1",  KER_ARG_TILE_H),
					K_Arg("KerM2",  KER_ARG_TILE),    K_Arg("KerM2",  KER_ARG_TILE_W),
					K_Arg("KerOut", KER_ARG_TILE),    K_Arg("KerOut", KER_ARG_TILE_W), K_Arg("KerM2",  KER_ARG_TILE_BASE),
          			K_Arg("KerBuff", KER_ARG)
				)
			)
		),
		KerArgs(4,
			KerArg("KerBuff",OBJ_BUFFER_NTILED,  LineM2, 1,     sizeof(Word16), 0, 0,                                                0, "",    0),
			KerArg("KerM1",  OBJ_IN_DB|O_TILE1,  ColM1, LineM1, sizeof(Word16), 0, 0,                          0, "M1",  0),
			KerArg("KerM2",  OBJ_IN_DB,          ColM2, LineM2, sizeof(Word16), 0, OBJ_CONSTRAINTS_TILE_VER, 0, "M2",  0),
			KerArg("KerOut", OBJ_OUT_DB|O_TILE1, ColO,  LineO,  sizeof(Word16), 0, 0,                          0, "Out", 0)
		)
	);
}

void Model(unsigned int L1Memory)
{
	SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
	SetSymbolDynamics();

	SetUsedFilesNames("KernelLibStdTypes.h", 1, "MatMultBasicKernels.h");
	SetGeneratedFilesNames("ModelKernelsInit.c", "ModelKernelsInit.h", "ModelKernels.c", "ModelKernels.h");
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);


	SetL1MemorySize(L1Memory);

	LoadLibrary();

	ParMatMultGenerator    ("ParMatMult",     200, 70, 70, 150);
	ParVectMatMultGenerator("ParVectMatMult", 200, 70, 70, 150);
}

int main(int argc, char **argv)

{
        if (TilerParseOptions(argc, argv)) GenTilingError("Failed to initialize or incorrect output arguments directory.\n");
        Model(40000);
        GenerateTilingCode();
        return 0;
}


