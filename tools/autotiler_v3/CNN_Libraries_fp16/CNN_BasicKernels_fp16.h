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



#ifndef __CNN_BASIC_KERNELS_FP16_H__
#define __CNN_BASIC_KERNELS_FP16_H__
#include "Gap.h"
#include "CNN_Defines_fp16.h"
#include "CNN_FloatType.h"
#include "../CNN_Libraries_SQ8/CNN_AT_Misc.h"
#include "../CNN_Libraries/CNN_Copy.h"

#define gap_pack2f16(x, y)                 ((F16V) {(F16)   (x), (F16)   (y)})

static inline F16 LeakyReLU_fp16(F16 x)

{
	if (x<0) x *= LEAK_CONSTANT;
	return x;
}

/********************************************************************************************************************************************************************/
/****************** Bias setting. ***********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	F16 * __restrict__ Out;			/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	F16 * __restrict__ Bias;		/**< Bias */
} KerSetBias_fp16_T;

/********************************************************************************************************************************************************************/
/****************** Convolution. ************************************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	F16 * __restrict__ In;    		/**< Pointer to input tile  */
	unsigned short int W;	   		/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   		/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of input features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	F16 * __restrict__ Filter;		/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	F16 * __restrict__ Bias;   		/**< Pointer to bias tile, used when convolution is depth wise */
	F16 * __restrict__ Out;   		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. */
	unsigned char N;	   		/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;      			/**< Output stride, Sx, used only for general versions */
	unsigned char Orientation;      	/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_fp16_T;
typedef KerConv_fp16_T KerConvDW_fp16_T;	/* Alias to separate regular conv from depth wise convolution */

typedef struct {
	F16 * __restrict__ In;			/**< Second input matrix tile, convolution features */
	unsigned short int W;		   	/**< Feature Width */
	unsigned short int H;		   	/**< Feature Height */
	unsigned char Fx;		       	/**< Filter width */
	unsigned char Fy;		       	/**< Filter height */
	unsigned char Sx;		       	/**< Filter horizontal stride */
	unsigned char Sy;		       	/**< Filter vertical stride */
	unsigned char Dx;		       	/**< Filter horizontal dilation */
	unsigned char Dy;		       	/**< Filter vertical dilation */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
	v4s Pad;				/**< Filter Pad */
	F16 * __restrict__ Filter;      	/**< First input matrix tile, convolution weights */
	F16 * __restrict__ Bias;		/**< Bias tensor */
	F16 * __restrict__ Out;	 		/**< Output tensor */
	unsigned short int InFeat;	      	/**< Input Features: First input matrix tile width */
	unsigned short int OutFeat;	     	/**< OutputFeatures: First input matrix tile height */
	unsigned short int Wo;		  	/**< Output tile width */
	unsigned short int Ho;		  	/**< Output tile height */
	F16 * __restrict__ ColBuff;     	/**< Temp Buffer, must be at least Align(InFeat*Fx, 8) */
} Ker_MM_Conv_fp16_T;

/********************************************************************************************************************************************************************/
/****************** Pooling/Linear rectification. *******************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	F16 * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;   	/**< Part of the input height to be used */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	F16 * __restrict__ Out;   	/**< Pointer to output tile */
	v4s Pad;			/**< Padding Left, Right, Top, Bottom */
	unsigned char M;		/**< Pooling x dimension */
	unsigned char S;	       	/**< Stride x */
	unsigned char Orientation;      /**< Tile orientation: 1 => Horizontal, 0 => Vertical */
	unsigned char Oper;		/**< Bit0: ReLU, Bit: MayPool/AvgPool. 0x0: MaxPool and NoReLU, 0x1: MaxPool and ReLU, 0x2: AvgPool, No ReLU, 0x3: AvgPool and ReLU */
	unsigned char D;		/**< Dilation, Dx */
	unsigned char My;		/**< Filter My, used only if Mx!=My */
	unsigned char Sy;		/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;		/**< Dilation Dy, used only if Dx!=Dy */
} KerPool_fp16_T;

/* For HWC forms */
typedef struct {
	F16 * __restrict__ In;		/**< input tensor tile */
	unsigned short int W;		/**< Feature Width */
	unsigned short int H;  		/**< Feature Height */
	unsigned char Fx;      		/**< Filter width */
	unsigned char Fy;       	/**< Filter height */
	unsigned char Sx;              	/**< Filter horizontal stride */
	unsigned char Sy;             	/**< Filter vertical stride */
	unsigned char FirstTile;      	/**< 1 if we are on the first tile */
	v4s Pad;                      	/**< Filter Pad */
	F16 * __restrict__ Out;		/**< Output tensor */
	unsigned short int Feat;       	/**< Input Features */
	unsigned short int Wo;         	/**< Output tile width */
	unsigned short int Ho;         	/**< Output tile height */
} Ker_MM_Pool_fp16_T;

typedef struct {
	F16 * __restrict__ In;   	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	F16 * __restrict__ Out;  	/**< Pointer to output tile */
	unsigned int TileIndex;		/**< Tile Index */
} KerGlobalPool_fp16_T;

/********************************************************************************************************************************************************************/
/****************** Linear layer/Linear rectification. For both one output in parallel and several outputs evaluated in parallel ************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	F16 * __restrict__ In;			/**< Pointer to input tile */
	F16 * __restrict__ Filter;		/**< Pointer to filter tile, width is TotalInSize */
	F16 * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	F16 * __restrict__ Out;			/**< Pointer to output tile, size if OutDim */
	unsigned short int InDim;		/**< Size of the the tile */
	unsigned short int TotalInDim;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutDim;		/**< Size of the output tile */
	F16 UB;					/**< For ReLUN variant, upper bound */
	unsigned short int TileIndex;		/**< Tile Number */
} KerLinear_fp16_T;

/******************************************************************************************************************************/
/******************* ACTIVATIONS  *********************************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	F16 *__restrict__ In;			/**< Input tile */
	F16 *__restrict__ Out;			/**< Output tile */
	unsigned short int Feat;		/**< Number of features */
	unsigned short int W;			/**< Feature width */
	unsigned short int H;			/**< Feature height */
	F16 UB;					/**< For ReLUN variant, upper bound */
} KerActivation_fp16_T;

/******************************************************************************************************************************/
/******************* MAT ALGEBRA  *********************************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	F16 *__restrict__ In1;			/**< First input of a matric diadic operation */
	F16 *__restrict__ In2;			/**< Second input of a matric diadic operation */
	F16 *__restrict__ Out;			/**< Out of a matric diadic operation */
	unsigned short int W;			/**< Input Width */
	unsigned short int H;			/**< Input Height */
	unsigned short int N;			/**< Number of (input, input, output) */
} KerMat3_fp16_T;

typedef struct {
	F16 * __restrict__ In1;			/**< First input matrix tile */
	unsigned short int W_In1;		/**< First input matrix tile width */
	unsigned short int H_In1;		/**< First input matrix tile height */
	F16 * __restrict__ In2;			/**< Second input matrix tile */
	unsigned short int W_In2;		/**< Second input matrix tile width, height is by construction H_In1 */
	F16 * __restrict__ Bias;		/**< Bias input tile, will be added to the product */
	F16 * __restrict__ Out;			/**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;		/**< Output matrix full width */
	unsigned short int OutFirstCol;       	/**< Equal M2FirstCol */
	F16 * __restrict__ BufferColIn2;	/**< In case vectorization is used will be used to copy a column of In2 into a line */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
	F16 UB;					/**< In case RELUN */
} KerMatMul_fp16_T;

/******************************************************************************************************************************/
/******************* SOFT MAX *************************************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	F16 *__restrict__ In;			/**< Pointer to input tile */
	unsigned short int Feat;                /**< Number of features of the tile */
	unsigned short int N;			/**< Size of the tile */
	F16 *__restrict__ Out;			/**< Pointer to output tile */
} KerSoftMax_fp16_T;

/******************************************************************************************************************************/
/******************* Recursive NN (RNN, LSTM, GRU) ****************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	F16 *__restrict__ StateInOut;   	/**< Pointer to In/Out state, Dim=DimState   */
	F16 *__restrict__ State;        	/**< Pointer to to a copy of state with extra space for in (DimState+DimIn)   */
	F16 *__restrict__ Xin;          	/**< Pointer to In state, Dim=DimIn */
	unsigned short int DimState;            /**< State dimension */
	unsigned short int DimIn;               /**< Input dimension */
	F16 *__restrict__ Wf;           	/**< Pointer to Forget gate (whether to erase cell) weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bf;                  /**< Pointer to Forget gate bias */
	F16 *__restrict__ Hout;         	/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;                /**< Number of output produced in StateInOut */
	unsigned short int OutBase;             /**< Index of first output produced in StateInOut */
	char FirstOut;                          /**< 1 if first out of one cell to eval */
	char FirstCell;                         /**< 1 if first cell of a group of cell */
	char Reset;                             /**< 1 if RNN State has to be reset */
} KerRNN_fp16_T;

typedef struct {
	F16 *__restrict__ StateInOut;   	/**< Pointer to In/Out state, Dim=DimState   */
	F16 *__restrict__ CInOut;       	/**< Pointer to In/Out State C (LSTM), DimIn = DimOut = DimState */
	F16 *__restrict__ State;        	/**< Pointer to to a copy of state with extra space for in (2*DimState+DimIn)   */
	F16 *__restrict__ Xin;          	/**< Pointer to In state, Dim=DimIn */
	unsigned short int DimState;            /**< State dimension */
	unsigned short int DimIn;               /**< Input dimension */
	F16 *__restrict__ Wf;           	/**< Pointer to Forget gate (whether to erase cell) weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bf;                  /**< Pointer to Forget gate bias */
	F16 *__restrict__ Wi;           	/**< Pointer to Input gate (whether to write cell) weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bi;                  /**< Pointer to Input gate bias */
	F16 *__restrict__ Wg;           	/**< Pointer to Gate gate (how much to write to cell) weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bg;                  /**< Pointer to Gate gate bias */
	F16 *__restrict__ Wo;           	/**< Pointer to Output gate (how much to reveal cell) weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bo;                  /**< Pointer to Output gate bias */
	F16 *__restrict__ Hout;         	/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;                /**< Number of output produced in StateInOut and CInOut */
	unsigned short int OutBase;             /**< Index of first output produced in StateInOut and CInOut*/
	char FirstOut;                          /**< 1 if first out of one cell to eval */
	char FirstCell;                         /**< 1 if first cell of a group of cell */
	char Reset;                             /**< If 1 LSTM State is reset */
} KerLSTM_fp16_T;

typedef struct {
	F16 *__restrict__ StateInOut;   	/**< Pointer to In/Out state, Dim=DimState   */
	F16 *__restrict__ Xin;         	 	/**< Pointer to In state, Dim=DimIn */
	F16 *__restrict__ State;        	/**< Pointer to to a copy of state with extra space for in (DimState+DimIn)   */
	unsigned short int DimState;            /**< State dimension */
	unsigned short int DimIn;               /**< Input dimension */
	F16 *__restrict__ Wr;           	/**< Pointer to R gate weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Br;                  /**< Pointer to R gate bias */
	F16 *__restrict__ Wz;           	/**< Pointer to Z gate weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bz;                  /**< Pointer to Z gate bias */
	F16 *__restrict__ Wh;           	/**< Pointer to H gate weights, Dim=[DimState+DimIn,DimState] */
	F16 * __restrict__ Bwh;                  /**< Pointer to H gate bias */
	F16 * __restrict__ Brh;                  /**< Pointer to H gate bias */
	F16 *__restrict__ Sbuff;        	/**< Pointer to Ht buffer, Dim=[DimState] */
	F16 *__restrict__ Hout;         	/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;                /**< Number of output produced in StateInOut */
	unsigned short int OutBase;             /**< Index of first output produced in StateInOut */
	char FirstOut;                          /**< 1 if first out of one cell to eval */
	char FirstCell;                         /**< 1 if first cell of a group of cell */
	char Reset;                             /**< If 1 GRU State is reset */
} KerGRU_fp16_T;


/******************************************************************************************************************************/
/**************** BIAS SETTING ************************************************************************************************/
/******************************************************************************************************************************/

extern void KerParSetBias_fp16(KerSetBias_fp16_T *Arg);
extern void KerSetBias_fp16(KerSetBias_fp16_T *Arg);

/******************************************************************************************************************************/
/**************** CONVOLUTIONS ************************************************************************************************/
/******************************************************************************************************************************/
/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are half precision float (_fp16)
	Features are evaluated in parallel, one per core
*/

/* Point Wise*/
extern void KerParConv1x1Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x1Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x1StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConv3x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv3x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x3Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x3Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerParConv3x3Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv3x3Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConv3x3StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConv5x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv5x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x5Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv1x5Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerParConv5x5Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConv5x5Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConv5x5StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConvNxNStrideS_fp16(KerConv_fp16_T *Arg);
extern void KerParConvNxMStrideSxSy_fp16(KerConv_fp16_T *Arg);

extern void KerParConvNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg);

/* Depth Wise*/
extern void KerParConvDW1x1Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x1Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x1StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConvDW3x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW3x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x3Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x3Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW3x3Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW3x3Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW3x3StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConvDW5x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW5x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x5Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW1x5Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW5x5Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW5x5Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDW5x5StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerParConvDWNxNStrideS_fp16(KerConv_fp16_T *Arg);
extern void KerParConvDWNxMStrideSxSy_fp16(KerConv_fp16_T *Arg);

extern void KerParConvDWNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg);

/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are half precision (_fp16)
	A single convolution is evaluated in parallel on all the cores
*/

/* Point Wise */
extern void KerConv1x1Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x1Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x1StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConv3x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv3x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x3Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x3Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerConv3x3Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConv3x3Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConv3x3StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConv5x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv5x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x5Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConv1x5Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerConv5x5Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConv5x5Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConv5x5StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConvNxNStrideS_fp16(KerConv_fp16_T *Arg);
extern void KerConvNxMStrideSxSy_fp16(KerConv_fp16_T *Arg);

extern void KerConvNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg);

/* Depth Wise */
extern void KerConvDW1x1Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x1Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x1StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConvDW3x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW3x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x3Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x3Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW3x3Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW3x3Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW3x3StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConvDW5x1Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW5x1Stride2x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x5Stride1x1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW1x5Stride1x2_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW5x5Stride1_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW5x5Stride2_fp16(KerConv_fp16_T *Arg);
extern void KerConvDW5x5StrideS_fp16(KerConv_fp16_T *Arg);

extern void KerConvDWNxNStrideS_fp16(KerConv_fp16_T *Arg);
extern void KerConvDWNxMStrideSxSy_fp16(KerConv_fp16_T *Arg);

extern void KerConvDWNxMDxDyStrideSxSy_fp16(KerConv_fp16_T *Arg);

/* Im2Col based implementation for 2D and 1D conv */

/* CHW, out chan parallel */
extern void KerPar_MM_Conv1D_fp16(Ker_MM_Conv_fp16_T *Arg);
extern void KerPar_MM_Conv1D_ReLU_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv1D_DxDy_fp16(Ker_MM_Conv_fp16_T *Arg);
extern void KerPar_MM_Conv1D_DxDy_ReLU_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv2D_fp16(Ker_MM_Conv_fp16_T *Arg);
extern void KerPar_MM_Conv2D_ReLU_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv2D_DxDy_fp16(Ker_MM_Conv_fp16_T *Arg);
extern void KerPar_MM_Conv2D_DxDy_ReLU_fp16(Ker_MM_Conv_fp16_T *Arg);

/* HWC, out chan parallel */
extern void KerPar_MM_Conv1D_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv1D_DxDy_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv1x1_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv2D_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);

extern void KerPar_MM_Conv2D_DxDy_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);

/* HWC, parallel along W axis, to be used when in feat is < 8 */
extern void Ker_MM_Conv2D_HWC_fp16(Ker_MM_Conv_fp16_T *Arg);


/******************************************************************************************************************************/
/**************** ACTIVATIONS *************************************************************************************************/
/******************************************************************************************************************************/

/* Feature maps are evaluated in parallel, one per core.
   Feature maps of half precision floats (_fp16)
*/
extern void KerParReLU_fp16(KerActivation_fp16_T *Arg);
extern void KerParReLUN_fp16(KerActivation_fp16_T *Arg);
extern void KerParSwish_fp16(KerActivation_fp16_T *Arg);
extern void KerParTanh_fp16(KerActivation_fp16_T *Arg);
extern void KerParSigmoid_fp16(KerActivation_fp16_T *Arg);
extern void KerParLeakyReLU_fp16(KerActivation_fp16_T *Arg);

/******************************************************************************************************************************/
/**************** MAX/AVG POOLING WITH OPTIONAL ReLU **************************************************************************/
/******************************************************************************************************************************/
/*
	Performs Max or Average pooling followed by an optional linear rectification (ReLU).

	Zero padding is optional (Arg->Pad)

	Arg->Oper == 0	Max Pooling, No ReLU
	Arg->Oper == 1	Max Pooling then ReLU
	Arg->Oper == 2	Average Pooling, No ReLU
	Arg->Oper == 3	Average Pooling then ReLU
*/

/* Several output feature maps are evaluated in parallel, one output map per core
   Feature maps of half precision floats (_fp16)
   Pool 2x2 Stride 2 as a special case
*/
extern void KerParPool2x2Stride2_fp16(KerPool_fp16_T *Arg);
extern void KerParPoolNxNStrideS_fp16(KerPool_fp16_T *Arg);
extern void KerParPoolNxMStrideSxSy_fp16(KerPool_fp16_T *Arg);

extern void KerParGlobalMaxPool_fp16(KerGlobalPool_fp16_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_fp16(KerGlobalPool_fp16_T *Arg);
extern void KerParGlobalAvgPool_fp16(KerGlobalPool_fp16_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_fp16(KerGlobalPool_fp16_T *Arg);

/* One output feature map is evaluated in parallel on all cores.
   Feature maps of half precision floats (_fp16)
   Pool 2x2 Stride 2 as a special case
*/
extern void KerPool2x2Stride2_fp16(KerPool_fp16_T *Arg);
extern void KerPoolNxNStrideS_fp16(KerPool_fp16_T *Arg);
extern void KerPoolNxMStrideSxSy_fp16(KerPool_fp16_T *Arg);

extern void KerParMaxPoolNxMStrideSxSy_HWC_fp16(Ker_MM_Pool_fp16_T *Arg);
extern void KerParAvgPoolNxMStrideSxSy_HWC_fp16(Ker_MM_Pool_fp16_T *Arg);


/******************************************************************************************************************************/
/**************** FULLY CONNECTED *********************************************************************************************/
/******************************************************************************************************************************/

/*
	_fp16		: Input, Bias, Filter and Output are half precision floats
*/

/* Several output are evaluated in parallel, one per core */
extern void KerParLinearLayer_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerReLU_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerReLUN_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerSwish_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerSigmoid_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerTanh_fp16(KerLinear_fp16_T *Arg);
extern void KerParLinearLayerLeakyReLU_fp16(KerLinear_fp16_T *Arg);

/******************************************************************************************************************************/
/******************* MAT ALGEBRA  *********************************************************************************************/
/******************************************************************************************************************************/

extern void KerParMatAdd_fp16(KerMat3_fp16_T *Arg);
extern void KerParMatAddReLU_fp16(KerMat3_fp16_T *Arg);

extern void KerParMatMul_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSxSy_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulReLU_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSxSyReLU_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulReLUN_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSxSyReLUN_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulSwish_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSwishSxSy_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulSigmoid_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSigmoidSxSy_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulTanh_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulTanhSxSy_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulLeakyrelu_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulLeakyreluSxSy_fp16(KerMatMul_fp16_T *Arg);

extern void KerParMatMulSmallFeat_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSmallFeatReLU_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSmallFeatReLUN_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSwishSmallFeat_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulSigmoidSmallFeat_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulTanhSmallFeat_fp16(KerMatMul_fp16_T *Arg);
extern void KerParMatMulLeakyreluSmallFeat_fp16(KerMatMul_fp16_T *Arg);

/******************************************************************************************************************************/
/******************* SOFT MAX *************************************************************************************************/
/******************************************************************************************************************************/

extern void KerParSoftMax_fp16(KerSoftMax_fp16_T *Arg);

/******************************************************************************************************************************/
/******************* Recursive NN (RNN, LSTM, GRU) ****************************************************************************/
/******************************************************************************************************************************/

extern void RNN_ParKer_fp16(KerRNN_fp16_T *Arg);
extern void LSTM_ParKer_fp16(KerLSTM_fp16_T *Arg);
extern void GRU_ParKer_fp16(KerGRU_fp16_T *Arg);

#endif

