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
#include "MatAddGenerator.h"

void LoadMatAddLibrary()
{
	//
	LibKernel("MatSumPar", CALL_PARALLEL,
        CArgs(5,
			TCArg("int * __restrict__", "In1"),
			TCArg("int * __restrict__", "In2"),
			TCArg("int * __restrict__", "Out"),

			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H")
		),
		"MatrixAdd_Arg_T"

	);

}

void GenerateMatAddPar(char *UserKernelName, int W, int H)
{
  UserKernel(UserKernelName,
    // 2D kernel
    KernelDimensions(1, W, H, 1),
    // Iteration space is 2D
    KernelIterationOrder(KER_DIM2, KER_TILE),
    // Tile horizontally
    TILE_HOR,
    // C template for this user kernel
    CArgs(3,
      TCArg("int * __restrict__", "In1"),
      TCArg("int * __restrict__", "In2"),
      TCArg("int * __restrict__", "Out")
    ),
    // Basic kernel we want to call on tiles, in this case MatSumPar
    Calls(1,
      Call("MatSumPar", LOC_INNER_LOOP,
        Bindings(5,
          K_Arg("KerIn1", KER_ARG_TILE),   // A tile from In1
          K_Arg("KerIn2", KER_ARG_TILE),   // A tile from In2
          K_Arg("KerOut", KER_ARG_TILE),   // A tile from Out
          // Tile(Out) = Tile(In1) + Tile(In2)

          K_Arg("KerIn1", KER_ARG_TILE_W), // Tile width
          K_Arg("KerIn1", KER_ARG_TILE_H)  // Tile height
        )
      )
    ),
    // All 3 user kernel arguments are of dimension WxH with
    // basic type = plain int
    KerArgs(3,
    // First kernel argument, bound to In1. In1 in L2, double buffered access
    KerArg("KerIn1", OBJ_IN_DB,  W, H,  sizeof(int), 0, 0, 0, "In1", 0),
    // Second kernel argument, bound to In2. In2 in L2, double buffered access
    KerArg("KerIn2", OBJ_IN_DB,  W, H,  sizeof(int), 0, 0, 0, "In2", 0),
    // Third kernel argument, bound to Out. Out in L2, double buffered access
    KerArg("KerOut", OBJ_OUT_DB, W, H,  sizeof(int), 0, 0, 0, "Out", 0)
    )
  );
}

void MatAddConfiguration(unsigned int L1Memory)
{
    // Inline the kernel
    SetInlineMode(ALWAYS_INLINE);

    // Dynamically allocate memory
    SetSymbolDynamics();

    // Define the names that we want to create
    SetUsedFilesNames(0, 1, "MatAddBasicKernels.h");
    SetGeneratedFilesNames("MatAddKernelsInit.c", "MatAddKernelsInit.h", "MatAddKernels.c", "MatAddKernels.h");

    // Set the shared L1 memory available
    SetL1MemorySize(L1Memory);
}
