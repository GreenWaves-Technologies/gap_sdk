#include "CnnKernelsInit.h"
#include "CnnKernels.h"
#include "CNN_BasicKernels.h"
void ConvLayer1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	dma_req_t DmaR_Evt1;
	dma_req_t DmaR_Evt2;
	dma_req_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerSetInBiasT S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_fpT S_KerArg1, *KerArg1 = &S_KerArg1;
	KerReLUMaxPool2x2_fpT S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->W = (int) (33);
	KerArg0->H = (int) (79);
	KerArg1->W = (int) (40);
	KerArg1->H = (int) (98);
	KerArg1->Norm = (Norm);
	KerArg1->N = (N);
	KerArg1->M = (M);
	KerArg2->W = (int) (33);
	KerArg2->H = (int) (79);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+(0), (unsigned int) (L1_Memory + 0)+0, 7840, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+0, (unsigned int) (L1_Memory + 15680)+0, 320, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Output Planes */
	for (OutPlane=0; OutPlane<32; OutPlane++) {
		int LastOutPlane = ((OutPlane+1) == 32), NextLastOutPlane = ((OutPlane+2) == 32);
		/* =======================Read Next Partial Buff=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
		if (!LastOutPlane) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + (1*320),
					(unsigned int) (L1_Memory + 15680) + 320*((OutPlane+1) % 2), 320, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
		}
		/* =======================End Read Next Partial Buff======================================= */
		/* Kernel Iteration Loop on tiled inner space 0 */
		Iter=0; {
			/* Loop Iteration Body on tiled inner space 0 */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* Call Kernel LOC_IN_PLANE_PROLOG */
			KerArg0->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 16320) + 0);
			KerArg0->Bias = (Bias[OutPlane]);
#ifdef TILED
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerSetInBias_int, (void *) KerArg0);
#else
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerSetInBias, (void *) KerArg0);
#endif
			/* Kernel Iteration Loop on Input Planes */
			InPlane=0; {
				/* Elaborate LastInPlane */
				int LastInPlane = ((InPlane+1) == 1);
				int NextLastInPlane = ((InPlane+2) == 1);
				/* =======================Read Next Tile=========================================== */
				DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
				if (!LastInPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((InPlane+1)*7840 + (0)*7280),
							(unsigned int) (L1_Memory + 0) + 7840*((N_TiIp+1) % 2), 7840, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				} else
				if (!Last) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((0+1)*7280),
							(unsigned int) (L1_Memory + 0) + 7840*((N_TiIp+1) % 2), 7840, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				else if (!LastOutPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + (0),
							(unsigned int) (L1_Memory + 0) + 7840*((N_TiIp+1) % 2), 7840, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				/* ===================End Read Next Tile=========================================== */
				/* Call Kernel LOC_INNER_LOOP */
				KerArg1->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 7840*((N_TiIp) % 2));


				KerArg1->Filter = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 15680) + (InPlane)*320 + 320*((OutPlane) % 2));
				KerArg1->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 16320) + 0);
#ifdef TILED
				int* outbu = (int*)  KerArg1->Out;
				AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerDirectConvNxM_fp_tiled, (void *) KerArg1);
#else
				AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerDirectConvNxM_fp, (void *) KerArg1);
#endif

				// convert to short int
#ifdef TILED
				for (int i=0;i<KerArg0->W*KerArg0->H;i++) {

				  KerArg1->Out[i] =  gap8_clip(gap8_roundnorm_reg(outbu[i], Norm), 15);

				}
#endif


				N_TiIp++;
				/* End Kernel Iteration Loop on Input Planes */
			}
			/* Call Kernel LOC_IN_PLANE_EPILOG */
			KerArg2->In = (short int * __restrict__) ((unsigned int) (L1_Memory + 16320) + 0);
			// extended memory for int 10000B
			KerArg2->Out = (short int * __restrict__) ((unsigned int) (L1_Memory + 26320) + 1248*((N_Ti) % 2));
			//KerArg2->Out = (short int * __restrict__) ((unsigned int) (L1_Memory + 21536) + 1248*((N_Ti) % 2));

			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerReLUMaxPool2x2_fp, (void *) KerArg2);
			/* =======================Write Tile=========================================== */
			if (0||OutPlane) {
				DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
			}
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + ((0)*1248),
     //				(unsigned int) (L1_Memory + 21536) + 1248*(N_Ti % 2), 1248, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
				(unsigned int) (L1_Memory + 26320) + 1248*(N_Ti % 2), 1248, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
			/* ===================End Write Tile=========================================== */
			N_Ti++;
			/* End Kernel Iteration Loop on tiled inner space 0 */
		}
		Iter=1;
		Filter = Filter + 160;
		Out = Out + 624;
		/* End Kernel Iteration Loop on Output Planes */
	}
	/* =======================Write Last Tile=========================================== */
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

void ConvLayer3(
		short int * __restrict__ In,
		Word8 * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	dma_req_t DmaR_Evt1;
	dma_req_t DmaR_Evt2;
	dma_req_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerSetInBiasT S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_fpsT S_KerArg1, *KerArg1 = &S_KerArg1;
	KerReLUMaxPool2x2_fpT S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->W = (int) (13);
	KerArg0->H = (int) (30);
	KerArg1->W = (int) (16);
	KerArg1->H = (int) (39);
	KerArg1->Norm = (Norm);
	KerArg1->N = (N);
	KerArg1->M = (M);
	KerArg2->W = (int) (13);
	KerArg2->H = (int) (30);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+(0), (unsigned int) (L1_Memory + 0)+0, 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+0, (unsigned int) (L1_Memory + 2496)+0, 1280, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Output Planes */
	for (OutPlane=0; OutPlane<32; OutPlane++) {
		int LastOutPlane = ((OutPlane+1) == 32), NextLastOutPlane = ((OutPlane+2) == 32);
		/* =======================Read Next Partial Buff=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
		if (!LastOutPlane) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + (1*1280),
					(unsigned int) (L1_Memory + 2496) + 1280*((OutPlane+1) % 2), 1280, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
		}
		/* =======================End Read Next Partial Buff======================================= */
		/* Kernel Iteration Loop on tiled inner space 0 */
		Iter=0; {
			/* Loop Iteration Body on tiled inner space 0 */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* Call Kernel LOC_IN_PLANE_PROLOG */
			KerArg0->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 5056) + 0);
			KerArg0->Bias = (Bias[OutPlane]);
#ifdef TILED
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerSetInBias_int, (void *) KerArg0);
#else
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerSetInBias, (void *) KerArg0);
#endif
			/* Kernel Iteration Loop on Input Planes */
			for (InPlane=0; InPlane<32; InPlane++) {
				/* Elaborate LastInPlane */
				int LastInPlane = ((InPlane+1) == 32);
				int NextLastInPlane = ((InPlane+2) == 32);
				/* =======================Read Next Tile=========================================== */
				DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
				if (!LastInPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((InPlane+1)*1248 + (0)*1152),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				} else
				if (!Last) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((0+1)*1152),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				else if (!LastOutPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + (0),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				/* ===================End Read Next Tile=========================================== */
				/* Call Kernel LOC_INNER_LOOP */
				KerArg1->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 1248*((N_TiIp) % 2));
				KerArg1->Filter = (Word8 * __restrict__) ((unsigned int) (L1_Memory + 2496) + (InPlane)*40 + 1280*((OutPlane) % 2));
				KerArg1->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 5056) + 0);

#ifdef TILED
				AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerDirectConvNxM_tiled_2, (void *) KerArg1);
#else
				AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerDirectConvNxM_char, (void *) KerArg1);
#endif


				N_TiIp++;
				/* End Kernel Iteration Loop on Input Planes */
			}
#ifdef TILED
			int* outbu = (int*)  KerArg1->Out;
			for (int i=0;i<KerArg0->W*KerArg0->H;i++) {

			  KerArg1->Out[i] =  gap8_clip(gap8_roundnorm_reg(outbu[i], Norm), 15);

			}
#endif
				/* Call Kernel LOC_IN_PLANE_EPILOG */
			// note room is made for the output in int format (+1000B)
			KerArg2->In = (short int * __restrict__) ((unsigned int) (L1_Memory + 5056) + 0);
			KerArg2->Out = (short int * __restrict__) ((unsigned int) (L1_Memory + 6836) + 780*((N_Ti) % 2));
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerReLU_fp, (void *) KerArg2);
			/* =======================Write Tile=========================================== */
			if (0||OutPlane) {
				DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
			}
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + ((0)*780),
				(unsigned int) (L1_Memory + 6836) + 780*(N_Ti % 2), 780, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
			/* ===================End Write Tile=========================================== */
			N_Ti++;
			/* End Kernel Iteration Loop on tiled inner space 0 */
		}
		Iter=1;
		Filter = Filter + 1280;
		Out = Out + 390;
		/* End Kernel Iteration Loop on Output Planes */
	}
	/* =======================Write Last Tile=========================================== */
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

void ConvLayer2(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
		int  N,
		int  M,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	dma_req_t DmaR_Evt1;
	dma_req_t DmaR_Evt2;
	dma_req_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerSetInBiasT S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_fpT S_KerArg1, *KerArg1 = &S_KerArg1;
	KerReLUMaxPool2x2_fpT S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->W = (int) (13);
	KerArg0->H = (int) (30);
	KerArg1->W = (int) (16);
	KerArg1->H = (int) (39);
	KerArg1->Norm = (Norm);
	KerArg1->N = (N);
	KerArg1->M = (M);
	KerArg2->W = (int) (13);
	KerArg2->H = (int) (30);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+(0), (unsigned int) (L1_Memory + 0)+0, 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+0, (unsigned int) (L1_Memory + 2496)+0, 2560, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Output Planes */
	for (OutPlane=0; OutPlane<32; OutPlane++) {
		int LastOutPlane = ((OutPlane+1) == 32), NextLastOutPlane = ((OutPlane+2) == 32);
		/* =======================Read Next Partial Buff=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
		if (!LastOutPlane) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + (1*2560),
					(unsigned int) (L1_Memory + 2496) + 2560*((OutPlane+1) % 2), 2560, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
		}
		/* =======================End Read Next Partial Buff======================================= */
		/* Kernel Iteration Loop on tiled inner space 0 */
		Iter=0; {
			/* Loop Iteration Body on tiled inner space 0 */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* Call Kernel LOC_IN_PLANE_PROLOG */
			KerArg0->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 7616) + 0);
			KerArg0->Bias = (Bias[OutPlane]);
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerSetInBias, (void *) KerArg0);
			/* Kernel Iteration Loop on Input Planes */
			for (InPlane=0; InPlane<32; InPlane++) {
				/* Elaborate LastInPlane */
				int LastInPlane = ((InPlane+1) == 32);
				int NextLastInPlane = ((InPlane+2) == 32);
				/* =======================Read Next Tile=========================================== */
				DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
				if (!LastInPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((InPlane+1)*1248 + (0)*1152),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				} else
				if (!Last) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + ((0+1)*1152),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				else if (!LastOutPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In + (0),
							(unsigned int) (L1_Memory + 0) + 1248*((N_TiIp+1) % 2), 1248, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				/* ===================End Read Next Tile=========================================== */
				/* Call Kernel LOC_INNER_LOOP */
				KerArg1->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 1248*((N_TiIp) % 2));
				KerArg1->Filter = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 2496) + (InPlane)*80 + 2560*((OutPlane) % 2));
				KerArg1->Out = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 7616) + 0);
				AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerDirectConvNxM_fp, (void *) KerArg1);
				N_TiIp++;
				/* End Kernel Iteration Loop on Input Planes */
			}
			/* Call Kernel LOC_IN_PLANE_EPILOG */
			KerArg2->In = (short int * __restrict__) ((unsigned int) (L1_Memory + 7616) + 0);
			KerArg2->Out = (short int * __restrict__) ((unsigned int) (L1_Memory + 8396) + 780*((N_Ti) % 2));
			AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerReLU_fp, (void *) KerArg2);
			/* =======================Write Tile=========================================== */
			if (0||OutPlane) {
				DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
			}
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + ((0)*780),
				(unsigned int) (L1_Memory + 8396) + 780*(N_Ti % 2), 780, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
			/* ===================End Write Tile=========================================== */
			N_Ti++;
			/* End Kernel Iteration Loop on tiled inner space 0 */
		}
		Iter=1;
		Filter = Filter + 1280;
		Out = Out + 390;
		/* End Kernel Iteration Loop on Output Planes */
	}
	/* =======================Write Last Tile=========================================== */
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

void Dense2(
		Word16 * __restrict__ In,
		Word8 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	dma_req_t DmaR_Evt1;
	dma_req_t DmaR_Evt2;
	dma_req_t DmaR_Evt3;
	dma_req_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerLinearLayer_fpsT S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->InSize = (int) (12480);
	KerArg0->NormFilter = (NormFilter);
	KerArg0->NormBias = (NormBias);
	KerArg0->OutSize = (int) (1);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+0, (unsigned int) (L1_Memory + 0)+0, 24960, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Bias+(0), (unsigned int) (L1_Memory + 24960)+0, 24, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+(0), (unsigned int) (L1_Memory + 24984)+0, 12480, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
	/* Wait for BUFF read in L2 */
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on tiled inner space 0 */
	for (Iter=0; Iter<12; Iter++) {
		/* Loop Iteration Body on tiled inner space 0 */
		/* Elaborate Last, Next_Last, Next_Next_Last */
		Last = ((Iter+1) == 12); NextLast = ((Iter+2) == 12); NextNextLast = ((Iter+3) == 12);
		/* =======================Read Next Tile=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt3);
		if (!Last) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + ((Iter+1)*12480),
					(unsigned int) (L1_Memory + 24984) + 12480*((N_Ti+1) % 2), 12480, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
		}
		/* ===================End Read Next Tile=========================================== */
		/* Call Kernel LOC_INNER_LOOP */
		KerArg0->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 0);
		KerArg0->Filter = (Word8 * __restrict__) ((unsigned int) (L1_Memory + 24984) + 12480*((N_Ti) % 2));
		KerArg0->Bias = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 24960) + 0 + (Iter)*2);
		KerArg0->Out = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 49944) + 0 + (Iter)*2);
		AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerLinearLayer_fps, (void *) KerArg0);
		N_Ti++;
		/* End Kernel Iteration Loop on tiled inner space 0 */
	}
	Iter=12;
	/* =======================Write Last Tile=========================================== */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + (0),
		(unsigned int) (L1_Memory + 49944) + 0, 24, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

void Dense1(
		Word16 * __restrict__ In,
		Word16 * __restrict__ Filter,
		unsigned int NormFilter,
		Word16 * __restrict__ Bias,
		unsigned int NormBias,
		Word16 * __restrict__ Out,
		int OutSize,
		Kernel_T *Ker)

{
	/* Local variables used by this kernel */
	dma_req_t DmaR_Evt1;
	dma_req_t DmaR_Evt2;
	dma_req_t DmaR_Evt3;
	dma_req_t DmaW_Evt1;
	int Iter;
	int Last, NextLast, NextNextLast;
	int N_Ti = 0;
	int N_TiIp = 0, InPlane, OutPlane=0;
	KerLinearLayer_fpT S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg0->InSize = (int) (6240);
	KerArg0->NormFilter = (NormFilter);
	KerArg0->NormBias = (NormBias);
	KerArg0->OutSize = (int) (1);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+0, (unsigned int) (L1_Memory + 0)+0, 12480, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Bias+(0), (unsigned int) (L1_Memory + 12480)+0, 24, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+(0), (unsigned int) (L1_Memory + 12504)+0, 12480, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
	/* Wait for BUFF read in L2 */
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on tiled inner space 0 */
	for (Iter=0; Iter<12; Iter++) {
		/* Loop Iteration Body on tiled inner space 0 */
		/* Elaborate Last, Next_Last, Next_Next_Last */
		Last = ((Iter+1) == 12); NextLast = ((Iter+2) == 12); NextNextLast = ((Iter+3) == 12);
		/* =======================Read Next Tile=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt3);
		if (!Last) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + ((Iter+1)*12480),
					(unsigned int) (L1_Memory + 12504) + 12480*((N_Ti+1) % 2), 12480, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
		}
		/* ===================End Read Next Tile=========================================== */
		/* Call Kernel LOC_INNER_LOOP */
		KerArg0->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 0);
		KerArg0->Filter = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 12504) + 12480*((N_Ti) % 2));
		KerArg0->Bias = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 12480) + 0 + (Iter)*2);
		KerArg0->Out = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 37464) + 0 + (Iter)*2);
		AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerLinearLayer_fp, (void *) KerArg0);
		N_Ti++;
		/* End Kernel Iteration Loop on tiled inner space 0 */
	}
	Iter=12;
	/* =======================Write Last Tile=========================================== */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + (0),
		(unsigned int) (L1_Memory + 37464) + 0, 24, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

