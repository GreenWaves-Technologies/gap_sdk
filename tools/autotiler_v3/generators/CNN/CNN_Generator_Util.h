#ifndef __CNN_GENERATOR_UTIL_H__
#define __CNN_GENERATOR_UTIL_H__

#include "AutoTilerLib.h"
#include "Gap.h"

#define MaxS(a, b) (((int)(a)>(int)(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
#define Abs(x) (((x)<0)?-(x):(x))

#define D0	KER_ITER_D0
#define D1	KER_ITER_D1
#define D2	KER_ITER_D2
#define D3	KER_ITER_D3
#define T0	KER_ITER_TILE0
#define T1	KER_ITER_TILE1
#define T2	KER_ITER_TILE2

#define MAXDPPREC

#ifdef MAXDPPREC
#define DP_fps_S 4
#else
#define DP_fps_S 2
#endif

int CNN_EncodePoolOperation(KernelOper_T PoolOper, KernelOper_T ReLUOper);
int CNN_EncodeActivation(KernelOper_T Oper);
int CNN_Gcd(int a, int b);
int CNN_Scm(int a, int b);
int CNN_UsedInputDimension(int Dim, int F, int S, int D, int Pad);
int CNN_TotalPaddingValue(int Dim, int F, int S, int D);
v4s CNN_EdgePaddingValue(AT_PadType PadType, int Padw, int Padh);
void CNN_LayerOutputDim(int Width, int Height,
			KernelOper_T ConvOper, int Fcx, int Fcy, int Dcx, int Dcy, int Scx, int Scy, int ConvPad,
			KernelOper_T PoolOper, int Fpx, int Fpy, int Dpx, int Dpy, int Spx, int Spy, int PoolPad,
			int *Wc, int *Hc, int *Wo, int *Ho,
			int *Pcw, int *Pch, int *Ppw, int *Pph);
void CNN_TileOverlap(Tile_Orientation_T TileOrientation,
			int Fcx, int Fcy, int Dcx, int Dcy, int Scx, int Scy,
			int Fpx, int Fpy, int Dpx, int Dpy, int Spx, int Spy,
			int *OverlapC, int *OverlapP);
int CNN_CheckIfRepresentable(int Value, int Nbits);
int CNN_SetUpperLowerBounds(KernelOper_T ReLUOper, int DataSize, int DoReLU, int *LB, int *UB, int ReluN, int Precision);
#endif
