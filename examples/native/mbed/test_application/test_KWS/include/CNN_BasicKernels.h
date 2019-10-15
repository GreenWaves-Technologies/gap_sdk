/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __BASIC_KERNEL_LIB_H__
#define __BASIC_KERNEL_LIB_H__

#include "Gap8.h"
#include "StdTypes.h"

/** @addtogroup groupCNN
@{ */

/** @defgroup CNNBasicK CNNBasicKernels

@{ */

/** @brief Template for CNN bias setting

Template for CNN bias setting
*/
typedef struct {
	Word16 * __restrict__ Out;	/**< Pointer to an ouput tile */
	int	W;			/**< Width of the tile */
	int	H;			/**< Height of the tile */
	Word16 Bias;			/**< Bias value to assign to all tile's elements */
} KerSetInBiasT;

/** @brief Assign a bias to all elements in the output tile

Assign Bias to all elements in the input/output tile
*/
void KerSetInBias(KerSetInBiasT *Arg);

/** @brief Template for CNN bias setting, 2 outputs, 2 bias

Template for CNN bias setting, 2 outputs, 2 bias
*/
typedef struct {
	Word16 * __restrict__ Out0;	/**< Pointer to 1st output tile */
	Word16 * __restrict__ Out1;	/**< Pointer to 2nd output tile */
	int	W;			/**< Width of the tile, both have same width */
	int	H;			/**< Height of the tile, both have same height */
	Word16 Bias0;			/**< Bias to assign to all 1st tile's elements */
	Word16 Bias1;			/**< Bias to assign to all 2nd tile's elements */
} KerSetInBias2T;

/** @brief Assign a bias to all elements in the 2 output tiles, each tile has a different bias

Assign a bias to all elements in the 2 output tiles, each tile has a different bias
*/
void KerSetInBias2(KerSetInBias2T *Arg);

/** @brief Template for CNN bias setting, 3 outputs, 3 bias

Template for CNN bias setting, 3 outputs, 3 bias
*/
typedef struct {
	Word16 * __restrict__ Out0;	/**< Pointer to 1st output tile */
	Word16 * __restrict__ Out1;	/**< Pointer to 2nd output tile */
	Word16 * __restrict__ Out2;	/**< Pointer to 3rd output tile */
	int	W;			/**< Width of the tile, all have same width */
	int	H;			/**< Height of the tile, all have same height */
	Word16 Bias0;			/**< Bias to assign to all 1st tile's elements */
	Word16 Bias1;			/**< Bias to assign to all 2nd tile's elements */
	Word16 Bias2;			/**< Bias to assign to all 3rd tile's elements */
} KerSetInBias3T;

/** @brief Assign a bias to all elements in the 3 output tiles, each tile has a different bias

Assign a bias to all elements in the 3 output tiles, each tile has a different bias
*/
extern void KerSetInBias3(KerSetInBias3T *Arg);


/* Convolution */

/** @brief Template for convolutions, CNN and pure ones

Template for convolutions, CNN and pure ones
*/
typedef struct {
	Word16 * __restrict__ In;	/**< Pointer to input tile  */
	int W;				/**< Width of the input tile */
	int H;				/**< Height of the input tile */
	Word16 * __restrict__ Filter;	/**< Pointer to convolution coefficients. (N x N) coeffs in Q15 */
	Word16 * __restrict__ Out;	/**< Pointer to output tile, this tile has N-1 lines and N-1 column than In */
	unsigned int Norm;		/**< Fixed point format, should be <= 15 */
	int N;				/**< Dimension of the convolution: NxM, used only for general versions */
	int M;				/**< Dimension of the convolution: NxM, used only for general versions */
} KerConv_fpT;

typedef struct {
	Word16 * __restrict__ In;	/**< Pointer to input tile  */
	int W;				/**< Width of the input tile */
	int H;				/**< Height of the input tile */
	Word8 * __restrict__ Filter;	/**< Pointer to convolution coefficients. (N x N) coeffs in Q15 */
	Word16 * __restrict__ Out;	/**< Pointer to output tile, this tile has N-1 lines and N-1 column than In */
	unsigned int Norm;		/**< Fixed point format, should be <= 15 */
	int N;				/**< Dimension of the convolution: NxM, used only for general versions */
	int M;				/**< Dimension of the convolution: NxM, used only for general versions */
} KerConv_fpsT;

/** @brief CNN 3x3 convolution, Q15 inputs and outputs. Accumulation with previous output.

CNN 3x3 convolution, Q15 inputs and outputs. Accumulation with previous output.

        3x3 convolution, short int inputs and output
        The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the 9 convolution coefficients
        Norm:   Fixed point format
*/
void KerConv3x3_fp(KerConv_fpT *Arg);

/** @brief Pure 3x3 convolution, Q15 inputs and outputs. No accumulation with previous output.

Pure 3x3 convolution, Q15 inputs and outputs. No accumulation with previous output.

        3x3 convolution, short int inputs and output
        The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the 9 convolution coefficients
        Norm:   Fixed point format
*/
void KerDirectConv3x3_fp(KerConv_fpT *Arg);

/** @brief CNN 5x5 convolution, Q15 inputs and outputs. Accumulation with previous output.

CNN 5x5 convolution, Q15 inputs and outputs. Accumulation with previous output.

        5x5 convolution, short int inputs and output
        The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the 9 convolution coefficients
        Norm:   Fixed point format
*/
void KerConv5x5_fp(KerConv_fpT *Arg);

/** @brief Pure 5x5 convolution, Q15 inputs and outputs. No accumulation with previous output.

Pure 5x5 convolution, Q15 inputs and outputs. No accumulation with previous output.

        5x5 convolution, short int inputs and output
        The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the 9 convolution coefficients
        Norm:   Fixed point format
*/
void KerDirectConv5x5_fp(KerConv_fpT *Arg);

/** @brief CNN NxN convolution, Q15 inputs and outputs. Accumulation with previous output.

CNN NxN convolution, Q15 inputs and outputs. Accumulation with previous output.

        NxN convolution, short int inputs and output
        The result of the convolution is accumulated to the existing output, then it is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the NxN convolution coefficients
        Norm:   Fixed point format
*/
void KerConvNxN_fp(KerConv_fpT *Arg);

/** @brief Pure NxN convolution, Q15 inputs and outputs. No accumulation with previous output.

Pure NxN convolution, Q15 inputs and outputs. No accumulation with previous output.

        NxN convolution, short int inputs and output
        The result of the convolution is rounded, normalized and cliped to 16bits before beeing written
        In:     short int *, convolution input
        W, H:   Input dimension [W x H]
        Filter: short int *, pointer to the NxN convolution coefficients
        Norm:   Fixed point format
*/
void KerDirectConvNxN_fp(KerConv_fpT *Arg);
void KerDirectConvNxM_char(KerConv_fpsT *Arg);
void KerDirectConvNxM_fp_tiled(KerConv_fpT *Arg);
void KerDirectConvNxM_tiled_2(KerConv_fpT *Arg);
void KerDirectConvNxM_fp(KerConv_fpT *Arg);
void KerSetInBias_int(KerSetInBiasT *Arg);
void KerDirectConvNxM_fp_model(KerConv_fpT *Arg);
void KerDirectConvNxM_char_model(KerConv_fpsT *Arg);

/* Rectification, Pooling */

/** @brief Template for rcetification and pooling basic kernels

Template for rcetification and pooling basic kernels
*/
typedef struct {
        Word16 * __restrict__ In;	/**< Pointer to input tile  */
        int W;				/**< Width of the input tile */
        int H;				/**< Height of the input tile */
        Word16 * __restrict__ Out;	/**< Pointer to output tile */
} KerReLUMaxPool2x2_fpT;

/** @brief Linear rectification basic kernel

Linear rectification basic kernel

        Linear rectification
        In:     short int *, Input data [W x H]
        W, H:   Input data dimension [W x H]
        Out:    short int *, Output data [W, H]
*/
void KerReLU_fp(KerReLUMaxPool2x2_fpT *Arg);

/** @brief 2x2 Max Pooling basic kernel

2x2 Max Pooling basic kernel

        2x2 max pooling
        In:     short int *, Input data
        W, H:   Input data dimension [W x H]
        Out:    short int *, Output data [W/2, H/2]
*/
void KerMaxPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg);

/** @brief 2x2 Average Pooling basic kernel

2x2 Average Pooling basic kernel

        2x2 Average pooling
        In:     short int *, Input data
        W, H:   Input data dimension [W x H]
        Out:    short int *, Output data [W/2, H/2]
*/
void KerAvgPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg);

/** @brief Linear rectification followed by 2x2 Max Pooling basic kernel

Linear rectification followed by 2x2 Max Pooling basic kernel

        Linear rectification followed by a 2x2 max pooling, single output version
        In:     short int *, Input data
        W, H:   Input data dimension [W x H]
        Out:    short int *, Output data [W/2, H/2]
*/
void KerReLUMaxPool2x2_fp(KerReLUMaxPool2x2_fpT *Arg);


/** @brief Template for rectification and pooling basic kernels, 2 tiles version

Template for rectification and pooling basic kernels, 2 tiles version
*/
typedef struct {
        Word16 * __restrict__ In0;	/**< Pointer to 1st input tile  */
        Word16 * __restrict__ In1;	/**< Pointer to 2nd input tile  */
        int W;				/**< Width of the tile. Input and output have the same */
        int H;				/**< Height of the tile. Input and output have the same */
        Word16 * __restrict__ Out0;	/**< Pointer to 1st output tile  */
        Word16 * __restrict__ Out1;	/**< Pointer to 2nd output tile  */
} KerReLUMaxPool2x2_2_fpT;

/** @brief Linear rectification followed by 2x2 Max Pooling basic kernel, 2 output version.

Linear rectification followed by 2x2 Max Pooling basic kernel, 2 output version.

        Linear rectification followed by a 2x2 max pooling, double output version
        In0:    short int *, Input data
        In1:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
*/
void KerReLUMaxPool2x2_2_fp(KerReLUMaxPool2x2_2_fpT *Arg);

/** @brief Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version.

Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version.

        Linear rectification followed by a 2x2 Average pooling, double output version
        In0:    short int *, Input data
        In1:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
*/
void KerReLUAvgPool2x2_2_fp(KerReLUMaxPool2x2_2_fpT *Arg);

/** @brief Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version. Pads with 0 if odd.

Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version. Pads with 0 if odd.

        Linear rectification followed by a 2x2 max pooling, double output version, pad with an extra 0 at end of line in case W is odd
        In0:    short int *, Input data
        In1:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
*/
void KerReLUMaxPool2x2_2_Even_fp(KerReLUMaxPool2x2_2_fpT *Arg);

/** @brief Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version. Drops last.

Linear rectification followed by 2x2 Average Pooling basic kernel, 2 output version. Drops last.

        Linear rectification followed by a 2x2 max pooling, double output version, drops one output at the end of each line, this is the counter
        part of KerReLUMaxPool2x2_2_Even_fp where we pad with one extra 0
        In0:    short int *, Input data
        In1:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
*/
void KerReLUMaxPool2x2_2_Drop_fp(KerReLUMaxPool2x2_2_fpT *Arg);


/** @brief Template for rectification and pooling basic kernels, 3 tiles version

Template for rectification and pooling basic kernels, 3 tiles version
*/
typedef struct {
        Word16 * __restrict__ In0;	/**< Pointer to 1st input tile  */
        Word16 * __restrict__ In1;	/**< Pointer to 2nd input tile  */
        Word16 * __restrict__ In2;	/**< Pointer to 3rd input tile  */
        int W;				/**< Width of the tile. Input and output have the same */
        int H;				/**< Height of the tile. Input and output have the same */
        Word16 * __restrict__ Out0;	/**< Pointer to 1st output tile  */
        Word16 * __restrict__ Out1;	/**< Pointer to 2nd output tile  */
        Word16 * __restrict__ Out2;	/**< Pointer to 3rd output tile  */
} KerReLUMaxPool2x2_3_fpT;

/** @brief Linear rectification followed by 2x2 Max Pooling basic kernel, 3 output version.

Linear rectification followed by 2x2 Max Pooling basic kernel, 3 output version.

        Linear rectification followed by a 2x2 max pooling, triple output version
        In0:    short int *, Input data
        In1:    short int *, Input data
        In2:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
        Out2:   short int *, Output data [W/2, H/2]
*/
void KerReLUMaxPool2x2_3_fp(KerReLUMaxPool2x2_3_fpT *Arg);

/** @brief Linear rectification followed by 2x2 Average Pooling basic kernel, 3 output version.

Linear rectification followed by 2x2 Average Pooling basic kernel, 3 output version.

        Linear rectification followed by a 2x2 Average pooling, triple output version
        In0:    short int *, Input data
        In1:    short int *, Input data
        In2:    short int *, Input data
        W, H:   Input data dimension [W x H]
        Out0:   short int *, Output data [W/2, H/2]
        Out1:   short int *, Output data [W/2, H/2]
        Out2:   short int *, Output data [W/2, H/2]
*/
void KerReLUAvgPool2x2_3_fp(KerReLUMaxPool2x2_3_fpT *Arg);

/* Dense layer*/

/** @brief Template for Linear Layer, max Q15 for Input, Coeff, Bias and Output

Template for Linear Layer
*/
typedef struct {
	Word16 * __restrict__ In;	/**< Pointer to input tile, 1D */
	int InSize;			/**< Size of the input tile */
	Word16 * __restrict__ Filter;	/**< Pointer to the coefficients, 1D, size is InSize*OutSize */
	unsigned int NormFilter;	/**< Fixed point format for Filter, must be <= 15 */
	Word16 *  __restrict__ Bias;	/**< Pointer to output bias, 1D, size of OutSize */
	unsigned int NormBias;		/**< Fixed point format for Bias, must be <= 15 */
	Word16 *  __restrict__ Out;	/**< Pointer to output tile, 1D, size is OutSize */
	int OutSize;			/**< Size of the output tile */
} KerLinearLayer_fpT;

/** @brief Linear Layer, In, Out, Filter and Bias use short ints.

Linear Layer, In, Out, Filter and Bias use short ints.

        Linear layer:   Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     short int, output vector
        OutSize output dimension
*/
void KerLinearLayer_fp(KerLinearLayer_fpT *Arg);

/** @brief Linear Layer, In, Out, Filter and Bias use short ints.

Linear Layer, In, Out, Filter and Bias use short ints.

        Linear layer:   Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
        Outputs are evaluated in parallel, a given output is evaluated in sequence

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     short int, output vector
        OutSize output dimension
*/
void KerLinearLayerParOut_fp(KerLinearLayer_fpT *Arg);

/** @brief Linear Layer followed by linear rectification, In, Out, Filter and Bias use short ints.

Linear Layer followed by linear rectification, In, Out, Filter and Bias use short ints.

        Linear layer:   Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     short int, output vector
        OutSize output dimension
*/
void KerLinearLayerReLU_fp(KerLinearLayer_fpT *Arg);


/** @brief Template for Linear Layer, max Q15 for Input, Coeff, Bias. Max Q31 for Output.

Template for Linear Layer, max Q15 for Input, Coeff, Bias. Max Q31 for Output.
*/
typedef struct {
	Word16 * __restrict__ In;	/**< Pointer to input tile, 1D */
	int InSize;			/**< Size of the input tile */
	Word16 * __restrict__ Filter;	/**< Pointer to the coefficients, 1D, size is InSize*OutSize */
	unsigned int NormFilter;	/**< Fixed point format for Filter, must be <= 15 */
	Word16 *  __restrict__ Bias;	/**< Pointer to output bias, 1D, size of OutSize */
	unsigned int NormBias;		/**< Fixed point format for Bias, must be <= 15 */
	Word32 *  __restrict__ Out;	/**< Pointer to output tile, 1D, size is OutSize */
	int OutSize;			/**< Size of the output tile */
} KerLinearLayer_fpdT;

/** @brief Linear Layer. In, Filter and Bias use short ints. Out uses plain int.

Linear Layer. In, Filter and Bias use short ints. Out uses plain int.

        Linear layer:   Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     int, output vector
        OutSize output dimension
*/
void KerLinearLayer_fpd(KerLinearLayer_fpdT *Arg);

/** @brief Linear Layer followed by rectification. In, Filter and Bias use short ints. Out uses plain int.

Linear Layer followed by rectification. In, Filter and Bias use short ints. Out uses plain int.

        Linear layer:   Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     int, output vector
        OutSize output dimension
*/
void KerLinearLayerReLU_fpd(KerLinearLayer_fpdT *Arg);

/** @brief Template for Linear Layer. Max Q15 for Input, Output and Bias. Max Q7 for Coeffs.

Template for Linear Layer. Max Q15 for Input, Output and Bias. Max Q7 for Coeffs.
*/
typedef struct {
	Word16 * __restrict__ In;	/**< Pointer to input tile, 1D */
	int InSize;			/**< Size of the input tile */
	Word8 * __restrict__ Filter;	/**< Pointer to the coefficients, 1D, size is InSize*OutSize */
	unsigned int NormFilter;	/**< Fixed point format for Filter, must be <= 15 */
	Word16 *  __restrict__ Bias;	/**< Pointer to output bias, 1D, size of OutSize */
	unsigned int NormBias;		/**< Fixed point format for Bias, must be <= 15 */
	Word16 *  __restrict__ Out;	/**< Pointer to output tile, 1D, size is OutSize */
	int OutSize;			/**< Size of the output tile */
} KerLinearLayer_fpsT;

/** @brief Linear Layer. In, Out and Bias use short ints. Filter uses byte.

Linear Layer. In, Out and Bias use short ints. Filter uses byte.

        Linear layer:   Out[i] = (sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     short, output vector
        OutSize output dimension
*/
void KerLinearLayer_fps(KerLinearLayer_fpsT *Arg);

/** @brief Linear Layer followed by rectification. In, Out and Bias use short ints. Filter uses byte.

Linear Layer followed by rectification. In, Out and Bias use short ints. Filter uses byte.

        Linear layer:   Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  short int, Linear filter coefficients. Dimension: InSize*OutSize
        Out     int, output vector
        OutSize output dimension
*/
void KerLinearLayer_halfchar(KerLinearLayer_fpsT *Arg);

/** @brief Linear Layer followed by rectification. In, Out and Bias use short ints. Filter uses byte.

Linear Layer followed by rectification. In, Out and Bias use short ints. Filter uses byte.

        Linear layer:   Out[i] = ReLU((sum_product(In, Filter[i], Dim: InSize) + Bias[i]<<NormBias) >> NormFilter)
        Outputs are evaluated in sequence, a given output is evaluated in parallel

        In      short int, input vector
        InSize  Number of inputs
        Filter  (half) char, Linear filter coefficients. Dimension: InSize*OutSize
        Out     short, output vector
        OutSize output dimension
*/
void KerLinearLayerReLU_fps(KerLinearLayer_fpsT *Arg);
void __attribute__ ((noinline)) HWCE_ProcessOneTile8x20(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H);

  void __attribute__ ((noinline)) HWCE_ProcessOneTile4x10(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H );

/** @} */ // End of CNNBasicKernels
/** @} */

#include "CNN_HwCE.h"

#endif
