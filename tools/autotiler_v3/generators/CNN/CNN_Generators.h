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

#ifndef __CNN_GENERATORS_H__
#define __CNN_GENERATORS_H__

/** @addtogroup groupCNN
@{ */

/**
 @brief Load Convolutional Neural Network Library

 Load Convolutional Neural Network Library
*/
extern void LoadCNNLibrary();

/** \brief CNN_ConvolutionPoolReLU
 *
 * Generators for Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    In_DataSize:    1: byte, 2: half word, 4: word
 \param    Filter_DataSize:1: byte, 2: half word, 4: word
 \param    Bias_DataSize:  1: byte, 2: half word, 4: word
 \param    Out_DataSize:   1: byte, 2: half word, 4: word

 \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
 \param    Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
 \param    Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
 \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

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

 \param    ReLUOper:       Optional linaer rectification to be performed as a final step, KOP_RELU or KOP_NONE

 */

extern void CNN_ConvolutionPoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int Out_DataSize,

	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
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
	int          ConvPad,

	KernelOper_T PoolOper,
	int Fpx,
	int Fpy,
	int Dpx,
	int Dpy,
	int Spx,
	int Spy,
	int          PoolPad,

       	KernelOper_T ReLUOper
	);

 /** \brief  CNN_PoolReLU

   Generators for Pooling (Max or Average) followed by an optional linear rectification (ReLU) or linear rectification only

   ParFeat = 0
   Input feature's maps are evaluated one after the other. A given feature map is tiled and the evaluation of a tile is dispatched on all cores for parallel evaluation.
   ParFeat = 1
   Input feature's maps are processed as a group, a tile crosses all input feature maps and then each core is given a subset of input feature's maps.

  \param     Name:           Name of the generated user kernel

  \param     Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

  \param     In_DataSize:    1: byte, 2: half word, 4: word
  \param     Out_DataSize:   1: byte, 2: half word, 4: word

  \param     In_InL3:        0: In is in L2, 1: In is in L3 memory
  \param     Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

  \param     InFeat:         Number of input feature's maps
  \param     OutFeat:        Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
  \param     Width:          Number of columns of a given feature map
  \param     Height:         Number of lines of a given feature map

  \param     PoolOper:       KOP_MAXPOOL or KOP_AVGPOOL or KOP_NONE
  \param     Fpx:            Size of the pooling filter, x dimension
  \param     Fpy:            Size of the pooling filter, y dimension
  \param     Dpx:            Dilation factor, x dimension
  \param     Dpy:            Dilation factor, y dimension
  \param     Spx:            Pooling stride, x dimension
  \param     Spy:            Pooling stride, y dimension

  \param     PoolPad:        0: No padding, 1: Zero padding
  \param     ReLUOper:       optional rectification to be applied after pooling, KOP_RELU or KOP_NONE

  Currently only homegeneous data size are supported (bytes and hald words)

 */

extern void CNN_PoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Out_DataSize,

	int In_InL3,           // 1 if In comes from L3, 0 if it comes from L2
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
	);

/** \brief CNN_LinearReLU
 *
 *  Generators for Linear layers followed by an optional linear rectification (ReLU)
 *
 *  Input, Bias and Output are assumed to fit into shared L1 (Buffer In or Out) and Filter is tiled.

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

    \param    In_DataSize:    1: byte, 2: half word, 4: word
    \param    Filter_DataSize:1: byte, 2: half word, 4: word
    \param    Bias_DataSize:  1: byte, 2: half word, 4: word
    \param    Out_DataSize:   1: byte, 2: half word, 4: word

    \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
    \param    Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
    \param    Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InDim:          Number of inputs
    \param    OutDim:         Number of outputs

    \param    KernelOper_T    LinearOper      Should always be KOP_LINEAR
    \param    KernelOper_T    ReLUOper        KOP_RELU if ReLU has to be applied after Linear, KOP_NONE otherwise

*/

extern void CNN_LinearReLU(
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
        );

/** \brief CNN_SoftMax

  Generators for SoftMax layers

  Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15 if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)

   \param    Name:           Name of the generated user kernel
   \param    Ctrl:           Overide generator default options (TileOrientation), Def=(TILE_HOR)

   \param    In_DataSize:    1: byte, 2: half word,
   \param    Out_DataSize:   2: half word (Q15 format)

   \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
   \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

   \param    Dim:            Number of inputs

   \param    KernelOper_T    SoftMaxOper     Should always be KOP_SOFTMAX

*/

extern void CNN_SoftMax(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,

        int In_InL3,
        int Out_InL3,

        int Dim,
        KernelOper_T SoftMaxOper
        );

/** \brief CNN_MatAdd

    Generators for Matrix Addition layers

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature

    \param    KernelOper_T    AddMatOper      Should always be KOP_MATADD

*/

extern void CNN_MatAdd(
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
	);

/** \brief CNN_MatMul

    Generators for Matrix Multiplication layers. Can be used for 2D 1x1 conv, in this case In1 contains the weights, In2 InputFeatures [Inf x W x H], Strides apply to In2

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Bias_DataSize:  1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Bias_InL3:      0: In is in L2, 1: In is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    ColM1:          Number of colums for matrix In1, for 1x1 convolution this is InFeat
    \param    LineM1:         Number of lines for matrix In1, for 1x1 convolution this is OutFeat
    \param    ColM2:          Number of colums for matrix In2, for 1x1 convolution this is Width*Height
    \param    LineM2:         Number of lines for matrix In2, for 1x1 convolution this is InFeat

    \param    Width           For 1x1 convolution, width of an input feature map
    \param    Height          For 1x1 convolution, height of an input feature map
    \param    Scx:            stride x dimension for In2
    \param    Scy:            stride y dimension for In2

    \param    KernelOper_T    MatMulOper   Should always be KOP_MATMUL
    \param    KernelOper_T    ReLUOper	   Optional Activation

*/
        
extern void CNN_MatMul(
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
	);

/** @} */
#endif
