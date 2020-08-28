#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "AutoTilerLib.h"
#include "nntool_extra_generators.h"
#include "Gap.h"

#define D0	KER_ITER_D0
#define D1	KER_ITER_D1
#define D2	KER_ITER_D2
#define D3	KER_ITER_D3
#define T0	KER_ITER_TILE0
#define T1	KER_ITER_TILE1
#define T2	KER_ITER_TILE2

void LoadNNTools_Extra_Library()

{
	LibKernel("CNN_NormRGB565_offset_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned short *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB565_fps_T", NULL
		 );

	LibKernel("CNN_NormRGB565_shift_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned short *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB565_fps_T", NULL
		 );

	LibKernel("CNN_NormRGB888_offset_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB888_fps_T", NULL
		 );

	LibKernel("CNN_NormRGB888_shift_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB888_fps_T", NULL
		 );
	
	LibKernel("CNN_NormRGB16_fp", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed short int *__restrict__", "Out0"),
				TCArg("signed short int *__restrict__", "Out1"),
				TCArg("signed short int *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB16_fp_T", NULL
		 );

	LibKernel("CNN_NormBW_offset_fps", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fps_T", NULL
		 );

	LibKernel("CNN_NormBW_shift_fps", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fps_T", NULL
		 );

	LibKernel("CNN_NormBW_fp", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed short int *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fp_T", NULL
		 );

	LibKernel("CNN_Copy_fps", CALL_PARALLEL,
			CArgs(4,
				TCArg("signed char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out"),
				TCArg("unsigned int", "W"),
				TCArg("unsigned int", "H")
			     ),
			"KerCopy_fps_T", NULL
		 );
}

/*********************************************************************************************************************************************************************
  Generator for RGB565 image preprocessing:

Template:
Name:		Name of the generated user kernel

Width		Image width
Height		Image height

DoOffset    If true offset pixel by -128

Signature:	Name(In, Out)

Kop: NNTOOL_KOP_RGB16, NNTOOL_KOP_RGB565, NNTOOL_KOP_RGB888

 *********************************************************************************************************************************************************************/

int CNN_NormRGB(
		char *Name,
		int Width,
		int Height,
		int DoOffset,
		nntool_kop_t kop
	       )

{
	int Log = 1;
	char *BodyName = AppendNames(Name, "Body");
	unsigned long long int LayerOp = (Width*Height*4) + (Width*Height*(DoOffset?2:1))/4;
	unsigned long long int LayerBandwidth = 0;

	char *NormRGBKerName = (kop==NNTOOL_KOP_RGB16?"CNN_NormRGB16_fp":(kop==NNTOOL_KOP_RGB565?(DoOffset?"CNN_NormRGB565_offset_fps":"CNN_NormRGB565_shift_fps"):(DoOffset?"CNN_NormRGB888_offset_fps":"CNN_NormRGB888_shift_fps")));

	LayerBandwidth += 2*Width*Height*1;
	LayerBandwidth += 3*Width*Height*1;

	if (Log) {
		printf("CNN_NormRGB%d: %s\n", (kop==NNTOOL_KOP_RGB16?16:(kop==NNTOOL_KOP_RGB565?565:888)), Name);
		printf("In  => Feat: %d W: %4d, H: %4d\n", (kop==NNTOOL_KOP_RGB565?1:3), Width, Height);
		printf("Out => Feat: 3, W: %4d, H: %4d\n", Width, Height);
		if (NormRGBKerName) printf("%20s: %s\n", "KerName", NormRGBKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(4);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width*(kop==NNTOOL_KOP_RGB565?1:3), Height, (kop==NNTOOL_KOP_RGB565?2:1),  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out0", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==NNTOOL_KOP_RGB16?2:1),  0, 0, 0, "Out0");
	PKerArgs[2] = KerArg("Out1", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==NNTOOL_KOP_RGB16?2:1),  0, 0, 0, "Out1");
	PKerArgs[3] = KerArg("Out2", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==NNTOOL_KOP_RGB16?2:1),  0, 0, 0, "Out2");
	OpenKernelGroup(Name);
	UserKernel(BodyName,
			KernelIterSpace(1, IterTiledSpace(T0)),
			TILE_HOR,
			CArgs(4, TCArg(CNN_ArgDataTypeUns((kop==NNTOOL_KOP_RGB565?2:1),1,1),  "In"),
					 TCArg(CNN_ArgDataType((kop==NNTOOL_KOP_RGB16?2:1),1,1), "Out0"),
					 TCArg(CNN_ArgDataType((kop==NNTOOL_KOP_RGB16?2:1),1,1), "Out1"),
					 TCArg(CNN_ArgDataType((kop==NNTOOL_KOP_RGB16?2:1),1,1), "Out2")
				 ),
			Calls(1,
				Call(NormRGBKerName, LOC_LOOP,
					Bindings(6,
						K_Arg("In", KER_ARG_TILE),	/* Input tile */
						K_Arg("Out0", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out1", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out2", KER_ARG_TILE),	/* Output tile */
						K_Arg((kop==NNTOOL_KOP_RGB565?"In":"Out0"), KER_ARG_TILE_W),	/* tile width */
						K_Arg((kop==NNTOOL_KOP_RGB565?"In":"Out0"), KER_ARG_TILE_H)	/* tile height */
						)
				    )
			     ),
			PKerArgs
		  );
	AddKernelInfos(BodyName, AT_KERINFO_OPER, LayerOp, 0);
	AddKernelInfos(BodyName, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
	AddKernelArgDim(BodyName, "In",   4, (kop==NNTOOL_KOP_RGB565?1:3), Height, Width, (kop==NNTOOL_KOP_RGB565?2:1));
	AddKernelArgDim(BodyName, "Out0", 4, 1, Height, Width, (kop==NNTOOL_KOP_RGB16?2:1));
	AddKernelArgDim(BodyName, "Out1", 4, 1, Height, Width, (kop==NNTOOL_KOP_RGB16?2:1));
	AddKernelArgDim(BodyName, "Out2", 4, 1, Height, Width, (kop==NNTOOL_KOP_RGB16?2:1));
	CloseKernelGroup();
	CKernel_Arg_T **KCArgs = AllocateCArgs(2);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns((kop==NNTOOL_KOP_RGB565?2:1),1,1),   "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(   (kop==NNTOOL_KOP_RGB16?2:1), 1,1),  "Out");
	Object_T **KArgs = AllocateKerArgs(7);
	int Ka=0;
	KArgs[Ka++] = KerGroupArg("In",     O_IN,  Width*Height*(kop==NNTOOL_KOP_RGB565?1:3), (kop==NNTOOL_KOP_RGB565?2:1), "In");
	KArgs[Ka++] = KerGroupArg("Out",    O_OUT, Width*Height*3,                            (kop==NNTOOL_KOP_RGB16?2:1), "Out");
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

int CNN_NormBW(
		char *Name,
		int Width,
		int Height,
		int DoOffset,
		nntool_kop_t kop
       )
{
	int Log = 1;
	unsigned long long int LayerOp = Width*Height;
	unsigned long long int LayerBandwidth = 0;
	char *NormBWKerName = (kop==NNTOOL_KOP_BW16?"CNN_NormBW_fp":(DoOffset?"CNN_NormBW_offset_fps":"CNN_NormBW_shift_fps"));

	LayerBandwidth += Width*Height*1;
	LayerBandwidth += Width*Height*1;

	if (Log) {
		printf("CNN_NormBW: %s\n", Name);
		printf("In  => Feat: 1 W: %4d, H: %4d\n", Width, Height);
		printf("Out => Feat: 1, W: %4d, H: %4d\n", Width, Height);
		if (NormBWKerName) printf("%20s: %s\n", "KerName", NormBWKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(2);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width, Height, 1,  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==NNTOOL_KOP_BW16?2:1),  0, 0, 0, "Out");
        UserKernel(Name,
                        KernelIterSpace(1, IterTiledSpace(T0)),
                        TILE_HOR,
                        CArgs(2, TCArg(CNN_ArgDataTypeUns(1,1,1),  "In"), TCArg(CNN_ArgDataType((kop==NNTOOL_KOP_BW16?2:1),1,1), "Out")),
                        Calls(1,
                                Call(NormBWKerName, LOC_LOOP,
                                        Bindings(4,
                                                K_Arg("In", KER_ARG_TILE),      /* Input tile */
                                                K_Arg("Out", KER_ARG_TILE),    /* Output tile */
                                                K_Arg("In", KER_ARG_TILE_W),    /* Input tile width */
                                                K_Arg("In", KER_ARG_TILE_H)     /* Input tile width */
                                                )
                                    )
                             ),
                        PKerArgs
                  );
        AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
        AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
        AddKernelArgDim(Name, "In", 4, 1, Height, Width, 1);
        AddKernelArgDim(Name, "Out", 4, 1, Height, Width, 1);
	return 0;
}

int CNN_Norm(
		char *Name,
		int Width,
		int Height,
		int DoOffset,
		nntool_kop_t kop
       )
{
	if (kop == NNTOOL_KOP_BW || kop == NNTOOL_KOP_BW16) {
		return CNN_NormBW(Name, Width, Height, DoOffset, kop);
	} else {
		return CNN_NormRGB(Name, Width, Height, DoOffset, kop);
	}
}

// int largest_factor(int sz)
// {
// 	int limit = (int) floor(sqrt(sz));
// 	for (int i=2; i<=limit; i++)
//     {
//         int c=0;
//         for (int j=1; j<=i; j++)
//         {
//             if (i%j==0)
//             {
//                 c++;
//             }
//         }
// 	    if (c==2 && sz%i==0) return sz/i;
//     }
// 	return 1;
// }

// int CNN_Copy_fps(
// 		char *Name,
// 		int Sz)
// {
// 	int Log = 1;
// 	int Width = largest_factor(Sz);
// 	int Height = Sz / Width;
// 	unsigned long long int LayerOp = 0;
// 	unsigned long long int LayerBandwidth = 0;

// 	LayerBandwidth += Sz*2;

// 	if (Log) {
// 		printf("CNN_Copy: %s\n", Name);
// 		printf("In  => Feat: 1 W: %4d, H: %4d\n", Width, Height);
// 		printf("Out => Feat: 1, W: %4d, H: %4d\n", Width, Height);
// 		printf("Nb Oper : %lld\n", LayerOp);
// 	}

// 	Object_T **PKerArgs = AllocateKerArgs(2);
// 	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width, Height, 1,  0, 0, 0, "In");
// 	PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, 1,  0, 0, 0, "Out");
// 	UserKernel(Name,
// 					KernelIterSpace(1, IterTiledSpace(T0)),
// 					TILE_HOR,
// 					CArgs(2, TCArg(CNN_ArgDataType(1,1,1),  "In"), TCArg(CNN_ArgDataType(1,1,1), "Out")),
// 					Calls(1,
// 							Call(NormBWKerName, LOC_LOOP,
// 									Bindings(4,
// 											K_Arg("In", KER_ARG_TILE),      /* Input tile */
// 											K_Arg("Out", KER_ARG_TILE),    	/* Output tile */
// 											K_Arg("In", KER_ARG_TILE_W),    /* Input tile width */
// 											K_Arg("In", KER_ARG_TILE_H)     /* Input tile height */
// 											)
// 								)
// 							),
// 					PKerArgs
// 				);
// 	AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
// 	AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
// 	AddKernelArgDim(Name, "In", 4, 1, Height, Width, 1);
// 	AddKernelArgDim(Name, "Out", 4, 1, Height, Width, 1);
// 	return 0;
// }

