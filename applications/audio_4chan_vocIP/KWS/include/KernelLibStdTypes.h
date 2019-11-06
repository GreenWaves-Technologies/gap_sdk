/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef _KERNEL_STD_ARG_TYPE_H_
#define _KERNEL_STD_ARG_TYPE_H_
#include "Gap8.h"
#include "StdTypes.h"
typedef struct {
        int DimRatio;
        unsigned int L2Offset;
        unsigned short int L1Offset;
        unsigned short int FixDim[2];
        unsigned short int VarDim[2];
        unsigned short int MoveSize[2];
        unsigned short int MoveStride;
        unsigned short int Length2D[2];
        unsigned short int Stride2D;
        unsigned short int BufferSize;
        unsigned short int BuffCount;
} Kernel_Arg_T;
typedef struct {
        unsigned char Iteration[3];
        unsigned char ArgCount;
        unsigned char NinPlanes;
        unsigned char NoutPlanes;
        Kernel_Arg_T *Arg;
} Kernel_T;
#endif