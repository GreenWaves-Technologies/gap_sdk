/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef __CNNGENERATOR_H__
#define __CNNGENERATOR_H__

/** @addtogroup groupCNN
@{ */

/** @defgroup CNNGene CNNGenerator
@{ */

/** @brief Load CNN related basic kernels, pure SW versions

Load CNN related basic kernels, pure SW versions
*/
void CNN_LoadSoftwareKernelLibrary();

/** @brief Load CNN related HWCE accelerated basic kernels.

Load CNN related HWCE accelerated basic kernels.
*/
void CNN_LoadHWCEKernelLibrary();


/** @brief Generic NxN Pure SW convolutions (Without Accumulation with the output plane).

Generic NxN Pure SW convolutions (Without Accumulation with the output plane).
*/
void CNN_TiledPureConvNxN_SW_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, needs to be odd */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height		/**< Input plane height */
	);

/** @brief Generic NxN SW convolution layer (Out is init to bias and then accumulated).

Generic NxN SW convolution layer (Out is init to bias and then accumulated).
*/
void CNN_TiledConvNxN_SW_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, needs to be odd */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height		/**< Input plane height */
	);

/** @brief Generic NxM SW convolution layer (Out is init to bias and then accumulated).

Generic NxM SW convolution layer (Out is init to bias and then accumulated).
*/
void CNN_TiledConvNxMReLUPool2x2_SW_fp(char *Name, unsigned int N, unsigned int M, unsigned int InPlane, unsigned int OutPlane,
				       unsigned int Width, unsigned int Height, unsigned int PoolMax);

/** @brief Generic NxM SW convolution layer (Out is init to bias and then accumulated).

Generic NxM SW convolution layer (Out is init to bias and then accumulated).
with filter coefficients are char

*/
void CNN_TiledConvNxMReLUPool2x2_SW_char(char *Name, unsigned int N, unsigned int M, unsigned int InPlane, unsigned int OutPlane,
					 unsigned int Width, unsigned int Height, unsigned int PoolMax);


/** @brief Generic NxN SW convolution layer (Out is init to bias and then accumulated). Parametric stride.

Generic NxN SW convolution layer (Out is init to bias and then accumulated). Parametric stride.
*/
void CNN_TiledConvNxNStrided_SW_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, needs to be odd */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height,		/**< Input plane height */
	unsigned int Stride		/**< Stride */
	);

/* @brief Software Composite kernels: Convolution, ReLU then Max or Avg Pooling.

Software Composite kernels: Convolution, ReLU then Max or Avg Pooling.
*/
void CNN_TiledConvNxNReLUPool2x2_SW_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, needs to be odd */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height,		/**< Input plane height */
	unsigned int PoolMax		/**< 0: ReLU then Average 2x2 pooling, 1: ReLU then Max 2x2 pooling,
					     2: Average 2x2 pooling,           3: Max 2x2 pooling.
					     4: ReLU then Average 3x3 pooling, 5: ReLU then Max 3x3 pooling,
					     6: Average 3x3 pooling,           7: Max 3x3 pooling.
					*/
	);

/* @brief Software Composite kernels: Convolution, ReLU then Max or Avg Pooling. Parametric Stride

Software Composite kernels: Convolution, ReLU then Max or Avg Pooling. Parametric Stride
*/
void CNN_TiledConvNxNReLUPoolStrided_SW_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, needs to be odd */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height,		/**< Input plane height */
	unsigned int PoolMax,		/**< 0: ReLU then Average 2x2 pooling, 1: ReLU then Max 2x2 pooling,
					     2: Average 2x2 pooling,           3: Max 2x2 pooling.
					     4: ReLU then Average 3x3 pooling, 5: ReLU then Max 3x3 pooling,
					     6: Average 3x3 pooling,           7: Max 3x3 pooling.
					*/
	unsigned int Stride		/**< Stride for the convolution */
	);

/* @brief HWCE enabled Pure convolution, no bias setting and no accumulation.

HWCE enabled Pure convolution, no bias setting and no accumulation.
*/
void CNN_TiledPlainConvNxN_HWCE_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, 3, 5, or 7 */
	unsigned int Width,		/**< Input plane width, Width should be even */
	unsigned int Height		/**< Input plane height */
	);

/* @brief HWCE enabled Convolution layer, bias setting and accumulation.

HWCE enabled Convolution layer, bias setting and accumulation.
*/
void CNN_TiledConvNxN_HWCE_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, 3, 5 or 7*/
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width, Width should be even */
	unsigned int Height		/**< Input plane height */
	);

/* @brief HWCE enabled Convolution layer, 3x3 convolution multiple output mode, bias setting and accumulation.

HWCE enabled Convolution layer, 3x3 convolution multiple output mode, bias setting and accumulation.
*/
void CNN_TiledConv3x3_HWCE_MultiOut_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int Nout,		/**< Number of output processed in parallel: 1, 2 or 3 */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width, Width should be even */
	unsigned int Height		/**< Input plane height */
	);

/* @brief HWCE enabled composite kernels: NxN convolution (3/5/7), ReLU and Max or Average Pooling 2x2 -> 1.

HWCE enabled composite kernels: NxN convolution (3/5/7), ReLU and Max or Average Pooling 2x2 -> 1.
*/
void CNN_TiledConvNxNReLUPool2x2_HWCE_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int FS,		/**< Convolution dimension, 3, 5 or 7 */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width, Width should be even */
	unsigned int Height,		/**< Input plane height */
	unsigned int PoolMax		/**< 0: ReLU then Average 2x2 pooling, 1: ReLU then Max 2x2 pooling, 2: Average 2x2 pooling, 3: Max 2x2 pooling. */
	);


/* @brief HWCE enabled composite kernels: NxN convolutions, ReLU and Max or Average Pooling 2x2 -> 1 */
void CNN_TiledConvNxMReLUPool2x2_HWCE_fp(
        char *Name,
        unsigned int FSN,
        unsigned int FSM,
        unsigned int InPlane,
        unsigned int OutPlane,
        unsigned int Width,
        unsigned int Height,
        unsigned int PoolMax);

/* @brief HWCE enabled composite kernel: 3x3 convolution, ReLU and Max Pooling 2x2 -> 1, multiple out mode.

HWCE enabled composite kernel: 3x3 convolution, ReLU and Max Pooling 2x2 -> 1, multiple out mode.
*/
void CNN_TiledConv3x3ReLUPool2x2_HWCE_MultiOut_fp(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int Nout,		/**< Number of simultaneously produced outputs, 1, 2 or 3 */
        unsigned int InPlane,		/**< Number of input planes */
	unsigned int OutPlane,		/**< Number of output planes */
	unsigned int Width,		/**< Input plane width, Width should be even */
	unsigned int Height,		/**< Input plane height */
        unsigned int PoolMax		/**< 0: Average 2x2 pooling. 1: Max 2x2 pooling */
	);

/* @brief Linear layer Software composite kernels: linear layer then ReLU, pure linear layer on different data format.

Linear layer Software composite kernels: linear layer then ReLU, pure linear layer on different data format.
*/
void CNN_TiledLinearLayer(
	char *Name,			/**< Name of the generated tiled kernel */
	unsigned int InPlane,		/**< Number of input planes */
	unsigned int Width,		/**< Input plane width */
	unsigned int Height,		/**< Input plane height */
	unsigned int OutPlane,		/**< Number of output planes */
	int ModeSize,			/**< 0: Coeff: byte,   In: word16, Out: word16,  
					   1: Coeff: word16, In: word16, Out: word16  
					   2: Coeff: word16, In: word16, Out: word32  
					*/
	int ReLU,			/**< 0: No ReLU after linear layer. 1: ReLU after linear layer */
	int CoeffInL3			/**< 0: Coeff are in L2. 1: Coeff are in external flash */
	);

/**
@brief Setup AutoTiler configuration for CNN generators.

Setup AutoTiler configuration for CNN generators.
*/
void CNNConfiguration(
	unsigned int L1Memory
	);
/** @} */ // End of CNNGene
/** @} */

#endif
