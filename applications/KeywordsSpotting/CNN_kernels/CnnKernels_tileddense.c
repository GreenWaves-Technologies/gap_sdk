#include "CnnKernelsInit.h"
#include "CnnKernels_tileddense.h"



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
	rt_dma_copy_t DmaR_Evt1;
	rt_dma_copy_t DmaR_Evt2;
	rt_dma_copy_t DmaR_Evt3;
	rt_dma_copy_t DmaW_Evt1;
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
	rt_dma_memcpy((unsigned int) In+0, (unsigned int) (L1_Memory + 0)+0, 2*InSize, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	rt_dma_memcpy((unsigned int) Bias+(0), (unsigned int) (L1_Memory + 2*InSize)+0, 24, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt2);
	rt_dma_memcpy((unsigned int) Filter+(0), (unsigned int) (L1_Memory + 2*InSize + 24)+0, FilterSize, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt3);
	/* Wait for BUFF read in L2 */
	rt_dma_wait(&DmaR_Evt1);
	rt_dma_wait(&DmaR_Evt2);
	/* ===================End Read First Tile=========================================== */
	/* Kernel Iteration Loop on tiled inner space 0 */
	for (Iter=0; Iter<12; Iter++) {
		/* Loop Iteration Body on tiled inner space 0 */
		/* Elaborate Last, Next_Last, Next_Next_Last */
		Last = ((Iter+1) == 12); NextLast = ((Iter+2) == 12); NextNextLast = ((Iter+3) == 12);
		/* =======================Read Next Tile=========================================== */
		rt_dma_wait(&DmaR_Evt3);
		if (!Last) {
			rt_dma_memcpy((unsigned int) Filter + ((Iter+1)*FilterSize),
				      (unsigned int) (L1_Memory + 2*InSize + 24) + (FilterSize)*((N_Ti+1) % 2), FilterSize, RT_DMA_DIR_EXT2LOC, 0, &DmaR_Evt3);
		}
		/* ===================End Read Next Tile=========================================== */
		/* Call Kernel LOC_INNER_LOOP */
		KerArg0->In = (Word16 * __restrict__) ((unsigned int) (L1_Memory + 0) + 0);
		KerArg0->Filter = (Word8 * __restrict__) ((unsigned int) (L1_Memory + 2*InSize + 24) + FilterSize*((N_Ti) % 2));
		KerArg0->Bias = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 2*InSize) + 0 + (Iter)*2);
		KerArg0->Out = (Word16 *  __restrict__) ((unsigned int) (L1_Memory + 4*InSize + 24) + 0 + (Iter)*2);
		rt_team_fork(gap8_ncore(), (void *) KerLinearLayer_halfchar, (void *) KerArg0);
		N_Ti++;
		/* End Kernel Iteration Loop on tiled inner space 0 */
	}
	Iter=12;
	/* =======================Write Last Tile=========================================== */
	rt_dma_memcpy((unsigned int) Out + (0),
		(unsigned int) (L1_Memory + 4*InSize + 24) + 0, 24, RT_DMA_DIR_LOC2EXT, 0, &DmaW_Evt1);
	rt_dma_wait(&DmaW_Evt1);
	/* ===================End Write Last Tile=========================================== */
}
