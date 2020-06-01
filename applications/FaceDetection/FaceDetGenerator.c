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

void LoadFaceDetectionLibrary()
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

	LibKernel("KerIntegralImagePrime", CALL_PARALLEL,
            CArgs(2,
            TCArg("unsigned int * __restrict__", "KerBuffer"),
			TCArg("unsigned int", "W")
		),
		"KerPrimeImage_ArgT",
		NULL
	);
	LibKernel("KerIntegralImageProcess", CALL_PARALLEL,
               CArgs(5,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned int * __restrict__", "IntegralImage"),
			TCArg("unsigned int * __restrict__", "KerBuffer")

		),
		"KerProcessImage_ArgT",
		NULL
	);


	LibKernel("KerSquaredIntegralImageProcess", CALL_PARALLEL,
               CArgs(5,
			TCArg("unsigned char * __restrict__", "In"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("unsigned int * __restrict__", "IntegralImage"),
			TCArg("unsigned int * __restrict__", "KerBuffer")

		),
		"KerProcessImage_ArgT",
		NULL
	);

	LibKernel("KerEvaluateCascade", CALL_SEQUENTIAL,
            CArgs(8,
			TCArg("unsigned int * __restrict__", "IntegralImage"),
			TCArg("unsigned int * __restrict__", "SquaredIntegralImage"),
			TCArg("unsigned int", "W"),
			TCArg("unsigned int", "H"),
			TCArg("void *","cascade_model"),
			TCArg("unsigned char","WinW"),
			TCArg("unsigned char","WinH"),
			TCArg("int * __restrict__", "CascadeReponse")
		),
		"KerEvaluateCascade_ArgT",
		NULL
	);
}


void GenerateResize(char *Name, int Wi, int Hi, int Wo, int Ho)
{
	UserKernel(Name,
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2, TCArg("unsigned char *", "In"), TCArg("unsigned char *", "Out")),
		Calls(1, Call("KerResizeBilinear", LOC_LOOP,
			Bindings(8, K_Arg("In", KER_ARG_TILE),
				        K_Arg("In", KER_ARG_W),
				        K_Arg("In", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE),
				        K_Arg("Out", KER_ARG_W),
				        K_Arg("Out", KER_ARG_H),
				        K_Arg("Out", KER_ARG_TILE_H),
				        K_Arg("In", KER_ARG_TILE_BASE)))),
		KerArgs(2,
			KerArg("In" , KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB,  Wi, Hi, sizeof(char), 1, OBJ_CONSTRAINTS_DYNAMIC, 0, "In"),
			KerArg("Out", KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB, Wo, Ho, sizeof(char), 0, OBJ_CONSTRAINTS_DYNAMIC, 0, "Out")
		)
	);

}

void GenerateIntegralImage(char *Name,
		unsigned int W,     /* Image width */
		unsigned int H      /* Image Height */
	)
{

	UserKernel(AppendNames("Process", Name),
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2,
			TCArg("unsigned char *  __restrict__",  "ImageIn"),
			TCArg("unsigned int *  __restrict__", "IntegralImage")
		),
		Calls(2,
			Call("KerIntegralImagePrime", LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerBuffer",KER_ARG),
					K_Arg("KerIn", KER_ARG_TILE_W)
				)
			),
			Call("KerIntegralImageProcess", LOC_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					K_Arg("KerIn", KER_ARG_TILE_H),
					K_Arg("KerOut",KER_ARG_TILE),
					K_Arg("KerBuffer",KER_ARG)
				)
			)
		),
			KerArgs(3,
                KerArg("KerIn",    KerArgSpace(1,KER_ITER_TILE0), OBJ_IN_DB,           W,  H, sizeof(char), 0, 0, 0, "ImageIn"),
                KerArg("KerBuffer",KerArgSpace(1,KER_ITER_TILE0), O_BUFF | O_NDB | O_NOUT | O_NIN | O_NTILED ,   W,  1, sizeof(int),  0, 0, 0, 0),
                KerArg("KerOut",   KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB,          W,  H, sizeof(int),  0, 0, 0, "IntegralImage")
            )
	);
}


void GenerateSquaredIntegralImage(char *Name,
		unsigned int W,		/* Image width */
		unsigned int H 		/* Image Height */
	)
{

	UserKernel(AppendNames("Process", Name),
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(2,
			TCArg("unsigned char *  __restrict__",  "ImageIn"),
			TCArg("unsigned int *  __restrict__", "IntegralImage")
		),
		Calls(2,
			Call("KerIntegralImagePrime", LOC_LOOP_PROLOG,
				Bindings(2,
					K_Arg("KerBuffer",KER_ARG),
					K_Arg("KerIn", KER_ARG_TILE_W)
				)
			),
			Call("KerSquaredIntegralImageProcess", LOC_LOOP,
				Bindings(5,
					K_Arg("KerIn", KER_ARG_TILE),
					K_Arg("KerIn", KER_ARG_TILE_W),
					K_Arg("KerIn", KER_ARG_TILE_H),
					K_Arg("KerOut",KER_ARG_TILE),
					K_Arg("KerBuffer",KER_ARG)
				)
			)
		),
			KerArgs(3,
                KerArg("KerIn",     KerArgSpace(1,KER_ITER_TILE0),OBJ_IN_DB,           W,  H, sizeof(char), 0, 0, 0, "ImageIn"),
                KerArg("KerBuffer", KerArgSpace(1,KER_ITER_TILE0),O_BUFF | O_NDB | O_NOUT | O_NIN | O_NTILED ,   W,  1, sizeof(int),  0, 0, 0, 0),
                KerArg("KerOut",    KerArgSpace(1,KER_ITER_TILE0),OBJ_OUT_DB,          W,  H, sizeof(int),  0, 0, 0, "IntegralImage")
            )
	);
}

//result = windows_cascade_classifier(ArgC->ImageIntegral,ArgC->SquaredImageIntegral,face_model,24,24,Wout,j,i);

void GenerateCascadeClassifier(char *Name,
		unsigned int W,     /* Image width */
		unsigned int H,     /* Image Height */
		unsigned int WinW,  /* Detection window width */
		unsigned int WinH   /* Detection window Height */
	)
{


	UserKernel(AppendNames("Process", Name),
		KernelIterSpace(1, IterTiledSpace(KER_ITER_TILE0)),
		TILE_HOR,
		CArgs(4,
			TCArg("unsigned int *  __restrict__",  "IntegralImage"),
			TCArg("unsigned int *  __restrict__", "SquaredIntegralImage"),
			TCArg("void * "                     , "cascade_model"),
			TCArg("int  *  __restrict__"        , "CascadeReponse")
		),
		Calls(1,

			Call("KerEvaluateCascade", LOC_LOOP,
				Bindings(8,
					K_Arg("KerII", KER_ARG_TILE),
					K_Arg("KerIISQ", KER_ARG_TILE),
					K_Arg("KerII", KER_ARG_TILE_W),
					K_Arg("KerII", KER_ARG_TILE_H),
					C_Arg("cascade_model"),
					Imm(WinW),
					Imm(WinH),
					K_Arg("KerOut",KER_ARG_TILE)
				)
			)
		),
		KerArgs(3,
			KerArg("KerII",  KerArgSpace(1,KER_ITER_TILE0),  OBJ_IN_DB,   W,  H, sizeof(unsigned int), WinH-1, 0, 0, "IntegralImage" ),
			KerArg("KerIISQ",KerArgSpace(1,KER_ITER_TILE0),  OBJ_IN_DB,   W,  H, sizeof(unsigned int), WinH-1, 0, 0, "SquaredIntegralImage"),
			KerArg("KerOut", KerArgSpace(1,KER_ITER_TILE0), OBJ_OUT_DB,   W-WinW+1,  H-WinH+1, sizeof(int),  0, 0, 0, "CascadeReponse")
		)
	);
}


void FaceDetectionConfiguration(unsigned int L1Memory)

{
    SetInlineMode(ALWAYS_INLINE);
    //SetInlineMode(NEVER_INLINE);
	SetSymbolNames("FaceDet_L1_Memory", "FaceDet_L2_Memory");
    SetSymbolDynamics();
	//SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);

    SetUsedFilesNames(0, 1, "FaceDetBasicKernels.h");
    SetGeneratedFilesNames("FaceDetKernels.c", "FaceDetKernels.h");

    SetL1MemorySize(L1Memory);
}

