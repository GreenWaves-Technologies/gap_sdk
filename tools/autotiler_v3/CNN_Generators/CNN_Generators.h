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
 * Generator for Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    In_DataSize:    1: byte, 2: half word, 4: word
 \param    Filter_DataSize:1: byte, 2: half word, 4: word
 \param    Bias_DataSize:  1: byte, 2: half word, 4: word
 \param    Out_DataSize:   1: byte, 2: half word, 4: word

 \param	   In_Q:           In fixed point format
 \param	   Filter_Q:       Filter fixed point format
 \param	   Bias_Q:         Bias fixed point format
 \param	   Out_Q:          Out fixed point format

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

 \param    ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, Out)
 \param                    Name(In, Filter, Bias, ReLUN, Out)

 */

extern int CNN_ConvolutionPoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int Out_DataSize,

	int In_Q,
	int Filter_Q,
	int Bias_Q,
	int Out_Q,

	int In_InL3,
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

/** \brief CNN_ConvolutionMulBiasPoolReLU
 *
 * Generator for Convolutions with multiplicative bias, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).


 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    In_DataSize:    1: byte, 2: half word, 4: word
 \param    Filter_DataSize:1: byte, 2: half word, 4: word
 \param    Bias_DataSize:  1: byte, 2: half word, 4: word
 \param    MulBias_DataSize:1: byte, 2: half word, 4: word
 \param    Out_DataSize:   1: byte, 2: half word, 4: word

 \param	   In_Q:           In fixed point format
 \param	   Filter_Q:       Filter fixed point format
 \param	   Bias_Q:         Bias fixed point format
 \param	   MulBias_Q:      MulBias fixed point format
 \param	   Out_Q:          Out fixed point format

 \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
 \param    Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
 \param    Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
 \param    MulBias_InL3:   0: MulBias is in L2, 1: MulBias is in L3 memory
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

 \param    ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, MulBias, Out)
 \param                    Name(In, Filter, Bias, MulBias, ReLUN, Out)

 */

extern int CNN_ConvolutionMulBiasPoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int MulBias_DataSize,
	int Out_DataSize,

	int In_Q,
	int Filter_Q,
	int Bias_Q,
	int MulBias_Q,
	int Out_Q,

	int In_InL3,
	int Filter_InL3,
	int Bias_InL3,
	int MulBias_InL3,
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

/** \brief CNN_GroupedConvolutionPoolReLU
 *
 * Generator for Grouped Convolutions, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    GroupIn:        Size of the group for input features
 \param    GroupOut:       Size of the group for output features

 \param    In_DataSize:    1: byte, 2: half word, 4: word
 \param    Filter_DataSize:1: byte, 2: half word, 4: word
 \param    Bias_DataSize:  1: byte, 2: half word, 4: word
 \param    Out_DataSize:   1: byte, 2: half word, 4: word

 \param	   In_Q:           In fixed point format
 \param	   Filter_Q:       Filter fixed point format
 \param	   Bias_Q:         Bias fixed point format
 \param	   Out_Q:          Out fixed point format

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

 \param    ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, Out)
 \param                    Name(In, Filter, Bias, ReLUN, Out)

 */

extern int CNN_GroupedConvolutionPoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int GroupIn,
	int GroupOut,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int Out_DataSize,

	int In_Q,
	int Filter_Q,
	int Bias_Q,
	int Out_Q,

	int In_InL3,
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

/** \brief CNN_GroupedConvolutionPoolReLU
 *
 * Generator for Grouped Convolutions with multiplicative bias, followed by an optional pooling (Max or Average), followed by an optional linear rectification (ReLU).

 \param    Name:           Name of the generated user kernel

 \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

 \param    GroupIn:        Size of the group for input features
 \param    GroupOut:       Size of the group for output features

 \param    In_DataSize:    1: byte, 2: half word, 4: word
 \param    Filter_DataSize:1: byte, 2: half word, 4: word
 \param    Bias_DataSize:  1: byte, 2: half word, 4: word
 \param    MulBias_DataSize:1: byte, 2: half word, 4: word
 \param    Out_DataSize:   1: byte, 2: half word, 4: word

 \param	   In_Q:           In fixed point format
 \param	   Filter_Q:       Filter fixed point format
 \param	   Bias_Q:         Bias fixed point format
 \param	   MulBias_Q:      MulBias fixed point format
 \param	   Out_Q:          Out fixed point format

 \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
 \param    Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
 \param    Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
 \param    MulBias_InL3:   0: MulBias is in L2, 1: MulBias is in L3 memory
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

 \param    ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

 \param    Signature:      Name(In, Filter, Bias, MulBias, Out)
 \param                    Name(In, Filter, Bias, MulBias, ReLUN, Out)

 */

extern int CNN_GroupedConvolutionMulBiasPoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int GroupIn,
	int GroupOut,

	int In_DataSize,
	int Filter_DataSize,
	int Bias_DataSize,
	int MulBias_DataSize,
	int Out_DataSize,

	int In_Q,
	int Filter_Q,
	int Bias_Q,
	int MulBias_Q,
	int Out_Q,

	int In_InL3,
	int Filter_InL3,
	int Bias_InL3,
	int MulBias_InL3,
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

   Generator for Pooling (Max or Average) followed by an optional linear rectification (ReLU) or linear rectification only

   ParFeat = 0
   Input feature's maps are evaluated one after the other. A given feature map is tiled and the evaluation of a tile is dispatched on all cores for parallel evaluation.
   ParFeat = 1
   Input feature's maps are processed as a group, a tile crosses all input feature maps and then each core is given a subset of input feature's maps.

  \param     Name:           Name of the generated user kernel

  \param     Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

  \param     In_DataSize:    1: byte, 2: half word, 4: word
  \param     Out_DataSize:   1: byte, 2: half word, 4: word

  \param     In_Q:           In fixed point format
  \param     Out_Q:          Out fixed point format

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
  \param     ReLUOper:       Optional activation function: if (PoolOper!=KOP_NONE) KOP_RELU or KOP_NONE
  \param                     else Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

  \param     ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

  \param     Signature:      Name(In, Out)
  \param                     Name(In, ReLUN, Out)

  Currently only homegeneous data size are supported (bytes and hald words)

 */

extern int CNN_PoolReLU(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Out_DataSize,

	int In_Q,
	int Out_Q,

	int In_InL3,
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

/** \brief CNN_GlobalPool
 *  Generator for Global Pooling (Max or Average)
 * 
 
    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In_DataSize:    1: byte, 2: half word, 4: word
    \param    Out_DataSize:   1: byte, 2: half word, 4: word

    \param    In_Q:           In fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InFeat:         Number of input feature's maps
    \param    OutFeat:        Number of output feature's maps (InFeat has to be equal to OutFeat for these generators
    \param    Width:          Number of columns of a given feature map
    \param    Height:         Number of lines of a given feature map

    \param    PoolOper:       KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL

    \param    Signature:      Name(In, Out)

  Currently only homegeneous data size are supported (bytes and hald words)
*/

extern int CNN_GlobalPool(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Out_DataSize,

	int In_Q,
	int Out_Q,

	int In_InL3,
	int Out_InL3,

	int InFeat,
	int OutFeat,
	int Width,
	int Height,

	KernelOper_T PoolOper
	);


/** \brief CNN_LinearReLU
 *
 *  Generator for Linear layers followed by an optional linear rectification (ReLU)
 *
 *  Input, Bias and Output are assumed to fit into shared L1 (Buffer In or Out) and Filter is tiled.

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

    \param    In_DataSize:    1: byte, 2: half word, 4: word
    \param    Filter_DataSize:1: byte, 2: half word, 4: word
    \param    Bias_DataSize:  1: byte, 2: half word, 4: word
    \param    Out_DataSize:   1: byte, 2: half word, 4: word

    \param    In_Q:           In fixed point format
    \param    Filter_Q:       Filter fixed point format
    \param    Bias_Q:         Bias fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
    \param    Filter_InL3:    0: Filter is in L2, 1: Filter is in L3 memory
    \param    Bias_InL3:      0: Bias is in L2, 1: Bias is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InDim:          Number of inputs
    \param    OutDim:         Number of outputs

    \param    LinearOper:     Should always be KOP_LINEAR
    \param    ReLUOper:       Optional activation function: KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

    \param    Signature:      Name(In, Filter, Bias, Out)
    \param                    Name(In, Filter, Bias, ReLUN, Out)

*/

extern int CNN_LinearReLU(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Filter_DataSize,
        int Bias_DataSize,
        int Out_DataSize,

	int In_Q,
	int Filter_Q,
	int Bias_Q,
	int Out_Q,

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

  Generator for SoftMax layers

  Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15 if DataSize is half word or in Q7 is DataSize is byte while for Input the point position must be provided (Norm)

   \param    Name:           Name of the generated user kernel
   \param    Ctrl:           Overide generator default options (TileOrientation), Def=(TILE_HOR)

   \param    In_DataSize:    1: byte, 2: half word,
   \param    Out_DataSize:   2: half word (Q15 format)

   \param    In_Q:           In fixed point format
   \param    Out_Q:          Out fixed point format

   \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
   \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

   \param    Dim:            Number of inputs

   \param    SoftMaxOper:    Should always be KOP_SOFTMAX
   \param    Signature:      Name(In, Out)

*/

extern int CNN_SoftMax(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,

	int In_Q,
	int Out_Q,

        int In_InL3,
        int Out_InL3,

        int Dim,
        KernelOper_T SoftMaxOper
        );

/** \brief CNN_MatAdd

    Generator for Matrix Addition layers

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature

    \param    AddMatOper:     Should always be KOP_MATADD

    \param    Signature:      Name(In1, In2, Out)

*/

extern int CNN_MatAdd(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In1_DataSize,
	int In2_DataSize,
	int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Out_Q,

	int In1_InL3,
	int In2_InL3,
	int Out_InL3,

	int InFeat,
	int OutFeat,
	int Width,
	int Height,

	KernelOper_T AddMatOper);


extern int CNN_MatAddRelu(
        char *Name,

	CNN_GenControl_T *Ctrl,

    int In1_DataSize,
    int In2_DataSize,
    int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Out_Q,

    int In1_InL3,
    int In2_InL3,
    int Out_InL3,

    int InFeat,
    int OutFeat,
    int Width,
    int Height,

    KernelOper_T AddMatOper,
	KernelOper_T ReLUOper
	);

/** \brief CNN_MatAddDynAdjust

    Generator for Matrix Addition layers with dynamic adjustement

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    In1_Q:          In1 Quantization
    \param    In2_Q:          In2 Quantization
    \param    Out_Q:          Out Quantization


    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature

    \param    AddMatOper:     Should always be KOP_MATADD

    \param    Signature:      Name(In1, In2, Out)

*/

extern int CNN_MatAddDynAdjust(
        char *Name,

	CNN_GenControl_T *Ctrl,

        int In1_DataSize,
        int In2_DataSize,
        int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Out_Q,

        int In1_InL3,
        int In2_InL3,
        int Out_InL3,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T AddMatOper
	);

/** \brief CNN_MatScale

    Generator for Matrix Scaling layers, one scale per channel or a single scale for all channels (scalar) or scalar for each and then vector per channel


    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In_DataSize:    1: byte, 2: half word,
    \param    Vector_DataSize:1: byte, 2: half word,
    \param    Scalar_DataSize:1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In_Q:           In fixed point format
    \param    Vector_Q:       Vector fixed point format
    \param    Scalar_Q:       Scalar fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In_InL3:        0: In is in L2, 1: In is in L3 memory
    \param    Vector_InL3:    0: Vector is in L2, 1: Vector is in L3 memory
    \param    Scalar_InL3:    0: Scalar is in L2, 1: Scalar is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    InFeat:         Number of input features
    \param    OutFeat:        Number of input features, should always be equal to InFeat
    \param    Width:          Width of a given feature
    \param    Height:         Height of a given feature

    \param    ScaleOper       Should always be KOP_MATSCALE_VECTOR, KOP_MATSCALE_SCALAR or KOP_MATSCALE_VECTOR_SCALAR
    \param    ReLUOper        Optional activation, should be KOP_NONE, KOP_RELU, KOP_RELUN

    \param    Signature:      Name(In, Scalar, Out)
    \param                    Name(In, Scalar, Out, ReLUN)
    \param                    Name(In, Vector, Out)
    \param                    Name(In, Vector, Out, ReLUN)
    \param                    Name(In, Vector, Scalar, Out)
    \param                    Name(In, Vector, Scalar, Out, ReLUN)

*/

extern int CNN_MatScale(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Vector_DataSize,
        int Scalar_DataSize,
        int Out_DataSize,

	int In_Q,
	int Vector_Q,
	int Scalar_Q,
	int Out_Q,

        int In_InL3,
        int Vector_InL3,
        int Scalar_InL3,
        int Out_InL3,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ScaleOper,
        KernelOper_T ReLUOper
);


/** \brief CNN_MatMul

    Generator for Matrix Multiplication layers.

    Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
    When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
    Bias [OutFeat x 1] is added to each individual features
    Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Bias_DataSize:  1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Bias_Q:         Bias fixed point format
    \param    Out_Q:          Out fixed point format

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

    \param    ReLU_LowerBound In case ReLUOper!=KOP_NONE Lower bound to be used for activation
    \param    ReLU_UpperBound In case ReLUOper!=KOP_NONE Upper bound to be used for activation

    \param    MatMulOper:     Should always be KOP_MATMUL
    \param    ReLUOper:	      Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

    \param    Signature:      Name(In2, In1, Bias, Out)
    \param                    Name(In2, In1, Bias, ReLUN, Out)

*/
        
extern int CNN_MatMul(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In1_DataSize,
        int In2_DataSize,
        int Bias_DataSize,
        int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Bias_Q,
	int Out_Q,

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

	int ReLU_LowerBound,
	int ReLU_UpperBound,

        KernelOper_T MatMulOper,
        KernelOper_T ReLUOper
	);

/** \brief CNN_MatMulSmallM1

    Generator for Matrix Multiplication layers.  Special form to handle small form factor In1 (InFeat x OutFeat)

    Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
    When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
    Bias [OutFeat x 1] is added to each individual features
    Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Bias_DataSize:  1: byte, 2: half word,
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Bias_Q:         Bias fixed point format
    \param    Out_Q:          Out fixed point format

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

    \param    ReLU_LowerBound In case ReLUOper!=KOP_NONE Lower bound to be used for activation
    \param    ReLU_UpperBound In case ReLUOper!=KOP_NONE Upper bound to be used for activation

    \param    MatMulOper:     Should always be KOP_MATMUL
    \param    ReLUOper:	      Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

    \param    Signature:      Name(In2, In1, Bias, Out)
    \param                    Name(In2, In1, Bias, ReLUN, Out)

*/
extern int CNN_MatMulSmallM1(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In1_DataSize,
        int In2_DataSize,
        int Bias_DataSize,
        int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Bias_Q,
	int Out_Q,

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

	int ReLU_LowerBound,
	int ReLU_UpperBound,

        KernelOper_T MatMulOper,
        KernelOper_T ReLUOper
	);


/** \brief CNN_MatMulScale
 
    Generator for Matrix Multiplication layers with output scaling.

    Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
    When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
    Bias [OutFeat x 1] is added to each individual features
    Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

    Template:
    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Bias_DataSize:  1: byte, 2: half word,
    \param    MulBias_DataSize:1: byte, 2: half word, 4: word
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Bias_Q:         Bias fixed point format
    \param    MulBias_Q:      MulBias fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Bias_InL3:      0: In is in L2, 1: In is in L3 memory
    \param    MulBias_InL3:   0: MulBias is in L2, 1: Bias is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    ColM1:          Number of colums for matrix In1, for 1x1 convolution this is InFeat
    \param    LineM1:         Number of lines for matrix In1, for 1x1 convolution this is OutFeat
    \param    ColM2:          Number of colums for matrix In2, for 1x1 convolution this is W*H
    \param    LineM2:         Number of lines for matrix In2, for 1x1 convolution this is InFeat

    \param    Width           For 1x1 convolution, width of an input feature map
    \param    Height          For 1x1 convolution, height of an input feature map
    \param    Scx:            stride x dimension for In2
    \param    Scy:            stride y dimension for In2

    \param    ReLU_LowerBound In case ReLUOper!=KOP_NONE Lower bound to be used for activation
    \param    ReLU_UpperBound In case ReLUOper!=KOP_NONE Upper bound to be used for activation

    \param    MatMulOper      Should always be KOP_MATMUL_SCALE_SCALAR or KOP_MATMUL_SCALE_SCALAR
    \param    ReLUOper        Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

    \param    Signature:      Name(In2, In1, Bias, Out)
    \param                    Name(In2, In1, Bias, ReLUN, Out)

*/

extern int CNN_MatMulScale(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In1_DataSize,
        int In2_DataSize,
        int Bias_DataSize,
        int MulBias_DataSize,
        int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Bias_Q,
	int MulBias_Q,
	int Out_Q,

        int In1_InL3,
        int In2_InL3,
        int Bias_InL3,
        int MulBias_InL3,
        int Out_InL3,

        int ColM1,
        int LineM1,
        int ColM2,
        int LineM2,

        int Width,
        int Height,
        int Scx,
        int Scy,

        int ReLU_LowerBound,
        int ReLU_UpperBound,

        KernelOper_T MatMulOper,
        KernelOper_T ReLUOper
	);

/** \brief CNN_MatMulScaleSmallM1
 
    Generator for Matrix Multiplication layers with output scaling.  Special form to handle small form factor In1 (InFeat x OutFeat).

    Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
    When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
    Bias [OutFeat x 1] is added to each individual features
    Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

    Template:
    \param    Name:           Name of the generated user kernel

    \param    Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

    \param    In1_DataSize:   1: byte, 2: half word,
    \param    In2_DataSize:   1: byte, 2: half word,
    \param    Bias_DataSize:  1: byte, 2: half word,
    \param    MulBias_DataSize:1: byte, 2: half word, 4: word
    \param    Out_DataSize:   1: byte, 2: half word

    \param    In1_Q:          In1 fixed point format
    \param    In2_Q:          In2 fixed point format
    \param    Bias_Q:         Bias fixed point format
    \param    MulBias_Q:      MulBias fixed point format
    \param    Out_Q:          Out fixed point format

    \param    In1_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    In2_InL3:       0: In is in L2, 1: In is in L3 memory
    \param    Bias_InL3:      0: In is in L2, 1: In is in L3 memory
    \param    MulBias_InL3:   0: MulBias is in L2, 1: Bias is in L3 memory
    \param    Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

    \param    ColM1:          Number of colums for matrix In1, for 1x1 convolution this is InFeat
    \param    LineM1:         Number of lines for matrix In1, for 1x1 convolution this is OutFeat
    \param    ColM2:          Number of colums for matrix In2, for 1x1 convolution this is W*H
    \param    LineM2:         Number of lines for matrix In2, for 1x1 convolution this is InFeat

    \param    Width           For 1x1 convolution, width of an input feature map
    \param    Height          For 1x1 convolution, height of an input feature map
    \param    Scx:            stride x dimension for In2
    \param    Scy:            stride y dimension for In2

    \param    ReLU_LowerBound In case ReLUOper!=KOP_NONE Lower bound to be used for activation
    \param    ReLU_UpperBound In case ReLUOper!=KOP_NONE Upper bound to be used for activation

    \param    MatMulOper      Should always be KOP_MATMUL_SCALE_SCALAR or KOP_MATMUL_SCALE_SCALAR
    \param    ReLUOper        Optionnal Activation (KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

    \param    Signature:      Name(In2, In1, Bias, Out)
    \param                    Name(In2, In1, Bias, ReLUN, Out)

*/
extern int CNN_MatMulScaleSmallM1(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In1_DataSize,
        int In2_DataSize,
        int Bias_DataSize,
        int MulBias_DataSize,
        int Out_DataSize,

	int In1_Q,
	int In2_Q,
	int Bias_Q,
	int MulBias_Q,
	int Out_Q,

        int In1_InL3,
        int In2_InL3,
        int Bias_InL3,
        int MulBias_InL3,
        int Out_InL3,

        int ColM1,
        int LineM1,
        int ColM2,
        int LineM2,

        int Width,
        int Height,
        int Scx,
        int Scy,

        int ReLU_LowerBound,
        int ReLU_UpperBound,

        KernelOper_T MatMulOper,
        KernelOper_T ReLUOper
	);

/** \brief CNN_MatTranspose

        Generator for Matrix Transposition

        Template:
        \param  Name:           Name of the generated user kernel

        \param  Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

        \param  In_DataSize:    1: byte, 2: half word,
        \param  Out_DataSize:   1: byte, 2: half word

    	\param  In_Q:           In fixed point format
    	\param  Out_Q:          Out fixed point format

        \param  In_InL3:        0: In is in L2, 1: In is in L3 memory
        \param  Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

        \param  InFeat          Number of matrices
        \param  Width           For 1x1 convolution, width of an input feature map
        \param  Height          For 1x1 convolution, height of an input feature map

	\param  Signature:	Name(In, Out)
*/
int CNN_MatTranspose(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,

	int In_Q,
	int Out_Q,

        int In_InL3,
        int Out_InL3,

        int InFeat,
        int Width,
        int Height
);


/** \brief CNN_3DTensorPermute
 
        Generator for 3D Tensor permutations:  CHW => {CWH, HWC, WHC, WCH, HCW}

        Template:
	\param	Name:           Name of the generated user kernel

	\param	Ctrl:           Overide generator default options

	\param	In_DataSize:    1: byte, 2: half word,
	\param	Out_DataSize:   1: byte, 2: half word

	\param	In_Q:           In fixed point format
	\param	Out_Q:          Out fixed point format

	\param	In_InL3:        0: In is in L2, 1: In is in L3 memory
	\param	Out_InL3:       0: Out is in L2, 1: Out is in L3 memory

	\param	InFeat          Number of channels of the tensor
	\param	Width           Tensor width
	\param	Height          Tensor height

	\param	MatPermOper     Permutation oper:  KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH, KOP_MATPERM_CHW2HCW

	\param  Signature:	Name(In, Out)

*/
int CNN_3DTensorPermute(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int In_DataSize,
	int Out_DataSize,

	int In_Q,
	int Out_Q,

	int In_InL3,
	int Out_InL3,

	int InFeat,
	int Width,
	int Height,
 	KernelOper_T MatPermOper
);

int CNN_ConvNxN_HWCE(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Filter_DataSize,
        int Bias_DataSize,
        int Out_DataSize,

        int In_Q,
        int Filter_Q,
        int Bias_Q,
        int Out_Q,

        int In_InL3,
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
        int ConvPad
        );

/** @} */
#endif
