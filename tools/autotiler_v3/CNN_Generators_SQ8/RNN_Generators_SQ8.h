#ifndef __CNN_LSTM_GENERATORS_SQ8_H__
#define __CNN_LSTM_GENERATORS_SQ8_H__

#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

void Load_RNN_SQ8_Library();

int RNN_Sequence(
	int Nc,
	int K0,
	int K1,
	int *n1,
	int *n2,
	int *n3,
	int *n2_io);

int RNN_Stack_SQ8(
        char *Name,
        CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int BiDir
        );

int LSTM_Stack_SQ8(
        char *Name,
        CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int BiDir
        );

int GRU_Stack_SQ8(
        char *Name,
        CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

        int NCells,
	int K0,
	int K1,
        int DimState,
        int DimIn,
	int AlwaysReset,
	int BiDir
        );

#endif
