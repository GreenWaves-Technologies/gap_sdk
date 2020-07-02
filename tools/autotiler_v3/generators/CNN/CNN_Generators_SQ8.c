#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators_SQ8.h"
#include "CNN_Generator_Util.h"
#include "Gap.h"

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

#define AT_INF_BIASL_SM         0
#define AT_INF_ACTSCALE         0
#define AT_INF_ACTSCALEN        1
#define AT_INF_A0               2
#define AT_INF_B0               3
#define AT_INF_C0               4

#define AT_INF_BIASN            5
#define AT_INF_IN1SCALE         5
#define AT_INF_SCALEN           5

#define AT_INF_IN1SCALEN        6
#define AT_INF_OUTSCALE         7
#define AT_INF_OUTSCALEN        8

#define AT_INF_DIM              9

void LoadCNN_SQ8_Library()

{
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
                        TCArg("unsigned char", "N"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "Ny"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
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
			TCArg("unsigned char", "FS"),
			TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "FSy"),
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
	LibKernelTemplate("KerMatTranspose_fps_T",
                  CArgs(7,
			TCArg("signed char *__restrict__", "In"),
			TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy")
			)
	);
	LibKernelTemplate("KerSoftMax_SQ8_T",
                  CArgs(5,
			TCArg("signed char *__restrict__", "In"),
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

        LibKernel("KerParConv5x1Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 5,5,1,1,-1,-2));
        LibKernel("KerParConv7x7StrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), 7,7,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerConv_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));

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
	LibKernel("KerParReduct_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReduct_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerParReductIO_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSIGMOID), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSWISH), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParReductIO_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_LEAKYRELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	/* Activation wth tensor centric scaling */
        LibKernel("KerPar_ReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_RELU), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_ReLUN_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_RELUN), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_HSIGMOID), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_HSwish_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",			CNN_Match(CNN_OperList(1, KOP_HSWISH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_LEAKYRELU), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

	/* Pooling (Max or Avg) with tensor centric scaling and optional ReLU or ReLUN activation */
        LibKernel("KerParPool2x2Stride2_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPool2x2Stride2_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 2,2,1,1,2,2));

        LibKernel("KerParPoolNxNStrideS_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxNStrideS_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), -1,-2,1,1,-1,-2));

        LibKernel("KerParPoolNxMStrideSxSy_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLU_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));
        LibKernel("KerParPoolNxMStrideSxSy_ReLUN_SQ8", CALL_PARALLEL, 0, "KerPool_SQ8_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), -1,-1,1,1,-1,-1));

	/* Global Pooling (Max or Avg) with tensor centric scaling and optional ReLU or ReLUN activation */
        LibKernel("KerParGlobalMaxPoolFullFeat_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPoolFullFeat_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalMaxPool_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalMaxPool_Reduct_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalAvgPoolFullFeat_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", 	CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPoolFullFeat_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T", CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        LibKernel("KerParGlobalAvgPool_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_NONE), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLU_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELU), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_Reduct_ReLUN_SQ8", CALL_PARALLEL, 0, "KerGlobalPool_SQ8_T",CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL_REDUCT), CNN_OperList(1, KOP_RELUN), 1,
													  CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));

	/* Matrix Algebra */

	/* Matrix Addition with tensor centric scaling and optional activation */
	LibKernel("KerParMatAdd_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAdd_ReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAdd_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAdd_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_HSIGMOID), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAdd_HSwish_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_HSWISH), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAdd_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(1,1,0,0,1), 0,0,0,0,0,0));

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

	/* 32b Bias */
	LibKernel("KerParMatMulB32_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));

	LibKernel("KerParMatMulSxSyB32_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB32_ReLU_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSxSyB32_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,-1,-1));

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
	LibKernel("KerParMatMulB32_ReLU_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulB32_ReLUN_SF_SQ8", CALL_PARALLEL, 0, "KerMatMul_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,1,1));


	/* Matrix by vector multiplication with tensor centric scaling and optional activation */
	LibKernel("KerParMatVectMul_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_ReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_RELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_ReLUN_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_RELUN), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_HSIGMOID), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_HSwish_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_HSWISH), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));
	LibKernel("KerParMatVectMul_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerMat3_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_MATVECTMUL), CNN_OperList(1, KOP_LEAKYRELU), 1, CNN_Type(1,1,1,0,1), 0,0,0,0,1,1));

	/* Matrix Transposition, no scaling */
	LibKernel("CNN_ParTranspose_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_ParTransposeSxSy_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,-1,-1));
	LibKernel("CNN_Transpose_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_TransposeSxSy_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,-1,-1));

	/* Tensor Permutation, no scaling */
	LibKernel("CNN_MatPermCHW2CWH_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2CWH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HWC_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HWC), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WHC_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WHC), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WCH_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WCH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HCW_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HCW), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));

	/* SoftMax, pre scaling */
	LibKernel("KerParSoftMax_SQ8", CALL_PARALLEL, 0, "KerSoftMax_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, -1, CNN_Type(1,0,0,0,2), 0,0,0,0,0,0));



	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 8 bits. Kernels for a single feature evaluated in parallel          */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerSetBiasB8_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(1,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB16_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,0,4), 0,0,0,0,0,0));
        LibKernel("KerSetBiasB32_SQ8", CALL_PARALLEL, 0, "KerSetBias_SQ8_T", 		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));

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
	LibKernel("KerReduct_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_HSWISH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReduct_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_LEAKYRELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	LibKernel("KerReductIO_CC_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_NONE), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_ReLUN_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_RELUN), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSIGMOID), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_HSwish_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_HSWISH), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerReductIO_CC_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerConvLinReduct_SQ8_T",CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), CNN_OperList(1, KOP_LEAKYRELU), 0,
												  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));

	/* Activations with tensor centric scaling */
        LibKernel("Ker_ReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELU), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_ReLUN_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_RELUN), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSigmoid_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_HSIGMOID), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_HSwish_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_HSWISH), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_LeakyReLU_SQ8", CALL_PARALLEL, 0, "KerActivation_SQ8_T",		CNN_Match(CNN_OperList(1, KOP_LEAKYRELU), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));


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

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_ConvolutionPoolAct_SQ8
	
*********************************************************************************************************************************************************************/

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
	if (ConvOper==KOP_NONE) {
		if (PoolOper!=KOP_NONE)
			return CNN_PoolAct_SQ8(Name, Ctrl, InFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_SQ8(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, All requested operations are KOP_NONE", Name);
	}

	int ParFeat = 1; 
	Tile_Orientation_T TileOrientation = TILE_HOR;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
	}
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
	int Log=1;

	if (!(ConvOper == KOP_NONE || ConvOper == KOP_CONV || ConvOper == KOP_CONV_DW))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

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
	LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
	if (!DWConv) LayerOp *= InFeat;
	if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += OutFeat*Wo*Ho;
	LayerBandwidth += Width*Height*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += Wo*Ho*1*OutFeat;
	LayerBandwidth += Fcx*Fcy*1*InFeat*(DWConv?1:OutFeat);
	LayerBandwidth += Bias_DataSize*OutFeat;

	/* Basic Kernel Matching */
	if (!DWConv) {
		SetBiasKerName = CNN_FindMatchingKernel(KOP_SETBIAS, KOP_NONE, ParFeat, Bias_DataSize, 0, 0, 0, 4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolAct_SQ8 Kernel: %s, Can't find a matching Set Bias basic kernel", Name);
	}

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
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
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
		if ((InFeat+OutFeat)<100) {
			if (Log) printf("Mapping this convolution to matrix multiplication with small first operand\n");
			int Ok = CNN_MatMulSmallM1Act_SQ8(Name, 0, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL_SM1, ActOper);
			if (!Ok&&Log) printf("Mapping this convolution to matrix multiplication with small first operand FAILED, trying with standard mult implementation\n");
			if (Ok) return Ok;
		}
		if (Log) printf("Mapping this convolution to matrix multiplication\n");
		int Ok = CNN_MatMulAct_SQ8(Name, 0, Bias_DataSize, Scale_DataSize, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL, ActOper);
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
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),    "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");

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
	return (Kernel!=0);
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

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

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
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_GroupedConvolutionPoolAct_SQ8: Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

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
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),                 "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),             "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),                "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");

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

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Out, Infos)

	CNN_PoolAct_SQ8
		
*********************************************************************************************************************************************************************/

int CNN_PoolAct_SQ8(
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
	if (PoolOper==KOP_NONE && ActOper!=KOP_NONE) return CNN_Act_SQ8(Name, Ctrl, Feat, Width, Height, ActOper);

	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
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
	int Log=1;

	if (!(PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_Pool_SQ8 Kernel: %s, PoolOper, expecting KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_Pool_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

	/* Set Kernel characteristics */
	CNN_LayerOutputDim(Width, Height, KOP_NONE, 1, 1, 1, 1, 1, 1, 1, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, 0, 0, &Wo, &Ho, 0, 0, &PadPw, &PadPh);
	PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
	CNN_TileOverlap(TileOrientation, 1, 1, 1, 1, 1, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy, 0, &OverlapP);
	UsedWidth  = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHeight = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
	TileCons = (TileOrientation==TILE_HOR)?(Spy):(Spx);
	/* Re evaluate truly used width/height and update padding accordingly */
	PadInp[1] = Max(0, PadInp[1]-(Width-UsedWidth)); PadInp[3] = Max(0, PadInp[3]-(Height-UsedHeight));



	PoolKerName = CNN_FindMatchingKernel(PoolOper, ActOper, ParFeat, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
	if (PoolKerName==0) PoolKerName = CNN_FindMatchingKernel(PoolOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
	else if (ActOper) StandAloneAct = 0;
	if (PoolKerName==0) GenTilingError("CNN_Pool_SQ8 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_Pool_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (PoolOper) LayerOp += Feat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += Feat*Wo*Ho;

	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += Wo*Ho*1*Feat;

	if (Log) {
        	printf("Pool => W: %d, Pad:[%d,%d] => Wo: %d\n", Width,  PadInp[0], PadInp[1], Wo);
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
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),     "In");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),    "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),  "Infos");

	Object_T **KArgs = AllocateKerArgs(3);
	int Ka=0;
	KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,D0,T0), OBJ_IN_DB,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, 1, OverlapP, 0, TileCons, "In");
	KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,D0,T0), OBJ_OUT_DB,     Wo,     Ho,                   		        1,        0, 0,        0, "Out");
	KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,   1,  	                1,        0, 0,        0, "Infos");

        Kernel_T *Kernel = UserKernel(Name,
		ParFeat?
		KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
		KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
                TileOrientation,
                KCArgs,
                Calls(2,
			Call(PoolKerName, LOC_LOOP,
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
	return (Kernel!=0);
}


/*********************************************************************************************************************************************************************
 	Generator for Activation with tensor centric scaling

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ActOper:	KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Out, Infos)

	CNN_Act_SQ8
		
*********************************************************************************************************************************************************************/

int CNN_Act_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

	KernelOper_T ActOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	int TileCons = 0;
	char *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=KOP_NONE);
	int Log=1;

	if (!(ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_Act_SQ8 Kernel: %s, ActOper, expecting KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

	ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (ActKerName==0) GenTilingError("CNN_Act_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);

	LayerOp += Feat*Width*Height;

	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += Width*Height*1*Feat;

	if (Log) {
        	printf("Act  => W: %d, Wo: %d\n", Width,  Width);
        	printf("     => H: %d, Ho: %d\n", Height, Height);
		printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

        Kernel_T *Kernel = UserKernel(Name,
		ParFeat?
		KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
		KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
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
					ParFeat?K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* Number of features in this tile */
					K_Arg("In", KER_ARG_TILE_W),					/* Tile width */
					K_Arg("In", KER_ARG_TILE_H),					/* Tile height */
					K_Arg("Infos", KER_ARG_TILE)					/* Infos */
				)
			)
                     ),
                KerArgs(3,
			KerArg("In",     KerArgSpace(2,D0,T0), OBJ_IN_DB,                         Width, Height, 1, 0, 0, 0, "In"),
			KerArg("Out",    KerArgSpace(2,D0,T0), OBJ_OUT_DB,                        Width, Height, 1, 0, 0, 0, "Out"),
			KerArg("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, AT_INF_DIM,      1, 1, 0, 0, 0, "Infos")
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
	return (Kernel!=0);
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

		ActOper:	Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Out, Infos)


	CNN_GlobalPoolAct_SQ8
		
*********************************************************************************************************************************************************************/

int CNN_GlobalPoolAct_SQ8(
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
	int ParFeat = 1;
	int Wo, Ho;
	char *PoolKerName=0, *ActKerName=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=0);
	int Log=1;

	if (!(PoolOper == KOP_GLOBAL_MAXPOOL || PoolOper == KOP_GLOBAL_AVGPOOL))
		GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, PoolOper should be KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

	PoolKerName = CNN_FindMatchingKernel(PoolOper, ActOper, ParFeat, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (PoolKerName) StandAloneAct = 0;
	else if (StandAloneAct) PoolKerName = CNN_FindMatchingKernel(PoolOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (PoolKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

	if (StandAloneAct) {
	       	ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		if (ActKerName==0) GenTilingError("CNN_GlobalPoolAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

        Wo = 1; Ho = 1;


	if (PoolOper) LayerOp += Feat*Wo*Ho*Width*Height;
	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += Wo*Ho*1*Feat;

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
		KernelOper_T ReductOper = (PoolOper==KOP_GLOBAL_MAXPOOL)?KOP_GLOBAL_MAXPOOL_REDUCT:KOP_GLOBAL_AVGPOOL_REDUCT;

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
	return (Kernel!=0);
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
		ActOper		Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_LinearAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_LinearAct_SQ8(
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
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 1, Bias_DataSize, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
	}

	LayerOp += InDim*OutDim;
	if (ActOper) LayerOp += OutDim;
	LayerBandwidth += InDim*OutDim*1;
	LayerBandwidth += OutDim*1;
	LayerBandwidth += InDim*OutDim*1;
	LayerBandwidth += Bias_DataSize*OutDim;

	if (Log) {
		printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s\n", Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
		if (LinearKerName) printf("Linear Kernel: %s\n", LinearKerName);
		if (ActKerName)    printf("Act Kernel: %s\n", ActKerName);
	}
	Kernel_T *Kernel;

	CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),                "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),            "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),  "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),               "Out");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1),          "Scale");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1),         "ScaleN");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),             "Infos");

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
	return (Kernel!=0);
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

int CNN_SoftMax_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Dim,

        KernelOper_T SoftMaxOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernel(SoftMaxOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax_SQ8 Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only, Q15 output", Name);

	LayerOp += Dim;
	LayerBandwidth += Dim*1 + Dim*2;
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(1,1,1), "In"),
                      TCArg(CNN_ArgDataType(2,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_LOOP,
				Bindings(5,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
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
		AddKernelArgDim(Name, "Out", 2, Dim, 2);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Dim,Dim,1,1, 1, SoftMaxOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, 0);
	}
	return (Kernel!=0);
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

int CNN_MatAddAct_SQ8(
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
	int ParFeat = 1;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int StandAloneAct = (ActOper!=KOP_NONE);
	char *MatAddKerName=0, *ActKerName=0;

	MatAddKerName = CNN_FindMatchingKernel(AddMatOper, ActOper, ParFeat, 1, 1, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatAddKerName) StandAloneAct = 0;
	else MatAddKerName = CNN_FindMatchingKernel(AddMatOper, KOP_NONE, ParFeat, 1, 1, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatAddKerName==0) GenTilingError("CNN_MatAddAct_SQ8 Kernel: %s, Can't find a matching basic kernel for MatAdd", Name);

	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 1, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatAddAct_SQ8 Kernel: %s, Can't find a matching basic kerne for Activationl", Name);
	}

	LayerOp += Feat * Width * Height;
	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += Width*Height*1*Feat;

        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(4,
                      TCArg(CNN_ArgDataType(1,1,1), "In1"),
                      TCArg(CNN_ArgDataType(1,1,1), "In2"),
                      TCArg(CNN_ArgDataType(1,1,1), "Out"),
                      TCArg(CNN_ArgDataType(1,1,1), "Infos")
                     ),
                Calls(2,
			Call(MatAddKerName, LOC_LOOP,
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
                KerArgs(4,
                        KerArg("In1",    KerArgSpace(2,D0,T0), O_IN|O_DB,            Width, Height,  1,             0, 0, 0, "In1"),
                        KerArg("In2",    KerArgSpace(2,D0,T0), O_IN|O_DB,            Width, Height,  1,             0, 0, 0, "In2"),
                        KerArg("Out",    KerArgSpace(2,D0,T0), O_OUT|O_DB,           Width, Height,  1,             0, 0, 0, "Out"),
			KerArg("Infos",  KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED,      1,     1,  AT_INF_DIM*1,  0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In1", 4, Feat,  Height, Width, 1);
		AddKernelArgDim(Name, "In2", 4, Feat,  Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, AddMatOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
	}
	return (Kernel!=0);
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
		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

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
	int ParFeat = 1;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *MatOperKerName=0, *ActKerName=0;
	int StandAloneAct=(ActOper!=KOP_NONE);

	MatOperKerName = CNN_FindMatchingKernel(MatOper, ActOper, ParFeat, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatOperKerName) StandAloneAct=0;
	else if (StandAloneAct) MatOperKerName = CNN_FindMatchingKernel(MatOper, KOP_NONE, ParFeat, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatOperKerName==0) GenTilingError("CNN_TensorVectMultAct_SQ8 Kernel: %s, Can't find a matching basic kernel for MatVectMult", Name);
	if (StandAloneAct) {
		ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 1, 1, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_TensorVectMultAct_SQ8 Kernel: %s, Can't find a matching basic kernel for Activation", Name);
	}


	LayerOp += Feat * Width * Height;

	LayerBandwidth += Width*Height*1*Feat;
	LayerBandwidth += 1*Feat;
	LayerBandwidth += Width*Height*1*Feat;

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
	KArgs[Ka++] = KerArg("In1",   KerArgSpace(2,D0,T0), O_IN|O_DB,            1, 1, Width*Height*1,  0, 0, 0, "In1");
	KArgs[Ka++] = KerArg("In2",   KerArgSpace(1,D0),    O_IN|O_DB,            1, 1, 1,               0, 0, 0, "In2");
	KArgs[Ka++] = KerArg("Out",   KerArgSpace(2,D0,T0), O_OUT|O_DB,           1, 1, Width*Height*1,  0, 0, 0, "Out");
	KArgs[Ka++] = KerArg("Infos", KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED, 1, 1, AT_INF_DIM*1,    0, 0, 0, "Infos");


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

int CNN_MatMulAct_SQ8(
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

	if (!(MatMulOper == KOP_MATMUL)) GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, MatMulOper should be KOP_MATMUL", Name);

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MatMulAct_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

	if (ColM1 != LineM2) GenTilingError("CNN_MatMulAct_SQ8: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d] %s", Name, LineM1, ColM1, LineM2, ColM2);
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
	LayerOp += ColM1*ColO*LineM1;
	LayerBandwidth += LineM1*(ColM1*ColM2*(1+1));
	LayerBandwidth += LineM1*ColM2*1;
	LayerBandwidth += LineM1*Bias_DataSize;

	if (Scy!=1) ConsT0 = Width*Scy; else ConsT0 = 4;
	
	if (Log) {
		printf("CNN_MatMul_SQ8: %s\n", Name);
		printf("In1  => W: %4d, H: %4d\n", ColM1, LineM1);
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d\n", ColM2, LineM2, Width, Height, Scx, Scy);
		printf("Out  => W: %4d, H: %4d => %s\n", ColO, LineO, ColFirst?"Column first":"Line First");
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		if (ActKerName)    printf("%20s: %s\n", "ActKerName", ActKerName);
		// printf("Nb Oper : %lld\n", LayerOp);
	}

	Kernel_T *Kernel;

	/* First try buffering small objects */
	Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  "In2"),
                      TCArg(CNN_ArgDataType(1,1,1),  "In1"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"ScaleN"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(2,
			Call(MatMulKerName, LOC_LOOP,
				Bindings(19,
					K_Arg("In1",  KER_ARG_TILE), K_Arg("In1",  KER_ARG_TILE_W), K_Arg("In1",  KER_ARG_TILE_H),
					K_Arg("In2",  KER_ARG_TILE), K_Arg("In2",  KER_ARG_TILE_W),
					K_Arg("Bias", KER_ARG_TILE),
					K_Arg("Scale", KER_ARG_TILE), K_Arg("ScaleN", KER_ARG_TILE),
					K_Arg("Out", KER_ARG_TILE),  K_Arg("Out", KER_ARG_TILE_W), K_Arg(ColFirst?"In1":"In2",  KER_ARG_TILE_BASE),
					K_Arg("KerBuff", KER_ARG_TILE),
					K_TileOper("Infos", "char *", '@', AT_INF_BIASN),
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
			KerArg("KerBuff",KerArgSpace(1, T1), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0),
			KerArg("In1",    KerArgSpace(1, T0), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T1), O_IN|O_DB,             ColM2,  LineM2, 1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
			KerArg("Bias",   KerArgSpace(1, T0), O_BUFF|O_IN|O_CONST,       1,  LineO,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"),
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "Out"),
			KerArg("Scale",  KerArgSpace(1, T0), O_BUFF|O_IN|O_CONST,        1, LineO,  1,             0, 0,                                                0, "Scale"),
			KerArg("ScaleN", KerArgSpace(1, T0), O_BUFF|O_IN|O_CONST,        1, LineO,  1,             0, 0,                                                0, "ScaleN"),
			KerArg("Infos",  KerArgSpace(1, T1), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
		):
		KerArgs(8,
			KerArg("KerBuff",KerArgSpace(1, T0), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0),
			KerArg("In1",    KerArgSpace(1, T1), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T0), O_IN|O_DB,             ColM2,  LineM2, 1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
			KerArg("Bias",   KerArgSpace(1, T1), O_BUFF|O_IN|O_CONST,       1,  LineO,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"),
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out"),
			KerArg("Scale",  KerArgSpace(1, T1), O_BUFF|O_IN|O_CONST,        1, LineO,  1,             0, 0,                                                0, "Scale"),
			KerArg("ScaleN", KerArgSpace(1, T1), O_BUFF|O_IN|O_CONST,        1, LineO,  1,             0, 0,                                                0, "ScaleN"),
			KerArg("Infos",  KerArgSpace(1, T0), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
		)
	);
	/* If no soultion found roll back to tiled approach for small objects */
	if (Kernel==0) 
	Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),  "In2"),
                      TCArg(CNN_ArgDataType(1,1,1),  "In1"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Out"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"Scale"),
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"ScaleN"),
                      TCArg(CNN_ArgDataType(1,1,1),  "Infos")
                ),
		Calls(2,
			Call(MatMulKerName, LOC_LOOP,
				Bindings(19,
					K_Arg("In1",  KER_ARG_TILE), K_Arg("In1",  KER_ARG_TILE_W), K_Arg("In1",  KER_ARG_TILE_H),
					K_Arg("In2",  KER_ARG_TILE), K_Arg("In2",  KER_ARG_TILE_W),
					K_Arg("Bias", KER_ARG_TILE),
					K_Arg("Scale", KER_ARG_TILE), K_Arg("ScaleN", KER_ARG_TILE),
					K_Arg("Out", KER_ARG_TILE),  K_Arg("Out", KER_ARG_TILE_W), K_Arg(ColFirst?"In1":"In2",  KER_ARG_TILE_BASE),
					K_Arg("KerBuff", KER_ARG_TILE),
					K_TileOper("Infos", "char *", '@', AT_INF_BIASN),
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
			KerArg("KerBuff",KerArgSpace(1, T1), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0),
			KerArg("In1",    KerArgSpace(1, T0), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T1), O_IN|O_DB,             ColM2,  LineM2, 1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
			KerArg("Bias",   KerArgSpace(1, T0), O_IN|O_DB|O_CONST,         1,  LineO,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"),
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "Out"),
			KerArg("Scale",  KerArgSpace(1, T0), OBJ_IN_DB|O_CONST,          1, LineO,  1,             0, 0,                                                0, "Scale"),
			KerArg("ScaleN", KerArgSpace(1, T0), OBJ_IN_DB|O_CONST,          1, LineO,  1,             0, 0,                                                0, "ScaleN"),
			KerArg("Infos",  KerArgSpace(1, T1), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
		):
		KerArgs(8,
			KerArg("KerBuff",KerArgSpace(1, T0), O_BUFF|O_NTILED, Nbuff*ColM1,  1,      1,             0, 0,                                                0, 0),
			KerArg("In1",    KerArgSpace(1, T1), O_IN|O_DB|O_CONST,     ColM1,  LineM1, 1,             0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T0), O_IN|O_DB,             ColM2,  LineM2, 1,             0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
			KerArg("Bias",   KerArgSpace(1, T1), O_IN|O_DB|O_CONST,         1,  LineO,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"),
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB,             ColO,  LineO,  1,             0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out"),
			KerArg("Scale",  KerArgSpace(1, T1), OBJ_IN_DB|O_CONST,          1, LineO,  1,             0, 0,                                                0, "Scale"),
			KerArg("ScaleN", KerArgSpace(1, T1), OBJ_IN_DB|O_CONST,          1, LineO,  1,             0, 0,                                                0, "ScaleN"),
			KerArg("Infos",  KerArgSpace(1, T0), O_IN|O_BUFF|O_NTILED,       1,     1,  AT_INF_DIM*1,  0, 0,                                                0, "Infos")
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
	return (Kernel!=0);
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

        	ActOper		Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out, Scale, ScaleN, Infos)

	CNN_MatMulSmallM1Act_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatMulSmallM1Act_SQ8(
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

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MatMulSmallM1Act_SQ8 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

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
	LayerOp += ColM1*ColO*LineM1;
	LayerBandwidth += LineM1*(ColM1*ColM2*(1+1));
	LayerBandwidth += LineM1*ColM2*1;
	LayerBandwidth += LineM1*Bias_DataSize;

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
                      TCArg(CNN_ArgDataTypeUns(1,1,1),"ScaleN"),
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
			Call(ActKerName, LOC_LOOP_EPILOG,
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
	return (Kernel!=0);

}

/*********************************************************************************************************************************************************************
 	Generator for Matrix Transposition

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat		Number of matrices
		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map

		MatTransOper	KOP_MATTRANSP

		Signature:	Name(In, Out)

	CNN_MatTranspose_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatTranspose_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatTransOper
)

{
	int Log = 1;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	unsigned int OutTileOrientation;
	int ParFeat = 1;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	unsigned long long int LayerOp = Width*Height*Feat;
	unsigned long long int LayerBandwidth = 0;

	if (!(MatTransOper == KOP_MATTRANSP)) GenTilingError("CNN_MatTranspose_SQ8 Kernel: %s, MatTransOper should be KOP_MATTRANSP", Name);

	char *MatTransKerName = CNN_FindMatchingKernel(MatTransOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
	if (MatTransKerName==0) GenTilingError("CNN_MatTranspose_SQ8 Kernel: %s, Can't find a matching basic kernel for Matrix Transpose", Name);

	if (TileOrientation==TILE_HOR) OutTileOrientation = OBJ_CONSTRAINTS_TILE_VER; else OutTileOrientation = OBJ_CONSTRAINTS_TILE_HOR;
	LayerBandwidth += Width*Height*1;
	LayerBandwidth += Width*Height*1;

	if (Log) {
		printf("CNN_MatTranspose: %s %s\n", Name, ParFeat?"Par Feat":"");
		printf("In  => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		printf("Out => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		if (MatTransKerName) printf("%20s: %s\n", "MatTransKerName", MatTransKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Kernel_T *Kernel =
		UserKernel(Name,
			(ParFeat)?
			KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
			KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
	        	TileOrientation,
                	CArgs(2,
                      		TCArg(CNN_ArgDataType(1,1,1),  "In"),
                      		TCArg(CNN_ArgDataType(1,1,1), "Out")
                	),
			Calls(1,
				Call(MatTransKerName, LOC_LOOP,
					Bindings(7,
						K_Arg("In", KER_ARG_TILE),			/* Input tile */
						K_Arg("Out", KER_ARG_TILE),			/* Output tile */
						(ParFeat)?
						K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):	/* Number of Matrices involved */
						Imm(1),						/* A single matrix */
						K_Arg("In", KER_ARG_TILE_W),			/* Input tile width */
						K_Arg("In", KER_ARG_TILE_H),			/* Input tile height */
						AT_IGNORE_ARG_BINDING,				/* StrideX */
						AT_IGNORE_ARG_BINDING				/* StrideY */
					)
				)
			),
	                KerArgs(2,
	                        KerArg("In",   KerArgSpace(2,D0,T0), O_IN|O_DB,  Width, Height, 1,  0, 0, 0, "In"),
	                        KerArg("Out",  KerArgSpace(2,D0,T0), O_OUT|O_DB, Height, Width, 1, 0, OutTileOrientation, 0, "Out")
			)
		);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In",  4, Feat, Height, Width, 1);
		AddKernelArgDim(Name, "Out", 4, Feat, Width, Height, 1);
		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, MatTransOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for 3D Tensor permutations:  CHW => {CWH, HWC, WHC, WCH, HCW}

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat		Number of channels of the tensor
		Width		Tensor width
		Height		Tensor height

		MatPermOper	Permutation oper:  KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH, KOP_MATPERM_CHW2HCW

		Signature:	Name(In, Out)

	CNN_3DTensorPermute_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_3DTensorPermute_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatPermOper
)

{
	int Log = 1;
	int ParFeat = 1;
	unsigned long long int LayerOp = Width*Height*Feat;
	unsigned long long int LayerBandwidth = 0;

	if (!(MatPermOper == KOP_MATPERM_CHW2CWH || MatPermOper == KOP_MATPERM_CHW2HWC || MatPermOper == KOP_MATPERM_CHW2WHC ||
	      MatPermOper == KOP_MATPERM_CHW2WCH || MatPermOper == KOP_MATPERM_CHW2HCW))
		GenTilingError("CNN_3DTensorPermute_SQ8 Kernel: %s, MatTransOper should be KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH or KOP_MATPERM_CHW2HCW", Name);
	char *MatPermKerName = CNN_FindMatchingKernel(MatPermOper, KOP_NONE, ParFeat, 1, 0, 0, 0, 1, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
	if (MatPermKerName==0) GenTilingError("CNN_3DTensorPermute_SQ8 Kernel: %s, Can't find a matching basic kernel for 3D Tensor Permutation", Name);

	LayerBandwidth += Feat*Width*Height*1;
	LayerBandwidth += Feat*Width*Height*1;
	if (Log) {
		printf("CNN_MatPermute: %s %s\n", Name, ParFeat?"Par Feat":"");
		printf("In  => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		printf("Out => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		if (MatPermKerName) printf("%20s: %s\n", "MatPermKerName", MatPermKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(2);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width, Height*Feat, 1,  0, 0, 0, "In");
	switch (MatPermOper) {
		case KOP_MATPERM_CHW2CWH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Height, Feat, 1, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_CHW2HWC:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Feat, Height, 1, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_CHW2WHC:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Height*Feat, Width, 1, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_CHW2WCH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Height*Feat, Width, 1, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_CHW2HCW:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width, Height*Feat, 1, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
	}
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Kernel_T *Kernel = UserKernel(Name,
			KernelIterSpace(1, IterTiledSpace(T0)),
	        	TILE_VER,
                	CArgs(2, TCArg(CNN_ArgDataType(1,1,1),  "In"), TCArg(CNN_ArgDataType(1,1,1), "Out")),
			Calls(1,
				Call(MatPermKerName, LOC_LOOP,
					Bindings(7,
						K_Arg("In", KER_ARG_TILE),	/* Input tile */
						K_Arg("Out", KER_ARG_TILE),	/* Output tile */
						Imm(Feat), 			/* Number of Channels */
						K_Arg("In", KER_ARG_TILE_W),	/* Input tile width */
						Imm(Height),			/* Input tile height */
						AT_IGNORE_ARG_BINDING,		/* StrideX */
						AT_IGNORE_ARG_BINDING		/* StrideY */
					)
				)
			),
			PKerArgs
		);
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
	if (Kernel==0) {
		printf("NEW SCHEME\n");
		PKerArgs[0] = KerArg("In",   KerArgSpace(3,D2,D1,D0), O_IN|O_DB,  1, 1,  1,  0, 0, 0, "In");
		switch (MatPermOper) {
			case KOP_MATPERM_CHW2CWH:
				PKerArgs[1] = KerArg("Out",  KerArgSpace(3,D2,D0,D1), O_OUT|O_DB, 1, 1,  1,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2HWC:
				PKerArgs[1] = KerArg("Out",  KerArgSpace(3,D1,D0,D2), O_OUT|O_DB, 1, 1,  1,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2WHC:
				PKerArgs[1] = KerArg("Out",  KerArgSpace(3,D0,D1,D2), O_OUT|O_DB, 1, 1,  1,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2WCH:
				PKerArgs[1] = KerArg("Out",  KerArgSpace(3,D0,D2,D1), O_OUT|O_DB, 1, 1,  1,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2HCW:
				PKerArgs[1] = KerArg("Out",  KerArgSpace(3,D1,D2,D0), O_OUT|O_DB, 1, 1,  1,  0, 0, 0, "Out");
				break;
		}
		Kernel = UserKernel(Name,
				KernelIterSpace(3, IterParSpace(D2, Feat, 1), IterParSpace(D1, Height, 1), IterParSpace(D0, Width, Width)),
	        		TILE_HOR,
                		CArgs(2, TCArg(CNN_ArgDataType(1,1,1),  "In"), TCArg(CNN_ArgDataType(1,1,1), "Out")),
				Calls(1,
					Call(MatPermKerName, LOC_LOOP,
						Bindings(7,
							K_Arg("In", KER_ARG_TILE),			/* Input tile */
							K_Arg("Out", KER_ARG_TILE),			/* Output tile */
							K_ArgPar("In", KER_ARG_PARTILE_SIZE, D2),	/* Number of Channels */
							K_Arg("In", KER_ARG_TILE_W),			/* Input tile width */
							K_Arg("In", KER_ARG_TILE_H),			/* Input tile height */
							AT_IGNORE_ARG_BINDING,				/* StrideX */
							AT_IGNORE_ARG_BINDING				/* StrideY */
						)
					)
				),
				PKerArgs
		);
	}
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Feat, Height, Width, 1);
		switch (MatPermOper) {
			case KOP_MATPERM_CHW2CWH:
				AddKernelArgDim(Name, "Out", 4, Feat, Width, Height, 1);
				break;
			case KOP_MATPERM_CHW2HWC:
				AddKernelArgDim(Name, "Out", 4, Height, Width, Feat, 1);
				break;
			case KOP_MATPERM_CHW2WHC:
				AddKernelArgDim(Name, "Out", 4, Width, Height, Feat, 1);
				break;
			case KOP_MATPERM_CHW2WCH:
				AddKernelArgDim(Name, "Out", 4, Width, Feat, Height, 1);
				break;
			case KOP_MATPERM_CHW2HCW:
				AddKernelArgDim(Name, "Out", 4, Height, Feat, Width, 1);
				break;
		}
		AT_PrepareForTest_SQ8(Name, Feat,Feat,Width,Height, 1, MatPermOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, 0);
	}
	return (Kernel!=0);
}
