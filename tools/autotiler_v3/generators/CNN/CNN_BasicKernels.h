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



#ifndef __CNN_BASIC_KERNELS_H__
#define __CNN_BASIC_KERNELS_H__
#include "Gap.h"

#ifdef MAXDPPREC
#define DP_fps_T int
#else
#define DP_fps_T short int
#endif

/******************************************************************************************************************************/
/******************* Autotiler Internal calls *********************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	char *__restrict__ In;  /**< Tile */
	short int W;		/**< Tile width */
	short int H;		/**< Tile Height */
	short int Feat;		/**< Number of features */
	short int Size;		/**< Tile data type size in bytes */
	short int Pad;		/**< Number of lines to be cleared at bottom of each tile if horizontal tile, at right if vertical */
	short int Orientation;	/**< Tile orientation. 0: Horizontal, 1: vertical */
} AT_KerTileClear_T;

void AT_TileClear(
	char *__restrict__ In,  /**< Tile */
	int W,		  /**< Tile width */
	int H,		  /**< Tile height */
	int Feat,	       /**< Number of features */
	int Size,	       /**< Tile element size in bytes */
	int Pad,		/**< Height or width of the area to be 0 padded */
	int Orientation	 /**< 0: Horizontal tile, 1: Vertical tile */
	);

/********************************************************************************************************************************************************************/
/****************** Bias setting. ***********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	signed char * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	unsigned short int Norm;		/**< Normalization to be applied to Bias when setting Out */
	signed char * __restrict__ Bias;	/**< Bias */
} KerSetBias_fps_T;

typedef struct {
	short int * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	unsigned short int Norm;		/**< Normalization to be applied to Bias when setting Out */
	short int * __restrict__ Bias;		/**< Bias */
} KerSetBias_fp_T;

typedef struct {
	short int * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	unsigned short int Norm;		/**< Normalization to be applied to Bias when setting Out */
	signed char * __restrict__ Bias;	/**< Bias */
} KerSetNormedBias_fp_fps_T;

typedef struct {
        int * __restrict__ Out;                 /**< Output */
        unsigned short int W;                   /**< Output width */
        unsigned short int H;                   /**< Output height */
        unsigned short int OutFeatures;         /**< Number of features, used for channel parallel kernels */
        unsigned short int Norm;                /**< Normalization to be applied to Bias when setting Out */
        signed char * __restrict__ Bias;        /**< Bias */
} KerSetNormedBias_fpd_fps_T;

typedef struct {
	int * __restrict__ Out;			/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	unsigned short int Norm;		/**< Normalization to be applied to Bias when setting Out */
	short int * __restrict__ Bias;		/**< Bias */
} KerSetNormedBias_fpd_fp_T;


/********************************************************************************************************************************************************************/
/****************** Convolution. ************************************************************************************************************************************/
/********************************************************************************************************************************************************************/


/* Single precision */
typedef struct {
	short int * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of input features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	short int * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	short int * __restrict__ Out;   	/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	short int * __restrict__ Bias;   	/**< Pointer to bias tile, used when convolution is depth wise */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< Fixed point format, should be <= 15 */
	unsigned char N;	   		/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;      			/**< Output stride, Sx, used only for general versions */
	unsigned char Orientation;      	/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_fp_T;
typedef KerConv_fp_T KerConvDW_fp_T;		/* Alias to separate regular conv from depth wise convolution */

typedef struct {
	signed char * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of input features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	signed char * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	signed char * __restrict__ Out;   	/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	signed char * __restrict__ Bias;   	/**< Pointer to bias tile, used when convolution is depth wise */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< Fixed point format, should be <= 7 */
	unsigned char N;	   		/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;      			/**< Output stride, S, used only for general versions */
	unsigned char Orientation;      	/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_fps_T;
typedef KerConv_fps_T KerConvDW_fps_T;		/* Alias to separate regular conv from depth wise convolution */


/* Double precision */
typedef struct {
	short int * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of output features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	short int * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	int * __restrict__ Out;   		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< Fixed point format, should be <= 15 */
	unsigned char N;	   		/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;      			/**< Output stride, Sx, used only for general versions */
	unsigned char Orientation;      	/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_DP_fp_T;

typedef struct {
	signed char * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of output features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	signed char * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	DP_fps_T * __restrict__ Out;   		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< Fixed point format, should be <= 7 */
	unsigned char N;	   		/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;      			/**< Output stride, S, used only for general versions */
	unsigned char Orientation;      	/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_DP_fps_T;


/********************************************************************************************************************************************************************/
/****************** Convolution. Convert double precision to single precision ***************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	int * __restrict__ In;			/**< Input in double precision Q2*Norm */
	unsigned short int W;			/**< Input width */
	unsigned short int H;			/**< Output height */
	short int * __restrict__ Out;		/**< Output in single precision QNorm */
	unsigned short int Norm;		/**< Precision */
	unsigned short int InFeatures;		/**< Number of channels, used only for channel parallel kernels */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	short int * __restrict__ MulBias;	/**< Output Multiplicative bias */
} KerDP_fp_T;

typedef struct {
	DP_fps_T * __restrict__ In;		/**< Input in double precision Q2*Norm */
	unsigned short int W;			/**< Input width */
	unsigned short int H;			/**< Output height */
	signed char * __restrict__ Out;		/**< Output in single precision QNorm */
	unsigned short int Norm;		/**< Precision */
	unsigned short int InFeatures;		/**< Number of channels, used only for channel parallel kernels */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	signed char * __restrict__ MulBias;	/**< Output Multiplicative bias */
} KerDP_fps_T;


/********************************************************************************************************************************************************************/
/****************** Pooling/Linear rectification. *******************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	short int * __restrict__ In;    /**< Pointer to input tile  */
	unsigned short int W;	   /**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   /**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	short int * __restrict__ Out;   /**< Pointer to output tile */
	v4s Pad;			/**< Padding Left, Right, Top, Bottom */
	unsigned char M;		/**< Pooling x dimension */
	unsigned char S;	       	/**< Stride x */
	unsigned char Orientation;      /**< Tile orientation: 1 => Horizontal, 0 => Vertical */
	unsigned char Oper;		/**< Bit0: ReLU, Bit: MayPool/AvgPool. 0x0: MaxPool and NoReLU, 0x1: MaxPool and ReLU, 0x2: AvgPool, No ReLU, 0x3: AvgPool and ReLU */
	unsigned char D;		/**< Dilation, Dx */
	unsigned char My;		/**< Filter My, used only if Mx!=My */
	unsigned char Sy;		/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;		/**< Dilation Dy, used only if Dx!=Dy */
	int LB;				/**< Lower bound for the output */
	int UB;				/**< Upper bound for the output */
} KerReLUPool_fp_T;

typedef struct {
	signed char * __restrict__ In;  /**< Pointer to input tile  */
	unsigned short int W;	   /**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   /**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	signed char * __restrict__ Out; /**< Pointer to output tile */
	v4s Pad;			/**< Padding Left, Right, Top, Bottom */
	unsigned char M;		/**< Pooling x dimension */
	unsigned char S;	       	/**< Stride x */
	unsigned char Orientation;      /**< Tile orientation: 1 => Horizontal, 0 => Vertical */
	unsigned char Oper;		/**< Bit0: ReLU, Bit1: MayPool/AvgPool. 0x0: MaxPool and NoReLU, 0x1: MaxPool and ReLU, 0x2: AvgPool, No ReLU, 0x3: AvgPool and ReLU */
	unsigned char D;		/**< Dilation, Dx */
	unsigned char My;		/**< Filter My, used only if Mx!=My */
	unsigned char Sy;		/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;		/**< Dilation Dy, used only if Dx!=Dy */
	int LB;				/**< Lower bound for the output */
	int UB;				/**< Upper bound for the output */
} KerReLUPool_fps_T;


/********************************************************************************************************************************************************************/
/****************** Linear layer/Linear rectification. For both one output in parallel and several outputs evaluated in parallel ************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	short int * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	short int * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	short int * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	short int * __restrict__ Out;		/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fp_T;

typedef struct {
	signed char * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	signed char * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	signed char * __restrict__ Bias;	/**< Pointer to bias tile, size is OutSize */
	signed char * __restrict__ Out;		/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fps_T;

typedef struct {
	short int * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	signed char * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	short int * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	short int * __restrict__ Out;		/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	unsigned char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fp_fps_fp_T;

typedef struct {
	signed char * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	signed char * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	short int * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	short int * __restrict__ Out;		/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	unsigned char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fps_fps_fp_T;

typedef struct {
	short int * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	short int * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	short int * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	int * __restrict__ Out;			/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	unsigned char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fp_fp_fpd_T;

typedef struct {
        short int * __restrict__ In;		/**< Pointer to input tile */
        short int * __restrict__ Filter;	/**< Pointer to Filter tile */
        int * __restrict__ Out;			/**< Pointer to one output or to a vector[N Cores] of intermediate resuts */
        short int InSize;			/**< Number of items in In/Filter */
	char Tile;				/**< Tile index, to control init */
} KerDPLinear_fp_T;

typedef struct {
        signed char * __restrict__ In;		/**< Pointer to input tile */
        signed char * __restrict__ Filter;	/**< Pointer to Filter tile */
        int * __restrict__ Out;			/**< Pointer to one output or to a vector[N Cores] of intermediate resuts */
        short int InSize;			/**< Number of items in In/Filter */
	char Tile;				/**< Tile index, too control when to initialize Out */
} KerDPLinear_fps_T;

typedef struct {
        int * __restrict__ In;			/**< Pointer to a vector[N Cores] of intermediate resuts */
        short int * __restrict__ Bias;		/**< Pointer to Bias */
        short int * __restrict__ Out;		/**< Pointer to Out */
        int LB;					/**< Max output value */
        int UB;					/**< Min output value */
        char Norm;				/**< Precision, input/output/filter */
        char NormBias;				/**< Bias precision */
        char Oper;				/**< Activation operation after linear layer, see CNN_Activation_Oper_T */
} KerDPLinearReduct_fp_T;

typedef struct {
        int * __restrict__ In;			/**< Pointer to a vector[N Cores] of intermediate resuts */
        signed char * __restrict__ Bias;	/**< Pointer to Bias */
        signed char * __restrict__ Out;		/**< Pointer to Out */
        int LB;					/**< Max output value */
        int UB;					/**< Min output value */
        char Norm;				/**< Precision, input/output/filter */
        char NormBias;				/**< Bias precision */
        char Oper;				/**< Activation operation after linear layer, see CNN_Activation_Oper_T */
} KerDPLinearReduct_fps_T;

/******************************************************************************************************************************/
/******************* MAT ALGEBRA  *********************************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	short int *__restrict__ In1;		/**< First input of a matric diadic operation */
	short int *__restrict__ In2;		/**< Second input of a matric diadic operation */
	short int *__restrict__ Out;		/**< Out of a matric diadic operation */
	unsigned short int W;			/**< Input Width */
	unsigned short int H;			/**< Input Height */
	unsigned short int N;			/**< Number of (input, input, output) */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
	unsigned char In1_Q;			/**< In1 quantization */
	unsigned char In2_Q;			/**< In2 quantization */
	unsigned char Out_Q;			/**< Out quantization */
} KerMat3_fp_T;

typedef struct {
	signed char *__restrict__ In1;		/**< First input of a matric diadic operation */
	signed char *__restrict__ In2;		/**< Second input of a matric diadic operation */
	signed char *__restrict__ Out;		/**< Output of a matric diadic operation */
	unsigned short int W;			/**< Input Width */
	unsigned short int H;			/**< Input Height */
	unsigned short int N;			/**< Number of (input, input, output) */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
	unsigned char In1_Q;			/**< In1 quantization */
	unsigned char In2_Q;			/**< In2 quantization */
	unsigned char Out_Q;			/**< Out quantization */
} KerMat3_fps_T;

typedef struct {
	short int * __restrict__ In1;		/**< First input matrix tile */
	unsigned short int W_In1;		/**< First input matrix tile width */
	unsigned short int H_In1;		/**< First input matrix tile height */
	short int * __restrict__ In2;		/**< Second input matrix tile */
	unsigned short int W_In2;		/**< Second input matrix tile width, height is by construction H_In1 */
	short int * __restrict__ Bias;		/**< Bias input tile, will be added to the product */
	short int * __restrict__ MulBias;       /**< Output Multiplicative bias */
	short int * __restrict__ Out;		/**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;		/**< Output matrix full width */
	unsigned short int OutFirstCol;       	/**< Equal M2FirstCol */
	short int * __restrict__ BufferColIn2;	/**< In case vectorization is used will be used to copy a column of In2 into a line */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	unsigned char Norm;			/**< Fixed point format */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
} KerMatMul_fp_T;

typedef struct {
	short int * __restrict__ In1;		/**< First input matrix tile */
	unsigned short int W_In1;		/**< First input matrix tile width */
	unsigned short int H_In1;		/**< First input matrix tile height */
	short int * __restrict__ In2;		/**< Second input matrix tile */
	unsigned short int W_In2;		/**< Second input matrix tile width, height is by construction H_In1 */
	int * __restrict__ Bias;		/**< Bias input tile, will be added to the product, double precision */
	int * __restrict__ MulBias;		/**< Output Multiplicative bias */
	short int * __restrict__ Out;		/**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;		/**< Output matrix full width */
	unsigned short int OutFirstCol;       	/**< Equal M2FirstCol */
	short int * __restrict__ BufferColIn2;	/**< In case vectorization is used will be used to copy a column of In2 into a line */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	unsigned char Norm;			/**< Fixed point format */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
} KerMatMul_fpd_fp_T;

typedef struct {
	signed char * __restrict__ In1;		/**< First input matrix tile */
	unsigned short int W_In1;		/**< First input matrix tile width */
	unsigned short int H_In1;		/**< First input matrix tile height */
	signed char * __restrict__ In2;		/**< Second input matrix tile */
	unsigned short int W_In2;		/**< Second input matrix tile width, height is by construction H_In1 */
	signed char * __restrict__ Bias;	/**< Bias input tile, will be added to the product */
	signed char * __restrict__ MulBias;	/**< Output Multiplicative bias */
	signed char * __restrict__ Out;		/**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;		/**< Output matrix full width */
	unsigned short int OutFirstCol;       	/**< Equal M2FirstCol */
	signed char * __restrict__ BufferColIn2;/**< In case vectorization is used will be used to copy a column of In2 into a line */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	unsigned char Norm;			/**< Fixed point format */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
} KerMatMul_fps_T;

typedef struct {
	signed char * __restrict__ In1;		/**< First input matrix tile */
	unsigned short int W_In1;		/**< First input matrix tile width */
	unsigned short int H_In1;		/**< First input matrix tile height */
	signed char * __restrict__ In2;		/**< Second input matrix tile */
	unsigned short int W_In2;		/**< Second input matrix tile width, height is by construction H_In1 */
	short int * __restrict__ Bias;		/**< Bias input tile, will be added to the product, double precision */
	short int * __restrict__ MulBias;	/**< Output Multiplicative bias */
	signed char * __restrict__ Out;		/**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;		/**< Output matrix full width */
	unsigned short int OutFirstCol;       	/**< Equal M2FirstCol */
	signed char * __restrict__ BufferColIn2;/**< In case vectorization is used will be used to copy a column of In2 into a line */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	unsigned char Norm;			/**< Fixed point format */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
} KerMatMul_fp_fps_T;

/******************************************************************************************************************************/
/******************* SOFT MAX *************************************************************************************************/
/******************************************************************************************************************************/

typedef struct {
	short int *__restrict__ In;		/**< Pointer to input tile */
	unsigned short int N;			/**< Size of the tile */
	unsigned short int Norm;		/**< Normalization factor */
	short int *__restrict__ Out;		/**< Pointer to output tile */
} KerSoftMax_fp_T;

typedef struct {
	signed char *__restrict__ In;		/**< Pointer to input tile */
	unsigned short int N;			/**< Size of the tile */
	unsigned short int Norm;		/**< Normalization factor */
	short int *__restrict__ Out;		/**< Pointer to output tile */
} KerSoftMax_fps_T;

/******************************************************************************************************************************/
/**************** BIAS SETTING ************************************************************************************************/
/******************************************************************************************************************************/

extern void KerParSetBias_fp(KerSetBias_fp_T *Arg);
extern void KerParSetBias_fps(KerSetBias_fps_T *Arg);
extern void KerParSetNormedBias_fp_fps(KerSetNormedBias_fp_fps_T *Arg);
extern void KerParSetNormedBias_fpd_fps(KerSetNormedBias_fpd_fps_T *Arg);
extern void KerParSetNormedBias_fpd_fp(KerSetNormedBias_fpd_fp_T *Arg);

extern void KerSetBias_fp(KerSetBias_fp_T *Arg);
extern void KerSetBias_fps(KerSetBias_fps_T *Arg);
extern void KerSetNormedBias_fp_fps(KerSetNormedBias_fp_fps_T *Arg);
extern void KerSetNormedBias_fpd_fps(KerSetNormedBias_fpd_fps_T *Arg);
extern void KerSetNormedBias_fpd_fp(KerSetNormedBias_fpd_fp_T *Arg);


/******************************************************************************************************************************/
/**************** CONVOLUTIONS ************************************************************************************************/
/******************************************************************************************************************************/
/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are half words (_fp)
	Features are evaluated in parallel, one per core
*/

/* Single precision */
extern void KerParConv1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConv1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConv1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConv3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConv3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConv1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConv1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConv3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConv3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConv3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConv5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConv5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConv1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConv1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConv5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConv5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConv5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerParConvNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerParConvNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Single precision Depth Wise*/
extern void KerParConvDW1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDW3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDW5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDWNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerParConvDWNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerParConvDWNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Single precision Depth Wise with double precision bias */
extern void KerParConvDW1x1Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x1Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x1StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDW3x1Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x1Stride2x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x3Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x3Stride1x2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW3x3StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDW5x1Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x1Stride2x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x5Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW1x5Stride1x2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDW5x5StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWNxNStrideS_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWNxMStrideSxSy_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWNxMDxDyStrideSxSy_fpd_fp(KerConv_fp_T *Arg);

/* Double precision Depth Wise*/
extern void KerParConvDWDP1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDP3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDP5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDPNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDPNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDPNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Double precision Depth Wise with double precision bias */
extern void KerParConvDWDP1x1Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x1Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x1StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDP3x1Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x1Stride2x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x3Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x3Stride1x2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP3x3StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDP5x1Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x1Stride2x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x5Stride1x1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP1x5Stride1x2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5Stride1_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5Stride2_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDP5x5StrideS_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDPNxNStrideS_fpd_fp(KerConv_fp_T *Arg);
extern void KerParConvDWDPNxMStrideSxSy_fpd_fp(KerConv_fp_T *Arg);

extern void KerParConvDWDPNxMDxDyStrideSxSy_fpd_fp(KerConv_fp_T *Arg);

/* Double precision */
extern void KerParConv1x1Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x1Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x1StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerParConv3x1Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv3x1Stride2x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x3Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x3Stride1x2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv3x3Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv3x3Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv3x3StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerParConv5x1Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv5x1Stride2x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x5Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv1x5Stride1x2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv5x5Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv5x5Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConv5x5StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerParConvNxNStrideS_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerParConvNxMStrideSxSy_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerParConvNxMDxDyStrideSxSy_DP_fp(KerConv_DP_fp_T *Arg);

/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are bytes (_fps)
	Features are evaluated in parallel, one per core
*/
/* Single precision */
extern void KerParConv1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConv1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConv1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConv3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConv3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConv1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConv1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConv3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConv3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConv3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConv5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConv5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConv1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConv1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConv5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConv5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConv5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerParConvNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerParConvNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Single precision, Depth Wise */
extern void KerParConvDW1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDW3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDW5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDWNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerParConvDWNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerParConvDWNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Single precision, Depth Wise, double precision bias */
extern void KerParConvDW1x1Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x1Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x1StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDW3x1Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x1Stride2x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x3Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x3Stride1x2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW3x3StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDW5x1Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x1Stride2x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x5Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW1x5Stride1x2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDW5x5StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWNxNStrideS_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWNxMStrideSxSy_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWNxMDxDyStrideSxSy_fp_fps(KerConv_fps_T *Arg);

/* Double precision, Depth Wise */
extern void KerParConvDWDP1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDP3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDP5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDPNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDPNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDPNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Double precision, Depth Wise, double precision bias */
extern void KerParConvDWDP1x1Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x1Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x1StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDP3x1Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x1Stride2x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x3Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x3Stride1x2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP3x3StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDP5x1Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x1Stride2x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x5Stride1x1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP1x5Stride1x2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5Stride1_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5Stride2_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDP5x5StrideS_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDPNxNStrideS_fp_fps(KerConv_fps_T *Arg);
extern void KerParConvDWDPNxMStrideSxSy_fp_fps(KerConv_fps_T *Arg);

extern void KerParConvDWDPNxMDxDyStrideSxSy_fp_fps(KerConv_fps_T *Arg);

/* Double precision */
extern void KerParConv1x1Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x1Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x1StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerParConv3x1Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv3x1Stride2x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x3Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x3Stride1x2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv3x3Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv3x3Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv3x3StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerParConv5x1Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv5x1Stride2x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x5Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv1x5Stride1x2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv5x5Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv5x5Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConv5x5StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerParConvNxNStrideS_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerParConvNxMStrideSxSy_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerParConvNxMDxDyStrideSxSy_DP_fps(KerConv_DP_fps_T *Arg);

/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are half words (_fp)
	A single convolution is evaluated in parallel on all the cores
*/
/* Single precision */
extern void KerConv1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerConv1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerConv1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerConv3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConv3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConv1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConv1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConv3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerConv3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerConv3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerConv5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConv5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConv1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConv1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConv5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerConv5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerConv5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerConvNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerConvNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Single precision, Depth Wise */
extern void KerConvDW1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDW3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConvDW3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDW3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDW3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDW5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDW1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConvDW5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDW5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDW5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDWNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerConvDWNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerConvDWNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Double precision, Depth Wise */
extern void KerConvDWDP1x1Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x1Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x1StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDWDP3x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP3x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x3Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x3Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP3x3Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP3x3Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP3x3StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDWDP5x1Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP5x1Stride2x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x5Stride1x1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP1x5Stride1x2_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP5x5Stride1_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP5x5Stride2_fp(KerConv_fp_T *Arg);
extern void KerConvDWDP5x5StrideS_fp(KerConv_fp_T *Arg);

extern void KerConvDWDPNxNStrideS_fp(KerConv_fp_T *Arg);
extern void KerConvDWDPNxMStrideSxSy_fp(KerConv_fp_T *Arg);

extern void KerConvDWDPNxMDxDyStrideSxSy_fp(KerConv_fp_T *Arg);

/* Double precision */
extern void KerConv1x1Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x1Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x1StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerConv3x1Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv3x1Stride2x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x3Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x3Stride1x2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv3x3Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv3x3Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv3x3StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerConv5x1Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv5x1Stride2x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x5Stride1x1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv1x5Stride1x2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv5x5Stride1_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv5x5Stride2_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConv5x5StrideS_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerConvNxNStrideS_DP_fp(KerConv_DP_fp_T *Arg);
extern void KerConvNxMStrideSxSy_DP_fp(KerConv_DP_fp_T *Arg);

extern void KerConvNxMDxDyStrideSxSy_DP_fp(KerConv_DP_fp_T *Arg);

/*
	1x1, 3x3, 5x5, NxN convolutions with Stride 1, 2 or S.
	Input, Output and filter are bytes(_fps)
	A single convolution is evaluated in parallel on all the cores
*/
/* Single precision */
extern void KerConv1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerConv1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerConv1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerConv3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConv3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConv1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConv1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConv3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerConv3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerConv3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerConv5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConv5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConv1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConv1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConv5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerConv5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerConv5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerConvNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerConvNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Single precision, Depth Wise */
extern void KerConvDW1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDW3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConvDW3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDW3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDW3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDW5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDW1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConvDW5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDW5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDW5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDWNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerConvDWNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerConvDWNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Double precision, Depth Wise */
extern void KerConvDWDP1x1Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x1Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x1StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDWDP3x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP3x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x3Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x3Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP3x3Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP3x3Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP3x3StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDWDP5x1Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP5x1Stride2x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x5Stride1x1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP1x5Stride1x2_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP5x5Stride1_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP5x5Stride2_fps(KerConv_fps_T *Arg);
extern void KerConvDWDP5x5StrideS_fps(KerConv_fps_T *Arg);

extern void KerConvDWDPNxNStrideS_fps(KerConv_fps_T *Arg);
extern void KerConvDWDPNxMStrideSxSy_fps(KerConv_fps_T *Arg);

extern void KerConvDWDPNxMDxDyStrideSxSy_fps(KerConv_fps_T *Arg);

/* Double precision */
extern void KerConv1x1Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x1Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x1StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerConv3x1Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv3x1Stride2x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x3Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x3Stride1x2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv3x3Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv3x3Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv3x3StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerConv5x1Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv5x1Stride2x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x5Stride1x1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv1x5Stride1x2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv5x5Stride1_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv5x5Stride2_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConv5x5StrideS_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerConvNxNStrideS_DP_fps(KerConv_DP_fps_T *Arg);
extern void KerConvNxMStrideSxSy_DP_fps(KerConv_DP_fps_T *Arg);

extern void KerConvNxMDxDyStrideSxSy_DP_fps(KerConv_DP_fps_T *Arg);

/******************************************************************************************************************************/
/**************** LINEAR  RECTIFICATION ***************************************************************************************/
/******************************************************************************************************************************/

/* Feature maps are evaluated in parallel, one per core.
   Feature maps of bytes (_fps) or half words (_fp)
*/
extern void KerParReLU_fp(KerReLUPool_fp_T *Arg);
extern void KerParReLU_fps(KerReLUPool_fps_T *Arg);

/* One Feature map is evaluated in parallel on all cores.
   Feature maps of bytes (_fps) or half words (_fp)
*/
extern void KerReLU_fp(KerReLUPool_fp_T *Arg);
extern void KerReLU_fps(KerReLUPool_fps_T *Arg);


/******************************************************************************************************************************/
/**************** DOUBLE PRECISION TO SINGLE PRECISION REDUCTION **************************************************************/
/******************************************************************************************************************************/

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints */
extern void KerDP_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to same location */
extern void KerDP_IO_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to same location,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_IO_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints,
   each out channel has its own MulBias */
extern void KerDPMulBias_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to same location,
   each out channel has its own MulBias */
extern void KerDPMulBias_IO_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_hswish_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to the same location,
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_IO_hswish_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_hsigmoid_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to the same location,
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_IO_hsigmoid_fp(KerDP_fp_T *Arg);



/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints */
extern void KerDP_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location */
extern void KerDP_IO_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_IO_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints,
   each out channel has its own MulBias */
extern void KerDPMulBias_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location,
   each out channel has its own MulBias */
extern void KerDPMulBias_IO_fps(KerDP_fps_T *Arg);


/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_hswish_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to the same location,
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_IO_hswish_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_hsigmoid_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to the same location,
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_IO_hsigmoid_fps(KerDP_fps_T *Arg);

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
   Feature map is either half word (_fp) or byte (_fps)
   Pool 2x2 Stride 2 as a special case
*/
extern void KerParPool2x2Stride2_fp(KerReLUPool_fp_T *Arg);
extern void KerParPoolNxNStrideS_fp(KerReLUPool_fp_T *Arg);
extern void KerParPoolNxMStrideSxSy_fp(KerReLUPool_fp_T *Arg);
extern void KerParGlobalPoolInit_fp(KerReLUPool_fp_T *Arg);
extern void KerParGlobalPool_fp(KerReLUPool_fp_T *Arg);
extern void KerParGlobalPoolFinal_fp(KerReLUPool_fp_T *Arg);

extern void KerParPool2x2Stride2_fps(KerReLUPool_fps_T *Arg);
extern void KerParPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg);
extern void KerParPoolNxMStrideSxSy_fps(KerReLUPool_fps_T *Arg);
extern void KerParGlobalPoolInit_fps(KerReLUPool_fps_T *Arg);
extern void KerParGlobalPool_fps(KerReLUPool_fps_T *Arg);
extern void KerParGlobalPoolFinal_fps(KerReLUPool_fps_T *Arg);

/* One output feature map is evaluated in parallel on all cores.
   Feature map is either half word (_fp) or byte (_fps)
   Pool 2x2 Stride 2 as a special case
*/
extern void KerPool2x2Stride2_fp(KerReLUPool_fp_T *Arg);
extern void KerPoolNxNStrideS_fp(KerReLUPool_fp_T *Arg);
extern void KerPoolNxMStrideSxSy_fp(KerReLUPool_fp_T *Arg);

extern void KerPool2x2Stride2_fps(KerReLUPool_fps_T *Arg);
extern void KerPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg);
extern void KerPoolNxMStrideSxSy_fps(KerReLUPool_fps_T *Arg);




/******************************************************************************************************************************/
/**************** FULLY CONNECTED *********************************************************************************************/
/******************************************************************************************************************************/

/*
	_fp		: Input, Bias, Filter and Output are half words
	_fps		: Input, Bias, Filter and Output are half bytes
	_fp_fps_fp	: Input, Bias and Output are half words, Filter is byte
	_fp_fp_fpd	: Input, Bias and Filter are half words, Output is word
*/

/* A single output is evaluated in parallel on all cores */
extern void KerLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg);
extern void KerLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg);
extern void KerLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg);
extern void KerLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg);
extern void KerLinearLayerReLU_fps_fps_fp(KerLinearLayerReLU_fps_fps_fp_T *Arg);

/* A single output is evaluated in parallel on all cores, double precision output, need reduction step after */
extern void KerDPLinearLayer_fp(KerDPLinear_fp_T *Arg);
extern void KerDPLinearLayerReduct_fp(KerDPLinearReduct_fp_T *Arg);
extern void KerDPLinearLayer_fps(KerDPLinear_fps_T *Arg);
extern void KerDPLinearLayerReduct_fps(KerDPLinearReduct_fps_T *Arg);

/* Several output are evaluated in parallel, one per core */
extern void KerParLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg);
extern void KerParLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg);
extern void KerParLinearLayerReLU_fp_fps_fp(KerLinearLayerReLU_fp_fps_fp_T *Arg);
extern void KerParLinearLayerReLU_fp_fp_fpd(KerLinearLayerReLU_fp_fp_fpd_T *Arg);


/******************************************************************************************************************************/
/******************* MAT ALGEBRA  *********************************************************************************************/
/******************************************************************************************************************************/

extern void KerParMatAdd_fp(KerMat3_fp_T *Arg);
extern void KerParMatAddDynAdjust_fp(KerMat3_fp_T *Arg);

extern void KerParMatAdd_fps(KerMat3_fps_T *Arg);
extern void KerParMatAddDynAdjust_fps(KerMat3_fps_T *Arg);

extern void KerParMatAddReLU_fp(KerMat3_fp_T *Arg);
extern void KerParMatAddReLU_fps(KerMat3_fps_T *Arg);

extern void KerParMatMul_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMul_fpd_fp(KerMatMul_fpd_fp_T *Arg);
extern void KerParMatMulSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg);

extern void KerParMatMulScaleScalar_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulScaleScalarSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulScaleScalar_fpd_fp(KerMatMul_fpd_fp_T *Arg);
extern void KerParMatMulScaleScalarSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg);

extern void KerParMatMulScale_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulScaleSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulScale_fpd_fp(KerMatMul_fpd_fp_T *Arg);
extern void KerParMatMulScaleSxSy_fpd_fp(KerMatMul_fpd_fp_T *Arg);

extern void KerParMatMul_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulSxSy_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMul_fp_fps(KerMatMul_fp_fps_T *Arg);
extern void KerParMatMulSxSy_fp_fps(KerMatMul_fp_fps_T *Arg);

extern void KerParMatMulScaleScalar_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulScaleScalarSxSy_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulScaleScalar_fp_fps(KerMatMul_fp_fps_T *Arg);
extern void KerParMatMulScaleScalarSxSy_fp_fps(KerMatMul_fp_fps_T *Arg);

extern void KerParMatMulScale_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulScaleSxSy_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulScale_fp_fps(KerMatMul_fp_fps_T *Arg);
extern void KerParMatMulScaleSxSy_fp_fps(KerMatMul_fp_fps_T *Arg);

extern void KerParMatScale_fp(KerMat3_fp_T *Arg);
extern void KerParMatScale_fps(KerMat3_fps_T *Arg);


/******************************************************************************************************************************/
/******************* SOFT MAX *************************************************************************************************/
/******************************************************************************************************************************/

extern void KerParSoftMax_fp(KerSoftMax_fp_T *Arg);
extern void KerParSoftMax_fps(KerSoftMax_fps_T *Arg);


#endif

