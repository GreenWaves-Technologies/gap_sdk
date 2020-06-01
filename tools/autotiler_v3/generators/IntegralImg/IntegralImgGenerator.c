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

void LoadIntegralImageLibrary()
{
    LibKernel("KerPrime", CALL_PARALLEL,
            CArgs(2,
            TCArg("unsigned int * __restrict__", "KerBuffer"),
            TCArg("unsigned int", "W")
        ),
        "KerPrimeImage_ArgT",
        NULL
    );
    LibKernel("KerProcess", CALL_PARALLEL,
               CArgs(5,
            TCArg("unsigned char * __restrict__", "In"),
            TCArg("unsigned int", "W"),
            TCArg("unsigned int", "H"),
            TCArg("unsigned int * __restrict__", "IntegralImage"),
            TCArg("unsigned int * __restrict__", "KerBuffer")

        ),
        "KerProcessImage_ArgT",
        NULL
    );
}


void GenerateIntegralImage(char *Name,
        unsigned int W,     /* Image width */
        unsigned int H      /* Image Height */
    )
{
    UserKernel(AppendNames("Body", Name),
        KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
        TILE_HOR,
        CArgs(2,
            TCArg("unsigned char *  __restrict__",  "ImageIn"),
            TCArg("unsigned int *  __restrict__", "IntegralImage")
        ),
        Calls(2,
            Call("KerPrime", LOC_LOOP_PROLOG,
                Bindings(2,
                    K_Arg("KerBuffer",KER_ARG),
                    K_Arg("KerIn", KER_ARG_TILE_W)
                )
            ),
            Call("KerProcess", LOC_LOOP,
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
                KerArg("KerIn",    KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB,                                  W, H, sizeof(char), 0, 0, 0, "ImageIn"),
                KerArg("KerBuffer",KerArgSpace(1,KER_ITER_TILE0), O_BUFF| O_NDB | O_NOUT | O_NIN | O_NTILED, W, 1, sizeof(int),  0, 0, 0, ""),
                KerArg("KerOut",   KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB,                                 W, H, sizeof(int),  0, 0, 0, "IntegralImage")
            )
    );
}


void IntegralImageConfiguration(unsigned int L1Memory)

{
    SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
    SetSymbolDynamics();
    SetSymbolNames("II_L1_Memory", "II_L2_Memory");

    SetUsedFilesNames(0, 1, "IntegralImgBasicKernels.h");
    SetGeneratedFilesNames("IntegralImgKernels.c", "IntegralImgKernels.h");

    SetL1MemorySize(L1Memory);
}
