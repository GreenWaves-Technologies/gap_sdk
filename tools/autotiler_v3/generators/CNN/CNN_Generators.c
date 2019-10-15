#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
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

void LoadCNNLibrary()

{
	/* Templates for Features and coefficients on 16 bits */
	LibKernelTemplate("KerSetBias_fp_T",
                  CArgs(6,
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("unsigned short int", "Norm")
			)
	);
	LibKernelTemplate("KerConv_fp_T",
                  CArgs(20,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int * __restrict__", "Filter"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("short int", "TotalInFeatures"),
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
	LibKernelTemplate("KerConv_DP_fp_T",
                  CArgs(20,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int * __restrict__", "Filter"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("short int", "TotalInFeatures"),
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
	LibKernelTemplate("KerDP_fp_T",
		CArgs(6,
			TCArg("int * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned short int", "Norm"),
			TCArg("unsigned short int", "InFeatures")
		)
	);
	LibKernelTemplate("KerReLUPool_fp_T",
                  CArgs(16,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned char", "M"),
			TCArg("unsigned char", "S"),
			TCArg("unsigned char", "Orientation"),
			TCArg("unsigned char", "Oper"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "My"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
			)
	);
	LibKernelTemplate("KerLinearLayerReLU_fp_T",
                  CArgs(10,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "InSize"),
			TCArg("unsigned short int", "TotalInSize"),
			TCArg("unsigned short int", "OutSize"),
			TCArg("short int * __restrict__", "Filter"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "DoReLU")
			)
	);
	LibKernelTemplate("KerMat3_fp_T",
                  CArgs(6,
			TCArg("short int *__restrict__", "In1"),
			TCArg("short int *__restrict__", "In2"),
			TCArg("short int *__restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "N")
			)
	);
	LibKernelTemplate("KerMatMul_fp_T",
                  CArgs(17,
			TCArg("short int * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned int", "OutFirstCol"),
			TCArg("short int * __restrict__", "BufferColIn2"),
			TCArg("int", "OutLowBound"),
			TCArg("int", "OutUpBound"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
			)
	);
	LibKernelTemplate("KerMatMul_fpd_fp_T",
                  CArgs(17,
			TCArg("short int * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("int * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned short int", "OutFirstCol"),
			TCArg("short int * __restrict__", "BufferColIn2"),
			TCArg("int", "OutLowBound"),
			TCArg("int", "OutUpBound"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
			)
	);
	LibKernelTemplate("KerSoftMax_fp_T",
                  CArgs(4,
			TCArg("short int *__restrict__", "In"),
			TCArg("unsigned short int", "N"),
			TCArg("unsigned short int", "Norm"),
			TCArg("short int *__restrict__", "Out")
			)
	);

	/* Templates for Features and coefficients on 8 bits */
	LibKernelTemplate("KerSetBias_fps_T",
                  CArgs(6,
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("unsigned short int", "Norm")
			)
	);
	LibKernelTemplate("KerConv_fps_T",
                  CArgs(20,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("short int", "TotalInFeatures"),
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
	LibKernelTemplate("KerConv_DP_fps_T",
                  CArgs(20,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "InFeatures"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("short int", "TotalInFeatures"),
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
	LibKernelTemplate("KerDP_fps_T",
		CArgs(6,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "Norm"),
			TCArg("unsigned short int", "InFeatures")
		)
	);
	LibKernelTemplate("KerReLUPool_fps_T",
                  CArgs(16,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "UsedW"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "UsedH"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("v4s", "Pad"),
			TCArg("unsigned char", "M"),
			TCArg("unsigned char", "S"),
			TCArg("unsigned char", "Orientation"),
			TCArg("unsigned char", "Oper"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "My"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
			)
	);
	LibKernelTemplate("KerLinearLayerReLU_fps_T",
                  CArgs(10,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned short int", "InSize"),
			TCArg("unsigned short int", "TotalInSize"),
			TCArg("unsigned short int", "OutSize"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "DoReLU")
			)
	);
	LibKernelTemplate("KerMat3_fps_T",
                  CArgs(6,
			TCArg("signed char *__restrict__", "In1"),
			TCArg("signed char *__restrict__", "In2"),
			TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "N")
			)
	);
	LibKernelTemplate("KerMatMul_fps_T",
                  CArgs(17,
			TCArg("signed char * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("signed char * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned short int", "OutFirstCol"),
			TCArg("signed char * __restrict__", "BufferColIn2"),
			TCArg("int", "OutLowBound"),
			TCArg("int", "OutUpBound"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
			)
	);
	LibKernelTemplate("KerMatMul_fp_fps_T",
                  CArgs(17,
			TCArg("signed char * __restrict__", "In1"),
			TCArg("unsigned short int", "W_In1"),
			TCArg("unsigned short int", "H_In1"),
			TCArg("signed char * __restrict__", "In2"),
			TCArg("unsigned short int", "W_In2"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned short int", "W_Out"),
			TCArg("unsigned short int", "OutFirstCol"),
			TCArg("signed char * __restrict__", "BufferColIn2"),
			TCArg("int", "OutLowBound"),
			TCArg("int", "OutUpBound"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
			)
	);
	LibKernelTemplate("KerSoftMax_fps_T",
                  CArgs(4,
			TCArg("signed char *__restrict__", "In"),
			TCArg("unsigned short int", "N"),
			TCArg("unsigned short int", "Norm"),
			TCArg("short int *__restrict__", "Out")
			)
	);

	/* Templates for Features and coefficients on hybrid formats */
	LibKernelTemplate("KerSetNormedBias_fp_fps_T",
                  CArgs(6,
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("signed char * __restrict__", "Bias"),
			TCArg("unsigned char", "Norm")
			)
	);
	LibKernelTemplate("KerSetNormedBias_fpd_fp_T",
                  CArgs(6,
			TCArg("int * __restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned short int", "OutFeatures"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("unsigned char", "Norm")
			)
	);
	LibKernelTemplate("KerLinearLayerReLU_fp_fps_fp_T",
                  CArgs(10,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "InSize"),
			TCArg("unsigned short int", "TotalInSize"),
			TCArg("unsigned short int", "OutSize"),
			TCArg("signed char * __restrict__", "Filter"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("short int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "DoReLU")
			)
	);
	LibKernelTemplate("KerLinearLayerReLU_fp_fp_fpd_T",
                  CArgs(10,
			TCArg("short int * __restrict__", "In"),
			TCArg("unsigned short int", "InSize"),
			TCArg("unsigned short int", "TotalInSize"),
			TCArg("unsigned short int", "OutSize"),
			TCArg("short int * __restrict__", "Filter"),
			TCArg("short int * __restrict__", "Bias"),
			TCArg("int * __restrict__", "Out"),
			TCArg("unsigned char", "Norm"),
			TCArg("unsigned char", "NormBias"),
			TCArg("unsigned char", "DoReLU")
			)
	);

	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 16 bits. Kernels for multiple output features evaluated in parallel */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerParSetBias_fp", CALL_PARALLEL, 0, "KerSetBias_fp_T",              	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(2,0,0,2), 0,0,0,0,0,0));
        // LibKernel("KerParSetBias_DP_fp", CALL_PARALLEL, 0, "KerSetNormedBias_fpd_fp_T", 	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(2,0,0,4), 0,0,0,0,0,0));

	/* Regular Convolutions */
        LibKernel("KerParConv1x1Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T", 			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,3,1,1,1,2));
        
	LibKernel("KerParConv3x3Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,-1,-2));

        LibKernel("KerParConv5x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(2,2,2,2), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerParConvDW1x1Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,3,1,1,1,2));
        
	LibKernel("KerParConvDW3x3Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(2,2,2,2), -1,-1,-1,-1,-1,-1));

	/* Convolutions with double precision output */
        LibKernel("KerParConv1x1Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,3,1,1,1,2));

        LibKernel("KerParConv3x3Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 3,3,1,1,-1,-2));

        LibKernel("KerParConv5x1Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), 5,5,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(2,2,2,4), -1,-1,-1,-1,-1,-1));

	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerParPool2x2Stride2_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,0,0,2), 2,2,1,1,2,2));
        LibKernel("KerParPoolNxNStrideS_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,0,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,0,0,2), -1,-1,1,1,-1,-1));

	/* Linear Rectification (ReLU) */
        LibKernel("KerParReLU_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",			CNN_Match(CNN_OperList(1, KOP_RELU), 0, 1, CNN_Type(2,0,0,2), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
	LibKernel("KerParLinearLayerReLU_fp", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,2,2,2), 0,0,0,0,0,0));

	/* Matrix Algebra */
	LibKernel("KerParMatAdd_fp", CALL_PARALLEL, 0, "KerMat3_fp_T",				CNN_Match(CNN_OperList(1, KOP_MATADD), 0, 1, CNN_Type(2,2,0,2), 0,0,0,0,0,0));
	LibKernel("KerParMatAddReLU_fp", CALL_PARALLEL, 0, "KerMat3_fp_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), CNN_OperList(1, KOP_RELU), 1, CNN_Type(2,2,0,2), 0,0,0,0,0,0));

	LibKernel("KerParMatMul_fp", CALL_PARALLEL, 0, "KerMatMul_fp_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(2,2,2,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSxSy_fp", CALL_PARALLEL, 0, "KerMatMul_fp_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(2,2,2,2), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMul_fpd_fp", CALL_PARALLEL, 0, "KerMatMul_fpd_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(2,2,4,2), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSxSy_fpd_fp", CALL_PARALLEL, 0, "KerMatMul_fpd_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(2,2,4,2), 0,0,0,0,-1,-1));

	/* SoftMax */
	LibKernel("KerParSoftMax_fp", CALL_PARALLEL, 0, "KerSoftMax_fp_T",			CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, 0, CNN_Type(2,0,0,2), 0,0,0,0,0,0));


	/****************************************************************************************************************/
	/* Kernels for Features and coefficients on 16 bits. Kernels for a single feature evaluated in parallel         */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerSetBias_fp", CALL_PARALLEL, 0, "KerSetBias_fp_T",				CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,2), 0,0,0,0,0,0));
        // LibKernel("KerSetBias_DP_fp", CALL_PARALLEL, 0, "KerSetNormedBias_fpd_fp_T",		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(2,0,0,4), 0,0,0,0,0,0));

	/* Convolutions */
        LibKernel("KerConv1x1Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,-1,-2));

        LibKernel("KerConvNxNStrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(2,2,2,2), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerConvDW1x1Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), 5,5,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideS_fp", CALL_PARALLEL, 0, "KerConv_fp_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSy_fp", CALL_PARALLEL, 0, "KerConv_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(2,2,2,2), -1,-1,-1,-1,-1,-1));

	/* Convolutions with double precision output */
        LibKernel("KerConv1x1Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), 5,5,1,1,-1,-2));

        LibKernel("KerConvNxNStrideS_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_DP_fp", CALL_PARALLEL, 0, "KerConv_DP_fp_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(2,2,2,4), -1,-1,-1,-1,-1,-1));

	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerPool2x2Stride2_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",			CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,0,0,2), 2,2,1,1,2,2));
        LibKernel("KerPoolNxNStrideS_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",			CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,0,0,2), -1,-2,1,1,-1,-2));
        LibKernel("KerPoolNxMStrideSxSy_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,0,0,2), -1,-1,1,1,-1,-1));

	/* Linear Rectification (ReLU) */
        LibKernel("KerReLU_fp", CALL_PARALLEL, 0, "KerReLUPool_fp_T",				CNN_Match(CNN_OperList(1, KOP_RELU), 0, 0, CNN_Type(2,0,0,2), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
	LibKernel("KerLinearLayerReLU_fp", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,2,2,2), 0,0,0,0,0,0));

	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 8 bits. Kernels for multiple output features evaluated in parallel  */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerParSetBias_fps", CALL_PARALLEL, 0, "KerSetBias_fps_T",             	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(1,0,0,1), 0,0,0,0,0,0));
        // LibKernel("KerParSetBias_DP_fps", CALL_PARALLEL, 0, "KerSetNormedBias_fp_fps_T", 	CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(1,0,0,2), 0,0,0,0,0,0));

	/* Convolutions */
        LibKernel("KerParConv1x1Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T", 		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,3,1,1,1,2));

        LibKernel("KerParConv3x3Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,-1,-2));

        LibKernel("KerParConv5x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,1), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerParConvDW1x1Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T", 		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,1,1));
        LibKernel("KerParConvDW1x1Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,2,2));
        LibKernel("KerParConvDW1x1StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,1,1,1,-1,-2));

        LibKernel("KerParConvDW3x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 3,1,1,1,1,1));
        LibKernel("KerParConvDW3x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 3,1,1,1,2,1));
        LibKernel("KerParConvDW1x3Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,3,1,1,1,1));
        LibKernel("KerParConvDW1x3Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,3,1,1,1,2));

        LibKernel("KerParConvDW3x3Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,1,1));
        LibKernel("KerParConvDW3x3Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,2,2));
        LibKernel("KerParConvDW3x3StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 3,3,1,1,-1,-2));

        LibKernel("KerParConvDW5x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 5,1,1,1,1,1));
        LibKernel("KerParConvDW5x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 5,1,1,1,2,1));
        LibKernel("KerParConvDW1x5Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,5,1,1,1,1));
        LibKernel("KerParConvDW1x5Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 1,5,1,1,1,2));

        LibKernel("KerParConvDW5x5Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,1,1));
        LibKernel("KerParConvDW5x5Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,2,2));
        LibKernel("KerParConvDW5x5StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), 5,5,1,1,-1,-2));

        LibKernel("KerParConvDWNxNStrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvDWNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvDWNxMDxDyStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 1, CNN_Type(1,1,1,1), -1,-1,-1,-1,-1,-1));

	/* Convolutions with double precision output */
        LibKernel("KerParConv1x1Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,1,1,1,1,1));
        LibKernel("KerParConv1x1Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,1,1,1,2,2));
        LibKernel("KerParConv1x1StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,1,1,1,-1,-2));

        LibKernel("KerParConv3x1Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 3,1,1,1,1,1));
        LibKernel("KerParConv3x1Stride2x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 3,1,1,1,2,1));
        LibKernel("KerParConv1x3Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,3,1,1,1,1));
        LibKernel("KerParConv1x3Stride1x2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,3,1,1,1,2));

        LibKernel("KerParConv3x3Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 3,3,1,1,1,1));
        LibKernel("KerParConv3x3Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 3,3,1,1,2,2));
        LibKernel("KerParConv3x3StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 3,3,1,1,-1,-2));

        LibKernel("KerParConv5x1Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 5,1,1,1,1,1));
        LibKernel("KerParConv5x1Stride2x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 5,1,1,1,2,1));
        LibKernel("KerParConv1x5Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,5,1,1,1,1));
        LibKernel("KerParConv1x5Stride1x2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 1,5,1,1,1,2));

        LibKernel("KerParConv5x5Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 5,5,1,1,1,1));
        LibKernel("KerParConv5x5Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 5,5,1,1,2,2));
        LibKernel("KerParConv5x5StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), 5,5,1,1,-1,-2));

        LibKernel("KerParConvNxNStrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), -1,-2,1,1,-1,-2));
        LibKernel("KerParConvNxMStrideSxSy_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), -1,-1,1,1,-1,-1));

        LibKernel("KerParConvNxMDxDyStrideSxSy_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 1, CNN_Type(1,1,1,2), -1,-1,-1,-1,-1,-1));


	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerParPool2x2Stride2_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(1,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerParPoolNxNStrideS_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(1,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerParPoolNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(1,0,0,1), -1,-1,1,1,-1,-1));

	/* Linear Rectification (ReLU) */
        LibKernel("KerParReLU_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",			CNN_Match(CNN_OperList(1, KOP_RELU), 0, 1, CNN_Type(1,0,0,1), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
	LibKernel("KerParLinearLayerReLU_fps", CALL_PARALLEL, 0, "KerLinearLayerReLU_fps_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(1,1,1,1), 0,0,0,0,0,0));

	/* Matrix Algebra */
	LibKernel("KerParMatAdd_fps", CALL_PARALLEL, 0, "KerMat3_fps_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), 0, 1, CNN_Type(1,1,0,1), 0,0,0,0,0,0));
	LibKernel("KerParMatAddReLU_fps", CALL_PARALLEL, 0, "KerMat3_fps_T",			CNN_Match(CNN_OperList(1, KOP_MATADD), 0, 1, CNN_Type(1,1,0,1), 0,0,0,0,0,0));

	LibKernel("KerParMatMul_fps", CALL_PARALLEL, 0, "KerMatMul_fps_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(1,1,1,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSxSy_fps", CALL_PARALLEL, 0, "KerMatMul_fps_T",			CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(1,1,1,1), 0,0,0,0,-1,-1));
	LibKernel("KerParMatMul_fp_fps", CALL_PARALLEL, 0, "KerMatMul_fp_fps_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(1,1,2,1), 0,0,0,0,1,1));
	LibKernel("KerParMatMulSxSy_fp_fps", CALL_PARALLEL, 0, "KerMatMul_fp_fps_T",		CNN_Match(CNN_OperList(1, KOP_MATMUL), 0, 1, CNN_Type(1,1,2,1), 0,0,0,0,-1,-1));

	/* SoftMax */
	LibKernel("KerParSoftMax_fps", CALL_PARALLEL, 0, "KerSoftMax_fps_T",			CNN_Match(CNN_OperList(1, KOP_SOFTMAX), 0, 0, CNN_Type(1,0,0,2), 0,0,0,0,0,0));

	/****************************************************************************************************************/
	/* Kernels for features and coefficients on 8 bits. Kernels for a single feature evaluated in parallel          */
	/****************************************************************************************************************/

	/* Bias setting */
        LibKernel("KerSetBias_fps", CALL_PARALLEL, 0, "KerSetBias_fps_T",			CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(1,0,0,1), 0,0,0,0,0,0));
        // LibKernel("KerSetBias_DP_fps", CALL_PARALLEL, 0, "KerSetNormedBias_fp_fps_T",		CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 0, CNN_Type(1,0,0,2), 0,0,0,0,0,0));

	/* Convolutions */
        LibKernel("KerConv1x1Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,-1,-2));

        LibKernel("KerConvNxNStrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 0, CNN_Type(1,1,1,1), -1,-1,-1,-1,-1,-1));

	/* Depth Wise Convolutions */
        LibKernel("KerConvDW1x1Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,1,1));
        LibKernel("KerConvDW1x1Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,2,2));
        LibKernel("KerConvDW1x1StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,1,1,1,-1,-2));

        LibKernel("KerConvDW3x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 3,1,1,1,1,1));
        LibKernel("KerConvDW3x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 3,1,1,1,2,1));
        LibKernel("KerConvDW1x3Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,3,1,1,1,1));
        LibKernel("KerConvDW1x3Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,3,1,1,1,2));

        LibKernel("KerConvDW3x3Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,2,2));
        LibKernel("KerConvDW3x3StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 3,3,1,1,-1,-2));

        LibKernel("KerConvDW5x1Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 5,1,1,1,1,1));
        LibKernel("KerConvDW5x1Stride2x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 5,1,1,1,2,1));
        LibKernel("KerConvDW1x5Stride1x1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,5,1,1,1,1));
        LibKernel("KerConvDW1x5Stride1x2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 1,5,1,1,1,2));

        LibKernel("KerConvDW5x5Stride1_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,1,1));
        LibKernel("KerConvDW5x5Stride2_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,2,2));
        LibKernel("KerConvDW5x5StrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), 5,5,1,1,-1,-2));

        LibKernel("KerConvDWNxNStrideS_fps", CALL_PARALLEL, 0, "KerConv_fps_T",			CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), -1,-2,1,1,-1,-2));
        LibKernel("KerConvDWNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), -1,-1,1,1,-1,-1));

        LibKernel("KerConvDWNxMDxDyStrideSxSy_fps", CALL_PARALLEL, 0, "KerConv_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,1,1), -1,-1,-1,-1,-1,-1));

	/* Convolutions with double precision output */
        LibKernel("KerConv1x1Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,1,1,1,1,1));
        LibKernel("KerConv1x1Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,1,1,1,2,2));
        LibKernel("KerConv1x1StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,1,1,1,-1,-2));

        LibKernel("KerConv3x1Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 3,1,1,1,1,1));
        LibKernel("KerConv3x1Stride2x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 3,1,1,1,2,1));
        LibKernel("KerConv1x3Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,3,1,1,1,1));
        LibKernel("KerConv1x3Stride1x2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,3,1,1,1,2));

        LibKernel("KerConv3x3Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 3,3,1,1,1,1));
        LibKernel("KerConv3x3Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 3,3,1,1,2,2));
        LibKernel("KerConv3x3StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 3,3,1,1,-1,-2));

        LibKernel("KerConv5x1Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 5,1,1,1,1,1));
        LibKernel("KerConv5x1Stride2x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 5,1,1,1,2,1));
        LibKernel("KerConv1x5Stride1x1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,5,1,1,1,1));
        LibKernel("KerConv1x5Stride1x2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 1,5,1,1,1,2));

        LibKernel("KerConv5x5Stride1_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 5,5,1,1,1,1));
        LibKernel("KerConv5x5Stride2_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 5,5,1,1,2,2));
        LibKernel("KerConv5x5StrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), 5,5,1,1,-1,-2));

        LibKernel("KerConvNxNStrideS_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), -1,-2,1,1,-1,-2));
        LibKernel("KerConvNxMStrideSxSy_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",		CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), -1,-1,1,1,-1,-1));

        LibKernel("KerConvNxMDxDyStrideSxSy_DP_fps", CALL_PARALLEL, 0, "KerConv_DP_fps_T",	CNN_Match(CNN_OperList(1, KOP_CONV_DP), 0, 0, CNN_Type(1,1,1,2), -1,-1,-1,-1,-1,-1));

	/* Pooling (Max or Avg) followed by an optional ReLU */
        LibKernel("KerPool2x2Stride2_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(1,0,0,1), 2,2,1,1,2,2));
        LibKernel("KerPoolNxNStrideS_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(1,0,0,1), -1,-2,1,1,-1,-2));
        LibKernel("KerPoolNxMStrideSxSy_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",		CNN_Match(CNN_OperList(2, KOP_MAXPOOL, KOP_AVGPOOL), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(1,0,0,1), -1,-1,1,1,-1,-1));

	/* Linear Rectification (ReLU) */
        LibKernel("KerReLU_fps", CALL_PARALLEL, 0, "KerReLUPool_fps_T",				CNN_Match(CNN_OperList(1, KOP_RELU), 0, 0, CNN_Type(1,0,0,1), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
	LibKernel("KerLinearLayerReLU_fps", CALL_PARALLEL, 0, "KerLinearLayerReLU_fps_T", 	CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(1,1,1,1), 0,0,0,0,0,0));





	/****************************************************************************************************************/
	/* Kernels for double precision reduction to single precision                                                   */
	/****************************************************************************************************************/

	LibKernel("KerDP_fp", CALL_PARALLEL, 0, "KerDP_fp_T",					CNN_Match(CNN_OperList(1, KOP_DP_REDUCT),    0, -1, CNN_Type(4,0,0,2), 0,0,0,0,0,0));
	LibKernel("KerDP_IO_fp", CALL_PARALLEL, 0, "KerDP_fp_T",				CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), 0, -1, CNN_Type(4,0,0,2), 0,0,0,0,0,0));

	LibKernel("KerDP_fps", CALL_PARALLEL, 0, "KerDP_fps_T",					CNN_Match(CNN_OperList(1, KOP_DP_REDUCT),    0, -1, CNN_Type(2,0,0,1), 0,0,0,0,0,0));
	LibKernel("KerDP_IO_fps", CALL_PARALLEL, 0, "KerDP_fps_T",				CNN_Match(CNN_OperList(1, KOP_DP_REDUCT_IO), 0, -1, CNN_Type(2,0,0,1), 0,0,0,0,0,0));

	/****************************************************************************************************************/
	/* Kernels for features/coeffs on different sizes. Kernels for multiple output features evaluated in parallel   */
	/****************************************************************************************************************/

	/* Bias setting */
	LibKernel("KerParSetNormedBias_fp_fps", CALL_PARALLEL, 0, "KerSetNormedBias_fp_fps_T",	CNN_Match(CNN_OperList(1, KOP_SETBIAS),    0, 1, CNN_Type(1,0,0,2), 0,0,0,0,0,0));
	LibKernel("KerParSetNormedBias_fpd_fp", CALL_PARALLEL, 0, "KerSetNormedBias_fpd_fp_T",	CNN_Match(CNN_OperList(1, KOP_SETBIAS),    0, 1, CNN_Type(2,0,0,4), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
#ifdef UNSAFE_ARITH
	/* Warning channel parallel evaluation leads to major numerical error in case a given feature is evaluated in multiple rounds with norm and clipping in between the rounds */
	LibKernel("KerParLinearLayerReLU_fp_fps_fp", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_fps_fp_T",
												CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,1,2,2), 0,0,0,0,0,0));

	LibKernel("KerParLinearLayerReLU_fp_fp_fpd", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_fp_fpd_T",
												CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 1, CNN_Type(2,2,2,4), 0,0,0,0,0,0));
#endif


	/****************************************************************************************************************/
	/* Kernels for features/coeffs on different sizes. Kernels for a single feature evaluated in parallel.          */
	/****************************************************************************************************************/

	/* Bias setting */
	LibKernel("KerSetNormedBias_fp_fps", CALL_PARALLEL, 0, "KerSetNormedBias_fp_fps_T",	CNN_Match(CNN_OperList(1, KOP_SETBIAS),    0, 0, CNN_Type(1,0,0,2), 0,0,0,0,0,0));
	LibKernel("KerSetNormedBias_fpd_fp", CALL_PARALLEL, 0, "KerSetNormedBias_fpd_fp_T",	CNN_Match(CNN_OperList(1, KOP_SETBIAS),    0, 0, CNN_Type(2,0,0,4), 0,0,0,0,0,0));

	/* Linear layer followed by an optional ReLU */
	LibKernel("KerLinearLayerReLU_fp_fps_fp", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_fps_fp_T",
												CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,1,2,2), 0,0,0,0,0,0));

	LibKernel("KerLinearLayerReLU_fp_fp_fpd", CALL_PARALLEL, 0, "KerLinearLayerReLU_fp_fp_fpd_T",
												CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(2, KOP_RELU, KOP_NONE), 0, CNN_Type(2,2,2,4), 0,0,0,0,0,0));
}

static int EncodePoolOperation(KernelOper_T PoolOper, KernelOper_T ReLUOper)

{
	int Pool=0;
	int ReLU = (ReLUOper==KOP_RELU)?1:0;

	switch (PoolOper) {
		case KOP_MAXPOOL: Pool = 0; break;
		case KOP_AVGPOOL: Pool = 1; break;
	}
	return ((Pool<<1)|ReLU);
}

int Gcd(int a, int b)

{
	int x, y, z;

	x = Abs (a); y = Abs (b);
	while (x > 0) {
		z = y % x; y = x; x = z;
	}
	return y;
}

int Scm(int a, int b)

{
        return ((a*b)/Gcd(a,b));
}

/*********************************************************************************************************************************************************************
 	Generators for Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

		In_DataSize:	1: byte, 2: half word, 4: word
		Filter_DataSize:1: byte, 2: half word, 4: word
		Bias_DataSize:	1: byte, 2: half word, 4: word
		Out_DataSize:	1: byte, 2: half word, 4: word

		In_InL3:	0: In is in L2, 1: In is in L3 memory
		Filter_InL3:	0: Filter is in L2, 1: Filter is in L3 memory
		Bias_InL3:	0: Bias is in L2, 1: Bias is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

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

		ReLUOper:	Optional linaer rectification to be performed as a final step, KOP_RELU or KOP_NONE


	CNN_ConvolutionPoolReLU
	
*********************************************************************************************************************************************************************/

void CNN_ConvolutionPoolReLU(
			char         *Name,

			CNN_GenControl_T *Ctrl,

			int In_DataSize,
			int Filter_DataSize,
			int Bias_DataSize,
			int Out_DataSize,

			int In_InL3,		// 1 if In comes from L3, 0 if it comes from L2
			int Filter_InL3,
			int Bias_InL3,
			int Out_InL3,

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

			KernelOper_T ReLUOper
			)

{
	int ParFeat = 1; 
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int InL3 = In_InL3?O_L2DB:0;
	int OutL3 = Out_InL3?O_L2DB:0;
	int FilterL3 = Filter_InL3?O_L2DB:0;
	int BiasL3 = Bias_InL3?O_L2DB:0;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->ForceDPconv != -1) {
			if (Ctrl->ForceDPconv) {
				if (ConvOper==KOP_CONV) ConvOper=KOP_CONV_DP;
			} else if (Ctrl->ForceDPconv==0) {
				if (ConvOper==KOP_CONV_DP) ConvOper=KOP_CONV;
			}
		}
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
	}
        int OverlapCx = (Dcx*(Fcx-1)+1) + Scx*((Dpx*(Fpx-1)+1)-Spx-1), OverlapCy = (Dcy*(Fcy-1)+1)+ Scy*((Dpy*(Fpy-1)+1)-Spy-1);
	int OverlapC = (TileOrientation==TILE_HOR)?OverlapCy:OverlapCx;
        int OverlapPx = (Dpx*(Fpx-1)+1)-Spx, OverlapPy = (Dpy*(Fpy-1)+1)-Spy;
	int OverlapP = (TileOrientation==TILE_HOR)?OverlapPy:OverlapPx;
	// int TileCons = (TileOrientation==TILE_HOR)?(Scy * Spy):(Scx * Spx);
	int TileCons = (TileOrientation==TILE_HOR)?Scm(Scy, Spy):Scm(Scx, Spx);
	int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0;
        int PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0};
        v4s PadInc  = (v4s){0,0,0,0};
        v4s PadIncT = (v4s){0,0,0,0};
	char *ConvKerName=0, *PoolKerName=0, *ReLUKerName=0, *SetBiasKerName=0, *DPReductionKerName=0;
	int ConvDoReLU=0, PoolDoReLU=0, ConvDP=(ConvOper==KOP_CONV_DP), DWConv=(ConvOper==KOP_CONV_DW);
	int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy;
	int NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
	int          ConvOut_DataSize = ConvDP?(2*Out_DataSize):Out_DataSize;
	KernelOper_T KernelOper = CNN_CompositeKernel(ConvOper, PoolOper, ReLUOper);
	int Os=(DWConv?D0:D1);
	int UsedWidth, UsedHeight, UsedWc, UsedHc;
	int InTileCons = 1;
	unsigned int LayerOp = 0;
	int Log=1;

	if (ReLUOper) {
		if (PoolOper) PoolDoReLU=1; else ConvDoReLU=1;
	}
	if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);
	SetBiasKerName = CNN_FindMatchingKernel(KOP_SETBIAS, KOP_NONE, ParFeat, Bias_DataSize, 0, 0, ConvOut_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
	if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Can't find a matching Set Bias basic kernel", Name);
	ConvKerName = CNN_FindMatchingKernel(ConvOper, KOP_NONE, ParFeat,
					 In_DataSize, Filter_DataSize, 0 /* Bias_DataSize */, ConvOut_DataSize,
					 Fcx, Fcy, Dcx, Dcy, Scx, Scy,
					 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
	if (ConvKerName==0) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Can't find a matching Convolution basic kernel", Name);
	if (ConvDP) {
		DPReductionKerName = CNN_FindMatchingKernel((PoolOper||ConvDoReLU)?KOP_DP_REDUCT_IO:KOP_DP_REDUCT, KOP_NONE, ParFeat, ConvOut_DataSize, 0, 0, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (DPReductionKerName==0) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Can't find a matching double precision to single precision basic kernel", Name);
	}
	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernel(PoolOper, ReLUOper, ParFeat,
						Out_DataSize, 0, 0, Out_DataSize,
						Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						&NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ReLUOper?"with linear rectification":"");
	}
	if (ReLUOper && (PoolOper==KOP_NONE)) {
		ReLUKerName = CNN_FindMatchingKernel(KOP_RELU, KOP_NONE, ParFeat, Out_DataSize, 0, 0, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ReLUKerName==0) GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, Can't find a matching Linear Rectification basic kernel", Name);
	}

        if (ConvPad) {
                PadCw = ((Width%Scx)  == 0)?Max((Dcx*(Fcx-1)+1)-Scx, 0):Max((Dcx*(Fcx-1)+1) -(Width%Scx), 0);
		PadCh = ((Height%Scy) == 0)?Max((Dcy*(Fcy-1)+1)-Scy, 0):Max((Dcy*(Fcy-1)+1) -(Height%Scy), 0);
        }
	switch (PadType) {
		case PAD_LEFT: PadInc = (v4s) {PadCw, 0, PadCh, 0}; break;
		case PAD_RIGHT: PadInc = (v4s) {0, PadCw, 0, PadCh}; break;
		case PAD_BALANCED_LEFT: PadInc = (v4s) {PadCw-PadCw/2, PadCw/2, PadCh-PadCh/2, PadCh/2}; break;
		case PAD_BALANCED_RIGHT: PadInc = (v4s) {PadCw/2, PadCw-PadCw/2, PadCh/2, PadCh-PadCh/2}; break;
		default: GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, unknown padding type %d", Name, PadType);
	}

        Wc = (Width - (Dcx*(Fcx-1)+1)  + PadCw)/Scx + 1; Hc = (Height - (Dcy*(Fcy-1)+1) + PadCh)/Scy + 1;

        if (PoolOper && PoolPad) {
                PadPw = ((Wc%Spx) == 0)?Max((Dpx*(Fpx-1)+1)-Spx, 0):Max((Dpx*(Fpx-1)+1)-(Wc%Spx), 0);
		PadPh = ((Hc%Spy) == 0)?Max((Dpy*(Fpy-1)+1)-Spy, 0):Max((Dpy*(Fpy-1)+1)-(Hc%Spy), 0);
        }
	switch (PadType) {
		case PAD_LEFT: PadInp = (v4s) {PadPw, 0, PadPh, 0}; break;
		case PAD_RIGHT: PadInp = (v4s) {0, PadPw, 0, PadPh}; break;
		case PAD_BALANCED_LEFT: PadInp = (v4s) {PadPw-PadPw/2, PadPw/2, PadPh-PadPh/2, PadPh/2}; break;
		case PAD_BALANCED_RIGHT: PadInp = (v4s) {PadPw/2, PadPw-PadPw/2, PadPh/2, PadPh-PadPh/2}; break;
		default: GenTilingError("CNN_ConvolutionPoolReLU Kernel: %s, unknown padding type %d", Name, PadType);
	}
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

        Wo = (Wc - (Dpx*(Fpx-1)+1) + PadPw)/Spx + 1; Ho = (Hc - (Dpy*(Fpy-1)+1) + PadPh)/Spy + 1;

	UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];

	/* Re evaluate now that we know exactly what is used */
	PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
	PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
	UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
	Wc = UsedWc; Hc = UsedHc;

	/* Set a constraint on input tile width either for vectorization friendly dimension or for tile alignement sake */
	switch (Filter_DataSize) {
		case 1: InTileCons = 4; break;
		case 2:	InTileCons = 2; break;
		default: InTileCons = 1;
	}

	LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
	if (DWConv==0) LayerOp *= InFeat;
	if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
	if (ReLUOper) LayerOp += OutFeat*Wo*Ho;

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
		if (ReLUKerName) printf("%20s: %s\n", "ReLUKerName", ReLUKerName);
		
	}

        UserKernel(Name,
		ParFeat?
		(DWConv?
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
		KernelIterSpace(3, IterParSpace(D1, OutFeat, 8), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons))):
		(DWConv?
		KernelIterSpace(2, IterFixedSpace(D0, InFeat), IterTiledSpace(T0)):
		KernelIterSpace(3, IterFixedSpace(D1, OutFeat), IterTiledSpace(T0), IterFixedSpace(D0, InFeat))),
                TileOrientation,
                CArgs(5,
                      TCArg(CNN_ArgDataType(In_DataSize,1,1),     "In"),
                      TCArg(CNN_ArgDataType(Filter_DataSize,1,1), "Filter"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1),    "Out"),
                      TCArg("unsigned int", "Norm")
                     ),
                Calls(4,
			DWConv?AT_NO_CALL:
                        Call(SetBiasKerName, LOC_D0_PROLOG,
                                Bindings(6,
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE),		/* SetBias output tile */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_W),		/* SetBias output tile width */
                                        K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_H),		/* SetBias output tile height */
					ParFeat?
					K_ArgPar((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os): /* Number of output features in this tile */
					AT_IGNORE_ARG_BINDING,							/* Not relevant for non KerPar */
					K_Arg("Bias", KER_ARG_TILE),						/* SetBias Bias tile */
					ConvDP?C_Arg("Norm"):AT_IGNORE_ARG_BINDING)),				/* if double precision conv Norm factor to apply to bias */
                        Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
                                Bindings(20, 
					K_Arg("In", KER_ARG_TILE),						/* Conv input tile */
					K_Arg("In", KER_ARG_TILE_W),						/* Conv input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),					/* Conv input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),						/* Conv input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),					/* Conv input tile height, used part of it */
					ParFeat?
					K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0):				/* Number of input features in this tile */
					AT_IGNORE_ARG_BINDING,							/* Not relevant for non KerPar */
					(ParFeat)?
					K_ArgPar((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, DWConv?D0:D1):	/* Number of output features in this tile */
					AT_IGNORE_ARG_BINDING,							/* Not relevant for non KerPar */
					K_Arg("Filter", KER_ARG_TILE),						/* Conv filter */
					DWConv?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,		/* Conv Bias when Conv depth wise */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE),		/* Conv output */
					ConvDP?AT_IGNORE_ARG_BINDING:C_Arg("Norm"),				/* if not double precision conv Normalization */
					ParFeat?
					(DWConv?
					C_Arg("Norm"):								/* For Depth Wise convolution Norm factor for Bias */
					K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0)):			/* Total number of input features currently in L1 memory, argument promotion */
					C_Arg("Norm"),								/* For DW conv Norm factor for Bias */
					K_Arg("In", KER_ARG_TILE_PAD),						/* Conv Padding */
					Imm((TileOrientation==TILE_HOR)?1:0),					/* Orientation */
					NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,					/* Conv Fx */
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,					/* Conv Stridex */
					NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,					/* Conv Dx */
					NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,					/* Conv Fy */
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,					/* Conv Stridey */
					NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING					/* Conv Dy */
					)
			),
			ConvDP?
			Call(DPReductionKerName, LOC_D0_EPILOG,
				Bindings(6,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Double precision input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Double precision input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Double precision input tile height */
					K_Arg((PoolOper||ConvDoReLU)?"ConvOut":"Out", KER_ARG_TILE),		/* Single precision output tile, warning use IO kernel when In=Out */
					C_Arg("Norm"),								/* Normalization factor to go from double to single precision */
					ParFeat?
					K_ArgPar((PoolOper||ConvDoReLU)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):
					Imm(1)									/* Number of features */
				)
			):AT_NO_CALL,
			ConvDoReLU?
			Call(ReLUKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(16,
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE),		/* ReLU input tile */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_W),		/* ReLU input tile width */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_USEDW),		/* ReLU input tile width, used part of it */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_H),		/* ReLU input tile height */
					K_Arg((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_TILE_USEDH),		/* ReLU input tile height, used part of it */
					ParFeat?
					K_ArgPar((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):	/* Number of output features */
					AT_IGNORE_ARG_BINDING,							/* Not relevant for non KerPar */
					K_Arg("Out", KER_ARG_TILE),						/* ReLU output tile */
					AT_IGNORE_ARG_BINDING,							/* Pooling only,  Pad */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool Fx */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool Stridex */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool orientation */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool operation */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool Dx */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool Fy */
					AT_IGNORE_ARG_BINDING,							/* Pooling only, Pool Stridey */
					AT_IGNORE_ARG_BINDING							/* Pooling only, Pool Dy */
				)
			):(PoolOper?
			Call(PoolKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
				Bindings(16,
					K_Arg("ConvOut", KER_ARG_TILE),						/* Pooling input tile */
					K_Arg("ConvOut", KER_ARG_TILE_W),					/* Pooling input tile width */
					K_Arg("ConvOut", KER_ARG_TILE_USEDW),					/* Pooling input tile width, used part of it */
					K_Arg("ConvOut", KER_ARG_TILE_H),					/* Pooling input tile height */
					K_Arg("ConvOut", KER_ARG_TILE_USEDH),					/* Pooling input tile height, used part of it */
					ParFeat?
					K_ArgPar((PoolOper||ConvDP)?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os):	/* Number of output features in this tile */
					AT_IGNORE_ARG_BINDING,							/* Not relevant for non KerPar */
					K_Arg("Out", KER_ARG_TILE),						/* Pooling output tile */
					K_Arg("ConvOut", KER_ARG_TILE_PAD),					/* Pooling Pad */
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,					/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,					/* Pooling Stridex */
					Imm((TileOrientation==TILE_HOR)?1:0),					/* Pooling Orientation */
					Imm(EncodePoolOperation(PoolOper, ReLUOper)),				/* Pooling operation with optional ReLU */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,					/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,					/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,					/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING					/* Pooling Dy */
				)
			):AT_NO_CALL)
                     ),
                KerArgs(5,
                        KerArgP("In",        KerArgSpace(2,D0,T0), O_IN|O_DB|InL3,             Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, In_DataSize, 	       OverlapC, 0, TileCons, "In"),
			DWConv?
			KerArg ("Bias",      KerArgSpace(1,D0),    O_IN|O_DB|O_CONST|BiasL3,   1,     1,                       			      Bias_DataSize,           0,        0,        0, "Bias"):
			KerArg ("Bias",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|BiasL3,   1,     1,                       			      Bias_DataSize,           0,        0,        0, "Bias"),
			DWConv?
			KerArg ("Filter",    KerArgSpace(1,D0),    O_IN|O_DB|O_CONST|FilterL3, 1,     1,                       			      Fcx*Fcy*Filter_DataSize, 0,        0,        0, "Filter"):
			KerArg ("Filter",    KerArgSpace(2,D1,D0), O_IN|O_DB|O_CONST|FilterL3, 1,     1,                       			      Fcx*Fcy*Filter_DataSize, 0,        0,        0, "Filter"),
			DWConv?
                        KerArg ("Out",       KerArgSpace(2,D0,T0), O_OUT|O_DB|OutL3,          Wo,    Ho,                      			      Out_DataSize,            0,        0,        0, "Out"):
                        KerArg ("Out",       KerArgSpace(2,D1,T0), O_OUT|O_DB|OutL3,          Wo,    Ho,                      			      Out_DataSize,            0,        0,        0, "Out"),
			(PoolOper||ConvDP)?(ParFeat?
			(DWConv?
			KerArgP("ConvOut",   KerArgSpace(2,D0,T0), O_BUFF|O_ONETILE,          Wc,    Hc,     UsedWc, UsedHc, PadInp, PadInp,  	      ConvOut_DataSize,        OverlapP, 0,        0,  ""):
			KerArgP("ConvOut",   KerArgSpace(2,D1,T0), O_BUFF|O_ONETILE,          Wc,    Hc,     UsedWc, UsedHc, PadInp, PadInp,  	      ConvOut_DataSize,        OverlapP, 0,        0,  "")):
			KerArgP("ConvOut",   KerArgSpace(1,T0),    O_BUFF|O_ONETILE,          Wc,    Hc,     UsedWc, UsedHc, PadInp, PadInp,  	      ConvOut_DataSize,        OverlapP, 0,        0,  "")):
			AT_NO_KER_ARG

		)
        );
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);

	AT_PrepareForTest(Name,
			  (v4s) {In_DataSize, Filter_DataSize, Bias_DataSize, Out_DataSize},
			  (v4s) {InL3!=0,FilterL3!=0,BiasL3!=0,OutL3!=0},
			  InFeat, OutFeat, Width, Height,
			  Fcx, Fcy, Scx, Scy, Dcx, Dcy, PadInc, Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp, KernelOper,
			  (In_DataSize==1)?7:15, 0);
}

/*********************************************************************************************************************************************************************
 	Generators for Pooling (Max or Average) followed by an optional linear rectification (ReLU) or linear rectification only

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		In_DataSize:	1: byte, 2: half word, 4: word
		Out_DataSize:	1: byte, 2: half word, 4: word

		In_InL3:	0: In is in L2, 1: In is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

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

		ReLUOper	optional rectification to be applied after pooling, KOP_RELU or KOP_NONE

	Currently only homegeneous data size are supported (bytes and hald words)

	CNN_PoolReLU
		
*********************************************************************************************************************************************************************/

void CNN_PoolReLU(
			char *Name,

			CNN_GenControl_T *Ctrl,

			int In_DataSize,
			int Out_DataSize,

			int In_InL3,		// 1 if In comes from L3, 0 if it comes from L2
			int Out_InL3,

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

			KernelOper_T ReLUOper
			)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1;
	int InL3 = In_InL3?O_L2DB:0;
	int OutL3 = Out_InL3?O_L2DB:0;
	AT_PadType PadType = PAD_BALANCED_LEFT;
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
		if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
	}
        int OverlapPx = (Dpx*(Fpx-1)+1)-Spx, OverlapPy = (Dpy*(Fpy-1)+1)-Spy;
	int OverlapP = (TileOrientation==TILE_HOR)?OverlapPy:OverlapPx;
	int TileCons = (TileOrientation==TILE_HOR)?(Spy):(Spx);
	int NeedFpx=0, NeedFpy=0, NeedDpx=0, NeedDpy=0, NeedSpx=0, NeedSpy=0;
	int Wo, Ho;
	int UsedWidth, UsedHeight;
	int PadPw=0, PadPh=0;
	v4s PadInp = (v4s){0,0,0,0};
	char *PoolKerName=0, *ReLUKerName=0;
	KernelOper_T KernelOper = CNN_CompositeKernel(PoolOper, ReLUOper, KOP_NONE);
	unsigned int LayerOp = 0;
	int Log=1;

	if (InFeat != OutFeat) GenTilingError("CNN_PoolReLU Kernel: %s, InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);
	if (PoolOper!=KOP_NONE) {
		PoolKerName = CNN_FindMatchingKernel(PoolOper, ReLUOper, ParFeat,
						 In_DataSize, 0, 0, Out_DataSize,
						 Fpx, Fpy, Dpx, Dpy, Spx, Spy,
						 &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
		if (PoolKerName==0) GenTilingError("CNN_PoolReLU Kernel: %s, Can't find a matching Pooling %s basic kernel", Name, ReLUOper?"with linear rectification":"");
	}
	if (ReLUOper && (PoolOper==KOP_NONE)) {
		ReLUKerName = CNN_FindMatchingKernel(KOP_RELU, KOP_NONE, ParFeat, In_DataSize, 0, 0, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
		if (ReLUKerName==0) GenTilingError("CNN_PoolReLU Kernel: %s, Can't find a matching Linear Rectification basic kernel", Name);
	}

        if (PoolOper && PoolPad) {
                PadPw = ((Width%Spx)  == 0)?Max((Dpx*(Fpx-1)+1)-Spx, 0):Max((Dpx*(Fpx-1)+1)-(Width%Spx), 0);
		PadPh = ((Height%Spy) == 0)?Max((Dpy*(Fpy-1)+1)-Spy, 0):Max((Dpy*(Fpy-1)+1)-(Height%Spy), 0);
        }
	switch (PadType) {
		case PAD_LEFT: PadInp = (v4s) {PadPw, 0, PadPh, 0}; break;
		case PAD_RIGHT: PadInp = (v4s) {0, PadPw, 0, PadPh}; break;
		case PAD_BALANCED_LEFT: PadInp = (v4s) {PadPw-PadPw/2, PadPw/2, PadPh-PadPh/2, PadPh/2}; break;
		case PAD_BALANCED_RIGHT: PadInp = (v4s) {PadPw/2, PadPw-PadPw/2, PadPh/2, PadPh-PadPh/2}; break;
		default: GenTilingError("CNN_PoolReLU Kernel: %s, unknown padding type %d", Name, PadType);
	}

        Wo = (Width  - (Dpx*(Fpx-1)+1) + PadPw)/Spx + 1; Ho = (Height - (Dpy*(Fpy-1)+1) + PadPh)/Spy + 1;

	/* Re evaluate truly used width/height and update padding accordingly */
	UsedWidth = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHeight = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
	PadInp[1] = Max(0, PadInp[1]-(Width-UsedWidth)); PadInp[3] = Max(0, PadInp[3]-(Height-UsedHeight));

	if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
	if (ReLUOper) LayerOp += OutFeat*Wo*Ho;

	if (Log) {
        	printf("Pool => W: %d, Pad:[%d,%d] => Wo: %d\n", Width,  PadInp[0], PadInp[1], Wo);
        	printf("     => H: %d, Pad:[%d,%d] => Ho: %d\n", Height, PadInp[2], PadInp[3], Ho);
        	printf("OverlapP: %d\n", OverlapP);
        	printf("TileCons: %d\n", TileCons);
		printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
		if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
		if (ReLUKerName) printf("%20s: %s\n", "ReLUKerName", ReLUKerName);
	}

        UserKernel(Name,
		ParFeat?
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
		KernelIterSpace(2, IterFixedSpace(D0, InFeat), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(2,
                      TCArg(CNN_ArgDataType(In_DataSize,1,1), "In"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1), "Out")
                     ),
                Calls(1,
			PoolOper?
			Call(PoolKerName, LOC_LOOP,
				Bindings(16,
					K_Arg("In", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_W),
					K_Arg("In", KER_ARG_TILE_USEDW),
					K_Arg("In", KER_ARG_TILE_H),
					K_Arg("In", KER_ARG_TILE_USEDH),
					ParFeat?
					K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D0):		/* Number of output features in this tile */
					AT_IGNORE_ARG_BINDING,					/* Not relevant for non KerPar */
					K_Arg("Out", KER_ARG_TILE),
					K_Arg("In", KER_ARG_TILE_PAD),
					NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,			/* Pooling Fx */
					NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,			/* Pooling Stridex */
					Imm((TileOrientation==TILE_HOR)?1:0),			/* Pooling Orientation */
					Imm(EncodePoolOperation(PoolOper, ReLUOper)),		/* Pooling operation with optional ReLU */
					NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,			/* Pooling Dx */
					NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,			/* Pooling Fy */
					NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,			/* Pooling Stridey */
					NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING			/* Pooling Dy */
				)
			):
			Call(ReLUKerName, LOC_LOOP,
				Bindings(16,
					K_Arg("In", KER_ARG_TILE),				/* ReLU input tile */
					K_Arg("In", KER_ARG_TILE_W),				/* ReLU input tile width */
					K_Arg("In", KER_ARG_TILE_USEDW),			/* ReLU input tile width, used part of it */
					K_Arg("In", KER_ARG_TILE_H),				/* ReLU input tile height */
					K_Arg("In", KER_ARG_TILE_USEDH),			/* ReLU input tile height, used part of it */
					ParFeat?
					K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):		/* Number of output features */
					AT_IGNORE_ARG_BINDING,					/* Not relevant for non KerPar */
					K_Arg("Out", KER_ARG_TILE),				/* ReLU output tile */
					AT_IGNORE_ARG_BINDING,					/* Pooling only,  Pad */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool Fx */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool Stridex */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool orientation */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool operation */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool Dx */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool Fy */
					AT_IGNORE_ARG_BINDING,					/* Pooling only, Pool Stridey */
					AT_IGNORE_ARG_BINDING					/* Pooling only, Pool Dy */
				)
			)
                     ),
                KerArgs(2,
			KerArgP("In",     KerArgSpace(2,D0,T0), OBJ_IN_DB|InL3,   Width, Height, UsedWidth, UsedHeight, PadInp,PadInp, In_DataSize,  OverlapP,  0, TileCons, "In"),
                        KerArg ("Out",    KerArgSpace(2,D0,T0), OBJ_OUT_DB|OutL3, Wo,    Ho,                   		               Out_DataSize,        0,  0,        0, "Out")
		)
        );
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
	AT_PrepareForTest(Name,
			  (v4s) {In_DataSize, 0, 0, Out_DataSize},
			  (v4s) {InL3!=0,0,0,OutL3!=0},
			  InFeat, OutFeat, Width, Height,
			  0,0, 0,0, 0,0, (v4s) 0,
			  Fpx, Fpy, Spx, Spy, Dpx, Dpy, PadInp,
			  KernelOper,
			  (In_DataSize==1)?7:15, 0);
}

/*********************************************************************************************************************************************************************
 	Generators for Linear layers followed by an optional linear rectification (ReLU)

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

		In_DataSize:	1: byte, 2: half word, 4: word
		Filter_DataSize:1: byte, 2: half word, 4: word
		Bias_DataSize:	1: byte, 2: half word, 4: word
		Out_DataSize:	1: byte, 2: half word, 4: word

		In_InL3:	0: In is in L2, 1: In is in L3 memory
		Filter_InL3:	0: Filter is in L2, 1: Filter is in L3 memory
		Bias_InL3:	0: Bias is in L2, 1: Bias is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

		InDim:		Number of inputs
		OutDim:		Number of outputs

		KernelOper_T 	LinearOper	Should always be KOP_LINEAR
		KernelOper_T 	ReLUOper	KOP_RELU if ReLU has to be applied after Linear, KOP_NONE otherwise

	CNN_LinearReLU
		Input, Bias and Output are assumed to fit into shared L1 (Buffer In or Out) and Filter is tiled.
	
*********************************************************************************************************************************************************************/

void CNN_LinearReLU(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int Out_DataSize,

	int In_InL3,
	int Filter_InL3,
	int Bias_InL3,
	int Out_InL3,

	int InDim,
	int OutDim,

	KernelOper_T LinearOper,
	KernelOper_T ReLUOper
	)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 0;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	int InL3 = In_InL3?O_L2DB:0;
	int FilterL3 = Filter_InL3?O_L2DB:0;
	int BiasL3 = Bias_InL3?O_L2DB:0;
	int OutL3 = Out_InL3?O_L2DB:0;
	int NormedBias = !(In_DataSize==Filter_DataSize&&Filter_DataSize==Bias_DataSize&&Bias_DataSize==Out_DataSize);
	KernelOper_T KernelOper = CNN_CompositeKernel(LinearOper, ReLUOper, KOP_NONE);
	unsigned int LayerOp = 0;

	char *LinearKerName = CNN_FindMatchingKernel(LinearOper, ReLUOper, ParFeat, In_DataSize, Filter_DataSize, Bias_DataSize, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (LinearKerName==0) GenTilingError("CNN_LinearReLU Kernel: %s, Can't find a matching %s basic kernel", Name, ReLUOper?"with linear rectification":"");

	LayerOp += InDim*OutDim;
	if (ReLUOper) LayerOp += OutDim;
        UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(D0, 1), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(6,
                      TCArg(CNN_ArgDataType(In_DataSize,1,1),     "In"),
                      TCArg(CNN_ArgDataType(Filter_DataSize,1,1), "Filter"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1),    "Out"),
                      TCArg("unsigned int", "Norm"),
                      TCArg("unsigned int", "NormBias")
                     ),
                Calls(1,
			Call(LinearKerName, LOC_INNER_LOOP,
				Bindings(10,
					K_Arg("In", KER_ARG_TILE),				/* Input tile */
					K_Arg("In", KER_ARG_TILE_H),				/* Input tile size */
					AT_IGNORE_ARG_BINDING,					/* TotalInSize when Parallel Out Feature */
					K_Arg("Filter", KER_ARG_TILE_H),			/* Output tile size */
					K_Arg("Filter", KER_ARG_TILE),				/* Filter tile */
					K_Arg("Bias", KER_ARG_TILE),				/* Bias tile */
					K_Arg("Out", KER_ARG_TILE),				/* Output tile */
					C_Arg("Norm"),						/* Normalization factor to be used for sum of product */
					NormedBias?C_Arg("NormBias"):AT_IGNORE_ARG_BINDING,	/* Normalization factor to be used to adjust bias or filter */
					Imm(ReLUOper?1:0)					/* Optionnal ReLU to be performed after linear */
				)
			)
		),
                KerArgs(4,
                        KerArg("In",      KerArgSpace(1,D0), OBJ_IN_DB|InL3,                   1, InDim,  In_DataSize,     0, 0, 0, "In"),
                        KerArg("Filter",  KerArgSpace(1,T0), OBJ_IN_DB|O_CONST|FilterL3,   InDim, OutDim, Filter_DataSize, 0, 0, 0, "Filter"),
                        KerArg("Bias",    KerArgSpace(1,T0), OBJ_BUFFER_IN|O_CONST|BiasL3,     1, OutDim, Bias_DataSize,   0, 0, 0, "Bias"),
                        KerArg("Out",     KerArgSpace(1,T0), OBJ_BUFFER_OUT|OutL3,             1, OutDim, Out_DataSize,    0, 0, 0, "Out")
                        )
	);
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
	AT_PrepareForTest(Name,
			  (v4s) {In_DataSize, Filter_DataSize, Bias_DataSize, Out_DataSize},
			  (v4s) {InL3!=0,FilterL3!=0,BiasL3!=0,OutL3!=0},
			  InDim, OutDim, 1, 1,
			  0,0, 0,0, 0,0, (v4s) 0,
			  0,0, 0,0, 0,0, (v4s) 0,
			  KernelOper,
			  (In_DataSize==1)?7:15,
			  (Filter_DataSize==1)?7:15);
}

/*********************************************************************************************************************************************************************
 	Generators for SoftMax layers 

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation), Def=(TILE_HOR)

		In_DataSize:	1: byte, 2: half word,
		Out_DataSize:	2: half word only, Q15 format

		In_InL3:	0: In is in L2, 1: In is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

		Dim:		Number of inputs

        	KernelOper_T 	SoftMaxOper	Should always be KOP_SOFTMAX

	CNN_SoftMax
                Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to
                a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15
                if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)
	
*********************************************************************************************************************************************************************/

void CNN_SoftMax(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Out_DataSize,

	int In_InL3,
	int Out_InL3,

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
	int InL3 = In_InL3?O_L2DB:0;
	int OutL3 = Out_InL3?O_L2DB:0;
	KernelOper_T KernelOper = CNN_CompositeKernel(SoftMaxOper, KOP_NONE, KOP_NONE);
	unsigned int LayerOp = 0;
	char *SoftMaxKerName = CNN_FindMatchingKernel(SoftMaxOper, KOP_NONE, ParFeat, In_DataSize, 0, 0, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (SoftMaxKerName==0) GenTilingError("CNN_SoftMax Kernel: %s, Can't find a matching basic kernel, warning 16 bits output only", Name);

	LayerOp += Dim;
        UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(In_DataSize,1,1), "In"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1), "Out"),
                      TCArg("unsigned int", "Norm")
                     ),
                Calls(1,
			Call(SoftMaxKerName, LOC_INNER_LOOP,
				Bindings(4,
					K_Arg("In", KER_ARG_TILE),	/* Input tile */
					K_Arg("In", KER_ARG_TILE_H),	/* Number of inputs */
					C_Arg("Norm"),			/* Normalization factor */
					K_Arg("Out", KER_ARG_TILE)	/* Output tile */
				)
			)
		),
                KerArgs(2,
                        KerArg("In",  KerArgSpace(1,T0), OBJ_BUFFER_IN|InL3,   1, Dim,  In_DataSize, 0, 0, 8, "In"),
                        KerArg("Out", KerArgSpace(1,T0), OBJ_BUFFER_OUT|OutL3, 1, Dim, Out_DataSize, 0, 0, 0, "Out")
		)
	);
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
	AT_PrepareForTest(Name,
			  (v4s) {In_DataSize, 0, 0, Out_DataSize},
			  (v4s) {InL3!=0,0,0,OutL3!=0},
			  1, 1, 1, Dim,
			  0,0, 0,0, 0,0, (v4s) 0,
			  0,0, 0,0, 0,0, (v4s) 0,
			  KernelOper,
			  (Out_DataSize==1)?7:15, 0);
}


/*********************************************************************************************************************************************************************
 	Generators for Matrix Addition layers 

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		In1_DataSize:	1: byte, 2: half word,
		In2_DataSize:	1: byte, 2: half word,
		Out_DataSize:	1: byte, 2: half word

		In1_InL3:	0: In is in L2, 1: In is in L3 memory
		In2_InL3:	0: In is in L2, 1: In is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

		InFeat:		Number of input features
		OutFeat:	Number of input features, should always be equal to InFeat
		Width:		Width of a given feature
		Height:		Height of a given feature

        	KernelOper_T 	AddMatOper	Should always be KOP_MATADD

	CNN_MatAdd
	
*********************************************************************************************************************************************************************/

void CNN_MatAdd(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In1_DataSize,
	int In2_DataSize,
	int Out_DataSize,

	int In1_InL3,
	int In2_InL3,
	int Out_InL3,

	int InFeat,
	int OutFeat,
	int Width,
	int Height,

        KernelOper_T AddMatOper
)

{
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int ParFeat = 1;
	if (Ctrl) {
		if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	unsigned int In1L3 = In1_InL3?O_L2DB:0;
	unsigned int In2L3 = In2_InL3?O_L2DB:0;
	unsigned int OutL3 = Out_InL3?O_L2DB:0;
	unsigned int LayerOp = 0;
	KernelOper_T KernelOper = CNN_CompositeKernel(AddMatOper, KOP_NONE, KOP_NONE);

	char *MatAddKerName = CNN_FindMatchingKernel(AddMatOper, KOP_NONE, ParFeat, In1_DataSize, In2_DataSize, 0, Out_DataSize, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);

	if (MatAddKerName==0) GenTilingError("CNN_MatAdd Kernel: %s, Can't find a matching basic kernel", Name);
	LayerOp += OutFeat * Width * Height;
        UserKernel(Name,
		KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(3,
                      TCArg(CNN_ArgDataType(In1_DataSize,1,1), "In1"),
                      TCArg(CNN_ArgDataType(In2_DataSize,1,1), "In2"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1), "Out")
                     ),
                Calls(1,
			Call(MatAddKerName, LOC_INNER_LOOP,
				Bindings(6,
					K_Arg("In1", KER_ARG_TILE),			/* First input tile */
					K_Arg("In2", KER_ARG_TILE),			/* Second input tile */
					K_Arg("Out", KER_ARG_TILE),			/* Output tile */
					K_Arg("In1", KER_ARG_TILE_W),			/* Input tile width */
					K_Arg("In1", KER_ARG_TILE_H),			/* Input tile height */
					K_ArgPar("In1", KER_ARG_PARTILE_SIZE, D0)	/* Number of Matrices involved */
				)
			)
		),
                KerArgs(3,
                        KerArg("In1",  KerArgSpace(2,D0,T0), O_IN|O_DB|In1L3,  Width, Height,  In1_DataSize, 0, 0, 0, "In1"),
                        KerArg("In2",  KerArgSpace(2,D0,T0), O_IN|O_DB|In2L3,  Width, Height,  In2_DataSize, 0, 0, 0, "In2"),
                        KerArg("Out",  KerArgSpace(2,D0,T0), O_OUT|O_DB|In2L3, Width, Height,  Out_DataSize, 0, 0, 0, "Out")
		)
	);
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
	AT_PrepareForTest(Name,
			  (v4s) {In1_DataSize, In2_DataSize, 0, Out_DataSize},
			  (v4s) {In1L3!=0,In2L3!=0,0,OutL3!=0},
			  InFeat, OutFeat, Width, Height,
			  0,0, 0,0, 0,0, (v4s) 0,
			  0,0, 0,0, 0,0, (v4s) 0,
			  KernelOper,
			  0, 0);
}

/*********************************************************************************************************************************************************************
 	Generators for Matrix Multiplication layers.

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		In1_DataSize:	1: byte, 2: half word,
		In2_DataSize:	1: byte, 2: half word,
		Bias_DataSize:	1: byte, 2: half word,
		Out_DataSize:	1: byte, 2: half word

		In1_InL3:	0: In is in L2, 1: In is in L3 memory
		In2_InL3:	0: In is in L2, 1: In is in L3 memory
		Bias_InL3:	0: In is in L2, 1: In is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

        	KernelOper_T 	MatMulOper	Should always be KOP_MATMUL
        	KernelOper_T 	ReLUOper	Optionnal Activation

	CNN_MatMul
	
*********************************************************************************************************************************************************************/

void CNN_MatMul(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In1_DataSize,
	int In2_DataSize,
	int Bias_DataSize,
	int Out_DataSize,

	int In1_InL3,
	int In2_InL3,
	int Bias_InL3,
	int Out_InL3,

	int ColM1,
	int LineM1,
	int ColM2,
	int LineM2,

	int Width,
	int Height,
	int Scx,
	int Scy,

        KernelOper_T MatMulOper,
        KernelOper_T ReLUOper
)

{
	int Log = 1;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	unsigned int In1L3 = In1_InL3?O_L2DB:0;
	unsigned int In2L3 = In2_InL3?O_L2DB:0;
	unsigned int BiasL3 = Bias_InL3?O_L2DB:0;
	unsigned int OutL3 = Out_InL3?O_L2DB:0;
	int NeedScx, NeedScy;
	unsigned int LayerOp = 0;
        int LineO = LineM1, ColO = ColM2;
	int OutLB, OutUB;
	int ConsT0 = Scx;
	KernelOper_T KernelOper = CNN_CompositeKernel(MatMulOper, KOP_NONE, KOP_NONE);
	if (ColM1 != LineM2) GenTilingError("CNN_MatMul: %s, Incorrect input matrices dimensions for a matrix multiplication: [%d x %d]*[%d x %d] %s", Name, LineM1, ColM1, LineM2, ColM2);
	if ((Width*Height) != ColM2) GenTilingError("CNN_MatMul: %s, ColM2: %d should always be equal to Width: %d * Height: %d", Name, ColM2, Width, Height);
	char *MatMulKerName = CNN_FindMatchingKernel(MatMulOper, KOP_NONE, 1, In1_DataSize, In2_DataSize, Bias_DataSize, Out_DataSize, 0,0,0,0,Scx,Scy, 0,0,0,0, &NeedScx, &NeedScy, 0);

	if (MatMulKerName==0) GenTilingError("CNN_MatMul Kernel: %s, Can't find a matching basic kernel", Name);

	ColO = ((Width+Scx-1)/Scx) * ((Height+Scy-1)/Scy);
	LayerOp += ColM1*ColO*LineM1;

	if (In2_DataSize==1) ConsT0 = 4; else ConsT0 = 2;
	
	if (Out_DataSize==1) {
		OutLB = -128; OutUB = 127;
	} else if (Out_DataSize==2) {
		OutLB = -32768; OutUB = 32767;
	} else GenTilingError("CNN_MatMul Kernel: %s, Unsupported Data Type Size for Output", Name);
	if (ReLUOper==KOP_RELU) OutLB = 0;

	if (Log) {
		printf("CNN_MatMul: %s\n", Name);
		printf("In1  => W: %4d, H: %4d\n", ColM1, LineM1);
		printf("In2  => W: %4d, H: %4d, w: %4d, h: %4d, Sx: %1d, Sy: %1d\n", ColM2, LineM2, Width, Height, Scx, Scy);
		printf("Out  => W: %4d, H: %4d\n", ColO, LineO);
		if (MatMulKerName) printf("%20s: %s\n", "MatMulKerName", MatMulKerName);
		printf("ReLU: %s\n", CNN_KernelOperImage(ReLUOper));
	}
	UserKernel(Name,
		KernelIterSpace(2, IterTiledSpace(T1), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(5,
                      TCArg(CNN_ArgDataType(In2_DataSize,1,1),  "In2"),
                      TCArg(CNN_ArgDataType(In1_DataSize,1,1),  "In1"),
                      TCArg(CNN_ArgDataType(Bias_DataSize,1,1), "Bias"),
                      TCArg(CNN_ArgDataType(Out_DataSize,1,1),  "Out"),
		      TCArg("unsigned int", "Norm")
                ),
		Calls(1,
			Call(MatMulKerName, LOC_INNER_LOOP,
				Bindings(17,
					K_Arg("In1",  KER_ARG_TILE),    K_Arg("In1",  KER_ARG_TILE_W), K_Arg("In1",  KER_ARG_TILE_H),
					K_Arg("In2",  KER_ARG_TILE),    K_Arg("In2",  KER_ARG_TILE_W),
					K_Arg("Bias",KER_ARG_TILE),     K_Arg("Out", KER_ARG_TILE),    K_Arg("Out", KER_ARG_TILE_W), K_Arg("In2",  KER_ARG_TILE_BASE),
					K_Arg("KerBuff", KER_ARG_TILE),
					Imm(OutLB),
					Imm(OutUB),
					C_Arg("Norm"),
					NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,
					NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Width):AT_IGNORE_ARG_BINDING,
					(NeedScx||NeedScy)?Imm(Height):AT_IGNORE_ARG_BINDING
				)
			)
		),
		KerArgs(5,
			KerArg("KerBuff",KerArgSpace(1, T0), O_BUFF|O_NTILED,          LineM1, 1,      In2_DataSize,  0, 0,                                                0, 0),
			KerArg("In1",    KerArgSpace(1, T1), O_IN|O_DB|O_CONST|In1L3,  ColM1,  LineM1, In1_DataSize,  0, OBJ_CONSTRAINTS_PAD_REM,                          8, "In1"),
			KerArg("In2",    KerArgSpace(1, T0), O_IN|O_DB|In2L3,          ColM2,  LineM2, In2_DataSize,  0, OBJ_CONSTRAINTS_TILE_VER|OBJ_CONSTRAINTS_PAD_REM, ConsT0, "In2"),
			KerArg("Bias",   KerArgSpace(1, T1), O_IN|O_DB|O_CONST|BiasL3,     1,  LineO,  Bias_DataSize, 0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Bias"),
			KerArg("Out",    KerArgSpace(1, T1), O_OUT|O_DB|OutL3,         ColO,   LineO,  Out_DataSize,  0, OBJ_CONSTRAINTS_PAD_REM,                          0, "Out")
		)
	);
	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
	AT_PrepareForTest(Name,
			  (v4s) {In1_DataSize, In2_DataSize, Bias_DataSize, Out_DataSize},
			  (v4s) {In1L3!=0,In2L3!=0,BiasL3!=0,OutL3!=0},
			  ColM1, LineM1, ColM2, LineM2,
			  0,0, Scx,Scy, 0,0, (v4s) Width,
			  0,0, 0,0, 0,0, (v4s) Height,
			  KernelOper,
			  (In1_DataSize==1)?7:15, 0);
}
