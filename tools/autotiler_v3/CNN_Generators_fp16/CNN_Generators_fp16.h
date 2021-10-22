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

#ifndef __CNN_GENERATORS_FP16_H__
#define __CNN_GENERATORS_FP16_H__

#include "CNN_Copy_Generators.h"


/** @addtogroup groupCNN
@{ */

/**
 @brief Load Convolutional Neural Network Library for float 16

 Load Convolutional Neural Network Library for float 16
*/
extern void LoadCNNLibrary_fp16();

/** \brief CNN_ConvolutionPoolAct_fp16
 *
 * Generator for Convolutions, followed by an optional pooling (Max or Average), followed by an optional Activation.

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    InFeat:         Number of input feature's maps
 \param    OutFeat:        Number of output feature's maps
 \param    Width:          Number of columns of a given feature map
 \param    Height:         Number of lines of a given feature map

 \param    ConvOper:       Type of convolution, Regular convolution: KOP_CONV, Regular convolution with double precision output: KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
 \param    Fcx:            Convolution filter x dimension
 \param    Fcy:            Convolution filter y dimension
 \param    Dcx:            Convolution filter dilation factor, x dimension
 \param    Dcy:            Convolution filter dilation factor, y dimension
 \param    Scx:            Convolution filter stride x dimension
 \param    Scy:            Convolution filter stride y dimension
 \param    ConvPad:        0: No padding, 1: Zero padding

 \param    PoolOper:       Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
 \param    Fpx:            Pooling filter x dimension
 \param    Fpy:            Pooling filter y dimension
 \param    Dpx:            Pooling filter dilation factor, x dimension
 \param    Dpy:            Pooling filter dilation factor, y dimension
 \param    Spx:            Pooling filter stride x dimension
 \param    Spy:            Pooling filter stride y dimension
 \param    PoolPad:        0: No padding, 1: Zero padding

 \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, Out)

 */

extern int CNN_ConvolutionPoolAct_fp16(
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
	int          ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int          PoolPad,

       	KernelOper_T ActOper
	);

/** \brief CNN_GroupedConvolutionPoolAct_fp16
 *
 * Generator for Grouped Convolutions, followed by an optional pooling (Max or Average), followed by an optional Activation.

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    GroupIn:        Size of the group for input features
 \param    GroupOut:       Size of the group for output features

 \param    InFeat:         Number of input feature's maps
 \param    OutFeat:        Number of output feature's maps
 \param    Width:          Number of columns of a given feature map
 \param    Height:         Number of lines of a given feature map

 \param    ConvOper:       Type of convolution, Regular convolution: KOP_CONV, Regular convolution with double precision output: KOP_CONV_DP, Depth wise convolution: KOP_CONV_DW
 \param    Fcx:            Convolution filter x dimension
 \param    Fcy:            Convolution filter y dimension
 \param    Dcx:            Convolution filter dilation factor, x dimension
 \param    Dcy:            Convolution filter dilation factor, y dimension
 \param    Scx:            Convolution filter stride x dimension
 \param    Scy:            Convolution filter stride y dimension
 \param    ConvPad:        0: No padding, 1: Zero padding

 \param    PoolOper:       Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
 \param    Fpx:            Pooling filter x dimension
 \param    Fpy:            Pooling filter y dimension
 \param    Dpx:            Pooling filter dilation factor, x dimension
 \param    Dpy:            Pooling filter dilation factor, y dimension
 \param    Spx:            Pooling filter stride x dimension
 \param    Spy:            Pooling filter stride y dimension
 \param    PoolPad:        0: No padding, 1: Zero padding

 \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, Out)

 */

extern int CNN_GroupedConvolutionPoolAct_fp16(
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
	int          ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int          PoolPad,

       	KernelOper_T ActOper
	);

/** \brief CNN_PoolAct_fp16
 *
 * Generator for pooling (Max or Average), followed by an optional Activation.

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    InFeat:         Number of input feature's maps
 \param    OutFeat:        Number of output feature's maps
 \param    Width:          Number of columns of a given feature map
 \param    Height:         Number of lines of a given feature map

 \param    PoolOper:       Type of Pooling, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
 \param    Fpx:            Pooling filter x dimension
 \param    Fpy:            Pooling filter y dimension
 \param    Dpx:            Pooling filter dilation factor, x dimension
 \param    Dpy:            Pooling filter dilation factor, y dimension
 \param    Spx:            Pooling filter stride x dimension
 \param    Spy:            Pooling filter stride y dimension
 \param    PoolPad:        0: No padding, 1: Zero padding

 \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Out)

 */

extern int CNN_PoolAct_fp16(
	char         *Name,

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
	);

/** \brief CNN_GlobalPoolAct_fp16
 *  Generator for Global Pooling (Max or Average), followed by an optional Activation.
 * 
 
    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    InFeat:         Number of input feature's maps
    \param    OutFeat:        Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
    \param    Width:          Number of columns of a given feature map
    \param    Height:         Number of lines of a given feature map

    \param    PoolOper:       KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL
    \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

    \param    Signature:      Name(In, Out)

*/

extern int CNN_GlobalPoolAct_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int InFeat,
	int OutFeat,
	int Width,
	int Height,

	KernelOper_T PoolOper,
	KernelOper_T ActOper
	);

/*********************************************************************************************************************************************************************
        Generator for Activation

        Template:
                Name:           Name of the generated user kernel

                Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

                Feat:           Number of feature's maps
                Width:          Number of columns of a given feature map
                Height:         Number of lines of a given feature map

                ActOper:        KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

                Signature:      Name(In, Out, Infos)

        CNN_Act_fp16

*********************************************************************************************************************************************************************/

extern int CNN_Act_fp16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int Feat,
        int Width,
        int Height,

        KernelOper_T ActOper
        );


/** \brief CNN_LinearAct_fp16
 *
 *  Generator for Linear layers followed by an optional Activation.
 *
 *  Input, Bias and Output are assumed to fit into shared L1 (Buffer In or Out) and Filter is tiled.

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

    \param    InDim:          Number of inputs
    \param    OutDim:         Number of outputs

    \param    LinearOper:     Should always be KOP_LINEAR
    \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

    \param    Signature:      Name(In, Filter, Bias, Out)

*/

extern int CNN_LinearAct_fp16(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int InDim,
        int OutDim,

        KernelOper_T LinearOper,
        KernelOper_T ActOper
        );

/** \brief CNN_SoftMax_fp16

  Generator for SoftMax layers

  Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15 if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)

   \param    Name:           Name of the generated user kernel
   \param    Ctrl:           Overide generator default options (TileOrientation), Def=(TILE_HOR)

   \param    Dim:            Number of inputs

   \param    SoftMaxOper:    Should always be KOP_SOFTMAX
   \param    Signature:      Name(In, Out)

*/

extern int CNN_SoftMax_fp16(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int Dim,
        KernelOper_T SoftMaxOper
        );

extern int CNN_SoftMax2D_fp16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int Dim,
        int N,
        KernelOper_T SoftMaxOper
        );

/** \brief CNN_MatAddAct_fp16

    Generator for Matrix Addition layers

    \param    Name:           Name of the generated user kernel followed by an optional Activation.

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature

    \param    AddMatOper:     Should always be KOP_MATADD
    \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

    \param    Signature:      Name(In1, In2, Out)

*/

extern int CNN_MatAddAct_fp16(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int InFeat,
	int OutFeat,
	int Width,
	int Height,

	KernelOper_T AddMatOper,
	KernelOper_T ActOper
	);

/** \brief CNN_MatAddPaddedAct_fp16

    Generator for Matrix Addition layers

    \param    Name:           Name of the generated user kernel followed by an optional Activation.

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature
    \param    PadTop:         Number of feature to skip in the IdxPaddedIn Input at the beginning
    \param    PadBot:         Number of feature to skip in the IdxPaddedIn Input at the end
    \param    IdxPaddedIn:    Which input must be skipped (0 or 1)

    \param    AddMatOper:     Should always be KOP_MATADD
    \param    ActOper:        Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

    \param    Signature:      Name(In1, In2, Out)

*/

extern int CNN_MatAddPaddedAct_fp16(
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
        );

/** \brief CNN_MatMulAct_fp16

    Generator for Matrix Multiplication layers.

    Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
    When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
    Bias [OutFeat x 1] is added to each individual features
    Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    ColM1:          Number of colums for matrix In1, for 1x1 convolution this is InFeat
    \param    LineM1:         Number of lines for matrix In1, for 1x1 convolution this is OutFeat
    \param    ColM2:          Number of colums for matrix In2, for 1x1 convolution this is Width*Height
    \param    LineM2:         Number of lines for matrix In2, for 1x1 convolution this is InFeat

    \param    Width           For 1x1 convolution, width of an input feature map
    \param    Height          For 1x1 convolution, height of an input feature map
    \param    Scx:            stride x dimension for In2
    \param    Scy:            stride y dimension for In2

    \param    MatMulOper:     Should always be KOP_MATMUL
    \param    ActOper:	      Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

    \param    Signature:      Name(In2, In1, Bias, Out)
*/
        
extern int CNN_MatMulAct_fp16(
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
	);

/*********************************************************************************************************************************************************************
        Generator for Matrix Multiplication layers. Special form to handle small form factor In1 (InFeat x OutFeat)

        Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
        When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
        Bias [OutFeat x 1] is added to each individual features
        Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP
        This generator assumes that the whole In1 and Bias can be accomodated into shared L1. Expecting to be used for us to 32 InFeat x 32 OutFeat

        Template:
                Name:           Name of the generated user kernel

                Ctrl:           Overide generator default options (ReluN), Def=(6)

                ColM1:          Number of colums for matrix In1, for 1x1 convolution this is InFeat
                LineM1:         Number of lines for matrix In1, for 1x1 convolution this is OutFeat
                ColM2:          Number of colums for matrix In2, for 1x1 convolution this is W*H
                LineM2:         Number of lines for matrix In2, for 1x1 convolution this is InFeat

                Width           For 1x1 convolution, width of an input feature map
                Height          For 1x1 convolution, height of an input feature map
                Scx:            stride x dimension for In2
                Scy:            stride y dimension for In2

                MatMulOper      Should always be KOP_MATMUL_SM1
                ActOper         Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

                Signature:      Name(In2, In1, Bias, Out)

        CNN_MatMulSmallM1Act_fp16

*********************************************************************************************************************************************************************/

extern int CNN_MatMulSmallM1Act_fp16(
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
);

/** @} */
#endif
