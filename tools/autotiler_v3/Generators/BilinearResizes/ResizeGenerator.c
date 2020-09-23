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
#include "ResizeGenerator.h"

void LoadResizeLibrary()
{
	LibKernel("KerResizeBilinear", CALL_PARALLEL,
		CArgs(8,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("unsigned char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeBilinear_ArgT",
		NULL
	);
	LibKernel("KerResizeNearestNeighbor", CALL_PARALLEL,
		CArgs(8,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("unsigned char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeNearestNeighbor_ArgT",
		NULL
	);
}

void GenerateResizeNew(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout, resize_kop_t Type)

{
	char *ResizeKerName;
	switch (Type){
		case KOP_BILINEAR_RESIZE:
			ResizeKerName = "KerResizeBilinear";
			break;
		case KOP_NEAREST_NEIGHBOR_RESIZE:
			ResizeKerName = "KerResizeNearestNeighbor";
			break;
		default:
			ResizeKerName = "KerResizeBilinear";
	}
	printf("%s\n", ResizeKerName);
	UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2, TCArg("unsigned char *", "In"), TCArg("unsigned char *", "Out")),
		Calls(1, Call(ResizeKerName, LOC_LOOP,
			Bindings(8, K_Arg("In", KER_ARG_TILE),
				        K_Arg("In", KER_ARG_W),
				        K_Arg("In", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_W),
				        K_Arg("Out", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("In", KER_ARG_TILE_BASE)))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB,  Win,  Hin,  sizeof(char), 1, OBJ_CONSTRAINTS_DYNAMIC, 0, "In"),
			KerArg("Out", KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB, Wout, Hout, sizeof(char), 0, OBJ_CONSTRAINTS_DYNAMIC, 0, "Out")
		)
	);

}


void ResizeConfiguration(unsigned int L1Memory)

{
	SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
	SetSymbolNames("Resize_L1_Memory", "Resize_L2_Memory");
	SetSymbolDynamics();
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

	SetUsedFilesNames(0, 1, "ResizeBasicKernels.h");
	SetGeneratedFilesNames("ResizeKernels.c", "ResizeKernels.h");

	SetL1MemorySize(L1Memory);
}
