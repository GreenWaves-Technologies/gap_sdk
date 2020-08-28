#include <stdio.h>
#include "Gap8.h"
#include "MFCC.h"

#include "../FFT_Lib/FFT_Lib.h"

#include "../FFT_Lib/TwiddlesDef.h"
#include "../FFT_Lib/SwapTablesDef.h"
#include "MFCC_FB40.def"
#include "HammingLUT.def"

#define Q15                     15
#define Q12                     12
#define Q6                      6

#include "DataIn.def"

#define Max(a, b)       (((a)>(b))?(a):(b))
#define Min(a, b)       (((a)<(b))?(a):(b))

static int CoreCountDynamic = 0;
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

int SetupConfiguration(MFCC_Config_T *Config)

{
	FFT_InstallArg_T FFT_Config;
	gap8_dma_type DmaR_Evt1, DmaR_Evt2, DmaR_Evt3;

        printf("Available L1 Memory: %d\n", plp_getL1HeapSize());
	Config->FFT_Twiddles 	= (short int *) plp_alloc_l1(N_FFT*sizeof(short int));
	Config->FFT_Swap 	= (short int *) plp_alloc_l1(N_FFT*sizeof(short int));
	Config->Hamming 	= (short int *) plp_alloc_l1(FRAME*sizeof(short int));
	Config->FilterBank 	= (FbankType *) plp_alloc_l1(MFCC_BANK_CNT*sizeof(FbankType));
	Config->MFCC_Coeffs 	= (short int *) plp_alloc_l1(MFCC_COEFF_CNT*sizeof(short int));
	Config->MFCC 		= (short int *) plp_alloc_l1(MFCC_BANK_CNT*sizeof(short int));
	Config->W_Frame		= (v2s *)       plp_alloc_l1(N_FFT*sizeof(v2s));
	Config->FramePower	= (short int *) plp_alloc_l1(HALF_FFT*sizeof(short int));
	Config->Frame		= (short int *) plp_alloc_l1(FRAME*sizeof(short int));

	if (Config->FFT_Twiddles==0 || Config->FFT_Swap==0 || Config->Hamming==0 ||
	    Config->FilterBank==0 || Config->MFCC_Coeffs==0 || Config->MFCC==0 ||
	    Config->W_Frame==0 || Config->FramePower==0 || Config->Frame==0) {
		return 1;
	}

	FFT_Config = (FFT_InstallArg_T) {N_FFT, 2, R2_Twiddles_512, R2_SwapTable_512, Config->FFT_Twiddles, Config->FFT_Swap};
	FFT_InstallTwiddlesAndSwapLUT(&FFT_Config);

	gap8_dma_memcpy((unsigned int) HammingLUT, (unsigned int) Config->Hamming, FRAME*sizeof(short int), DMA_COPY_IN, 0, &DmaR_Evt1);
	gap8_dma_memcpy((unsigned int) MFCC_FilterBank, (unsigned int) Config->FilterBank, MFCC_BANK_CNT*sizeof(short int), DMA_COPY_IN, 0, &DmaR_Evt2);
	gap8_dma_memcpy((unsigned int) MFCC_Coeffs, (unsigned int) Config->MFCC_Coeffs, MFCC_COEFF_CNT*sizeof(short int), DMA_COPY_IN, 0, &DmaR_Evt3);
	gap8_dma_wait(DmaR_Evt1);
	gap8_dma_wait(DmaR_Evt2);
	gap8_dma_wait(DmaR_Evt3);

	return 0;
}

static void PreEmphasis(short int * __restrict__ Frame, short int * __restrict__ Out, int FrameSize)

{
        static int Active = 0;
        static int Gain = 2;
        unsigned int i;
        int S;
        unsigned int Chunk, First, Last, CoreId=gap8_coreid();

        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        // for (i=0; i<FrameSize; i++) {
        if (First) S = Frame[First-1]; else S = 0;
        for (i=First; i<Last; i++) {
                if (Active) {
                        // Y[n]=X[n]−0.95⋅X[n−1]
                        Out[i] = Frame[i] - gap8_mulsRN(FP2FIX(0.95, Q15), S, Q15);
                        S = Frame[i];
                } else {
                        Out[i] = Frame[i]<<Gain;
                }

        }
        gap8_waitbarrier(0);
}

static void WindowedFrame(short int *__restrict__ Frame, v2s *__restrict__ OutFrame,
                          short int *__restrict__ Window, int FrameSize, int FFT_Dim)

{
        unsigned int i;
        unsigned int Chunk, First, Last, CoreId=gap8_coreid();

        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        // for (i=0; i<FrameSize; i++) {
        for (i=First; i<Last; i++) {
                /* Frame and Window are Q15, we want OutFrame with precision FFT2_SAMPLE_DYN */
                OutFrame[i] = (v2s) {gap8_mulsRN(Frame[i], Window[i], Q15+(Q15-FFT2_SAMPLE_DYN)), 0};
        }
        Chunk = ChunkSize(FFT_Dim - FrameSize);
        First = CoreId*Chunk+FrameSize; Last = Min(First + Chunk, (FFT_Dim - FrameSize));
        // for (i=FrameSize; i<FFT_Dim;i++) OutFrame[i] = (v2s) {0, 0};
        for (i=First; i<Last;i++) OutFrame[i] = (v2s) {0, 0};
        gap8_waitbarrier(0);
}

#define INVSQRT2        FP2FIX(0.707106781, FFT2_SAMPLE_DYN)

static void EstimatePower(v2s *__restrict__ FrameIn, short int *__restrict__ Power, int N, int N_fft)

{
        unsigned int i;
        unsigned int Log2N_fft = gap8_fl1(N_fft);
        static int Margin = 8;
        unsigned int Chunk, First, Last, CoreId=gap8_coreid();

        Chunk = ChunkSize(N); First = CoreId*Chunk; Last = Min(First + Chunk, N);

        // for (i=0; i<N; i++) {
        for (i=First; i<Last; i++) {
                int P = gap8_dotp2(FrameIn[i], FrameIn[i])>>(FFT2_SAMPLE_DYN-Margin);
                Power[i] = P; // gap8_mulsRN(P, INVSQRT2, FFT2_SAMPLE_DYN);
        }
        gap8_waitbarrier(0);
}

static void ComputeMFCC(
	short int *__restrict__ FramePower,
	short int *__restrict__ MFCC,
	FbankType *__restrict__ MFCC_FilterBank,
	short int *__restrict__ MFCC_Coeffs)

{
        unsigned int i, j, k;
	unsigned int CoreId=gap8_coreid(), Chunk = ChunkSize(MFCC_BANK_CNT);
	unsigned int First = CoreId*Chunk, Last = Min(First + Chunk, MFCC_BANK_CNT);

        /*
                FramePower is in Q FFT2_SAMPLE_DYN      13
                MFCC_Coeffs is in MFCC_COEFF_DYN        10
                MFCC is in FFT2_SAMPLE_DYN              13
        */

	for (i=0; i<Last; i++) {
                int Coeff = 0;
                int Base = MFCC_FilterBank[i].Base;
                int Count = (MFCC_FilterBank[i].Stop-MFCC_FilterBank[i].Start+1);

                for (k=0, j=MFCC_FilterBank[i].Start; j<(MFCC_FilterBank[i].Start+Count); j++, k++) {
                        Coeff = Coeff + MFCC_Coeffs[Base+k]*FramePower[j];
                }
                Coeff = Coeff >> MFCC_COEFF_DYN;
                MFCC[i] = (Coeff*MFCC_FilterBank[i].Norm)>>(MFCC_COEFF_DYN);
        }
}

static void ProcessOneFrame(MFCC_Config_T *Config)

{
	PreEmphasis(Config->InSignal, Config->Frame, FRAME);
	WindowedFrame(Config->Frame, Config->W_Frame, Config->Hamming, FRAME, N_FFT);
	{
		FFT_Arg_T FFT_Arg = (FFT_Arg_T) {(short int *) Config->W_Frame, Config->FFT_Twiddles, N_FFT};
		SwapSamples_Arg_T FFT_Swap_Arg = (SwapSamples_Arg_T) {Config->W_Frame, Config->FFT_Swap, N_FFT};

		Radix2FFT_DIF_Par(&FFT_Arg);
		SwapSamples_Par(&FFT_Swap_Arg);
	}
	EstimatePower(Config->W_Frame, Config->FramePower, HALF_FFT, N_FFT);
	ComputeMFCC(Config->FramePower, Config->MFCC, Config->FilterBank, Config->MFCC_Coeffs);

}

void ProcessAllFrames(MFCC_Config_T *Config, short int *InSignal)

{
        int i;

        for (i=0; i<(sizeof(DataIn)/sizeof(short int)); i+=FRAME_STEP) {
		printf("Processing Frame %d\n", i/FRAME_STEP);
		Config->InSignal = &InSignal[i];
		gap8_task_dispatch((1<<gap8_ncore())-1, ProcessOneFrame, (unsigned int) Config);
		ProcessOneFrame(Config);
        }
}


int main()

{
        if (gap8_clusterid()!=0) {
                printf("FC Launched\n");

                printf("Available L2 Memory: %d\n", plp_getL2HeapSize());

                plp_cluster_fetch(0x1);
                gap8_cluster_wait(0);
	} else {
                unsigned int i, MaxCore = gap8_ncore();
                unsigned int stacks = plp_alloc_l1(plp_stackSize()*(MaxCore-1));
		MFCC_Config_T Config;

                printf("Cluster core %d Launched, %d cores configuration\n", gap8_coreid(), gap8_ncore());
                plp_setSlaveStacks(((1<<MaxCore)-1), stacks, plp_stackSize());
                gap8_setupbarrier(0, (1<<MaxCore)-1);

		SetupConfiguration(&Config);
		ProcessAllFrames(&Config, DataIn);

	}
}
