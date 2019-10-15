/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

// The auto tiles generates code for a fixed size of model.
// This defines the matrix size used for the addition
#define MAT_W 100
#define MAT_H 100

// And this defines how much memory we will give the model in shared cluster L1 memory
#define L1_MEMORY_SIZE 45000


void MatAddConfiguration(unsigned int L1Memory);
void LoadMatAddLibrary();
void GenerateMatAddPar(char *UserKernelName, int W, int H);
