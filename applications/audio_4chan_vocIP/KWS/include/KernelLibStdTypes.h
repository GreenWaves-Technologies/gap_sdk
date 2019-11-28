/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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