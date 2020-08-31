#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"
#include "CNN_Generator_Util.h"
#include "Gap.h"

int CNN_EncodePoolOperation(KernelOper_T PoolOper, KernelOper_T ReLUOper)

{
	int Pool=0;
	int ReLU = (ReLUOper==KOP_RELU)?1:0;

	switch (PoolOper) {
		case KOP_GLOBAL_MAXPOOL:
		case KOP_MAXPOOL: Pool = 0; break;
		case KOP_GLOBAL_AVGPOOL:
		case KOP_AVGPOOL: Pool = 1; break;
	}
	return ((Pool<<1)|ReLU);
}

int CNN_EncodeActivation(KernelOper_T Oper)

{
	switch (Oper) {
		case KOP_RELU: return 1;
		case KOP_RELUN: return 2;
		case KOP_HSIGMOID: return 3;
		case KOP_HSWISH: return 4;
		case KOP_LEAKYRELU: return 5;
		default: return 0;
	}
}

int CNN_Gcd(int a, int b)

{
	int x, y, z;

	x = Abs (a); y = Abs (b);
	while (x > 0) {
		z = y % x; y = x; x = z;
	}
	return y;
}

int CNN_Scm(int a, int b)

{
        return ((a*b)/CNN_Gcd(a,b));
}

int CNN_UsedInputDimension(int Dim, int F, int S, int D, int Pad)

{
	/* Dim: input dimension, F: Filter dim, S: Stride, D: Dilation, Pad: pad values (sum of both sides) */
	return ((Dim-1)*S+(D*(F-1)+1)-Pad);
}

int CNN_TotalPaddingValue(int Dim, int F, int S, int D)

{
	/* F: Filter dim, S: Stride, D: Dilation */
	return ((Dim%S) == 0)?Max((D*(F-1)+1)-S, 0):Max((D*(F-1)+1) - (Dim%S), 0);
}

v4s CNN_EdgePaddingValue(AT_PadType PadType, int Padw, int Padh)

{
	v4s Pad;
        switch (PadType) {
		case PAD_LEFT: Pad = (v4s) {Padw, 0, Padh, 0}; break;
		case PAD_RIGHT: Pad = (v4s) {0, Padw, 0, Padh}; break;
		case PAD_BALANCED_LEFT: Pad = (v4s) {Padw-Padw/2, Padw/2, Padh-Padh/2, Padh/2}; break;
		case PAD_BALANCED_RIGHT: Pad = (v4s) {Padw/2, Padw-Padw/2, Padh/2, Padh-Padh/2}; break;
		default: GenTilingError("CNN_EdgePaddingValue: unknown padding method %d", PadType);
        }
	return Pad;
}

void CNN_LayerOutputDim(int Width, int Height,
			KernelOper_T ConvOper, int Fcx, int Fcy, int Dcx, int Dcy, int Scx, int Scy, int ConvPad,
			KernelOper_T PoolOper, int Fpx, int Fpy, int Dpx, int Dpy, int Spx, int Spy, int PoolPad,
			int *Wc, int *Hc, int *Wo, int *Ho,
			int *Pcw, int *Pch, int *Ppw, int *Pph)

{
	/* Convolution: Fc = Filter dim, Sc = Stride, Dc = Dilation
	   Pooling    : Fp = Filter dim, Sp = Stride, Dp = Dilation
	   3 different configurations:
	   	Convolution then Pooling
	   	Convolution
	   	Pooling
	   Wc, Hc  : convolution output dimension if present, otherwise returns Width, Eight
	   Wo, Ho  : If conv then pool output dimension after conv and pooling, if pool only pool out dim, if conv only conv out dim
	   Pcw, Pch: Horizontal and vertical padding for convolution
	   Ppw, Pph: Horizontal and vertical padding for pooling
	*/
        int PadCw=0, PadCh=0;
        int PadPw=0, PadPh=0;

	if (ConvOper==KOP_NONE) {
		Fcx=1; Dcx=1; Scx=1; Fcy=1; Dcy=1; Scy=1;
	}
	if (PoolOper==KOP_NONE) {
		Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
	}
        if (ConvOper!=KOP_NONE && ConvPad) {
		PadCw = CNN_TotalPaddingValue(Width, Fcx, Scx, Dcx); PadCh = CNN_TotalPaddingValue(Height, Fcy, Scy, Dcy);
        }
	int ConvW = (Width  - (Dcx*(Fcx-1)+1) + PadCw)/Scx + 1;
	int ConvH = (Height - (Dcy*(Fcy-1)+1) + PadCh)/Scy + 1;

	if (Wc) *Wc = ConvW; else ConvW = Width;
	if (Hc) *Hc = ConvH; else ConvH = Height;

        if (PoolOper!=KOP_NONE && PoolPad) {
		PadPw = CNN_TotalPaddingValue(ConvW, Fpx, Spx, Dpx); PadPh = CNN_TotalPaddingValue(ConvH, Fpy, Spy, Dpy);
        }

        if (Wo) *Wo = (ConvW - (Dpx*(Fpx-1)+1) + PadPw)/Spx + 1;
	if (Ho) *Ho = (ConvH - (Dpy*(Fpy-1)+1) + PadPh)/Spy + 1;
	if (Pcw) *Pcw = PadCw;
	if (Pch) *Pch = PadCh;
	if (Ppw) *Ppw = PadPw;
	if (Pph) *Pph = PadPh;
}

void CNN_TileOverlap(Tile_Orientation_T TileOrientation,
			int Fcx, int Fcy, int Dcx, int Dcy, int Scx, int Scy,
			int Fpx, int Fpy, int Dpx, int Dpy, int Spx, int Spy,
			int *OverlapC, int *OverlapP)

{
	/* Convolution: Fc = Filter dim, Sc = Stride, Dc = Dilation
	   Pooling    : Fp = Filter dim, Sp = Stride, Dp = Dilation
	   3 different configurations:
	   	Convolution then Pooling
	   	Convolution
	   	Pooling
	*/
	if (OverlapC == 0) {
		Fcx = Scx = Dcx = 1; Fcy = Scy = Dcy = 1;
	}
	if (OverlapP == 0) {
		Fpx = Spx = Dpx = 1; Fpy = Spy = Dpy = 1;
	}
	int OverlapCx = (Dcx*(Fcx-1)+1) + Scx*((Dpx*(Fpx-1)+1)-Spx-1);
	int OverlapCy = (Dcy*(Fcy-1)+1)+ Scy*((Dpy*(Fpy-1)+1)-Spy-1);
	int OverlapPx = (Dpx*(Fpx-1)+1)-Spx;
	int OverlapPy = (Dpy*(Fpy-1)+1)-Spy;

	if (OverlapC) *OverlapC = (TileOrientation==TILE_HOR)?OverlapCy:OverlapCx;
	if (OverlapP) *OverlapP = (TileOrientation==TILE_HOR)?OverlapPy:OverlapPx;


}

int CNN_CheckIfRepresentable(int Value, int Nbits)

{
	return ((Abs(Value)&((1<<Nbits)-1)) == Value);
}

int CNN_SetUpperLowerBounds(KernelOper_T ReLUOper, int DataSize, int DoReLU, int *LB, int *UB, int ReluN, int Precision)

{
	if (DataSize==4) {
		if (DoReLU) {
			if (ReLUOper == KOP_RELU) {
				*LB = 0; *UB = 0x7fffffff;
			} else if (ReLUOper == KOP_RELUN) {
				*LB = 0; *UB = ReluN<<Precision;
				if (!CNN_CheckIfRepresentable(*UB, 31)) return 1;
			}
		} else {
			*LB = 0x80000000; *UB = 0x7fffffff;
		}
	} else if (DataSize==2) {
		if (DoReLU) {
			if (ReLUOper == KOP_RELU) {
				*LB = 0; *UB = 32767;
			} else if (ReLUOper == KOP_RELUN) {
				*LB = 0; *UB = ReluN<<Precision;
				if (!CNN_CheckIfRepresentable(*UB, 15)) return 1;
			}
		} else {
			*LB = -32768; *UB = 32767;
		}
	} else if (DataSize==1) {
		if (DoReLU) {
			if (ReLUOper == KOP_RELU) {
				*LB = 0; *UB = 127;
			} else if (ReLUOper == KOP_RELUN) {
				*LB = 0; *UB = ReluN<<Precision;
				if (!CNN_CheckIfRepresentable(*UB, 7)) return 1;
			}
		} else {
			*LB = -128; *UB = 127;
		}
	}
	return 0;
}
