#include "MnistKernels.h"
L1_CL_MEM AT_L1_POINTER Mnist_L1_Memory;
L2_MEM AT_L2_POINTER Mnist_L2_Memory;
static AT_HYPERFLASH_FS_T HyperFlash;
void Conv5x5ReLUMaxPool2x2_0(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out)

{
	/* Shared L1: 40768 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT DmaR_Evt1;
	AT_L2_EVENT DmaR_Evt2;
	AT_L2_EVENT DmaR_Evt3;
	AT_L2_EVENT DmaW_Evt1;
	KerSetBias_fp_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_fp_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerReLUPool_fp_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Last;
	/* User kernel arguments related variables */
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	unsigned int _LP_Out, _LC_Out;
	unsigned int _N_In;
	unsigned int _SN_In;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 32, Tiled: 1][Tile0 Dim: 2][D0 Dim: Init: 1, Tiled: 1]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 2 physical tiles
			Total Size: 9216 [D1, [0 x 9216, 9216]][Tile0, 2:[12x8, 12x4], 2]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 9216, 9216]][Tile0, 2:[12x8, 12x4], 2]
		Tile0: [0, 6144, 192], Tile1: [192, 3072, 96], Tile2; [0, 6144, 192]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 64 [D1, [0 x 64, 64]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 64, 64]]
		Tile0: [0, 64, 64], Tile1: [0, 64, 64], Tile2; [0, 64, 64]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 1600 [D1, [0 x 1600, 1600]][D0, [0 x 1600, 1600]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 1600, 1600]][D0, [0 x 1600, 1600]]
		Tile0: [0, 1600, 1600], Tile1: [0, 1600, 1600], Tile2; [0, 1600, 1600]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 2 physical tiles
			Total Size: 1568 [D0, [0 x 1568, 1568]][Tile0, 2:[28x20, 28x12], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 2:[28x20], 2][D0, [0 x 1568, 1568]]
		Tile0: [0, 1120, 1120], Tile1: [896, 672, 672], Tile2; [0, 1120, 1120]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 2 logical tiles, 1 physical tiles
			Total Size: 36864 [D1, [0 x 36864, 36864]][Tile0, 2:[24x16, 24x8], 2]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 36864, 36864]][Tile0, 2:[24x16, 24x8], 2]
		Tile0: [0, 24576, 768], Tile1: [0, 24576, 768], Tile2; [0, 24576, 768]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (short int * __restrict__) (Mnist_L1_Memory+16192);
	KerArg0->W = (unsigned short int) (24);
	KerArg0->OutFeatures = (unsigned short int) (32);
	KerArg0->Bias = (short int * __restrict__) (Mnist_L1_Memory+2240);
	KerArg1->W = (unsigned short int) (28);
	KerArg1->UsedW = (unsigned short int) (28);
	KerArg1->InFeatures = (unsigned short int) (1);
	KerArg1->OutFeatures = (unsigned short int) (32);
	KerArg1->Filter = (short int * __restrict__) (Mnist_L1_Memory+2304);
	KerArg1->Out = (short int * __restrict__) (Mnist_L1_Memory+16192);
	KerArg1->Norm = (unsigned char) (14);
	KerArg1->TotalInFeatures = (short int) (1);
	KerArg1->Pad = (v4s) 0;
	KerArg1->Orientation = (unsigned char) (1);
	KerArg2->In = (short int * __restrict__) (Mnist_L1_Memory+16192);
	KerArg2->W = (unsigned short int) (24);
	KerArg2->UsedW = (unsigned short int) (24);
	KerArg2->OutFeatures = (unsigned short int) (32);
	KerArg2->Pad = (v4s) 0;
	KerArg2->Orientation = (unsigned char) (1);
	KerArg2->Oper = (unsigned char) (1);
	KerArg2->LB = (int) (0);
	KerArg2->UB = (int) (32767);
	/*================================= Read Tiles Prolog ===============================*/
	_C_Out=0; _SC_Out=6144; _LC_Out=192;
	_SP_Out=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2240), 64, 0, &DmaR_Evt1);
	AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read Bias */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2304), 1600, 0, &DmaR_Evt2);
	AT_L2_WAIT(0, &DmaR_Evt2); /* Wait previous DMA read Filter */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+0+0), 1120, 0, &DmaR_Evt3);
	_N_In=0;
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1;
		for (T0Ind=0; T0Ind<2; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==1), T0Ind_NextLast = ((T0Ind+1)==1);
			/*================================= Prepare Tiles ===================================*/
			_SN_In = 0;
			if (!(T0Ind_Last)) {
				_N_In = _N_In + (896); _SN_In = (1*((1)?672:1120)); 
			} else if (!(1)) {
				_N_In = _N_In + (-896); _SN_In = (1*(1120)); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, &DmaR_Evt3); /* Wait previous DMA read In */
			if (_SN_In) {
				AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+0+1120*((T0Ind_Total+1)%2)),
						_SN_In, 0, &DmaR_Evt3);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			KerArg0->H = (unsigned short int) (T0Ind_Last?8:16);
			AT_FORK(gap_ncore(), (void *) KerParSetBias_fp, (void *) KerArg0);
			__CALL(KerParSetBias_fp, KerArg0);
			{ /* Single iteration on D0 */
				int D0Ind_Last = 1;
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (short int * __restrict__) (Mnist_L1_Memory+0+1120*((T0Ind_Total)%2));
				KerArg1->H = (unsigned short int) (T0Ind_Last?12:20);
				KerArg1->UsedH = (unsigned short int) (T0Ind_Last?12:20);
				AT_FORK(gap_ncore(), (void *) KerParConv5x5Stride1_fp, (void *) KerArg1);
				__CALL(KerParConv5x5Stride1_fp, KerArg1);
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			KerArg2->H = (unsigned short int) (T0Ind_Last?8:16);
			KerArg2->UsedH = (unsigned short int) (T0Ind_Last?8:16);
			KerArg2->Out = (short int * __restrict__) (Mnist_L1_Memory+3904+6144*((T0Ind_Total)%2));
			AT_FORK(gap_ncore(), (void *) KerParPool2x2Stride2_fp, (void *) KerArg2);
			__CALL(KerParPool2x2Stride2_fp, KerArg2);
			/*================================= Write Tiles =====================================*/
			if (_SP_Out) AT_L2_WAIT(0, &DmaW_Evt1); /* Wait previous DMA write Out */
			AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+3904+6144*((T0Ind_Total)%2)),
					_SC_Out, 288, _LC_Out, 1, &DmaW_Evt1);
			/*============================= End Write Tiles =====================================*/
			/*================================= Update Arg Pipeline =============================*/
			_SP_Out = _SC_Out;_LP_Out = _LC_Out;
			/*============================= End Update Arg Pipeline =============================*/
			/*================================= Prepare Tiles ===================================*/
			_SC_Out = 0;
			if (!(T0Ind_Last)) {
				_C_Out = _C_Out + (192); _LC_Out = ((1)?96:192); _SC_Out = (32*_LC_Out); 
			}
			/*============================= End Prepare Tiles ===================================*/
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, &DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void Conv5x5ReLUMaxPool2x2_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out)

{
	/* Shared L1: 38272 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT DmaR_Evt1;
	AT_L2_EVENT DmaR_Evt2;
	AT_L2_EVENT DmaR_Evt3;
	AT_L2_EVENT DmaW_Evt1;
	KerSetBias_fp_T S_KerArg0, *KerArg0 = &S_KerArg0;
	KerConv_fp_T S_KerArg1, *KerArg1 = &S_KerArg1;
	KerReLUPool_fp_T S_KerArg2, *KerArg2 = &S_KerArg2;

	/* Iteration space related variables */
	int D1Ind, D1Ind_Last, D1Ind_NextLast;
	int T0Ind, T0Ind_Last, T0Ind_NextLast;
	int D0Ind, D0Ind_Total=0, D0Ind_Last, D0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _N_In;
	unsigned int _SN_In;
	unsigned int _LN_In;
	unsigned int _N_Filter;
	unsigned int _SN_Filter;
	unsigned int _LN_Filter;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D1 Dim: Init: 64, Tiled: 1][Tile0 Dim: 1][D0 Dim: Init: 32, Tiled: 8]
	Ker Arg: In, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 9216 [D0, [7 x 1152, 1152]][Tile0, 1:[12x12], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 1:[12x12, 6:12x12, 12x12], 2][D0, [7 x 1152, 1152]]
		Tile0: [0, 1152, 288], Tile1: [1152, 1152, 288], Tile2; [2304, 1152, 288]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 128 [D1, [0 x 128, 128]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 128, 128]]
		Tile0: [0, 128, 128], Tile1: [0, 128, 128], Tile2; [0, 128, 128]
	Ker Arg: Filter, Tiled Space: D0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 8 logical tiles, 8 physical tiles
			Total Size: 102400 [D1, [0 x 102400, 102400]][D0, [7 x 12800, 12800]]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 102400, 102400]][D0, [7 x 12800, 12800]]
		Tile0: [0, 12800, 200], Tile1: [200, 12800, 200], Tile2; [400, 12800, 200]
	Ker Arg: Out, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 2048 [D1, [0 x 2048, 2048]][Tile0, 1:[4x4], 2]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 2048, 2048]][Tile0, 1:[4x4], 2]
		Tile0: [0, 2048, 2048], Tile1: [0, 2048, 2048], Tile2; [0, 2048, 2048]
	Ker Arg: ConvOut, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 8192 [D1, [0 x 8192, 8192]][Tile0, 1:[8x8], 2]
		KerArgItSpace (User Kernel Iter Order):
			[D1, [0 x 8192, 8192]][Tile0, 1:[8x8], 2]
		Tile0: [0, 8192, 128], Tile1: [0, 8192, 128], Tile2; [0, 8192, 128]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->Out = (short int * __restrict__) (Mnist_L1_Memory+30080);
	KerArg0->W = (unsigned short int) (8);
	KerArg0->H = (unsigned short int) (8);
	KerArg0->OutFeatures = (unsigned short int) (64);
	KerArg0->Bias = (short int * __restrict__) (Mnist_L1_Memory+2304);
	KerArg1->W = (unsigned short int) (12);
	KerArg1->UsedW = (unsigned short int) (12);
	KerArg1->H = (unsigned short int) (12);
	KerArg1->UsedH = (unsigned short int) (12);
	KerArg1->InFeatures = (unsigned short int) (4);
	KerArg1->OutFeatures = (unsigned short int) (64);
	KerArg1->Out = (short int * __restrict__) (Mnist_L1_Memory+30080);
	KerArg1->Norm = (unsigned char) (14);
	KerArg1->TotalInFeatures = (short int) (4);
	KerArg1->Pad = (v4s) 0;
	KerArg1->Orientation = (unsigned char) (1);
	KerArg2->In = (short int * __restrict__) (Mnist_L1_Memory+30080);
	KerArg2->W = (unsigned short int) (8);
	KerArg2->UsedW = (unsigned short int) (8);
	KerArg2->H = (unsigned short int) (8);
	KerArg2->UsedH = (unsigned short int) (8);
	KerArg2->OutFeatures = (unsigned short int) (64);
	KerArg2->Out = (short int * __restrict__) (Mnist_L1_Memory+28032);
	KerArg2->Pad = (v4s) 0;
	KerArg2->Orientation = (unsigned char) (1);
	KerArg2->Oper = (unsigned char) (1);
	KerArg2->LB = (int) (0);
	KerArg2->UB = (int) (32767);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+0+0), 1152, 288, 288, 0, &DmaR_Evt1);
	_N_In=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2304), 128, 0, &DmaR_Evt2);
	AT_L2_WAIT(0, &DmaR_Evt2); /* Wait previous DMA read Bias */
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2432+0), 12800, 1600, 200, 0, &DmaR_Evt3);
	_N_Filter=0;
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D1 */
		int D1Ind_Last = 1, D1Ind_NextLast = 1;
		{ /* Single iteration on Tile0 */
			int T0Ind_Last = 1, T0Ind_NextLast = 1;
			/*====================== Call Kernel LOC_D0_PROLOG =========================*/
			AT_FORK(gap_ncore(), (void *) KerParSetBias_fp, (void *) KerArg0);
			__CALL(KerParSetBias_fp, KerArg0);
			for (D0Ind=0; D0Ind<8; D0Ind++, D0Ind_Total++) { /* Iteration on D0 */
				int D0Ind_Last = (D0Ind==7), D0Ind_NextLast = ((D0Ind+1)==7);
				/*================================= Prepare Tiles ===================================*/
				_SN_In = 0;
				if (!(D0Ind_Last)) {
					_N_In = _N_In + (1152); _LN_In = (288); _SN_In = (4*_LN_In); 
				} else if (!(1)) {
					_N_In = _N_In + (-8064); _LN_In = (288); _SN_In = (4*_LN_In); 
				}
				_SN_Filter = 0;
				if (!(D0Ind_Last)) {
					_N_Filter = _N_Filter + (200); _LN_Filter = (200); _SN_Filter = (64*_LN_Filter); 
				} else if (!((1))) {
					_N_Filter = _N_Filter + (-1400); _LN_Filter = (200); _SN_Filter = (64*_LN_Filter); 
				}
				/*============================= End Prepare Tiles ===================================*/
				/*================================= Read Tiles ======================================*/
				AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
				if (_SN_In) {
					AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) In+_N_In), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+0+1152*((D0Ind_Total+1)%2)),
							_SN_In, 288, _LN_In, 0, &DmaR_Evt1);
				}
				AT_L2_WAIT(0, &DmaR_Evt3); /* Wait previous DMA read Filter */
				if (_SN_Filter) {
					AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) Filter+_N_Filter), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2432+12800*((D0Ind_Total+1)%2)),
							_SN_Filter, 1600, _LN_Filter, 0, &DmaR_Evt3);
				}
				/*============================= End Read Tiles ======================================*/
				/*====================== Call Kernel LOC_D0 =========================*/
				KerArg1->In = (short int * __restrict__) (Mnist_L1_Memory+0+1152*((D0Ind_Total)%2));
				KerArg1->Filter = (short int * __restrict__) (Mnist_L1_Memory+2432+12800*((D0Ind_Total)%2));
				AT_FORK(gap_ncore(), (void *) KerParConv5x5Stride1_fp, (void *) KerArg1);
				__CALL(KerParConv5x5Stride1_fp, KerArg1);
				/*================================= Update Arg Pipeline =============================*/
				/*============================= End Update Arg Pipeline =============================*/
			} /* End iteration on D0 */
			/*====================== Call Kernel LOC_D0_EPILOG =========================*/
			AT_FORK(gap_ncore(), (void *) KerParPool2x2Stride2_fp, (void *) KerArg2);
			__CALL(KerParPool2x2Stride2_fp, KerArg2);
		} /* End iteration on Tile0 */
	} /* End iteration on D1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+28032), 2048, 1, &DmaW_Evt1);
	AT_L2_WAIT(0, &DmaW_Evt1); /* Wait DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
void LinearLayerReLU_1(
		short int * __restrict__ In,
		short int * __restrict__ Filter,
		short int * __restrict__ Bias,
		short int * __restrict__ Out)

{
	/* Shared L1: 22568 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT DmaR_Evt1;
	AT_L2_EVENT DmaR_Evt2;
	AT_L2_EVENT DmaR_Evt3;
	AT_L2_EVENT DmaW_Evt1;
	KerLinearLayerReLU_fp_T S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Iteration space related variables */
	int D0Ind, D0Ind_Last;
	int T0Ind, T0Ind_Last;
	/* User kernel arguments related variables */
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[D0 Dim: Init: 10, Tiled: 1][Tile0 Dim: 1]
	Ker Arg: In, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 2048 [Tile0, 1:[1x1], 2048]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 1:[1x1], 2048]
		Tile0: [0, 2048, 2048], Tile1: [0, 2048, 2048], Tile2; [0, 2048, 2048]
	Ker Arg: Filter, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 20480 [D0, [0 x 20480, 20480]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 20480, 20480]]
		Tile0: [0, 20480, 20480], Tile1: [0, 20480, 20480], Tile2; [0, 20480, 20480]
	Ker Arg: Bias, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 20 [D0, [0 x 20, 20]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 20, 20]]
		Tile0: [0, 20, 20], Tile1: [0, 20, 20], Tile2; [0, 20, 20]
	Ker Arg: Out, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 1 logical tiles, 1 physical tiles
			Total Size: 20 [D0, [0 x 20, 20]]
		KerArgItSpace (User Kernel Iter Order):
			[D0, [0 x 20, 20]]
		Tile0: [0, 20, 20], Tile1: [0, 20, 20], Tile2; [0, 20, 20]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->In = (short int * __restrict__) (Mnist_L1_Memory+0);
	KerArg0->InSize = (unsigned short int) (1024);
	KerArg0->TotalInSize = (unsigned short int) (1024);
	KerArg0->OutSize = (unsigned short int) (10);
	KerArg0->Filter = (short int * __restrict__) (Mnist_L1_Memory+2048);
	KerArg0->Bias = (short int * __restrict__) (Mnist_L1_Memory+22528);
	KerArg0->Out = (short int * __restrict__) (Mnist_L1_Memory+22548);
	KerArg0->Norm = (unsigned char) (13);
	KerArg0->NormBias = (signed char) (16);
	KerArg0->LB = (int) (-32768);
	KerArg0->UB = (int) (32767);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) In+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+0), 2048, 0, &DmaR_Evt1);
	AT_L2_WAIT(0, &DmaR_Evt1); /* Wait previous DMA read In */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Filter+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+2048), 20480, 0, &DmaR_Evt2);
	AT_L2_WAIT(0, &DmaR_Evt2); /* Wait previous DMA read Filter */
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Bias+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+22528), 20, 0, &DmaR_Evt3);
	AT_L2_WAIT(0, &DmaR_Evt3); /* Wait previous DMA read Bias */
	/*============================= End Read Tiles Prolog ===============================*/
	{ /* Single iteration on D0 */
		int D0Ind_Last = 1;
		{ /* Single iteration on Tile0 */
			int T0Ind_Last = 1;
			/*====================== Call Kernel LOC_LOOP =========================*/
			AT_FORK(gap_ncore(), (void *) KerParLinearLayerReLU_fp, (void *) KerArg0);
			__CALL(KerParLinearLayerReLU_fp, KerArg0);
		} /* End iteration on Tile0 */
	} /* End iteration on D0 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+0), ((AT_L2_INT_ADDR_TYPE) Mnist_L1_Memory+22548), 20, 1, &DmaW_Evt1);
	AT_L2_WAIT(0, &DmaW_Evt1); /* Wait DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
