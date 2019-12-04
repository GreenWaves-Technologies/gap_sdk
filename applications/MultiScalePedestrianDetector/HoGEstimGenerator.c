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

void LoadHOGLibrary()

{
	LibKernel("KerProcessCellLine", CALL_PARALLEL,
               CArgs(5,
			TCArg("uint8_t * __restrict__", "In"),
			TCArg("uint32_t", "W"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint32_t", "CellLineIndex"),
			TCArg("uint32_t", "CellLineCount")

		),
		"KerProcessCellLine_ArgT"
	);
	LibKernel("KerProcessBlockLine", CALL_PARALLEL,
		CArgs(3,
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint32_t", "W"),
			TCArg("uint16_t * __restrict__", "HOGFeatures")
		),
		"KerProcessBlockLine_ArgT"
	);


	LibKernel("KerReadHoGFeatCol", CALL_PARALLEL,
               CArgs(6,
			TCArg("uint16_t * __restrict__", "In"),
			TCArg("uint32_t", "H"),
			TCArg("uint32_t", "FeatureSize"),
			TCArg("uint32_t", "EstimWidth"),
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HoGFeatColIndex")
		),
		"KerReadHoGFeatCol_ArgT"
	);

	LibKernel("KerEstimate", CALL_PARALLEL,
               CArgs(6,
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "FeatureSize"),
			TCArg("uint32_t", "WEstimator"),
			TCArg("uint32_t", "HEstimator"),
			TCArg("uint32_t", "HFeatCols"),
			TCArg("uint32_t * __restrict__", "Out")

		),
		"KerEstimate_ArgT"
	);
	LibKernel("KerEstimateWin", CALL_SEQUENTIAL,
		CArgs(7,
        		TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
        		TCArg("uint32_t", "FeatureSize"),
        		TCArg("uint32_t", "WEstimator"),
        		TCArg("uint32_t", "HEstimator"),
        		TCArg("uint32_t", "HFeatCols"),
        		TCArg("uint32_t * __restrict__", "Out"),
        		TCArg("uint16_t * __restrict__", "Buffer")
		),
		""
	);
	LibKernel("KerWeakEstimateWin", CALL_SEQUENTIAL,
		CArgs(9,
        		TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
        		TCArg("uint32_t", "FeatureSize"),
        		TCArg("uint32_t", "WEstimator"),
        		TCArg("uint32_t", "HEstimator"),
        		TCArg("uint32_t", "HFeatCols"),
        		TCArg("uint8_t * __restrict__", "Out"),
        		TCArg("uint16_t * __restrict__", "Buffer"),
			TCArg("HoGWeakPredictor_T * __restrict__","Model"),
			TCArg("uint32_t", "ModelSize")
		),
		""
	);
	LibKernel("KerWeakEstimateAllWindows", CALL_PARALLEL,
		CArgs(7,
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "ColumnIndexM1"),
			TCArg("uint32_t", "HEstimator"),
			TCArg("uint32_t", "HFeatCols"),
			TCArg("uint32_t", "FeatureSize"),
			TCArg("HoGWeakPredictorBis_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		"KerWeakEstimate_ArgT"
	);

}


void GenerateHOG(char *Name,
		unsigned int W,			/* Image width */
		unsigned int H, 		/* Image Height */
		unsigned int CellSize, 		/* Cell is [CellSize x CellSize] pixels */
		unsigned int BlockSize, 	/* Block is [BlockSize x BlockSize] Cells */
		unsigned int BlockOverlap, 	/* Number Cells in common between 2 adjacent blocks */
		unsigned int BinsPerCell)	/* Number of bins for a cell */

{
/*
	(H-2)/CellSize = BlockSize + Kh*(BlockSize-BlockOverlap)
		=> Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap)
		=> Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2

	Kh + 1 is the number of HOG feature blocks in the vertical direction
	Ha is the adjusted height of the image in order to have an a number of lines in the image that exactly gives Kh+1
	HOG features in the vertical direction.

	(W-2)/CellSize = BlockSize + Kw*(BlockSize-BlockOverlap)
		=> Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap)
		=> Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2

	Kw + 1 is the number of HOG feature blocks in the horizontal direction
	Wa is the adjusted width of the image in order to have an a number of lines in the image that exactly gives Kw+1
	HOG features in the horizontal direction.
*/
	unsigned int Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	// if ((W & 0x3) != 0) GenTilingError("Image width must be a multiple of 4, Actual: %d\n", W);

	printf("Generating HOG Feature Extraction:\n");
	printf("\tInput Image         [%3d x %3d], Adjusted to [%3d x %3d]\n",  W, H, Wa, Ha);
	printf("\tHOG Cell Size       [%3d      ], Bins per Cells: %d\n", CellSize, BinsPerCell);
	printf("\tHOG Block Size      [%3d      ], Overlap: %d,\n", BlockSize, BlockOverlap);
	printf("\tHOG Features output:[%3d x %3d]\n", Kw+1, Kh+1);

	OpenKernelGroup(Name);
	UserKernel(AppendNames("Prime", Name),
		KernelDimensions(0, W, CellSize*BlockSize+2, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_HOR,
		CArgs(3,
			TCArg("uint8_t *  __restrict__",  "ImageIn"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures")
		),
		Calls(2,
			Call("KerProcessCellLine", LOC_INNER_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_TILEINDEX),
					Imm(1)
				)
			),
			Call("KerProcessBlockLine", LOC_INNER_LOOP_EPILOG,
				Bindings(3,
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_W),
					K_Arg("KerFeatOut", KER_ARG_TILE)
				)
			)
		),
                KerArgs(2,
                        KerArg("KerIn",      OBJ_IN_DB,             W,    BlockSize*CellSize+2, sizeof(char),
		  	       2, OBJ_CONSTRAINTS_ONEPREFTILE, CellSize, "ImageIn", 0),
                        KerArg("KerFeatOut", OBJ_BUFFER_OUT_NTILED, 1+Kw, 1,                    BlockSize*BlockSize*BinsPerCell*sizeof(short),
			       0, 0, 0, "HOGFeatures", 0)
                )
	);
	UserKernel(AppendNames("Body", Name),
		KernelDimensions(0, W, H-CellSize*BlockSize, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_HOR,
		CArgs(3,
			TCArg("uint8_t *  __restrict__",  "ImageIn"),
			TCArg("uint16_t ** __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures")
		),
		Calls(2,
			Call("KerProcessCellLine", LOC_INNER_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					C_Arg("CellLines"),
					Imm(BlockSize),
					Imm(BlockSize-BlockOverlap)
				)
			),
			Call("KerProcessBlockLine", LOC_INNER_LOOP,
				Bindings(3,
					C_Arg("CellLines"),
					K_Arg("KerIn", KER_ARG_W),
					K_Arg("KerFeatOut", KER_ARG_TILE)
				)
			)
		),
                KerArgs(2,
                        KerArg("KerIn",      OBJ_IN_DB,  W,    Ha - BlockSize*CellSize, sizeof(char),
		  	       	2, OBJ_CONSTRAINTS_ONEPREFTILE, (BlockSize-BlockOverlap)*CellSize, "ImageIn", 0),
                        KerArg("KerFeatOut", OBJ_OUT_DB, 1+Kw, Kh,                      BlockSize*BlockSize*BinsPerCell*sizeof(short),
			       0, 0, 0, "HOGFeatures", 0)
                )
	);
	CloseKernelGroup();
	UserKernelGroup(Name,
		CArgs(5,
			TCArg("uint8_t *  __restrict__", "ImageIn"),
			TCArg("uint16_t **  __restrict__", "CellLines"),
			TCArg("uint16_t *  __restrict__", "HOGFeatures"),
			TCArg("uint32_t", "OneFullBlockLineOffset"),
			TCArg("uint32_t", "OneFullHoGFeatureLineOffset")
		),
		Calls(2,
			UserKernelCall(AppendNames("Prime", Name), LOC_GROUP,
				Bindings(3, C_Arg("ImageIn"), C_Arg("CellLines"), C_Arg("HOGFeatures"))
			),
			UserKernelCall(AppendNames("Body", Name), LOC_GROUP,
				Bindings(3,
					C_ArgPlusCvarOffset("ImageIn", "OneFullBlockLineOffset"),
					C_Arg("CellLines"),
					C_ArgPlusCvarOffset("HOGFeatures", "OneFullHoGFeatureLineOffset")
				)
			)
		)
	);
}

void GenerateWeakHOGEstimator(
		char *Name,
		unsigned int W,			/* Image width */
		unsigned int H, 		/* Image Height */
		unsigned int CellSize, 		/* Cell is [CellSize x CellSize] pixels */
		unsigned int BlockSize, 	/* Block is [BlockSize x BlockSize] Cells */
		unsigned int BlockOverlap, 	/* Number Cells in common between 2 adjacent blocks */
		unsigned int BinsPerCell,	/* Number of bins for a cell */
		unsigned int WEstimPix, 	/* Width of the estimation window, in pixels */
		unsigned int HEstimPix, 	/* Height of the estimation window, in pixels */
		unsigned int EstimSize, 	/* Size of estimation result per estimation window, in bytes */
		unsigned int NWeakEstim 	/* Number of weak estimators in the model */
	)

{
	unsigned int Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int HoGFeatSize = BlockSize*BlockSize*BinsPerCell*sizeof(unsigned short int);

	unsigned int Whf = 1 + Kw;	/* Width in HoG features */
	unsigned int Hhf = 1 + Kh;	/* Height in HoG features */

	unsigned int KhE = ((HEstimPix)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int KwE = ((WEstimPix)/CellSize - BlockSize)/(BlockSize - BlockOverlap);

	unsigned int WEstim = 1 + KwE;	/* Width of Estim window in HoG Features */
	unsigned int HEstim = 1 + KhE;	/* Height of Estim window in HoG Features */

	if (W<WEstimPix || H<HEstimPix) GenTilingError("Image size [%d x %d] must be larger than estimation window [%d x %d]\n", W, H, WEstimPix, HEstimPix);

	printf("Generating HOG Based Estimator, Weak Estimator Version:\n");
	printf("\tHOG Features       [%3d x %3d] from Image [%3d x %3d]\n", Whf, Hhf, W, H);
	printf("\tEstimation Window: [%3d x %3d] pixels or  [%3d x %3d] HOG features\n", WEstimPix, HEstimPix, WEstim, HEstim);
	printf("\tEstimation output: [%3d x %3d]\n", (Whf-WEstim)+1, (Hhf-HEstim)+1);
	printf("\tWeak estimators:   [%4d ]\n", NWeakEstim);


	OpenKernelGroup(Name);
	UserKernel(AppendNames("Prime", Name),
		KernelDimensions(0, WEstim, Hhf, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_VER,
		CArgs(6,
			TCArg("uint16_t * __restrict__",  "HoGFeatIn"),
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("uint8_t *  __restrict__", "Out"),
			TCArg("HoGWeakPredictor_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			Call("KerReadHoGFeatCol", LOC_INNER_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					C_Arg("HoGFeatCols"),
					K_Arg("KerIn", KER_ARG_TILEINDEX)
				)
			),
			Call("KerWeakEstimateWin", LOC_EPILOG,
				Bindings(9,
					C_Arg("HoGFeatCols"),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					Imm(HEstim),
					C_Arg("HFeat"), // Imm(Hhf),
					K_Arg("KerOut", KER_ARG_TILE), // C_Arg("Out")
					K_Arg("Buffer", KER_ARG),
					C_Arg("Model"),
					C_Arg("ModelSize")
				)
			)
		),
                KerArgs(3,
			KerArgPart("KerIn",  OBJ_IN_DB,             Whf, WEstim, Hhf,          Hhf,              HoGFeatSize, 0, OBJ_CONSTRAINTS_ONEPREFTILE, 1, "HoGFeatIn", 0),
			KerArgPart("KerOut", OBJ_BUFFER_OUT_NTILED, Whf-WEstim+1, 1, Hhf-HEstim+1, Hhf-HEstim+1, EstimSize,   0, 0, 0, "Out", 0),
			KerArg("Buffer", OBJ_BUFFER_NTILED, WEstim, HEstim, HoGFeatSize, 0, 0, 0, "", 0)


)
	);
	UserKernel(AppendNames("Body", Name),
		KernelDimensions(0, Whf-WEstim, Hhf, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_VER,
		CArgs(6,
			TCArg("uint16_t * __restrict__",  "HoGFeatIn"),
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("uint8_t *  __restrict__", "Out"),
			TCArg("HoGWeakPredictor_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			Call("KerReadHoGFeatCol", LOC_INNER_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					C_Arg("HoGFeatCols"),
					Imm(WEstim)
				)
			),
			Call("KerWeakEstimateWin", LOC_INNER_LOOP,
				Bindings(9,
					C_Arg("HoGFeatCols"),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					Imm(HEstim),
					C_Arg("HFeat"), // Imm(Hhf),
					K_Arg("KerOut", KER_ARG_TILE),
					K_Arg("Buffer", KER_ARG),
					C_Arg("Model"),
					C_Arg("ModelSize")
				)
			)
		),
                KerArgs(3,
                        KerArgPart("KerIn",  OBJ_IN_DB,  Whf,          Whf-WEstim, Hhf,          Hhf,          HoGFeatSize, 0, OBJ_CONSTRAINTS_ONEPREFTILE, 1, "HoGFeatIn", 0),
                        KerArgPart("KerOut", OBJ_OUT_DB, Whf-WEstim+1, Whf-WEstim, Hhf-HEstim+1, Hhf-HEstim+1, EstimSize,   0, 0,                           0, "Out", 0),
			KerArg    ("Buffer", OBJ_BUFFER_NTILED, WEstim, HEstim, HoGFeatSize, 0, 0, 0, "", 0)
                )
	);
	CloseKernelGroup();
	UserKernelGroup(Name,
		CArgs(6,
			TCArg("uint16_t *  __restrict__", "HoGFeatIn"),
			TCArg("uint16_t **  __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("uint8_t *  __restrict__", "Out"),
			TCArg("HoGWeakPredictor_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			UserKernelCall(AppendNames("Prime", Name), LOC_GROUP,
				Bindings(6, C_Arg("HoGFeatIn"), C_Arg("HoGFeatCols"), C_Arg("HFeat"), C_Arg("Out"), C_Arg("Model"), C_Arg("ModelSize"))
			),
			UserKernelCall(AppendNames("Body", Name), LOC_GROUP,
				Bindings(6, C_ArgPlusImmOffset("HoGFeatIn", WEstim*BlockSize*BlockSize*BinsPerCell),
					    C_Arg("HoGFeatCols"), C_Arg("HFeat"), C_ArgPlusImmOffset("Out", 1), C_Arg("Model"), C_Arg("ModelSize"))
			)
		)
	);
}


void GenerateWeakHOGEstimatorBis(
		char *Name,
		unsigned int W,			/* Image width */
		unsigned int H, 		/* Image Height */
		unsigned int CellSize, 		/* Cell is [CellSize x CellSize] pixels */
		unsigned int BlockSize, 	/* Block is [BlockSize x BlockSize] Cells */
		unsigned int BlockOverlap, 	/* Number Cells in common between 2 adjacent blocks */
		unsigned int BinsPerCell,	/* Number of bins for a cell */
		unsigned int WEstimPix, 	/* Width of the estimation window, in pixels */
		unsigned int HEstimPix, 	/* Height of the estimation window, in pixels */
		unsigned int EstimSize, 	/* Size of estimation result per estimation window, in bytes */
		unsigned int NWeakEstim 	/* Number of weak estimators in the model */
	)

{
	unsigned int Kh = ((H-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Ha = (Kh*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int Kw = ((W-2)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int Wa = (Kw*(BlockSize - BlockOverlap) + BlockSize) * CellSize + 2;

	unsigned int HoGFeatSize = BlockSize*BlockSize*BinsPerCell*sizeof(unsigned short int);

	unsigned int Whf = 1 + Kw;	/* Width in HoG features */
	unsigned int Hhf = 1 + Kh;	/* Height in HoG features */

	unsigned int KhE = ((HEstimPix)/CellSize - BlockSize)/(BlockSize - BlockOverlap);
	unsigned int KwE = ((WEstimPix)/CellSize - BlockSize)/(BlockSize - BlockOverlap);

	unsigned int WEstim = 1 + KwE;	/* Width of Estim window in HoG Features */
	unsigned int HEstim = 1 + KhE;	/* Height of Estim window in HoG Features */

	if (W<WEstimPix || H<HEstimPix) GenTilingError("Image size [%d x %d] must be larger than estimation window [%d x %d]\n", W, H, WEstimPix, HEstimPix);

	printf("Generating HOG Based Estimator, Weak Estimator Version:\n");
	printf("\tHOG Features       [%3d x %3d] from Image [%3d x %3d]\n", Whf, Hhf, W, H);
	printf("\tEstimation Window: [%3d x %3d] pixels or  [%3d x %3d] HOG features\n", WEstimPix, HEstimPix, WEstim, HEstim);
	printf("\tEstimation output: [%3d x %3d]\n", (Whf-WEstim)+1, (Hhf-HEstim)+1);
	printf("\tWeak estimators:   [%4d ]\n", NWeakEstim);


	OpenKernelGroup(Name);
	UserKernel(AppendNames("Prime", Name),
		KernelDimensions(0, WEstim, Hhf, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_VER,
		CArgs(5,
			TCArg("uint16_t * __restrict__",  "HoGFeatIn"),
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("HoGWeakPredictorBis_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			Call("KerReadHoGFeatCol", LOC_INNER_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					C_Arg("HoGFeatCols"),
					K_Arg("KerIn", KER_ARG_TILEINDEX)
				)
			),
			Call("KerWeakEstimateAllWindows", LOC_EPILOG,
				Bindings(7,
					C_Arg("HoGFeatCols"),
					Imm(-1),
					Imm(HEstim),
					C_Arg("HFeat"), // Imm(Hhf),
					Imm(BlockSize*BlockSize*BinsPerCell),
					C_Arg("Model"),
					C_Arg("ModelSize")
				)
			)
		),
                KerArgs(1, KerArgPart("KerIn",  OBJ_IN_DB, Whf, WEstim, Hhf, Hhf, HoGFeatSize, 0, OBJ_CONSTRAINTS_ONEPREFTILE, 1, "HoGFeatIn", 0))
	);
	UserKernel(AppendNames("Body", Name),
		KernelDimensions(0, Whf-WEstim, Hhf, 0),
		KernelIterationOrder(KER_DIM2, KER_TILE),
		TILE_VER,
		CArgs(5,
			TCArg("uint16_t * __restrict__",  "HoGFeatIn"),
			TCArg("uint16_t ** __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("HoGWeakPredictorBis_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			Call("KerReadHoGFeatCol", LOC_INNER_LOOP,
				Bindings(6,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_H),
					Imm(BlockSize*BlockSize*BinsPerCell),
					Imm(WEstim),
					C_Arg("HoGFeatCols"),
					Imm(WEstim)
				)
			),
			Call("KerWeakEstimateAllWindows", LOC_INNER_LOOP,
				Bindings(7,
					C_Arg("HoGFeatCols"),
					K_Arg("KerIn", KER_ARG_TILEINDEX),
					Imm(HEstim),
					C_Arg("HFeat"), // Imm(Hhf),
					Imm(BlockSize*BlockSize*BinsPerCell),
					C_Arg("Model"),
					C_Arg("ModelSize")
				)
			)
		),
                KerArgs(1, KerArgPart("KerIn",  OBJ_IN_DB, Whf, Whf-WEstim, Hhf, Hhf, HoGFeatSize, 0, OBJ_CONSTRAINTS_ONEPREFTILE, 1, "HoGFeatIn", 0))
	);
	CloseKernelGroup();
	UserKernelGroup(Name,
		CArgs(5,
			TCArg("uint16_t *  __restrict__", "HoGFeatIn"),
			TCArg("uint16_t **  __restrict__", "HoGFeatCols"),
			TCArg("uint32_t", "HFeat"),
			TCArg("HoGWeakPredictorBis_T * __restrict__", "Model"),
			TCArg("uint32_t", "ModelSize")
		),
		Calls(2,
			UserKernelCall(AppendNames("Prime", Name), LOC_GROUP,
				Bindings(5, C_Arg("HoGFeatIn"), C_Arg("HoGFeatCols"), C_Arg("HFeat"), C_Arg("Model"), C_Arg("ModelSize"))
			),
			UserKernelCall(AppendNames("Body", Name), LOC_GROUP,
				Bindings(5, C_ArgPlusImmOffset("HoGFeatIn", WEstim*BlockSize*BlockSize*BinsPerCell),
					    C_Arg("HoGFeatCols"), C_Arg("HFeat"), C_Arg("Model"), C_Arg("ModelSize"))
			)
		)
	);
}

void HOGEstimDependencies()

{
        SetUsedFilesNames(0, 2, "HoGBasicKernels.h", "HoGEstimBasicKernels.h");
}

void HOGEstimConfiguration(unsigned int L1Memory)

{
         SetInlineMode(ALWAYS_INLINE);
	//SetInlineMode(NEVER_INLINE);
	SetSymbolNames("HOG_L1_Memory", "HOG_L2_Memory", "HOG_KernelDescr", "HOG_KernelArgs");
        SetSymbolDynamics();
	SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

	HOGEstimDependencies();
        SetGeneratedFilesNames("HoGEstimKernelsInit.c", "HoGEstimKernelsInit.h", "HoGEstimKernels.c", "HoGEstimKernels.h");

        SetL1MemorySize(L1Memory);
}
