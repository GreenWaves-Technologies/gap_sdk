#ifndef __CNN_LSTM_GENERATORS_SQ8_H__
#define __CNN_LSTM_GENERATORS_SQ8_H__

#include "Gap.h"

void LoadRNN_fp16_Library();

int RNN_Sequence_fp16(
	int Nc,
	int K0,
	int K1,
	int *n1,
	int *n2,
	int *n3,
	int *n2_io);

int RNN_Stack_fp16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int Revert
        );

int LSTM_Stack_fp16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int Revert
        );

int GRU_Stack_fp16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int Revert
        );

#endif
