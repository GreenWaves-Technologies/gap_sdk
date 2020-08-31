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

#define MAXDPPREC
#ifdef MAXDPPREC
#define DP_fps_T int
#else
#define DP_fps_T short int
#endif

/* Select Normalization with floor or Normalization with rounding */
#ifdef NORM_ROUND
#define AT_NORM(x, n)	gap_roundnorm_reg((x), (n))
#else
#define AT_NORM(x, n)	gap_norm_reg((x), (n))
#endif
#define AT_LSHIFT(x, n) ((x)<<(n))
#define AT_RSHIFT(x, n) ((x)>>(n))
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
	int W,		  	/**< Tile width */
	int H,		  	/**< Tile height */
	int Feat,	       	/**< Number of features */
	int Size,	       	/**< Tile element size in bytes */
	int Pad,		/**< Height or width of the area to be 0 padded */
	int Orientation	 	/**< 0: Horizontal tile, 1: Vertical tile */
	);

void AT_DumpTensor(
	char *NodeName,			/**< Graph Node Name, a User Kernel */
        char *ArgName,			/**< Argument name of this user kernel */
        int Loc,			/**< Exec location if this argument, AT_MEM_xyz */
        void *L3_Device,		/**< Pointer to device descriptor in case Loc is external */
        void *L3_Event,			/**< Pointer to a read event for this device descriptor if any */
        int ItemSize,			/**< Data type size in bytes */
        int Dim,			/**< Number of dimensions, up to 5, from D0 most outer to D4 most inner */
        int D0,				/**< Actual value of this dimension if defined, 1 otherwise */
        int D1,				/**< Actual value of this dimension if defined, 1 otherwise */
        int D2,				/**< Actual value of this dimension if defined, 1 otherwise */
        int D3,				/**< Actual value of this dimension if defined, 1 otherwise */
        int D4,				/**< Actual value of this dimension if defined, 1 otherwise */
        void *L2_BufferAddr,		/**< In case exec loc is external pointer to a buffer in L2 to host partial copy of Arg */
        unsigned int L2_BufferSize,	/**< Size of this buffer */
        void *Addr			/**< Address of Arg */
	);


/********************************************************************************************************************************************************************/
/****************** Bias setting. ***********************************************************************************************************************************/
/********************************************************************************************************************************************************************/

typedef struct {
	signed char * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	signed char * __restrict__ Bias;	/**< Bias */
} KerSetBias_fps_T;

typedef struct {
	short int * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	short int * __restrict__ Bias;		/**< Bias */
} KerSetBias_fp_T;

typedef struct {
	int * __restrict__ Out;			/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	int * __restrict__ Bias;		/**< Bias */
} KerSetBias_fpd_T;

typedef struct {
	int * __restrict__ Out;			/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	short int * __restrict__ Bias;		/**< Bias */
} KerSetBias_fpd_fp_T;

typedef struct {
	int * __restrict__ Out;			/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	signed char * __restrict__ Bias;	/**< Bias */
} KerSetBias_fpd_fps_T;

typedef struct {
	short int * __restrict__ Out;		/**< Output */
	unsigned short int W;			/**< Output width */
	unsigned short int H;			/**< Output height */
	unsigned short int OutFeatures;		/**< Number of features, used for channel parallel kernels */
	signed char NormBias;			/**< Bias correction factor */
	signed char * __restrict__ Bias;	/**< Bias */
} KerSetBias_fp_fps_T;

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
	unsigned char Norm;			/**< ConvOut to Out correction factor */
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
	unsigned char Norm;			/**< ConvOut to Out correction factor */
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
	unsigned short int W;	 	  	/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   		/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of output features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	short int * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	int * __restrict__ Out;   		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< ConvOut to Out correction factor */
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
	unsigned short int W;	   		/**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   		/**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of output features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	short int TotalInFeatures;		/**< For regular conv and conv dp total Input feature space in current tile. For depth wise conv norm factor for bias */
	signed char * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	DP_fps_T * __restrict__ Out;   		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char Norm;			/**< ConvOut to Out correction factor */
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
	int * __restrict__ In;			/**< Input in double precision */
	unsigned short int W;			/**< Input width */
	unsigned short int H;			/**< Output height */
	short int * __restrict__ Out;		/**< Output in single precision */
	unsigned char Norm;			/**< In to Out correction factor */
	signed char NormBias;			/**< Bias to ConvOut correction factor */
	unsigned short int InFeatures;		/**< Number of channels, used only for channel parallel kernels */
	int LB;					/**< If Out<LB then Out=LB */
	int UB;					/**< If Out>UB then Out=UB */
	short int * __restrict__ MulBias;	/**< Output Multiplicative bias */
} KerDP_fp_T;

typedef struct {
	DP_fps_T * __restrict__ In;		/**< Input in double precision */
	unsigned short int W;			/**< Input width */
	unsigned short int H;			/**< Output height */
	signed char * __restrict__ Out;		/**< Output in signle precision */
	unsigned char Norm;			/**< Bias to ConvOut correction factor */
	signed char NormBias;			/**< Precision of the multiplicative bias */
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
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int UsedW;   	/**< Part of the input width to be used */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int UsedH;   	/**< Part of the input height to be used */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	signed char Norm;		/**< To out correction factor */
	signed char NormBias;		/**< To in correction factor */
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
	short int * __restrict__ In;    /**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned short int TileIndex;	/**< Index of the current input tile */
	int * __restrict__ Out;   	/**< Pointer to output tile */
} KerGlobalPoolDP_fp_T;

typedef struct {
	int * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Total input Width */
	unsigned short int H;	   	/**< Total input Height */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned char Norm;		/**< Fixed point format of Out */
	short int * __restrict__ Out;  	/**< Pointer to output tile */
} KerGlobalPoolDPReduct_fp_T;

typedef struct {
	short int * __restrict__ In;   	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned char Norm;		/**< Fixed point format of Out */
	short int * __restrict__ Out;  	/**< Pointer to output tile */
} KerGlobalPool_fp_T;

typedef struct {
	signed char * __restrict__ In;  /**< Pointer to input tile  */
	unsigned short int W;	   /**< Width of the input tile */
	unsigned short int UsedW;	   	/**< Part of the input width to be used */
	unsigned short int H;	   /**< Height of the input tile */
	unsigned short int UsedH;	   	/**< Part of the input height to be used */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	signed char Norm;		/**< To out correction factor */
	signed char NormBias;		/**< To in correction factor */
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

typedef struct {
	signed char * __restrict__ In;  /**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned short int TileIndex;	/**< Index of the current input tile */
	int * __restrict__ Out;   	/**< Pointer to output tile */
} KerGlobalPoolDP_fps_T;

typedef struct {
	int * __restrict__ In;    	/**< Pointer to input tile  */
	unsigned short int W;	   	/**< Total input Width */
	unsigned short int H;	   	/**< Total input Height */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned char Norm;		/**< Fixed point format of Out */
	signed char * __restrict__ Out; /**< Pointer to output tile */
} KerGlobalPoolDPReduct_fps_T;

typedef struct {
	signed char * __restrict__ In;  /**< Pointer to input tile  */
	unsigned short int W;	   	/**< Width of the input tile */
	unsigned short int H;	   	/**< Height of the input tile */
	unsigned short int OutFeatures;	/**< Number of features, used only for channel parallel kernels */
	unsigned char Norm;		/**< Fixed point format of Out */
	signed char * __restrict__ Out; /**< Pointer to output tile */
} KerGlobalPool_fps_T;


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
	signed char NormBias;			/**< Bias Normalization factor */
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
	signed char NormBias;			/**< Bias Normalization factor */
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
	signed char NormBias;			/**< Normalization factor for the bias */
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
	signed char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fps_fps_fp_T;

typedef struct {
	signed char * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	signed char * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	signed char * __restrict__ Bias;	/**< Pointer to bias tile, size is OutSize */
	int * __restrict__ Out;			/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	signed char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fps_fps_fpd_T;

typedef struct {
	short int * __restrict__ In;		/**< Pointer to input tile */
	unsigned short int InSize;		/**< Size of the the tile */
	unsigned short int TotalInSize;		/**< Total input size in case parallelization is performed on outputs */
	unsigned short int OutSize;		/**< Size of the output tile */
	short int * __restrict__ Filter;	/**< Pointer to filter tile, width is TotalInSize */
	short int * __restrict__ Bias;		/**< Pointer to bias tile, size is OutSize */
	int * __restrict__ Out;			/**< Pointer to output tile, size if OutSize */
	unsigned char Norm;			/**< Normalization factor */
	signed char NormBias;			/**< Normalization factor for the bias */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
} KerLinearLayerReLU_fp_fp_fpd_T;

typedef struct {
        short int * __restrict__ In;		/**< Pointer to input tile */
        short int * __restrict__ Filter;	/**< Pointer to Filter tile */
        int * __restrict__ Out;			/**< Pointer to one output or to a vector[N Cores] of intermediate resuts */
        short int InSize;			/**< Number of items in In/Filter */
	char Tile;				/**< Tile index, to control init */
} KerDPLinearLayer_fp_T;

typedef struct {
        short int * __restrict__ In;		/**< Pointer to input tile */
        signed char * __restrict__ Filter;	/**< Pointer to Filter tile */
        int * __restrict__ Out;			/**< Pointer to one output or to a vector[N Cores] of intermediate resuts */
        short int InSize;			/**< Number of items in In/Filter */
	char Tile;				/**< Tile index, to control init */
} KerDPLinearLayer_fp_fps_T;

typedef struct {
        signed char * __restrict__ In;		/**< Pointer to input tile */
        signed char * __restrict__ Filter;	/**< Pointer to Filter tile */
        int * __restrict__ Out;			/**< Pointer to one output or to a vector[N Cores] of intermediate resuts */
        short int InSize;			/**< Number of items in In/Filter */
	char Tile;				/**< Tile index, too control when to initialize Out */
} KerDPLinearLayer_fps_T;

typedef struct {
        int * __restrict__ In;			/**< Pointer to a vector[N Cores] of intermediate resuts */
        short int * __restrict__ Bias;		/**< Pointer to Bias */
        short int * __restrict__ Out;		/**< Pointer to Out */
        int LB;					/**< Min output value */
        int UB;					/**< Max output value */
        char Norm;				/**< Precision, input/output/filter */
        signed char NormBias;			/**< Bias precision */
        char Oper;				/**< Activation operation after linear layer, see CNN_Activation_Oper_T */
} KerDPLinearLayerReduct_fp_T;

typedef struct {
        int * __restrict__ In;			/**< Pointer to a vector[N Cores] of intermediate resuts */
        signed char * __restrict__ Bias;	/**< Pointer to Bias */
        signed char * __restrict__ Out;		/**< Pointer to Out */
        int LB;					/**< Min output value */
        int UB;					/**< Max output value */
        char Norm;				/**< Precision, input/output/filter */
        signed char NormBias;			/**< Bias precision */
        char Oper;				/**< Activation operation after linear layer, see CNN_Activation_Oper_T */
} KerDPLinearLayerReduct_fps_T;

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
	short int *__restrict__ In1;		/**< Input tensor NxHxW */
	short int *__restrict__ In2;		/**< Optional Vector N, scale coefficient to be applied to each of the N channels */
	short int *__restrict__ Out;		/**< Out tensor N*H*W  */
	unsigned short int W;			/**< Input Width */
	unsigned short int H;			/**< Input Height */
	unsigned short int N;			/**< Number of channels/features */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
	short int *__restrict__ ScaleScalar;	/**< Pointer to optional Scalar value to be applied to all channels */
	unsigned char Scale_Q;			/**< Scalar fixed point format */
	unsigned char Norm;			/**< Normalization factor to be applied to result */
} KerMatScale_fp_T;

typedef struct {
	signed char *__restrict__ In1;		/**< Input tensor NxHxW */
	signed char *__restrict__ In2;		/**< Optional Vector N, scale coefficient to be applied to each of the N channels */
	signed char *__restrict__ Out;		/**< Out tensor N*H*W  */
	unsigned short int W;			/**< Input Width */
	unsigned short int H;			/**< Input Height */
	unsigned short int N;			/**< Number of channels/features */
	int LB;					/**< Lower bound for the output */
	int UB;					/**< Upper bound for the output */
	signed char *__restrict__ ScaleScalar;	/**< Pointer to optional Scalar value to be applied to all channels */
	unsigned char Scale_Q;			/**< Scalar fixed point format */
	unsigned char Norm;			/**< Normalization factor to be applied to result */
} KerMatScale_fps_T;

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
	signed char NormBias;			/**< Precision of the bias */
	unsigned char NormMulBias;		/**< Precision of the multiplicative bias */
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
	signed char NormBias;			/**< Precision of the bias */
	unsigned char NormMulBias;		/**< Precision of the multiplicative bias */
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
	signed char NormBias;			/**< Precision of the bias */
	unsigned char NormMulBias;		/**< Precision of the multiplicative bias */
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
	signed char NormBias;			/**< Precision of the bias */
	unsigned char NormMulBias;		/**< Precision of the multiplicative bias */
	unsigned char ColFirst;			/**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;			/**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
} KerMatMul_fp_fps_T;

typedef struct {
	short int *__restrict__ In;		/**< Input matrix */
	short int *__restrict__ Out;		/**< Output matrix */
	unsigned int Feat;			/**< Number of matrices */
	unsigned int W;				/**< Matrix width */
	unsigned int H;				/**< Matrix height */
	unsigned char Sx;			/**< Stride for W dimension */
	unsigned char Sy;			/**< Stride for H dimension */
} KerMatTranspose_fp_T;

typedef struct {
	signed char *__restrict__ In;		/**< Input matrix */
	signed char *__restrict__ Out;		/**< Output matrix */
	unsigned int Feat;			/**< Number of matrices */
	unsigned int W;				/**< Matrix width */
	unsigned int H;				/**< Matrix height */
	unsigned char Sx;			/**< Stride for W dimension */
	unsigned char Sy;			/**< Stride for H dimension */
} KerMatTranspose_fps_T;

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

extern void KerParSetBias_fpd(KerSetBias_fpd_T *Arg);
extern void KerParSetBias_fp(KerSetBias_fp_T *Arg);
extern void KerParSetBias_fps(KerSetBias_fps_T *Arg);
extern void KerParSetBias_fpd_fp(KerSetBias_fpd_fp_T *Arg);
extern void KerParSetBias_fpd_fps(KerSetBias_fpd_fps_T *Arg);
extern void KerParSetBias_fp_fps(KerSetBias_fp_fps_T *Arg);
extern void KerParSetBias_DP_fp(KerSetBias_fpd_fp_T *Arg);
extern void KerParSetBias_DP_fps(KerSetBias_fpd_fps_T *Arg);


extern void KerSetBias_fpd(KerSetBias_fpd_T *Arg);
extern void KerSetBias_fp(KerSetBias_fp_T *Arg);
extern void KerSetBias_fps(KerSetBias_fps_T *Arg);
extern void KerSetBias_fpd_fp(KerSetBias_fpd_fp_T *Arg);
extern void KerSetBias_fpd_fps(KerSetBias_fpd_fps_T *Arg);
extern void KerSetBias_fp_fps(KerSetBias_fp_fps_T *Arg);
extern void KerSetBias_DP_fp(KerSetBias_fpd_fp_T *Arg);
extern void KerSetBias_DP_fps(KerSetBias_fpd_fps_T *Arg);


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
extern void KerParConv7x7StrideS_DP_fp(KerConv_DP_fp_T *Arg);

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
extern void KerParConv7x7StrideS_DP_fps(KerConv_DP_fps_T *Arg);

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
extern void KerConv7x7StrideS_DP_fp(KerConv_DP_fp_T *Arg);

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
extern void KerConv7x7StrideS_DP_fps(KerConv_DP_fps_T *Arg);

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
extern void KerParHswish_fp(KerReLUPool_fp_T *Arg);
extern void KerParHsigmoid_fp(KerReLUPool_fp_T *Arg);

extern void KerParReLU_fps(KerReLUPool_fps_T *Arg);
extern void KerParHswish_fps(KerReLUPool_fps_T *Arg);
extern void KerParHsigmoid_fps(KerReLUPool_fps_T *Arg);

/* One Feature map is evaluated in parallel on all cores.
   Feature maps of bytes (_fps) or half words (_fp)
*/
extern void KerReLU_fp(KerReLUPool_fp_T *Arg);
extern void KerReLU_fps(KerReLUPool_fps_T *Arg);

/******************************************************************************************************************************/
/**************** DOUBLE PRECISION TO SINGLE PRECISION REDUCTION **************************************************************/
/******************************************************************************************************************************/

#define LEAK_CONSTANT_FORMAT    12
#define LEAK_CONSTANT   FP2FIX(0.1, LEAK_CONSTANT_FORMAT)

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
/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output are disjoints
   out is clip((x<0)?(x*0.1):x, 15) */
extern void KerDP_leakyrelu_fp(KerDP_fp_T *Arg);
/* Input is Double precision on 32 bits Qx.2N, Output is Single precision on 16 bits Qx.N, input and output point to the same location,
   out is clip((x<0)?(x*0.1):x, 15) */
extern void KerDP_IO_leakyrelu_fp(KerDP_fp_T *Arg);

/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints */
extern void KerDP_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location */
extern void KerDP_IO_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location,
   same MulBias applied to all out channels */
extern void KerDPMulBiasScalar_IO_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints,
   each out channel has its own MulBias */
extern void KerDPMulBias_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to same location,
   each out channel has its own MulBias */
extern void KerDPMulBias_IO_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_hswish_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to the same location,
   out is (in * ReLU6(in+3))/6 */
extern void KerDP_IO_hswish_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_hsigmoid_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to the same location,
   out is Max(0, Min(1, (x+1)/2)) */
extern void KerDP_IO_hsigmoid_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output are disjoints
   out is clip((x<0)?(x*0.1):x, 7) */
extern void KerDP_leakyrelu_fps(KerDP_fps_T *Arg);
/* Input is Double precision on 16 or 32 bits Qx.2N, Output is Single precision on 8 bits Qx.N, input and output point to the same location,
   out is clip((x<0)?(x*0.1):x, 15) */
extern void KerDP_IO_leakyrelu_fps(KerDP_fps_T *Arg);

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

extern void KerParGlobalMaxPoolDP_fp(KerGlobalPoolDP_fp_T *Arg);
extern void KerParGlobalAvgPoolDP_fp(KerGlobalPoolDP_fp_T *Arg);
extern void KerParGlobalMaxPoolDPReduct_fp(KerGlobalPoolDPReduct_fp_T *Arg);
extern void KerParGlobalAvgPoolDPReduct_fp(KerGlobalPoolDPReduct_fp_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_fp(KerGlobalPool_fp_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_fp(KerGlobalPool_fp_T *Arg);

extern void KerParPool2x2Stride2_fps(KerReLUPool_fps_T *Arg);
extern void KerParPoolNxNStrideS_fps(KerReLUPool_fps_T *Arg);
extern void KerParPoolNxMStrideSxSy_fps(KerReLUPool_fps_T *Arg);

extern void KerParGlobalMaxPoolDP_fps(KerGlobalPoolDP_fps_T *Arg);
extern void KerParGlobalAvgPoolDP_fps(KerGlobalPoolDP_fps_T *Arg);
extern void KerParGlobalMaxPoolDPReduct_fps(KerGlobalPoolDPReduct_fps_T *Arg);
extern void KerParGlobalAvgPoolDPReduct_fps(KerGlobalPoolDPReduct_fps_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_fps(KerGlobalPool_fps_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_fps(KerGlobalPool_fps_T *Arg);

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
extern void KerDPLinearLayer_fp(KerDPLinearLayer_fp_T *Arg);
extern void KerDPLinearLayer_fp_fps(KerDPLinearLayer_fp_fps_T *Arg);
extern void KerDPLinearLayerReduct_fp(KerDPLinearLayerReduct_fp_T *Arg);
extern void KerDPLinearLayer_fps(KerDPLinearLayer_fps_T *Arg);
extern void KerDPLinearLayerReduct_fps(KerDPLinearLayerReduct_fps_T *Arg);

/* Several output are evaluated in parallel, one per core */
extern void KerParLinearLayerReLU_fp(KerLinearLayerReLU_fp_T *Arg);
extern void KerParLinearLayerHswish_fp(KerLinearLayerReLU_fp_T *Arg);
extern void KerParLinearLayerHsigmoid_fp(KerLinearLayerReLU_fp_T *Arg);

extern void KerParLinearLayerReLU_fps(KerLinearLayerReLU_fps_T *Arg);
extern void KerParLinearLayerHswish_fps(KerLinearLayerReLU_fps_T *Arg);
extern void KerParLinearLayerHsigmoid_fps(KerLinearLayerReLU_fps_T *Arg);

extern void KerParLinearLayerReLU_fps_fps_fp(KerLinearLayerReLU_fps_fps_fp_T *Arg);
extern void KerParLinearLayerReLU_fps_fps_fpd(KerLinearLayerReLU_fps_fps_fpd_T *Arg);
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

extern void KerParMatMulHswish_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulHswishSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulHswish_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulHswishSxSy_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulHsigmoid_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulHsigmoidSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulHsigmoid_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulHsigmoidSxSy_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulLeakyrelu_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulLeakyreluSxSy_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulLeakyrelu_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulLeakyreluSxSy_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulSmallFeat_fp(KerMatMul_fp_T *Arg);

extern void KerParMatMulHswishSmallFeat_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulHsigmoidSmallFeat_fp(KerMatMul_fp_T *Arg);
extern void KerParMatMulLeakyreluSmallFeat_fp(KerMatMul_fp_T *Arg);

extern void KerParMatMulScaleScalarSmallFeat_fp(KerMatMul_fp_T *Arg);

extern void KerParMatMulScaleSmallFeat_fp(KerMatMul_fp_T *Arg);

extern void KerParMatMulSmallFeat_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulHswishSmallFeat_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulHsigmoidSmallFeat_fps(KerMatMul_fps_T *Arg);
extern void KerParMatMulLeakyreluSmallFeat_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulScaleScalarSmallFeat_fps(KerMatMul_fps_T *Arg);

extern void KerParMatMulScaleSmallFeat_fps(KerMatMul_fps_T *Arg);

extern void KerParMatScaleVector_fp(KerMatScale_fp_T *Arg);

extern void KerParMatScaleVector_fps(KerMatScale_fps_T *Arg);

extern void KerParMatScaleScalar_fp(KerMatScale_fp_T *Arg);

extern void KerParMatScaleScalar_fps(KerMatScale_fps_T *Arg);

extern void KerParMatScaleVectorScalar_fp(KerMatScale_fp_T *Arg);

extern void KerParMatScaleVectorScalar_fps(KerMatScale_fps_T *Arg);

extern void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg);

extern void CNN_ParTranspose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_ParTransposeSxSy_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_Transpose_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_TransposeSxSy_fp(KerMatTranspose_fp_T *Arg);

extern void CNN_MatPermCHW2CWH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HWC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WHC_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2WCH_fp(KerMatTranspose_fp_T *Arg);
extern void CNN_MatPermCHW2HCW_fp(KerMatTranspose_fp_T *Arg);

extern void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg);
extern void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg);

/******************************************************************************************************************************/
/******************* SOFT MAX *************************************************************************************************/
/******************************************************************************************************************************/

extern void KerParSoftMax_fp(KerSoftMax_fp_T *Arg);
extern void KerParSoftMax_fps(KerSoftMax_fps_T *Arg);


#endif

