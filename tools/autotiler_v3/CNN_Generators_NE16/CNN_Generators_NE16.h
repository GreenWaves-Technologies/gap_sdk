#ifndef __CNN_GENERATORS_NE16_SQ8_H__
#define __CNN_GENERATORS_NE16_SQ8_H__
#include <stdint.h>
#include "AutoTilerLib.h"

void LoadCNN_NE16_SQ8_Library();

int CNN_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
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
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InDim,
        int OutDim,

        KernelOper_T LinearOper,
        KernelOper_T ActOper
        );

#endif
