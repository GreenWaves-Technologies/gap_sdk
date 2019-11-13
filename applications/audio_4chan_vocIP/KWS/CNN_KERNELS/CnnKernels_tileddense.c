#include "CnnKernelsInit.h"
#include "CnnKernels_tileddense.h"

#ifdef ALEXA
#define NB_ITER 3
#endif

#ifdef KWS
#define NB_ITER 12
#endif

void Dense_halfchar(
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
	/* row of length 14 for HWCE compatibility*/

#ifdef RT_HAS_HWCE
	int InSize = 14*30*32;
#else
	int InSize = 13*30*32;
#endif

	int FilterSize = InSize/2;
	KerArg0->InSize = (int) (InSize);
	KerArg0->NormFilter = (NormFilter);
	KerArg0->NormBias = (NormBias);
	KerArg0->OutSize = (int) (1);
	/* =======================Read First Tile=========================================== */
	/* Initial reads in L2, O_DB or O_BUFF */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) In+0, (unsigned int) (L1_Memory + 0)+0, 2*InSize, GAP_DMA_L22TCDM, 0, &DmaR_Evt1);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Bias+(0), (unsigned int) (L1_Memory + 2*InSize)+0, 2*NB_ITER, GAP_DMA_L22TCDM, 0, &DmaR_Evt2);
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter+(0), (unsigned int) (L1_Memory + 2*InSize + 2*NB_ITER)+0, FilterSize, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
	/* Wait for BUFF read in L2 */
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on tiled inner space 0 */
	for (Iter=0; Iter<NB_ITER; Iter++) {
		/* Loop Iteration Body on tiled inner space 0 */
		/* Elaborate Last, Next_Last, Next_Next_Last */
		Last = ((Iter+1) == NB_ITER); NextLast = ((Iter+2) == NB_ITER); NextNextLast = ((Iter+3) == NB_ITER);
		/* =======================Read Next Tile=========================================== */
		DMAMCHAN_WaitRequestEnd(&DmaR_Evt3);
		if (!Last) {
			AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Filter + ((Iter+1)*FilterSize),
				      (unsigned int) (L1_Memory + 2*InSize + 2*NB_ITER) + (FilterSize)*((N_Ti+1) % 2), FilterSize, GAP_DMA_L22TCDM, 0, &DmaR_Evt3);
		}
		/* ===================End Read Next Tile=========================================== */
		/* Call Kernel LOC_INNER_LOOP */
		KerArg0->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 0);
		KerArg0->Filter = (Word8 * __restrict__) ((unsigned int) (L1_Memory + 2*InSize + 2*NB_ITER) + FilterSize*((N_Ti) % 2));
		KerArg0->Bias = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 2*InSize) + 0 + (Iter)*2);
		KerArg0->Out = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 4*InSize + 2*NB_ITER) + 0 + (Iter)*2);
		AUTOTILE_CLUSTER_CoresFork(gap8_ncore(), (void *) KerLinearLayer_halfchar, (void *) KerArg0);
		N_Ti++;
		/* End Kernel Iteration Loop on tiled inner space 0 */
	}
	Iter=NB_ITER;
	/* =======================Write Last Tile=========================================== */
	AUTOTILE_DMAMCHAN_Memcpy_1D((unsigned int) Out + (0),
		(unsigned int) (L1_Memory + 4*InSize + 2*NB_ITER) + 0, 2*NB_ITER, GAP_DMA_TCDM2L2, 0, &DmaW_Evt1);
	DMAMCHAN_WaitRequestEnd(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}
