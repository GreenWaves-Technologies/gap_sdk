/*
 * Copyright 2019 GreenWaves Technologies, SAS
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


#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "StdTypes.h"

#define NO_UPD	0
#define UPD	1
#define UPD1	1
#define UPD2	2
#define UPD3	3

static void CNN_LoadSWPlainConvolutionLibrary()

{
/* Book keeping */
	LibKernel("KerSetInBias", CALL_PARALLEL,
		   CArgs(4,
			 TCArg("int * __restrict__", "Out"),
			 TCArg("int", "W"),
			 TCArg("int", "H"),
			 TCArg("Word16", "Bias")
			),
		   "KerSetInBiasT"
	);
	LibKernel("KerSetInBias_int", CALL_PARALLEL,
		   CArgs(4,
			 TCArg("Word16 * __restrict__", "Out"),
			 TCArg("int", "W"),
			 TCArg("int", "H"),
			 TCArg("int", "Bias")
			),
		   "KerSetInBiasT"
	);

	/* Plain SW convolutions, short int input, short int filters, short int output */
	LibKernel("KerConv3x3_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerConv3x3Stride2_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerDirectConv3x3_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerConv5x5_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerConv5x5Stride2_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerDirectConv5x5_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerConvNxN_fp", CALL_PARALLEL,
		  CArgs(7,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerDirectConvNxM_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "M")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerDirectConvNxM_char", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word8 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "M")
		       ),
		   "KerConv_fpsT"
		);
	LibKernel("KerDirectConvNxM_tiled", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "M")
		       ),
		   "KerConv_fpsT"
		);
	LibKernel("KerDirectConvNxM_fp_tiled", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "M")
		       ),
		   "KerConv_fpsT"
		);
	LibKernel("KerDirectConvNxM_tiled_2", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "M")
		       ),
		   "KerConv_fpsT"
		);
	LibKernel("KerConvNxNStride2_fp", CALL_PARALLEL,
		  CArgs(7,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerConvNxNStrideM_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N"),
			TCArg("int", "Stride")
		       ),
		   "KerConv_fpT"
		);
	LibKernel("KerDirectConvNxN_fp", CALL_PARALLEL,
		  CArgs(7,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("Word16 * __restrict__", "Out"),
			TCArg("unsigned int", "Norm"),
			TCArg("int", "N")
		       ),
		   "KerConv_fpT"
		);
}

static void CNN_LoadSWReLUPoolingLibrary()

{
/* ReLU, Pooling */
	LibKernel("KerReLU_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerMaxPool2x2_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerMaxPool3x3_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerAvgPool2x2_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerAvgPool3x3_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerReLUMaxPool2x2_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerReLUMaxPool3x3_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerReLUAvgPool2x2_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerReLUAvgPool3x3_fp", CALL_PARALLEL,
		  CArgs(4,
			TCArg("short int * __restrict__", "In"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out")
		  ),
		  "KerReLUMaxPool2x2_fpT"
	);
	LibKernel("KerReLUMaxPool2x2_2_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1")
		  ),
		  "KerReLUMaxPool2x2_2_fpT"
	);
	LibKernel("KerReLUAvgPool2x2_2_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1")
		  ),
		  "KerReLUMaxPool2x2_2_fpT"
	);
	LibKernel("KerReLUMaxPool2x2_2_Even_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1")
		  ),
		  "KerReLUMaxPool2x2_2_fpT"
	);
	LibKernel("KerReLUMaxPool2x2_2_Drop_fp", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1")
		  ),
		  "KerReLUMaxPool2x2_2_fpT"
	);
	LibKernel("KerReLUMaxPool2x2_3_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1"),
			TCArg("short int * __restrict__", "Out2")
		  ),
		  "KerReLUMaxPool2x2_3_fpT"
	);
	LibKernel("KerReLUAvgPool2x2_3_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("short int * __restrict__", "In0"),
			TCArg("short int * __restrict__", "In1"),
			TCArg("short int * __restrict__", "In2"),
			TCArg("int", "W"),
			TCArg("int", "H"),
			TCArg("short int * __restrict__", "Out0"),
			TCArg("short int * __restrict__", "Out1"),
			TCArg("short int * __restrict__", "Out2")
		  ),
		  "KerReLUMaxPool2x2_3_fpT"
	);
}

static void CNN_LoadDenseLayerLibrary()

{
/* Linear Layer */
	LibKernel("KerLinearLayer_fps", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word8 * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word16 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpsT"
	);
	LibKernel("KerLinearLayer_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word16 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpT"
	);
	LibKernel("KerLinearLayer_fpd", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word32 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpdT"
	);
	LibKernel("KerLinearLayer_halfchar", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word8 * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word16 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpsT"
	);
	LibKernel("KerLinearLayerReLU_fps", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word8  * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word16 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpsT"
	);
	LibKernel("KerLinearLayerReLU_fp", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word16  * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word16 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpT"
	);
	LibKernel("KerLinearLayerReLU_fpd", CALL_PARALLEL,
		  CArgs(8,
			TCArg("Word16 * __restrict__", "In"),
			TCArg("int", "InSize"),
			TCArg("Word16 * __restrict__", "Filter"),
			TCArg("unsigned int", "NormFilter"),
			TCArg("Word16 *  __restrict__", "Bias"),
			TCArg("unsigned int", "NormBias"),
			TCArg("Word32 *  __restrict__", "Out"),
			TCArg("int", "OutSize")
		  ),
		  "KerLinearLayer_fpdT"
	);
}

void CNN_LoadSoftwareKernelLibrary()

{
	CNN_LoadSWPlainConvolutionLibrary();
	CNN_LoadSWReLUPoolingLibrary();
	CNN_LoadDenseLayerLibrary();
}

static void CNN_LoadHWCEBookKeepingLibrary()

{
        LibKernel("HWCE_Enable",  CALL_SEQUENTIAL, CArgs(0), "");
        LibKernel("HWCE_Disable", CALL_SEQUENTIAL, CArgs(0), "");
        LibKernel("HWCE_GenericInit", CALL_SEQUENTIAL,
                   CArgs(3,
                        TCArg("unsigned int", "ConvType"),
                        TCArg("unsigned int", "WStride"),
                        TCArg("unsigned int", "Norm")
                   ),
                   ""
        );
        LibKernel("HwCE_SetYinMode", CALL_SEQUENTIAL,
                CArgs(1,
                        TCArg("unsigned int", "Disable")
                ),
                ""
        );
}

static void CNN_LoadHWCEConvolutionLibrary()

{
        LibKernel("KerSetInBias2", CALL_PARALLEL,
                   CArgs(6,
                         TCArg("Word16 * __restrict__", "Out0"),
                         TCArg("Word16 * __restrict__", "Out1"),
                         TCArg("int", "W"),
                         TCArg("int", "H"),
                         TCArg("Word16", "Bias0"),
                         TCArg("Word16", "Bias1")
                        ),
                   "KerSetInBias2T"
        );
        LibKernel("KerSetInBias3", CALL_PARALLEL,
                   CArgs(8,
                         TCArg("Word16 * __restrict__", "Out0"),
                         TCArg("Word16 * __restrict__", "Out1"),
                         TCArg("Word16 * __restrict__", "Out2"),
                         TCArg("int", "W"),
                         TCArg("int", "H"),
                         TCArg("Word16", "Bias0"),
                         TCArg("Word16", "Bias1"),
                         TCArg("Word16", "Bias2")
                        ),
                   "KerSetInBias3T"
        );
        LibKernel("HWCE_ProcessOneTile3x3_MultiOut", CALL_SEQUENTIAL,
                  CArgs(9,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out0"),
                        TCArg("Word16 * __restrict__", "Out1"),
                        TCArg("Word16 * __restrict__", "Out2"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H"),
                        TCArg("unsigned int", "OutMask")
                       ),
                   ""
                );
        LibKernel("HWCE_ProcessOneTile5x5", CALL_SEQUENTIAL,
                  CArgs(6,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H")
                       ),
                   ""
                );
        LibKernel("HWCE_ProcessOneTile7x7", CALL_SEQUENTIAL,
                  CArgs(6,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H")
                       ),
                   ""
                );
        LibKernel("HWCE_ProcessOneTile7x4", CALL_SEQUENTIAL,
                  CArgs(6,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H")
                       ),
                   ""
                );
	LibKernel("HWCE_ProcessOneTile8x20", CALL_SEQUENTIAL,
                  CArgs(6,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H")
                       ),
                   ""
                );
	LibKernel("HWCE_ProcessOneTile4x10", CALL_SEQUENTIAL,
                  CArgs(6,
                        TCArg("Word16 * __restrict__", "In"),
                        TCArg("Word16 * __restrict__", "Out"),
                        TCArg("Word16 * __restrict__", "Filter"),
                        TCArg("Word16", "Bias"),
                        TCArg("unsigned int", "W"),
                        TCArg("unsigned int", "H")
                       ),
                   ""
                );

}


void CNN_LoadHWCEKernelLibrary()

{
	CNN_LoadHWCEBookKeepingLibrary();
	CNN_LoadHWCEConvolutionLibrary();
}


/* Pure SW convolutions (Without Accumulation with the output plane) */
void CNN_TiledPureConvNxN_SW_fp(char *Name, unsigned int FS, unsigned int Width, unsigned int Height)

{
	char *ConvKerName;

	switch (FS) {
		case 3:
			ConvKerName = "KerDirectConv3x3_fp"; break;
		case 5:
			ConvKerName = "KerDirectConv5x5_fp"; break;
		default:
			if ((FS&1 == 0) || (FS == 0)) GenTilingError("TiledPureConvNxN_SW_fp: Convolution dimension has to be > 0 and odd");
			ConvKerName = "KerDirectConvNxN_fp"; break;
	}
        UserKernel(Name,
                KernelDimensions(1, Width, Height, 1),
                KernelIterationOrder(KER_DIM2, KER_TILE),
                TILE_HOR,
                CArgs(5,
                      TCArg("short int * __restrict__", "In"),
                      TCArg("short int * __restrict__", "Filter"),
                      TCArg("short int * __restrict__", "Out"),
                      TCArg("unsigned int",             "Norm"),
                      TCArg("int",                      "N")
                     ),
                Calls(1,
                        Call(ConvKerName, LOC_INNER_LOOP,
                                Bindings(7,
                                         Bind("In",     BIND_K_ARG, "In",               KER_ARG_TILE),
                                         Bind("W",      BIND_K_ARG, "In",               KER_ARG_TILE_W),
                                         Bind("H",      BIND_K_ARG, "In",               KER_ARG_TILE_H),
                                         Bind("Filter", BIND_K_ARG, "Filter",           KER_ARG_TILE),
                                         Bind("Out",    BIND_K_ARG, "Out",              KER_ARG_TILE),
                                         Bind("Norm",   BIND_C_ARG, "Norm",             TC_ARG),
                                         Bind("N",      BIND_C_ARG, "N",                TC_ARG)
                                        )
                            )
                     ),
                KerArgs(3,
                        KerArg("In",     OBJ_IN_DB,             Width,        Height,        sizeof(short int), FS-1, 0, 0, "In", 0),
                        KerArg("Filter", OBJ_BUFFER_IN_NTILED,  FS,           FS,            sizeof(short int), 0,    0, 0, "Filter", 0),
                        KerArg("Out",    OBJ_OUT_DB,            (Width-FS+1), (Height-FS+1), sizeof(short int), 0,    0, 0, "Out", 0)
                        )
        );
}

/* SW convolution layer (Out is init to bias and then accumulated) */
void CNN_TiledConvNxN_SW_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height)

{
	char *ConvKerName;
	int NeedConvDim=0;

	switch (FS) {
		case 3:
			ConvKerName = "KerConv3x3_fp"; break;
		case 5:
			ConvKerName = "KerConv5x5_fp"; break;
		default:
			if ((FS&1 == 0) || (FS == 0)) GenTilingError("TiledConvNxN_SW_fp: Convolution dimension has to be > 0 and odd");
			NeedConvDim=1;
			ConvKerName = "KerConvNxN_fp"; break;
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(2,
			Call("KerSetInBias", LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("Out", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE_W),
					    K_Arg("Out", KER_ARG_TILE_H), C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				(NeedConvDim)?
					Bindings(7, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
						    K_Arg("Filter", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), C_Arg("Norm"), Imm(FS))
				:
					Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
						    K_Arg("Filter", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), C_Arg("Norm"))
			)
		     ),
		KerArgs(3,
			KerArg("In", 	 OBJ_IN_DB_3D,     	Width,      Height,      sizeof(short int), FS-1, 0, 0, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D,	FS,	    FS, 	 sizeof(short int), 0,    0, 0, "Filter", 0), 
			KerArg("Out", 	 OBJ_OUT_DB_3D,		Width-FS+1, Height-FS+1, sizeof(short int), 0,    0, 0, "Out", 0)
			)
	);
}


/* SW convolution layer (Out is init to bias and then accumulated), output Stride can be defined */
void CNN_TiledConvNxNStrided_SW_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane,
				   unsigned int Width, unsigned int Height, unsigned int Stride)

{
	char *ConvKerName;
	int Wo = (Width-FS)/Stride + 1;
	int Ho = (Height-FS)/Stride + 1;
	int NeedConvDim=0, NeedStride=0;

	switch (FS) {
		case 3:
			if (Stride==1) {
				ConvKerName = "KerConv3x3_fp";
			} else if (Stride==2) {
				ConvKerName = "KerConv3x3Stride2_fp";
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedStride=1; NeedConvDim=1;
			}
			break;
		case 5:
			if (Stride==1) {
				ConvKerName = "KerConv5x5_fp";
			} else if (Stride==2) {
				ConvKerName = "KerConv5x5Stride2_fp";
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedStride=1; NeedConvDim=1;
			}
			break;
		default:
			if ((FS&1 == 0) || (FS == 0)) GenTilingError("TiledConvNxNStrided_SW_fp: Convolution dimension has to be > 0 and odd");
			if (Stride==1) {
				ConvKerName = "KerConvNxN_fp"; NeedConvDim=1;
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedStride=1; NeedConvDim=1;
			}
			break;
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(2,
			Call("KerSetInBias", LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("Out", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE_W),
					    K_Arg("Out", KER_ARG_TILE_H), C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				(NeedConvDim&&NeedStride)?
					Bindings(8, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), C_Arg("Norm"), Imm(FS), Imm(Stride))
				:((NeedConvDim)?
					Bindings(7, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), C_Arg("Norm"), Imm(FS))
					:
					Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), C_Arg("Norm"))
				 )
			)
		     ),
		KerArgs(3,
			KerArg("In", 	 OBJ_IN_DB_3D,     	Width,  Height,	sizeof(short int), FS-1, 0, Stride, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D,	FS,	FS, 	sizeof(short int), 0,    0, 0,      "Filter", 0), 
			KerArg("Out", 	 OBJ_OUT_DB_3D,		Wo, 	Ho, 	sizeof(short int), 0,    0, 0,      "Out", 0)
			)
	);
}


/* Software Composite kernels: Convolution, ReLU then Max or Avg Pooling */
void CNN_TiledConvNxMReLUPool2x2_SW_fp(char *Name, unsigned int N, unsigned int M, unsigned int InPlane, unsigned int OutPlane,
				   unsigned int Width, unsigned int Height, unsigned int PoolMax)

{
  char *ConvKerName, *KerReLUPoolName,*KerSetInBiasName;

	int WW = (Width-N+1)/2;
	int HH = (Height-M+1)/2;
	ConvKerName = "KerDirectConvNxM_fp";
	KerSetInBiasName = "KerSetInBias";
	
	switch (PoolMax) {
		case 0:	/* Relu, PoolMax */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; break;
		case 1:	/* Relu, PoolAvg */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; break;
		case 2:	/* No Relu, PoolMax */
			KerReLUPoolName = "KerAvgPool2x2_fp"; break;
		case 3:	/* No Relu, PoolAvg */
			KerReLUPoolName = "KerMaxPool2x2_fp"; break;
		case 4:	/*  Relu, no pool */
			KerReLUPoolName = "KerReLU_fp";
			WW = (Width-N+1);
			HH = (Height-M+1);
			break;
	        case 5:    KerReLUPoolName = "KerMaxPool2x2_fp";
		  ConvKerName = "KerDirectConvNxM_fp_tiled";
		  KerSetInBiasName = "KerSetInBias_int";

		  break;

	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(7,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias"),
		      TCArg("int ",	"N"),
		      TCArg("int ",	"M")
		     ),
		Calls(3,
			Call("KerSetInBias", LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE_W),
					 K_Arg("BiasOut", KER_ARG_TILE_H),
					 C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(8,
					 K_Arg("In", KER_ARG_TILE),
					 K_Arg("In", KER_ARG_TILE_W),
					 K_Arg("In", KER_ARG_TILE_H),
					 K_Arg("Filter", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE),
					 C_Arg("Norm"),
					 C_Arg("N"),
					 C_Arg("M"))
			     ),
			Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE_W),
					 K_Arg("BiasOut", KER_ARG_TILE_H),
					 K_Arg("Out", KER_ARG_TILE)))
		     ),
		KerArgs(4,
			KerArg("In",      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), N-1,OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter",  OBJ_IN_DB_NTILED_4D, 	N,	    	M, 		 sizeof(short int), 0, 	0, 0, "Filter", 0),
			KerArg("BiasOut", OBJ_BUFFER_ONETILE,	Width-N+1,	Height-M+1,	 sizeof(short int), 0, 	0, 0, "", 0),
			KerArg("Out",     OBJ_OUT_DB_3D,   	WW,             HH,              sizeof(short int), 0, 	0, 0, "Out", 0)
			)
	);
}

/* Software Composite kernels: Convolution, ReLU then Max or Avg Pooling */
void CNN_TiledConvNxMReLUPool2x2_SW_char(char *Name, unsigned int N, unsigned int M, unsigned int InPlane, unsigned int OutPlane,
				   unsigned int Width, unsigned int Height, unsigned int PoolMax)

{
  char *ConvKerName, *KerReLUPoolName, *KerSetInBiasName;

	int WW = (Width-N+1)/2;
	int HH = (Height-M+1)/2;
	ConvKerName = "KerDirectConvNxM_char";
	KerSetInBiasName = "KerSetInBias";	
	
	switch (PoolMax) {
		case 0:	/* Relu, PoolMax */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; break;
		case 1:	/* Relu, PoolAvg */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; break;
		case 2:	/* No Relu, PoolMax */
			KerReLUPoolName = "KerAvgPool2x2_fp"; break;
		case 3:	/* No Relu, PoolAvg */
			KerReLUPoolName = "KerMaxPool2x2_fp"; break;
		case 4:	/*  Relu, no pool tiled version (for kws)*/
		  ConvKerName = "KerDirectConvNxM_tiled_2";
		  KerReLUPoolName = "KerReLU_fp";
		  KerSetInBiasName = "KerSetInBias_int";	
			WW = (Width-N+1);
			HH = (Height-M+1);
			break;
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(7,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("Word8 * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias"),
		      TCArg("int ",	"N"),
		      TCArg("int ",	"M")
		     ),
		Calls(3,
			Call(KerSetInBiasName, LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE_W),
					 K_Arg("BiasOut", KER_ARG_TILE_H),
					 C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(8,
					 K_Arg("In", KER_ARG_TILE),
					 K_Arg("In", KER_ARG_TILE_W),
					 K_Arg("In", KER_ARG_TILE_H),
					 K_Arg("Filter", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE),
					 C_Arg("Norm"),
					 C_Arg("N"),
					 C_Arg("M"))
			     ),
			Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE),
					 K_Arg("BiasOut", KER_ARG_TILE_W),
					 K_Arg("BiasOut", KER_ARG_TILE_H),
					 K_Arg("Out", KER_ARG_TILE)))
		     ),
		KerArgs(4,
			KerArg("In",      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), N-1,OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter",  OBJ_IN_DB_NTILED_4D, 	N,	    	M, 		 sizeof(char), 0, 	0, 0, "Filter", 0),
			KerArg("BiasOut", OBJ_BUFFER_ONETILE,	Width-N+1,	Height-M+1,	 sizeof(short int), 0, 	0, 0, "", 0),
			KerArg("Out",     OBJ_OUT_DB_3D,   	WW,             HH,              sizeof(short int), 0, 	0, 0, "Out", 0)
			)
	);
}


/* Software Composite kernels: Convolution, ReLU then Max or Avg Pooling */
void CNN_TiledConvNxNReLUPool2x2_SW_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane,
				       unsigned int Width, unsigned int Height, unsigned int PoolMax)

{
	char *ConvKerName, *KerReLUPoolName;
	int Cons = 1;
	int NeedConvDim = 0;

	switch (FS) {
		case 3:
			ConvKerName = "KerConv3x3_fp"; break;
		case 5:
			ConvKerName = "KerConv5x5_fp"; break;
		default:
			if ((FS&1 == 0) || (FS == 0)) GenTilingError("TiledConvNxNReLUMaxPool2x2_SW_fp: Convolution dimension has to be > 0 and odd");
			ConvKerName = "KerConvNxN_fp"; NeedConvDim = 1; break;
	}
	switch (PoolMax) {
		case 0:	/* Relu, PoolMax 2x2 */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; Cons = 2; break;
		case 1:	/* Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; Cons = 2; break;
		case 2:	/* No Relu, PoolMax 2x2 */
			KerReLUPoolName = "KerAvgPool2x2_fp"; Cons = 2; break;
		case 3:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool2x2_fp"; Cons = 2; break;

		case 4:	/* Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerReLUAvgPool3x3_fp"; Cons = 3; break;
		case 5:	/* Relu, PoolAvg 3x3 */
			KerReLUPoolName = "KerReLUMaxPool3x3_fp"; Cons = 3; break;
		case 6:	/* No Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerAvgPool3x3_fp"; Cons = 3; break;
		case 7:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool3x3_fp"; Cons = 3; break;

		default: GenTilingError("TiledConvNxNReLUMaxPool2x2_SW_fp: incorrect PoolMax value: %d", PoolMax);
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(3,
			Call("KerSetInBias", LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE_W),
					    K_Arg("BiasOut", KER_ARG_TILE_H), C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				(NeedConvDim)?
					Bindings(7, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE), C_Arg("Norm"), Imm(FS))
				:
					Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE), C_Arg("Norm"))),
			Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE_W), K_Arg("BiasOut", KER_ARG_TILE_H),
					    K_Arg("Out", KER_ARG_TILE)))
		     ),
		KerArgs(4,
			KerArg("In",      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), FS-1,0, Cons, "In", 0),
			KerArg("Filter",  OBJ_IN_DB_NTILED_4D, 	FS,	    	FS, 		 sizeof(short int), 0, 	 0, 0,    "Filter", 0), 
			KerArg("BiasOut", OBJ_BUFFER_ONETILE,	Width-FS+1,	Height-FS+1,	 sizeof(short int), 0, 	 0, 0,    "", 0),
			KerArg("Out",     OBJ_OUT_DB_3D,   	(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0, 	 0, 0,    "Out", 0)
			)
	);
}



/* Software Composite kernels: Convolution, ReLU then Max or Avg Pooling */
void CNN_TiledConvNxNReLUPoolStrided_SW_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane,
				           unsigned int Width, unsigned int Height, unsigned int PoolMax, unsigned int Stride)

{
	char *ConvKerName, *KerReLUPoolName;
	int NeedConvDim=0,NeedStride=0;
	int WCOut = (Width-FS)/Stride + 1;
	int HCOut = (Height-FS)/Stride + 1;

	switch (FS) {
		case 3:
			if (Stride==1) {
				ConvKerName = "KerConv3x3_fp";
			} else if (Stride==2) {
				ConvKerName = "KerConv3x3Stride2_fp";
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedConvDim=1; NeedStride=1;
			}
			break;
		case 5:
			if (Stride==1) {
				ConvKerName = "KerConv5x5_fp";
			} else if (Stride==2) {
				ConvKerName = "KerConv5x5Stride2_fp";
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedConvDim=1; NeedStride=1;
			}
			break;
		default:
			if ((FS&1 == 0) || (FS == 0)) GenTilingError("TiledConvNxNReLUMaxPool2x2_SW_fp: Convolution dimension has to be > 0 and odd");
			if (Stride==1) {
				ConvKerName = "KerConvNxN_fp"; NeedConvDim=1;
			} else {
				ConvKerName = "KerConvNxNStrideM_fp"; NeedConvDim=1; NeedStride=1;
			}
			NeedConvDim = 1; break;
	}
	switch (PoolMax) {
		case 0:	/* Relu, PoolMax 2x2 */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; break;
		case 1:	/* Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; break;
		case 2:	/* No Relu, PoolMax 2x2 */
			KerReLUPoolName = "KerAvgPool2x2_fp"; break;
		case 3:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool2x2_fp"; break;

		case 4:	/* Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerReLUAvgPool3x3_fp"; break;
		case 5:	/* Relu, PoolAvg 3x3 */
			KerReLUPoolName = "KerReLUMaxPool3x3_fp"; break;
		case 6:	/* No Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerAvgPool3x3_fp"; break;
		case 7:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool3x3_fp"; break;

		default: GenTilingError("TiledConvNxNReLUMaxPool2x2_SW_fp: incorrect PoolMax value: %d", PoolMax);
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_HOR,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(3,
			Call("KerSetInBias", LOC_IN_PLANE_PROLOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE_W),
					    K_Arg("BiasOut", KER_ARG_TILE_H), C_ArgIndex("Bias", KER_OUT_PLANE, 1))),
			Call(ConvKerName, LOC_INNER_LOOP,
				(NeedConvDim&&NeedStride)?
					Bindings(8, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE), C_Arg("Norm"), Imm(FS), Imm(Stride))
				:
				((NeedConvDim)?
					Bindings(7, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), 
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE), C_Arg("Norm"), Imm(FS))
				 :
					Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H),
					    	    K_Arg("Filter", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE), C_Arg("Norm"))
				)
			),
			Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
				Bindings(4, K_Arg("BiasOut", KER_ARG_TILE), K_Arg("BiasOut", KER_ARG_TILE_W), K_Arg("BiasOut", KER_ARG_TILE_H),
					    K_Arg("Out", KER_ARG_TILE)))
		     ),
		KerArgs(4,
			KerArg("In",      OBJ_IN_DB_3D,     	Width,	 Height,  sizeof(short int), FS-1, 0, Stride*2, "In", 0),
			KerArg("Filter",  OBJ_IN_DB_NTILED_4D, 	FS,	 FS, 	  sizeof(short int), 0,    0, 0,        "Filter", 0), 
			KerArg("BiasOut", OBJ_BUFFER_ONETILE,	WCOut,	 HCOut,   sizeof(short int), 0,    0, 0,        "", 0),
			KerArg("Out",     OBJ_OUT_DB_3D,   	WCOut/2, HCOut/2, sizeof(short int), 0,    0, 0,        "Out", 0)
			)
	);
}

/* HWCE enabled convolutions */

/* Pure convolutions, no bias setting and no accumulation */
void CNN_TiledPlainConvNxN_HWCE_fp(char *Name, unsigned int FS, unsigned int Width, unsigned int Height)

{
	char *ConvKerName;
	int Fw, Fh;	/* Filter dimensions, Since FS*FS is odd and HwCE supports only 4 byte aligned accesses Fw * Fh = FS * FS + 1 */
	int ConvMode, Mode3x3 = 0;

	switch (FS) {
		case 3:
			ConvKerName = "HWCE_ProcessOneTile3x3_MultiOut"; Fw = 5; Fh = 2; ConvMode = 1; Mode3x3 = 1; break;
		case 5:
			ConvKerName = "HWCE_ProcessOneTile5x5"; Fw = 13; Fh = 2; ConvMode = 0; break;
		case 7:
			ConvKerName = "HWCE_ProcessOneTile7x7"; Fw = 25; Fh = 2; ConvMode = 2; break;
		default:
			GenTilingError("TiledPlainConvNxN_HWCE: Only 3x3, 5x5 and 7x7 are supported for HWCE enabled configurations");
	}
	UserKernel(Name,
		KernelDimensions(1, Width, Height, 1),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_VER,
		CArgs(4,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm")
		     ),
		Calls(4,
			Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
			Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(ConvMode), Imm(0), C_Arg("Norm"))),
		Mode3x3?
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(9, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), Imm(0), Imm(0),
					    K_Arg("Filter", KER_ARG_TILE), Imm(0), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), Imm(0x7))):
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), K_Arg("Filter", KER_ARG_TILE), Imm(0),
					    K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H))),
			Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		KerArgs(3,
			KerArg("In",   	 OBJ_IN_DB,            Width,	   Height, 	sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter", OBJ_BUFFER_IN_NTILED, Fw,	   Fh, 		sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("Out",    OBJ_OUT_DB,	       Width-FS+1, Height-FS+1,	sizeof(short int), 0,  0, 0,  "Out", 0)
		       )
	);
}

/*  Convolution layer, bias setting and accumulation */
void CNN_TiledConvNxN_HWCE_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height)

{
	char *ConvKerName;
	int Fw, Fh;	/* Filter dimensions, Since FS*FS is odd and HwCE supports only 4 byte aligned accesses Fw * Fh = FS * FS + 1 */
	int ConvMode, Mode3x3 = 0;

	switch (FS) {
		case 3:
			ConvKerName = "HWCE_ProcessOneTile3x3_MultiOut"; Fw = 5; Fh = 2; ConvMode = 1; Mode3x3 = 1; break;
		case 5:
			ConvKerName = "HWCE_ProcessOneTile5x5"; Fw = 13; Fh = 2; ConvMode = 0; break;
		case 7:
			ConvKerName = "HWCE_ProcessOneTile7x7"; Fw = 25; Fh = 2; ConvMode = 2; break;
		default:
			GenTilingError("TiledConvNxN_HWCE: Only 3x3, 5x5 and 7x7 are supported for HWCE enabled configurations");
	}
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_VER,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Bias"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm")
		     ),
		Calls(6,
			Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
			Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(ConvMode), Imm(0), C_Arg("Norm"))),
			Call("HwCE_SetYinMode", LOC_IN_PLANE_PROLOG, Bindings(1, Imm(1))),
		Mode3x3?
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(9, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), Imm(0), Imm(0),
					    K_Arg("Filter", KER_ARG_TILE), C_ArgIndex("Bias", KER_OUT_PLANE, 1),
					    K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), Imm(0x7))):
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), K_Arg("Filter", KER_ARG_TILE),
					    C_ArgIndex("Bias", KER_OUT_PLANE, 1), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H))),
			Call("HwCE_SetYinMode", LOC_INNER_LOOP, Bindings(1, Imm(0))),
			Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		KerArgs(3,
			KerArg("In",   	 OBJ_IN_DB_3D,        Width,      Height,      sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D, Fw,	  Fh, 	       sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("Out",    OBJ_OUT_DB_3D,	      Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out", 0)
		       )
	);
}

/*  Convolution layer, 3x3 convolution multiple output mode, bias setting and accumulation */
void CNN_TiledConv3x3_HWCE_MultiOut_fp(char *Name, unsigned int Nout, unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height)

{
	char *ConvKerName, *SetBiasKerName;
	int FS = 3;
	int Mode, ConvMode = 1;

	ConvKerName = "HWCE_ProcessOneTile3x3_MultiOut";
	switch (Nout) {
		case 1: SetBiasKerName = "KerSetInBias"; Mode = 0x7; break;
		case 2: SetBiasKerName = "KerSetInBias2"; Mode = 0x3; break;
		case 3: SetBiasKerName = "KerSetInBias3"; Mode = 0x1; break;
		default:
			GenTilingError("TiledConv3x3MultiOut_HWCE: Only 1, 2 or 3 output mode supported for HWCE 3x3 enabled configurations");
	}

	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_VER,
	(Nout == 3)?
		CArgs(9,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
	      	      TCArg("short int * __restrict__", "Out1"),
	      	      TCArg("short int * __restrict__", "Out2"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0"),
		      TCArg("short int * __restrict__", "Bias1"),
		      TCArg("short int * __restrict__", "Bias2")
		     ):
	((Nout == 2)?
		CArgs(7,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
	      	      TCArg("short int * __restrict__", "Out1"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0"),
		      TCArg("short int * __restrict__", "Bias1")
		     ):
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0")
		     )),
		Calls(6,
			Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
			Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(ConvMode), Imm(0), C_Arg("Norm"))),
			Call("HwCE_SetYinMode", LOC_PROLOG, Bindings(1, Imm(0))),
			Call(SetBiasKerName, LOC_IN_PLANE_PROLOG,
				(Nout == 3)?
				Bindings(8, K_Arg("Out0", KER_ARG_TILE), K_Arg("Out1", KER_ARG_TILE), K_Arg("Out2", KER_ARG_TILE),
					    K_Arg("Out0", KER_ARG_TILE_W), K_Arg("Out0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 3), C_ArgIndex("Bias1", KER_OUT_PLANE, 3), C_ArgIndex("Bias2", KER_OUT_PLANE, 3)):
				((Nout == 2)?
				Bindings(6, K_Arg("Out0", KER_ARG_TILE), K_Arg("Out1", KER_ARG_TILE),
					    K_Arg("Out0", KER_ARG_TILE_W), K_Arg("Out0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 2), C_ArgIndex("Bias1", KER_OUT_PLANE, 2)):
				Bindings(4, K_Arg("Out0", KER_ARG_TILE), K_Arg("Out0", KER_ARG_TILE_W), K_Arg("Out0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 1)))),
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(9, K_Arg("In", KER_ARG_TILE),
					    K_Arg("Out0", KER_ARG_TILE), (Nout >=2)?K_Arg("Out1", KER_ARG_TILE):Imm(0), (Nout==3)?K_Arg("Out2", KER_ARG_TILE):Imm(0),
					    K_Arg("Filter", KER_ARG_TILE), Imm(0), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), Imm(Mode))),
			Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		(Nout == 3)?
		KerArgs(5,
			KerArg("In",   	 OBJ_IN_DB_3D,        Width,      Height,      sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D, 7,	  4, 	       sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("Out0",   OBJ_OUT_DB_3D,	      Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out0", 3),
			KerArg("Out1",   OBJ_OUT_DB_3D,       Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out1", 3),
			KerArg("Out2",   OBJ_OUT_DB_3D,       Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out2", 3)
		       ):
		((Nout == 2)?
		KerArgs(4,
			KerArg("In",   	 OBJ_IN_DB_3D,        Width,      Height,      sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D, 3*2,	  3, 	       sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("Out0",   OBJ_OUT_DB_3D,       Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out0", 2),
			KerArg("Out1",   OBJ_OUT_DB_3D,       Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out1", 2)
		       ):
		KerArgs(3,
			KerArg("In",   	 OBJ_IN_DB_3D,        Width,      Height,      sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter", OBJ_IN_DB_NTILED_4D, 5,	  2, 	       sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("Out0",   OBJ_OUT_DB_3D,	      Width-FS+1, Height-FS+1, sizeof(short int), 0,  0, 0,  "Out0", 0)
		       ))
	);
}


/* HWCE enabled composite kernels: NxN convolutions, ReLU and Max or Average Pooling 2x2 -> 1 */
void CNN_TiledConvNxNReLUPool2x2_HWCE_fp(char *Name, unsigned int FS, unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height,
				     unsigned int PoolMax)

{
	char *ConvKerName, *KerReLUPoolName;
	int Fw, Fh;	/* Filter dimensions, Since FS*FS is odd and HwCE supports only 4 byte aligned accesses Fw * Fh = FS * FS + 1 */
	int ConvMode, Mode3x3 = 0;
	int Cons = 0;

	switch (FS) {
		case 3:
			ConvKerName = "HWCE_ProcessOneTile3x3_MultiOut"; Fw = 5; Fh = 2; ConvMode = 1; Mode3x3 = 1; break;
		case 5:
			ConvKerName = "HWCE_ProcessOneTile5x5"; Fw = 13; Fh = 2; ConvMode = 0; break;
		case 7:
			ConvKerName = "HWCE_ProcessOneTile7x7"; Fw = 25; Fh = 2; ConvMode = 2; break;
		default:
			GenTilingError("TiledConvNxN_HWCE: Only 3x3, 5x5 and 7x7 are supported for HWCE enabled configurations");
	}
	switch (PoolMax) {
		case 0:	/* Relu, PoolMax */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; Cons = 2; break;
		case 1:	/* Relu, PoolAvg */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; Cons = 2; break;
		case 2:	/* No Relu, PoolMax */
			KerReLUPoolName = "KerAvgPool2x2_fp"; Cons = 2; break;
		case 3:	/* No Relu, PoolAvg */
			KerReLUPoolName = "KerMaxPool2x2_fp"; Cons = 2; break;

		case 4:	/* Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerReLUAvgPool3x3_fp"; Cons = 3; break;
		case 5:	/* Relu, PoolAvg 3x3 */
			KerReLUPoolName = "KerReLUMaxPool3x3_fp"; Cons = 3; break;
		case 6:	/* No Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerAvgPool3x3_fp"; Cons = 3; break;
		case 7:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool3x3_fp"; Cons = 3; break;
	}
	// UserKernel("Conv5x5ReLUMaxPool2x2_HWCE",
	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_VER,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(7,
			Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
			Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(0), Imm(0), C_Arg("Norm"))),
			Call("HwCE_SetYinMode", LOC_IN_PLANE_PROLOG, Bindings(1, Imm(1))),
			Mode3x3?
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(9, K_Arg("In", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE), Imm(0), Imm(0),
					    K_Arg("Filter", KER_ARG_TILE), C_ArgIndex("Bias", KER_OUT_PLANE, 1),
					    K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), Imm(0x7))):
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("SetBiasOut", KER_ARG_TILE), K_Arg("Filter", KER_ARG_TILE),
					    C_ArgIndex("Bias", KER_OUT_PLANE, 1), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H))),
			Call("HwCE_SetYinMode", LOC_INNER_LOOP, Bindings(1, Imm(0))),
			Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
				Bindings(4, K_Arg("SetBiasOut", KER_ARG_TILE), K_Arg("SetBiasOut", KER_ARG_TILE_W), K_Arg("SetBiasOut", KER_ARG_TILE_H),
					    K_Arg("Out", KER_ARG_TILE))),
			Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		KerArgs(4,
			KerArg("In",   	      OBJ_IN_DB_3D,        Width,	   Height, 	    sizeof(short int), FS-1, 0, Cons, "In", 0),
			KerArg("Filter",      OBJ_IN_DB_NTILED_4D, Fw,	    	   Fh, 	     	    sizeof(short int), 0,    0, 0,    "Filter", 0), 
			KerArg("SetBiasOut",  OBJ_BUFFER_ONETILE,  (Width-FS+1),   Height-FS+1,     sizeof(short int), 0,    0, 0,    "", 0),
			KerArg("Out",         OBJ_OUT_DB_3D, 	   (Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,    0, 0,    "Out", 0)
		       )
	);
}
/* HWCE enabled composite kernels: NxN convolutions, ReLU and Max or Average Pooling 2x2 -> 1 */
void CNN_TiledConvNxMReLUPool2x2_HWCE_fp(char *Name, unsigned int FSN, unsigned int FSM, unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height,
				     unsigned int PoolMax)

{
	char *ConvKerName, *KerReLUPoolName;
	int Fw, Fh;	/* Filter dimensions, Since FS*FS is odd and HwCE supports only 4 byte aligned accesses Fw * Fh = FS * FS + 1 */
	int FS;

	int ConvMode, Mode3x3 = 0;
	int Cons = 0;
	int WW = (Width-FSN+1)/2;
	int HH = (Height-FSM+1)/2;

	// align W on words (even number of Word16: pooling )
	int WO =  (Width-FSN+1);
	if (PoolMax!=8 && (WO%2)) WO--; 
	if (PoolMax==8 && (WO%2)) WO++; 
	
	switch (FSN) {
		case 8:
		  ConvKerName = "HWCE_ProcessOneTile8x20"; Fw = (2*4*(5*5+1)); Fh = 1; ConvMode = 1; Mode3x3 = 1;
		  FS=5;
		  break;
		case 4:
		  ConvKerName = "HWCE_ProcessOneTile4x10"; Fw = (2*(5*5+1)); Fh = 1; ConvMode = 0;
		  FS=5;
		  break;
		default:
			GenTilingError("TiledConvNxM_HWCE: Only 8x20 and 4x10 are supported for HWCE enabled configurations");
	}
	switch (PoolMax) {
	        case 0:	/* Relu, PoolMax */
			KerReLUPoolName = "KerReLUAvgPool2x2_fp"; Cons = 2; break;
		case 1:	/* Relu, PoolAvg */
			KerReLUPoolName = "KerReLUMaxPool2x2_fp"; Cons = 2; break;
		case 2:	/* No Relu, PoolMax */
			KerReLUPoolName = "KerAvgPool2x2_fp"; Cons = 2; break;
		case 3:	/* No Relu, PoolAvg */
			KerReLUPoolName = "KerMaxPool2x2_fp"; Cons = 2; break;

		case 4:	/* Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerReLUAvgPool3x3_fp"; Cons = 3; break;
		case 5:	/* Relu, PoolAvg 3x3 */
			KerReLUPoolName = "KerReLUMaxPool3x3_fp"; Cons = 3; break;
		case 6:	/* No Relu, PoolMax 3x3 */
			KerReLUPoolName = "KerAvgPool3x3_fp"; Cons = 3; break;
		case 7:	/* No Relu, PoolAvg 2x2 */
			KerReLUPoolName = "KerMaxPool3x3_fp"; Cons = 3; break;
	        case 8:	/*  Relu, no pool */
			KerReLUPoolName = "KerReLU_fp";
			// align width on word (no pooling => next  even)
			WW = (Width-FSN+1);
			if (WW%2) WW++;
			HH = (Height-FSM+1);
			break;

	}
	// UserKernel("Conv5x5ReLUMaxPool2x2_HWCE",

	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_VER,
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__",	"Bias")
		     ),
		Calls(7,
		      Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
		      Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(0), Imm(0), C_Arg("Norm"))),
		      Call("HwCE_SetYinMode", LOC_IN_PLANE_PROLOG, Bindings(1, Imm(1))),
		      Call(ConvKerName, LOC_IN_PLANE,
			   Bindings(6, K_Arg("In", KER_ARG_TILE), K_Arg("SetBiasOut", KER_ARG_TILE), K_Arg("Filter", KER_ARG_TILE),
				    C_ArgIndex("Bias", KER_OUT_PLANE, 1), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H))),
		      Call("HwCE_SetYinMode", LOC_IN_PLANE, Bindings(1, Imm(0))),
		      Call(KerReLUPoolName, LOC_IN_PLANE_EPILOG,
			   Bindings(4, K_Arg("SetBiasOut", KER_ARG_TILE), K_Arg("SetBiasOut", KER_ARG_TILE_W), K_Arg("SetBiasOut", KER_ARG_TILE_H),
				    K_Arg("Out", KER_ARG_TILE))),
		      Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		KerArgs(4,
			KerArg("In",   	      OBJ_IN_DB_3D,        Width,	   Height, 	    sizeof(short int), FS-1, 0, Cons, "In", 0),
			KerArg("Filter",      OBJ_IN_DB_NTILED_4D, Fw,	    	   Fh, 	     	    sizeof(short int), 0,    0, 0,    "Filter", 0), 
			KerArg("SetBiasOut",  OBJ_BUFFER_ONETILE,  WO,   (Height-FSM+1),     sizeof(short int), 0,    0, 0,    "", 0),
			KerArg("Out",         OBJ_OUT_DB_3D, 	   WW, HH, sizeof(short int), 0,    0, 0,    "Out", 0)
		       )
	);
}

/* HWCE enabled composite kernels: 3x3 convolutions, ReLU and Max Pooling 2x2 -> 1, multiple out mode */
void CNN_TiledConv3x3ReLUPool2x2_HWCE_MultiOut_fp(char *Name, unsigned int Nout,
					      unsigned int InPlane, unsigned int OutPlane, unsigned int Width, unsigned int Height,
					      unsigned int PoolMax)

{
	char *ConvKerName, *SetBiasKerName, *ReLUPoolKerName;
	int FS = 3;
	int Mode, ConvMode = 1;

	ConvKerName = "HWCE_ProcessOneTile3x3_MultiOut";
	switch (Nout) {
		case 1:
			if (PoolMax) ReLUPoolKerName = "KerReLUMaxPool2x2_fp"; else ReLUPoolKerName = "KerReLUAvgPool2x2_fp";
			SetBiasKerName = "KerSetInBias"; Mode = 0x7; break;
		case 2:
			if (PoolMax) ReLUPoolKerName = "KerReLUMaxPool2x2_2_fp"; else ReLUPoolKerName = "KerReLUAvgPool2x2_2_fp";
			SetBiasKerName = "KerSetInBias2"; Mode = 0x3; break;
		case 3:
			if (PoolMax) ReLUPoolKerName = "KerReLUMaxPool2x2_3_fp"; else ReLUPoolKerName = "KerReLUAvgPool2x2_3_fp";
			SetBiasKerName = "KerSetInBias3"; Mode = 0x1; break;
		default:
			GenTilingError("TiledConv3x3MultiOut_HWCE: Only 1, 2 or 3 output mode supported for HWCE 3x3 enabled configurations");
	}

	UserKernel(Name,
		KernelDimensions(InPlane, Width, Height, OutPlane),
		KernelIterationOrder(KER_DIM4, KER_OUT_PLANE, KER_TILE, KER_IN_PLANE),
		TILE_VER,
	(Nout == 3)?
		CArgs(9,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
	      	      TCArg("short int * __restrict__", "Out1"),
	      	      TCArg("short int * __restrict__", "Out2"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0"),
		      TCArg("short int * __restrict__", "Bias1"),
		      TCArg("short int * __restrict__", "Bias2")
		     ):
	((Nout == 2)?
		CArgs(7,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
	      	      TCArg("short int * __restrict__", "Out1"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0"),
		      TCArg("short int * __restrict__", "Bias1")
		     ):
		CArgs(5,
		      TCArg("short int * __restrict__", "In"),
		      TCArg("short int * __restrict__", "Filter"),
		      TCArg("short int * __restrict__", "Out0"),
		      TCArg("unsigned int", 		"Norm"),
		      TCArg("short int * __restrict__", "Bias0")
		     )),
		Calls(6,
			Call("HWCE_Enable", LOC_PROLOG, Bindings(0)),
			Call("HWCE_GenericInit", LOC_PROLOG, Bindings(3, Imm(ConvMode), Imm(0), C_Arg("Norm"))),
			Call("HwCE_SetYinMode", LOC_PROLOG, Bindings(1, Imm(0))),
			Call(SetBiasKerName, LOC_IN_PLANE_PROLOG,
				(Nout == 3)?
				Bindings(8, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut1", KER_ARG_TILE), K_Arg("SetBiasOut2", KER_ARG_TILE),
					    K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 3), C_ArgIndex("Bias1", KER_OUT_PLANE, 3), C_ArgIndex("Bias2", KER_OUT_PLANE, 3)):
				((Nout == 2)?
				Bindings(6, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut1", KER_ARG_TILE),
					    K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 2), C_ArgIndex("Bias1", KER_OUT_PLANE, 2)):
				Bindings(4, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    C_ArgIndex("Bias0", KER_OUT_PLANE, 1)))),
			Call(ConvKerName, LOC_INNER_LOOP,
				Bindings(9, K_Arg("In", KER_ARG_TILE),
					    K_Arg("SetBiasOut0", KER_ARG_TILE),
					    (Nout >= 2)?K_Arg("SetBiasOut1", KER_ARG_TILE):Imm(0),
					    (Nout == 3)?K_Arg("SetBiasOut2", KER_ARG_TILE):Imm(0),
					    K_Arg("Filter", KER_ARG_TILE), Imm(0), K_Arg("In", KER_ARG_TILE_W), K_Arg("In", KER_ARG_TILE_H), Imm(Mode))),

			Call(ReLUPoolKerName, LOC_IN_PLANE_EPILOG,
				(Nout == 3)?
				Bindings(8, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut1", KER_ARG_TILE), K_Arg("SetBiasOut2", KER_ARG_TILE),
					    K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    K_Arg("Out0", KER_ARG_TILE), K_Arg("Out1", KER_ARG_TILE), K_Arg("Out2", KER_ARG_TILE)):
				((Nout == 2)?
				Bindings(6, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut1", KER_ARG_TILE),
					    K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    K_Arg("Out0", KER_ARG_TILE), K_Arg("Out1", KER_ARG_TILE)):
				Bindings(4, K_Arg("SetBiasOut0", KER_ARG_TILE), K_Arg("SetBiasOut0", KER_ARG_TILE_W), K_Arg("SetBiasOut0", KER_ARG_TILE_H),
					    K_Arg("Out", KER_ARG_TILE))
				)),

			Call("HWCE_Disable", LOC_EPILOG, Bindings(0))
		     ),
		(Nout == 3)?
		KerArgs(8,
			KerArg("In",   	      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter",      OBJ_IN_DB_NTILED_4D, 	7,	    	4, 	     	 sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("SetBiasOut0", OBJ_BUFFER_ONETILE,	(Width-FS+1),	(Height-FS+1),   sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("SetBiasOut1", OBJ_BUFFER_ONETILE,	(Width-FS+1),	(Height-FS+1),   sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("SetBiasOut2", OBJ_BUFFER_ONETILE,	(Width-FS+1),	(Height-FS+1),   sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("Out0",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out0", 3),
			KerArg("Out1",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out1", 3),
			KerArg("Out2",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out2", 3)
		       ):
		((Nout == 2) ?
		KerArgs(6,
			KerArg("In",   	      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter",      OBJ_IN_DB_NTILED_4D, 	3*2,	    	3, 	     	 sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("SetBiasOut0", OBJ_BUFFER_ONETILE,	(Width-FS+1),	(Height-FS+1),   sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("SetBiasOut1", OBJ_BUFFER_ONETILE,	(Width-FS+1),	(Height-FS+1),   sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("Out0",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out0", 2),
			KerArg("Out1",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out1", 2)
		       ):
		KerArgs(4,
			KerArg("In",   	      OBJ_IN_DB_3D,     	Width,	    	Height, 	 sizeof(short int), FS-1, OBJ_CONSTRAINTS_EVEN, 0, "In", 0),
			KerArg("Filter",      OBJ_IN_DB_NTILED_4D, 	5,	    	2, 	     	 sizeof(short int), 0,  0, 0,  "Filter", 0), 
			KerArg("SetBiasOut0", OBJ_BUFFER_ONETILE,	(Width-FS+1),	Height-FS+1,     sizeof(short int), 0,  0, 0,  "", 0),
			KerArg("Out0",        OBJ_OUT_DB_3D, 		(Width-FS+1)/2, (Height-FS+1)/2, sizeof(short int), 0,  0, 0,  "Out0", 0)
		       )
		)
	);
}

/* Software composite kernels: ReLU then linear layer, pure linear layer on different data format */
void CNN_TiledLinearLayer(char *Name, unsigned int InPlane, unsigned int Width, unsigned int Height, unsigned int OutPlane, int ModeSize, int ReLU, int CoeffInL3)

{
	char *KerName, *CoeffType, *OutType;
	unsigned int CSize, OSize;
	unsigned int InSize = (Width*Height)*InPlane;
	unsigned int OutSize = OutPlane;
	unsigned int flag_halfchar = 1;
	
	switch (ModeSize) {
		case 0:
			CoeffType = "Word8 * __restrict__"; CSize = sizeof(Word8); OutType = "Word16 * __restrict__";  OSize = sizeof(Word16);
			if (ReLU) KerName ="KerLinearLayerReLU_fps"; else KerName = "KerLinearLayer_fps";
			break;
		case 1:
			CoeffType = "Word16 * __restrict__"; CSize = sizeof(Word16); OutType = "Word16 * __restrict__";  OSize = sizeof(Word16);
			if (ReLU) KerName ="KerLinearLayerReLU_fp"; else KerName = "KerLinearLayer_fp";
			break;
		case 2:
			CoeffType = "Word16 * __restrict__"; CSize = sizeof(Word16); OutType = "Word32 * __restrict__";  OSize = sizeof(Word32);
			if (ReLU) KerName ="KerLinearLayerReLU_fpd"; else KerName = "KerLinearLayer_fpd";
			break;
		case 3:
			CoeffType = "Word8 * __restrict__"; CSize = sizeof(Word8); OutType = "Word32 * __restrict__";  OSize = sizeof(Word32);
			KerName ="KerLinearLayer_halfchar"; 
			flag_halfchar = 2;
 break;
	default:
			GenTilingError("TiledLinearLayer: valid ModeSize = [0, 1, 2] for [Coeff, Out] = [(Byte,Short), (Short, Short), (Short, Int)]");
	}

	UserKernel(Name,
		KernelDimensions(1, Width, Height, 1),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_HOR,
		CArgs(7,
		      TCArg("Word16 * __restrict__", "In"),
		      TCArg(CoeffType, "Filter"),
		      TCArg("unsigned int", "NormFilter"),
		      TCArg("Word16 * __restrict__", "Bias"),
		      TCArg("unsigned int", "NormBias"),
		      TCArg(OutType, "Out"),
		      TCArg("int", "OutSize")
		     ),
		Calls(1,
			Call(KerName, LOC_INNER_LOOP,
				Bindings(8, K_Arg("In", KER_ARG_TILE), K_Arg("In", KER_ARG_TILE_W), K_Arg("Filter", KER_ARG_TILE), C_Arg("NormFilter"),
					    K_Arg("Bias", KER_ARG_TILE), C_Arg("NormBias"), K_Arg("Out", KER_ARG_TILE), K_Arg("Out", KER_ARG_TILE_H))
			    )
		     ),
		KerArgs(4,
			KerArg("In",   	OBJ_BUFFER_IN_NTILED,			InSize,	1, 	 sizeof(Word16), 0, 0, 0, "In", 0),
			KerArg("Bias", 	OBJ_BUFFER_IN,				1,	OutSize, sizeof(Word16), 0, 0, 0, "Bias", 0),
			KerArg("Filter",CoeffInL3?OBJ_IN_DB_L2DB:OBJ_IN_DB,	InSize,	OutSize/flag_halfchar, CSize, 	 0, 0, 0, "Filter", 0),
			KerArg("Out",   OBJ_BUFFER_OUT,				1,	OutSize, OSize, 	 0, 0, 0, "Out", 0)
			)
	);
}

void CNNConfiguration(unsigned int L1Memory)

{
        SetInlineMode(ALWAYS_INLINE); // SetInlineMode(NEVER_INLINE);
        SetSymbolNames("CNN_L1_Memory", "CNN_L2_Memory", "CNN_KernelDescr", "CNN_KernelArgs");
        SetSymbolDynamics();

        SetUsedFilesNames("KernelLibStdTypes.h", 1, "CNN_BasicKernels.h");
        SetGeneratedFilesNames("CNN_KernelsInit.c", "CNN_KernelsInit.h", "CNN_Kernels.c", "CNN_Kernels.h");

        SetL1MemorySize(L1Memory);
}

