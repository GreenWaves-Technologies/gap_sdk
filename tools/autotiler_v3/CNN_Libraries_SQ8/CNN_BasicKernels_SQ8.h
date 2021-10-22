#ifndef __CNN_BASICKERNELS_SQ8__
#define __CNN_BASICKERNELS_SQ8__
#include "Gap.h"
#include "../CNN_Libraries/CNN_Defines.h"
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

#include "../CNN_Libraries/CNN_Copy.h"
#include "CNN_AT_Misc.h"

#ifdef GENASM
#ifdef __EMUL__
#define gap_ncore()     8
#define gap_coreid()    __builtin_pulp_CoreId()
#endif
#endif

#define AT_INF_BIASL_SM         0
#define AT_INF_ACTSCALE		0
#define AT_INF_ACTSCALEN	1
#define AT_INF_A0		2
#define AT_INF_B0		3
#define AT_INF_C0		4

#define AT_INF_BIASN		5
#define AT_INF_IN1SCALE		5
#define AT_INF_SCALE		5

#define AT_INF_SCALEN		6
#define AT_INF_IN1SCALEN	6

#define AT_INF_OUTSCALE		7
#define AT_INF_OUTSCALEN	8

#define AT_INF_DIM		9

#define AT_INF_PRENORM          9
#define AT_INF_SQ16_DIM         10

#define AT_INF_ADD_BIAS		9
#define AT_INF_ASYM_ADD_DIM	11

#ifndef Prec
#define Prec 			(10)
#endif

typedef enum {
        ACT_NONE = 0,
        ACT_RELU,
        ACT_RELUN,
        ACT_HSIGMOID,
        ACT_HSWISH,
        ACT_LEAKYRELU,
        ACT_SIGMOID,
        ACT_TANH,
		ACT_RELUM,
		ACT_RELUMN
} CNN_ActivationOper_T;

/******************************************************************************************************************
	Bias setting for convolution and linear layers
******************************************************************************************************************/
typedef struct {
	int *__restrict__ Out;
	unsigned short int W;
	unsigned short int H;
	unsigned short int Feat;
	void *__restrict__ Bias;
	unsigned char NormBias;
} KerSetBias_SQ8_T;

/******************************************************************************************************************
	Linear, Bias 32b, 16b or 8b
	1) 32b output with Bias assumed to be set in Out before, need channel scaling and reduction after
	2) Output can be fully evaluated (FullFeat) then Bias setting scaling and activation (ReLU, ReLUN) is
	performed in a single shot
******************************************************************************************************************/
typedef struct {
	signed char * __restrict__ In;
	signed char * __restrict__ Weights;
	void *__restrict__ Bias;
	int * __restrict__ Out;
	unsigned short int InDim;
	unsigned short int TotalInDim;
	unsigned short int OutDim;
	unsigned char *__restrict__ Scale;
	unsigned char *__restrict__ ScaleN;
	signed char *__restrict__ Infos;
} KerLinear_SQ8_T;

/******************************************************************************************************************
	Point Wise and Depth Convolution, 32b output
	Bias assumed to be set in Out before for Point Wise conv, directly managed for depth wise conv
	Need scaling and reduction after
******************************************************************************************************************/

typedef struct {
	signed char * __restrict__ In;		/**< Pointer to input tile  */
	unsigned short int W;			/**< Width of the input tile */
	unsigned short int UsedW;		/**< Part of the input width to be used */
	unsigned short int H;			/**< Height of the input tile */
	unsigned short int UsedH;		/**< Part of the input height to be used */
	unsigned short int InFeatures;		/**< Number of input features, used for channel parallel kernels */
	unsigned short int OutFeatures;		/**< Number of output features, used for channel parallel kernels */
	unsigned short int TotalInFeatures;	/**< Total Input feature space in current tile */
	signed char * __restrict__ Filter;      /**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	signed char * __restrict__ Bias;	/**< Pointer to bias tile, used when convolution is depth wise */
	int * __restrict__ Out;			/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	v4s Pad;				/**< Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom */
	unsigned char NormBias;			/**< Bias init correction factor */
	unsigned char Orientation;		/**< Tile orientation: 1 => Horizontal, 0 => Vertical, used only for feature parallel kernels */
	unsigned short N;			/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;			/**< Output stride, S, used only for general versions */
	unsigned char D;			/**< Dilation Dx */
	unsigned short Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_SQ8_T;

/******************************************************************************************************************
	Point wise Convolution, Implementation based on Imm2Col transformation. 8b output
******************************************************************************************************************/

typedef struct {
        signed char * __restrict__ In;          /**< Second input matrix tile, convolution features */
        unsigned short int W;                   /**< Feature Width */
        unsigned short int H;                   /**< Feature Height */
        unsigned char Fx;                       /**< Filter width */
        unsigned char Fy;                       /**< Filter height */
        unsigned char Sx;                       /**< Filter horizontal stride */
        unsigned char Sy;                       /**< Filter vertical stride */
	unsigned char Dx;			/**< Filter horizontal dilation */
	unsigned char Dy;			/**< Filter vertical dilation */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
        v4s Pad;                                /**< Filter Pad */
        signed char * __restrict__ Filter;      /**< First input matrix tile, convolution weights */
        int * __restrict__ Bias;                /**< Bias tensor */
        signed char * __restrict__ Out;         /**< Output tensor */
        unsigned short int InFeat;              /**< Input Features: First input matrix tile width */
        unsigned short int OutFeat;             /**< OutputFeatures: First input matrix tile height */
        unsigned short int Wo;			/**< Output tile width */
        unsigned short int Ho;			/**< Output tile height */
        unsigned char * __restrict__ Scale;     /**< Scale Factor to be applied after convolution */
        unsigned char * __restrict__ ScaleN;    /**< Normalization Factor to be applied after scaling */
        signed char * __restrict__ ColBuff;     /**< Temp Buffer, must be at least Align(InFeat*Fx, 8) */
        signed char * __restrict__ Infos;       /**< Scaling and constants data */
} Ker_MM_Conv_SQ8_T;


typedef struct {
        signed char * __restrict__ In;          /**< Second input matrix tile, convolution features */
        unsigned short int W;                   /**< Feature Width */
        unsigned short int H;                   /**< Feature Height */
        unsigned char Fx;                       /**< Filter width */
        unsigned char Fy;                       /**< Filter height */
        unsigned char Sx;                       /**< Filter horizontal stride */
        unsigned char Sy;                       /**< Filter vertical stride */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
        v4s Pad;                                /**< Filter Pad */
        signed char * __restrict__ Out;         /**< Output tensor */
        unsigned short int Feat;              	/**< Input Features */
        unsigned short int Wo;			/**< Output tile width */
        unsigned short int Ho;			/**< Output tile height */
} Ker_MM_Pool_SQ8_T;

typedef struct {
        unsigned char * __restrict__ In;          /**< Second input matrix tile, convolution features */
        unsigned short int W;                   /**< Feature Width */
        unsigned short int H;                   /**< Feature Height */
        unsigned char Fx;                       /**< Filter width */
        unsigned char Fy;                       /**< Filter height */
        unsigned char Sx;                       /**< Filter horizontal stride */
        unsigned char Sy;                       /**< Filter vertical stride */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
        v4s Pad;                                /**< Filter Pad */
        unsigned char * __restrict__ Out;         /**< Output tensor */
        unsigned short int Feat;              	/**< Input Features */
        unsigned short int Wo;			/**< Output tile width */
        unsigned short int Ho;			/**< Output tile height */
} Ker_MM_Pool_USQ8_T;

typedef struct {
        signed short * __restrict__ In;          /**< Second input matrix tile, convolution features */
        unsigned short int W;                   /**< Feature Width */
        unsigned short int H;                   /**< Feature Height */
        unsigned char Fx;                       /**< Filter width */
        unsigned char Fy;                       /**< Filter height */
        unsigned char Sx;                       /**< Filter horizontal stride */
        unsigned char Sy;                       /**< Filter vertical stride */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
        v4s Pad;                                /**< Filter Pad */
        signed short * __restrict__ Out;         /**< Output tensor */
        unsigned short int Feat;              	/**< Input Features */
        unsigned short int Wo;			/**< Output tile width */
        unsigned short int Ho;			/**< Output tile height */
} Ker_MM_Pool_SQ16_T;

typedef struct {
        unsigned short * __restrict__ In;          /**< Second input matrix tile, convolution features */
        unsigned short int W;                   /**< Feature Width */
        unsigned short int H;                   /**< Feature Height */
        unsigned char Fx;                       /**< Filter width */
        unsigned char Fy;                       /**< Filter height */
        unsigned char Sx;                       /**< Filter horizontal stride */
        unsigned char Sy;                       /**< Filter vertical stride */
	unsigned char FirstTile;		/**< 1 if we are on the first tile */
        v4s Pad;                                /**< Filter Pad */
        unsigned short * __restrict__ Out;         /**< Output tensor */
        unsigned short int Feat;              	/**< Input Features */
        unsigned short int Wo;			/**< Output tile width */
        unsigned short int Ho;			/**< Output tile height */
} Ker_MM_Pool_USQ16_T;

/******************************************************************************************************************
	Reduction scaling and activation after double precision convolution or linear layer
******************************************************************************************************************/
typedef struct {
	int *__restrict__ In;
	void *__restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int H;
	unsigned char * __restrict__ Scale;
	unsigned char * __restrict__ ScaleN;
	signed char * __restrict__ Infos;
} KerConvLinReduct_SQ8_T;

/******************************************************************************************************************
	Standalone scaling and activation
******************************************************************************************************************/
typedef struct {
	signed char *__restrict__ In;
	signed char *__restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int H;
	signed char * __restrict__ Infos;
} KerActivation_SQ8_T;

typedef struct {
	unsigned char *__restrict__ In;
	unsigned char *__restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int H;
	signed char * __restrict__ Infos;
} KerActivation_USQ8_T;

/******************************************************************************************************************
	Standalone scaling and activation for HWC layer layout
******************************************************************************************************************/
typedef struct {
	void *__restrict__ In;
	void *__restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int H;
	signed char * __restrict__ Infos;
} KerActivation_HWC_SQ8_T;

/******************************************************************************************************************
	Pooling followed by optional scaling and activation
******************************************************************************************************************/
typedef struct {
	unsigned char * __restrict__ In;
	unsigned char * __restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int UsedW;
	unsigned short int H;
	unsigned short int UsedH;
	unsigned short FS;		/* Filter Size, x */
	unsigned short FSy;		/* Filter Size, y */
	unsigned char S;		/* Filter Stride, x */
	unsigned char Sy;		/* Filter Stride, y */
	v4s Pad;
	signed char * __restrict__ Infos;
} KerPool_HWC_USQ8_T;


/******************************************************************************************************************
	Pooling followed by optional scaling and activation
******************************************************************************************************************/
typedef struct {
	signed char * __restrict__ In;
	signed char * __restrict__ Out;
	unsigned short int Feat;
	unsigned short int W;
	unsigned short int UsedW;
	unsigned short int H;
	unsigned short int UsedH;
	unsigned char PoolMax;
	unsigned short FS;		/* Filter Size, x */
	unsigned short FSy;		/* Filter Size, y */
	unsigned char S;		/* Filter Stride, x */
	unsigned char Sy;		/* Filter Stride, y */
	unsigned char D;
	unsigned char Dy;
	unsigned char Orientation;
	unsigned char DoScale;
	v4s Pad;
	signed char * __restrict__ Infos;
} KerPool_SQ8_T;

/******************************************************************************************************************
	Global average Pooling followed by optional scaling and activation
******************************************************************************************************************/
typedef struct {
        void * __restrict__ In;		/**< Pointer to input tile  */
        unsigned short int W;		/**< Width of the input tile */
        unsigned short int H;		/**< Height of the input tile */
        unsigned short int Feat; 	/**< Number of features, used only for channel parallel kernels */
        unsigned char FirstTile;	/**< 1 if this tile is the first one of its group */
	unsigned char DoScale;		/**< Apply Scaling */
        void * __restrict__ Out;	/**< Pointer to output tile */
	signed char * __restrict__ Infos;
} KerGlobalPool_SQ8_T;


/******************************************************************************************************************
	Tensor multiplication used for 1x1 convolution. Channel centric scaling.
******************************************************************************************************************/

typedef struct {
	signed char * __restrict__ In1;         /**< First input matrix tile, convolution weights */
	unsigned short int W_In1;               /**< First input matrix tile width */
	unsigned short int H_In1;               /**< First input matrix tile height */
	signed char * __restrict__ In2;         /**< Second input matrix tile, convolution features */
	unsigned short int W_In2;               /**< Second input matrix tile width, height is by construction H_In1 */
	void * __restrict__ Bias;        	/**< Bias input tile, convolution bias */
	unsigned char * __restrict__ Scale;   	/**< Scale Factor to be applied after convolution */
	unsigned char * __restrict__ ScaleN;   	/**< Normalization Factor to be applied after scaling */
	signed char * __restrict__ Out;         /**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;               /**< Output matrix full width */
	unsigned short int OutFirstCol;         /**< Equal M2FirstCol */
	signed char * __restrict__ BufferColIn2;/**< In case vectorization is used will be used to copy a column of In2 into a line */
	unsigned char NormBias;                 /**< Normalization factor to be applied to Bias */
	unsigned char ColFirst;                 /**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;                       /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;                       /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;                   /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;                   /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerMatMul_SQ8_T;

/* Per Layer Quantized Matrix Mul */
typedef struct {
	signed char * __restrict__ In1;         /**< First input matrix tile, convolution weights */
	unsigned short int W_In1;               /**< First input matrix tile width */
	unsigned short int H_In1;               /**< First input matrix tile height */
	signed char * __restrict__ In2;         /**< Second input matrix tile, convolution features */
	unsigned short int W_In2;               /**< Second input matrix tile width, height is by construction H_In1 */
	void * __restrict__ Bias;        	/**< Bias input tile, convolution bias */
	unsigned char Scale;  		 	/**< Scale Factor to be applied after convolution */
	unsigned char ScaleN; 		  	/**< Normalization Factor to be applied after scaling */
	signed char * __restrict__ Out;         /**< Output matrix tile, W=W_In2, H=H_In1 by construction */
	unsigned short int W_Out;               /**< Output matrix full width */
	unsigned short int OutFirstCol;         /**< Equal M2FirstCol */
	signed char * __restrict__ BufferColIn2;/**< In case vectorization is used will be used to copy a column of In2 into a line */
	unsigned char NormBias;                 /**< Normalization factor to be applied to Bias */
	unsigned char ColFirst;                 /**< 1 if product is formed with a vertical tile from In1 and a horizontal from In2, 0 if Hor tile In1 Ver tile In2 */
	unsigned char Sx;                       /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sx applies to W and Sy to H */
	unsigned char Sy;                       /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], Sy applies to W and Sy to H */
	unsigned short int W;                   /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], W */
	unsigned short int H;                   /**< When used for 1x1 convolution In2 is a feature maps [H_In2=W_In1=InFeat, W_In2=W*H], H */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerMatMul_PL_SQ8_T;

/******************************************************************************************************************
	Tensor addition with optional In1 and Output scaling
******************************************************************************************************************/

typedef struct {
	signed char * __restrict__ In1;		/**< First input tensor */
	signed char * __restrict__ In2;		/**< Second input tensor */
	signed char * __restrict__ Out;		/**< Onput tensor */
	unsigned short int Feat;		/**< Number of features */
	unsigned short int W;			/**< Feature width */
	unsigned short int H;			/**< Feature height */
	unsigned char DoScale;			/**< Apply Scaling */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerMat3_SQ8_T;

typedef struct {
	unsigned char * __restrict__ In1;	/**< First input tensor */
	unsigned char * __restrict__ In2;	/**< Second input tensor */
	unsigned char * __restrict__ Out;	/**< Onput tensor */
	unsigned short int Feat;		/**< Number of features */
	unsigned short int W;			/**< Feature width */
	unsigned short int H;			/**< Feature height */
	unsigned char DoScale;			/**< Apply Scaling */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerMat3_USQ8_T;

/******************************************************************************************************************
	SoftMax, Q15 output
******************************************************************************************************************/

typedef struct {
	signed char *__restrict__ In;           /**< Pointer to input tile */
	unsigned short int Feat;                /**< Number of features of the tile */
	unsigned short int N;                   /**< Size of the tile */
	unsigned short int Norm;                /**< Normalization factor */
	short int *__restrict__ Out;            /**< Pointer to output tile */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerSoftMax_SQ8_T;

/******************************************************************************************************************
 	Recursive NN (RNN, LSTM, GRU)
******************************************************************************************************************/

#define RNN_F_INF              8
#define RNN_F_OFF              0
#define RNN_F_SCALE            0
#define RNN_F_SCALEN           1
#define RNN_F_A0               2
#define RNN_F_B0               3

#define RNN_F_IN_SCALE         4
#define RNN_F_IN_SCALEN        5
#define RNN_OUT_SCALE          6
#define RNN_OUT_SCALEN         7

#define RNN_CELL_INFOS (RNN_F_INF)

typedef struct {
	signed char *__restrict__ StateInOut;	/**< Pointer to In/Out state, Dim=DimState   */
	signed char *__restrict__ State;	/**< Pointer to to a copy of state with extra space for in (DimState+DimIn)   */
	signed char *__restrict__ Xin;		/**< Pointer to In state, Dim=DimIn */
	unsigned short int DimState;		/**< State dimension */
	unsigned short int DimIn;		/**< Input dimension */
	signed char *__restrict__ Wf;		/**< Pointer to Forget gate (whether to erase cell) weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bf;			/**< Pointer to Forget gate bias */
	signed char *__restrict__ Hout;		/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;		/**< Number of output produced in StateInOut */
	unsigned short int OutBase;		/**< Index of first output produced in StateInOut */
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< 1 if RNN State has to be reset */
	int TileOffset;				/**< Buffer Offset related to the current Tile index */
} KerRNN_SQ8_T;

#define LSTM_F_INF              2
#define LSTM_F_OFF              0
#define LSTM_F_SCALE            0
#define LSTM_F_SCALEN           1

#define LSTM_I_INF              2
#define LSTM_I_OFF              (LSTM_F_OFF+LSTM_F_INF)
#define LSTM_I_SCALE            (0 + LSTM_I_OFF)
#define LSTM_I_SCALEN           (1 + LSTM_I_OFF)

#define LSTM_G_INF              2
#define LSTM_G_OFF              (LSTM_I_OFF+LSTM_I_INF)
#define LSTM_G_SCALE            (0 + LSTM_G_OFF)
#define LSTM_G_SCALEN           (1 + LSTM_G_OFF)

#define LSTM_O_INF              2 
#define LSTM_O_OFF              (LSTM_G_OFF+LSTM_G_INF)
#define LSTM_O_SCALE            (0 + LSTM_O_OFF)
#define LSTM_O_SCALEN           (1 + LSTM_O_OFF)

#define LSTM_COUT_INF           6 
#define LSTM_COUT_OFF           (LSTM_O_OFF+LSTM_O_INF)
#define LSTM_CIN_SCALE          (0 + LSTM_COUT_OFF)
#define LSTM_CIN_SCALEN         (1 + LSTM_COUT_OFF)
#define LSTM_COUT_SCALE         (2 + LSTM_COUT_OFF)
#define LSTM_COUT_SCALEN        (3 + LSTM_COUT_OFF)
#define LSTM_OUT_SCALE          (4 + LSTM_COUT_OFF)
#define LSTM_OUT_SCALEN         (5 + LSTM_COUT_OFF)

#define LSTM_INT_INF            7
#define LSTM_INT_OFF            (LSTM_COUT_OFF+LSTM_COUT_INF)
#define LSTM_INT_A0             (0 + LSTM_INT_OFF)
#define LSTM_INT_B0             (2 + LSTM_INT_OFF)
#define LSTM_INT_C0             (4 + LSTM_INT_OFF)
#define LSTM_INT_Q              (6 + LSTM_INT_OFF)

#define LSTM_X_IN_INF           8
#define LSTM_X_IN_OFF           (LSTM_INT_OFF+LSTM_INT_INF)
#define LSTM_F_IN_SCALE         (0 + LSTM_X_IN_OFF)
#define LSTM_F_IN_SCALEN        (1 + LSTM_X_IN_OFF)
#define LSTM_I_IN_SCALE         (2 + LSTM_X_IN_OFF)
#define LSTM_I_IN_SCALEN        (3 + LSTM_X_IN_OFF)
#define LSTM_G_IN_SCALE         (4 + LSTM_X_IN_OFF)
#define LSTM_G_IN_SCALEN        (5 + LSTM_X_IN_OFF)
#define LSTM_O_IN_SCALE         (6 + LSTM_X_IN_OFF)
#define LSTM_O_IN_SCALEN        (7 + LSTM_X_IN_OFF)

//#define LSTM_CELL_INFOS 	    (LSTM_F_INF+LSTM_I_INF+LSTM_G_INF+LSTM_O_INF+LSTM_COUT_INF+LSTM_INT_INF)
#define LSTM_CELL_INFOS  (LSTM_F_INF+LSTM_I_INF+LSTM_G_INF+LSTM_O_INF+LSTM_COUT_INF+LSTM_INT_INF+LSTM_X_IN_INF)

typedef struct {
	signed char *__restrict__ StateInOut;	/**< Pointer to In/Out state, Dim=DimState   */
	signed char *__restrict__ CInOut;	/**< Pointer to In/Out State C (LSTM), DimIn = DimOut = DimState */
	signed char *__restrict__ State;	/**< Pointer to to a copy of state with extra space for in (2*DimState+DimIn)   */
	signed char *__restrict__ Xin;		/**< Pointer to In state, Dim=DimIn */
	unsigned short int DimState;		/**< State dimension */
	unsigned short int DimIn;		/**< Input dimension */
	signed char *__restrict__ Wf;		/**< Pointer to Forget gate (whether to erase cell) weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bf;			/**< Pointer to Forget gate bias */
	signed char *__restrict__ Wi;		/**< Pointer to Input gate (whether to write cell) weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bi;			/**< Pointer to Input gate bias */
	signed char *__restrict__ Wg;		/**< Pointer to Gate gate (how much to write to cell) weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bg;			/**< Pointer to Gate gate bias */
	signed char *__restrict__ Wo;		/**< Pointer to Output gate (how much to reveal cell) weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bo;			/**< Pointer to Output gate bias */
	signed char *__restrict__ Hout;		/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;		/**< Number of output produced in StateInOut and CInOut */
	unsigned short int OutBase;		/**< Index of first output produced in StateInOut and CInOut*/
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< If 1 LSTM State is reset */
	int TileOffset;				/**< Buffer Offset related to the current Tile index */
} KerLSTM_SQ8_T;


#define GRU_R_INF              4
#define GRU_R_OFF              0
#define GRU_R_INT_SCALE        0
#define GRU_R_INT_SCALEN       1
#define GRU_R_IN_SCALE         2
#define GRU_R_IN_SCALEN        3

#define GRU_Z_INF              4
#define GRU_Z_OFF              (GRU_R_OFF+GRU_R_INF)
#define GRU_Z_INT_SCALE        (0 + GRU_Z_OFF)
#define GRU_Z_INT_SCALEN       (1 + GRU_Z_OFF)
#define GRU_Z_IN_SCALE         (2 + GRU_Z_OFF)
#define GRU_Z_IN_SCALEN        (3 + GRU_Z_OFF)

#define GRU_HT_INF              2
#define GRU_HT_OFF              (GRU_Z_OFF+GRU_Z_INF)
#define GRU_HT_IN_SCALE         (0 + GRU_HT_OFF)
#define GRU_HT_IN_SCALEN        (1 + GRU_HT_OFF)

#define GRU_H_INF				2
#define GRU_H_OFF				(GRU_HT_OFF+GRU_HT_INF)
#define GRU_H_INT_SCALE			(0 + GRU_H_OFF)
#define GRU_H_INT_SCALEN		(1 + GRU_H_OFF)

#define GRU_INT_INF             7
#define GRU_INT_OFF             (GRU_H_OFF+GRU_H_INF)
#define GRU_INT_A0              (0 + GRU_INT_OFF)
#define GRU_INT_B0              (2 + GRU_INT_OFF)
#define GRU_INT_C0              (4 + GRU_INT_OFF)
#define GRU_INT_Q               (6 + GRU_INT_OFF)

#define GRU_CELL_INFOS (GRU_R_INF+GRU_Z_INF+GRU_HT_INF+GRU_H_INF+GRU_INT_INF)

typedef struct {
	signed char *__restrict__ StateInOut;	/**< Pointer to In/Out state, Dim=DimState   */
	signed char *__restrict__ Xin;		/**< Pointer to In state, Dim=DimIn */
	signed char *__restrict__ State;	/**< Pointer to to a copy of state with extra space for in (DimState+DimIn)   */
	unsigned short int DimState;		/**< State dimension */
	unsigned short int DimIn;		/**< Input dimension */
	signed char *__restrict__ Wr;		/**< Pointer to R gate weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Br;			/**< Pointer to R gate bias */
	signed char *__restrict__ Wz;		/**< Pointer to Z gate weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bz;			/**< Pointer to Z gate bias */
	signed char *__restrict__ Wh;		/**< Pointer to H gate weights, Dim=[DimState+DimIn,DimState] */
	void * __restrict__ Bwh;		/**< Pointer to H gate bias vs Inputs */
	void * __restrict__ Brh;		/**< Pointer to H gate bias vs States */
	signed char *__restrict__ Hout;		/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;		/**< Number of output produced in StateInOut */
	unsigned short int OutBase;		/**< Index of first output produced in StateInOut */
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< If 1 GRU State is reset */
	int TileOffset;				/**< Buffer Offset related to the current Tile index */
} KerGRU_SQ8_T;


/******************************************************************************************************************
	Bias setting for convolution and linear layers, output is 32b, input is 8,16 or 32b
******************************************************************************************************************/

/* Features in parallel */
extern void KerParSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerParSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerParSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg);

/* Each feature in parallel */
extern void KerSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg);

/* Features in parallel, HWC Layout */
extern void KerParSetBiasB32_HWC_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerParSetBiasB16_HWC_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerParSetBiasB8_HWC_SQ8(KerSetBias_SQ8_T *Arg);

/* Each feature in parallel, HWC Layout */
extern void KerSetBiasB32_HWC_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerSetBiasB16_HWC_SQ8(KerSetBias_SQ8_T *Arg);
extern void KerSetBiasB8_HWC_SQ8(KerSetBias_SQ8_T *Arg);

/******************************************************************************************************************
	Convolution, Bias is assigned separately. Output is 32 bits. Parallel Features and Features Parallel
******************************************************************************************************************/

extern void KerParConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv4x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x4Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv13x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv4x10StrideSxSy_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv1D_NStrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvNx1StrideSxS1_SQ8(KerConv_SQ8_T *Arg);

extern void KerParConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3DxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3DxDyStride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D2x2Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D4x4Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D8x8Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D2x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D4x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3D8x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConv3x3DxD2Stride1x1_SQ8(KerConv_SQ8_T *Arg);

extern void KerPar_MM_Conv2D_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);

extern void KerConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg);

extern void KerConv1D_NStrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvNx1StrideSxS1_SQ8(KerConv_SQ8_T *Arg);

extern void KerConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg);


/******************************************************************************************************************
	Depth Wise convolution, 8b, 16b and 32b Bias. Output is 32 bits. Parallel Features and Features Parallel
******************************************************************************************************************/

/* 8 Bits Bias, Features in parallel */
extern void KerParConvDW1x1Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW7x7StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxNStrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMDxDyStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);

/* 16 Bits Bias, Features in parallel */
extern void KerParConvDW1x1Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW7x7StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxNStrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMDxDyStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);

/* 32 Bits Bias, Features in parallel */
extern void KerParConvDW1x1Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x1StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x3Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW3x3StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW1x5Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW5x5StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDW7x7StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxNStrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerParConvDWNxMDxDyStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);

/* 8 Bits Bias, One Feature in parallel */
extern void KerConvDW1x1Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride1B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride2B8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW7x7StrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxNStrideSB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMDxDyStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);

/* 16 Bits Bias, One Feature in parallel */
extern void KerConvDW1x1Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride1B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride2B16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW7x7StrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxNStrideSB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMDxDyStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);

/* 32 Bits Bias, One Feature in parallel */
extern void KerConvDW1x1Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x1StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x3Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW3x3StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW1x5Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride1B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5Stride2B32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW5x5StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDW7x7StrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxNStrideSB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);
extern void KerConvDWNxMDxDyStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);

/******************************************************************************************************************
          Input Scaling followed by an optional activation. Parallel Feature, Feature Parallel
	  Input is assumed to be the 32b unnormalized output of a convolution or a linear layer
	  Optional activation is applied to the scaled input and can be optionaly scaled also
	  Output is a scaled 8b quantized tensor
	  Channel Centric (CC)
******************************************************************************************************************/

extern unsigned short int SIGMOID_LUT_uint16[];

int SigmoidTable(int x, unsigned short int * table);
int TanhTable(int x, unsigned short int * table);

#define Tanh(__x) TanhTable((__x), SIGMOID_LUT_uint16)
#define Sigmoid(__x) SigmoidTable((__x), SIGMOID_LUT_uint16)

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated in parallel
*/
extern void KerParReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

extern void KerParReduct_CC_CHW2HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_CHW2HWC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated in parallel
*/
extern void KerParReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReductIO_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated one after the other in parallel
*/
extern void KerReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated one after the other in parallel
*/
extern void KerReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReductIO_CC_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

/* HWC Layout */
extern void KerParReduct_CC_ReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUM_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUMN_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSwish_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_LeakyReLU_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Sigmoid_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Tanh_HWC_SQ8(KerConvLinReduct_SQ8_T *Arg);

extern void KerParReduct_CC_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUM_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUMN_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSwish_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_LeakyReLU_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Sigmoid_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Tanh_HWC_SQ16(KerConvLinReduct_SQ8_T *Arg);

extern void KerParReduct_CC_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUM_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_ReLUMN_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_HSwish_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_LeakyReLU_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Sigmoid_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerParReduct_CC_Tanh_HWC_USQ16(KerConvLinReduct_SQ8_T *Arg);


/* No PerChannel Scale */
extern void KerReduct_CC_NoScale_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUM_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUMN_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_Sigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_Tanh_SQ8(KerConvLinReduct_SQ8_T *Arg);

extern void KerReduct_CC_NoScale_USQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLU_USQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUN_USQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUM_USQ8(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUMN_USQ8(KerConvLinReduct_SQ8_T *Arg);

extern void KerReduct_CC_NoScale_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLU_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUN_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUM_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUMN_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_HSigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_HSwish_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_LeakyReLU_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_Sigmoid_SQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_Tanh_SQ16(KerConvLinReduct_SQ8_T *Arg);

extern void KerReduct_CC_NoScale_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLU_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUN_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUM_USQ16(KerConvLinReduct_SQ8_T *Arg);
extern void KerReduct_CC_NoScale_ReLUMN_USQ16(KerConvLinReduct_SQ8_T *Arg);

/******************************************************************************************************************
          Stand alone activation. Parallel Feature, Feature Parallel
	  Input is a scaled 8b tensor
	  Output is a scaled 8b tensor, Scale can be different from the one of input
******************************************************************************************************************/

/*
 * Standalone Scaled Activation, Features are evaluated one after the other in parallel
*/
extern void Ker_ActNone_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_Scale_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLU_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUN_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUM_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUMN_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_HSigmoid_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_HSwish_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_Sigmoid_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_Tanh_SQ8(KerActivation_SQ8_T *Arg);

extern void Ker_ActNone_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLU_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUN_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUM_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_ReLUMN_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_HSigmoid_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_HSwish_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_LeakyReLU_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_Sigmoid_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);
extern void Ker_Tanh_ScaleIn_SQ8(KerActivation_SQ8_T *Arg);


/******************************************************************************************************************
	Pooling group.
	Performs Max, Average or Global average pooling followed by an optional Scaling or Scaling and Activation.
	Several output feature maps are evaluated in parallel, one feature map per core
******************************************************************************************************************/

extern void KerParPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPool2x2Stride2_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPool2x2Stride2_ReLUMN_SQ8(KerPool_SQ8_T *Arg);

extern void KerParPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxNStrideS_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxNStrideS_ReLUMN_SQ8(KerPool_SQ8_T *Arg);

extern void KerParPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxMStrideSxSy_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerParPoolNxMStrideSxSy_ReLUMN_SQ8(KerPool_SQ8_T *Arg);

extern void KerParGlobalMaxPool_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPool_Reduct_ReLUM_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPool_Reduct_ReLUMN_SQ8(KerGlobalPool_SQ8_T *Arg);

extern void KerParGlobalAvgPool_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPool_Reduct_ReLUM_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPool_Reduct_ReLUMN_SQ8(KerGlobalPool_SQ8_T *Arg);

extern void KerParGlobalMaxPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_ReLUM_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalMaxPoolFullFeat_ReLUMN_SQ8(KerGlobalPool_SQ8_T *Arg);

extern void KerParGlobalAvgPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_ReLUM_SQ8(KerGlobalPool_SQ8_T *Arg);
extern void KerParGlobalAvgPoolFullFeat_ReLUMN_SQ8(KerGlobalPool_SQ8_T *Arg);

/* Pooling Basic Kernels for HWC Layers layout */
extern void KerParMaxPoolNxMStrideSxSy_HWC_USQ8(Ker_MM_Pool_USQ8_T *Arg);
extern void KerParAvgPoolNxMStrideSxSy_HWC_USQ8(Ker_MM_Pool_USQ8_T *Arg);
extern void KerParPool_MaxPoolNxMStrideSxSy__HWC_USQ8(KerPool_HWC_USQ8_T *Arg);

extern void KerParMaxPoolNxMStrideSxSy_HWC_SQ8(Ker_MM_Pool_SQ8_T *Arg);
extern void KerParAvgPoolNxMStrideSxSy_HWC_SQ8(Ker_MM_Pool_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Pooling group.
	Performs Max or Average pooling followed by an optional Scaling or Scaling and Activation.
	One output feature map is evaluated in parallel on all cores
*************************************************************************************************************************************************/

extern void KerPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg);
extern void KerPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerPool2x2Stride2_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerPool2x2Stride2_ReLUMN_SQ8(KerPool_SQ8_T *Arg);

extern void KerPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxNStrideS_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxNStrideS_ReLUMN_SQ8(KerPool_SQ8_T *Arg);

extern void KerPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxMStrideSxSy_ReLUM_SQ8(KerPool_SQ8_T *Arg);
extern void KerPoolNxMStrideSxSy_ReLUMN_SQ8(KerPool_SQ8_T *Arg);


/*************************************************************************************************************************************************
	Tensor Addition with Input1 and Output optionally scaled (Tensor centric) followed by optional activation
*************************************************************************************************************************************************/

extern void KerMatAdd_SQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLU_SQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLUN_SQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_HSigmoid_SQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_HSwish_SQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg);

extern void KerMatAdd_USQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLU_USQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLUN_USQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLUM_USQ8(KerMat3_SQ8_T *Arg);
extern void KerMatAdd_ReLUMN_USQ8(KerMat3_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling, followed by optional activation: ReLU and ReLUN, other activations should be performed
	using standalone activation kernels
   	In1 convolution weights
	In2 convolution Features
*************************************************************************************************************************************************/

extern void KerParMatMulB8_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB8_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

extern void KerParMatMulB16_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB16_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

extern void KerParMatMulB32_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulTransposedB32_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulTransposedB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulTransposedB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_PL_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_ReLU_PL_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_ReLUN_PL_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulNoBias_2x4_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulNoBias_2x4_ReLU_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulNoBias_2x4_ReLUN_SQ8(KerMatMul_PL_SQ8_T *Arg);
extern void KerParMatMulSxSyB32_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulSxSyB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling for small first matrix in the product, goal is to improve parallelism in this specific situation
	followed by an optional activation: ReLU and ReLUN, other activations should be performed using standalone activation kernels

   	In1 fits completly in shared L1, convolution weights
	In2 has been transposed before being used, convolution Features
	Parallelization scheme partition In2 along H_In2
*************************************************************************************************************************************************/

extern void KerParMatMulB8_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB8_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB8_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

extern void KerParMatMulB16_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB16_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB16_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

extern void KerParMatMulB32_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_2x4_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
extern void KerParMatMulB32_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

extern void KerPar_MM_Conv1x1_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1x1_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void Ker_MM_Conv1x1_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_ReLUN_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_LeakyReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_DxDy_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_DxDy_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv1D_DxDy_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);

extern void KerPar_MM_Conv2D_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv2D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv2D_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv2D_DxDy_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv2D_DxDy_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void KerPar_MM_Conv2D_DxDy_ReLU_SQ8(Ker_MM_Conv_SQ8_T *Arg);

extern void KerPar_MM_ConvDW2D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);

extern void Ker_MM_Conv2D_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);
extern void Ker_MM_Conv2D_ReLU_HWC_SQ8(Ker_MM_Conv_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix by Vector Multiplication followed by an optional Activation (all of them supported)
*************************************************************************************************************************************************/

extern void KerParMatVectMul_SQ8(KerMat3_SQ8_T *Arg);
extern void KerParMatVectMul_ReLU_SQ8(KerMat3_SQ8_T *Arg);
extern void KerParMatVectMul_ReLUN_SQ8(KerMat3_SQ8_T *Arg);
extern void KerParMatVectMul_HSigmoid_SQ8(KerMat3_SQ8_T *Arg);
extern void KerParMatVectMul_HSwish_SQ8(KerMat3_SQ8_T *Arg);
extern void KerParMatVectMul_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg);


/*************************************************************************************************************************************************
	Linear Layer with 32b output.
	When an output cannot be evaluated by a single call (Non FullFeat) Bias assumed to be set before. Scaling and activation done after
	When FullFeat Bias setting, Linear and channel scaling all performed in the same call. Bias can be 8,16 or 32b
*************************************************************************************************************************************************/

extern void KerParLinearLayer_SQ8(KerLinear_SQ8_T *Arg);

extern void KerParLinearLayerFullFeatB8_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB8_ReLU_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB8_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

extern void KerParLinearLayerFullFeatB16_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB16_ReLU_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB16_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

extern void KerParLinearLayerFullFeatB32_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB32_ReLU_SQ8(KerLinear_SQ8_T *Arg);
extern void KerParLinearLayerFullFeatB32_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

/*************************************************************************************************************************************************
	SotMax, Q15 Output
*************************************************************************************************************************************************/

extern void KerParSoftMax_SQ8(KerSoftMax_SQ8_T *Arg);
extern void KerParSoftMax8Bits_SQ8(KerSoftMax_SQ8_T *Arg);
extern void KerParSoftMax_HWC_SQ8(KerSoftMax_SQ8_T *Arg);

/******************************************************************************************************************
 	Recursive NN (RNN, LSTM, GRU)
******************************************************************************************************************/

extern void RNN_ParKerB32_SQ8(KerRNN_SQ8_T *Arg);
extern void RNN_ParKerB32_SameInStateScale_SQ8(KerRNN_SQ8_T *Arg);
extern void RNN_ParKerB32_Hard_SQ8(KerRNN_SQ8_T *Arg);
extern void RNN_ParKerB32_Hard_SameInStateScale_SQ8(KerRNN_SQ8_T *Arg);
extern void LSTM_ParKerB32_SQ8(KerLSTM_SQ8_T *Arg);
extern void LSTM_ParKerB32_SameInStateScale_SQ8(KerLSTM_SQ8_T *Arg);
extern void LSTM_ParKerB32_Hard_SQ8(KerLSTM_SQ8_T *Arg);
extern void LSTM_ParKerB32_Hard_SameInStateScale_SQ8(KerLSTM_SQ8_T *Arg);
extern void GRU_ParKerB32_SQ8(KerGRU_SQ8_T *Arg);
#endif
