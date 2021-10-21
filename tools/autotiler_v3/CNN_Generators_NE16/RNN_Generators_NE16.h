/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#ifndef __CNN_LSTM_GENERATORS_NE16_H__
#define __CNN_LSTM_GENERATORS_NE16_H__

#include "Gap.h"
#define CODE_GENERATION
#include "RNN_BasicKernels_NE16.h"
#include "AutoTilerLib.h"

void Load_RNN_NE16_Library();

int RNN_Sequence(
        int Nc,
        int K0,
        int K1,
        int *n1,
        int *n2,
        int *n3,
        int *n2_io);

int RNN_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int NCells,
        int K0,
        int K1,
        int DimState,
        int DimIn,
        int AlwaysReset,
        int Revert
        );

int LSTM_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int NCells,
        int K0,
        int K1,
        int DimState,
        int DimIn,
        int AlwaysReset,
        int Revert
        );

int GRU_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int NCells,
        int K0,
        int K1,
        int DimState,
        int DimIn,
        int AlwaysReset,
        int Revert
        );

#endif