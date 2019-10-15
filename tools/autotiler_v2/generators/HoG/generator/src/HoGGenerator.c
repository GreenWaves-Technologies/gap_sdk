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

void LoadHOGLibrary()

{
	LibKernel("KerProcessCellLine", CALL_PARALLEL,
               CArgs(5,
			TCArg("uint8_t * __restrict__", "In"),
			TCArg("uint32_t", "W"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint32_t", "CellLineIndex"),
			TCArg("uint32_t", "CellLineCount")

		),
		"KerProcessCellLine_ArgT"
	);
	LibKernel("KerProcessBlockLine", CALL_PARALLEL,
		CArgs(3,
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint32_t", "W"),
			TCArg("uint16_t * __restrict__", "HOGFeatures")
		),
		"KerProcessBlockLine_ArgT"
	);
}


void GenerateHOG(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H, 		/* Image Height */
		unsigned int CellSize, 		/* Cell is [CellSize x CellSize] pixels */
		unsigned int BlockSize, 	/* Block is [BlockSize x BlockSize] Cells */
		unsigned int BlockOverlap, 	/* Number Cells in common between 2 adjacent blocks */
		unsigned int BinsPerCell)	/* Number of bins for a cell */

{
/*
	(H-2)/CellSize = BlockSize + Kh*(BlockSize-BlockOverlap)
		=> Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap)
		=> Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2

	Kh + 1 is the number of HOG feature blocks in the vertical direction
	Ha is the adjusted height of the image in order to have an a number of lines in the image that exactly gives Kh+1
	HOG features in the vertical direction.

	(W-2)/CellSize = BlockSize + Kw*(BlockSize-BlockOverlap)
		=> Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap)
		=> Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2

	Kw + 1 is the number of HOG feature blocks in the horizontal direction
	Wa is the adjusted width of the image in order to have an a number of lines in the image that exactly gives Kw+1
	HOG features in the horizontal direction.
*/
	unsigned int Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	//if ((W & 0x3) != 0) GenTilingError("Image width must be a multiple of 4, Actual: %d\n", W);

	OpenKernelGroup(Name);
	UserKernel(AppendNames("Prime", Name),
		KernelDimensions(0, W, CellSize*BlockSize+2, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_HOR,
		CArgs(3,
			TCArg("uint8_t *  __restrict__",  "ImageIn"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures")
		),
		Calls(2,
			Call("KerProcessCellLine", LOC_INNER_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_TILEINDEX),
					Imm(1)
				)
			),
			Call("KerProcessBlockLine", LOC_INNER_LOOP_EPILOG,
				Bindings(3,
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_W),
					K_Arg("KerFeatOut", KER_ARG_TILE)
				)
			)
		),
                KerArgs(2,
                        KerArg("KerIn",      OBJ_IN_DB,             W,    BlockSize*CellSize+2, sizeof(char),
		  	       2, OBJ_CONSTRAINTS_ONEPREFTILE, CellSize, "ImageIn", 0),
                        KerArg("KerFeatOut", OBJ_BUFFER_OUT_NTILED, 1+Kw, 1,                    BlockSize*BlockSize*BinsPerCell*sizeof(short),
			       0, 0, 0, "HOGFeatures", 0)
                )
	);
	UserKernel(AppendNames("Body", Name),
		KernelDimensions(0, W, H-CellSize*BlockSize, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_HOR,
		CArgs(3,
			TCArg("uint8_t *  __restrict__",  "ImageIn"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures")
		),
		Calls(2,
			Call("KerProcessCellLine", LOC_INNER_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					C_Arg("CellLines"),
					Imm(BlockSize),
					Imm(BlockSize-BlockOverlap)
				)
			),
			Call("KerProcessBlockLine", LOC_INNER_LOOP,
				Bindings(3,
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_W),
					K_Arg("KerFeatOut", KER_ARG_TILE)
				)
			)
		),
                KerArgs(2,
                        KerArg("KerIn",      OBJ_IN_DB,  W,    Ha - BlockSize*CellSize, sizeof(char),
		  	       	2, OBJ_CONSTRAINTS_ONEPREFTILE, (BlockSize-BlockOverlap)*CellSize, "ImageIn", 0),
                        KerArg("KerFeatOut", OBJ_OUT_DB, 1+Kw, Kh,                      BlockSize*BlockSize*BinsPerCell*sizeof(short),
			       0, 0, 0, "HOGFeatures", 0)
                )
	);
	CloseKernelGroup();
	UserKernelGroup(Name,
		CArgs(3,
			TCArg("uint8_t *  __restrict__", "ImageIn"),
			TCArg("uint16_t **  __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures")
		),
		Calls(2,
			UserKernelCall(AppendNames("Prime", Name), LOC_GROUP,
				Bindings(3, C_Arg("ImageIn"), C_Arg("CellLines"), C_Arg("HOGFeatures"))
			),
			UserKernelCall(AppendNames("Body", Name), LOC_GROUP,
				Bindings(3,
					C_ArgPlusImmOffset("ImageIn", W*BlockSize*CellSize),
					C_Arg("CellLines"),
					C_ArgPlusImmOffset("HOGFeatures", (1+Kw)*BlockSize*BlockSize*BinsPerCell)
				)
			)
		)
	);
}

void HOGConfiguration(unsigned int L1Memory)

{
    SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
	SetSymbolNames("HOG_L1_Memory", "HOG_L2_Memory", "HOG_KernelDescr", "HOG_KernelArgs");
    SetSymbolDynamics();
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

    SetUsedFilesNames(0, 1, "HoGBasicKernels.h");
    SetGeneratedFilesNames("HoGKernelsInit.c", "HoGKernelsInit.h", "HoGKernels.c", "HoGKernels.h");

    SetL1MemorySize(L1Memory);
}
