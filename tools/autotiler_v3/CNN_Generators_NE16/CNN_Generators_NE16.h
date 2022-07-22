/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __CNN_GENERATORS_NE16_SQ8_H__
#define __CNN_GENERATORS_NE16_SQ8_H__
#include <stdint.h>
#include "AutoTilerLib.h"

void LoadCNN_NE16_SQ8_Library();

int CNN_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ConvOper,
        int Fcx,
        int Fcy,
        int Dcx,
        int Dcy,
        int Scx,
        int Scy,
        int ConvPad,
        int PadValue,

        KernelOper_T PoolOper,
        int Fpx,
        int Fpy,
        int Dpx,
        int Dpy,
        int Spx,
        int Spy,
        int PoolPad,

        KernelOper_T ActOper
        );

Kernel_T *CNN_MM_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ConvOper,
        int Fcx,
        int Fcy,
        int Dcx,
        int Dcy,
        int Scx,
        int Scy,
        int ConvPad,
        int PadValue,

        KernelOper_T PoolOper,
        int Fpx,
        int Fpy,
        int Dpx,
        int Dpy,
        int Spx,
        int Spy,
        int PoolPad,

        KernelOper_T ActOper
        );

int CNN_LinearAct_NE16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InDim,
        int OutDim,

        KernelOper_T LinearOper,
        KernelOper_T ActOper
        );

int CNN_MatMulAct_NE16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
        int Bias_DataSize,
        int Filter_DataSizeBits,
        int PadValue,

        int ColM1,
        int LineM1,
        int ColM2,
        int LineM2,

        KernelOper_T MatMulOper,
        KernelOper_T ActOper
        );

int CNN_BatchedMatMulAct_NE16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
        int Bias_DataSize,
        int Filter_DataSizeBits,
        int PadValue,

        int NBatches,
        int ColM1,
        int LineM1,
        int ColM2,
        int LineM2,

        KernelOper_T MatMulOper,
        KernelOper_T ActOper
        );

#endif
