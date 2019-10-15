/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include <stdio.h>
#include "Gap8.h"
#include "MatAddBasicKernels.h"

// optimize the division to find the chunk size
// equivalent to ceil(KerArg0->W/rt_nb_pe())
inline static unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
    unsigned int NCore = rt_nb_pe();
    unsigned int Log2Core = gap8_fl1(NCore);
    unsigned int Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
    return Chunk;
}

// The actual code that does the tile addition
void MatSumPar(MatrixAdd_Arg_T  *KerArg0 ){
    // Work out which vertical strip of the tile we need to do
    unsigned int CoreId = gap8_coreid();
    unsigned int Chunk = ChunkSize(KerArg0->W);
    unsigned int First = CoreId*Chunk;
    unsigned int Last  = (First+Chunk > KerArg0->W) ? (KerArg0->W) : (First+Chunk);

    for (unsigned int i = 0; i<KerArg0->H; ++i)
	{
    	for (unsigned int j=First; j<Last; j++){
    		KerArg0->Out[(i*KerArg0->W)+j] = KerArg0->In1[(i*KerArg0->W)+j] + KerArg0->In2[(i*KerArg0->W)+j];
    	}
    }
    // Wait for the other cores to complete the tile
	rt_team_barrier();
}
