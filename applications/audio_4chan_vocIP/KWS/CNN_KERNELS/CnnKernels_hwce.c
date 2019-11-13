#include "CnnKernelsInit_hwce.h"
#include "CnnKernels_hwce.h"
void Conv8x20MaxPool2x2_HWCE_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
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
	KerReLUMaxPool2x2_fpT S_KerArg5, *KerArg5 = &S_KerArg5;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg5->W = (int) (32);
	KerArg5->H = (int) (79);
	/* Call Kernel LOC_PROLOG */
	HWCE_Enable(
	);
	HWCE_GenericInit(
		(unsigned int) (0),
		(unsigned int) (0),
		(Norm)
	);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In+(0), (rt_pointerT) (L1_Memory + 0)+0, 7840, 
		80, 80, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((rt_pointerT) Filter+0, (rt_pointerT) (L1_Memory + 15680)+0, 416, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Output Planes */
	for (OutPlane=0; OutPlane<32; OutPlane++) {
		int LastOutPlane = ((OutPlane+1) == 32), NextLastOutPlane = ((OutPlane+2) == 32);
		/* =======================Read Next Partial Buff=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
		if (!LastOutPlane) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((rt_pointerT) Filter + (1*416),
					(rt_pointerT) (L1_Memory + 15680) + (416*((OutPlane+1) % 2)), 416, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
		}
		/* =======================End Read Next Partial Buff======================================= */
		/* Kernel Iteration Loop on Iter space */
		Iter=0; {
			/* Loop Iteration Body on Iter space */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* Call Kernel LOC_IN_PLANE_PROLOG */
			HwCE_SetYinMode(
				(unsigned int) (1)
			);
			/* Kernel Iteration Loop on InPlane space */
			InPlane=0; {
				/* Loop Iteration Body on InPlane space */
				/* Elaborate Last, Next_Last */
				int LastInPlane = ((InPlane+1) == 1); int NextLastInPlane = ((InPlane+2) == 1);
				/* =======================Read Next Tile=========================================== */
				DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
				if (!LastInPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + ((InPlane+1)*7840 + (0)*72),
							(rt_pointerT) (L1_Memory + 0) + (7840*((N_TiIp+1) % 2)), 7840, 
							80, 80, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				} else
				if (!Last) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + ((0+1)*72),
							(rt_pointerT) (L1_Memory + 0) + (7840*((N_TiIp+1) % 2)), 7840, 
							80, 80, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				else if (!LastOutPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + (0),
							(rt_pointerT) (L1_Memory + 0) + (7840*((N_TiIp+1) % 2)), 7840, 
							80, 80, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				/* ===================End Read Next Tile=========================================== */
				/* Call Kernel LOC_Unknown */
				HWCE_ProcessOneTile8x20(
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 0) + 7840*((N_TiIp) % 2)),
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 16512) + 0),
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 15680) + (InPlane)*416 + 416*((OutPlane) % 2)),
					(Bias[OutPlane]),
					(unsigned int) (40),
					(unsigned int) (98)
				);
				HwCE_SetYinMode(
					(unsigned int) (0)
				);
				N_TiIp++;
				/* End Kernel Iteration Loop on InPlane space */
			}
			InPlane=1;
			/* Call Kernel LOC_IN_PLANE_EPILOG */
			KerArg5->In = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 16512) + 0);
			KerArg5->Out = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 21568) + 1248*((N_Ti) % 2));
			AUTOTILE_CLUSTER_CoresFork(CLUSTER_GetCoreMask(), (void *) KerReLUMaxPool2x2_fp, (void *) KerArg5);
			/* =======================Write Tile=========================================== */
			if (0||OutPlane) {
				DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
			}
			AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) Out + ((0)*32),
				(rt_pointerT) (L1_Memory + 21568) + (1248*(N_Ti % 2)), 1248, 
				32, 32, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
			/* ===================End Write Tile=========================================== */
			N_Ti++;
			/* End Kernel Iteration Loop on Iter space */
		}
		Iter=1;
		Filter = Filter + 208;
		Out = Out + 624;
		/* End Kernel Iteration Loop on Output Planes */
	}
	/* Call Kernel LOC_EPILOG */
	HWCE_Disable(
	);
	/* =======================Write Last Tile=========================================== */
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

void Conv6x10_HWCE_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Out,
		unsigned int Norm,
		short int * __restrict__ Bias,
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
	KerReLUMaxPool2x2_fpT S_KerArg5, *KerArg5 = &S_KerArg5;

	/* Initialize KerArg, Kernel invariant arguments */
	KerArg5->W = (int) (14);
	KerArg5->H = (int) (30);
	/* Call Kernel LOC_PROLOG */
	HWCE_Enable(
	);
	HWCE_GenericInit(
		(unsigned int) (0),
		(unsigned int) (0),
		(Norm)
	);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In+(0), (rt_pointerT) (L1_Memory + 0)+0, 1248, 
		32, 32, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((rt_pointerT) Filter+0, (rt_pointerT) (L1_Memory + 2496)+0, 3328, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on Output Planes */
	for (OutPlane=0; OutPlane<32; OutPlane++) {
		int LastOutPlane = ((OutPlane+1) == 32), NextLastOutPlane = ((OutPlane+2) == 32);
		/* =======================Read Next Partial Buff=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
		if (!LastOutPlane) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((rt_pointerT) Filter + (1*3328),
					(rt_pointerT) (L1_Memory + 2496) + (3328*((OutPlane+1) % 2)), 3328, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
		}
		/* =======================End Read Next Partial Buff======================================= */
		/* Kernel Iteration Loop on Iter space */
		Iter=0; {
			/* Loop Iteration Body on Iter space */
			/* Elaborate Last, Next_Last, Next_Next_Last */
			Last = ((Iter+1) == 1); NextLast = ((Iter+2) == 1); NextNextLast = ((Iter+3) == 1);
			/* Call Kernel LOC_IN_PLANE_PROLOG */
			HwCE_SetYinMode(
				(unsigned int) (1)
			);
			/* Kernel Iteration Loop on InPlane space */
			for (InPlane=0; InPlane<32; InPlane++) {
				/* Loop Iteration Body on InPlane space */
				/* Elaborate Last, Next_Last */
				int LastInPlane = ((InPlane+1) == 32); int NextLastInPlane = ((InPlane+2) == 32);
				/* =======================Read Next Tile=========================================== */
				DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
				if (!LastInPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + ((InPlane+1)*1248 + (0)*24),
							(rt_pointerT) (L1_Memory + 0) + (1248*((N_TiIp+1) % 2)), 1248, 
							32, 32, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				} else
				if (!Last) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + ((0+1)*24),
							(rt_pointerT) (L1_Memory + 0) + (1248*((N_TiIp+1) % 2)), 1248, 
							32, 32, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				else if (!LastOutPlane) {
					AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) In + (0),
							(rt_pointerT) (L1_Memory + 0) + (1248*((N_TiIp+1) % 2)), 1248, 
							32, 32, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
				}
				/* ===================End Read Next Tile=========================================== */
				/* Call Kernel LOC_Unknown */
				HWCE_ProcessOneTile4x10(
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 0) + 1248*((N_TiIp) % 2)),
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 9152) + 0),
					(Word16 * __restrict__) ((rt_pointerT) (L1_Memory + 2496) + (InPlane)*104 + 3328*((OutPlane) % 2)),
					(Bias[OutPlane]),
					(unsigned int) (16),
					(unsigned int) (39)
				);
				HwCE_SetYinMode(
					(unsigned int) (0)
				);
				N_TiIp++;
				/* End Kernel Iteration Loop on InPlane space */
			}
			InPlane=32;
			/* Call Kernel LOC_IN_PLANE_EPILOG */
			KerArg5->In = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 9152) + 0);
			KerArg5->Out = (short int * __restrict__) ((rt_pointerT) (L1_Memory + 9992) + 840*((N_Ti) % 2));
			AUTOTILE_CLUSTER_CoresFork(CLUSTER_GetCoreMask(), (void *) KerReLU_fp, (void *) KerArg5);
			/* =======================Write Tile=========================================== */
			if (0||OutPlane) {
				DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
			}
			AUTOTILE_DMAMCHAN_Memcpy_2D((rt_pointerT) Out + ((0)*28),
				(rt_pointerT) (L1_Memory + 9992) + (840*(N_Ti % 2)), 840, 
				28, 28, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
			/* ===================End Write Tile=========================================== */
			N_Ti++;
			/* End Kernel Iteration Loop on Iter space */
		}
		Iter=1;
		Filter = Filter + 1664;
		Out = Out + 420;
		/* End Kernel Iteration Loop on Output Planes */
	}
	/* Call Kernel LOC_EPILOG */
	HWCE_Disable(
	);
	/* =======================Write Last Tile=========================================== */
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}

