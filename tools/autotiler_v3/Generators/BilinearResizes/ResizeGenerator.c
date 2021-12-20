/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "ResizeGenerator.h"

void LoadResizeLibrary()
{
	LibKernel("KerResizeBilinear", CALL_PARALLEL,
		CArgs(8,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("unsigned char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeBilinear_ArgT",
		NULL
	);
	LibKernel("KerResizeNearestNeighbor", CALL_PARALLEL,
		CArgs(8,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("unsigned char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeNearestNeighbor_ArgT",
		NULL
	);
	LibKernel("KerResizeBilinearSigned", CALL_PARALLEL,
		CArgs(8,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeBilinearSigned_ArgT",
		NULL
	);
	LibKernel("KerResizeNearestNeighborSigned", CALL_PARALLEL,
		CArgs(8,
			TCArg("signed char * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("signed char * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeNearestNeighborSigned_ArgT",
		NULL
	);
	LibKernel("KerResizeBilinearSigned_Q16", CALL_PARALLEL,
		CArgs(8,
			TCArg("signed short * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("signed short * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeSigned16_ArgT",
		NULL
	);
	LibKernel("KerResizeNearestNeighborSigned_Q16", CALL_PARALLEL,
		CArgs(8,
			TCArg("signed short * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("signed short * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResizeSigned16_ArgT",
		NULL
	);
	LibKernel("KerResizeNearestNeighbor_fp16", CALL_PARALLEL,
		CArgs(8,
			TCArg("F16 * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResize_fp16_ArgT",
		NULL
	);
	LibKernel("KerResizeBilinear_fp16", CALL_PARALLEL,
		CArgs(8,
			TCArg("F16 * __restrict__", "In"),
			TCArg("unsigned int", "Win"),
			TCArg("unsigned int", "Hin"),
			TCArg("F16 * __restrict__", "Out"),
			TCArg("unsigned int", "Wout"),
			TCArg("unsigned int", "Hout"),
			TCArg("unsigned int", "HTileOut"),
			TCArg("unsigned int", "FirstLineIndex")),
		"KerResize_fp16_ArgT",
		NULL
	);
}

int GenerateResizeMultiChannel(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout, unsigned int Channels, InOut_t InOut_Type, resize_kop_t Type)

{
	char *ResizeKerName;
	switch (Type){
		case KOP_BILINEAR_RESIZE:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeBilinearSigned":"KerResizeBilinear";
			break;
		case KOP_NEAREST_NEIGHBOR_RESIZE:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeNearestNeighborSigned":"KerResizeNearestNeighbor";
			break;
		default:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeBilinearSigned":"KerResizeBilinear";
	}
	printf("%s\n", ResizeKerName);
	int LayerOp = Channels * Wout * Hout * (3 + 6 + 3);
	int LayerBandwidth = Channels * Win * Hin + Channels * Hout * Wout;
	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, Channels), IterTiledSpace(KER_ITER_TILE0)),
		(Hin==1)?TILE_VER:TILE_HOR,
		(InOut_Type==SIGNED_INOUT)?CArgs(2, TCArg("signed char *", "In"), TCArg("signed char *", "Out")):
								   CArgs(2, TCArg("unsigned char *", "In"), TCArg("unsigned char *", "Out")),
		Calls(1, Call(ResizeKerName, LOC_LOOP,
			Bindings(8, K_Arg("In", KER_ARG_TILE),
				        K_Arg("In", KER_ARG_W),
				        K_Arg("In", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_W),
				        K_Arg("Out", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("In", KER_ARG_TILE_BASE)))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_IN_DB,  Win,  Hin,  sizeof(char), 1, OBJ_CONSTRAINTS_DYNAMIC, 0, "In"),
			KerArg("Out", KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_OUT_DB, Wout, Hout, sizeof(char), 0, OBJ_CONSTRAINTS_DYNAMIC, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Channels, Hin, Win, 1);
		AddKernelArgDim(Name, "Out", 4, Channels, Hout, Wout, 1);
	}
	return (Kernel!=0);
}

int GenerateResizeMultiChannelQ16(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout, unsigned int Channels, InOut_t InOut_Type, resize_kop_t Type)

{
	char *ResizeKerName;
	switch (Type){
		case KOP_BILINEAR_RESIZE:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeBilinearSigned_Q16":0;
			break;
		case KOP_NEAREST_NEIGHBOR_RESIZE:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeNearestNeighborSigned_Q16":0;
			break;
		default:
			ResizeKerName = (InOut_Type==SIGNED_INOUT)?"KerResizeBilinearSigned_Q16":0;
	}
	printf("%s\n", ResizeKerName);
	int LayerOp = Channels * Wout * Hout * (3 + 6 + 3);
	int LayerBandwidth = Channels * Win * Hin + Channels * Hout * Wout;
	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, Channels), IterTiledSpace(KER_ITER_TILE0)),
		(Hin==1)?TILE_VER:TILE_HOR,
		CArgs(2, TCArg("signed short *", "In"), TCArg("signed short *", "Out")),
		Calls(1, Call(ResizeKerName, LOC_LOOP,
			Bindings(8, K_Arg("In", KER_ARG_TILE),
				        K_Arg("In", KER_ARG_W),
				        K_Arg("In", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_W),
				        K_Arg("Out", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("In", KER_ARG_TILE_BASE)))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_IN_DB,  Win,  Hin,  sizeof(short), 1, OBJ_CONSTRAINTS_DYNAMIC, 0, "In"),
			KerArg("Out", KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_OUT_DB, Wout, Hout, sizeof(short), 0, OBJ_CONSTRAINTS_DYNAMIC, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "In", 4, Channels, Hin, Win, 2);
		AddKernelArgDim(Name, "Out", 4, Channels, Hout, Wout, 2);
	}
	return (Kernel!=0);
}


int GenerateResizeMultiChannel_fp16(char *Name, unsigned int Win, unsigned int Hin, unsigned int Wout, unsigned int Hout, unsigned int Channels, InOut_t InOut_Type, resize_kop_t Type)

{
	char *ResizeKerName;
	switch (Type){
		case KOP_BILINEAR_RESIZE:
			ResizeKerName = "KerResizeBilinear_fp16";
			break;
		case KOP_NEAREST_NEIGHBOR_RESIZE:
			ResizeKerName = "KerResizeNearestNeighbor_fp16";
			break;
		default:
			ResizeKerName = "KerResizeBilinear_fp16";
	}
	printf("%s\n", ResizeKerName);
	int LayerOp = Channels * Wout * Hout * (3 + 6 + 3);
	int LayerBandwidth = Channels * Win * Hin + Channels * Hout * Wout;
	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, IterFixedSpace(KER_ITER_D0, Channels), IterTiledSpace(KER_ITER_TILE0)),
		(Hin==1)?TILE_VER:TILE_HOR,
		CArgs(2, TCArg("F16 *", "In"), TCArg("F16 *", "Out")),
		Calls(1, Call(ResizeKerName, LOC_LOOP,
			Bindings(8, K_Arg("In", KER_ARG_TILE),
				        K_Arg("In", KER_ARG_W),
				        K_Arg("In", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_W),
				        K_Arg("Out", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("In", KER_ARG_TILE_BASE)))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_IN_DB,  Win,  Hin,  sizeof(short), 1, OBJ_CONSTRAINTS_DYNAMIC, 0, "In"),
			KerArg("Out", KerArgSpace(2,KER_ITER_D0,KER_ITER_TILE0), OBJ_OUT_DB, Wout, Hout, sizeof(short), 0, OBJ_CONSTRAINTS_DYNAMIC, 0, "Out")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelFloatArgDim(Name, "In", 4, Channels, Hin, Win, 2);
		AddKernelFloatArgDim(Name, "Out", 4, Channels, Hout, Wout, 2);
	}
	return (Kernel!=0);
}

void ResizeConfiguration(unsigned int L1Memory)

{
	SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
	SetSymbolNames("Resize_L1_Memory", "Resize_L2_Memory");
	SetSymbolDynamics();
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

	SetUsedFilesNames(0, 1, "ResizeBasicKernels.h");
	SetGeneratedFilesNames("ResizeKernels.c", "ResizeKernels.h");

	SetL1MemorySize(L1Memory);
}
