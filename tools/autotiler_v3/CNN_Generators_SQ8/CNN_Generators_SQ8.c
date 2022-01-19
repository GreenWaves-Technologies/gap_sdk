#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#include "CNN_Generators_SQ8.h"
#include "CNN_Generator_Util.h"
#include "CNN_Copy_Generators.h"
#include "Gap.h"
#include "../CNN_Libraries_SQ8/CNN_Infos_SQ8.h"


#define MaxS(a, b) (((int)(a)>(int)(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
#define Abs(x) (((x)<0)?-(x):(x))

#define D0	KER_ITER_D0
#define D1	KER_ITER_D1
#define D2	KER_ITER_D2
#define D3	KER_ITER_D3
#define T0	KER_ITER_TILE0
#define T1	KER_ITER_TILE1
#define T2	KER_ITER_TILE2


void LoadCNN_SQ8_Library()

{
	LoadCNN_Copy_Library();
	LibKernelTemplate("KerSetBias_SQ8_T",
                  CArgs(6,
			TCArg("int * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "Feat"),
			TCArg("void * __restrict__", "Bias"),
			TCArg("unsigned char", "NormBias")
			)
	);
	LibKernelTemplate("KerConv_SQ8_T",
                  CArgs(20,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("unsigned short int", "TotalInFeatures"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("int * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "Orientation"),
                        TCArg("unsigned short", "N"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned short", "Ny"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
			)
	);
	LibKernelTemplate("Ker_MM_Conv_SQ8_T",
		CArgs(22,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Fx"),
			TCArg("unsigned char", "Fy"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned char", "Dx"),
			TCArg("unsigned char", "Dy"),
			TCArg("unsigned char", "FirstTile"),
			TCArg("v4s", "Pad"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("int * __restrict__", "Bias"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "InFeat"),
			TCArg("unsigned short int", "OutFeat"),
			TCArg("unsigned short int", "Wo"),
			TCArg("unsigned short int", "Ho"),
			TCArg("unsigned char * __restrict__", "Scale"),
			TCArg("unsigned char * __restrict__", "ScaleN"),
			TCArg("signed char * __restrict__", "ColBuff"),
			TCArg("signed char * __restrict__", "Infos")
		)
	);

	LibKernelTemplate("Ker_MM_Pool_SQ8_T",
		CArgs(13,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Fx"),
			TCArg("unsigned char", "Fy"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned char", "FirstTile"),
			TCArg("v4s", "Pad"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "Wo"),
			TCArg("unsigned short int", "Ho")
		)
	);
	LibKernelTemplate("Ker_MM_Pool_USQ8_T",
		CArgs(13,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Fx"),
			TCArg("unsigned char", "Fy"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned char", "FirstTile"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned char * __restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "Wo"),
			TCArg("unsigned short int", "Ho")
		)
	);
	LibKernelTemplate("KerConvLinReduct_SQ8_T",
                  CArgs(8,
			TCArg("int *__restrict__", "In"),
			TCArg("void *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char *__restrict__", "Scale"),
			TCArg("unsigned char *__restrict__", "ScaleN"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);

        LibKernelTemplate("KerPool_HWC_USQ8_T",
                  CArgs(13,
                        TCArg("unsigned char * __restrict__", "In"),
                        TCArg("unsigned char * __restrict__", "Out"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "UsedW"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned short int", "UsedH"),
                        TCArg("unsigned short int", "FS"),
                        TCArg("unsigned short int", "FSy"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("v4s", "Pad"),
                        TCArg("signed char * __restrict__", "Infos")
                        )
        );
	LibKernelTemplate("KerActivation_SQ8_T",
                  CArgs(6,
			TCArg("signed char *__restrict__", "In"),
			TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("signed char *__restrict__", "Infos")
		       )
	);
	LibKernelTemplate("KerPool_SQ8_T",
                  CArgs(18,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "Feat"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned short", "FS"),
			TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned short", "FSy"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy"),
			TCArg("unsigned char", "PoolMax"),
			TCArg("unsigned char", "Orientation"),
			TCArg("unsigned char", "DoScale"),
			TCArg("signed char * __restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerGlobalPool_SQ8_T",
                  CArgs(8,
			TCArg("void * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "TileIndex"),
			TCArg("int * __restrict__", "Out"),
			TCArg("unsigned char", "DoScale"),
			TCArg("void * __restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerLinear_SQ8_T",
                  CArgs(10,
			TCArg("signed char * __restrict__", "In"),
			TCArg("signed char * __restrict__", "Weights"),
			TCArg("void * __restrict__", "Bias"),
			TCArg("void * __restrict__", "Out"),
			TCArg("unsigned short int", "InDim"),
			TCArg("unsigned short int", "TotalInDim"),
			TCArg("unsigned short int", "OutDim"),
			TCArg("unsigned char *__restrict__", "Scale"),
			TCArg("unsigned char *__restrict__", "ScaleN"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerMat3_SQ8_T",
                  CArgs(8,
			TCArg("signed char *__restrict__", "In1"),
			TCArg("signed char *__restrict__", "In2"),
			TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "DoScale"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerMatMul_SQ8_T",
                  CArgs(19,
			TCArg("signed char * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("signed char * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("void * __restrict__", "Bias"),
			TCArg("unsigned char * __restrict__", "Scale"),
			TCArg("unsigned char * __restrict__", "ScaleN"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned short int", "OutFirstCol"),
			TCArg("signed char * __restrict__", "BufferColIn2"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "ColFirst"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerMatMul_PL_SQ8_T",
                  CArgs(19,
			TCArg("signed char * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("signed char * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("void * __restrict__", "Bias"),
			TCArg("unsigned char", "Scale"),
			TCArg("unsigned char", "ScaleN"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned short int", "OutFirstCol"),
			TCArg("signed char * __restrict__", "BufferColIn2"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "ColFirst"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);
	LibKernelTemplate("KerSoftMax_SQ8_T",
                  CArgs(6,
			TCArg("signed char *__restrict__", "In"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "N"),
			TCArg("unsigned short int", "Norm"),
			TCArg("short int *__restrict__", "Out"),
			TCArg("signed char *__restrict__", "Infos")
			)
	);


	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 8 bits. Kernels for multiple output features evaluated in parallel  */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerParSetBiasB8_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParSetBiasB16_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(2,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParSetBiasB32_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));

        LibKernel("KerParSetBiasB8_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParSetBiasB16_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T",CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(2,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParSetBiasB32_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T",CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));

	/* Convolutions with 32b output, Bias set before */
        LibKernel("KerParConv1x1Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,3,1,1,1,2));

        LibKernel("KerParConv3x3Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerParConv4x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 4,1,1,1,1,1));
        LibKernel("KerParConv1x4Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,4,1,1,1,1));
        LibKernel("KerParConv5x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,-1,-2));
        LibKernel("KerParConv7x7StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 7,7,1,1,-1,-2));
        LibKernel("KerParConv13x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 13,1,1,1,1,1));
        //LibKernel("KerParConv4x10StrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 4,10,1,1,-1,-1));

        LibKernel("KerParConv1D_NStrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV1D), 0, 1, CNN_Type(1,1,1,0,4), -1,1,1,1,-1,1));
        LibKernel("KerParConvNx1StrideSxS1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,1,1,1,-1,1));

        LibKernel("KerParConvNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));

        LibKernel("KerParConv3x3DxDyStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,-1,-1,-1,-1));
        LibKernel("KerParConv3x3DxDyStride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,-1,-1,1,1));
        LibKernel("KerParConv3x3D2x2Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,2,2,1,1));
        LibKernel("KerParConv3x3D4x4Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,4,4,1,1));
        LibKernel("KerParConv3x3D8x8Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,8,8,1,1));
        LibKernel("KerParConv3x3D2x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,2,1,1,1));
        LibKernel("KerParConv3x3D4x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,4,1,1,1));
        LibKernel("KerParConv3x3D8x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,8,1,1,1));
        LibKernel("KerParConv3x3DxD2Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 3,3,-1,2,1,1));

	/* Depth Wise Convolutions, 8b bias, 32b output */
        LibKernel("KerParConvDW1x1Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,3,1,1,1,2));

        LibKernel("KerParConvDW3x3Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDW7x7StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSyB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSyB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions, 16b bias, 32b output */
        LibKernel("KerParConvDW1x1Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,3,1,1,1,2));

        LibKernel("KerParConvDW3x3Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDW7x7StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSyB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSyB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,2,0,4), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions, 32b bias, 32b output */
        LibKernel("KerParConvDW1x1Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,3,1,1,1,2));

        LibKernel("KerParConvDW3x3Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDW7x7StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSyB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSyB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,4,0,4), -1,-1,-1,-1,-1,-1));

	/* Linear layer, 32b output with bias set before and scaling/activation done after */
	LibKernel("KerParLinearLayer_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_LINEAR), 0, 1, CNN_Type(1,1,0,0,4), 0,0,0,0,0,0));

	/* Linear layer, 8b output with bias and scaling/activation (ReLU, ReLUN) done in a single shot */
	LibKernel("KerParLinearLayerFullFeatB8_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB8_ReLU_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB8_ReLUN_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,0,0));

	LibKernel("KerParLinearLayerFullFeatB16_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB16_ReLU_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB16_ReLUN_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T",CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,0,0));

	LibKernel("KerParLinearLayerFullFeatB32_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB32_ReLU_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T", CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerFullFeatB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerLinear_SQ8_T",CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,0,0));

	/* Convolution or Linear output reduction with per channel scaling and optional activation. Out != In and In Place (IO)  */
	LibKernel("KerParReduct_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_ReLUM_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_Sigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_SIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_Tanh_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_TANH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerParReductIO_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLUM_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSWISH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_LEAKYRELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_Sigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_SIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_Tanh_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_TANH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	/* Pooling (Max or Avg) with tensor centric scaling and optional ReLU or ReLUN activation */
        LibKernel("KerParPool2x2Stride2_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLUM_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));

        LibKernel("KerParPoolNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLUM_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));

        LibKernel("KerParPoolNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLUM_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));

        LibKernel("KerParMaxPoolNxMStrideSxSy_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Pool_SQ8_T",
												CNN_Match(CNN_OperList(1, KOP_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParAvgPoolNxMStrideSxSy_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Pool_SQ8_T",
												CNN_Match(CNN_OperList(1, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));

	/* Global Pooling (Max or Avg) with tensor centric scaling and optional ReLU or ReLUN activation */
        LibKernel("KerParGlobalMaxPoolFullFeat_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalMaxPool_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalAvgPoolFullFeat_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalAvgPool_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));

        LibKernel("KerParGlobalSumPoolFullFeat_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPoolFullFeat_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPoolFullFeat_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPoolFullFeat_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPoolFullFeat_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalSumPool_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPool_Reduct_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPool_Reduct_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPool_Reduct_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPool_Reduct_ReLUM_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL_REDUCT), CNN_OperList(1, KOP_RELUM), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalSumPool_Reduct_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_SUMPOOL_REDUCT), CNN_OperList(1, KOP_RELUMN), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));

	/* Matrix Algebra */

	/* Matrix Addition with tensor centric scaling and optional activation */
	LibKernel("KerMatAdd_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_NONE), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELU), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELUN), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_HSIGMOID), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_HSwish_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_HSWISH), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_LEAKYRELU), 0, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerMatAdd_USQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_NONE), 0, CNN_Type(-1,-1,0,0,-1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLU_USQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELU), 0, CNN_Type(-1,-1,0,0,-1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLUN_USQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELUN), 0, CNN_Type(-1,-1,0,0,-1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLUM_USQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELUM), 0, CNN_Type(-1,-1,0,0,-1), 0,0,0,0,0,0));
	LibKernel("KerMatAdd_ReLUMN_USQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELUMN), 0, CNN_Type(-1,-1,0,0,-1), 0,0,0,0,0,0));

	/* Matrix Multiplication for 1x1 convolutions with channel scaling and optional ReLU or ReLUN activation */
	/* 8b Bias */
	LibKernel("KerParMatMulB8_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB8_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB8_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulSxSyB8_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB8_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB8_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,-1,-1));

	/* 16b Bias */
	LibKernel("KerParMatMulB16_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB16_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB16_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulSxSyB16_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB16_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB16_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,-1,-1));

	/* 32b Bias or No Bias at all */
	LibKernel("KerParMatMulB32_2x4_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_2x4_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulTransposedB32_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MATMUL_TRANSPOSED, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedB32_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL_TRANSPOSED, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL_TRANSPOSED, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulB32_2x4_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR, KOP_MATMUL_NOBIAS_SCALE_SCALAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_2x4_ReLU_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR, KOP_MATMUL_NOBIAS_SCALE_SCALAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_2x4_ReLUN_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR, KOP_MATMUL_NOBIAS_SCALE_SCALAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulTransposedB32_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",	  	CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR_TRANSPOSED, KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedB32_ReLU_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",  	CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR_TRANSPOSED, KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedB32_ReLUN_PL_SQ8", CALL_PARALLEL, 0, "KerMatMul_PL_SQ8_T",  	CNN_Match(CNN_OperList(2, KOP_MATMUL_SCALE_SCALAR_TRANSPOSED, KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulSxSyB32_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB32_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MATMUL, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));

	/* Mat Mul based convolutions */

	/* CHW In and Out tensors, [OutFeat,InFeat,Fy,Fx] weights */
	LibKernel("KerPar_MM_Conv1D_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_ReLU_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), -1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_ReLUN_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), -1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_LeakyReLU_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(1,1,4,0,1), -1,1,1,1,-1,-1));

	LibKernel("KerPar_MM_Conv1D_DxDy_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,1,-1,-1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_DxDy_ReLU_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,1,-1,-1,-1,-1));

	LibKernel("KerPar_MM_Conv2D_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv2D_DxDy_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,-1,-1,-1,-1));
	LibKernel("KerPar_MM_Conv2D_DxDy_ReLU_SQ8", CALL_PARALLEL, 0, "Ker_MM_Conv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,-1,-1,-1,-1));


	/* HWC In and Out tensors, [OutFeat,Fy,Fx,InFeat] weights */
	LibKernel("KerPar_MM_Conv1x1_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1x1_ReLU_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 1,1,1,1,-1,-1));
	LibKernel("Ker_MM_Conv1x1_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 0, CNN_Type(1,1,4,0,1), 1,1,1,1,-1,-1));
	LibKernel("Ker_MM_Conv1x1_ReLU_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 0, CNN_Type(1,1,4,0,1), 1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv1D_DxDy_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,1,-1,-1,-1,-1));
	LibKernel("KerPar_MM_Conv2D_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv2D_ReLU_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));
	LibKernel("KerPar_MM_Conv2D_DxDy_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,-1,-1,-1,-1));
	LibKernel("KerPar_MM_ConvDW2D_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV_DW), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));
	LibKernel("Ker_MM_Conv2D_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 0, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));
	LibKernel("Ker_MM_Conv2D_ReLU_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Conv_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 0, CNN_Type(1,1,4,0,1), -1,-1,1,1,-1,-1));

	/* Matrix Multiplication for 1x1 convolutions with channel scaling and optional ReLU or ReLUN activation, optimized form when In1 fits entirely into shared L1 */
	/* 8b Bias */
	LibKernel("KerParMatMulB8_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB8_ReLU_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB8_ReLUN_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));

	/* 16b Bias */
	LibKernel("KerParMatMulB16_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB16_ReLU_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB16_ReLUN_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,2,0,1), 0,0,0,0,1,1));

	/* 32b Bias */
	LibKernel("KerParMatMulB32_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	//LibKernel("KerParMatMulB32_ReLU_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_2x4_ReLU_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_ReLUN_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));


	/* Matrix by vector multiplication with tensor centric scaling and optional activation */
	LibKernel("KerParMatVectMul_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_ReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_HSIGMOID), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_HSwish_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_HSWISH), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));

	/* SoftMax, pre scaling */
	LibKernel("KerParSoftMax_SQ8",      CALL_PARALLEL, 0, "KerSoftMax_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, -1, CNN_Type(1,0,0,0,2), 0,0,0,0,0,0));
	LibKernel("KerParSoftMax8Bits_SQ8", CALL_PARALLEL, 0, "KerSoftMax_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, -1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerParSoftMax_HWC_SQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "KerSoftMax_SQ8_T",
											CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, -1, CNN_Type(1,0,0,0,2), 0,0,0,0,0,0));

	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 8 bits. Kernels for a single feature evaluated in parallel          */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerSetBiasB8_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB16_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB32_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));

        LibKernel("KerSetBiasB8_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T", CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB16_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T",CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB32_HWC_SQ8", CALL_HWC_KER|CALL_PARALLEL, 0, "KerSetBias_SQ8_T",CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));

	/* Convolutions with 32b output */
        LibKernel("KerConv1x1Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerConv7x7StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerConv1D_NStrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV1D),0,0, CNN_Type(1,1,1,0,4), -1,1,1,1,-1,1));
        LibKernel("KerConvNx1StrideSxS1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), -1,1,1,1,-1,1));

        LibKernel("KerConvNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions, 8b bias, 32b output */
        LibKernel("KerConvDW1x1Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2B8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerConvDW7x7StrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideSB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSyB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSyB8_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions, 16b bias, 32b output */
        LibKernel("KerConvDW1x1Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2B16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerConvDW7x7StrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideSB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSyB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSyB16_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,2,0,4), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions, 32b bias, 32b output */
        LibKernel("KerConvDW1x1Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2B32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 5,5,1,1,-1,-2));

        LibKernel("KerConvDW7x7StrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideSB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSyB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSyB32_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,4,0,4), -1,-1,-1,-1,-1,-1));

	/* Convolution, Linear output reduction with per channel scaling and optional activation. Out != In and In Place (IO)  */
	LibKernel("KerReduct_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUN), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_ReLUM_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUM), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUMN), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_Sigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_SIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_Tanh_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_TANH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerReductIO_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUN), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLUM_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUM), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUMN), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSWISH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_LEAKYRELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_Sigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_SIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_Tanh_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_TANH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	/* Activation and reduct for CHW input and HWC output Layer Layout */
	LibKernel("KerParReduct_CC_CHW2HWC_SQ8",	        CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_NONE),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_ReLU_SQ8",    	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_ReLUN_SQ8",   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_ReLUM_SQ8",   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_ReLUMN_SQ8",   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_HSigmoid_SQ8",	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_HSwish_SQ8",  	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_LeakyReLU_SQ8",	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_Sigmoid_SQ8",	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_CHW2HWC_Tanh_SQ8",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_CHW2HWC), CNN_OperList(1, KOP_TANH), 	   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	/* Activation and reduct for HWC input/output Layer Layout */
	LibKernel("KerParReduct_CC_HWC_SQ8",  	       		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLU_HWC_SQ8",      		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUN_HWC_SQ8",     		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUM_HWC_SQ8",     		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUMN_HWC_SQ8",    		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSigmoid_HWC_SQ8",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSwish_HWC_SQ8",    		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_LeakyReLU_HWC_SQ8", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Sigmoid_HWC_SQ8",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Tanh_HWC_SQ8",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_TANH),  	   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
 
	LibKernel("KerParReduct_CC_HWC_SQ16",  	       		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE),      1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLU_HWC_SQ16",      	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUN_HWC_SQ16",     	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUM_HWC_SQ16",     	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUMN_HWC_SQ16",    	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSigmoid_HWC_SQ16",  	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSwish_HWC_SQ16",    	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_LeakyReLU_HWC_SQ16", 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Sigmoid_HWC_SQ16",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Tanh_HWC_SQ16",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_TANH),  	   1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));

	LibKernel("KerParReduct_CC_HWC_USQ16", 	       		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE),      1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLU_HWC_USQ16",     	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUN_HWC_USQ16",    	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUM_HWC_USQ16",    	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_ReLUMN_HWC_USQ16",   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSigmoid_HWC_USQ16", 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_HSwish_HWC_USQ16",   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_LeakyReLU_HWC_USQ16", 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Sigmoid_HWC_USQ16", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerParReduct_CC_Tanh_HWC_USQ16", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_TANH),  	   1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));

        /* Activation and Reduct without PerChannel Scaling */
        LibKernel("KerReduct_CC_NoScale_SQ8",	   		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_NONE),	   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLU_SQ8", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUN_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUM_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUMN_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_HSigmoid_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_HSwish_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_LeakyReLU_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_Sigmoid_SQ8",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_Tanh_SQ8", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_TANH),      1, CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerReduct_CC_NoScale_SQ16",	   		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_NONE),	   1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLU_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELU),      1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUN_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUN),     1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUM_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUM),     1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUMN_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_RELUMN),    1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_HSigmoid_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_HSIGMOID),  1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_HSwish_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_HSWISH),    1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_LeakyReLU_SQ16",	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_Sigmoid_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_SIGMOID),   1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_Tanh_SQ16",	 	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE), CNN_OperList(1, KOP_TANH),      1, CNN_Type(4,0,0,0,2), 0,0,0,0,0,0));

        /* Unsigned */
        LibKernel("KerReduct_CC_NoScale_USQ8",  	   	CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_NONE),  1, CNN_Type(4,0,0,0,-1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLU_USQ8", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELU),  1, CNN_Type(4,0,0,0,-1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUN_USQ8",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUN), 1, CNN_Type(4,0,0,0,-1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUM_USQ8",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUM), 1, CNN_Type(4,0,0,0,-1), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUMN_USQ8",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUMN),1, CNN_Type(4,0,0,0,-1), 0,0,0,0,0,0));

        LibKernel("KerReduct_CC_NoScale_USQ16",  		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_NONE),  1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLU_USQ16", 		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELU),  1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUN_USQ16",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUN), 1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUM_USQ16",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUM), 1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));
        LibKernel("KerReduct_CC_NoScale_ReLUMN_USQ16",		CALL_PARALLEL|CALL_HWC_KER, 0, "KerConvLinReduct_SQ8_T", CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_NOSCALE),CNN_OperList(1, KOP_RELUMN),1, CNN_Type(4,0,0,0,-2), 0,0,0,0,0,0));

	/* Activations with tensor centric scaling */
        LibKernel("Ker_Scale_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_SCALE), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("Ker_ActNone_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_ACT_NONE), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELU),     0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUN_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELUN),    0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUM_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELUM),    0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUMN_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELUMN),   0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_HSIGMOID), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSwish_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_HSWISH),   0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_LEAKYRELU),0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_Sigmoid_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_SIGMOID),  0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_Tanh_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_TANH),  0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("Ker_ActNone_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_ACT_NONE_IN_SCALE), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLU_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_RELU_IN_SCALE),     0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUN_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_RELUN_IN_SCALE),    0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUM_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_RELUM_IN_SCALE),    0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUMN_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_RELUMN_IN_SCALE),   0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSigmoid_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_HSIGMOID_IN_SCALE), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSwish_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_HSWISH_IN_SCALE),   0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_LeakyReLU_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_LEAKYRELU_IN_SCALE),0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_Sigmoid_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_SIGMOID_IN_SCALE),  0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_Tanh_ScaleIn_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_TANH_IN_SCALE),  0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

	/* Pooling (Max or Avg) with tensor centric scaling and optional ReLU or ReLUN activation */
        LibKernel("KerPool2x2Stride2_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerPool2x2Stride2_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerPool2x2Stride2_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 0,
												  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));

        LibKernel("KerPoolNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerPoolNxNStrideS_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerPoolNxNStrideS_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 0,
												  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));

        LibKernel("KerPoolNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerPoolNxMStrideSxSy_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerPoolNxMStrideSxSy_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 0,
        										 	  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));

        /* Unsigned int8 input/output functions for NE16 */
        LibKernel("KerParMaxPoolNxMStrideSxSy_HWC_USQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Pool_USQ8_T",
        										CNN_Match(CNN_OperList(1, KOP_MAXPOOL), 0, 1,
        										 	  CNN_Type(-1,0,0,0,-1), -1,-1,1,1,-1,-1));
        LibKernel("KerParAvgPoolNxMStrideSxSy_HWC_USQ8", CALL_PARALLEL|CALL_HWC_KER, 0, "Ker_MM_Pool_USQ8_T",
        										CNN_Match(CNN_OperList(1, KOP_AVGPOOL), 0, 1,
        										 	  CNN_Type(-1,0,0,0,-1), -1,-1,1,1,-1,-1));
        LoadCNN_Copy_Library();
}

/*********************************************************************************************************************************************************************
 	Generator for Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional Activation. Imm2Col based

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use HWCE), Def=(TILE_HOR, 1, 0)

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, KOP_NONE, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_MM_ConvolutionPoolAct_SQ8

*********************************************************************************************************************************************************************/

static Kernel_T *CNN_MM_ConvolutionPoolAct_SQ8_Internal(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)

{
	if (ConvOper==KOP_NONE) {
		if (PoolOper!=KOP_NONE)
			return CNN_PoolAct_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, All requested operations are KOP_NONE", Name);
	}

	int ParFeat = 1, HWC = 0, ParFeatConv = 2;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	// In HWC only you can have cases where you must tile also the input feature, in CHW if you fail the entire infeat solution then go back to the normal (non-MM) implementation
        // unsigned int InFeatProp = (InFeatOneTile!=0)?SPACE_PROP_ONE_TILE:0;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC) HWC = 1;
		if (Ctrl->ParallelFeatures != -1) ParFeatConv = Ctrl->ParallelFeatures;
	}

	if (HWC && Fcy==1 && Fcx==1 && Scy==1 && Scx==1 && Dcy==1 && Dcx==1)
		return CNN_MatMulAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, Height*Width, OutFeat, InFeat, 0,0,0,0, KOP_MATMUL_TRANSPOSED, ActOper, 0);

	if (ParFeatConv == 2 && HWC && Fcy>1 && (InFeat < 8))
		ParFeatConv = 0;
	else
		ParFeatConv = 1;
	unsigned int Bs = Bias_DataSize;
	KernelOper_T COper = ConvOper;
	int OverlapC, OverlapP;
	int TileCons;
	int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
	char *ConvKerName=0, *PoolKerName=0, *ActKerName=0;
	int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
	int UsedWidth, UsedHeight, UsedWc, UsedHc;
	int StandAloneAct = (ActOper!=KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int Log=1;
	int DWConv = ConvOper == KOP_CONV_DW;
	int Os = DWConv?D0:D1;

	if ((ConvOper != KOP_CONV) && (ConvOper != KOP_CONV_DW))
		GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, ConvOper, expecting KOP_CONV, KOP_CONV_DW", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);
        if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
			   &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
	PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
	PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

	CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC, &OverlapP);
	UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
	UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
	UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
	UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);
	TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);


	/* Re evaluate now that we know exactly what is used */
	PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
	PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
	UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
	Wc = UsedWc; Hc = UsedHc;
	int KerLayout = HWC?CALL_HWC_KER:0;

	int BuffS = ALIGN(InFeat*Fcx*Fcy, 3);
	if (HWC) {
		if (Fcx==1&&Fcy==1) BuffS = 1;
		else if (ParFeatConv) BuffS = 2*ALIGN(InFeat*Fcx*Fcy, 3);
		else BuffS = 2 * InFeat*Fcx*Fcy*8;
	}

	/* Layer number of operations and memory bandwidth requirements */
	LayerOp += (int64_t) Wc*Hc*Fcx*Fcy*OutFeat;
        if (!DWConv) LayerOp *= InFeat;
	if (PoolOper) LayerOp += (int64_t) OutFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) OutFeat*Wo*Ho;
	LayerBandwidth += (int64_t) Width*Height*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += (int64_t) Wo*Ho*1*OutFeat;
	LayerBandwidth += (int64_t) Fcx*Fcy*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += (int64_t) Bias_DataSize*OutFeat;

	ConvKerName = CNN_FindMatchingKernelAttr(DWConv?KOP_MM_CONV_DW:KOP_MM_CONV, (PoolOper==KOP_NONE)?ActOper:KOP_NONE, ParFeatConv, KerLayout, 1, 1, Bias_DataSize, 0, 1, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
				     	     	 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName) {
		if (PoolOper==KOP_NONE) StandAloneAct = 0;
	} else ConvKerName = CNN_FindMatchingKernelAttr(DWConv?KOP_MM_CONV_DW:KOP_MM_CONV, KOP_NONE, ParFeatConv, KerLayout, 1, 1, Bias_DataSize, 0, 1, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
	 				            	 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Convolution basic kernel", Name);

	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) 
			PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
							     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		else if (ActOper) StandAloneAct = 0;
		else if (PoolOper==KOP_AVGPOOL && ActOper==KOP_NONE && HWC) {
			StandAloneAct = 1; ActOper = KOP_SCALE;
		}
		if (PoolKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
	}
	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, 0, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (Log) {
		printf("InFeat: %d, OutFeat: %d%s%s\n", InFeat, OutFeat, HWC?", HWC":", CHW", ParFeatConv?", Out Chan Parallel":", H Parallel");
        	printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
        	printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
        	printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
        	printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
        	printf("OverlapC: %d\n", OverlapC);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
		if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
		
	}
	int OutTileCons = 8;
	int TileFirst = 0; // ((InFeat*OutFeat*Fcx*Fcy) < (InFeat*Width*Height));
        Kernel_T *Kernel = UserKernel(Name,
        	DWConv?
		KernelIterSpace(2, IterParSpace(D0, InFeat, OutTileCons), IterTiledSpace(T0)):
		TileFirst?
		KernelIterSpace(3, IterTiledSpace(T0), IterParSpace(D1, OutFeat, OutTileCons), IterParSpace(D0|SPACE_PROP_ONE_TILE, InFeat, InFeat)):
		KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0|SPACE_PROP_ONE_TILE, InFeat, InFeat)),
                TileOrientation,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  "In"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Filter"),
                      TCArg(CNN_ArgDataType(Bs,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"),
                      TCArg(CNN_ArgDataType(1,1,1),"ScaleN"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(3,
			Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
				Bindings(22,
					K_Arg("In", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_W),
					K_Arg("In", KER_ARG_TILE_H),
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,					/* Conv Fx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,					/* Conv Fy */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,					/* Conv Stridex */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,					/* Conv Stridey */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,					/* Conv x dilation */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING,					/* Conv y dilation */
					K_ArgPred("In", KER_ARG_TILEFIRST, T0), 				/* First Tile */
					K_Arg("In", KER_ARG_TILE_PAD),						/* Conv Padding */
					K_Arg("Filter", KER_ARG_TILE),
					K_Arg("Bias", KER_ARG_TILE),
					K_Arg(PoolKerName?"ConvOut":"Out", KER_ARG_TILE),
					DWConv?K_ArgPar(PoolKerName?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):Imm(InFeat), /* In Features */
					K_ArgPar(PoolKerName?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os),	/* Out Features */
					K_Arg(PoolKerName?"ConvOut":"Out", KER_ARG_TILE_W),
					K_Arg(PoolKerName?"ConvOut":"Out", KER_ARG_TILE_H),
					K_Arg("Scale", KER_ARG_TILE),
					K_Arg("ScaleN", KER_ARG_TILE),
					K_Arg("KerBuff", KER_ARG_TILE),
					K_Arg("Infos", KER_ARG_TILE)
				)
			),
			(PoolKerName==0)?AT_NO_CALL:
			Call(PoolKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				(HWC==0)?
				Bindings(18,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Pooling input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Pooling input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_USEDW),					/* Pooling input tile width, used part of it */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Pooling input tile height */
					K_Arg("ConvOut", KER_ARG_TILE_USEDH),					/* Pooling input tile height, used part of it */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),				/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pooling Pad */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,					/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,					/* Pooling Dy */
					Imm((PoolOper==KOP_MAXPOOL)?1:0),					/* PoolMax or PoolAverage */
					AT_IGNORE_ARG_BINDING,							/* Pooling Orientation when feature parallel */
					Imm((ActOper==KOP_NONE)),						/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				):
				Bindings(13,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Input tile height */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pool Fx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pool Fy */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pool Stridex */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pool Stridey */
					K_ArgPred("ConvOut", KER_ARG_TILEFIRST, T0), 				/* First Tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pool Padding */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),				/* In Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Output tile width */
					K_Arg("Out", KER_ARG_TILE_H)						/* Output tile height */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),						/* Input tile */
					K_Arg("Out", KER_ARG_TILE),						/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, Os),				/* Number of Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),						/* Input tile height */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			)
		),
		(HWC==0)?
		KerArgs(9,
			KerArg ("KerBuff",KerArgSpace(1,T0),    O_BUFF|O_NTILED, BuffS,      1,                             	 	1,          0, 0,        0, 0),
			KerArgP("In",     KerArgSpace(2,D0,T0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC, 0, TileCons, "In"),
			KerArg ("Bias",   KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         Bs,         0, 0,        0, "Bias"),
			KerArg ("Scale",  KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "Scale"),
			KerArg ("ScaleN", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "ScaleN"),
			DWConv?
			KerArg ("Filter", KerArgSpace(1,Os),	O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         1,          0, 0,        0, "Filter"):
			KerArg ("Filter", KerArgSpace(2,Os,D0), O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         1,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,Os,T0), O_OUT|O_DB,          Wo,    Ho,                                         1,          0, 0,        0, "Out"),
			(PoolKerName==0)?AT_NO_KER_ARG:
			KerArgP("ConvOut",KerArgSpace(2,Os,T0), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        1,   OverlapP, 0,        0, ""),
			KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1,     1,  					AT_INF_DIM, 0, 0,        0, "Infos")
		):
		KerArgs(9,
			KerArg ("KerBuff",KerArgSpace(1,T0),    O_BUFF|O_NTILED, BuffS,      1,                             	 	1,          0, 0,        0, 0),
			KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC, 0, TileCons, "In"),
			KerArg ("Bias",   KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         Bs,         0, 0,        0, "Bias"),
			KerArg ("Scale",  KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "Scale"),
			KerArg ("ScaleN", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "ScaleN"),
			DWConv?
			KerArg ("Filter", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         1,          0, 0,        0, "Filter"):
			KerArg ("Filter", KerArgSpace(2,Os,D0), O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         1,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,T0,Os), O_OUT|O_DB,          Wo,    Ho,                                         1,          0, 0,        0, "Out"),
			(PoolKerName==0)?AT_NO_KER_ARG:
			KerArgP("ConvOut",KerArgSpace(2,T0,Os), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        1,   OverlapP, 0,        0, ""),
			KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1,     1,  					AT_INF_DIM, 0, 0,        0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                AddKernelArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, 1);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 1);
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		if (Ctrl && (Ctrl->Out_L3)) SetKerArgInL3(Name, "Out");
		if (Ctrl && (Ctrl->Filter_L3)) SetKerArgInL3(Name, "Filter");
		AT_PrepareForTest_SQ8(Name, InFeat, OutFeat, Width, Height, Bias_DataSize,
				      ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, PadInc,
				      PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp,
				      ActOper);
	}
	return Kernel;
}


static Kernel_T *CNN_HWC_DWConvolutionPoolAct_SQ8_Internal(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)

{
	if (ConvOper==KOP_NONE) {
		if (PoolOper!=KOP_NONE)
			return CNN_PoolAct_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, All requested operations are KOP_NONE", Name);
	}

	int ParFeat = 1, HWC = 1;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
	}
	unsigned int Bs = Bias_DataSize;
	KernelOper_T COper = ConvOper;
	int OverlapC, OverlapP;
	int TileCons;
	int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
	char *MatPermKerName=0, *ConvKerName=0, *DPReductionKerName=0, *PoolKerName=0, *ActKerName=0;
	int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
	int UsedWidth, UsedHeight, UsedWc, UsedHc;
	int StandAloneAct = (ActOper!=KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int Log=1;

	if (ConvOper != KOP_CONV_DW)
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, ConvOper, expecting KOP_CONV", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);
        if (InFeat != OutFeat) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
			   &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
	PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
	PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

	CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC, &OverlapP);
	UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
	UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
	UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
	UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);
	TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);


	/* Re evaluate now that we know exactly what is used */
	PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
	PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
	UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
	Wc = UsedWc; Hc = UsedHc;
	int KerLayout = HWC?CALL_HWC_KER:0;
	int DP_OP = HWC?KOP_DP_REDUCT_CHW2HWC:KOP_DP_REDUCT;

	int BuffS = InFeat*Fcx*Fcy;

	/* Layer number of operations and memory bandwidth requirements */
	LayerOp += (int64_t) Wc*Hc*Fcx*Fcy*InFeat;
	if (PoolOper) LayerOp += (int64_t) InFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) InFeat*Wo*Ho;
	LayerBandwidth += (int64_t) Width*Height*1*InFeat;
	LayerBandwidth += (int64_t) Wo*Ho*1*InFeat;
	LayerBandwidth += (int64_t) Fcx*Fcy*1*InFeat;
	LayerBandwidth += (int64_t) Bias_DataSize*InFeat;

	ConvKerName = CNN_FindMatchingKernel(KOP_CONV_DW, (PoolOper==KOP_NONE)?ActOper:KOP_NONE, ParFeat, 1, 1, Bias_DataSize, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
				     	     &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName) {
		if (PoolOper==KOP_NONE) StandAloneAct = 0;
	} else ConvKerName = CNN_FindMatchingKernel(KOP_CONV_DW, KOP_NONE, ParFeat, 1, 1, Bias_DataSize, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
	 				            &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, Can't find a matching Convolution basic kernel", Name);

	MatPermKerName = CNN_FindMatchingKernelAttr(KOP_MATPERM_HWC2CHW, KOP_NONE, ParFeat, KerLayout, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
	if (MatPermKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching basic kernel for 3D Tensor Permutation", Name);

        if (PoolOper != KOP_NONE) {
                DPReductionKerName = CNN_FindMatchingKernelAttr(DP_OP, KOP_NONE, ParFeat, KerLayout, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (DPReductionKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
        } else {
                DPReductionKerName = CNN_FindMatchingKernelAttr(DP_OP, ActOper, ParFeat, KerLayout, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (DPReductionKerName==0)
                        DPReductionKerName = CNN_FindMatchingKernelAttr(DP_OP, KOP_NONE, ParFeat, KerLayout, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                else if (ActOper) StandAloneAct = 0;
                if (DPReductionKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
        }

	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) 
			PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
							     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		else if (ActOper) StandAloneAct = 0;
		if (PoolKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
	}
	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, 0, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_SQ8: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (Log) {
		printf("InFeat: %d, OutFeat: %d%s - TileOrientation: %s\n", InFeat, OutFeat, HWC?", HWC":", CHW", TileOrientation==TILE_HOR?"TILE_HOR":"TILE_VER");
        	printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
        	printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
        	printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
        	printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
        	printf("OverlapC: %d\n", OverlapC);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
		if (MatPermKerName) printf("%20s: %s\n", "MatPermKerName", MatPermKerName);
		if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
		if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
		
	}
	int OutTileCons = 8;
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T0), IterParSpace(D0, InFeat, 8)),
                TileOrientation,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  "In"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Filter"),
                      TCArg(CNN_ArgDataType(Bs,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"),
                      TCArg(CNN_ArgDataType(1,1,1),  "ScaleN"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(5,
			Call(MatPermKerName, LOC_D0,
				Bindings(7,
					K_Arg("In", KER_ARG_TILE),						/* Input tile */
					K_Arg("PermOut", KER_ARG_TILE),						/* Output tile */
					K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),				/* Out Features */
					K_Arg("In", KER_ARG_TILE_W),						/* Input tile width */
					K_Arg("In", KER_ARG_TILE_H),						/* Input tile height */
					AT_IGNORE_ARG_BINDING,							/* StrideX */
					AT_IGNORE_ARG_BINDING							/* StrideY */
				)
			),
                        Call(ConvKerName, LOC_D0,
                                Bindings(20, 
					K_Arg("PermOut", KER_ARG_TILE),						/* Conv input tile */
					K_Arg("In", KER_ARG_TILE_W),						/* Conv input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),					/* Conv input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),						/* Conv input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),					/* Conv input tile height, used part of it */
					K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),				/* Number of input features in this tile */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D0),				/* Number of output features in this tile */
					K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),			/* Total number of input features currently in L1 memory, argument promotion */
					K_Arg("Filter", KER_ARG_TILE),						/* Conv filter */
					K_Arg("Bias", KER_ARG_TILE),						/* Conv Bias when depth wise conv*/
					K_Arg("ConvOut", KER_ARG_TILE),						/* Conv output */
					K_Arg("In", KER_ARG_TILE_PAD),						/* Conv Padding */
					K_TileOper("Infos", "char *", '@', AT_INF_BIASN),			/* NormBias is depth wise conv */
					AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,					/* Conv Fx */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,					/* Conv Stridex */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,					/* Conv Dx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,					/* Conv Fy */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,					/* Conv Stridey */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING					/* Conv Dy */
					)
			),
			Call(DPReductionKerName, LOC_D0,							/* DP Reduction also take care of optional activation */
				Bindings(8,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Double precision input tile */
					K_Arg(PoolOper?"ConvOut":"Out", KER_ARG_TILE),				/* Single precision output tile, warning use IO kernel when In=Out */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D0),				/* Input tile Number of features */
					K_Arg("ConvOut", KER_ARG_TILE_W),				       	/* Input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_H),				       	/* Input tile height */
					K_Arg("Scale", KER_ARG_TILE),						/* Per channel scale tile */
					K_Arg("ScaleN", KER_ARG_TILE),						/* Per channel scale normalization tile */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			),
			(PoolKerName==0)?AT_NO_CALL:
			Call(PoolKerName, LOC_D0,
				Bindings(13,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Input tile height */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pool Fx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pool Fy */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pool Stridex */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pool Stridey */
					K_ArgPred("ConvOut", KER_ARG_TILEFIRST, T0), 				/* First Tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pool Padding */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D0),				/* In Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Output tile width */
					K_Arg("Out", KER_ARG_TILE_H)						/* Output tile height */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_D0,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),						/* Input tile */
					K_Arg("Out", KER_ARG_TILE),						/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),				/* Number of Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),						/* Input tile height */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			)
		),
		KerArgs(9,
			KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC, 0, TileCons, "In"),
			KerArgP("PermOut",KerArgSpace(2,D0,T0), O_BUFF|O_ONETILE,Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC, 0,        0, ""),
			KerArg ("Bias",   KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,   1,      1,                                         Bs,         0, 0,        0, "Bias"),
			KerArg ("Scale",  KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "Scale"),
			KerArg ("ScaleN", KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,   1,      1,                                         1,          0, 0,        0, "ScaleN"),
			KerArg ("Filter", KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         1,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,T0,D0), O_OUT|O_DB,          Wo,    Ho,                                         1,          0, 0,        0, "Out"),
			KerArgP("ConvOut",KerArgSpace(2,T0,D0), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        4,   OverlapP, 0,        0, ""),
			KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1,     1,  					AT_INF_DIM, 0, 0,        0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                AddKernelArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 1);
                AddKernelArgDim(Name, "Bias", 2, InFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, InFeat, 1);
                AddKernelArgDim(Name, "Scale", 2, InFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, InFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		if (Ctrl && (Ctrl->Out_L3)) SetKerArgInL3(Name, "Out");
		AT_PrepareForTest_SQ8(Name, InFeat, OutFeat, Width, Height, Bias_DataSize,
				      ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, PadInc,
				      PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp,
				      ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional Activation.

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use HWCE), Def=(TILE_HOR, 1, 0)

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, KOP_NONE, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_ConvolutionPoolAct_SQ8
	
*********************************************************************************************************************************************************************/

Kernel_T *CNN_ConvolutionPoolAct_SQ8_Internal(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)

{
	int Log=1;
	int ParFeat = 1, HWC = 0; 
	Tile_Orientation_T TileOrientation = TILE_HOR;
	AT_PadType PadType = PAD_BALANCED_LEFT;

	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;

	}
        if (ConvOper == KOP_CONV && Width  == 1 && Fcy > 1 && Fcx == 1) {
                // It's a 1D so swap x and y dimensions
                int Temp;
                Temp = Fcx; Fcx = Fcy; Fcy = Temp;
                Temp = Scx; Scx = Scy; Scy = Temp;
                Temp = Dcx; Dcx = Dcy; Dcy = Temp;
                Temp = Fpx; Fpx = Fpy; Fpy = Temp;
                Temp = Spx; Spx = Spy; Spy = Temp;
                Temp = Dpx; Dpx = Dpy; Dpy = Temp;
                Temp = Width; Width = Height; Height = Temp;
        }
	if (ConvOper==KOP_NONE) {
		if (PoolOper!=KOP_NONE)
			return CNN_PoolAct_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_SQ8_Internal(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, All requested operations are KOP_NONE", Name);
	} else if (HWC) {
		if (ConvOper == KOP_CONV_DW)
			return CNN_HWC_DWConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height,
								ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
								PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else
			return CNN_MM_ConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height,
							     ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
							     PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
	} else if (ConvOper==KOP_CONV && ((Fcx > 1 && Fcy == 1))) {
		// AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
		Kernel_T *Ok = CNN_MM_ConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height,
						       ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
				      		       PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		// AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Ok!=0) return Ok;
		if (Log) printf("No solution found for im2col scheme, reverting to standard implementation\n");
	}
	if (Fcx==1 && Fcy==1 && Scx==1 && Scy==1 && Dcx==1 && Dcy==1 && Height==1 && Width==1) {
		printf("This is a pointwise on 1x1 input --> Mapping to CNN_Linear_NE16\n");
		return CNN_LinearAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, KOP_LINEAR, ActOper);
	}

	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	KernelOper_T COper = ConvOper;
	int OverlapC, OverlapP;
	int TileCons;
	int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
	char *ConvKerName=0, *PoolKerName=0, *ActKerName=0, *SetBiasKerName=0, *DPReductionKerName=0;
	int DWConv=(ConvOper==KOP_CONV_DW);
	int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
	int Os=(DWConv?D0:D1);
	int UsedWidth, UsedHeight, UsedWc, UsedHc;
	int InTileCons = 4;
	int StandAloneAct = (ActOper!=KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	if (!(ConvOper == KOP_NONE || ConvOper == KOP_CONV || ConvOper == KOP_CONV_DW))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU || ActOper == KOP_SIGMOID || ActOper == KOP_TANH))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_RELUM, KOP_RELUMN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
			   &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
	PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
	PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

	CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC, &OverlapP);
	UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
	UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
	UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
	UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);
	TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);

	/* Re evaluate now that we know exactly what is used */
	PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
	PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
	UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
	Wc = UsedWc; Hc = UsedHc;

	/* Layer number of operations and memory bandwidth requirements */
	LayerOp += (int64_t) Wc*Hc*Fcx*Fcy*OutFeat;
	if (!DWConv) LayerOp *= (int64_t) InFeat;
	if (PoolOper) LayerOp += (int64_t) OutFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) OutFeat*Wo*Ho;
	LayerBandwidth += (int64_t) Width*Height*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += (int64_t) Wo*Ho*1*OutFeat;
	LayerBandwidth += (int64_t) Fcx*Fcy*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += (int64_t) Bias_DataSize*OutFeat;

	/* Basic Kernel Matching */
	if (!DWConv) {
		SetBiasKerName = CNN_FindMatchingKernel(KOP_SETBIAS, KOP_NONE, ParFeat, Bias_DataSize, 0, 0, 0, 4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Set Bias basic kernel", Name);
	}

	if (ConvOper == KOP_CONV && Height == 1 && Fcx != 1 && Fcy == 1) ConvOper = KOP_CONV1D;
	ConvKerName = CNN_FindMatchingKernel(ConvOper, KOP_NONE, ParFeat, 1, 1, DWConv?Bias_DataSize:0, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
					     &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Convolution basic kernel", Name);

	if (PoolOper != KOP_NONE) {
		DPReductionKerName = CNN_FindMatchingKernel(KOP_DP_REDUCT_IO, KOP_NONE, ParFeat, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (DPReductionKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
	} else {
		DPReductionKerName = CNN_FindMatchingKernel(KOP_DP_REDUCT, ActOper, ParFeat, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (DPReductionKerName==0)
			DPReductionKerName = CNN_FindMatchingKernel(KOP_DP_REDUCT, KOP_NONE, ParFeat, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		else if (ActOper) StandAloneAct = 0;
		if (DPReductionKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
	}

	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernel(PoolOper, ActOper, ParFeat, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						     &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) 
			PoolKerName = CNN_FindMatchingKernel(PoolOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
							     &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		else if (ActOper) StandAloneAct = 0;
		if (PoolKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
	}
	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (Log) {
		printf("InFeat: %d, OutFeat: %d\n", InFeat, OutFeat);
        	printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
        	printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
        	printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
        	printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
        	printf("OverlapC: %d\n", OverlapC);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
		if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
		if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
		if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
		
	}
	if (Ctrl && (Ctrl->EnableIm2Col==1) && (ConvOper==KOP_CONV) && (PoolOper==KOP_NONE) && (Fcx==1) && (Fcy==1) && (Dcx==1) && (Dcy==1)) {
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
		// if ((InFeat+OutFeat)<80) {
		if ((InFeat+OutFeat)<100 && (Scx==1) && (Scy==1)) {
			if (Log) printf("Mapping this convolution to matrix multiplication with small first operand\n");
			Kernel_T *Ok = CNN_MatMulSmallM1Act_SQ8_Internal(Name, 0, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL_SM1, ActOper);
			if (!Ok&&Log) printf("Mapping this convolution to matrix multiplication with small first operand FAILED, trying with standard mult implementation\n");
			if (Ok) return Ok;
		}
		if (Log) printf("Mapping this convolution to matrix multiplication\n");
		Kernel_T *Ok = CNN_MatMulAct_SQ8_Internal(Name, 0, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL, ActOper, 1);
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Ok) return Ok;
		if (Log) printf("Mapping this convolution to matrix multiplication FAILED, reverting to standard implementation\n");
	}

	/* User kernel C arguments */
	CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	int Ca=0;

	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),     "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1), "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,        1,1),    "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,     1,1),  "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,        1,1), "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,        1,1),  "Infos");

	/* User kernel kernel arguments */
	Object_T **KArgs = AllocateKerArgs(8);
	int Ka=0;

	KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,D0,T0), O_IN|O_DB,     Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,      OverlapC, 0, TileCons, "In");
	KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,  1,     1,                      		    Bias_DataSize, 0, 0,        0, "Bias");
	KArgs[Ka++] = KerArg ("Scale",  KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,  1,     1,                       		    1,             0, 0,        0, "Scale");
	KArgs[Ka++] = KerArg ("ScaleN", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,  1,     1,                       		    1,             0, 0,        0, "ScaleN");
	if (DWConv)
	KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,  1,     1,                       		    Fcx*Fcy,       0, 0,        0, "Filter");
	else
	KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0), O_IN|O_DB|O_CONST,  1,     1,                       		    Fcx*Fcy,       0, 0,        0, "Filter");
       	KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,Os,T0), O_OUT|O_DB,        Wo,    Ho,                      		    1,             0, 0,        0, "Out");
	if (ParFeat)
	KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(2,Os,T0), O_BUFF|O_ONETILE,  Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        4,      OverlapP, 0,        0, "");
	else
	KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(1,T0),    O_BUFF|O_ONETILE,  Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        4,      OverlapP, 0,        0, "");

	KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,  		            1,      	   0, 0,        0, "Infos");

        Kernel_T *Kernel = UserKernel(Name,
		ParFeat?
		(DWConv?
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
		KernelIterSpace(3, IterParSpace(D1, OutFeat, 8), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons))):
		(DWConv?
		KernelIterSpace(2, IterFixedSpace(D0, InFeat), IterTiledSpace(T0)):
		KernelIterSpace(3, IterFixedSpace(D1, OutFeat), IterTiledSpace(T0), IterFixedSpace(D0, InFeat))),
                TileOrientation,
		KCArgs,
                Calls(5,
			(SetBiasKerName==0)?AT_NO_CALL:
                        Call(SetBiasKerName, LOC_D0_PROLOG,
                                Bindings(6,
					K_Arg("ConvOut", KER_ARG_TILE),						/* SetBias output tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* SetBias output tile width */
                                        K_Arg("ConvOut", KER_ARG_TILE_H),					/* SetBias output tile height */
					ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),		/* Number of output features in this tile */
					K_Arg("Bias", KER_ARG_TILE),						/* SetBias Bias tile */
					K_TileOper("Infos", "char *", '@', AT_INF_BIASN) 			/* Bias Norm */
					)),
                        Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
                                Bindings(20, 
					K_Arg("In", KER_ARG_TILE),						/* Conv input tile */
					K_Arg("In", KER_ARG_TILE_W),						/* Conv input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),					/* Conv input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),						/* Conv input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),					/* Conv input tile height, used part of it */
					K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),				/* Number of input features in this tile */
					ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),		/* Number of output features in this tile */
					K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),			/* Total number of input features currently in L1 memory, argument promotion */
					K_Arg("Filter", KER_ARG_TILE),						/* Conv filter */
					DWConv?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,		/* Conv Bias when depth wise conv*/
					K_Arg("ConvOut", KER_ARG_TILE),						/* Conv output */
					K_Arg("In", KER_ARG_TILE_PAD),						/* Conv Padding */
					DWConv?K_TileOper("Infos", "char *", '@', AT_INF_BIASN):AT_IGNORE_ARG_BINDING,	/* NormBias is depth wise conv */
					ParFeat?AT_IGNORE_ARG_BINDING:Imm((TileOrientation==TILE_HOR)?1:0),	/* Orientation when feature parallel */
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,					/* Conv Fx */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,					/* Conv Stridex */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,					/* Conv Dx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,					/* Conv Fy */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,					/* Conv Stridey */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING					/* Conv Dy */
					)
			),
			Call(DPReductionKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,					/* DP Reduction also take care of optional activation */
				Bindings(8,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Double precision input tile */
					K_Arg(PoolOper?"ConvOut":"Out", KER_ARG_TILE),				/* Single precision output tile, warning use IO kernel when In=Out */
					ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),		/* Input tile Number of features */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Input tile height */
					K_Arg("Scale", KER_ARG_TILE),						/* Per channel scale tile */
					K_Arg("ScaleN", KER_ARG_TILE),						/* Per channel scale normalization tile */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			),
			(PoolKerName==0)?AT_NO_CALL:
			Call(PoolKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(18,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Pooling input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Pooling input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_USEDW),					/* Pooling input tile width, used part of it */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Pooling input tile height */
					K_Arg("ConvOut", KER_ARG_TILE_USEDH),					/* Pooling input tile height, used part of it */
					ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),		/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pooling Pad */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,					/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,					/* Pooling Dy */
					Imm((PoolOper==KOP_MAXPOOL)?1:0),					/* PoolMax or PoolAverage */
					ParFeat?AT_IGNORE_ARG_BINDING:Imm((TileOrientation==TILE_HOR)?1:0),	/* Pooling Orientation when feature parallel */
					Imm((ActOper==KOP_NONE)),						/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),						/* Input tile */
					K_Arg("Out", KER_ARG_TILE),						/* Output tile */
					ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, Os):Imm(1),		/* Number of features in this tile */
					K_Arg("Out", KER_ARG_TILE_W),						/* Tile width */
					K_Arg("Out", KER_ARG_TILE_H),						/* Tile height */
					K_Arg("Infos", KER_ARG_TILE)						/* Infos */
				)
			)
                     ),
		KArgs
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, InFeat, Height, Width, 1);
		if (DWConv) AddKernelArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 1);
		else AddKernelArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, 1);
		AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
		AddKernelArgDim(Name, "Out", 4, OutFeat, Ho, Wo, 1);
		AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
		AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		if (Ctrl && (Ctrl->In_L3)) SetKerArgInL3(Name, "In");
		if (Ctrl && (Ctrl->Filter_L3)) SetKerArgInL3(Name, "Filter");
		if (Ctrl && (Ctrl->Bias_L3)) SetKerArgInL3(Name, "Bias");
		if (Ctrl && (Ctrl->Out_L3)) SetKerArgInL3(Name, "Out");
		if (Ctrl && (Ctrl->Scale_L3)) SetKerArgInL3(Name, "Scale");
		if (Ctrl && (Ctrl->ScaleN_L3)) SetKerArgInL3(Name, "ScaleN");

		AT_PrepareForTest_SQ8(Name, InFeat, OutFeat, Width, Height, Bias_DataSize,
				      ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, PadInc,
				      PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp,
				      ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for Grouped Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional activation.

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

		GroupIn:	Size of the group for input features
		GroupOut:	Size of the group for output features

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, KOP_NONE, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_GroupedConvolutionPoolAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_GroupedConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int GroupIn,
	int GroupOut,

	int Bias_DataSize,
	int Scale_DataSize,

	int InFeat,
	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)

{
	char *BodyName = AppendNames(Name, "Body");
	KernelGroup_T *UKGroup;
	int g, Wc, Hc, Wo, Ho;
	int NGroups = InFeat/GroupIn;

	if (!(ConvOper == KOP_CONV))
		GenTilingError("CNN_GroupedConvolutionPoolAct_SQ8: Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_GroupedConvolutionPoolAct_SQ8: Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_GroupedConvolutionPoolAct_SQ8: Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, &Wc, &Hc, &Wo, &Ho, 0, 0, 0, 0);

	if ((InFeat%GroupIn)||(OutFeat%GroupOut)||((InFeat/GroupIn)!=(OutFeat/GroupOut)))
		GenTilingError("CNN_GroupedConvolutionPoolAct_SQ8: %s cannot divide In(%d)/Out(%d) feature spaces with these group parameters: GroupIn %d, GroupOut: %d",
				Name, InFeat, OutFeat, GroupIn, GroupOut);

	OpenKernelGroup(Name);
	CNN_ConvolutionPoolAct_SQ8(BodyName, Ctrl,
				   Bias_DataSize, Scale_DataSize,
                        	   GroupIn, GroupOut, Width, Height,
				   ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
				   PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
				   ActOper
			);
	CloseKernelGroup();

	CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),              "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),              "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),  "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),          "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,      1,1), "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,         1,1), "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,         1,1), "Infos");

	Object_T **KArgs = AllocateKerArgs(7);
	int Ka=0;
	KArgs[Ka++] = KerGroupArg("In",	    O_IN,  NGroups*GroupIn*Width*Height,     1,               "In");
	KArgs[Ka++] = KerGroupArg("Filter", O_IN,  NGroups*GroupIn*GroupOut*Fcx*Fcy, 1,           "Filter");
	KArgs[Ka++] = KerGroupArg("Bias",   O_IN,  NGroups*GroupOut,                 Bias_DataSize, "Bias");
	KArgs[Ka++] = KerGroupArg("Out",    O_OUT, NGroups*GroupOut*Wo*Ho,           1,              "Out");
	KArgs[Ka++] = KerGroupArg("Scale",  O_IN,  NGroups*GroupOut,                 1,            "Scale");
	KArgs[Ka++] = KerGroupArg("ScaleN", O_IN,  NGroups*GroupOut,                 1,           "ScaleN");
	KArgs[Ka++] = KerGroupArg("Infos",  O_IN,  AT_INF_DIM,                 	     1,            "Infos");

        UKGroup = UserKernelGroupK(Name,
		NGroups,
		KCArgs,
		0,
		Calls(1,
		      UserKernelCall(BodyName, LOC_GROUP,
				Bindings(7,
					KG_ArgOper("In",     '*', GroupIn*Width*Height),
					KG_ArgOper("Filter", '*', GroupIn*GroupOut*Fcx*Fcy),
					KG_ArgOper("Bias",   '*', GroupOut*Bias_DataSize),
					KG_ArgOper("Out",    '*', GroupOut*Wo*Ho),
					KG_ArgOper("Scale",  '*', GroupOut),
					KG_ArgOper("ScaleN", '*', GroupOut),
					KG_ArgOper("Infos",  '+', 0)

				)
			)
		),
                KArgs
	);
	return (UKGroup!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Pooling (Max or Average) with tensor centric scaling followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_MAXPOOL or KOP_AVGPOOL
		Fpx:		Size of the pooling filter, x dimension
		Fpy:		Size of the pooling filter, y dimension
		Dpx:		Dilation factor, x dimension
		Dpy:		Dilation factor, y dimension
		Spx:		Pooling stride, x dimension
		Spy:		Pooling stride, y dimension

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In, Out, Infos)

	CNN_PoolAct_SQ8
		
*********************************************************************************************************************************************************************/

Kernel_T * CNN_PoolAct_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)

{
	if (PoolOper==KOP_NONE && ActOper!=KOP_NONE) return CNN_Act_SQ8_Internal(Name, Ctrl, Feat, Width, Height, ActOper);

	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1, HWC = 0;
	int Ids = 1, Ods = 1;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC) HWC = 1;
		if (Ctrl->InDataSize) Ids = Ctrl->InDataSize;
		if (Ctrl->OutDataSize) Ods = Ctrl->OutDataSize;
	}
	int TileCons, NeedFpx=0, NeedFpy=0, NeedDpx=0, NeedDpy=0, NeedSpx=0, NeedSpy=0, OverlapP;
	int Wo, Ho;
	int UsedWidth, UsedHeight;
	int PadPw=0, PadPh=0;
	v4s PadInp = (v4s){0,0,0,0};
	char *PoolKerName=0, *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=KOP_NONE);
	int KerLayout = HWC?CALL_HWC_KER:0;
	int Log=1;

	if (!(PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_Pool_SQ8 Kernel: %s, PoolOper, expecting KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_Pool_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	/* Set Kernel characteristics */
	CNN_LayerOutputDim(Width, Height, KOP_NONE, 1, 1, 1, 1, 1, 1, 1, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, 0, 0, &Wo, &Ho, 0, 0, &PadPw, &PadPh);
	PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
	CNN_TileOverlap(TileOrientation, 1, 1, 1, 1, 1, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy, 0, &OverlapP);
	UsedWidth  = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHeight = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
	TileCons = (TileOrientation==TILE_HOR)?(Spy):(Spx);
	/* Re evaluate truly used width/height and update padding accordingly */
	PadInp[1] = Max(0, PadInp[1]-(Width-UsedWidth)); PadInp[3] = Max(0, PadInp[3]-(Height-UsedHeight));

	PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, Ids, 0, 0, 0, Ods, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
	if (PoolKerName==0)
		PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, Ids, 0, 0, 0, Ods, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
	else if (ActOper) StandAloneAct = 0;
	if (PoolKerName==0) GenTilingError("CNN_Pool_SQ8 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, 0, Ods, 0, 0, 0, Ods, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_Pool_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (PoolOper) LayerOp += (int64_t) Feat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) Feat*Wo*Ho;

	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) Wo*Ho*1*Feat;

	if (Log) {
        	printf("Pool => W: %d, Pad:[%d,%d] => Wo: %d%s\n", Width,  PadInp[0], PadInp[1], Wo, HWC?", HWC":", CHW");
        	printf("     => H: %d, Pad:[%d,%d] => Ho: %d\n", Height, PadInp[2], PadInp[3], Ho);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	CKernel_Arg_T **KCArgs = AllocateCArgs(3);
	int Ca=0;
	KCArgs[Ca++] = TCArg(Ids>0?CNN_ArgDataType(1,1,1):CNN_ArgDataTypeUns(1,1,1),     "In");
       	KCArgs[Ca++] = TCArg(Ods>0?CNN_ArgDataType(1,1,1):CNN_ArgDataTypeUns(1,1,1),    "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),  "Infos");

	Object_T **KArgs = AllocateKerArgs(3);
	int Ka=0;
	if (HWC==0) {
		KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,D0,T0), OBJ_IN_DB,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, 1, OverlapP, 0, TileCons, "In");
		KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,D0,T0), OBJ_OUT_DB,     Wo,     Ho,                   		        1,        0, 0,        0, "Out");
		KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,   1,  	                1,        0, 0,        0, "Infos");
	} else {
		KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,T0,D0), OBJ_IN_DB,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, 1, OverlapP, 0, TileCons, "In");
		KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,T0,D0), OBJ_OUT_DB,     Wo,     Ho,                   		        1,        0, 0,        0, "Out");
		KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,   1,  	                1,        0, 0,        0, "Infos");
	}

        Kernel_T *Kernel = UserKernel(Name,
		ParFeat?
		KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
		KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
                TileOrientation,
                KCArgs,
                Calls(2,
			Call(PoolKerName, LOC_LOOP,
				(HWC==0)?
				Bindings(18,
					K_Arg("In", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_W),
					K_Arg("In", KER_ARG_TILE_USEDW),
					K_Arg("In", KER_ARG_TILE_H),
					K_Arg("In", KER_ARG_TILE_USEDH),
					ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* Number of features in this tile */
					K_Arg("Out", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_PAD),
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,				/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,				/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,				/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,				/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,				/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,				/* Pooling Dy */
					Imm((PoolOper==KOP_MAXPOOL)?1:0),				/* PoolMax or PoolAvg */
					Imm((TileOrientation==TILE_HOR)?1:0),				/* Pooling Orientation when feature parallel */
					Imm((ActOper==KOP_NONE)),					/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)					/* Infos */
				):
                                Bindings(13,
                                        K_Arg("In", KER_ARG_TILE),                             		/* Input tile */
                                        K_Arg("In", KER_ARG_TILE_W),                          		/* Input tile width */
                                        K_Arg("In", KER_ARG_TILE_H),                         		/* Input tile height */
                                        NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,                		/* Pool Fx */
                                        NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,                		/* Pool Fy */
                                        NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,                		/* Pool Stridex */
                                        NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,                		/* Pool Stridey */
                                        K_ArgPred("In", KER_ARG_TILEFIRST, T0),                		/* First Tile */
                                        K_Arg("In", KER_ARG_TILE_PAD),                         		/* Pool Padding */
                                        K_Arg("Out", KER_ARG_TILE),                            		/* Pooling output tile */
                                        ParFeat?K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* In Features */
                                        K_Arg("Out", KER_ARG_TILE_W),                          		/* Output tile width */
                                        K_Arg("Out", KER_ARG_TILE_H)                           		/* Output tile height */
                                )

			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),					/* Input tile */
					K_Arg("Out", KER_ARG_TILE),					/* Output tile */
					ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* Number of features in this tile */
					K_Arg("Out", KER_ARG_TILE_W),					/* Tile width */
					K_Arg("Out", KER_ARG_TILE_H),					/* Tile height */
					K_Arg("Infos", KER_ARG_TILE)					/* Infos */
				)
			)
                     ),
                KArgs
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, Ho, Wo, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Feat, Feat, Width, Height, 1, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp, 0, 0,0,0,0,0,0,(v4s) 0, ActOper);
	}
	return Kernel;
}



/*********************************************************************************************************************************************************************
 	Generator for Activation with tensor centric scaling

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ActOper:	KOP_ACT_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID
				KOP_ACT_NONE_IN_SCALE, KOP_RELU_IN_SCALE, KOP_RELUN_IN_SCALE, KOP_HSIGMOID_IN_SCALE, KOP_HSWISH_IN_SCALE, KOP_LEAKYRELU_IN_SCALE, KOP_SIGMOID_IN_SCALE

		Signature:	Name(In, Out, Infos)

	CNN_Act_SQ8
		
*********************************************************************************************************************************************************************/

Kernel_T * CNN_Act_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

	KernelOper_T ActOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0, HWC = 0;
	int TileCons = 0;
	char *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=KOP_NONE);
	int Log=1;

	if (!(ActOper == KOP_ACT_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU || ActOper == KOP_SIGMOID || ActOper == KOP_TANH ||
	      ActOper == KOP_ACT_NONE_IN_SCALE || ActOper == KOP_RELU_IN_SCALE || ActOper == KOP_RELUN_IN_SCALE || ActOper == KOP_RELUM_IN_SCALE || ActOper == KOP_RELUMN_IN_SCALE || ActOper == KOP_HSIGMOID_IN_SCALE || ActOper == KOP_HSWISH_IN_SCALE || ActOper == KOP_LEAKYRELU_IN_SCALE || ActOper == KOP_SIGMOID_IN_SCALE || ActOper == KOP_TANH_IN_SCALE))
		GenTilingError("CNN_Act_SQ8 Kernel: %s, ActOper, expecting KOP_ACT_NONE, KOP_RELU, KOP_RELUN, KOP_RELUM, KOP_RELUMN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (ActKerName==0) GenTilingError("CNN_Act_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);

	LayerOp += (int64_t) Feat*Width*Height;

	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;

	if (Log) {
        	printf("Act  => W: %d, Wo: %d\n", Width,  Width);
        	printf("     => H: %d, Ho: %d\n", Height, Height);
		printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(1,1,1), "In"),
                      TCArg(CNN_ArgDataType(1,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                ),
                Calls(1,
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("In",  KER_ARG_TILE),					/* Input tile */
					K_Arg("Out", KER_ARG_TILE),					/* Output tile */
					K_Arg("In", KER_ARG_TILE_H),					/* Tile height */
					Imm(1),
					Imm(1),
					K_Arg("Infos", KER_ARG_TILE)					/* Infos */
				)
			)
                     ),
                KerArgs(3,
			KerArg("In",     KerArgSpace(1,T0), OBJ_IN_DB,                    1, Width*Height*Feat, 1, 0, 0, 0, "In"),
			KerArg("Out",    KerArgSpace(1,T0), OBJ_OUT_DB,                   1, Width*Height*Feat, 1, 0, 0, 0, "Out"),
			KerArg("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST, 1, AT_INF_DIM,        1, 0, 0, 0, "Infos")
		)
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, ActOper, 0,0,0,0,0,0,(v4s) 0, 0, 0,0,0,0,0,0,(v4s) 0, KOP_NONE);
	}
	return Kernel;
}


/*********************************************************************************************************************************************************************
 	Generator for Global Pooling (Max or Average) with tensor centric scaling and optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL

		ActOper:	Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In, Out, Infos)


	CNN_GlobalPoolAct_SQ8
		
*********************************************************************************************************************************************************************/

static Kernel_T *CNN_GlobalPoolAct_SQ8_Interal(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

	KernelOper_T PoolOper,
	KernelOper_T ActOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1, HWC=0;
	int Wo, Ho;
	char *PoolKerName=0, *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=0);
	int Log=1;

	if (Ctrl) {
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
	}
	int KerLayout = HWC?CALL_HWC_KER:0;

	if (!(PoolOper == KOP_GLOBAL_MAXPOOL || PoolOper == KOP_GLOBAL_AVGPOOL || PoolOper == KOP_GLOBAL_SUMPOOL))
		GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, PoolOper should be KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL or KOP_GLOBAL_SUMPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (PoolKerName) StandAloneAct = 0;
	else if (StandAloneAct) PoolKerName = CNN_FindMatchingKernel(PoolOper, KOP_NONE, (HWC==0)?ParFeat:0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (PoolKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

	if (StandAloneAct) {
	       	ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		if (ActKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

        Wo = 1; Ho = 1;


	if (PoolOper) LayerOp += (int64_t) Feat*Wo*Ho*Width*Height;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) Wo*Ho*1*Feat;

	if (Log) {
        	printf("Global Pool => W: %d => Wo: %d\n", Width,  Wo);
        	printf("            => H: %d => Ho: %d\n", Height, Ho);
        	printf("            => Feat: %d\n", Feat);
		if (PoolKerName)      printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName)       printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

        Kernel_T *Kernel;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	/* First try moving entire features only */
       	Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(1,1,1), "In"),
                      TCArg(CNN_ArgDataType(1,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(2,
			Call(PoolKerName, LOC_LOOP,
				Bindings(8,
					K_Arg("In", KER_ARG_TILE),				/* In tile */
					Imm(Width),						/* In tile width */
					Imm(Height),						/* In Tile Height */
					K_Arg("Out", KER_ARG_TILE_H),				/* Number of output features in this tile */
					AT_IGNORE_ARG_BINDING,					/* In Tile Index, ignored here */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					Imm((ActOper==KOP_NONE)),				/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
		 	),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),				/* Input tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					K_Arg("Out", KER_ARG_TILE_H),				/* Number of features in this tile */
					Imm(1),							/* Tile width */
					Imm(1),							/* Tile height */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
		 	)
                ),
                KerArgs(3,
			KerArg("In",     KerArgSpace(1,T0), OBJ_IN_DB,                    Width*Height, Feat, 1, 0, 0, 8, "In"),
                        KerArg("Out",    KerArgSpace(1,T0), OBJ_OUT_DB,                              1, Feat, 1, 0, 0, 0, "Out"),
			KerArg("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST,   AT_INF_DIM,    1, 1, 0, 0, 0, "Infos")
		)
        );
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
	if (Kernel == 0) {
		char *PoolKerName=0, *PoolReductKerName=0, *ActKerName=0;
		int StandAloneAct = (ActOper!=KOP_NONE);
		KernelOper_T ReductOper = (PoolOper==KOP_GLOBAL_MAXPOOL)?KOP_GLOBAL_MAXPOOL_REDUCT:((PoolOper==KOP_GLOBAL_AVGPOOL)?KOP_GLOBAL_AVGPOOL_REDUCT:KOP_GLOBAL_SUMPOOL_REDUCT);

		PoolKerName = CNN_FindMatchingKernel(PoolOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		if (PoolKerName==0)       GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching global pooling basic kernel", Name);

		PoolReductKerName = CNN_FindMatchingKernel(ReductOper, ActOper, ParFeat, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		if (PoolReductKerName) StandAloneAct = 0;
		else if (StandAloneAct) PoolReductKerName = CNN_FindMatchingKernel(ReductOper, KOP_NONE, ParFeat, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		if (PoolReductKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching reduction basic kernel", Name);

		if (StandAloneAct) {
			ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			if (ActKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching activation basic kernel", Name);
		}
		if (Log) {
        		printf("Global Pool DP => W: %d => Wo: %d\n", Width,  Wo);
        		printf("               => H: %d => Ho: %d\n", Height, Ho);
        		printf("               => Feat: %d\n", Feat);
			if (PoolKerName)       printf("%20s: %s\n", "PoolKerName", PoolKerName);
			if (PoolReductKerName) printf("%20s: %s\n", "PoolReductKerName", PoolReductKerName);
			if (ActKerName)        printf("%20s: %s\n", "ActKerName", ActKerName);
			printf("Nb Oper : %lld\n", LayerOp);
		}
       		Kernel = UserKernel(Name,
			KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)),
                	TileOrientation,
                	CArgs(3,
                      		TCArg(CNN_ArgDataType(1,1,1), "In"),
                      		TCArg(CNN_ArgDataType(1,1,1), "Out"),
                      		TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     	),
	                Calls(3,
				Call(PoolKerName, LOC_LOOP,
					Bindings(8,
						K_Arg("In", KER_ARG_TILE),				/* In tile */
						K_Arg("In", KER_ARG_TILE_W),				/* In tile width */
						K_Arg("In", KER_ARG_TILE_H),				/* In Tile Height */
						K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),		/* Number of output features in this tile */
						K_Arg("In", KER_ARG_TILEINDEX),				/* In Tile Index */
						K_Arg("DPOut", KER_ARG_TILE),				/* Output tile */
						AT_IGNORE_ARG_BINDING,					/* Scaling when no activation, not needed here */
						AT_IGNORE_ARG_BINDING					/* Infos, not needed here */
					)
				),
				Call(PoolReductKerName, LOC_LOOP_EPILOG,
					Bindings(8,
						K_Arg("DPOut", KER_ARG_TILE),				/* In tile */
						K_Arg("In", KER_ARG_TILE_W),				/* In tile width */
						K_Arg("In", KER_ARG_TILE_H),				/* In Tile Height */
						K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),		/* Number of output features in this tile */
						AT_IGNORE_ARG_BINDING,					/* In Tile Index, not needed here */
						K_Arg("Out", KER_ARG_TILE),				/* Output tile */
						Imm((ActOper==KOP_NONE)),				/* Scaling when no activation */
						K_Arg("Infos", KER_ARG_TILE)				/* Infos */
					)
				),
				(ActKerName==0)?AT_NO_CALL:
				Call(ActKerName, LOC_LOOP_EPILOG,
					Bindings(6,
						K_Arg("Out", KER_ARG_TILE),				/* Input tile */
						K_Arg("Out", KER_ARG_TILE),				/* Output tile */
						K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),		/* Number of features in this tile */
						Imm(1),							/* Tile width */
						Imm(1),							/* Tile height */
						K_Arg("Infos", KER_ARG_TILE)				/* Infos */
					)
				)
	                ),
	                KerArgs(4,
				KerArg("In",     KerArgSpace(2,D0,T0), OBJ_IN_DB,                        Width, Height, 1, 0, 0, 0, "In"),
				KerArg("DPOut",  KerArgSpace(1,D0),    O_BUFF,                                1,      1, 4, 0, 0, 0,  ""),
	                        KerArg("Out",    KerArgSpace(1,D0),    OBJ_OUT_DB,                            1,      1, 1, 0, 0, 0, "Out"),
				KerArg("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, AT_INF_DIM,      1, 1, 0, 0, 0, "Infos")
			)
		);
	}
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, 1, 1, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, PoolOper, 0,0,0,0,0,0, (v4s)0, 0, 0,0,0,0,0,0,(v4s) 0, ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for Linear layers followed wth channel centric scaling followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InDim:		Number of inputs
		OutDim:		Number of outputs

		LinearOper	KOP_LINEAR
		ActOper		Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_LinearAct_SQ8
	
*********************************************************************************************************************************************************************/

Kernel_T * CNN_LinearAct_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

	int InDim,
	int OutDim,

	KernelOper_T LinearOper,
	KernelOper_T ActOper
	)

{
	int Log = 1;
	int Iter;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1;
	int StandAloneAct = (ActOper!=KOP_NONE);
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	char *LinearKerName=0, *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	/* First try to map on Linear Kernel without reduction, for that In and one full line of Weights must fit into L1 */
	if (LinearOper != KOP_LINEAR) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, only KOP_LINEAR should be used as LinearOper argument", Name);

	LinearKerName = CNN_FindMatchingKernel(LinearOper, ActOper, ParFeat, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (LinearKerName) StandAloneAct = 0;
	else if (StandAloneAct) LinearKerName = CNN_FindMatchingKernel(LinearOper, KOP_NONE, ParFeat, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (LinearKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Linear basic kernel", Name);

	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	LayerOp += (int64_t) InDim*OutDim;
	if (ActOper) LayerOp += (int64_t) OutDim;
	LayerBandwidth += (int64_t) InDim*OutDim*1;
	LayerBandwidth += (int64_t) OutDim*1;
	LayerBandwidth += (int64_t) InDim*OutDim*1;
	LayerBandwidth += (int64_t) Bias_DataSize*OutDim;

	if (Log) {
		printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s\n", Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
		if (LinearKerName) printf("Linear Kernel: %s\n", LinearKerName);
		if (ActKerName)    printf("Act Kernel: %s\n", ActKerName);
	}
	Kernel_T *Kernel;

	CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),             "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),             "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),         "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1),      "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),         "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),         "Infos");

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

	Object_T **KArgs = AllocateKerArgs(7);
	int Ka=0;
	KArgs[Ka++] = KerArg("In",      KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED, 1, 1,  InDim*1,       0, 0, 0, "In");
	KArgs[Ka++] = KerArg("Filter",  KerArgSpace(1,D0), OBJ_IN_DB|O_CONST,    1, 1,  InDim*1,       0, 0, 0, "Filter");
	KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D0), OBJ_IN_DB|O_CONST,    1, 1,  Bias_DataSize, 0, 0, 0, "Bias");
	KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D0), OBJ_OUT_DB,           1, 1,  1,             0, 0, 0, "Out");
	KArgs[Ka++] = KerArg("Scale",   KerArgSpace(1,D0), OBJ_IN_DB|O_CONST,    1, 1,  1,             0, 0, 0, "Scale");
	KArgs[Ka++] = KerArg("ScaleN",  KerArgSpace(1,D0), OBJ_IN_DB|O_CONST,    1, 1,  1,             0, 0, 0, "ScaleN");
	KArgs[Ka++] = KerArg("Infos",   KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED, 1, 1,  AT_INF_DIM*1,  0, 0, 0, "Infos");

        Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, OutDim, 8), IterTiledSpace(T0)),
                TileOrientation,
                KCArgs,
                Calls(2,
			Call(LinearKerName, LOC_LOOP,
				Bindings(10,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
					K_Arg("Filter", KER_ARG_TILE),				/* Filter tile */
					K_Arg("Bias", KER_ARG_TILE),				/* Bias tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					Imm(InDim),						/* Input tile size */
					Imm(InDim),						/* Total Input tile size */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),		/* Output tile size */
					K_Arg("Scale", KER_ARG_TILE),				/* Scale tile */
					K_Arg("ScaleN", KER_ARG_TILE),				/* Norm Scale tile */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP_EPILOG,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),				/* Input tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),		/* Number of features in this tile */
					Imm(1),							/* Tile width */
					Imm(1),							/* Tile height */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
			)
		),
                KArgs
	);
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
	if (Kernel==0) {
		char *SetBiasKerName=0, *LinearKerName=0, *ReductKerName=0;

		SetBiasKerName = CNN_FindMatchingKernel(KOP_SETBIAS, KOP_NONE, ParFeat, Bias_DataSize, 0, 0, 0, 4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (SetBiasKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Set Bias basic kernel", Name);

		LinearKerName = CNN_FindMatchingKernel(KOP_LINEAR, KOP_NONE, ParFeat, 1, 1, 0, 0, 4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (LinearKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Linear basic kernel", Name);

		ReductKerName = CNN_FindMatchingKernel(KOP_DP_REDUCT, ActOper, ParFeat, 4, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ReductKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
	
		if (Log) {
			printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s, output parallel failed, switching to feature parallel form\n",
				Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
			if (SetBiasKerName) printf("SetBias Kernel: %s\n", SetBiasKerName);
			if (LinearKerName)  printf("Linear Kernel   : %s\n", LinearKerName);
			if (ReductKerName)  printf("Reduction Kernel: %s\n", ReductKerName);
		}

		Object_T **KArgs = AllocateKerArgs(8);
		int Ka=0;
		KArgs[Ka++] = KerArg("In",     KerArgSpace(1,T0),    OBJ_IN_DB,            1, InDim,  1,             0, 0, 0, "In");
		KArgs[Ka++] = KerArg("Filter", KerArgSpace(2,D0,T0), OBJ_IN_DB|O_CONST,    1, InDim,  1,             0, 0, 0, "Filter");
		KArgs[Ka++] = KerArg("Bias",   KerArgSpace(1,D0),    OBJ_IN_DB|O_CONST,    1, 1,      Bias_DataSize, 0, 0, 0, "Bias");
		KArgs[Ka++] = KerArg("LinOut", KerArgSpace(1,D0),    O_BUFF|O_ONETILE,     1, 1,      4,             0, 0, 0, "");
		KArgs[Ka++] = KerArg("Out",    KerArgSpace(1,D0),    OBJ_OUT_DB,           1, 1,      1,             0, 0, 0, "Out");
		KArgs[Ka++] = KerArg("Scale",  KerArgSpace(1,D0),    OBJ_IN_DB|O_CONST,    1, 1,      1,             0, 0, 0, "Scale");
		KArgs[Ka++] = KerArg("ScaleN", KerArgSpace(1,D0),    OBJ_IN_DB|O_CONST,    1, 1,      1,             0, 0, 0, "ScaleN");
		KArgs[Ka++] = KerArg("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1, 1,      AT_INF_DIM*1,  0, 0, 0, "Infos");

	        Kernel = UserKernel(Name,
			KernelIterSpace(2, IterParSpace(D0, OutDim, 8), IterTiledSpace(T0)),
			// KernelIterSpace(2, IterFixedSpace(D0, OutDim), IterTiledSpace(T0)),
	                TileOrientation,
                	KCArgs,
	                Calls(3,
                        	Call(SetBiasKerName, LOC_LOOP_PROLOG,
                                	Bindings(6,
						K_Arg("LinOut", KER_ARG_TILE),			/* SetBias output tile */
						Imm(1),						/* SetBias output tile width */
						Imm(1),						/* SetBias output tile height */
						K_ArgPar("LinOut", KER_ARG_PARTILE_SIZE, D0),	/* Number of output features in this tile */
						K_Arg("Bias", KER_ARG_TILE),			/* SetBias Bias tile */
						K_TileOper("Infos", "char *", '@', AT_INF_BIASN)		/* Bias Norm */
						)
				),
				Call(LinearKerName, LOC_LOOP,
					Bindings(10,
						K_Arg("In", KER_ARG_TILE),			/* Input tile */
						K_Arg("Filter", KER_ARG_TILE),			/* Filter tile */
						AT_IGNORE_ARG_BINDING,				/* Bias tile, ignored */
						K_Arg("LinOut", KER_ARG_TILE),			/* Output tile */
						K_Arg("In", KER_ARG_TILE_H),			/* Input tile height, number of in elements */
						K_Arg("In", KER_ARG_TILE_H),			/* Total Input tile height, number of in elements */
						K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),	/* Output tile size, number of out elements */
						AT_IGNORE_ARG_BINDING,				/* Scale tile, ignored here */
						AT_IGNORE_ARG_BINDING,				/* Norm Scale tile, ignored here */
						K_Arg("Infos", KER_ARG_TILE)			/* Infos */
					)
				),
				Call(ReductKerName, LOC_LOOP_EPILOG,				/* Reduction also take care of optional activation */
					Bindings(8,
						K_Arg("LinOut", KER_ARG_TILE),			/* Double precision input tile */
						K_Arg("Out", KER_ARG_TILE),			/* Single precision output tile */
						K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),	/* Input tile Number of features */
						Imm(1),						/* Input tile width */
						Imm(1),						/* Input tile height */
						K_Arg("Scale", KER_ARG_TILE),			/* Per channel scale tile */
						K_Arg("ScaleN", KER_ARG_TILE),			/* Per channel scale normalization tile */
						K_Arg("Infos", KER_ARG_TILE)			/* Infos */
					)
				)
			),
	                KArgs
		);
	}

	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 2, InDim, 1);
		AddKernelArgDim(Name, "Filter", 3, OutDim, InDim, 1);
		AddKernelArgDim(Name, "Bias", 2, OutDim, Bias_DataSize);
		AddKernelArgDim(Name, "Out", 2, OutDim, 1);
		AddKernelArgDim(Name, "Scale", 2, OutDim, 1);
		AddKernelArgDim(Name, "ScaleN", 2, OutDim, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, InDim,OutDim,1,1, Bias_DataSize, LinearOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for SoftMax layers 

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation), Def=(TILE_HOR)

		Dim:		Number of inputs

		SoftMaxOper:	Should always be KOP_SOFTMAX

		Signature:	Name(In, Out, Infos)

	CNN_SoftMax
                Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to
                a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15
*********************************************************************************************************************************************************************/

static Kernel_T * CNN_SoftMax_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Dim,

        KernelOper_T SoftMaxOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0, OutBytes = 2;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->OutDataSize != 0) OutBytes = Ctrl->OutDataSize;
	}
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernel(SoftMaxOper, KOP_NONE, ParFeat, 1, 0, 0, 0, OutBytes, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax_SQ8 Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only, Q15 output", Name);

	LayerOp += Dim;
	LayerBandwidth += (int64_t) Dim*1 + Dim*OutBytes;
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(1,1,1), "In"),
                      TCArg(CNN_ArgDataType(OutBytes,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
					Imm(1),							/* Number of features */
					K_Arg("In", KER_ARG_TILE_H),				/* Number of inputs */
					K_TileOper("Infos", "char *", '@', AT_INF_BIASL_SM),	/* Input left Norm factor */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
			)
		),
                KerArgs(3,
                        KerArg("In",    KerArgSpace(1,T0), OBJ_BUFFER_IN,                1,          Dim, 1, 0, 0, 8, "In"),
                        KerArg("Out",   KerArgSpace(1,T0), OBJ_BUFFER_OUT,               1,          Dim, 2, 0, 0, 0, "Out"),
			KerArg("Infos", KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST, AT_INF_DIM, 1,   1, 0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 2, Dim, 1);
		AddKernelArgDim(Name, "Out", 2, Dim, OutBytes);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Dim,Dim,1,1, 1, SoftMaxOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, 0);
	}
	return Kernel;
}

static Kernel_T * CNN_SoftMax2D_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Dim,
	int N,

        KernelOper_T SoftMaxOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0, OutBytes = 2, HWC = 0;;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
	}
	int KerLayout = HWC?CALL_HWC_KER:0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernelAttr(SoftMaxOper, KOP_NONE, ParFeat, KerLayout, 1, 0, 0, 0, OutBytes, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax_SQ8 Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only, Q15 output", Name);

	LayerOp += N*Dim;
	LayerBandwidth += (int64_t) N*(Dim*1 + Dim*2);
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, N, 1), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(1,1,1), "In"),
                      TCArg(CNN_ArgDataType(OutBytes,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
					K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),		/* Number of features in this tile */
					K_Arg("In", KER_ARG_TILE_H),				/* Number of inputs */
					K_TileOper("Infos", "char *", '@', AT_INF_BIASL_SM),	/* Input left Norm factor */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					K_Arg("Infos", KER_ARG_TILE)				/* Infos */
				)
			)
		),
		(HWC==0)?
                KerArgs(3,
                        KerArg("In",    KerArgSpace(2,D0,T0), OBJ_BUFFER_IN,                1,          Dim, 1, 0, 0, 8, "In"),
                        KerArg("Out",   KerArgSpace(2,D0,T0), OBJ_BUFFER_OUT,               1,          Dim, OutBytes, 0, 0, 0, "Out"),
			KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, AT_INF_DIM, 1,   1, 0, 0, 0, "Infos")
		):
                KerArgs(3,
                        KerArg("In",    KerArgSpace(2,T0,D0), OBJ_BUFFER_IN,                1,          Dim, 1, 0, 0, 8, "In"),
                        KerArg("Out",   KerArgSpace(2,T0,D0), OBJ_BUFFER_OUT,               1,          Dim, OutBytes, 0, 0, 0, "Out"),
			KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, AT_INF_DIM, 1,   1, 0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 3, N, Dim, 1);
		AddKernelArgDim(Name, "Out", 3, N, Dim, 2);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Dim,Dim,1,1, 1, SoftMaxOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, 0);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for Matrix Addition layers with input scale adjustment (tensor centric), output scaling (tensor centric) and optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of features
		Width:		Width of a given feature
		Height:		Height of a given feature

		AddMatOper:	Should always be KOP_MATADD
		ActOper:	Optional activation

		Signature:	Name(In1, In2, Out, Infos)

	CNN_MatAddAct_SQ8
	
*********************************************************************************************************************************************************************/

static Kernel_T * CNN_MatAddAct_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

        KernelOper_T AddMatOper,
        KernelOper_T ActOper
)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0, HWC = 0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=KOP_NONE);
	int KerLayout = HWC?CALL_HWC_KER:0;
	char *MatAddKerName=0, *ActKerName=0;
	int Ids = 1, Ods = 1;
	if (Ctrl) {
		if (Ctrl->InDataSize) Ids = Ctrl->InDataSize;
		if (Ctrl->OutDataSize) Ods = Ctrl->OutDataSize;
	}

	MatAddKerName = CNN_FindMatchingKernel(AddMatOper, ActOper, ParFeat, Ids, Ids, 0, 0, Ods, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatAddKerName) StandAloneAct = 0;
	else MatAddKerName = CNN_FindMatchingKernel(AddMatOper, KOP_NONE, ParFeat, Ids, Ids, 0, 0, Ods, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatAddKerName==0) GenTilingError("CNN_MatAddAct_SQ8 Kernel: %s, Can't find a matching basic kernel for MatAdd", Name);

	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, Ods, 0, 0, 0, Ods, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatAddAct_SQ8 Kernel: %s, Can't find a matching basic kerne for Activationl", Name);
	}

	LayerOp += (int64_t) Feat * Width * Height;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	int InfosDim = Ods>0?AT_INF_DIM:AT_INF_ASYM_ADD_DIM;

        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(4,
                      TCArg(Ids>0?CNN_ArgDataType(1,1,1):CNN_ArgDataTypeUns(1,1,1), "In1"),
                      TCArg(Ids>0?CNN_ArgDataType(1,1,1):CNN_ArgDataTypeUns(1,1,1), "In2"),
                      TCArg(Ods>0?CNN_ArgDataType(1,1,1):CNN_ArgDataTypeUns(1,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(2,
			Call(MatAddKerName, LOC_LOOP,
				Bindings(8,
					K_Arg("In1", KER_ARG_TILE),			/* First input tile */
					K_Arg("In2", KER_ARG_TILE),			/* Second input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_Arg("In1", KER_ARG_TILE_H),			/* Input tile height */
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					Imm((ActOper==KOP_NONE)),			/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)			/* Infos */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),			/* Input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_Arg("In1", KER_ARG_TILE_H),			/* Input tile height */
					Imm(1),
					Imm(1),
					K_Arg("Infos", KER_ARG_TILE)			/* Infos */
				)
			)
		),
                KerArgs(4,
                        KerArg("In1",    KerArgSpace(1,T0), O_IN|O_DB,            1, Feat*Width*Height, 	   1, 0, 0, 0, "In1"),
                        KerArg("In2",    KerArgSpace(1,T0), O_IN|O_DB,            1, Feat*Width*Height, 	   1, 0, 0, 0, "In2"),
                        KerArg("Out",    KerArgSpace(1,T0), O_OUT|O_DB,           1, Feat*Width*Height, 	   1, 0, 0, 0, "Out"),
			KerArg("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED, 1, 		     1,   InfosDim*1, 0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In1", 4, Feat,  Height, Width, 1);
		AddKernelArgDim(Name, "In2", 4, Feat,  Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Infos", 2, InfosDim, 1);

		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, AddMatOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
	Generator for Channel Padded Matrix Addition layers with input scale adjustment (tensor centric), output scaling (tensor centric) and optional activation

	Template:
		Name:		Name of the generated user kernel
		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of features
		Width:		Width of a given feature
		Height:		Height of a given feature
		PadTop:		Top channel padding added to InIdxPaddedIn
		PadBot:		Bottom channel padding added to InIdxPaddedIn
		IdxPaddedIn:	Which input is padded

		AddMatOper:	Should always be KOP_MATADD
		ActOper:	Optional activation
		Signature:	Name(In1, In2, Out, Infos, InfosPad)

	CNN_MatAddPaddedAct_SQ8

*********************************************************************************************************************************************************************/

int CNN_MatAddPaddedAct_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,
	int PadTop,
	int PadBot,
	int IdxPaddedIn,

	KernelOper_T AddMatOper,
	KernelOper_T ActOper
)

{
	if (PadBot == 0 && PadTop == 0) return CNN_MatAddAct_SQ8(Name, Ctrl, Feat, Width, Height, AddMatOper, ActOper);
	if (PadTop + PadBot > Feat) GenTilingError("int CNN_MatAddPaddedAct_SQ8 Kernel: %s, Padding exceeds channel size", Name);
	int FeatBody = Feat - PadTop - PadBot;
	int Ok = 1;

	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	LayerOp += (Feat-PadTop-PadBot) * Width * Height;
	LayerBandwidth += Width*Height*1*(Feat); 		// In1
	LayerBandwidth += Width*Height*1*(Feat-PadTop-PadBot);	// In2
	LayerBandwidth += Width*Height*1*(Feat);		// Out

	OpenKernelGroup(Name);
	char *TopName = NULL, *BotName = NULL, *BodyName = NULL;
	Ok = Ok && CNN_MatAddAct_SQ8(BodyName = AppendNames(Name, "Body"), Ctrl, FeatBody, Width, Height, AddMatOper, ActOper);

	KernelOper_T PadActOper;
	switch (ActOper) {
		case KOP_NONE:	    PadActOper = KOP_ACT_NONE_IN_SCALE; break;
		case KOP_RELU:	    PadActOper = KOP_RELU_IN_SCALE; break;
		case KOP_RELUN:	    PadActOper = KOP_RELUN_IN_SCALE; break;
		case KOP_RELUM:	    PadActOper = KOP_RELUM_IN_SCALE; break;
		case KOP_RELUMN:    PadActOper = KOP_RELUMN_IN_SCALE; break;
		case KOP_HSIGMOID:  PadActOper = KOP_HSIGMOID_IN_SCALE; break;
		case KOP_HSWISH:    PadActOper = KOP_HSWISH_IN_SCALE; break;
		case KOP_LEAKYRELU: PadActOper = KOP_LEAKYRELU_IN_SCALE; break;
		case KOP_SIGMOID:   PadActOper = KOP_SIGMOID_IN_SCALE; break;
		case KOP_TANH:      PadActOper = KOP_TANH_IN_SCALE; break;
	}
	if (PadTop) {
		Ok = Ok && CNN_Act_SQ8(TopName = AppendNames(Name, "PadTop"), Ctrl, PadTop, Width, Height, PadActOper);
	}
	if (PadBot) {
		Ok = Ok && CNN_Act_SQ8(BotName = AppendNames(Name, "PadBot"), Ctrl, PadBot, Width, Height, PadActOper);
	}
	CloseKernelGroupNoMerge();
	if (Ok==0) return 0;
	int A = 0;
	CKernel_Arg_T **GroupCArgs = AllocateCArgs(5);
	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1), "In1");
	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1), "In2");
	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1), "Out");
	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1), "Infos");
	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1), "InfosPad");
	char *PaddedInput    = IdxPaddedIn==0?"In1":"In2";
	char *NonPaddedInput = IdxPaddedIn==0?"In2":"In1";
	A = 0;
	CKernelCall_T **GroupCCalls;
	GroupCCalls = AllocateCalls(1+(PadTop!=0?1:0)+(PadBot!=0?1:0));
	if (PadTop) {
		GroupCCalls[A++] = UserKernelCall(TopName, LOC_GROUP,
				Bindings(3,
					C_Arg(PaddedInput),
					C_Arg("Out"),
					C_Arg("InfosPad")
					)
				);
		GroupCCalls[A++] = UserKernelCall(BodyName, LOC_GROUP,
				Bindings(4,
					IdxPaddedIn==0?KG_ArgOper("In1", '+', PadTop*Width*Height):C_Arg("In1"),
					IdxPaddedIn==0?C_Arg("In2"):KG_ArgOper("In2", '+', PadTop*Width*Height),
					KG_ArgOper("Out", '+', PadTop*Width*Height),
					C_Arg("Infos")
					)
				);
	} else {
		GroupCCalls[A++] = UserKernelCall(BodyName, LOC_GROUP,
				Bindings(4,
					C_Arg("In1"),
					C_Arg("In2"),
					C_Arg("Out"),
					C_Arg("Infos")
					)
				);
	}
	if (PadBot) {
		GroupCCalls[A++] = UserKernelCall(BotName, LOC_GROUP,
				Bindings(3,
					KG_ArgOper(PaddedInput, '+', (PadTop + FeatBody)*Width*Height),
					KG_ArgOper("Out", '+', (PadTop + FeatBody)*Width*Height),
					C_Arg("InfosPad")
					)
				);
	}
	A = 0;
	int Sz  = Feat*Width*Height;
	int Sz2 = Width*Height*(Feat-PadTop-PadBot);
	Object_T **GroupKerArgs = AllocateKerArgs(5);
	GroupKerArgs[A++] = KerGroupArg("In1", 		O_IN,	Sz,		1, "In1");
	GroupKerArgs[A++] = KerGroupArg("In2", 		O_IN,	Sz2,		1, "In2");
	GroupKerArgs[A++] = KerGroupArg("Out", 		O_OUT,	Sz,		1, "Out");
	GroupKerArgs[A++] = KerGroupArg("Infos",	O_IN, 	AT_INF_DIM,	1, "Infos");
	GroupKerArgs[A++] = KerGroupArg("InfosPad",	O_IN, 	AT_INF_DIM,	1, "InfosPad");
	KernelGroup_T *UKGroup = UserKernelGroupK(
		Name,
		1,
		GroupCArgs,
		0,
		GroupCCalls,
		GroupKerArgs
	);
	return (UKGroup!=0);
}


/*********************************************************************************************************************************************************************
 	Generator for Tensor [CxHxW] by Vector [C] product with tensor centric scaling and optional Activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		InFeat:		Number of input features
		Width:		Width of a In1
		Height:		Height of a In1

		MatOper:	KOP_MATVECTMUL
		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID

		Signature:	Name(In1, In2, Out, Infos)

	CNN_TensorVectMultAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_TensorVectMultAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatOper,
	KernelOper_T ActOper
)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1, HWC = 0;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
	}
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *MatOperKerName=0, *ActKerName=0;
	int StandAloneAct=(ActOper!=KOP_NONE);
	int KerLayout = HWC?CALL_HWC_KER:0;

	MatOperKerName = CNN_FindMatchingKernelAttr(MatOper, ActOper, ParFeat, KerLayout, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatOperKerName) StandAloneAct=0;
	else if (StandAloneAct) MatOperKerName = CNN_FindMatchingKernelAttr(MatOper, KOP_NONE, ParFeat, KerLayout, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatOperKerName==0) GenTilingError("CNN_TensorVectMultAct_SQ8 Kernel: %s, Can't find a matching basic kernel for MatVectMult", Name);
	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_TensorVectMultAct_SQ8 Kernel: %s, Can't find a matching basic kernel for Activation", Name);
	}


	LayerOp += (int64_t) Feat * Width * Height;

	LayerBandwidth += (int64_t) Width*Height*1*Feat;
	LayerBandwidth += (int64_t) 1*Feat;
	LayerBandwidth += (int64_t) Width*Height*1*Feat;

        Kernel_T *Kernel;
	CKernel_Arg_T **KerCArgs;
	int Ca=0;
	Object_T **KArgs;
	int Ka=0;

	KerCArgs = AllocateCArgs(4);
	KerCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1), "In1");
	KerCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1), "In2");
	KerCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1), "Out");
	KerCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1), "Infos");

	KArgs = AllocateKerArgs(4);
	if (HWC==0) {
		KArgs[Ka++] = KerArg("In1",   KerArgSpace(2,D0,T0), O_IN|O_DB,            1, 1, Width*Height*1,  0, 0, 0, "In1");
		KArgs[Ka++] = KerArg("In2",   KerArgSpace(1,D0),    O_IN|O_DB,            1, 1, 1,               0, 0, 0, "In2");
		KArgs[Ka++] = KerArg("Out",   KerArgSpace(2,D0,T0), O_OUT|O_DB,           1, 1, Width*Height*1,  0, 0, 0, "Out");
		KArgs[Ka++] = KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1, 1, AT_INF_DIM*1,    0, 0, 0, "Infos");
	} else {
		KArgs[Ka++] = KerArg("In1",   KerArgSpace(2,T0,D0), O_IN|O_DB,            1, 1, Width*Height*1,  0, 0, 0, "In1");
		KArgs[Ka++] = KerArg("In2",   KerArgSpace(1,D0),    O_IN|O_DB,            1, 1, 1,               0, 0, 0, "In2");
		KArgs[Ka++] = KerArg("Out",   KerArgSpace(2,T0,D0), O_OUT|O_DB,           1, 1, Width*Height*1,  0, 0, 0, "Out");
		KArgs[Ka++] = KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1, 1, AT_INF_DIM*1,    0, 0, 0, "Infos");
	}


        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	/* First try with entire feature in L1 */
       	Kernel= UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)),
                TileOrientation,
		KerCArgs,
                Calls(2,
			Call(MatOperKerName, LOC_LOOP,
				Bindings(8,
					K_Arg("In1", KER_ARG_TILE),			/* First input tile */
					K_Arg("In2", KER_ARG_TILE),			/* Second input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_ArgPar("In1", KER_ARG_PARTILE_SIZE, D0),	/* Number of Matrices involved */
					Imm(Width), 					/* Input tile width */
					Imm(Height),					/* Input tile height */
					Imm((ActOper==KOP_NONE)),			/* Scaling when no activation */
					K_Arg("Infos", KER_ARG_TILE)			/* Infos */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),			/* Input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),	/* Number of features in this tile */
					Imm(Width), 					/* Input tile width */
					Imm(Height),					/* Input tile height */
					K_Arg("Infos", KER_ARG_TILE)			/* Infos */
				)
			)
		),
		KArgs
	);
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	if (Kernel == 0) {
		Ka=0;
		KArgs[Ka++] = KerArg("In1",   KerArgSpace(2,D0,T0), O_IN|O_DB,           Width, Height, 1,            0, 0, 0, "In1");
		KArgs[Ka++] = KerArg("In2",   KerArgSpace(1,D0),    O_IN|O_DB,               1,      1, 1,            0, 0, 0, "In2");
		KArgs[Ka++] = KerArg("Out",   KerArgSpace(2,D0,T0), O_OUT|O_DB,          Width, Height, 1,            0, 0, 0, "Out");
		KArgs[Ka++] = KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED,    1,      1, AT_INF_DIM*1, 0, 0, 0, "Infos");

	       	Kernel= UserKernel(Name,
			KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)),
	                TileOrientation,
			KerCArgs,
	                Calls(2,
				Call(MatOperKerName, LOC_LOOP,
					Bindings(8,
						K_Arg("In1", KER_ARG_TILE),			/* First input tile */
						K_Arg("In2", KER_ARG_TILE),			/* Second input tile */
						K_Arg("Out", KER_ARG_TILE),			/* Output tile */
						K_ArgPar("In1", KER_ARG_PARTILE_SIZE, D0),	/* Number of Matrices involved */
						K_Arg("In1", KER_ARG_TILE_W),			/* Input tile width */
						K_Arg("In1", KER_ARG_TILE_H),			/* Input tile height */
						Imm((ActOper==KOP_NONE)),			/* Scaling when no activation */
						K_Arg("Infos", KER_ARG_TILE)			/* Infos */
					)
				),
				(ActKerName==0)?AT_NO_CALL:
				Call(ActKerName, LOC_LOOP,
					Bindings(6,
						K_Arg("Out", KER_ARG_TILE),			/* Input tile */
						K_Arg("Out", KER_ARG_TILE),			/* Output tile */
						K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),	/* Number of features in this tile */
						K_Arg("Out", KER_ARG_TILE_W),			/* Input tile width */
						K_Arg("Out", KER_ARG_TILE_H),			/* Input tile height */
						K_Arg("Infos", KER_ARG_TILE)			/* Infos */
					)
				)
			),
			KArgs
		);
	}
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In1",   4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "In2",   2, Feat, 1);
		AddKernelArgDim(Name, "Out",   4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, MatOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return (Kernel!=0);
}


/*********************************************************************************************************************************************************************
 	Generator for Matrix Multiplication layers with channel centric scaling followed by an optional Activation.

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options

		Bias_DataSize:	1: byte, 2: half word,
		Scale_DataSize:	1: byte, 2: half word, 4: word

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

		MatMulOper:	Should always be KOP_MATMUL

		ActOper:	Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out, Scale, ScaleN, Infos)

	CNN_MatMulAct_SQ8
	
*********************************************************************************************************************************************************************/

Kernel_T *CNN_MatMulAct_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

	int ColM1,
	int LineM1,
	int ColM2,
	int LineM2,

	int Width,
	int Height,
	int Scx,
	int Scy,

        KernelOper_T MatMulOper,
        KernelOper_T ActOper,
        int InvertInputs
	)

{
	int Log = 1;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int NeedScx, NeedScy;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
        int LineO = LineM1, ColO = ColM2;
	int ConsT0 = Scx;
	int Nbuff;
	int ColFirst = ((LineM1*ColM1)<(LineM2*ColM2));
	char *MatMulKerName=0, *ActKerName=0;
	int StandAloneAct = (ActOper!=KOP_NONE);

	/* NoBias Basic Kernels have 32bits bias not used */
	int NoBias = (MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR) || (MatMulOper == KOP_MATMUL_NOBIAS) || (MatMulOper == KOP_MATMUL_NOBIAS_TRANSPOSED) || (MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED);
	Bias_DataSize = NoBias?4:Bias_DataSize;

	int ScaleScalar = (MatMulOper == KOP_MATMUL_SCALE_SCALAR) || (MatMulOper == KOP_MATMUL_SCALE_SCALAR_TRANSPOSED) || (MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED) || (MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR);
	int Transposed = (MatMulOper == KOP_MATMUL_TRANSPOSED) || (MatMulOper == KOP_MATMUL_NOBIAS_TRANSPOSED) || (MatMulOper == KOP_MATMUL_SCALE_SCALAR_TRANSPOSED) || (MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED);
        int SAxis = (MatMulOper == KOP_MATMUL)?LineO:ColO;
        int TA = (MatMulOper == KOP_MATMUL)?T0:T1;
        int TB = (MatMulOper == KOP_MATMUL)?T1:T0;

	if (!(MatMulOper == KOP_MATMUL) && !(MatMulOper == KOP_MATMUL_NOBIAS) && !(MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR) && !(MatMulOper == KOP_MATMUL_SCALE_SCALAR_TRANSPOSED) && !(MatMulOper == KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED) && !(MatMulOper == KOP_MATMUL_SCALE_SCALAR) && !(MatMulOper == KOP_MATMUL_TRANSPOSED) && !(MatMulOper == KOP_MATMUL_NOBIAS_TRANSPOSED))
		GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, MatMulOper should be KOP_MATMUL or KOP_MATMUL_NOBIAS or KOP_MATMUL_NOBIAS_SCALE_SCALAR or KOP_MATMUL_SCALE_SCALAR_TRANSPOSED or KOP_MATMUL_NOBIAS_SCALE_SCALAR_TRANSPOSED or KOP_MATMUL_SCALE_SCALAR or KOP_MATMUL_TRANSPOSED or KOP_MATMUL_NOBIAS_TRANSPOSED", Name);

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU || ActOper == KOP_SIGMOID || ActOper == KOP_TANH))
		GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	if (ColM1 != LineM2) GenTilingError("CNN_MatMulAct_SQ8: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", Name, LineM1, ColM1, LineM2, ColM2);
	if (Width==0||Height==0) {
		Width = ColM2; Height=1; Scx = 1; Scy = 1;
	}
	if ((Width*Height) != ColM2) GenTilingError("CNN_MatMulAct_SQ8: %s, ColM2: %d should always be equal to Width: %d * Height: %d", Name, ColM2, Width, Height);
	if (Scx==1 && Scy==1) Nbuff = 4; else Nbuff = 1;

	MatMulKerName = CNN_FindMatchingKernel(MatMulOper, ActOper, 1, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatMulKerName) StandAloneAct = 0;
	else MatMulKerName = CNN_FindMatchingKernel(MatMulOper, KOP_NONE, 1, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatMulKerName==0) GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, Can't find a matching basic kernel for MatMul", Name);

	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 0, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, Can't find a matching basic kernel for Activation", Name);
	}

	ColO = ((Width+Scx-1)/Scx) * ((Height+Scy-1)/Scy);
	LayerOp += (int64_t) ColM1*ColO*LineM1;
	LayerBandwidth += (int64_t) LineM1*(ColM1*ColM2*(1+1));
	LayerBandwidth += (int64_t) LineM1*ColM2*1;
	LayerBandwidth += (int64_t) LineM1*Bias_DataSize;

	if (Scy!=1) ConsT0 = Width*Scy; else ConsT0 = 4;
	
	if (Log) {
		printf("CNN_MatMul_SQ8: %s\n", Name);
		printf("In1  => W: %4d, H: %4d %s\n", ColM1, LineM1, Transposed?"(TRANSPOSED)":"");
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d\n", ColM2, LineM2, Width, Height, Scx, Scy);
		printf("Out  => W: %4d, H: %4d => %s\n", ColO, LineO, ColFirst?"Column first":"Line First");
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		if (ActKerName)    printf("%20s: %s\n", "ActKerName", ActKerName);
		// printf("Nb Oper : %lld\n", LayerOp);
	}

	Kernel_T *Kernel;

	int ObjCons = !Transposed?OBJ_CONSTRAINTS_TILE_VER:0;
	if (Transposed) {
		LineM2 = ColM2; ColM2 = ColM1;
	}
	/* First try buffering small objects */
	Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  InvertInputs?"In2":"In1"),
                      TCArg(CNN_ArgDataType(1,1,1),  InvertInputs?"In1":"In2"),
                      !NoBias?TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias"):AT_NO_C_ARG,
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      !ScaleScalar?TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"):AT_NO_C_ARG,
                      !ScaleScalar?TCArg(CNN_ArgDataType(1,1,1),"ScaleN"):AT_NO_C_ARG,
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(2,
			Call(MatMulKerName, LOC_LOOP,
				Bindings(19,
					K_Arg("In1",  KER_ARG_TILE), K_Arg("In1",  KER_ARG_TILE_W), K_Arg("In1",  KER_ARG_TILE_H),
					K_Arg("In2",  KER_ARG_TILE), Transposed?K_Arg("In2",  KER_ARG_TILE_H):K_Arg("In2",  KER_ARG_TILE_W),
					!NoBias?K_Arg("Bias", KER_ARG_TILE):Imm(0),
					!ScaleScalar?K_Arg("Scale", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
					!ScaleScalar?K_Arg("ScaleN", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
					K_Arg("Out", KER_ARG_TILE),  K_Arg("Out", KER_ARG_TILE_W), K_Arg(ColFirst?"In1":"In2",  KER_ARG_TILE_BASE),
					!Transposed?K_Arg("KerBuff", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
					!NoBias?K_TileOper("Infos", "char *", '@', AT_INF_BIASN):AT_IGNORE_ARG_BINDING,
					Imm(ColFirst),
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Width):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Height):AT_IGNORE_ARG_BINDING,
					K_Arg("Infos", KER_ARG_TILE)
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP_EPILOG,
				Bindings(6,					/* Uses feature par form with W: Out.W, H: Out.H given the fact that H is feat and W is w*h */
					K_Arg("Out", KER_ARG_TILE),		/* Input tile */
					K_Arg("Out", KER_ARG_TILE),		/* Output tile */
					Imm(1),					/* Input of features in this tile */
					K_Arg("Out", KER_ARG_TILE_W),		/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),		/* Input tile height */
					K_Arg("Infos", KER_ARG_TILE)		/* Infos */
				)
			)
		),
		ColFirst?
		KerArgs(8,
	    !Transposed?KerArg("KerBuff",KerArgSpace(1, T1), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0):AT_NO_KER_ARG,
			KerArg("In1",    KerArgSpace(1, T0), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T1), O_IN|O_DB,             ColM2,  LineM2, 1,             0, ObjCons|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
	        !NoBias?KerArg("Bias",   KerArgSpace(1, TA), O_BUFF|O_IN|O_CONST,       1,  SAxis,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"):AT_NO_KER_ARG,
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "Out"),
	   !ScaleScalar?KerArg("Scale",  KerArgSpace(1, TA), O_BUFF|O_IN|O_CONST,        1, SAxis,  1,             0, 0,                                                0, "Scale"):AT_NO_KER_ARG,
	   !ScaleScalar?KerArg("ScaleN", KerArgSpace(1, TA), O_BUFF|O_IN|O_CONST,        1, SAxis,  1,             0, 0,                                                0, "ScaleN"):AT_NO_KER_ARG,
			KerArg("Infos",  KerArgSpace(1, T1), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
		):
		KerArgs(8,
	    !Transposed?KerArg("KerBuff",KerArgSpace(1, T0), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0):AT_NO_KER_ARG,
			KerArg("In1",    KerArgSpace(1, T1), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T0), O_IN|O_DB,             ColM2,  LineM2, 1,             0, ObjCons|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
	        !NoBias?KerArg("Bias",   KerArgSpace(1, TB), O_BUFF|O_IN|O_CONST,       1,  SAxis,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"):AT_NO_KER_ARG,
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out"),
	   !ScaleScalar?KerArg("Scale",  KerArgSpace(1, TB), O_BUFF|O_IN|O_CONST,        1, SAxis,  1,             0, 0,                                                0, "Scale"):AT_NO_KER_ARG,
	   !ScaleScalar?KerArg("ScaleN", KerArgSpace(1, TB), O_BUFF|O_IN|O_CONST,        1, SAxis,  1,             0, 0,                                                0, "ScaleN"):AT_NO_KER_ARG,
			KerArg("Infos",  KerArgSpace(1, T0), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In1", 3, LineM1, ColM1, 1);
		AddKernelArgDim(Name, "In2", 4, LineM2, Height, Width, 1);
		if (!NoBias) AddKernelArgDim(Name, "Bias", 2, SAxis, Bias_DataSize);
		AddKernelArgDim(Name, "Out", 3, LineO, ColO, 1);
		if (!ScaleScalar) AddKernelArgDim(Name, "Scale", 2, SAxis, 1);
		if (!ScaleScalar) AddKernelArgDim(Name, "ScaleN", 2, SAxis, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, ColM1,LineM1,Width,Height, Bias_DataSize, MatMulOper, 1,1,1,1,Scx,Scy,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return Kernel;
}

/*********************************************************************************************************************************************************************
 	Generator for Matrix Multiplication layers with channel centric scaling followed by an optional Activation.
	Special form to handle small form factor In1 (InFeat x OutFeat)

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP
	This generator assumes that the whole In1 and Bias can be accomodated into shared L1. Expecting to be used for us to 32 InFeat x 32 OutFeat

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (ReluN), Def=(6)

		Bias_DataSize:	1: byte, 2: half word,
		Scale_DataSize:	1: byte, 2: half word, 4: word

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

        	MatMulOper	Should always be KOP_MATMUL

        	ActOper		Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU, KOP_SIGMOID)

		Signature:	Name(In2, In1, Bias, Out, Scale, ScaleN, Infos)

	CNN_MatMulSmallM1Act_SQ8
	
*********************************************************************************************************************************************************************/

Kernel_T * CNN_MatMulSmallM1Act_SQ8_Internal(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

	int ColM1,
	int LineM1,
	int ColM2,
	int LineM2,

	int Width,
	int Height,
	int Scx,
	int Scy,

        KernelOper_T MatMulOper,
        KernelOper_T ActOper
	)

{
	int Log = 1;
	Tile_Orientation_T TileOrientation = TILE_VER;
	int NeedScx, NeedScy;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
        int LineO = LineM1, ColO = ColM2;
	int OutLB, OutUB, ReluN = 6;
	int ConsT0 = Scx;
	int TileCons = 8;

	char *MatMulKerName=0, *MatTransKerName=0, *ActKerName=0;
	int StandAloneAct = (ActOper!=KOP_NONE);

	if (!(MatMulOper == KOP_MATMUL_SM1)) GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, MatMulOper should be KOP_MATMUL", Name);

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_RELUMN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU || ActOper == KOP_SIGMOID || ActOper == KOP_TANH))
		GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_TANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	if (ColM1 != LineM2) GenTilingError("CNN_MatMulSmallM1Act_SQ8: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d] %s", Name, LineM1, ColM1, LineM2, ColM2);
	if (Width==0||Height==0) {
		Width = ColM2; Height=1; Scx = 1; Scy = 1;
	} else if (Scy != 1) TileCons = Width*Scy;
	if ((Width*Height) != ColM2) GenTilingError("CNN_MatMulSmallM1Act_SQ8: %s, ColM2: %d should always be equal to Width: %d * Height: %d", Name, ColM2, Width, Height);

	MatMulKerName = CNN_FindMatchingKernel(MatMulOper, ActOper, 1, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,1,1, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatMulKerName) StandAloneAct = 0;
	else if (StandAloneAct) MatMulKerName = CNN_FindMatchingKernel(MatMulOper, KOP_NONE, 1, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,1,1, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatMulKerName==0) GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, Can't find a matrix multiplication matching basic kernel", Name);

	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, 0, 0, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, Can't find a matching basic kernel for Activation", Name);
	}

	MatTransKerName = CNN_FindMatchingKernel(KOP_MATTRANSP, KOP_NONE, 0, 1, 0, 0, 0, 1, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatTransKerName==0) GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, Can't find a matrix transpose matching basic kernel", Name);

	ColO = ((Width+Scx-1)/Scx) * ((Height+Scy-1)/Scy);
	LayerOp += (int64_t) ColM1*ColO*LineM1;
	LayerBandwidth += (int64_t) LineM1*(ColM1*ColM2*(1+1));
	LayerBandwidth += (int64_t) LineM1*ColM2*1;
	LayerBandwidth += (int64_t) LineM1*Bias_DataSize;

	if (Log) {
		printf("CNN_MatMulSmallM1_SQ8: %s\n", Name);
		printf("In1  => W: %4d, H: %4d\n", ColM1, LineM1);
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d, TileCons: %d\n", ColM2, LineM2, Width, Height, Scx, Scy, TileCons);
		printf("Out  => W: %4d, H: %4d\n", ColO, LineO);
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		if (MatTransKerName) printf("%20s: %s\n", "MatTransKerName", MatTransKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Act: %s\n", CNN_KernelOperImage(ActOper));
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TILE_VER,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  "In2"),
                      TCArg(CNN_ArgDataType(1,1,1),  "In1"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"),
                      TCArg(CNN_ArgDataType(1,1,1),"ScaleN"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(3,
			Call(MatTransKerName, LOC_LOOP,
				Bindings(7,
					K_Arg("In2", KER_ARG_TILE),			/* Input tile */
					K_Arg("TransIn2", KER_ARG_TILE),		/* Transposed input tile */
					Imm(1),						/* A single matrix */
					K_Arg("In2", KER_ARG_TILE_W),			/* Input tile width */
					K_Arg("In2", KER_ARG_TILE_H),			/* Input tile height */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING
				)
			),
			Call(MatMulKerName, LOC_LOOP,
				Bindings(19,
					K_Arg("In1",  KER_ARG_TILE), Imm(ColM1), Imm(LineM1),
					K_Arg("TransIn2",  KER_ARG_TILE), K_Arg("TransIn2",  KER_ARG_TILE_W),
					K_Arg("Bias", KER_ARG_TILE),
					K_Arg("Scale", KER_ARG_TILE), K_Arg("ScaleN", KER_ARG_TILE),
					K_Arg("Out", KER_ARG_TILE), AT_IGNORE_ARG_BINDING, AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					K_TileOper("Infos", "char *", '@', AT_INF_BIASN),
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					K_Arg("Infos", KER_ARG_TILE)
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),		/* Input tile */
					K_Arg("Out", KER_ARG_TILE),		/* Output tile */
					Imm(1),					/* Number of features in this tile */
					K_Arg("Out", KER_ARG_TILE_W),		/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),		/* Input tile height */
					K_Arg("Infos", KER_ARG_TILE)		/* Infos */
				)
			)
		),
		KerArgs(8,
			KerArg("In1",      KerArgSpace(1, T0), O_IN|O_BUFF|O_NTILED|O_CONST, ColM1, LineM1, 1,             0, 0, 0, "In1"),
			KerArg("In2",      KerArgSpace(1, T0), O_IN|O_DB,                    ColM2, LineM2, 1,             0, 0, TileCons, "In2"),
			// KerArg("TransIn2", KerArgSpace(1, T0), O_BUFF|O_ONETILE,             ColM2, LineM2, 1,             0, 0, 0,  ""),
			KerArg("TransIn2", KerArgSpace(1, T0), O_BUFF|O_ONETILE,              ColO, LineM2, 1,             0, 0, 0,  ""),
			KerArg("Bias",     KerArgSpace(1, T0), O_BUFF|O_IN|O_NTILED|O_CONST,     1, LineM1, Bias_DataSize, 0, 0, 0, "Bias"),
			KerArg("Out",      KerArgSpace(1, T0), O_OUT|O_DB,                    ColO, LineM1, 1,             0, 0, 0, "Out"),
			KerArg("Scale",    KerArgSpace(1, T0), O_BUFF|O_IN|O_NTILED|O_CONST,     1, LineM1, 1,             0, 0, 0, "Scale"),
			KerArg("ScaleN",   KerArgSpace(1, T0), O_BUFF|O_IN|O_NTILED|O_CONST,     1, LineM1, 1,             0, 0, 0, "ScaleN"),
			KerArg("Infos",    KerArgSpace(1, T0), O_IN|O_BUFF|O_NTILED,             1,      1, AT_INF_DIM*1,  0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In1", 3, LineM1, ColM1, 1);
		AddKernelArgDim(Name, "In2", 4, LineM2, Height, Width, 1);
		AddKernelArgDim(Name, "Bias", 2, LineO, Bias_DataSize);
		AddKernelArgDim(Name, "Out", 3, LineO, ColO, 1);
		AddKernelArgDim(Name, "Scale", 2, LineO, 1);
		AddKernelArgDim(Name, "ScaleN", 2, LineO, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, ColM1,LineM1,Width,Height, Bias_DataSize, MatMulOper, 1,1,1,1,Scx,Scy,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return Kernel;

}

/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */
/* ============================================================================================================================================================== */

int CNN_MM_ConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)
{
	Kernel_T *Ker = 0, *Sol1 = 0, *Sol2 = 0;
        float K = 0.9;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        if (Ctrl) {
		if (Ctrl->TileOrientation != -1) {
			printf("TileOrientation set by user\n");
			Ker = CNN_MM_ConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
			if (Ker!=0) return 1;
			else GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8: %s, Failed to gen with set tiling orientation, try to let the Autotiler set it for you", Name);
		}
	}

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;

	printf("\n\n=============================== Trying Tile Orientation: TILE_HOR ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(0));
        Ker = CNN_MM_ConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);

	printf("\n=============================== Trying Tile Orientation: TILE_VER ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(1));
        Ker = CNN_MM_ConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol2 = CopyAndPopUserKernel(Ker);

        if (Sol1 && Sol2) {
		int TakeSol1 = ((K*Sol1->Cost->TileOverhead) < Sol2->Cost->TileOverhead);  // K close to 1.0if (TakeSol1) {
		printf(">>>>>>>>>>>>>>>>>> %s is better: %.3f vs %.3f \n\n\n", TakeSol1?"TILE_HOR":"TILE_VER", Sol1->Cost->TileOverhead, Sol2->Cost->TileOverhead);
		if (TakeSol1) {
                    PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
                } else {
                    PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
                }
        } else if (Sol1) {
                PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
	} else if (Sol2) {
                PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
	} else {
		GenTilingError("Failed to Generate code for Kernel: %s", Name);
	}
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        return 1;
}

int CNN_HWC_DWConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)
{
	Kernel_T *Ker = 0, *Sol1 = 0, *Sol2 = 0;
        float K = 0.9;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        if (Ctrl) {
		if (Ctrl->TileOrientation != -1) {
			printf("TileOrientation set by user\n");
			Ker = CNN_HWC_DWConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
			if (Ker!=0) return 1;
			else GenTilingError("CNN_MM_ConvolutionPoolAct_SQ8: %s, Failed to gen with set tiling orientation, try to let the Autotiler set it for you", Name);
		}
	}

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;

	printf("\n\n=============================== Trying Tile Orientation: TILE_HOR ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(0));
        Ker = CNN_HWC_DWConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);

	printf("\n=============================== Trying Tile Orientation: TILE_VER ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(1));
        Ker = CNN_HWC_DWConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol2 = CopyAndPopUserKernel(Ker);

        if (Sol1 && Sol2) {
		int TakeSol1 = ((K*Sol1->Cost->TileOverhead) < Sol2->Cost->TileOverhead);  // K close to 1.0if (TakeSol1) {
		printf(">>>>>>>>>>>>>>>>>> %s is better: %.3f vs %.3f \n\n\n", TakeSol1?"TILE_HOR":"TILE_VER", Sol1->Cost->TileOverhead, Sol2->Cost->TileOverhead);
		if (TakeSol1) {
                    PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
                } else {
                    PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
                }
        } else if (Sol1) {
                PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
	} else if (Sol2) {
                PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
	} else {
		GenTilingError("Failed to Generate code for Kernel: %s", Name);
	}
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        return 1;
}

int CNN_ConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

       	int InFeat,
       	int OutFeat,
       	int Width,
       	int Height,

	KernelOper_T ConvOper,
       	int Fcx,
       	int Fcy,
	int Dcx,
	int Dcy,
	int Scx,
	int Scy,
	int ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int PoolPad,

	KernelOper_T ActOper
	)
{
	Kernel_T *Ker = 0, *Sol1 = 0, *Sol2 = 0;
        float K = 0.9;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        if (Ctrl) {
		if (Ctrl->TileOrientation != -1) {
			printf("TileOrientation set by user\n");
			Ker = CNN_ConvolutionPoolAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
			if (Ker!=0) return 1;
			else GenTilingError("CNN_ConvolutionPoolAct_SQ8: %s, Failed to gen with set tiling orientation, try to let the Autotiler set it for you", Name);
		}
	}
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;

	printf("\n\n=============================== Trying Tile Orientation: TILE_HOR ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(0));
        Ker = CNN_ConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);

	printf("\n=============================== Trying Tile Orientation: TILE_VER ===============================\n\n");
    	CNN_SetGenCtrl(&InternalCtrl, "TILEORIENTATION", AT_OPT_VAL(1));
        Ker = CNN_ConvolutionPoolAct_SQ8_Internal(Name, &InternalCtrl, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        if (Ker) Sol2 = CopyAndPopUserKernel(Ker);

        if (Sol1 && Sol2) {
		int TakeSol1 = ((K*Sol1->Cost->TileOverhead) < Sol2->Cost->TileOverhead);  // K close to 1.0if (TakeSol1) {
		printf(">>>>>>>>>>>>>>>>>> %s is better: %.3f vs %.3f \n\n\n", TakeSol1?"TILE_HOR":"TILE_VER", Sol1->Cost->TileOverhead, Sol2->Cost->TileOverhead);
		if (TakeSol1) {
                    PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
                } else {
                    PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
                }
        } else if (Sol1) {
                PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
	} else if (Sol2) {
                PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
	} else {
		GenTilingError("Failed to Generate code for Kernel: %s", Name);
	}
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        return 1;
}

int CNN_PoolAct_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Feat, int Width, int Height, KernelOper_T PoolOper, int Fpx, int Fpy, int Dpx, int Dpy, int Spx, int Spy, int PoolPad, KernelOper_T ActOper)
{
	return (CNN_PoolAct_SQ8_Internal(Name, Ctrl, Feat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper)!=0);
}

int CNN_Act_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Feat, int Width, int Height,KernelOper_T ActOper) {
	return (CNN_Act_SQ8_Internal(Name, Ctrl, Feat, Width, Height, ActOper)!=0);
}

int CNN_GlobalPoolAct_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Feat, int Width, int Height, KernelOper_T PoolOper, KernelOper_T ActOper) {
	return (CNN_GlobalPoolAct_SQ8_Interal(Name, Ctrl, Feat, Width, Height, PoolOper, ActOper)!=0);
}

int CNN_LinearAct_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Bias_DataSize, int Scale_DataSize, int InDim, int OutDim, KernelOper_T LinearOper, KernelOper_T ActOper)
{
	return (CNN_LinearAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, InDim, OutDim, LinearOper, ActOper)!=0);
}

int CNN_SoftMax_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Dim, KernelOper_T SoftMaxOper) {
	return (CNN_SoftMax_SQ8_Internal(Name, Ctrl, Dim, SoftMaxOper)!=0);
}

int CNN_SoftMax2D_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Dim, int N, KernelOper_T SoftMaxOper) {
	return (CNN_SoftMax2D_SQ8_Internal(Name, Ctrl, Dim, N, SoftMaxOper)!=0);
}

int CNN_MatAddAct_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Feat, int Width, int Height, KernelOper_T AddMatOper, KernelOper_T ActOper) {
	return (CNN_MatAddAct_SQ8_Internal(Name, Ctrl, Feat, Width, Height, AddMatOper, ActOper)!=0);
}

int CNN_MatMulAct_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Bias_DataSize, int Scale_DataSize, int ColM1, int LineM1, int ColM2, int LineM2, int Width, int Height, int Scx, int Scy, KernelOper_T MatMulOper, KernelOper_T ActOper) {
	return (CNN_MatMulAct_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, ColM1, LineM1, ColM2, LineM2, Width, Height, Scx, Scy, MatMulOper, ActOper, 1)!=0);
}

int CNN_MatMulSmallM1Act_SQ8(char *Name, CNN_GenControl_T *Ctrl, int Bias_DataSize, int Scale_DataSize, int ColM1, int LineM1, int ColM2, int LineM2, int Width, int Height, int Scx, int Scy, KernelOper_T MatMulOper, KernelOper_T ActOper) {
	return (CNN_MatMulSmallM1Act_SQ8_Internal(Name, Ctrl, Bias_DataSize, Scale_DataSize, ColM1, LineM1, ColM2, LineM2, Width, Height, Scx, Scy, MatMulOper, ActOper)!=0);
}

