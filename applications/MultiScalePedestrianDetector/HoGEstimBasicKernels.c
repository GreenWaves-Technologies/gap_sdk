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

#include "HoGEstimBasicKernels.h"
#include "HoGEstimManage.h"
#include <stdio.h>
#include <stdlib.h>

#define Min(a, b)               (((a)<(b))?(a):(b))
#define Max(a, b)               (((a)>(b))?(a):(b))

static int CoreCountDynamic = 1;
static int ActiveCore = gap8_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap8_ncore();
        Log2Core = gap8_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

void KerReadHoGFeatCol(KerReadHoGFeatCol_ArgT *Arg)

{
	uint16_t * __restrict__ In = Arg->In;
	uint32_t H = Arg->H;
	uint32_t FeatureSize = Arg->FeatureSize;
	uint32_t EstimWidth = Arg->EstimWidth;
	uint16_t ** __restrict__ HoGFeatCols = Arg->HoGFeatCols;
	uint32_t HoGFeatColIndex = Arg->HoGFeatColIndex;

    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize(H*FeatureSize);
	unsigned int First = CoreId*ChunkCell, Last  = Min(H*FeatureSize, First+ChunkCell);

	unsigned int i;
	uint16_t *FeatCol;

	if (HoGFeatColIndex>=EstimWidth) {
		if (CoreId==0) {
			FeatCol = HoGFeatCols[0];
			for (i=1; i<EstimWidth; i++) HoGFeatCols[i-1] = HoGFeatCols[i];
			HoGFeatCols[EstimWidth-1] = FeatCol;
		}
	}
	gap8_waitbarrier(0);
	FeatCol = HoGFeatCols[Min(EstimWidth-1, HoGFeatColIndex)];
	for (i=0; i<(H*FeatureSize); i++) {
		FeatCol[i] = In[i];
	}
	gap8_waitbarrier(0);
}

static unsigned int EstimateOneWindow(
	uint16_t ** __restrict__ HoGFeatCols,
	uint32_t First,
	uint32_t Last,
	uint32_t W)

{
	unsigned  int i, j;
	uint32_t Acc = 0;
	unsigned int SizeH = (Last-First)/2;
	static int Vect = 1;

	for (i=0; i<W; i++) {
		if (Vect) {
			v2u *FeatColV = (v2u *) (HoGFeatCols + i);
			for (j=First; j<(First+SizeH); j++) Acc = gap8_sumdotpu2(FeatColV[j], ((v2u) {1,1}), Acc);
		} else {
			uint16_t *FeatCol = HoGFeatCols[i];
			for (j=First; j<Last; j++) Acc += FeatCol[j];
		}
	}
	return Acc;
}

void KerEstimate(KerEstimate_ArgT *Arg)

{
	uint16_t ** __restrict__ HoGFeatCols = Arg->HoGFeatCols;
	uint32_t FeatureSize = Arg->FeatureSize;
	uint32_t WEstimator = Arg->WEstimator;
	uint32_t HEstimator = Arg->HEstimator;
	uint32_t HFeatCols = Arg->HFeatCols;
	uint32_t * __restrict__ Out = Arg->Out;
    unsigned int CoreId = gap8_coreid();
    unsigned int ChunkCell = ChunkSize((HFeatCols-HEstimator)+1);
	unsigned int First = CoreId*ChunkCell, Last  = Min((HFeatCols-HEstimator)+1, First+ChunkCell);
	unsigned int i;

	// for (i=0; i<(HFeatCols-HEstimator); i++) {
	for (i=First; i<Last; i++) {
		Out[i] = EstimateOneWindow(HoGFeatCols, i*FeatureSize, (i+HEstimator)*FeatureSize, WEstimator);
	}
	gap8_waitbarrier(0);
}

typedef struct {
	uint16_t ** __restrict__ HoGFeatCols;
	uint32_t FirstLine;
	uint32_t W;
	uint32_t H;
	uint32_t FeatureSize;
	uint16_t * __restrict__ Buffer;
} KerExtractWindow_ArgT;

void KerExtractWindow(KerExtractWindow_ArgT *Arg)

{
	uint16_t ** __restrict__ HoGFeatCols = Arg->HoGFeatCols;
	uint32_t FirstLine = Arg->FirstLine;
	uint32_t W = Arg->W;
	uint32_t H = Arg->H;
	uint32_t FeatureSize = Arg->FeatureSize;
	uint16_t * __restrict__ Buffer = Arg->Buffer;

	unsigned int CoreId = gap8_coreid();
	unsigned int ChunkCell = ChunkSize(W);
	unsigned int First = CoreId*ChunkCell, Last  = Min(W, First+ChunkCell);
	unsigned int N = H/2;

	for (unsigned int i=First; i<Last; i++) {
		unsigned short *FeatCol = HoGFeatCols[i]+FirstLine;
		for (unsigned j=0; j<H; j++) {
			v2u * __restrict__ VBuffer  = (v2u *) (&Buffer[FeatureSize*(W*j + i)]);
			v2u * __restrict__ VFeatCol = (v2u *) (&FeatCol[FeatureSize*j]);

			for (unsigned k=0; k<(FeatureSize/4); k++) {
				v2u V0 = VFeatCol[2*k], V1 = VFeatCol[2*k+1];
				VBuffer[2*k] = V0; VBuffer[2*k+1] = V1;
				// Buffer[FeatureSize*(W*j + i) + k] = FeatCol[FeatureSize*j+k];
			}
		}
	}
	gap8_waitbarrier(0);
}

static inline int EvalWeakPredictor(unsigned short int * __restrict__ Win, HoGWeakPredictor_T * __restrict__ WeakPred)

{
        int Ind;
	unsigned int X = Win[WeakPred->FeatureId];

#if HOG_ESTIM_TREE_DEPTH==1
#ifdef HOG_ESTIM_TD1
	if (X>WeakPred->ThresHold[0]) return *WeakPred->Value; else return -(*WeakPred->Value);
#else
        if (X>WeakPred->ThresHold[0]) Ind = 1; else Ind = 0;
#endif
#elif HOG_ESTIM_TREE_DEPTH==2
        if (X>WeakPred->ThresHold[0]) {
                if (X>WeakPred->ThresHold[2]) Ind = 3;
                else Ind = 2;
        } else if (X>WeakPred->ThresHold[1]) Ind = 1;
        else Ind = 0;
#elif HOG_ESTIM_TREE_DEPTH==3
        if (X>WeakPred->ThresHold[0]) {
        	if (X>WeakPred->ThresHold[2]) {
        		if (X>WeakPred->ThresHold[6]) Ind = 7; else Ind = 6;
		} else {
        		if (X>WeakPred->ThresHold[5]) Ind = 5; else Ind = 4;
		}
	} else {
        	if (X>WeakPred->ThresHold[1]) {
        		if (X>WeakPred->ThresHold[4]) Ind = 3; else Ind = 2;
		} else {
        		if (X>WeakPred->ThresHold[3]) Ind = 1; else Ind = 0;
		}
	}
#else
#error HOG_ESTIM_TREE_DEPTH value not supported
#endif
        return WeakPred->Value[Ind];
}

typedef struct {
	uint16_t * __restrict__ Win;
	HoGWeakPredictor_T *Model;
	uint32_t ModelSize;
	uint8_t * __restrict__ Out;
} KerPredictWin_ArgT;
static void KerPredictWin(KerPredictWin_ArgT *Arg)

{
	uint16_t * __restrict__ Win = Arg->Win;
	HoGWeakPredictor_T *Model = Arg->Model;
	uint32_t ModelSize = Arg->ModelSize;
	uint8_t * __restrict__ Out = Arg->Out;
        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(ModelSize);
        unsigned int First = CoreId*ChunkCell, Last  = Min(ModelSize, First+ChunkCell);
	static L1_CL_MEM int Merit[8];
	int Acc = 0;

	for (unsigned int i=First; i<Last; i++) {
		Acc += EvalWeakPredictor(Win, Model+i);
	}
	Merit[CoreId] = Acc;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		Acc = 0;
		for (unsigned int i=0; i<gap8_ncore(); i++) Acc += Merit[i];
		*Out = Max(Acc>>(10+6), 0);
		if ((Acc>>(10+6))>0) printf("Match: %X\n", Acc>>(10+6));
	}
	gap8_waitbarrier(0);

}

typedef struct {
	uint16_t * __restrict__ Win;
	uint32_t FeatureSize;
	uint32_t WEstimator;
	uint32_t HEstimator;
	uint32_t * __restrict__ Out;
} KerProcessWin_ArgT;
static void KerProcessWin(KerProcessWin_ArgT *Arg)

{
	uint16_t * __restrict__ Win = Arg->Win;
	uint32_t FeatureSize = Arg->FeatureSize;
	uint32_t WEstimator = Arg->WEstimator;
	uint32_t HEstimator = Arg->HEstimator;
	uint32_t * __restrict__ Out = Arg->Out;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize(WEstimator*FeatureSize);
        unsigned int First = CoreId*ChunkCell, Last  = Min(WEstimator*FeatureSize, First+ChunkCell);
	static L1_CL_MEM unsigned int PartialSum[8];
	unsigned int Acc = 0;
	unsigned int SizeH = (Last-First)/2;

	for (unsigned i=0; i<HEstimator; i++) {
		v2u *WinV = (v2u *) (Win + First);
		for (unsigned j=First; j<(First+SizeH); j++) {
			Acc = gap8_sumdotpu2(WinV[j], ((v2u) {1,1}), Acc);
		}
	}
	PartialSum[CoreId] = Acc;
	gap8_waitbarrier(0);
	if (CoreId==0) {
		Acc = 0;
		for (unsigned int i=0; i<gap8_ncore(); i++) Acc += PartialSum[i];
		*Out = Acc;
	}
	gap8_waitbarrier(0);
}

void KerEstimateWin(
	uint16_t ** __restrict__ HoGFeatCols,
	uint32_t FeatureSize,
	uint32_t WEstimator,
	uint32_t HEstimator,
	uint32_t HFeatCols,
	uint32_t * __restrict__ Out,
	uint16_t * __restrict__ Buffer)

{
	KerExtractWindow_ArgT Win;
	KerProcessWin_ArgT PWin;

	Win.HoGFeatCols = HoGFeatCols;
	Win.W = WEstimator;
	Win.H = HEstimator;
	Win.FeatureSize = FeatureSize;
	Win.Buffer = Buffer;

	PWin.Win = Buffer;
	PWin.FeatureSize = FeatureSize;
	PWin.WEstimator = WEstimator;
	PWin.HEstimator = HEstimator;

	for (unsigned int i=0; i<((HFeatCols-HEstimator)+1); i++) {
		/* Extract, in parallel, a window [Westimator x HEstimator] into Buffer */
		Win.FirstLine = i*FeatureSize;
		rt_team_fork(gap8_ncore(), (void *) KerExtractWindow, (void *) &Win);
		KerExtractWindow(&Win);
		/* Estimate, in parallel, the window now in buffer */
		PWin.Out = Out+i;
		rt_team_fork(gap8_ncore(), (void *) KerProcessWin, (void *) &PWin);
		KerProcessWin(&PWin);

	}
}

int CheckFirst(uint16_t ** __restrict__ HoGFeatCols,
        uint32_t LineIndex,
        uint32_t ColumnIndex,
        uint32_t FeatureSize,
        HoGWeakPredictorBis_T * __restrict__ Model,
        uint32_t Last)

{
	unsigned int FromLine = LineIndex*FeatureSize;
	HoGWeakPredictorBis_T * __restrict__ WeakPred = Model;
	int Acc = 0;

	for (unsigned int i=0; i<Last; i++) {
		unsigned int Pos = WeakPred->Pos;
		unsigned int X = *(HoGFeatCols[gap8_bitextractu(Pos, 4, 12)] + gap8_bitextractu(Pos, 12, 0) + FromLine);
		Acc += (2*(X>WeakPred->ThresHold)-1)*WeakPred->Value;
		WeakPred++;
	}
	return (Acc);
}

static void KerWeakPredictOneWindow(
	uint16_t ** __restrict__ HoGFeatCols,
	uint32_t LineIndex,
	uint32_t ColumnIndex,
	uint32_t FeatureSize,
	HoGWeakPredictorBis_T * __restrict__ Model,
	uint32_t ModelSize)

{
	unsigned int FromLine = LineIndex*FeatureSize;
	HoGWeakPredictorBis_T * __restrict__ WeakPred = Model;
	int Acc = 0;
	unsigned short Range[] = {0,15, 16,31, 32,63, 64,127, 128,255, 256,ModelSize};
	unsigned int Nr = (sizeof(Range)/sizeof(unsigned short))/2;
	int Check = 0, T = -7000;



	for (unsigned int r=0; r<Nr; r++) {
		unsigned int i;
		for (i=Range[2*r]; i<Range[2*r+1]; i++) {
			unsigned int Pos = WeakPred->Pos;
			unsigned int X = *(HoGFeatCols[gap8_bitextractu(Pos, 4, 12)] + gap8_bitextractu(Pos, 12, 0) + FromLine);
			Acc += (2*(X>WeakPred->ThresHold)-1)*WeakPred->Value;
			WeakPred++;
		}
		if ((Acc>>5)<T) {
			// printf("Abort at WeakPred = %d in [%d .. %d]\n", i, Range[2*r], Range[2*r+1]);
			break;
		}
	}
	if (Check) {
		int Exit = ((Acc>>5)<T);
		WeakPred = Model;
		Acc = 0;
		for (unsigned int i=0; i<ModelSize; i++) {
			unsigned int Pos = WeakPred->Pos;
			unsigned int X = *(HoGFeatCols[gap8_bitextractu(Pos, 4, 12)] + gap8_bitextractu(Pos, 12, 0) + FromLine);
			// unsigned int X = *(HoGFeatCols[WeakPred->Col] + WeakPred->Off + FromLine);
			Acc += (2*(X>WeakPred->ThresHold)-1)*WeakPred->Value;
			// if (X>WeakPred->ThresHold[0]) Acc += *WeakPred->Value; Acc -= *WeakPred->Value;
			WeakPred++;
		}
		if (Exit && ((Acc>>(10+6))>0)) {
			printf("Wrong exit decision\n");
		}
	}
	if ((Acc>>(10+6))>0) {
		PushBBAt(ColumnIndex, LineIndex, Acc>>(10+6), 1);
#ifdef CHECK
		unsigned int N = 16;
		int T = -7000, P;
		for (int i=0; i<5; i++) {
			P = CheckFirst(HoGFeatCols, LineIndex, ColumnIndex, FeatureSize, Model, N<<i)>>5;
			if (P>T) {
				printf("We have a hit confirmed by the first %d weak predictors: P=%d, Total P: %d\n", N<<i, P, Acc>>(10+6));
			} else {
				printf("We have a hit NOT confirmed by the first %d weak predictors: P=%d\n", N<<i, P);
			}
		}
#endif
	}
#ifdef CHECK
 else {
		unsigned int N = 16;
		int T = -7000, P;
		for (int i=0; i<5; i++) {
			P = CheckFirst(HoGFeatCols, LineIndex, ColumnIndex, FeatureSize, Model, N<<i);
			P = P>>5;
			if (P>T) {
				printf("We don't have a hit NOT confirmed by the first %d weak predictors: P=%d, Total P: %d\n", N<<i, P, Acc>>(10+6));
			}
		}
	}
#endif
}

void KerWeakEstimateAllWindows(KerWeakEstimate_ArgT *Arg)

{
	uint16_t ** __restrict__ HoGFeatCols = Arg->HoGFeatCols;
	uint32_t ColumnIndexM1 = Arg->ColumnIndexM1;
	uint32_t HEstimator = Arg->HEstimator;
	uint32_t HFeatCols = Arg->HFeatCols;
	uint32_t FeatureSize = Arg->FeatureSize;
	HoGWeakPredictorBis_T * __restrict__ Model = Arg->Model;
	uint32_t ModelSize = Arg->ModelSize;

        unsigned int CoreId = gap8_coreid();
        unsigned int ChunkCell = ChunkSize((HFeatCols-HEstimator)+1);
        unsigned int First = CoreId*ChunkCell, Last  = Min((HFeatCols-HEstimator)+1, First+ChunkCell);

	for (unsigned int i=First; i<Last; i++) {
		KerWeakPredictOneWindow(HoGFeatCols, i, ColumnIndexM1+1, FeatureSize, Model, ModelSize);
	}
	gap8_waitbarrier(0);
}

void KerWeakEstimateWin(
	uint16_t ** __restrict__ HoGFeatCols,
	uint32_t FeatureSize,
	uint32_t WEstimator,
	uint32_t HEstimator,
	uint32_t HFeatCols,
	uint8_t * __restrict__ Out,
	uint16_t * __restrict__ Buffer,
	HoGWeakPredictor_T * __restrict__ Model,
	uint32_t ModelSize)

{
	KerExtractWindow_ArgT Win;
	KerPredictWin_ArgT PWin;

	Win.HoGFeatCols = HoGFeatCols;
	Win.W = WEstimator;
	Win.H = HEstimator;
	Win.FeatureSize = FeatureSize;
	Win.Buffer = Buffer;

	PWin.Win = Buffer;
	PWin.Model = Model;
	PWin.ModelSize = ModelSize;
	for (unsigned int i=0; i<((HFeatCols-HEstimator)+1); i++) {
		/* Extract, in parallel, a window [Westimator x HEstimator] into Buffer */
		Win.FirstLine = i*FeatureSize;
		rt_team_fork(gap8_ncore(),(void*) KerExtractWindow, (void*) &Win);
		KerExtractWindow(&Win);
		/* Estimate, in parallel, the window now in buffer */
		PWin.Out = Out+i;
		rt_team_fork(gap8_ncore(), (void*) KerPredictWin, (void*) &PWin);
		KerPredictWin(&PWin);
	}
#ifdef TRACE
	printf("FeatLine: %d, Westim: %d ====================================================\n", HFeatCols, WEstimator);
	for (unsigned int i=0;i<HFeatCols; i++) {
		for (unsigned int j=0;j<WEstimator; j++) {
			unsigned short *Col = HoGFeatCols[j] +i*FeatureSize;
			printf("/* L%d, B%d */ ", i, j);
			for (unsigned int k=0; k<FeatureSize; k++) {
				printf("%d,", Col[k]);
			}
			printf("\n");
		}
	}
#endif
}

void InstallModel(HoGWeakPredictor_T *From, HoGWeakPredictor_T *To, unsigned int N)

{
	rt_dma_copy_t DmaR_Evt1;
	rt_dma_memcpy((rt_pointerT) From, (rt_pointerT) To, N*sizeof(HoGWeakPredictor_T), RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt1);
}

void InstallModelBis(HoGWeakPredictorBis_T *From, HoGWeakPredictorBis_T *To, unsigned int N)

{
	rt_dma_copy_t DmaR_Evt1;

	rt_dma_memcpy((rt_pointerT) From, (rt_pointerT) To, N*sizeof(HoGWeakPredictorBis_T), RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt1);
}

