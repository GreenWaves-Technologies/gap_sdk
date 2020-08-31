#ifndef __CNN_GENERATORS_SQ8_H__
#define __CNN_GENERATORS_SQ8_H__
#include <stdint.h>
#include "AutoTilerLib.h"

void LoadCNN_SQ8_Library();

/*********************************************************************************************************************************************************************
 	Generator for Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional Activation.

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use HWCE), Def=(TILE_HOR, 1, 0)

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, Depth wise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, KOP_NONE, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_ConvolutionPoolAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_ConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

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
	int ConvPad,

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

/*********************************************************************************************************************************************************************
 	Generator for Grouped Convolutions with channel centric scaling, followed by an optional pooling (Max or Average),
	followed by an optional activation.

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features, Use double precision convolution, Use HWCE), Def=(TILE_HOR, 1, 0, 0)

		GroupIn:	Size of the group for input features
		GroupOut:	Size of the group for output features

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InFeat:		Number of input feature's maps
		OutFeat:	Number of output feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		ConvOper:	Type of convolution, Regular convolution: KOP_CONV, DepthWise convolution: KOP_CONV_DW
		Fcx:		Convolution filter x dimension
		Fcy:		Convolution filter y dimension
		Dcx:		Convolution filter dilation factor, x dimension
		Dcy:		Convolution filter dilation factor, y dimension
		Scx:		Convolution filter stride x dimension
		Scy:		Convolution filter stride y dimension
		ConvPad:	0: No padding, 1: Zero padding

		PoolOper:	Type of Pooling, KOP_NONE, Max Pooling: KOP_MAXPOOL, Average Pooling: KOP_AVGPOOL
		Fpx:		Pooling filter x dimension
		Fpy:		Pooling filter y dimension
		Dpx:		Pooling filter dilation factor, x dimension
		Dpy:		Pooling filter dilation factor, y dimension
		Spx:		Pooling filter stride x dimension
		Spy:		Pooling filter stride y dimension
		PoolPad:	0: No padding, 1: Zero padding

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_GroupedConvolutionPoolAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_GroupedConvolutionPoolAct_SQ8(
	char         *Name,

	CNN_GenControl_T *Ctrl,

	int GroupIn,
	int GroupOut,

	int Bias_DataSize,
	int Scale_DataSize,

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
	int ConvPad,

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

/*********************************************************************************************************************************************************************
 	Generator for Pooling (Max or Average) with tensor centric scaling followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_MAXPOOL or KOP_AVGPOOL
		Fpx:		Size of the pooling filter, x dimension
		Fpy:		Size of the pooling filter, y dimension
		Dpx:		Dilation factor, x dimension
		Dpy:		Dilation factor, y dimension
		Spx:		Pooling stride, x dimension
		Spy:		Pooling stride, y dimension

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Out, Infos)

	CNN_PoolAct_SQ8
		
*********************************************************************************************************************************************************************/

int CNN_PoolAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
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

/*********************************************************************************************************************************************************************
        Generator for Activation with tensor centric scaling

        Template:
                Name:           Name of the generated user kernel

                Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

                Feat:           Number of feature's maps
                Width:          Number of columns of a given feature map
                Height:         Number of lines of a given feature map

                ActOper:        KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

                Signature:      Name(In, Out, Infos)

        CNN_Act_SQ8

*********************************************************************************************************************************************************************/

int CNN_Act_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

       	KernelOper_T ActOper
       	);


/*********************************************************************************************************************************************************************
 	Generator for Global Pooling (Max or Average) with tensor centric scaling and optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of feature's maps
		Width:		Number of columns of a given feature map
		Height:		Number of lines of a given feature map

		PoolOper:	KOP_GLOBAL_MAXPOOL or KOP_GLOBAL_AVGPOOL

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Out, Infos)


	CNN_GlobalPoolAct_SQ8
		
*********************************************************************************************************************************************************************/

int CNN_GlobalPoolAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

       	int Feat,
       	int Width,
       	int Height,

	KernelOper_T PoolOper,
	KernelOper_T ActOper
	);

/*********************************************************************************************************************************************************************
 	Generator for Linear layers followed with channel centric scaling followed by an optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

		Bias_DataSize:	1: byte, 2: half word, 4: word
		Scale_DataSize:	1: byte, 2: half word, 4: word

		InDim:		Number of inputs
		OutDim:		Number of outputs

		LinearOper	KOP_LINEAR

		ActOper		Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

	CNN_LinearAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_LinearAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

	int InDim,
	int OutDim,

	KernelOper_T LinearOper,
	KernelOper_T ActOper
	);

/*********************************************************************************************************************************************************************
 	Generator for SoftMax layers, no scaling

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation), Def=(TILE_HOR)

		Dim:		Number of inputs

		SoftMaxOper:	Should always be KOP_SOFTMAX

		Signature:	Name(In, Out, Infos)

	CNN_SoftMax_SQ8
                Input and output are assumed to fit within given shared L1 memory. Dim is partitionned into subsets of inputs and each subset is given to
                a different code. By definition Output contains value is the [0.0 .. 1.0] range with sum(Output)=1.0. Results are always represented in Q15
*********************************************************************************************************************************************************************/

int CNN_SoftMax_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Dim,

        KernelOper_T SoftMaxOper
	);

/*********************************************************************************************************************************************************************
 	Generator for Matrix Addition layers with input scale adjustment (tensor centric), output scaling (tensor centric) and optional activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat:		Number of features
		Width:		Width of a given feature
		Height:		Height of a given feature

		AddMatOper:	Should always be KOP_MATADD
		ActOper:	Optional activation

		Signature:	Name(In1, In2, Out, Infos)

	CNN_MatAddAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatAddAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

        KernelOper_T AddMatOper,
        KernelOper_T ActOper
	);

/*********************************************************************************************************************************************************************
 	Generator for Tensor [CxHxW] by Vector [C] product with tensor centric scaling and optional Activation

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		InFeat:		Number of input features
		Width:		Width of a In1
		Height:		Height of a In1

		MatOper:	KOP_MATVECTMUL

		ActOper:	Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

		Signature:	Name(In1, In2, Out, Infos)

	CNN_TensorVectMultAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_TensorVectMultAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

        KernelOper_T MatOper,
        KernelOper_T ActOper
	);

/*********************************************************************************************************************************************************************
 	Generator for Matrix Multiplication layers with channel centric scaling followed by an optional Activation.

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options

		Bias_DataSize:	1: byte, 2: half word,
		Scale_DataSize:	1: byte, 2: half word, 4: word

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

		MatMulOper:	Should always be KOP_MATMUL

		ActOper:	Optionnal Activation (KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out, Scale, ScaleN, Infos)

	CNN_MatMulAct_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatMulAct_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

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
 	Generator for Matrix Multiplication layers with channel centric scaling followed by an optional Activation.
	Special form to handle small form factor In1 (InFeat x OutFeat)

	Can be used for 1x1 convolutions with Filters in In1 [OutFeat x InFeat] and Features in In2 [InFeat x W*H]
	When non unit strides are used they apply to In2, produced output is [OutFeat x Floor((W+Scx-1)/Scx)*Floor((H+Scy-1)/Scy)]
	Bias [OutFeat x 1] is added to each individual features
	Line x Col sum of products are evaluated on 32 bits therefore, when used for 1x1 convolution, this generator is equivalent to KOP_CONV_DP
	This generator assumes that the whole In1 and Bias can be accomodated into shared L1. Expecting to be used for us to 32 InFeat x 32 OutFeat

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options

		Bias_DataSize:	1: byte, 2: half word,
		Scale_DataSize:	1: byte, 2: half word, 4: word

		ColM1:		Number of colums for matrix In1, for 1x1 convolution this is InFeat
		LineM1:		Number of lines for matrix In1, for 1x1 convolution this is OutFeat
		ColM2:		Number of colums for matrix In2, for 1x1 convolution this is W*H
		LineM2:		Number of lines for matrix In2, for 1x1 convolution this is InFeat

		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map
		Scx:		stride x dimension for In2
		Scy:		stride y dimension for In2

        	MatMulOper	Should always be KOP_MATMUL

        	ActOper		Optionnal Activation (KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU)

		Signature:	Name(In2, In1, Bias, Out, Scale, ScaleN, Infos)

	CNN_MatMulSmallM1Act_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatMulSmallM1Act_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Bias_DataSize,
	int Scale_DataSize,

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
 	Generator for Matrix Transposition, no scaling

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat		Number of matrices
		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map

		MatTransOper	KOP_MAT_TRANSPOSE

		Signature:	Name(In, Out)

	CNN_MatTranspose_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_MatTranspose_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatTransOper
	);

/*********************************************************************************************************************************************************************
 	Generator for 3D Tensor permutations:  CHW => {CWH, HWC, WHC, WCH, HCW}, no scaling

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		Feat		Number of channels of the tensor
		Width		Tensor width
		Height		Tensor height

		MatPermOper	Permutation oper:  KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH, KOP_MATPERM_CHW2HCW

		Signature:	Name(In, Out)

	CNN_3DTensorPermute_SQ8
	
*********************************************************************************************************************************************************************/

int CNN_3DTensorPermute_SQ8(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatPermOper
	);

#endif
