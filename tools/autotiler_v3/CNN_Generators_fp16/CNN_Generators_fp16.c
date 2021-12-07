#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators_fp16.h"
#include "CNN_Generator_Util.h"
#include "CNN_Copy_Generators.h"
#include "Gap.h"

void LoadCNNLibrary_fp16()

{
	LoadCNN_Copy_Library();
	/* Templates for Features and coefficients on 16 bits */
	LibKernelTemplate("KerSetBias_fp16_T",
                  CArgs(5,
			TCArg("F16 * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("F16 * __restrict__", "Bias")
			)
	);
	LibKernelTemplate("KerConv_fp16_T",
                  CArgs(19,
			TCArg("F16 * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int", "TotalInFeatures"),
			TCArg("F16 * __restrict__", "Filter"),
			TCArg("F16 * __restrict__", "Bias"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
                        TCArg("unsigned char", "Orientation"),
                        TCArg("unsigned char", "N"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "Ny"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
			)
	);
        LibKernelTemplate("Ker_MM_Conv_fp16_T",
                CArgs(19,
                        TCArg("F16 * __restrict__", "In"),
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
                        TCArg("F16 * __restrict__", "Filter"),
                        TCArg("F16 * __restrict__", "Bias"),
                        TCArg("F16 * __restrict__", "Out"),
                        TCArg("unsigned short int", "InFeat"),
                        TCArg("unsigned short int", "OutFeat"),
                        TCArg("unsigned short int", "Wo"),
                        TCArg("unsigned short int", "Ho"),
                        TCArg("F16 * __restrict__", "ColBuff")
                )
        );
	LibKernelTemplate("KerPool_fp16_T",
                  CArgs(16,
			TCArg("F16 * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned char", "M"),
			TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "My"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy"),
			TCArg("unsigned char", "Oper"),
			TCArg("unsigned char", "Orientation")
			)
	);
        LibKernelTemplate("Ker_MM_Pool_fp16_T",
                CArgs(13,
                        TCArg("F16 * __restrict__", "In"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned char", "Fx"),
                        TCArg("unsigned char", "Fy"),
                        TCArg("unsigned char", "Sx"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "FirstTile"),
                        TCArg("v4s", "Pad"),
                        TCArg("F16 * __restrict__", "Out"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("unsigned short int", "Wo"),
                        TCArg("unsigned short int", "Ho")
                )
        );
	LibKernelTemplate("KerGlobalPool_fp16_T",
                  CArgs(6,
			TCArg("F16 * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("F16 * __restrict__", "Out"),
                        TCArg("unsigned int", "TileIndex")
			)
	);
	LibKernelTemplate("KerLinear_fp16_T",
                  CArgs(9,
			TCArg("F16 * __restrict__", "In"),
			TCArg("F16 * __restrict__", "Filter"),
			TCArg("F16 * __restrict__", "Bias"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("unsigned short int", "InDim"),
			TCArg("unsigned short int", "TotalInDim"),
			TCArg("unsigned short int", "OutDim"),
                        TCArg("F16", "UB"),
                        TCArg("unsigned short int", "TileIndex")
			)
	);
        LibKernelTemplate("KerActivation_fp16_T",
                  CArgs(6,
                        TCArg("F16 *__restrict__", "In"),
                        TCArg("F16 *__restrict__", "Out"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("F16", "UB")
                       )
        );
	LibKernelTemplate("KerMat3_fp16_T",
                  CArgs(6,
			TCArg("F16 *__restrict__", "In1"),
			TCArg("F16 *__restrict__", "In2"),
			TCArg("F16 *__restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "N")
			)
	);
	LibKernelTemplate("KerMatMul_fp16_T",
                  CArgs(16,
			TCArg("F16 * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("F16 * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("F16 * __restrict__", "Bias"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned int", "OutFirstCol"),
			TCArg("F16 * __restrict__", "BufferColIn2"),
			TCArg("unsigned char", "ColFirst"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
                        TCArg("F16", "UB")
			)
	);
	LibKernelTemplate("KerSoftMax_fp16_T",
                  CArgs(4,
			TCArg("F16 *__restrict__", "In"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "N"),
			TCArg("F16 *__restrict__", "Out")
			)
	);

	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 16 bits. Kernels for multiple output features evaluated in parallel */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerParSetBias_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerSetBias_fp16_T",              	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));

	/* Regular Convolutions */
        LibKernel("KerParConv1x1Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,3,1,1,1,2));
        
	LibKernel("KerParConv3x3Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,-1,-2));

        LibKernel("KerParConv5x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerParConvDW1x1Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,3,1,1,1,2));
        
	LibKernel("KerParConvDW3x3Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));

	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerParPool2x2Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1,
													  		CNN_Type(2,0,0,0,2), 2,2,1,1,2,2));
        LibKernel("KerParPoolNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1,
													  		CNN_Type(2,0,0,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1,
													  		CNN_Type(2,0,0,0,2), -1,-1,1,1,-1,-1));
	/* Global Pooling (Max or Avg) */
        LibKernel("KerParGlobalMaxPool_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerGlobalPool_fp16_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_MAXPOOL), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParGlobalAvgPool_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerGlobalPool_fp16_T",		CNN_Match(CNN_OperList(1, KOP_GLOBAL_AVGPOOL), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));

	/* Activations */
        LibKernel("KerParReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",			CNN_Match(CNN_OperList(1, KOP_RELU), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",			CNN_Match(CNN_OperList(1, KOP_RELUN), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",			CNN_Match(CNN_OperList(1, KOP_SWISH), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",                 CNN_Match(CNN_OperList(1, KOP_HSWISH), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",		CNN_Match(CNN_OperList(1, KOP_SIGMOID), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",               CNN_Match(CNN_OperList(1, KOP_HSIGMOID), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",                   CNN_Match(CNN_OperList(1, KOP_TANH), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",                  CNN_Match(CNN_OperList(1, KOP_HTANH), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));
        LibKernel("KerParLeakyReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerActivation_fp16_T",		CNN_Match(CNN_OperList(1, KOP_LEAKYRELU), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));

	/* Linear layer followed by an optional activation, don't use when partial evaluation of the output is needed */
	LibKernel("KerParLinearLayer_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_LINEAR), 0, 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELU), 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_RELUN), 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 		CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_SWISH), 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
        LibKernel("KerParLinearLayerHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T",          CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_HSWISH), 1,
                                                                                                                        CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_SIGMOID), 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
        LibKernel("KerParLinearLayerHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T",        CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_HSIGMOID), 1,
                                                                                                                        CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
        LibKernel("KerParLinearLayerTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T",            CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_TANH), 1,
                                                                                                                        CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
        LibKernel("KerParLinearLayerHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T",           CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_HTANH), 1,
                                                                                                                        CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParLinearLayerLeakyReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_LEAKYRELU), 1,
													  		CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));

	/* Matrix Algebra */

	/* Matrix Addition */
	LibKernel("KerParMatAdd_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMat3_fp16_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), 0, 1, CNN_Type(2,2,0,0,2), 0,0,0,0,0,0));

	/* Matrix Addition with post ReLU or ReLUN */
	LibKernel("KerParMatAddReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMat3_fp16_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,0,0,2), 0,0,0,0,0,0));

	/* Matrix multiplication */
	LibKernel("KerParMatMul_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0,
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBias_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), 0,
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposed_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), 0,
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBias_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), 0,
													  		1, CNN_Type(2,2,0,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0,
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeat_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), 0, 1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

	/* Matrix multiplication with ReLU reduction */
	LibKernel("KerParMatMulReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBiasReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBiasReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulReLUSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeatReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

	/* Matrix multiplication with ReLUN reduction */
	LibKernel("KerParMatMulReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBiasReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBiasReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulReLUNSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeatReLUN_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_RELUN),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

	/* Matrix multiplication with Swish reduction */
	LibKernel("KerParMatMulSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBiasSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBiasSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSwishSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeatSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_SWISH),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

	/* Matrix multiplication with Sigmoid reduction */
	LibKernel("KerParMatMulSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBiasSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBiasSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSigmoidSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeatSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_SIGMOID),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

        /* Matrix multiplication with Tanh reduction */
        LibKernel("KerParMatMulTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",                 CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulNoBiasTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",           CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",       CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedNoBiasTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T", CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTanhSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",             CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
        LibKernel("KerParMatMulSmallFeatTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",        CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_TANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

        /* Matrix multiplication with H Swish reduction */
        LibKernel("KerParMatMulHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",               CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulNoBiasHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",         CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",     CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedNoBiasHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulHSwishSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",           CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
        LibKernel("KerParMatMulSmallFeatHSwish_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",      CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_HSWISH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

        /* Matrix multiplication with H Sigmoid reduction */
        LibKernel("KerParMatMulHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",             CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulNoBiasHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",       CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",   CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedNoBiasHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulHSigmoidSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",         CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
        LibKernel("KerParMatMulSmallFeatHSigmoid_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",    CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_HSIGMOID),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

        /* Matrix multiplication with H Tanh reduction */
        LibKernel("KerParMatMulHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",              	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulNoBiasHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",          CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",      CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulTransposedNoBiasHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
        LibKernel("KerParMatMulHTanhSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",          	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
        LibKernel("KerParMatMulSmallFeatHTanh_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",     	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_HTANH),
                                                                                                                        1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));

	/* Matrix multiplication with Leaky ReLU reduction */
	LibKernel("KerParMatMulLeakyrelu_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulNoBiasLeakyrelu_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedLeakyrelu_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_TRANSPOSED), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulTransposedNoBiasLeakyrelu_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",CNN_Match(CNN_OperList(1, KOP_MATMUL_NOBIAS_TRANSPOSED), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulLeakyreluSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMulSmallFeatLeakyrelu_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerMatMul_fp16_T",	CNN_Match(CNN_OperList(1, KOP_MATMUL_SM1), CNN_OperList(1, KOP_LEAKYRELU),
													  		1, CNN_Type(2,2,2,0,2), 0,0,0,0,1,1));
	/* SoftMax */
	LibKernel("KerParSoftMax_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerSoftMax_fp16_T",		CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, -1, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));


	/****************************************************************************************************************/
	/* Kernels for Features and coefficients on 16 bits. Kernels for a single feature evaluated in parallel         */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerSetBias_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerSetBias_fp16_T",		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,0,2), 0,0,0,0,0,0));

	/* Convolutions */
        LibKernel("KerConv1x1Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,-1,-2));

        LibKernel("KerConvNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerConvDW1x1Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), 5,5,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerConv_fp16_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));

	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerPool2x2Stride2_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0,
													  CNN_Type(2,0,0,0,2), 2,2,1,1,2,2));
        LibKernel("KerPoolNxNStrideS_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0,
													  CNN_Type(2,0,0,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerPoolNxMStrideSxSy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerPool_fp16_T",	CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0,
													  CNN_Type(2,0,0,0,2), -1,-1,1,1,-1,-1));

	/* Linear layer followed by an optional activation */
	LibKernel("KerLinearLayerReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "KerLinear_fp16_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(3, KOP_RELU, KOP_RELUN, KOP_NONE), 0,
													  CNN_Type(2,2,2,0,2), 0,0,0,0,0,0));

        /* Mat Mul based convolutions */

        /* CHW In and Out tensors, [OutFeat,InFeat,Fy,Fx] weights */
        LibKernel("KerPar_MM_Conv1D_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv1D_ReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,2,0,2), -1,1,1,1,-1,-1));

        LibKernel("KerPar_MM_Conv1D_DxDy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,1,-1,-1,-1,-1));
        LibKernel("KerPar_MM_Conv1D_DxDy_ReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,2,0,2), -1,1,-1,-1,-1,-1));

        LibKernel("KerPar_MM_Conv2D_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv2D_ReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv2D_DxDy_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));
        LibKernel("KerPar_MM_Conv2D_DxDy_ReLU_fp16", CALL_PARALLEL|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));


        /* HWC In and Out tensors, [OutFeat,Fy,Fx,InFeat] weights */
        LibKernel("KerPar_MM_Conv1x1_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), 1,1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv1D_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv1D_DxDy_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,1,-1,-1,-1,-1));
        LibKernel("KerPar_MM_Conv2D_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));
        LibKernel("KerPar_MM_Conv2D_DxDy_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 1, CNN_Type(2,2,2,0,2), -1,-1,-1,-1,-1,-1));

        LibKernel("Ker_MM_Conv2D_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Conv_fp16_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MM_CONV), CNN_OperList(1, KOP_NONE), 0, CNN_Type(2,2,2,0,2), -1,-1,1,1,-1,-1));

        /* HWC Pooling */
        LibKernel("KerParMaxPoolNxMStrideSxSy_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Pool_fp16_T",
                                                                                                CNN_Match(CNN_OperList(1, KOP_MAXPOOL), CNN_OperList(1, KOP_NONE), 1,
                                                                                                          CNN_Type(2,0,0,0,2), -1,-1,1,1,-1,-1));
        LibKernel("KerParAvgPoolNxMStrideSxSy_HWC_fp16", CALL_PARALLEL|CALL_HWC_KER|CALL_FLOAT_KER, 0, "Ker_MM_Pool_fp16_T",
                                                                                                CNN_Match(CNN_OperList(1, KOP_AVGPOOL), CNN_OperList(1, KOP_NONE), 1,
                                                                                                          CNN_Type(2,0,0,0,2), -1,-1,1,1,-1,-1));

}

/*********************************************************************************************************************************************************************
 	Generator for Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional Activation. Imm2Col based

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use HWCE), Def=(TILE_HOR, 1, 0)

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

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_MM_ConvolutionPoolAct_fp16

*********************************************************************************************************************************************************************/

int CNN_MM_ConvolutionPoolAct_fp16(
	char         *Name,

	CNN_GenControl_T *Ctrl,

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
			return CNN_PoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_fp16(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, All requested operations are KOP_NONE", Name);
	}

	int ParFeat = 1, HWC = 0, ParFeatConv = 2;
        float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	Tile_Orientation_T TileOrientation = Height>1?TILE_HOR:TILE_VER;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC) HWC = 1;
		if (Ctrl->ParallelFeatures != -1) ParFeatConv = Ctrl->ParallelFeatures;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
	}
        if (ParFeatConv == 2 && HWC && Fcy>1 && (InFeat < 8))
                ParFeatConv = 0;
        else
                ParFeatConv = 1;
	unsigned int Bs = 2;
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
	KernelOper_T KernelOper = CNN_CompositeKernel(ConvOper, PoolOper, ActOper);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int Log=1;

	if (ConvOper != KOP_CONV)
		GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, ConvOper, expecting KOP_CONV", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

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
	int KerLayout = CALL_FLOAT_KER|(HWC?CALL_HWC_KER:0);

	int BuffS = ALIGN(InFeat*Fcx*Fcy, 2);
	if (HWC) {
		//if (Fcx==1&&Fcy==1&&Scx==1&&Scy==1) return CNN_MatMulAct_fp16(Name, 0, InFeat, Width*Height, OutFeat, InFeat, 0, 0, 1, 1, KOP_MATMUL_TRANSPOSED, ActOper); 
		if (Fcx==1&&Fcy==1) BuffS = 1;
		else if (ParFeatConv) BuffS = ALIGN(InFeat*Fcx*Fcy, 2);
		else BuffS = 2 * InFeat*Fcx*Fcy*8;
	}

	/* Layer number of operations and memory bandwidth requirements */
	LayerOp += (int64_t) Wc*Hc*Fcx*Fcy*OutFeat*InFeat;
	if (PoolOper) LayerOp += (int64_t) OutFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) OutFeat*Wo*Ho;
	LayerBandwidth += (int64_t) Width*Height*2*InFeat*OutFeat;
	LayerBandwidth += (int64_t) Wo*Ho*2*OutFeat;
	LayerBandwidth += (int64_t) Fcx*Fcy*2*InFeat*OutFeat;
	LayerBandwidth += (int64_t) 2*OutFeat;

	ConvKerName = CNN_FindMatchingKernelAttr(KOP_MM_CONV, (PoolOper==KOP_NONE)?ActOper:KOP_NONE, ParFeatConv, KerLayout, 2, 2, 2, 0, 2, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
				     	     	 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName) {
		if (PoolOper==KOP_NONE) StandAloneAct = 0;
	} else ConvKerName = CNN_FindMatchingKernelAttr(KOP_MM_CONV, KOP_NONE, ParFeatConv, KerLayout, 2, 2, 2, 0, 2, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
	 				            	 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Convolution basic kernel", Name);

	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) 
			PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
							     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		else if (ActOper) StandAloneAct = 0;
		else if (PoolOper==KOP_AVGPOOL && ActOper==KOP_NONE && HWC) {
			StandAloneAct = 1; ActOper = KOP_SCALE;
		}
		if (PoolKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
	}
	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MM_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Activation basic kernel", Name);
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
	UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = UserKernel(Name,
		TileFirst?
		KernelIterSpace(3, IterTiledSpace(T0), IterParSpace(D1, OutFeat, OutTileCons), IterParSpace(D0|SPACE_PROP_ONE_TILE, InFeat, InFeat)):
		KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0|SPACE_PROP_ONE_TILE, InFeat, InFeat)),
                TileOrientation,
                CArgs(4,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Filter"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Bias"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                ),
		Calls(3,
			Call(ConvKerName, LOC_D0,
				Bindings(19,
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
					Imm(InFeat),								/* In Features */
					K_ArgPar(PoolKerName?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, D1),	/* Out Features */
					K_Arg(PoolKerName?"ConvOut":"Out", KER_ARG_TILE_W),
					K_Arg(PoolKerName?"ConvOut":"Out", KER_ARG_TILE_H),
					K_Arg("KerBuff", KER_ARG_TILE)
				)
			),
			(PoolKerName==0)?AT_NO_CALL:
			Call(PoolKerName, LOC_D0_EPILOG,
				(HWC==0)?
				Bindings(16,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Pooling input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Pooling input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_USEDW),					/* Pooling input tile width, used part of it */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Pooling input tile height */
					K_Arg("ConvOut", KER_ARG_TILE_USEDH),					/* Pooling input tile height, used part of it */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D1),				/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pooling Pad */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,					/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,					/* Pooling Dy */
					Imm((PoolOper==KOP_MAXPOOL)?1:0),					/* PoolMax or PoolAverage */
					AT_IGNORE_ARG_BINDING							/* Pooling Orientation when feature parallel */
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
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D1),				/* In Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Output tile width */
					K_Arg("Out", KER_ARG_TILE_H)						/* Output tile height */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_D0_EPILOG,
				Bindings(6,
					K_Arg("Out", KER_ARG_TILE),						/* Input tile */
					K_Arg("Out", KER_ARG_TILE),						/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D1),				/* Number of Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),						/* Input tile height */
					BindKExpr("UB")								/* ReLUN upper bound */
				)
			)
		),
		(HWC==0)?
		KerArgs(6,
			KerArg ("KerBuff",KerArgSpace(1,T0),    O_BUFF|O_NTILED, BuffS,      1,                             	 	2,          0, 0,        0, 0),
			KerArgP("In",     KerArgSpace(2,D0,T0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 2,   OverlapC, 0, TileCons, "In"),
			KerArg ("Bias",   KerArgSpace(1,D1),    O_IN|O_DB|O_CONST,   1,      1,                                         2,          0, 0,        0, "Bias"),
			KerArg ("Filter", KerArgSpace(2,D1,D0), O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         2,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,D1,T0), O_OUT|O_DB,          Wo,    Ho,                                         2,          0, 0,        0, "Out"),
			(PoolKerName==0)?AT_NO_KER_ARG:
			KerArgP("ConvOut",KerArgSpace(2,D1,T0), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        2,   OverlapP, 0,        0, "")
		):
		KerArgs(6,
			KerArg ("KerBuff",KerArgSpace(1,T0),    O_BUFF|O_NTILED, BuffS,      1,                             	 	2,          0, 0,        0, 0),
			KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 2,   OverlapC, 0, TileCons, "In"),
			KerArg ("Bias",   KerArgSpace(1,D1),    O_IN|O_DB|O_CONST,   1,      1,                                         2,          0, 0,        0, "Bias"),
			KerArg ("Filter", KerArgSpace(2,D1,D0), O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         2,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,T0,D1), O_OUT|O_DB,          Wo,    Ho,                                         2,          0, 0,        0, "Out"),
			(PoolKerName==0)?AT_NO_KER_ARG:
			KerArgP("ConvOut",KerArgSpace(2,T0,D1), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        2,   OverlapP, 0,        0, "")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
                AddKernelFloatArgDim(Name, "In", 4, Height, Width, InFeat, 2);
                AddKernelFloatArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, 2);
                AddKernelFloatArgDim(Name, "Bias", 2, OutFeat, 2);
                AddKernelFloatArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 2);

		if (Ctrl && (Ctrl->Out_L3)) SetKerArgInL3(Name, "Out");
                AT_PrepareForTest(Name,
                                  (v4s) {2, 2, 2, 2},
                                  (v4s) {0,0,0,0},
                                  InFeat, OutFeat, Width, Height,
                                  Fcx, Fcy, Scx, Scy, Dcx, Dcy, PadInc, Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp, KernelOper,
                                  0, 0);
	}
	return (Kernel!=0);
}


int CNN_HWC_DWConvolutionPoolAct_fp16(
	char         *Name,

	CNN_GenControl_T *Ctrl,

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
			return CNN_PoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
		else if (ActOper!=KOP_NONE)
			return CNN_Act_fp16(Name, Ctrl, InFeat, Width, Height, ActOper);
		else GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, All requested operations are KOP_NONE", Name);
	}

	int ParFeat = 1, HWC = 1;
        float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	Tile_Orientation_T TileOrientation = TILE_HOR;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
	}
	unsigned int Bs = 2;
	KernelOper_T COper = ConvOper;
	int OverlapC, OverlapP;
	int TileCons;
	int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
	char *MatPermInKerName=0, *MatPermOutKerName=0, *ConvKerName=0, *DPReductionKerName=0, *PoolKerName=0, *ActKerName=0;
	int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
	int UsedWidth, UsedHeight, UsedWc, UsedHc;
	int StandAloneAct = (ActOper!=KOP_NONE);
	KernelOper_T KernelOper = CNN_CompositeKernel(ConvOper, PoolOper, ActOper);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int Log=1;

	if (ConvOper != KOP_CONV_DW)
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, ConvOper, expecting KOP_CONV", Name);
	if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);
        if (InFeat != OutFeat) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

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
	int KerLayout = CALL_FLOAT_KER|(HWC?CALL_HWC_KER:0);

	int BuffS = InFeat*Fcx*Fcy;

	/* Layer number of operations and memory bandwidth requirements */
	LayerOp += (int64_t) Wc*Hc*Fcx*Fcy*InFeat;
	if (PoolOper) LayerOp += (int64_t) InFeat*Wo*Ho*Fpx*Fpy;
	if (ActOper) LayerOp += (int64_t) InFeat*Wo*Ho;
	LayerBandwidth += (int64_t) Width*Height*2*InFeat;
	LayerBandwidth += (int64_t) Wo*Ho*2*InFeat;
	LayerBandwidth += (int64_t) Fcx*Fcy*2*InFeat;
	LayerBandwidth += (int64_t) 2*InFeat;

	ConvKerName = CNN_FindMatchingKernelAttr(KOP_CONV_DW, (PoolOper==KOP_NONE)?ActOper:KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 2, 2, 0, 2, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
				     	         &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName) {
		if (PoolOper==KOP_NONE) StandAloneAct = 0;
	} else ConvKerName = CNN_FindMatchingKernelAttr(KOP_CONV_DW, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 2, 2, 0, 2, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
	 				                &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, Can't find a matching Convolution basic kernel", Name);

	MatPermInKerName = CNN_FindMatchingKernelAttr(KOP_MATPERM_HWC2CHW, KOP_NONE, ParFeat, (HWC?CALL_HWC_KER:0), 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
	if (MatPermInKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching basic kernel for 3D Tensor Permutation In", Name);

        MatPermOutKerName = CNN_FindMatchingKernelAttr(KOP_MATPERM_CHW2HWC, KOP_NONE, ParFeat, 0, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
        if (MatPermOutKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching basic kernel for 3D Tensor Permutation Out", Name);

	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) 
			PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
							     	 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		else if (ActOper) StandAloneAct = 0;
		if (PoolKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
	}
	if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_HWC_DWConvolutionPoolAct_fp16: %s, Can't find a matching Activation basic kernel", Name);
	}

	if (Log) {
		printf("InFeat: %d, OutFeat: %d%s\n", InFeat, OutFeat, HWC?", HWC":", CHW");
        	printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
        	printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
        	printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
        	printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
        	printf("OverlapC: %d\n", OverlapC);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
		if (MatPermInKerName) printf("%20s: %s\n", "MatPermInKerName", MatPermInKerName);
                if (MatPermOutKerName) printf("%20s: %s\n", "MatPermOutKerName", MatPermOutKerName);
		if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
		
	}
	int OutTileCons = 8;
	UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T0), IterParSpace(D0, InFeat, 8)),
                TILE_HOR,
                CArgs(4,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Filter"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Bias"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                ),
		Calls(5,
			Call(MatPermInKerName, LOC_D0,
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
                                Bindings(19, 
					K_Arg("PermOut", KER_ARG_TILE),						/* Conv input tile */
					K_Arg("In", KER_ARG_TILE_W),						/* Conv input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),					/* Conv input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),						/* Conv input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),					/* Conv input tile height, used part of it */
					K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),				/* Number of input features in this tile */
					K_ArgPar(PoolKerName?"ConvOut":"In", KER_ARG_PARTILE_SIZE, D0),	/* Number of output features in this tile */
					K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),			/* Total number of input features currently in L1 memory, argument promotion */
					K_Arg("Filter", KER_ARG_TILE),						/* Conv filter */
					K_Arg("Bias", KER_ARG_TILE),						/* Conv Bias when depth wise conv*/
					K_Arg(PoolKerName?"ConvOut":"In", KER_ARG_TILE),			/* Conv output */
					K_Arg("In", KER_ARG_TILE_PAD),						/* Conv Padding */
					AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,					/* Conv Fx */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,					/* Conv Stridex */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,					/* Conv Dx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,					/* Conv Fy */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,					/* Conv Stridey */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING					/* Conv Dy */
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
					K_Arg("In", KER_ARG_TILE),						/* Pooling output tile */
					K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D0),				/* In Features */
                                        K_Arg("Out", KER_ARG_TILE_W),                                           /* Output tile width */
                                        K_Arg("Out", KER_ARG_TILE_H)                                            /* Output tile height */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_D0,
				Bindings(6,
					K_Arg("In", KER_ARG_TILE),						/* Input tile */
					K_Arg("In", KER_ARG_TILE),						/* Output tile */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),				/* Number of Features */
					K_Arg("Out", KER_ARG_TILE_W),						/* Input tile width */
					K_Arg("Out", KER_ARG_TILE_H),						/* Input tile height */
					BindKExpr("UB")								/* ReLUN upper bound */
				)
			),
                        Call(MatPermOutKerName, LOC_D0,
                                Bindings(7,
                                        K_Arg("In", KER_ARG_TILE),                                         /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                             /* Output tile */
                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),                               /* Out Features */
                                        K_Arg("Out", KER_ARG_TILE_W),                                            /* Input tile width */
                                        K_Arg("Out", KER_ARG_TILE_H),                                            /* Input tile height */
                                        AT_IGNORE_ARG_BINDING,                                                  /* StrideX */
                                        AT_IGNORE_ARG_BINDING                                                   /* StrideY */
                                )
                        )
		),
		KerArgs(6,
			KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,       Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 2,   OverlapC, 0, TileCons, "In"),
			KerArgP("PermOut",KerArgSpace(2,D0,T0), O_BUFF|O_ONETILE,Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 2,   OverlapC, 0,        0, ""),
			KerArg ("Bias",   KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,   1,      1,                                         2,          0, 0,        0, "Bias"),
			KerArg ("Filter", KerArgSpace(1,D0),    O_IN|O_DB|O_CONST,  Fcx,   Fcy,                                         2,          0, 0,        0, "Filter"),
			KerArg ("Out",    KerArgSpace(2,T0,D0), O_OUT|O_DB,          Wo,    Ho,                                         2,          0, 0,        0, "Out"),
			KerArgP("ConvOut",KerArgSpace(2,T0,D0), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        2,   OverlapP, 0,        0, "")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
                AddKernelFloatArgDim(Name, "In", 4, Height, Width, InFeat, 2);
                AddKernelFloatArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 2);
                AddKernelFloatArgDim(Name, "Bias", 2, InFeat, 2);
                AddKernelFloatArgDim(Name, "Out", 4, Ho, Wo, InFeat, 2);

		if (Ctrl && (Ctrl->Out_L3)) SetKerArgInL3(Name, "Out");
                AT_PrepareForTest(Name,
                                  (v4s) {2, 2, 2, 2},
                                  (v4s) {0,0,0,0},
                                  InFeat, OutFeat, Width, Height,
                                  Fcx, Fcy, Scx, Scy, Dcx, Dcy, PadInc, Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp, KernelOper,
                                  0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, Regular convolution with double precision output: KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:        Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out)

	CNN_ConvolutionPoolAct_fp16
	
*********************************************************************************************************************************************************************/

int CNN_ConvolutionPoolAct_fp16(
			char         *Name,

			CNN_GenControl_T *Ctrl,

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
        float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
        Tile_Orientation_T TileOrientation = TILE_HOR;
        AT_PadType PadType = PAD_BALANCED_LEFT;

        if (Ctrl) {
                if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
                if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
                if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
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
                        return CNN_PoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
                else if (ActOper!=KOP_NONE)
                        return CNN_Act_fp16(Name, Ctrl, InFeat, Width, Height, ActOper);
                else GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, All requested operations are KOP_NONE", Name);
        } else if (HWC) {
                if (ConvOper == KOP_CONV_DW)
                        return CNN_HWC_DWConvolutionPoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height,
                                                                 ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
                                                                 PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
                else
                        return CNN_MM_ConvolutionPoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height,
                                                              ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
                                                              PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
        } else if (ConvOper==KOP_CONV && ((Fcx > 1 && Fcy == 1))) {
                AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
                int Ok = CNN_MM_ConvolutionPoolAct_fp16(Name, Ctrl, InFeat, OutFeat, Width, Height,
                                                        ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
                                                        PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper);
                AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
                if (Ok) return Ok;
                if (Log) printf("Mapping this convolution to im2col scheme failed, reverting to standard implementation\n");
        }


        if (PoolOper==KOP_NONE) {
                Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
        }
	int F = O_FLOAT;
        int OverlapC, OverlapP;
        int TileCons;
        int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
        char *ConvKerName=0, *PoolKerName=0, *ActKerName=0, *SetBiasKerName=0;
        int DWConv=(ConvOper==KOP_CONV_DW);
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int Os=(DWConv?D0:D1);
        int UsedWidth, UsedHeight, UsedWc, UsedHc;
        int InTileCons = 4;
        int StandAloneAct = (ActOper!=KOP_NONE);
	KernelOper_T KernelOper = CNN_CompositeKernel(ConvOper, PoolOper, ActOper);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        if (!(ConvOper == KOP_NONE || ConvOper == KOP_CONV || ConvOper == KOP_CONV_DW))
                GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
        if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

        if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

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
        LayerBandwidth += Width*Height*2*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Wo*Ho*2*OutFeat;
        LayerBandwidth += Fcx*Fcy*2*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += 2*OutFeat;

        /* Basic Kernel Matching */
        if (!DWConv) {
                SetBiasKerName = CNN_FindMatchingKernelAttr(KOP_SETBIAS, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Set Bias basic kernel", Name);
        }

        ConvKerName = CNN_FindMatchingKernelAttr(ConvOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 2, DWConv?2:0, 0, 2, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
                                             &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
        if (ConvKerName==0) GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Convolution basic kernel", Name);

        if (PoolOper!=KOP_NONE) {
                PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
                                                     &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
                if (PoolKerName==0)
                        PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
                                                             &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
                else if (ActOper) StandAloneAct = 0;
                if (PoolKerName==0) GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ActOper?"with linear rectification":"");
        }
        if (ActOper && StandAloneAct) {
                ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (ActKerName==0) GenTilingError("CNN_ConvolutionPoolAct_fp16 Kernel: %s, Can't find a matching Activation basic kernel", Name);
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
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
                printf("Nb Oper : %lld\n", LayerOp);

        }
        if (Ctrl && (Ctrl->EnableIm2Col==1) && (ConvOper==KOP_CONV) && (PoolOper==KOP_NONE) && (Fcx==1) && (Fcy==1) && (Dcx==1) && (Dcy==1)) {
                AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
                if ((InFeat+OutFeat)<100) {
                        if (Log) printf("Mapping this convolution to matrix multiplication with small first operand\n");
                        int Ok = CNN_MatMulSmallM1Act_fp16(Name, 0, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL_SM1, ActOper);
                        if (!Ok&&Log) printf("Mapping this convolution to matrix multiplication with small first operand FAILED, trying with standard mult implementation\n");
                        if (Ok) return Ok;
                }
                if (Log) printf("Mapping this convolution to matrix multiplication\n");
                int Ok = CNN_MatMulAct_fp16(Name, 0, InFeat, OutFeat, Width*Height, InFeat, Width, Height, Scx, Scy, KOP_MATMUL, ActOper);
                AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
                if (Ok) return Ok;
                if (Log) printf("Mapping this convolution to matrix multiplication FAILED, reverting to standard implementation\n");
        }

	/* User kernel C arguments */
	CKernel_Arg_T **KCArgs = AllocateCArgs(4);
	int Ca=0;

	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),     "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1), "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),   "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),    "Out");

	/* User kernel kernel arguments */
	Object_T **KArgs = AllocateKerArgs(4+(PoolOper!=0));
	int Ka=0;

	KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,D0,T0), F|O_IN|O_DB,         Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 2, 	   OverlapC, 0, TileCons, "In");
	KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),    F|O_IN|O_DB|O_CONST, 1,     1,                       		 	2,         0,        0,        0, "Bias");
	if (DWConv) 
	KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os),    F|O_IN|O_DB|O_CONST, 1,     1,                       			Fcx*Fcy*2, 0,        0,        0, "Filter");
	else 
	KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0), F|O_IN|O_DB|O_CONST, 1,     1,                       			Fcx*Fcy*2, 0,        0,        0, "Filter");
       	KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,Os,T0), F|O_OUT|O_DB,        Wo,    Ho,                      			2,         0,        0,        0, "Out");

	if (PoolOper) {
		if (ParFeat) {
	KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(2,Os,T0), F|O_BUFF|O_ONETILE,  Wc,    Hc,     UsedWc, UsedHc, PadInp, PadInp, 	2,         OverlapP, 0,        0,  "");
		} else {
	KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(1,T0),    F|O_BUFF|O_ONETILE,  Wc,    Hc,     UsedWc, UsedHc, PadInp, PadInp, 	2,         OverlapP, 0,        0,  "");
		}
	}

        UserSymbols(1, US_Float("UB", UB));
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
                Calls(4,
			(SetBiasKerName==0)?AT_NO_CALL:
                        Call(SetBiasKerName, LOC_D0_PROLOG,
                                Bindings(5,
					K_Arg((PoolOper)?"ConvOut":"Out", KER_ARG_TILE),				/* SetBias output tile */
					K_Arg((PoolOper)?"ConvOut":"Out", KER_ARG_TILE_W),				/* SetBias output tile width */
                                        K_Arg((PoolOper)?"ConvOut":"Out", KER_ARG_TILE_H),				/* SetBias output tile height */
					ParFeat?K_ArgPar((PoolOper)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):Imm(1),	/* Number of output features in this tile */
					K_Arg("Bias", KER_ARG_TILE)							/* SetBias Bias tile */
					)),
                        Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
                                Bindings(19, 
					K_Arg("In", KER_ARG_TILE),							/* Conv input tile */
					K_Arg("In", KER_ARG_TILE_W),							/* Conv input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),						/* Conv input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),							/* Conv input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),						/* Conv input tile height, used part of it */
					K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),					/* Number of input features in this tile */
					ParFeat?K_ArgPar((PoolOper)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):Imm(1),  /* Number of output features in this tile */
					K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),                     	/* Total number of input features currently in L1 memory, argument promotion */
					K_Arg("Filter", KER_ARG_TILE),							/* Conv filter */
					DWConv?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,			/* Conv Bias when Conv depth wise */
					K_Arg((PoolOper)?"ConvOut":"Out", KER_ARG_TILE),				/* Conv output */
					K_Arg("In", KER_ARG_TILE_PAD),							/* Conv Padding */
					ParFeat?AT_IGNORE_ARG_BINDING:Imm((TileOrientation==TILE_HOR)?1:0),     	/* Orientation when feature parallel */
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,						/* Conv Fx */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,						/* Conv Stridex */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,						/* Conv Dx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,						/* Conv Fy */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,						/* Conv Stridey */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING						/* Conv Dy */
					)
			),
			(PoolKerName==0)?AT_NO_CALL:
			Call(PoolKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(16,
					K_Arg("ConvOut", KER_ARG_TILE),							/* Pooling input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),						/* Pooling input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_USEDW),						/* Pooling input tile width, used part of it */
					K_Arg("ConvOut", KER_ARG_TILE_H),						/* Pooling input tile height */
					K_Arg("ConvOut", KER_ARG_TILE_USEDH),						/* Pooling input tile height, used part of it */
					ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),			/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),							/* Pooling output tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),						/* Pooling Pad */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,						/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,						/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,						/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,						/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,						/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,						/* Pooling Dy */
					Imm(CNN_EncodePoolOperation(PoolOper, ActOper)),				/* MaxPool or AvgPool, with optional ReLU */
					Imm((TileOrientation==TILE_HOR)?1:0)						/* Pooling Orientation */
				)
			),
                        (ActKerName==0)?AT_NO_CALL:
                        Call(ActKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
                                Bindings(6,
                                        K_Arg("Out", KER_ARG_TILE),                                          		/* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                        		/* Output tile */
                                        ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, Os):Imm(1),         		/* Number of features in this tile */
                                        K_Arg("Out", KER_ARG_TILE_W),                                         		/* Tile width */
                                        K_Arg("Out", KER_ARG_TILE_H),                                          		/* Tile height */
					BindKExpr("UB")									/* Act = RelUN, upper bound */
                                )
                        )
                     ),
		KArgs
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 4, InFeat, Height, Width, 2);
		if (DWConv) AddKernelFloatArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 2); else AddKernelFloatArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, 2);
		AddKernelFloatArgDim(Name, "Bias", 2, OutFeat, 2);
		AddKernelFloatArgDim(Name, "Out", 4, OutFeat, Ho, Wo, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 2, 2, 2},
				  (v4s) {0,0,0,0},
				  InFeat, OutFeat, Width, Height,
				  Fcx, Fcy, Scx, Scy, Dcx, Dcy, PadInc, Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp, KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Grouped Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

		GroupIn:	Size of the group for input features
		GroupOut:	Size of the group for output features

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, Regular convolution with double precision output: KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out)

	CNN_GroupedConvolutionPoolAct_fp16
	
*********************************************************************************************************************************************************************/

int CNN_GroupedConvolutionPoolAct_fp16(
			char         *Name,

			CNN_GenControl_T *Ctrl,

			int GroupIn,
			int GroupOut,

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
                GenTilingError("CNN_GroupedConvolutionPoolAct_fp16: Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
        if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_GroupedConvolutionPoolAct_fp16: Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL or KOP_AVGPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_GroupedConvolutionPoolAct_fp16: Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
		   		  PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
				  &Wc, &Hc, &Wo, &Ho, 0, 0, 0, 0);

	if ((InFeat%GroupIn)||(OutFeat%GroupOut)||((InFeat/GroupIn)!=(OutFeat/GroupOut)))
		GenTilingError("CNN_GroupedConvolutionPoolAct_fp16: %s cannot divide In(%d)/Out(%d) feature spaces with these group parameters: GroupIn %d, GroupOut: %d",
				Name, InFeat, OutFeat, GroupIn, GroupOut);

	OpenKernelGroup(Name);
	CNN_ConvolutionPoolAct_fp16(BodyName,
			Ctrl,
                        GroupIn, GroupOut, Width, Height,
			ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad,
			PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
			ActOper
			);
	CloseKernelGroup();

	CKernel_Arg_T **KCArgs = AllocateCArgs(4);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),     "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1), "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),   "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),    "Out");

	Object_T **KArgs = AllocateKerArgs(4);
	int Ka=0;
	KArgs[Ka++] = KerGroupArg("In",	    O_IN,  NGroups*GroupIn*Width*Height,     2,     "In");
	KArgs[Ka++] = KerGroupArg("Filter", O_IN,  NGroups*GroupIn*GroupOut*Fcx*Fcy, 2, "Filter");
	KArgs[Ka++] = KerGroupArg("Bias",   O_IN,  NGroups*GroupOut,                 2,   "Bias");
	KArgs[Ka++] = KerGroupArg("Out",    O_OUT, NGroups*GroupOut*Wo*Ho,           2,    "Out");

        UKGroup = UserKernelGroupK(Name,
		NGroups,
		KCArgs,
		0,
		Calls(1,
		      UserKernelCall(BodyName, LOC_GROUP,
				Bindings(4,
					KG_ArgOper("In",     '*', GroupIn*Width*Height*2),
					KG_ArgOper("Filter", '*', GroupIn*GroupOut*Fcx*Fcy*2),
					KG_ArgOper("Bias",   '*', GroupOut*2),
					KG_ArgOper("Out",    '*', GroupOut*Wo*Ho*2)
				)
			)
		),
                KArgs
	);
	return (UKGroup!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Pooling (Max or Average) followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_MAXPOOL or KOP_AVGPOOL or KOP_NONE
		Fpx:		Size of the pooling filter, x dimension
		Fpy:		Size of the pooling filter, y dimension
		Dpx:		Dilation factor, x dimension
		Dpy:		Dilation factor, y dimension
		Spx:		Pooling stride, x dimension
		Spy:		Pooling stride, y dimension

		ActOper:	Optional activation function: if (PoolOper!=KOP_NONE) KOP_RELU or KOP_NONE
				else Optional activation function: KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Out)

	CNN_PoolAct_fp16
		
*********************************************************************************************************************************************************************/

int CNN_PoolAct_fp16(
			char *Name,

			CNN_GenControl_T *Ctrl,

                        int InFeat,
                        int OutFeat,
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
        if (PoolOper==KOP_NONE && ActOper!=KOP_NONE) return CNN_Act_fp16(Name, Ctrl, InFeat, Width, Height, ActOper);

        Tile_Orientation_T TileOrientation = TILE_HOR;
        int ParFeat = 1, HWC = 0;
        float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
        AT_PadType PadType = PAD_BALANCED_LEFT;
        if (Ctrl) {
                if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
                if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
                if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
		if (Ctrl->HWC) HWC = 1;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
        }
	int F = O_FLOAT;
        int TileCons, NeedFpx=0, NeedFpy=0, NeedDpx=0, NeedDpy=0, NeedSpx=0, NeedSpy=0, OverlapP;
        int Wo, Ho;
        int UsedWidth, UsedHeight;
        int PadPw=0, PadPh=0;
        v4s PadInp = (v4s){0,0,0,0};
        char *PoolKerName=0, *ActKerName=0;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int StandAloneAct = (ActOper!=KOP_NONE);
	int KerLayout = CALL_FLOAT_KER|(HWC?CALL_HWC_KER:0);
	KernelOper_T KernelOper = CNN_CompositeKernel(PoolOper, ActOper, KOP_NONE);
        int Log=1;

        if (!(PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_Pool_fp16 Kernel: %s, PoolOper, expecting KOP_MAXPOOL or KOP_AVGPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_Pool_fp16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

        /* Set Kernel characteristics */
        CNN_LayerOutputDim(Width, Height, KOP_NONE, 1, 1, 1, 1, 1, 1, 1, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, 0, 0, &Wo, &Ho, 0, 0, &PadPw, &PadPh);
        PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        CNN_TileOverlap(TileOrientation, 1, 1, 1, 1, 1, 1, Fpx, Fpy, Dpx, Dpy, Spx, Spy, 0, &OverlapP);
        UsedWidth  = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHeight = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
        TileCons = (TileOrientation==TILE_HOR)?(Spy):(Spx);
        /* Re evaluate truly used width/height and update padding accordingly */
        PadInp[1] = Max(0, PadInp[1]-(Width-UsedWidth)); PadInp[3] = Max(0, PadInp[3]-(Height-UsedHeight));

        PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
        if (PoolKerName==0) PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, KerLayout, 2, 0, 0, 0, 2, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
								     &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
        else if (ActOper) StandAloneAct = 0;
        if (PoolKerName==0) GenTilingError("CNN_Pool_fp16 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

        if (ActOper && StandAloneAct) {
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (ActKerName==0) GenTilingError("CNN_Pool_fp16 Kernel: %s, Can't find a matching Activation basic kernel", Name);
        }

        if (PoolOper) LayerOp += InFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += InFeat*Wo*Ho;

        LayerBandwidth += Width*Height*2*InFeat;
        LayerBandwidth += Wo*Ho*2*InFeat;

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

	CKernel_Arg_T **KCArgs = AllocateCArgs(2);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),     "In");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),    "Out");

	Object_T **KArgs = AllocateKerArgs(2);
	int Ka=0;
	if (HWC==0) {
		KArgs[Ka++] = KerArgP("In",  KerArgSpace(2,D0,T0), F|OBJ_IN_DB,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, 2,  OverlapP, 0, TileCons, "In");
		KArgs[Ka++] = KerArg ("Out", KerArgSpace(2,D0,T0), F|OBJ_OUT_DB,     Wo,     Ho,                     		       2,         0, 0,        0, "Out");
	} else {
		KArgs[Ka++] = KerArgP("In",  KerArgSpace(2,T0,D0), F|OBJ_IN_DB,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, 2,  OverlapP, 0, TileCons, "In");
		KArgs[Ka++] = KerArg ("Out", KerArgSpace(2,T0,D0), F|OBJ_OUT_DB,     Wo,     Ho,                     		       2,         0, 0,        0, "Out");
	}

        UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = UserKernel(Name,
		ParFeat?
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
		KernelIterSpace(2, IterFixedSpace(D0, InFeat), IterTiledSpace(T0)),
                TileOrientation,
                KCArgs,
                Calls(2,
			Call(PoolKerName, LOC_LOOP,
				(HWC==0)?
				Bindings(16,
					K_Arg("In", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_W),
					K_Arg("In", KER_ARG_TILE_USEDW),
					K_Arg("In", KER_ARG_TILE_H),
					K_Arg("In", KER_ARG_TILE_USEDH),
					ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_PAD),
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,				/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,				/* Pooling Stridex */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,				/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,				/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,				/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING,				/* Pooling Dy */
					Imm(CNN_EncodePoolOperation(PoolOper, ActOper)),		/* MaxPool or AvgPool */
					Imm((TileOrientation==TILE_HOR)?1:0)				/* Pooling Orientation */
				):
				Bindings(13,
					K_Arg("In", KER_ARG_TILE),					/* Input tile */
					K_Arg("In", KER_ARG_TILE_W),					/* Input tile width */
					K_Arg("In", KER_ARG_TILE_H),					/* Input tile height */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,				/* Pool Fx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,				/* Pool Fy */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,				/* Pool Stridex */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,				/* Pool Stridey */
					K_ArgPred("In", KER_ARG_TILEFIRST, T0), 			/* First Tile */
					K_Arg("In", KER_ARG_TILE_PAD),					/* Pool Padding */
					K_Arg("Out", KER_ARG_TILE),					/* Pooling output tile */
					ParFeat?K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):Imm(1),	/* In Features */
					K_Arg("Out", KER_ARG_TILE_W),					/* Output tile width */
					K_Arg("Out", KER_ARG_TILE_H)					/* Output tile height */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                                Bindings(6,
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),       /* Number of features in this tile */
                                        K_Arg("Out", KER_ARG_TILE_W),                                   /* Tile width */
                                        K_Arg("Out", KER_ARG_TILE_H),                                   /* Tile height */
					BindKExpr("UB")							/* Activation upper bound, if ReLUN */
                                )
		        )
                     ),
                KArgs
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 4, InFeat, Height, Width, 2);
		AddKernelFloatArgDim(Name, "Out", 4, OutFeat, Ho, Wo, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 0, 0, 2},
				  (v4s) {0,0,0,0},
				  InFeat, OutFeat, Width, Height,
				  0,0, 0,0, 0,0, (v4s) 0,
				  Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp,
				  KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}


/*********************************************************************************************************************************************************************
 	Generator for Global Pooling (Max or Average)

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL
		ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Out)

	CNN_GlobalPoolAct_fp16
		
*********************************************************************************************************************************************************************/

int CNN_GlobalPoolAct_fp16(
			char *Name,

			CNN_GenControl_T *Ctrl,

                        int InFeat,
                        int OutFeat,
                        int Width,
                        int Height,

			KernelOper_T PoolOper,
			KernelOper_T ActOper
			)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1, HWC = 0;
	int F = O_FLOAT;
	int Wo, Ho;
	char *PoolKerName=0, *ActKerName=0;
	float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	KernelOper_T KernelOper = CNN_CompositeKernel(PoolOper, KOP_NONE, KOP_NONE);
	int StandAloneAct = (ActOper!=0);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	int Log=1;

        if (Ctrl) {
		if (Ctrl->HWC) HWC = 1;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
        }
	int KerLayout = CALL_FLOAT_KER|(HWC?CALL_HWC_KER:0);

	if (!(PoolOper == KOP_GLOBAL_MAXPOOL || PoolOper == KOP_GLOBAL_AVGPOOL))
		GenTilingError("CNN_GlobalPool Kernel: %s, PoolOper should be KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL", Name);
	if (InFeat != OutFeat) GenTilingError("CNN_GlobalPool Kernel: %s, InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);
	PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, ActOper, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (PoolKerName) StandAloneAct = 0;
        else if (StandAloneAct) PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (PoolKerName==0) GenTilingError("CNN_GlobalPoolAct_fp16 Kernel: %s, Can't find a matching Pooling basic kernel", Name);

        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_GlobalPoolAct_fp16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);
        if (StandAloneAct) {
                ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (ActKerName==0) GenTilingError("CNN_GlobalPoolAct_fp16 Kernel: %s, Can't find a matching Activation basic kernel", Name);
        }

        Wo = 1; Ho = 1;

	if (PoolOper) LayerOp += OutFeat*Wo*Ho*Width*Height;
	LayerBandwidth += Width*Height*2*InFeat;
	LayerBandwidth += Wo*Ho*2*OutFeat;

	if (Log) {
        	printf("Global Pool => W: %d => Wo: %d\n", Width,  Wo);
        	printf("            => H: %d => Ho: %d\n", Height, Ho);
        	printf("            => Feat: %d\n", OutFeat);
		if (PoolKerName)      printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ActKerName)      printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

        UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = 
      	UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)),
               	TileOrientation,
               	CArgs(2,
			TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
			TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                ),
		Calls(2,
			Call(PoolKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("In", KER_ARG_TILE),				/* In tile */
					K_Arg("In", KER_ARG_TILE_W),				/* In tile width */
					K_Arg("In", KER_ARG_TILE_H),				/* In Tile Height */
					K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),		/* Number of output features in this tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					K_Arg("In", KER_ARG_TILEINDEX)				/* In Tile Index */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                                Bindings(6,
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),                      /* Number of features in this tile */
                                        Imm(1),                                                         /* Tile width */
                                        Imm(1),                                                         /* Tile height */
					BindKExpr("UB")							/* Activation upper bound, if ReLUN */
                                )
		        )
                ),
                KerArgs(2,
			KerArg("In",     KerArgSpace(2,D0,T0), F|OBJ_IN_DB,   Width, Height, 2,  0, 0, 0, "In"),
                        KerArg("Out",    KerArgSpace(1,D0),    F|OBJ_OUT_DB,      1,      1, 2,  0, 0, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 4, InFeat, Height, Width, 2);
		AddKernelFloatArgDim(Name, "Out", 4, OutFeat, 1, 1, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 0, 0, 2},
				  (v4s) {0,0,0,0},
				  InFeat, OutFeat, Width, Height,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0, 0, 0, 0, 0, 0, (v4s) 0,
				  KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
        Generator for Activation with tensor centric scaling

        Template:
                Name:           Name of the generated user kernel

                Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

                Feat:           Number of feature's maps
                Width:          Number of columns of a given feature map
                Height:         Number of lines of a given feature map

                ActOper:        KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

                Signature:      Name(In, Out, Infos)

        CNN_Act_fp16

*********************************************************************************************************************************************************************/

int CNN_Act_fp16(
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
        float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
        if (Ctrl) {
                if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
                if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
                if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
        }
	int F = O_FLOAT;
        int TileCons = 0;
        char *ActKerName=0;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int StandAloneAct = (ActOper!=KOP_NONE);
        int Log=1;

        if (!(ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_Act_fp16 Kernel: %s, ActOper, expecting KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

        ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
        if (ActKerName==0) GenTilingError("CNN_Act_fp16 Kernel: %s, Can't find a matching Activation basic kernel", Name);

        LayerOp += Feat*Width*Height;

        LayerBandwidth += Width*Height*2*Feat;
        LayerBandwidth += Width*Height*2*Feat;

        if (Log) {
                printf("Act  => W: %d, Wo: %d\n", Width,  Width);
                printf("     => H: %d, Ho: %d\n", Height, Height);
                printf("%20s: %s\n", "ActKerName", ActKerName);
                printf("Nb Oper : %lld\n", LayerOp);
        }

        UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = UserKernel(Name,
                ParFeat?
                KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
                KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(2,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                ),
                Calls(1,
                        Call(ActKerName, LOC_LOOP,
                                Bindings(6,
                                        K_Arg("In",  KER_ARG_TILE),                                     /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        ParFeat?K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):Imm(1),        /* Number of features in this tile */
                                        K_Arg("In", KER_ARG_TILE_W),                                    /* Tile width */
                                        K_Arg("In", KER_ARG_TILE_H),                                    /* Tile height */
                                        BindKExpr("UB")                                                 /* Activation upper bound, if ReLUN */
                                )
                        )
                     ),
                KerArgs(2,
                        KerArg("In",     KerArgSpace(2,D0,T0), F|OBJ_IN_DB,                         Width, Height, 2, 0, 0, 0, "In"),
                        KerArg("Out",    KerArgSpace(2,D0,T0), F|OBJ_OUT_DB,                        Width, Height, 2, 0, 0, 0, "Out")
                )
        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelFloatArgDim(Name, "In", 4, Feat, Height, Width, 2);
                AddKernelFloatArgDim(Name, "Out", 4, Feat, Height, Width, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 0, 0, 2},
				  (v4s) {0,0,0,0},
				  Feat, Feat, Width, Height,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0,0, 0,0, 0,0, (v4s) 0,
				  ActOper,
				  0, 0);
        }
        return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Linear layers followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

		InDim:		Number of inputs
		OutDim:		Number of outputs

		LinearOper	Should always be KOP_LINEAR
		ActOper		Optional activation function: KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out)
				Name(In, Filter, Bias, ReLUN, Out)

	CNN_LinearAct_fp16
	
*********************************************************************************************************************************************************************/

int CNN_LinearAct_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

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
	float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
	}
	int F = O_FLOAT;
	KernelOper_T KernelOper = CNN_CompositeKernel(LinearOper, ActOper, KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	if (LinearOper != KOP_LINEAR) GenTilingError("CNN_LinearAct_fp16 Kernel: %s, only KOP_LINEAR should be used as LinearOper argument", Name);

	char *ActKerName = 0;
	char *LinearKerName = CNN_FindMatchingKernelAttr(LinearOper, ActOper, ParFeat, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (LinearKerName==0) GenTilingError("CNN_LinearAct_fp16 Kernel: %s, Can't find a matching %s basic kernel", Name, ActOper?"with activation":"");

	LayerOp += InDim*OutDim;
	if (ActOper) LayerOp += OutDim;
	LayerBandwidth += InDim*OutDim*2;
	LayerBandwidth += OutDim*2;
	LayerBandwidth += InDim*OutDim*2;
	LayerBandwidth += 2*OutDim;

	if (Log) {
		printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s\n", Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
		if (LinearKerName) printf("Linear Kernel: %s\n", LinearKerName);
	}
	Kernel_T *Kernel;

	CKernel_Arg_T **KCArgs = AllocateCArgs(4);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),     "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1), "Filter");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),   "Bias");
       	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeF(2,1,1),    "Out");

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

	Object_T **KArgs = AllocateKerArgs(4);
	int Ka=0;
	KArgs[Ka++] = KerArg("In",      KerArgSpace(1,T0), F|O_IN|O_BUFF|O_NTILED,  1, 1,      InDim*2, 0, 0, 0, "In");
	KArgs[Ka++] = KerArg("Filter",  KerArgSpace(1,D0), F|OBJ_IN_DB|O_CONST,     1, 1,      InDim*2, 0, 0, 0, "Filter");
	KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D0), F|OBJ_IN_DB|O_CONST,     1, 1,      2,       0, 0, 0, "Bias");
	KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D0), F|OBJ_OUT_DB,            1, 1,      2,       0, 0, 0, "Out");

        UserSymbols(1, US_Float("UB", UB));
        Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, OutDim, 8), IterTiledSpace(T0)),
                TileOrientation,
                KCArgs,
                Calls(2,
			Call(LinearKerName, LOC_LOOP,
				Bindings(9,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
					K_Arg("Filter", KER_ARG_TILE),				/* Filter tile */
					K_Arg("Bias", KER_ARG_TILE),				/* Bias tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					Imm(InDim),						/* Input tile size */
					Imm(InDim),						/* Input tile size */
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0),		/* Output tile size */
					BindKExpr("UB"),					/* ReLUN upper bound */
					Imm(0)							/* Tile index */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                                Bindings(6,
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),       /* Number of features in this tile */
                                        K_Arg("Out", KER_ARG_TILE_W),                                   /* Tile width */
                                        K_Arg("Out", KER_ARG_TILE_H),                                   /* Tile height */
					BindKExpr("UB")					                /* Activation upper bound, if ReLUN */
                                )
		        )
		),
                KArgs
	);
	if (Kernel==0) {
		if (Log) {
			printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s, output parallel failed, switching to feature parallel form\n",
				Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
			if (LinearKerName) printf("Linear Kernel   : %s\n", LinearKerName);
			if (ActKerName)    printf("Act Kernel: %s\n", ActKerName);
		}
		/* First try with Input as a buffer in */
		Object_T **KArgs = AllocateKerArgs(4);
		int Ka=0;
		KArgs[Ka++] = KerArg("In",      KerArgSpace(1,T0), 	F|OBJ_BUFFER_IN,          1, InDim,  2, 0, 0, 0, "In");
		KArgs[Ka++] = KerArg("Filter",  KerArgSpace(2,D0,T0), 	F|OBJ_IN_DB|O_CONST, 	1, InDim,  2, 0, 0, 0, "Filter");
		KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D0), 	F|OBJ_BUFFER_IN|O_CONST,  1, 1,      2, 0, 0, 0, "Bias");
		KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D0), 	F|OBJ_BUFFER_OUT,         1, 1,      2, 0, 0, 0, "Out");

	        Kernel = UserKernel(Name,
			KernelIterSpace(2, IterFixedSpace(D0, OutDim), IterTiledSpace(T0)),
	                TileOrientation,
                	KCArgs,
	                Calls(1,
				Call(LinearKerName, LOC_LOOP,
					Bindings(9,
						K_Arg("In", KER_ARG_TILE),				/* Input tile */
						K_Arg("Filter", KER_ARG_TILE),				/* Filter tile */
						K_Arg("Bias", KER_ARG_TILE),				/* Bias tile */
						K_Arg("Out", KER_ARG_TILE),				/* Output tile */
						K_Arg("Filter", KER_ARG_TILE_H),			/* Input tile size */
						Imm(InDim),						/* Total input dim */
						BindKExpr("UB"),					/* Activation upper bound, if ReLUN */
						Ker_IteratorIndex(T0)					/* Which tile index */
					)
				)
			),
	                KArgs
		);

		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Kernel==0) {
			LinearKerName = CNN_FindMatchingKernelAttr(LinearOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
			if (LinearKerName==0) GenTilingError("CNN_LinearAct_fp16 Kernel: %s, Can't find a Linear matching basic kernel", Name);
			ActKerName    = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
			if (ActKerName==0) GenTilingError("CNN_LinearAct_fp16 Kernel: %s, Can't find an Activation matching basic kernel", Name);

			if (Log) {
				printf("Linear Layer %s, %s: InDim: %d, OutDim: %d, Activation: %s, Feature parallel with in buffered failed, switching to non buffered in form\n",
					Name, CNN_KernelOperImage(LinearOper), InDim, OutDim, CNN_KernelOperImage(ActOper));
				if (LinearKerName) printf("Linear Kernel   : %s\n", LinearKerName);
				if (ActKerName)    printf("Act Kernel: %s\n", ActKerName);
			}

			Object_T **KArgs = AllocateKerArgs(4);
			int Ka=0;
			KArgs[Ka++] = KerArg("In",      KerArgSpace(1,T0), F|OBJ_IN_DB,             1, InDim,  2,  0, 0, 0, "In");
			KArgs[Ka++] = KerArg("Filter",  KerArgSpace(2,D0,T0), F|OBJ_IN_DB|O_CONST,  1, InDim,  2,  0, 0, 0, "Filter");
			KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D0), F|OBJ_BUFFER_IN|O_CONST, 1, 1,      2,  0, 0, 0, "Bias");
			KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D0), F|OBJ_BUFFER_OUT,        1, 1,      2,  0, 0, 0, "Out");

		        Kernel = UserKernel(Name,
				KernelIterSpace(2, IterFixedSpace(D0, OutDim), IterTiledSpace(T0)),
		                TileOrientation,
	                	KCArgs,
		                Calls(2,
					Call(LinearKerName, LOC_LOOP,
						Bindings(9,
							K_Arg("In", KER_ARG_TILE),				/* Input tile */
							K_Arg("Filter", KER_ARG_TILE),				/* Filter tile */
							K_Arg("Bias", KER_ARG_TILE),				/* Bias tile */
							K_Arg("Out", KER_ARG_TILE),				/* Output tile */
							K_Arg("Filter", KER_ARG_TILE_H),			/* Input tile size */
							Imm(InDim),						/* Total input dim */
							AT_IGNORE_ARG_BINDING,					/* Activation upper bound, if ReLUN */
							Ker_IteratorIndex(T0)					/* Which tile index */
						)
					),
					Call(ActKerName, LOC_LOOP_EPILOG,
                                		Bindings(6,
                                       	 		K_Arg("Out", KER_ARG_TILE),                                     /* Input tile */
                                        		K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        		ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),       /* Number of features in this tile */
                                        		K_Arg("Out", KER_ARG_TILE_W),                                   /* Tile width */
                                        		K_Arg("Out", KER_ARG_TILE_H),                                   /* Tile height */
							BindKExpr("UB")							/* Activation upper bound, if ReLUN */
                                		)
		        		)
				),
		                KArgs
			);
		}
	}

	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 2, InDim, 2);
		AddKernelFloatArgDim(Name, "Filter", 3, OutDim, InDim, 2);
		AddKernelFloatArgDim(Name, "Bias", 2, OutDim, 2);
		AddKernelFloatArgDim(Name, "Out", 2, OutDim, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 2, 2, 2},
				  (v4s) {0,0,0,0},
				  InDim, OutDim, 1, 1,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0,0, 0,0, 0,0, (v4s) 0,
				  KernelOper,
				  0,
				  (2==1)?7:15);
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

		Signature:	Name(In, Out)

	CNN_SoftMax_fp16
                Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to
                a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15
                if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)
	
*********************************************************************************************************************************************************************/

int CNN_SoftMax_fp16(
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
	int F = O_FLOAT;
	KernelOper_T KernelOper = CNN_CompositeKernel(SoftMaxOper, KOP_NONE, KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernelAttr(SoftMaxOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax_fp16 Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only, Q15 output", Name);

	LayerOp += Dim;
	LayerBandwidth += Dim*2 + Dim*2;
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(2,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_LOOP,
				Bindings(4,
					K_Arg("In", KER_ARG_TILE),	/* Input tile */
					Imm(1),
					K_Arg("In", KER_ARG_TILE_H),	/* Number of inputs */
					K_Arg("Out", KER_ARG_TILE)	/* Output tile */
				)
			)
		),
                KerArgs(2,
                        KerArg("In",  KerArgSpace(1,T0), F|OBJ_BUFFER_IN,  1, Dim, 2, 0, 0, 8, "In"),
                        KerArg("Out", KerArgSpace(1,T0), F|OBJ_BUFFER_OUT, 1, Dim, 2, 0, 0, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 2, Dim, 2);
		AddKernelFloatArgDim(Name, "Out", 2, Dim, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 0, 0, 2},
				  (v4s) {0,0,0,0},
				  1, 1, 1, Dim,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0,0, 0,0, 0,0, (v4s) 0,
				  KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}

int CNN_SoftMax2D_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Dim,
	int N,

	KernelOper_T SoftMaxOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	int F = O_FLOAT;
	KernelOper_T KernelOper = CNN_CompositeKernel(SoftMaxOper, KOP_NONE, KOP_NONE);
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernelAttr(SoftMaxOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 0, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax_fp16 Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only, Q15 output", Name);

	LayerOp += N*Dim;
	LayerBandwidth += (int64_t) N*(Dim*1 + Dim*2);
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, N, 1), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(2,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_LOOP,
				Bindings(4,
					K_Arg("In", KER_ARG_TILE),	/* Input tile */
					K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0), /* Number of features in this tile */
					K_Arg("In", KER_ARG_TILE_H),	/* Number of inputs */
					K_Arg("Out", KER_ARG_TILE)	/* Output tile */
				)
			)
		),
                KerArgs(2,
                        KerArg("In",  KerArgSpace(2,D0,T0), F|OBJ_BUFFER_IN,  1, Dim, 2, 0, 0, 8, "In"),
                        KerArg("Out", KerArgSpace(2,D0,T0), F|OBJ_BUFFER_OUT, 1, Dim, 2, 0, 0, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 3, N, Dim, 2);
		AddKernelFloatArgDim(Name, "Out", 3, N, Dim, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 0, 0, 2},
				  (v4s) {0,0,0,0},
				  1, 1, 1, Dim,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0,0, 0,0, 0,0, (v4s) 0,
				  KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}


/*********************************************************************************************************************************************************************
 	Generator for Matrix Addition layers 

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		InFeat:		Number of input features
		OutFeat:	Number of input features, should always be equal to InFeat
		Width:		Width of a given feature
		Height:		Height of a given feature

		AddMatOper:	Should always be KOP_MATADD
		ActOper	Optional activation function: KOP_RELU or KOP_NONE for no activation

		Signature:	Name(In1, In2, Out)

	CNN_MatAddAct_fp16
	
*********************************************************************************************************************************************************************/


int CNN_MatAddAct_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int InFeat,
	int OutFeat,
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
	int F = O_FLOAT;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	KernelOper_T KernelOper = CNN_CompositeKernel(AddMatOper, ActOper, KOP_NONE);

	char *MatAddKerName = CNN_FindMatchingKernelAttr(AddMatOper, ActOper, ParFeat, CALL_FLOAT_KER, 2, 2, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (MatAddKerName==0) GenTilingError("CNN_MatAdd Kernel: %s, Can't find a matching basic kernel", Name);

	char *ActKerName = 0;
	if (ActOper != KOP_NONE && !MatAddKerName) {
                MatAddKerName = CNN_FindMatchingKernelAttr(AddMatOper, KOP_NONE, ParFeat, CALL_FLOAT_KER, 2, 2, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		ActKerName = CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 2, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatAdd Kernel: %s, Can't find a matching activation basic kernel", Name);
	}

	
	LayerOp += OutFeat * Width * Height;
	LayerBandwidth += Width*Height*2*InFeat;
	LayerBandwidth += Width*Height*2*InFeat;
	LayerBandwidth += Width*Height*2*OutFeat;

        UserSymbols(1, US_Float("UB", UB));
        Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In1"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "In2"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Out")
                     ),
                Calls(2,
			Call(MatAddKerName, LOC_LOOP,
				Bindings(6,
					K_Arg("In1", KER_ARG_TILE),			/* First input tile */
					K_Arg("In2", KER_ARG_TILE),			/* Second input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_Arg("In1", KER_ARG_TILE_W),			/* Input tile width */
					K_Arg("In1", KER_ARG_TILE_H),			/* Input tile height */
					K_ArgPar("In1", KER_ARG_PARTILE_SIZE, D0)	/* Number of Matrices involved */
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                             	Bindings(6,
                               		K_Arg("Out", KER_ARG_TILE),                                     /* Input tile */
                                      	K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                       	ParFeat?K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):Imm(1),       /* Number of features in this tile */
                                       	K_Arg("Out", KER_ARG_TILE_W),                                   /* Tile width */
                                       	K_Arg("Out", KER_ARG_TILE_H),                                   /* Tile height */
					BindKExpr("UB")							/* Activation upper bound, if ReLUN */
                                	)
		        	)
		),
                KerArgs(3,
                        KerArg("In1",  KerArgSpace(2,D0,T0), F|O_IN|O_DB,  Width, Height,  2, 0, 0, 0, "In1"),
                        KerArg("In2",  KerArgSpace(2,D0,T0), F|O_IN|O_DB,  Width, Height,  2, 0, 0, 0, "In2"),
                        KerArg("Out",  KerArgSpace(2,D0,T0), F|O_OUT|O_DB, Width, Height,  2, 0, 0, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In1", 4, InFeat,  Height, Width, 2);
		AddKernelFloatArgDim(Name, "In2", 4, InFeat,  Height, Width, 2);
		AddKernelFloatArgDim(Name, "Out", 4, OutFeat, Height, Width, 2);

		AT_PrepareForTest(Name,
				  (v4s) {2, 2, 0, 2},
				  (v4s) {0,0,0,0},
				  InFeat, OutFeat, Width, Height,
				  0,0, 0,0, 0,0, (v4s) 0,
				  0,0, 0,0, 0,0, (v4s) 0,
				  KernelOper,
				  0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
    Generator for Channel Padded Matrix Addition layers with input scale adjustment (tensor centric), output scaling (tensor centric) and optional activation

    Template:
        Name:       Name of the generated user kernel
        Ctrl:       Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

        Feat:       Number of features
        Width:      Width of a given feature
        Height:     Height of a given feature
        PadTop:     Top channel padding added to InIdxPaddedIn
        PadBot:     Bottom channel padding added to InIdxPaddedIn
        IdxPaddedIn:    Which input is padded

        AddMatOper: Should always be KOP_MATADD
        ActOper:    Optional activation
        Signature:  Name(In1, In2, Out, Infos, InfosPad)

    CNN_MatAddPaddedAct_fp16

*********************************************************************************************************************************************************************/

int CNN_MatAddPaddedAct_fp16(
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
        if (PadBot == 0 && PadTop == 0) return CNN_MatAddAct_fp16(Name, Ctrl, Feat, Feat, Width, Height, AddMatOper, ActOper);
        if (PadTop + PadBot > Feat) GenTilingError("int CNN_MatAddPaddedAct_SQ8 Kernel: %s, Padding exceeds channel size", Name);
        int FeatBody = Feat - PadTop - PadBot;
        int Ok = 1;

        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        LayerOp += (Feat-PadTop-PadBot) * Width * Height;
        LayerBandwidth += Width*Height*2*(Feat);        // In1
        LayerBandwidth += Width*Height*2*(Feat-PadTop-PadBot);  // In2
        LayerBandwidth += Width*Height*2*(Feat);        // Out

        OpenKernelGroup(Name);
        char *TopName = NULL, *BotName = NULL, *BodyName = NULL;
        Ok = Ok && CNN_MatAddAct_fp16(BodyName = AppendNames(Name, "Body"), Ctrl, FeatBody, FeatBody, Width, Height, AddMatOper, ActOper);
        if (PadTop) {
                Ok = Ok && CNN_Act_fp16(TopName = AppendNames(Name, "PadTop"), Ctrl, PadTop, Width, Height, ActOper);
        }
        if (PadBot) {
                Ok = Ok && CNN_Act_fp16(BotName = AppendNames(Name, "PadBot"), Ctrl, PadBot, Width, Height, ActOper);
        }
        CloseKernelGroupNoMerge();
        if (Ok==0) return 0;

        int A = 0;
        CKernel_Arg_T **GroupCArgs = AllocateCArgs(3);
        GroupCArgs[A++] = TCArg(CNN_ArgDataTypeF(2,1,1), "In1");
        GroupCArgs[A++] = TCArg(CNN_ArgDataTypeF(2,1,1), "In2");
        GroupCArgs[A++] = TCArg(CNN_ArgDataTypeF(2,1,1), "Out");
        char *PaddedInput    = IdxPaddedIn==0?"In1":"In2";
        char *NonPaddedInput = IdxPaddedIn==0?"In2":"In1";
        A = 0;
        CKernelCall_T **GroupCCalls;
        GroupCCalls = AllocateCalls(1+(PadTop!=0?1:0)+(PadBot!=0?1:0));
        if (PadTop) {
                GroupCCalls[A++] = UserKernelCall(TopName, LOC_GROUP,
                        Bindings(2,
                                C_Arg(PaddedInput),
                                C_Arg("Out")
                                )
                        );
                GroupCCalls[A++] = UserKernelCall(BodyName, LOC_GROUP,
                        Bindings(3,
                                IdxPaddedIn==0?KG_ArgOper("In1", '+', PadTop*Width*Height*2):C_Arg("In1"),
                                IdxPaddedIn==0?C_Arg("In2"):KG_ArgOper("In2", '+', PadTop*Width*Height*2),
                                KG_ArgOper("Out", '+', PadTop*Width*Height*2)
                                )
                        );
        } else {
                GroupCCalls[A++] = UserKernelCall(BodyName, LOC_GROUP,
                        Bindings(3,
                                C_Arg("In1"),
                                C_Arg("In2"),
                                C_Arg("Out")
                                )
                        );
        }
        if (PadBot) {
                GroupCCalls[A++] = UserKernelCall(BotName, LOC_GROUP,
                        Bindings(2,
                                KG_ArgOper(PaddedInput, '+', (PadTop + FeatBody)*Width*Height*2),
                                KG_ArgOper("Out", '+', (PadTop + FeatBody)*Width*Height*2)
                                )
                        );
        }
        A = 0;
        int Sz  = Feat*Width*Height;
        int Sz2 = Width*Height*(Feat-PadTop-PadBot);
        Object_T **GroupKerArgs = AllocateKerArgs(3);
        GroupKerArgs[A++] = KerGroupArg("In1", O_IN,  Sz,  2, "In1");
        GroupKerArgs[A++] = KerGroupArg("In2", O_IN,  Sz2, 2, "In2");
        GroupKerArgs[A++] = KerGroupArg("Out", O_OUT, Sz,  2, "Out");
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
 	Generator for Matrix Multiplication layers.

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (ReluN), Def=(6)

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

		MatMulOper:	Should always be KOP_MATMUL
		ActOper:	Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out)
				Name(In2, In1, Bias, ReLUN, Out)

	CNN_MatMulAct_fp16
	
*********************************************************************************************************************************************************************/

int CNN_MatMulAct_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

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
	int F = O_FLOAT;
	int NeedScx, NeedScy;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
        int LineO = LineM1, ColO = ColM2;
	float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	int ConsT0 = Scx;
	int Nbuff;
	int NoBias = (MatMulOper == KOP_MATMUL_NOBIAS || MatMulOper == KOP_MATMUL_NOBIAS_TRANSPOSED);
        int SAxis = (MatMulOper == KOP_MATMUL)?LineO:ColO;
	int Transposed = (MatMulOper == KOP_MATMUL_TRANSPOSED);
        int TA = (MatMulOper == KOP_MATMUL)?T0:T1;
        int TB = (MatMulOper == KOP_MATMUL)?T1:T0;

	if (Ctrl) {
		if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
	}
	if (!(MatMulOper == KOP_MATMUL) && !(MatMulOper == KOP_MATMUL_TRANSPOSED) && !(MatMulOper == KOP_MATMUL_NOBIAS) && !(MatMulOper == KOP_MATMUL_NOBIAS_TRANSPOSED)) GenTilingError("CNN_MatMulAct_fp16 Kernel: %s, MatMulOper should be KOP_MATMUL or KOP_MATMUL_TRANSPOSED or KOP_MATMUL_NOBIAS or KOP_MATMUL_NOBIAS_TRANSPOSED", Name);

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MatMulAct_fp16 Kernel: %s, ActOper should be KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_LEAKYRELU", Name);

	KernelOper_T KernelOper = CNN_CompositeKernel(MatMulOper, ActOper, KOP_NONE);
	if (ColM1 != LineM2) GenTilingError("CNN_MatMulAct_fp16: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d]", Name, LineM1, ColM1, LineM2, ColM2);
	if (Width==0||Height==0) {
		Width = ColM2; Height=1; Scx = 1; Scy = 1;
	}
	if ((Width*Height) != ColM2) GenTilingError("CNN_MatMulAct_fp16: %s, ColM2: %d should always be equal to Width: %d * Height: %d", Name, ColM2, Width, Height);
	char *MatMulKerName = CNN_FindMatchingKernelAttr(MatMulOper, ActOper, 1, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);

	char *ActKerName = 0;
	if (ActOper != KOP_NONE && MatMulKerName == 0) {
		MatMulKerName = CNN_FindMatchingKernelAttr(MatMulOper, KOP_NONE, 1, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
		CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 2, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatAdd Kernel: %s, Can't find a matching activation basic kernel", Name);
	} 
	if (MatMulKerName==0) GenTilingError("CNN_MatMulAct_fp16 Kernel: %s, Can't find a matching basic kernel", Name);

	int ColFirst = ((LineM1*ColM1)<(LineM2*ColM2));

	Nbuff = 4;

	ColO = ((Width+Scx-1)/Scx) * ((Height+Scy-1)/Scy);
	LayerOp += ColM1*ColO*LineM1;
	LayerBandwidth += LineM1*(ColM1*ColM2*(2+2));
	LayerBandwidth += LineM1*ColM2*2;
	LayerBandwidth += LineM1*2;

	if (Scy!=1) ConsT0 = Width*Scy; else ConsT0 = 2;
	
	if (Log) {
		printf("CNN_MatMulAct_fp16: %s\n", Name);
		printf("In1  => W: %4d, H: %4d\n", ColM1, LineM1);
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d\n", ColM2, LineM2, Width, Height, Scx, Scy);
		printf("Out  => W: %4d, H: %4d => %s\n", ColO, LineO, ColFirst?"Column first":"Line First");
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Act: %s\n", CNN_KernelOperImage(ActOper));
		// printf("Nb Oper : %lld\n", LayerOp);
	}

	int ObjCons = !Transposed?OBJ_CONSTRAINTS_TILE_VER:0;
	if (Transposed) {
		LineM2 = ColM2; ColM2 = ColM1;
	}
        UserSymbols(1, US_Float("UB", UB));
	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(4,
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "In2"),
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "In1"),
                      !NoBias?TCArg(CNN_ArgDataTypeF(2,1,1), "Bias"):AT_NO_C_ARG,
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "Out")
                ),
		Calls(2,
			Call(MatMulKerName, LOC_LOOP,
				Bindings(16,
					K_Arg("In1",  KER_ARG_TILE), K_Arg("In1",  KER_ARG_TILE_W), K_Arg("In1",  KER_ARG_TILE_H),
					K_Arg("In2",  KER_ARG_TILE), Transposed?K_Arg("In2",  KER_ARG_TILE_H):K_Arg("In2",  KER_ARG_TILE_W),
					!NoBias?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
					K_Arg("Out",  KER_ARG_TILE),  K_Arg("Out", KER_ARG_TILE_W),
					K_Arg(ColFirst?"In1":"In2",  KER_ARG_TILE_BASE),
					!Transposed?K_Arg("KerBuff", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
					Imm(ColFirst),
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Width):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Height):AT_IGNORE_ARG_BINDING,
					BindKExpr("UB")
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                             	Bindings(6,
                               		K_Arg("Out", KER_ARG_TILE),                    	/* Input tile */
                                      	K_Arg("Out", KER_ARG_TILE),                    	/* Output tile */
                                       	K_Arg("Out", KER_ARG_TILE_H),                 	/* Number of features */
                                       	K_Arg("Out", KER_ARG_TILE_W),                  	/* Tile width */
					Imm(1),						/* Tile height */
					BindKExpr("UB")					/* Activation upper bound, if ReLUN */
                                	)
		        	)
		),
		ColFirst?
		KerArgs(5,
	    !Transposed?KerArg("KerBuff",KerArgSpace(1, T1), F|O_BUFF|O_NTILED,    Nbuff*ColM1,1,  2,  0, 0,                                                0, 0):AT_NO_KER_ARG,
			KerArg("In1",    KerArgSpace(1, T0), F|O_IN|O_DB|O_CONST,  ColM1,  LineM1, 2,  0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T1), F|O_IN|O_DB,          ColM2,  LineM2, 2,  0, ObjCons|OBJ_CONSTRAINTS_PAD_REM,		    ConsT0, "In2"),
		!NoBias?KerArg("Bias",   KerArgSpace(1, TA), F|O_IN|O_DB|O_CONST,      1,  SAxis,  2,  0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"):AT_NO_KER_ARG,
			KerArg("Out",    KerArgSpace(1, T1), F|O_OUT|O_DB,          ColO,  LineO,  2,  0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, 0, "Out")
		):
		KerArgs(5,
	    !Transposed?KerArg("KerBuff",KerArgSpace(1, T0), F|O_BUFF|O_NTILED,    Nbuff*ColM1,1,  2,  0, 0,                                                0, 0):AT_NO_KER_ARG,
			KerArg("In1",    KerArgSpace(1, T1), F|O_IN|O_DB|O_CONST,  ColM1,  LineM1, 2,  0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T0), F|O_IN|O_DB,          ColM2,  LineM2, 2,  0, ObjCons|OBJ_CONSTRAINTS_PAD_REM, 	            ConsT0, "In2"),
		!NoBias?KerArg("Bias",   KerArgSpace(1, TB), F|O_IN|O_DB|O_CONST,      1,  SAxis,  2,  0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"):AT_NO_KER_ARG,
			KerArg("Out",    KerArgSpace(1, T1), F|O_OUT|O_DB,          ColO,  LineO,  2,  0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In1", 3, LineM1, ColM1, 2);
		AddKernelFloatArgDim(Name, "In2", 4, LineM2, Height, Width, 2);
		if (!NoBias) AddKernelFloatArgDim(Name, "Bias", 2, SAxis, 2);
		AddKernelFloatArgDim(Name, "Out", 3, LineO, ColO, 2);

		AT_PrepareForTest(Name,
			  	(v4s) {2, 2, 2, 2},
			  	(v4s) {0,0,0,0},
			  	ColM1, LineM1, ColM2, LineM2,
			  	0,0, Scx,Scy, 0,0, (v4s) Width,
			  	0,0, 0,0, 0,0, (v4s) Height,
			  	KernelOper,
			  	0, 0);
	}
	return (Kernel!=0);
}

/*********************************************************************************************************************************************************************
 	Generator for Matrix Multiplication layers. Special form to handle small form factor In1 (InFeat x OutFeat)

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP
	This generator assumes that the whole In1 and Bias can be accomodated into shared L1. Expecting to be used for us to 32 InFeat x 32 OutFeat

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (ReluN), Def=(6)

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

        	MatMulOper	Should always be KOP_MATMUL_SM1
        	ActOper		Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_SWISH, KOP_SIGMOID, KOP_TANH, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out)

	CNN_MatMulSmallM1Act_fp16
	
*********************************************************************************************************************************************************************/

int CNN_MatMulSmallM1Act_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

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
	float UB = (ActOper==KOP_HSIGMOID)?3.0:6.0; // In Case of HSIGMOID, UB is the Offset (default: 3.0)
	int ConsT0 = Scx;
	int TileCons = 8;
	int F = O_FLOAT;

	if (Ctrl) {
		if (Ctrl->ReluN != -1) UB = Ctrl->ReluN;
	}
	if (!(MatMulOper == KOP_MATMUL_SM1)) GenTilingError("CNN_MatMulSmallM1Act_fp16 Kernel: %s, MatMulOper should be KOP_MATMUL_SM1_fp16", Name);

	if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_SIGMOID || ActOper == KOP_TANH || ActOper == KOP_SWISH || ActOper == KOP_HSIGMOID || ActOper == KOP_HTANH || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
		GenTilingError("CNN_MatMulSmallM1Act_fp16 Kernel: %s, ActOper should be KOP_NONE, KOP_RELU, KOP_RELUN, KOP_SIGMOID, KOP_TANH, KOP_SWISH, KOP_HSIGMOID, KOP_HTANH, KOP_HSWISH or KOP_RELUN", Name);

	KernelOper_T KernelOper = CNN_CompositeKernel(MatMulOper, ActOper, KOP_NONE);
	if (ColM1 != LineM2) GenTilingError("CNN_MatMulSmallM1Act_fp16: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d] %s", Name, LineM1, ColM1, LineM2, ColM2);
	if (Width==0||Height==0) {
		Width = ColM2; Height=1; Scx = 1; Scy = 1;
	} else if (Scy != 1) TileCons = Width*Scy;
	if ((Width*Height) != ColM2) GenTilingError("CNN_MatMulSmallM1Act_fp16: %s, ColM2: %d should always be equal to Width: %d * Height: %d", Name, ColM2, Width, Height);

	char *MatMulKerName = CNN_FindMatchingKernelAttr(MatMulOper, ActOper, 1, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,1,1, 0,0,0,0, &NeedScx, &NeedScy, 0);
	char *ActKerName = 0;
	if (ActOper != KOP_NONE && MatMulKerName == 0) {
		MatMulKerName = CNN_FindMatchingKernelAttr(MatMulOper, KOP_NONE, 1, CALL_FLOAT_KER, 2, 2, 2, 0, 2, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
		CNN_FindMatchingKernelAttr(ActOper, KOP_NONE, 0, CALL_FLOAT_KER, 2, 2, 0, 0, 2, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ActKerName==0) GenTilingError("CNN_MatMulSmallM1Act_fp16 Kernel: %s, Can't find a matching activation basic kernel", Name);
	} 
	if (MatMulKerName==0) GenTilingError("CNN_MatMulSmallM1Act_fp16 Kernel: %s, Can't find a matrix multiplication matching basic kernel", Name);

	char *MatTransKerName = CNN_FindMatchingKernel(KOP_MATTRANSP, KOP_NONE, 0, 2, 0, 0, 0, 2, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);
	if (MatTransKerName==0) GenTilingError("CNN_MatMulSmallM1Act_fp16 Kernel: %s, Can't find a matrix transpose matching basic kernel", Name);

	ColO = ((Width+Scx-1)/Scx) * ((Height+Scy-1)/Scy);
	LayerOp += ColM1*ColO*LineM1;
	LayerBandwidth += LineM1*(ColM1*ColM2*(2+2));
	LayerBandwidth += LineM1*ColM2*2;
	LayerBandwidth += LineM1*2;

	if (Log) {
		printf("CNN_MatMulSmallM1Act_fp16: %s\n", Name);
		printf("In1  => W: %4d, H: %4d\n", ColM1, LineM1);
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d, TileCons: %d\n", ColM2, LineM2, Width, Height, Scx, Scy, TileCons);
		printf("Out  => W: %4d, H: %4d\n", ColO, LineO);
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		if (MatTransKerName) printf("%20s: %s\n", "MatTransKerName", MatTransKerName);
		if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
		printf("Act: %s\n", CNN_KernelOperImage(ActOper));
		printf("Nb Oper : %lld\n", LayerOp);
	}
        UserSymbols(1, US_Float("UB", UB));
	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TILE_VER,
                CArgs(4,
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "In2"),
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "In1"),
                      TCArg(CNN_ArgDataTypeF(2,1,1), "Bias"),
                      TCArg(CNN_ArgDataTypeF(2,1,1),  "Out")
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
				Bindings(16,
					K_Arg("In1",  KER_ARG_TILE), Imm(ColM1), Imm(LineM1),
					K_Arg("TransIn2",  KER_ARG_TILE), K_Arg("TransIn2",  KER_ARG_TILE_W),
					K_Arg("Bias",KER_ARG_TILE),
					K_Arg("Out", KER_ARG_TILE),  AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					AT_IGNORE_ARG_BINDING,
					BindKExpr("UB")
				)
			),
			(ActKerName==0)?AT_NO_CALL:
			Call(ActKerName, LOC_LOOP,
                             	Bindings(6,
                               		K_Arg("Out", KER_ARG_TILE),                    	/* Input tile */
                                      	K_Arg("Out", KER_ARG_TILE),                    	/* Output tile */
                                       	K_Arg("Bias", KER_ARG_TILE_H),                 	/* Number of features */
                                       	K_Arg("Out", KER_ARG_TILE_W),                  	/* Tile width */
					Imm(1),						/* Tile height */
					BindKExpr("UB")					/* Activation upper bound, if ReLUN */
                                	)
		        	)
		),
		KerArgs(5,
			KerArg("In1",      KerArgSpace(1, T0), F|O_IN|O_BUFF|O_NTILED|O_CONST,  ColM1, LineM1, 2,  0, 0, 0, "In1"),
			KerArg("In2",      KerArgSpace(1, T0), F|O_IN|O_DB,                     ColM2, LineM2, 2,  0, 0, TileCons, "In2"),
			KerArg("TransIn2", KerArgSpace(1, T0), F|O_BUFF|O_ONETILE,               ColO, LineM2, 2,  0, 0, 0,  ""),
			KerArg("Bias",     KerArgSpace(1, T0), F|O_BUFF|O_IN|O_NTILED|O_CONST,      1, LineM1, 2,  0, 0, 0, "Bias"),
			KerArg("Out",      KerArgSpace(1, T0), F|O_OUT|O_DB,                     ColO, LineM1, 2,  0, 0, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In1", 3, LineM1, ColM1, 2);
		AddKernelFloatArgDim(Name, "In2", 4, LineM2, Height, Width, 2);
		AddKernelFloatArgDim(Name, "Bias", 2, LineO, 2);
		AddKernelFloatArgDim(Name, "Out", 3, LineO, ColO, 2);

		AT_PrepareForTest(Name,
			  	(v4s) {2, 2, 2, 2},
			  	(v4s) {0,0,0,0},
			  	ColM1, LineM1, ColM2, LineM2,
			  	0,0, Scx,Scy, 0,0, (v4s) Width,
			  	0,0, 0,0, 0,0, (v4s) Height,
			  	KernelOper,
			  	0, 0);
	}
	return (Kernel!=0);

}

