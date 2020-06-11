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
#include "Gap.h"

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

void LoadMatMultLibrary()

{
    LibKernel("KerMatMultParallelf32", CALL_PARALLEL,
        CArgs(9,
            TCArg("float * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("float * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("float * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("float * __restrict__", "BufferColIn2") 
        ),
        "KerMatMultParallelf32_ArgT", 0
    );
    LibKernel("KerMatMultParallelf16", CALL_PARALLEL,
        CArgs(9,
            TCArg("f16 * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("f16 * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("f16 * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("f16 * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallelf16_ArgT", 0
    );
    LibKernel("KerMatMultParallelVectorialf16", CALL_PARALLEL,
        CArgs(9,
            TCArg("f16 * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("f16 * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("f16 * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("f16 * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallelf16_ArgT", 0
    );

    LibKernel("KerMatMultParallel32", CALL_PARALLEL,
        CArgs(10,
            TCArg("int * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("int * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("int * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("int * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallel32_ArgT", 0
    );
    LibKernel("KerMatMultParallel16", CALL_PARALLEL,
        CArgs(10,
            TCArg("short int * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("short int * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("short int * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("short int * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallel16_ArgT", 0
    );

    LibKernel("KerMatMultParallelVectorial16", CALL_PARALLEL,
        CArgs(10,
            TCArg("short int * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("short int * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("short int * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("short int * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallel16_ArgT", 0
    );

    LibKernel("KerMatMultParallel8", CALL_PARALLEL,
        CArgs(10,
            TCArg("signed char * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("signed char * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("signed char * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("signed char * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallel8_ArgT", 0
    );

    LibKernel("KerMatMultParallelVectorial8", CALL_PARALLEL,
        CArgs(10,
            TCArg("signed char * __restrict__", "In1"),
            TCArg("unsigned int", "W_In1"),
            TCArg("unsigned int", "H_In1"),
            TCArg("signed char * __restrict__", "In2"),
            TCArg("unsigned int", "W_In2"),
            TCArg("signed char * __restrict__", "Out"),
            TCArg("unsigned int", "W_Out"),
            TCArg("unsigned int", "OutFirstCol"),
            TCArg("signed char * __restrict__", "BufferColIn2"),
            TCArg("unsigned int", "Norm")
        ),
        "KerMatMultParallel8_ArgT", 0
    );
}

void ParMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2, int Type)

{
    unsigned int LineO = LineM1, ColO = ColM2;
    char *DataType = 0;
    char *NameExt = 0;
    char *MatKer = 0;
    int DataSize = 0;
    int NeedNorm = 0;
    switch (Type) {
        case 0: /* byte */
            DataType = "signed char * __restrict__";
            DataSize = 1;
            NameExt = "fixedp8";
            MatKer = "KerMatMultParallel8";
            NeedNorm = 1;
            break;
        case 1: /* short int */
            DataType = "short int * __restrict__";
            DataSize = 2;
            NameExt = "fixedp16";
            MatKer = "KerMatMultParallel16";
            NeedNorm = 1;
            break;
        case 2: /* int */
            DataType = "int * __restrict__";
            DataSize = 4;
            NameExt = "fixedp32";
            MatKer = "KerMatMultParallel32";
            NeedNorm = 1;
            break;
        #ifdef __gap9__
        case 3: /* fp16 */
            DataType = "f16 * __restrict__";
            DataSize = 2;
            NameExt = "float16";
            MatKer = "KerMatMultParallelf16";
            NeedNorm = 0;
            break;
        
        case 4: /* fp32 */
            DataType = "float * __restrict__";
            DataSize = 4;
            NameExt = "float32";
            MatKer = "KerMatMultParallelf32";
            NeedNorm = 0;
            break;
        #endif
    }

    if (ColM1 != LineM2) GenTilingError("Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", LineM1, ColM1, LineM2, ColM2);
    UserKernel(Name,
        KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
        NeedNorm?
                CArgs(4,
                        TCArg(DataType, "M1"),
                        TCArg(DataType, "M2"),
                        TCArg(DataType, "Out"),
            TCArg("unsigned int", "Norm")
                ):
                CArgs(3,
                        TCArg(DataType, "M1"),
                        TCArg(DataType, "M2"),
                        TCArg(DataType, "Out")
                ),
        Calls(1,
            Call(MatKer, LOC_LOOP,
                Bindings(NeedNorm?10:9,
                    K_Arg("M1",  KER_ARG_TILE), K_Arg("M1",  KER_ARG_TILE_W), K_Arg("M1",  KER_ARG_TILE_H),
                    K_Arg("M2",  KER_ARG_TILE), K_Arg("M2",  KER_ARG_TILE_W),
                    K_Arg("Out", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE_W), K_Arg("M2",  KER_ARG_TILE_BASE),
                    AT_IGNORE_ARG_BINDING, // Imm(0),
                    NeedNorm?C_Arg("Norm"):AT_IGNORE_ARG_BINDING
                )
            )
        ),
        KerArgs(3,
            KerArg("M1", KerArgSpace(1, T1),  O_IN|O_DB,  ColM1, LineM1, DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "M1"),
            KerArg("M2", KerArgSpace(1, T0),  O_IN|O_DB,  ColM2, LineM2, DataSize, 0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "M2"),
            KerArg("Out",KerArgSpace(1, T1),  O_OUT|O_DB, ColO,  LineO,  DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out")
        )
    );
}

void ParVectMatMultGenerator(char *Name, unsigned int LineM1, unsigned int ColM1, unsigned int LineM2, unsigned int ColM2, int Type)

{
    unsigned int LineO = LineM1, ColO = ColM2;
    char *DataType = 0;
    char *NameExt = 0;
    char *MatKer = 0;
    int DataSize = 0;
    int NeedNorm = 0;
    switch (Type) {
        case 0: /* byte */
            DataType = "signed char * __restrict__";
            DataSize = 1;
            NameExt = "fixedp8";
            MatKer = "KerMatMultParallelVectorial8";
            NeedNorm = 1;
            break;
        case 1: /* short int */
            DataType = "short int * __restrict__";
            DataSize = 2;
            NameExt = "fixedp16";
            MatKer = "KerMatMultParallelVectorial16";
            NeedNorm = 1;
            break;
        #ifdef __gap9__
        case 2: /* f16 */
            DataType = "f16 * __restrict__";
            DataSize = 2;
            NameExt = "float16";
            MatKer = "KerMatMultParallelVectorialf16";
            break;
        #endif
    }

    if (ColM1 != LineM2) GenTilingError("Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", LineM1, ColM1, LineM2, ColM2);
    UserKernel(Name,
        KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
        NeedNorm?
                CArgs(4,
                        TCArg(DataType, "M1"),
                        TCArg(DataType, "M2"),
                        TCArg(DataType, "Out"),
            TCArg("unsigned int", "Norm")
                ):
                CArgs(3,
                        TCArg(DataType, "M1"),
                        TCArg(DataType, "M2"),
                        TCArg(DataType, "Out")
                ),
        Calls(1,
            Call(MatKer, LOC_LOOP,
                Bindings(NeedNorm?10:9,
                    K_Arg("M1",  KER_ARG_TILE),    K_Arg("M1",  KER_ARG_TILE_W), K_Arg("M1",  KER_ARG_TILE_H),
                    K_Arg("M2",  KER_ARG_TILE),    K_Arg("M2",  KER_ARG_TILE_W),
                    K_Arg("Out", KER_ARG_TILE),    K_Arg("Out", KER_ARG_TILE_W), K_Arg("M2",  KER_ARG_TILE_BASE),
                    K_Arg("KerBuff", KER_ARG_TILE),NeedNorm?C_Arg("Norm"):AT_IGNORE_ARG_BINDING
                )
            )
        ),
        KerArgs(4,
            KerArg("KerBuff",KerArgSpace(1, T0), O_BUFF|O_NTILED, LineM1, 1,      DataSize, 0, 0,                                                0, 0),
            KerArg("M1",     KerArgSpace(1, T1), O_IN|O_DB,       ColM1,  LineM1, DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "M1"),
            KerArg("M2",     KerArgSpace(1, T0), O_IN|O_DB,       ColM2,  LineM2, DataSize, 0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "M2"),
            KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,      ColO,   LineO,  DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out")
        )
    );
}


