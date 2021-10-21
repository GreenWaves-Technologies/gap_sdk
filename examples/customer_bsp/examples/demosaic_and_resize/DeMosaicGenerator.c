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
#include "DeMosaicGenerator.h"

#define D0	KER_ITER_D0
#define D1	KER_ITER_D1
#define D2	KER_ITER_D2
#define D3	KER_ITER_D3
#define T0	KER_ITER_TILE0
#define T1	KER_ITER_TILE1
#define T2	KER_ITER_TILE2


void LoadDeMosaicLibrary()
{
	LibKernel("KerDeMosaicGRBG", CALL_PARALLEL,
		CArgs(8,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned char * __restrict__", "Out0"),
			TCArg("unsigned char * __restrict__", "Out1"),
			TCArg("unsigned char * __restrict__", "Out2"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned int", "isTileFirst"),
			TCArg("unsigned int", "isTileLast")
			),
		"KerDeMosaic_ArgT",
		NULL
	);

	LibKernel("KerDeMosaicAndResizeGRBG", CALL_PARALLEL,
		CArgs(4,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned char * __restrict__", "Out")
			),
		"KerDeMosaicAndResize_ArgT",
		NULL
	);

	LibKernel("KerDeMosaicAndResizeGBRG", CALL_PARALLEL,
		CArgs(4,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned char * __restrict__", "Out")
			),
		"KerDeMosaicAndResize_ArgT",
		NULL
	);

	LibKernel("KerDeMosaicAndResizeBGGR", CALL_PARALLEL,
		CArgs(4,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned char * __restrict__", "Out")
			),
		"KerDeMosaicAndResize_ArgT",
		NULL
	);

	

	
}


// Types of kernels are based on bayer filter first 4 pixel block:
// 
// IE for KerDeMosaicGRBG:
// *************************
// * Green *  Red  *  ...  *
// *************************
// * Blue  * Green *  ...  *
// *************************
// *  ...  *  ...  *  ...  *


int GenerateDeMosaic_OutCHW(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt){
	
	int in_l=1,out_l=3;
	char *KerName;

	char *BodyName = AppendNames(Name, "Body");
	switch (bt){
		case GRBG:
			KerName = "KerDeMosaicGRBG";
			break;
		default:
			GenTilingError("Demosacing Order not implemented...\n");
			return -1;
	}

	if(In_L3 || Out_L3){ printf("IO in L3 not yet implemented"); return -1;}

	Object_T **PKerArgs = AllocateKerArgs(4);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN  | O_DB,  Width, Height, 1,  1, 0, 0, "In"); //Preferred tile size 2
	PKerArgs[1] = KerArg("Out0", KerArgSpace(1,T0), O_OUT | O_DB, Width, Height,  1,  0, 0, 2, "Out0");
	PKerArgs[2] = KerArg("Out1", KerArgSpace(1,T0), O_OUT | O_DB, Width, Height,  1,  0, 0, 0, "Out1");
	PKerArgs[3] = KerArg("Out2", KerArgSpace(1,T0), O_OUT | O_DB, Width, Height,  1,  0, 0, 0, "Out2");
	
	OpenKernelGroup(Name);

	UserKernel(BodyName,
			KernelIterSpace(1, IterTiledSpace(T0)),
			TILE_HOR,
			CArgs(4, TCArg(CNN_ArgDataTypeUns(1,1,1),"In"),
					 TCArg(CNN_ArgDataType(1,1,1),   "Out0"),
					 TCArg(CNN_ArgDataType(1,1,1),   "Out1"),
					 TCArg(CNN_ArgDataType(1,1,1),   "Out2")
				 ),
			Calls(1,
				Call(KerName, LOC_LOOP,
					Bindings(8,
						K_Arg("In", KER_ARG_TILE),	/* Input tile */
						K_Arg("Out0", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out1", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out2", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out0", KER_ARG_TILE_W), /* tile width */
						K_Arg("Out0", KER_ARG_TILE_H),	 /* tile height */
						K_Arg("Out0", KER_ARG_TILELAST ),
						K_Arg("Out0", KER_ARG_TILEFIRST)
						)
				    )
			     ),
			PKerArgs
		  );
	//AddKernelInfos(BodyName, AT_KERINFO_OPER, LayerOp, 0);
	//AddKernelInfos(BodyName, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
	AddKernelArgDim(BodyName, "In",   4, 1, Height, Width, 1);
	AddKernelArgDim(BodyName, "Out0", 4, 1, Height, Width, 1);
	AddKernelArgDim(BodyName, "Out1", 4, 1, Height, Width, 1);
	AddKernelArgDim(BodyName, "Out2", 4, 1, Height, Width, 1);
	CloseKernelGroup();
	CKernel_Arg_T **KCArgs = AllocateCArgs(2);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType   (1,1,1), "Out");
	Object_T **KArgs = AllocateKerArgs(7);
	int Ka=0;
	KArgs[Ka++] = KerGroupArg("In",     O_IN,  Width*Height*1,1, "In");
	KArgs[Ka++] = KerGroupArg("Out",    O_OUT, Width*Height*3,1, "Out");
	UserKernelGroup(Name,
			KCArgs,
			Calls(1,
				UserKernelCall(BodyName, LOC_GROUP,
					Bindings(4,
						C_Arg("In"),
						C_ArgPlusImmOffset("Out", 0),
						C_ArgPlusImmOffset("Out", Height * Width),
						C_ArgPlusImmOffset("Out", Height * Width * 2)

						)
					)
			     )
		       );


	return 0;
}

int GenerateDeMosaic_OutWHC(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt){
	char *KerName;

	if(Width%2!=0 || Height%2!=0){
		GenTilingError("Simple_DeMosaic_Resize needs dimension to be even...\n");
			return -1;
	}

	switch (bt){
		case GRBG:
			KerName = "KerDeMosaicGRBG";
			break;
		case GBRG:
			KerName = "KerDeMosaicGBRG";
			break;
		case BGGR:
			KerName = "KerDeMosaicBGGR";
			break;
		
		default:
			GenTilingError("Demosacing Order not implemented...\n");
			return -1;
	}

	int InL3 = In_L3?O_L2DB:0;
	int OutL3 = Out_L3?O_L2DB:0;

	UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2, 
			TCArg("unsigned char *", "In"), 
			TCArg("unsigned char *", "Out")),
			Calls(1, Call(KerName, LOC_LOOP,
				Bindings(4, K_Arg("In", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_TILE_W),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("Out", KER_ARG_TILE)
				        ))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(1,KER_ITER_TILE0) ,O_IN  | O_DB | InL3,  Width, Height, sizeof(char)  , 1, 0, 2, "In"),
			KerArg("Out", KerArgSpace(1,KER_ITER_TILE0) ,O_OUT | O_DB | OutL3, Width, Height, sizeof(char)*3, 0, 0, 0, "Out")
		)
	);

}


int Simple_DeMosaic_Resize(char *Name, unsigned int Width, unsigned int Height, char In_L3,char Out_L3,BayerOrder_t bt){

	char *KerName;

	if(Width%2!=0 || Height%2!=0){
		GenTilingError("Simple_DeMosaic_Resize needs dimension to be even...\n");
			return -1;
	}

	switch (bt){
		case GRBG:
			KerName = "KerDeMosaicAndResizeGRBG";
			break;
		case GBRG:
			KerName = "KerDeMosaicAndResizeGBRG";
			break;
		case BGGR:
			KerName = "KerDeMosaicAndResizeBGGR";
			break;
		
		default:
			GenTilingError("Demosacing Order not implemented...\n");
			return -1;
	}

	int InL3 = In_L3?O_L2DB:0;
	int OutL3 = Out_L3?O_L2DB:0;

	UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2, 
			TCArg("unsigned char *", "In"), 
			TCArg("unsigned char *", "Out")),
			Calls(1, Call(KerName, LOC_LOOP,
				Bindings(4, K_Arg("In", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_TILE_W),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("Out", KER_ARG_TILE)
				        ))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(1,KER_ITER_TILE0) ,O_IN  | O_DB | InL3,  Width, Height,     sizeof(char)  , 0, 0, 2, "In"),
			KerArg("Out", KerArgSpace(1,KER_ITER_TILE0) ,O_OUT | O_DB | OutL3, Width/2, Height/2, sizeof(char)*3, 0, 0, 0, "Out")
		)
	);
}

void DeMosaicConfiguration(unsigned int L1Memory,unsigned int L2Memory)
{
	SetInlineMode(ALWAYS_INLINE);
	SetSymbolDynamics();

	SetUsedFilesNames(0, 1, "DeMosaicBasicKernels.h");
	SetGeneratedFilesNames("DeMosaicKernels.c", "DeMosaicKernels.h");

    SetMemoryDeviceInfos(2,
        AT_MEM_L1, L1Memory, "DeMosaic_L1_Memory", 0, 0,
		AT_MEM_L2, L2Memory, "DeMosaic_L2_Memory", 0, 0
    );
}
