#ifndef __CNN_BASICKERNELS_SQ8__
#define __CNN_BASICKERNELS_SQ8__
#include "Gap.h"

#ifdef __pulp__
#define Min(a, b)       __builtin_pulp_minsi((a), (b))
#define Max(a, b)       __builtin_pulp_maxsi((a), (b))
#else
#define Min(a, b)       (((a)<(b))?(a):(b))
#define Max(a, b)       (((a)>(b))?(a):(b))
#endif


#define NORM_ROUND
#ifdef NORM_ROUND
#define AT_NORM(x, n)   gap_roundnorm_reg((x), (n))
#else
#define AT_NORM(x, n)   gap_norm_reg((x), (n))
#endif

#define AT_SCALE(X, Scale, ScaleN)      AT_NORM((X)*(Scale), (ScaleN))

#define AT_LSHIFT(x, n) ((x)<<(n))
#define AT_RSHIFT(x, n) ((x)>>(n))

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

typedef enum {
        ACT_NONE = 0,
        ACT_RELU,
        ACT_RELUN,
        ACT_HSIGMOID,
        ACT_HSWISH,
        ACT_LEAKYRELU,
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
	unsigned char N;			/**< Dimension of the convolution: Nx, NxN, used only for general versions */
	unsigned char S;			/**< Output stride, S, used only for general versions */
	unsigned char D;			/**< Dilation Dx */
	unsigned char Ny;			/**< Filter Ny, used only if Nx!=Ny */
	unsigned char Sy;			/**< Stride Sy, used only if Sx!=Sy */
	unsigned char Dy;			/**< Dilation Dy, used only if Dx!=Dy */
} KerConv_SQ8_T;

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
	unsigned char FS;		/* Filter Size, x */
	unsigned char FSy;		/* Filter Size, y */
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

/******************************************************************************************************************
	Matrix transposition and 3D Tensor dim permutation
******************************************************************************************************************/

typedef struct {
	signed char *__restrict__ In;           /**< Input matrix */
	signed char *__restrict__ Out;          /**< Output matrix */
	unsigned int Feat;                      /**< Number of matrices */
	unsigned int W;                         /**< Matrix width */
	unsigned int H;                         /**< Matrix height */
	unsigned char Sx;                       /**< Stride for W dimension */
	unsigned char Sy;                       /**< Stride for H dimension */
} KerMatTranspose_fps_T;


/******************************************************************************************************************
	SoftMax, Q15 output
******************************************************************************************************************/

typedef struct {
	signed char *__restrict__ In;           /**< Pointer to input tile */
	unsigned short int N;                   /**< Size of the tile */
	unsigned short int Norm;                /**< Normalization factor */
	short int *__restrict__ Out;            /**< Pointer to output tile */
	signed char * __restrict__ Infos;	/**< Scaling and constants data */
} KerSoftMax_SQ8_T;

/******************************************************************************************************************
 	Recursive NN (RNN, LSTM, GRU)
******************************************************************************************************************/

#define RNN_F_INF              4
#define RNN_F_OFF              0
#define RNN_F_SCALE            0
#define RNN_F_SCALEN           1
#define RNN_F_A0               2
#define RNN_F_B0               3

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
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< 1 if RNN State has to be reset */
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

#define LSTM_CELL_INFOS (LSTM_F_INF+LSTM_I_INF+LSTM_G_INF+LSTM_O_INF+LSTM_COUT_INF+LSTM_INT_INF)

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
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< If 1 LSTM State is reset */
} KerLSTM_SQ8_T;


#define GRU_R_INF              7
#define GRU_R_OFF              0
#define GRU_R_SCALE            0
#define GRU_R_SCALEN           1
#define GRU_R_A0               2
#define GRU_R_B0               3
#define GRU_R_C0               4
#define GRU_R_ASCALE           5
#define GRU_R_ASCALEN          6

#define GRU_Z_INF              7
#define GRU_Z_OFF              (GRU_R_OFF+GRU_R_INF)
#define GRU_Z_SCALE            (0 + GRU_Z_OFF)
#define GRU_Z_SCALEN           (1 + GRU_Z_OFF)
#define GRU_Z_A0               (2 + GRU_Z_OFF)
#define GRU_Z_B0               (3 + GRU_Z_OFF)
#define GRU_Z_C0               (4 + GRU_Z_OFF)
#define GRU_Z_ASCALE           (5 + GRU_Z_OFF)
#define GRU_Z_ASCALEN          (6 + GRU_Z_OFF)

#define GRU_HT_INF              4
#define GRU_HT_OFF              (GRU_Z_OFF+GRU_Z_INF)
#define GRU_HT_SCALE            (0 + GRU_HT_OFF)
#define GRU_HT_SCALEN           (1 + GRU_HT_OFF)
#define GRU_HT_A0               (2 + GRU_HT_OFF)
#define GRU_HT_B0               (3 + GRU_HT_OFF)

#define GRU_H_INF		3
#define GRU_H_OFF		(GRU_HT_OFF+GRU_HT_INF)
#define GRU_H_SCALE		(0 + GRU_HT_OFF)
#define GRU_H_SCALEN		(1 + GRU_HT_OFF)
#define GRU_H_A0		(2 + GRU_HT_OFF)

#define GRU_CELL_INFOS (GRU_R_INF+GRU_Z_INF+GRU_HT_INF+GRU_H_INF)

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
	void * __restrict__ Bh;			/**< Pointer to H gate bias */
	signed char *__restrict__ Sbuff;	/**< Pointer to Ht buffer, Dim=[DimState] */
	signed char *__restrict__ Hout;		/**< Pointer to Hout in case sequence must be exposed, null otherwise */
	unsigned short int Nout;		/**< Number of output produced in StateInOut */
	signed char *__restrict__ Infos;	/**< Infos vector for scaling */
	char FirstOut;				/**< 1 if first out of one cell to eval */
	char FirstCell;				/**< 1 if first cell of a group of cell */
	char Reset;				/**< If 1 GRU State is reset */
} KerGRU_SQ8_T;


/******************************************************************************************************************
	Bias setting for convolution and linear layers, output is 32b, input is 8,16 or 32b
******************************************************************************************************************/

/* Features in parallel */
void KerParSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg);
void KerParSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg);
void KerParSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg);

/* Each feature in parallel */
void KerSetBiasB32_SQ8(KerSetBias_SQ8_T *Arg);
void KerSetBiasB16_SQ8(KerSetBias_SQ8_T *Arg);
void KerSetBiasB8_SQ8(KerSetBias_SQ8_T *Arg);


/******************************************************************************************************************
	Convolution, Bias is assigned separately. Output is 32 bits. Parallel Features and Features Parallel
******************************************************************************************************************/

void KerParConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerParConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg);
void KerParConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerParConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerParConv4x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x4Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg);
void KerParConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerParConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerParConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerParConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerParConv13x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerParConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg);
void KerParConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
void KerParConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg);


void KerConv1x1Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x1Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x1StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerConv3x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv3x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x3Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x3Stride1x2_SQ8(KerConv_SQ8_T *Arg);
void KerConv3x3Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerConv3x3Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerConv3x3StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerConv5x1Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv5x1Stride2x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x5Stride1x1_SQ8(KerConv_SQ8_T *Arg);
void KerConv1x5Stride1x2_SQ8(KerConv_SQ8_T *Arg);
void KerConv5x5Stride1_SQ8(KerConv_SQ8_T *Arg);
void KerConv5x5Stride2_SQ8(KerConv_SQ8_T *Arg);
void KerConv5x5StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerConv7x7StrideS_SQ8(KerConv_SQ8_T *Arg);
void KerConvNxNStrideS_SQ8(KerConv_SQ8_T *Arg);
void KerConvNxMStrideSxSy_SQ8(KerConv_SQ8_T *Arg);
void KerConvNxMDxDyStrideSxSy_SQ8(KerConv_SQ8_T *Arg);


/******************************************************************************************************************
	Depth Wise convolution, 8b, 16b and 32b Bias. Output is 32 bits. Parallel Features and Features Parallel
******************************************************************************************************************/

/* 8 Bits Bias, Features in parallel */
void KerParConvDW1x1Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW7x7StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxNStrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMDxDyStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);

/* 16 Bits Bias, Features in parallel */
void KerParConvDW1x1Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW7x7StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxNStrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMDxDyStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);

/* 32 Bits Bias, Features in parallel */
void KerParConvDW1x1Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x1StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x3Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW3x3StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW1x5Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW5x5StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDW7x7StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxNStrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);
void KerParConvDWNxMDxDyStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);

/* 8 Bits Bias, One Feature in parallel */
void KerConvDW1x1Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride2x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x2B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride1B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride2B8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW7x7StrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxNStrideSB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMDxDyStrideSxSyB8_SQ8(KerConv_SQ8_T *Arg);

/* 16 Bits Bias, One Feature in parallel */
void KerConvDW1x1Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride2x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x2B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride1B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride2B16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW7x7StrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxNStrideSB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMDxDyStrideSxSyB16_SQ8(KerConv_SQ8_T *Arg);

/* 32 Bits Bias, One Feature in parallel */
void KerConvDW1x1Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x1StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x3Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW3x3StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x1Stride2x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW1x5Stride1x2B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride1B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5Stride2B32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW5x5StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDW7x7StrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxNStrideSB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);
void KerConvDWNxMDxDyStrideSxSyB32_SQ8(KerConv_SQ8_T *Arg);

/******************************************************************************************************************
          Input Scaling followed by an optional activation. Parallel Feature, Feature Parallel
	  Input is assumed to be the 32b unnormalized output of a convolution or a linear layer
	  Optional activation is applied to the scaled input and can be optionaly scaled also
	  Output is a scaled 8b quantized tensor
	  Channel Centric (CC)
******************************************************************************************************************/

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated in parallel
*/
void KerParReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated in parallel
*/
void KerParReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerParReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location != In location. Features are evaluated one after the other in parallel
*/
void KerReduct_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReduct_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReduct_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReduct_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReduct_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReduct_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);

/*
 * Input Scaling and reduction to 8b then channel centric activation, Out location = In location. Features are evaluated one after the other in parallel
*/
void KerReductIO_CC_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReductIO_CC_ReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReductIO_CC_ReLUN_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReductIO_CC_HSigmoid_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReductIO_CC_HSwish_SQ8(KerConvLinReduct_SQ8_T *Arg);
void KerReductIO_CC_LeakyReLU_SQ8(KerConvLinReduct_SQ8_T *Arg);

/******************************************************************************************************************
          Stand alone activation. Parallel Feature, Feature Parallel
	  Input is a scaled 8b tensor
	  Output is a scaled 8b tensor, Scale can be different from the one of input
******************************************************************************************************************/

/*
 * Standalone Scaled Activation, Features are evaluated in parallel
*/
void KerPar_ReLU_SQ8(KerActivation_SQ8_T *Arg);
void KerPar_ReLUN_SQ8(KerActivation_SQ8_T *Arg);
void KerPar_HSigmoid_SQ8(KerActivation_SQ8_T *Arg);
void KerPar_HSwish_SQ8(KerActivation_SQ8_T *Arg);
void KerPar_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg);

/*
 * Standalone Scaled Activation, Features are evaluated one after the other in parallel
*/
void Ker_ReLU_SQ8(KerActivation_SQ8_T *Arg);
void Ker_ReLUN_SQ8(KerActivation_SQ8_T *Arg);
void Ker_HSigmoid_SQ8(KerActivation_SQ8_T *Arg);
void Ker_HSwish_SQ8(KerActivation_SQ8_T *Arg);
void Ker_LeakyReLU_SQ8(KerActivation_SQ8_T *Arg);


/******************************************************************************************************************
	Pooling group.
	Performs Max, Average or Global average pooling followed by an optional Scaling or Scaling and Activation.
	Several output feature maps are evaluated in parallel, one feature map per core
******************************************************************************************************************/

void KerParPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg);
void KerParPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerParPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg);

void KerParPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg);
void KerParPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerParPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg);

void KerParPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg);
void KerParPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerParPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg);

void KerParGlobalMaxPool_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalMaxPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalMaxPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalMaxPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);

void KerParGlobalAvgPool_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalAvgPool_Reduct_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalAvgPool_Reduct_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalAvgPool_Reduct_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);

void KerParGlobalMaxPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalMaxPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalMaxPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);

void KerParGlobalAvgPoolFullFeat_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalAvgPoolFullFeat_ReLU_SQ8(KerGlobalPool_SQ8_T *Arg);
void KerParGlobalAvgPoolFullFeat_ReLUN_SQ8(KerGlobalPool_SQ8_T *Arg);


/*************************************************************************************************************************************************
	Pooling group.
	Performs Max or Average pooling followed by an optional Scaling or Scaling and Activation.
	One output feature map is evaluated in parallel on all cores
*************************************************************************************************************************************************/

void KerPool2x2Stride2_SQ8(KerPool_SQ8_T *Arg);
void KerPool2x2Stride2_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerPool2x2Stride2_ReLUN_SQ8(KerPool_SQ8_T *Arg);

void KerPoolNxNStrideS_SQ8(KerPool_SQ8_T *Arg);
void KerPoolNxNStrideS_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerPoolNxNStrideS_ReLUN_SQ8(KerPool_SQ8_T *Arg);

void KerPoolNxMStrideSxSy_SQ8(KerPool_SQ8_T *Arg);
void KerPoolNxMStrideSxSy_ReLU_SQ8(KerPool_SQ8_T *Arg);
void KerPoolNxMStrideSxSy_ReLUN_SQ8(KerPool_SQ8_T *Arg);


/*************************************************************************************************************************************************
	Tensor Addition with Input1 and Output optionally scaled (Tensor centric) followed by optional activation
*************************************************************************************************************************************************/

void KerParMatAdd_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatAdd_ReLU_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatAdd_ReLUN_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatAdd_HSigmoid_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatAdd_HSwish_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatAdd_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling, followed by optional activation: ReLU and ReLUN, other activations should be performed
	using standalone activation kernels
   	In1 convolution weights
	In2 convolution Features
*************************************************************************************************************************************************/

void KerParMatMulB8_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB8_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB8_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB8_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

void KerParMatMulB16_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB16_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB16_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB16_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

void KerParMatMulB32_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB32_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB32_ReLU_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulSxSyB32_ReLUN_SQ8(KerMatMul_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix mult with channel centric scaling for small first matrix in the product, goal is to improve parallelism in this specific situation
	followed by an optional activation: ReLU and ReLUN, other activations should be performed using standalone activation kernels

   	In1 fits completly in shared L1, convolution weights
	In2 has been transposed before being used, convolution Features
	Parallelization scheme partition In2 along H_In2
*************************************************************************************************************************************************/

void KerParMatMulB8_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB8_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB8_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

void KerParMatMulB16_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB16_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB16_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

void KerParMatMulB32_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB32_ReLU_SF_SQ8(KerMatMul_SQ8_T *Arg);
void KerParMatMulB32_ReLUN_SF_SQ8(KerMatMul_SQ8_T *Arg);

/*************************************************************************************************************************************************
	Matrix by Vector Multiplication followed by an optional Activation (all of them supported)
*************************************************************************************************************************************************/

void KerParMatVectMul_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatVectMul_ReLU_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatVectMul_ReLUN_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatVectMul_HSigmoid_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatVectMul_HSwish_SQ8(KerMat3_SQ8_T *Arg);
void KerParMatVectMul_LeakyReLU_SQ8(KerMat3_SQ8_T *Arg);


/*************************************************************************************************************************************************
	Linear Layer with 32b output.
	When an output cannot be evaluated by a single call (Non FullFeat) Bias assumed to be set before. Scaling and activation done after
	When FullFeat Bias setting, Linear and channel scaling all performed in the same call. Bias can be 8,16 or 32b
*************************************************************************************************************************************************/

void KerParLinearLayer_SQ8(KerLinear_SQ8_T *Arg);

void KerParLinearLayerFullFeatB8_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB8_ReLU_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB8_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

void KerParLinearLayerFullFeatB16_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB16_ReLU_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB16_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

void KerParLinearLayerFullFeatB32_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB32_ReLU_SQ8(KerLinear_SQ8_T *Arg);
void KerParLinearLayerFullFeatB32_ReLUN_SQ8(KerLinear_SQ8_T *Arg);

/*************************************************************************************************************************************************
	List of Matrix Transposition, no scaling
*************************************************************************************************************************************************/

void CNN_ParTranspose_fps(KerMatTranspose_fps_T *Arg);
void CNN_ParTransposeSxSy_fps(KerMatTranspose_fps_T *Arg);
void CNN_Transpose_fps(KerMatTranspose_fps_T *Arg);
void CNN_TransposeSxSy_fps(KerMatTranspose_fps_T *Arg);

/*************************************************************************************************************************************************
	3D Tensor dimension permutations, no scaling
*************************************************************************************************************************************************/

void CNN_MatPermCHW2CWH_fps(KerMatTranspose_fps_T *Arg);
void CNN_MatPermCHW2HWC_fps(KerMatTranspose_fps_T *Arg);
void CNN_MatPermCHW2WHC_fps(KerMatTranspose_fps_T *Arg);
void CNN_MatPermCHW2WCH_fps(KerMatTranspose_fps_T *Arg);
void CNN_MatPermCHW2HCW_fps(KerMatTranspose_fps_T *Arg);

/*************************************************************************************************************************************************
	SotMax, Q15 Output
*************************************************************************************************************************************************/

void KerParSoftMax_SQ8(KerSoftMax_SQ8_T *Arg);

/******************************************************************************************************************
 	Recursive NN (RNN, LSTM, GRU)
******************************************************************************************************************/

void RNN_ParKerB32_SQ8(KerRNN_SQ8_T *Arg);
void LSTM_ParKerB32_SQ8(KerLSTM_SQ8_T *Arg);
void GRU_ParKerB32_SQ8(KerGRU_SQ8_T *Arg);

/*************************************************************************************************************************************************
	AT book keeping functions
*************************************************************************************************************************************************/

void AT_TileClear(
        char *__restrict__ In,  	/**< Tile */
        int W,                  	/**< Tile width */
        int H,                 		/**< Tile height */
        int Feat,               	/**< Number of features */
        int Size,               	/**< Tile element size in bytes */
        int Pad,                	/**< Height or width of the area to be 0 padded */
        int Orientation         	/**< 0: Horizontal tile, 1: Vertical tile */
        );

void AT_DumpTensor(
        char *NodeName,                 /**< Graph Node Name, a User Kernel */
        char *ArgName,                  /**< Argument name of this user kernel */
        int Loc,                        /**< Exec location if this argument, AT_MEM_xyz */
        void *L3_Device,                /**< Pointer to device descriptor in case Loc is external */
        void *L3_Event,                 /**< Pointer to a read event for this device descriptor if any */
        int ItemSize,                   /**< Data type size in bytes */
        int Dim,                        /**< Number of dimensions, up to 5, from D0 most outer to D4 most inner */
        int D0,                         /**< Actual value of this dimension if defined, 1 otherwise */
        int D1,                         /**< Actual value of this dimension if defined, 1 otherwise */
        int D2,                         /**< Actual value of this dimension if defined, 1 otherwise */
        int D3,                         /**< Actual value of this dimension if defined, 1 otherwise */
        int D4,                         /**< Actual value of this dimension if defined, 1 otherwise */
        void *L2_BufferAddr,            /**< In case exec loc is external pointer to a buffer in L2 to host partial copy of Arg */
        unsigned int L2_BufferSize,     /**< Size of this buffer */
        void *Addr                      /**< Address of Arg */
        );


#endif
