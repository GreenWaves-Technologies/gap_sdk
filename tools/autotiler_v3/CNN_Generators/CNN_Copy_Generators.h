#ifndef __CNN_COPY_GENERATORS_H__
#define __CNN_COPY_GENERATORS_H__
#include <stdint.h>
#include "AutoTilerLib.h"

void LoadCNN_Copy_Library();

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

#define CNN_MatTranspose_SQ8(__name, __ctrl, __feat, __width, __hieght, __oper)\
	CNN_MatTranspose(__name, __ctrl, 1, 1, 0, 0, 0, 0, __feat, __width, __hieght)


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

#define CNN_3DTensorPermute_SQ8(__name, __ctrl, __feat, __width, __hieght, __oper) \
	CNN_3DTensorPermute(__name, __ctrl, 1, 1, 0, 0, 0, 0, __feat, __width, __hieght, __oper)

int CNN_Copy(
	char *Name,
    CNN_GenControl_T *Ctrl,
	int Sz
);

int CNN_Norm(
	char *Name,

	int Width,
	int Height,
	int DoOffset,
	KernelOper_T kop
);

int CNN_SignedUnsigned(
	char *Name,
    int In_DataSize,
	int Out_DataSize,
	int Sz
);

#endif
