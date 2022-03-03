/*
 * Copyright (C) 2018 GreenWaves Technologies
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
#include "Gap.h"
#include "RNN_Generators_SQ8.h"

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

void Load_RNN_SQ8_Library()

{
	LoadCNN_Copy_Library();
        LibKernelTemplate("KerRNN_SQ8_T",
                  CArgs(14,
                        TCArg("signed char *__restrict__", "StateInOut"),
                        TCArg("signed char *__restrict__", "State"),
                        TCArg("signed char *__restrict__", "Xin"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("signed char *__restrict__", "Wf"),
                        TCArg("void * __restrict__", "Bf"),
                        TCArg("signed char *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
			TCArg("char", "FirstCell"),
			TCArg("char", "FirstOut"),
			TCArg("int", "TileOffset"),
			TCArg("char", "Reset")
		       )
	);
	LibKernel("RNN_KerB32_SQ8",			     CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);
	LibKernel("RNN_ParKerB32_SQ8",			     CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);
	LibKernel("RNN_ParKerB32_SameInStateScale_SQ8",      CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);
	LibKernel("RNN_ParKerB32_Hard_SQ8",		     CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);
	LibKernel("RNN_ParKerB32_Hard_SameInStateScale_SQ8", CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);

	LibKernelTemplate("KerLSTM_SQ8_T",
		  CArgs(20,
			TCArg("signed char *__restrict__", "StateInOut"),
			TCArg("signed char *__restrict__", "State"),
			TCArg("signed char *__restrict__", "Xin"),
			TCArg("unsigned short int", "DimState"),
			TCArg("unsigned short int", "DimIn"),
			TCArg("signed char *__restrict__", "Wf"),
			TCArg("void * __restrict__", "Bf"),
			TCArg("signed char *__restrict__", "Wi"),
			TCArg("void * __restrict__", "Bi"),
			TCArg("signed char *__restrict__", "Wg"),
			TCArg("void * __restrict__", "Bg"),
			TCArg("signed char *__restrict__", "Wo"),
			TCArg("void * __restrict__", "Bo"),
			TCArg("signed char *__restrict__", "Hout"),
			TCArg("unsigned short int", "Nout"),
			TCArg("signed char *__restrict__", "Infos"),
			TCArg("char", "FirstCell"),
			TCArg("char", "FirstOut"),
			TCArg("int", "TileOffset"),
			TCArg("char", "Reset")
		       )
	);
	LibKernel("LSTM_KerB32_SQ8",                          CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);
	LibKernel("LSTM_ParKerB32_SQ8",                       CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);
	LibKernel("LSTM_ParKerB32_SameInStateScale_SQ8",      CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);
	LibKernel("LSTM_ParKerB32_Hard_SQ8",                  CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);
	LibKernel("LSTM_ParKerB32_Hard_SameInStateScale_SQ8", CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);


	LibKernelTemplate("KerGRU_SQ8_T",
		  CArgs(19,
			TCArg("signed char *__restrict__", "StateInOut"),
			TCArg("signed char *__restrict__", "State"),
			TCArg("signed char *__restrict__", "Xin"),
			TCArg("unsigned short int", "DimState"),
			TCArg("unsigned short int", "DimIn"),
			TCArg("signed char *__restrict__", "Wr"),
			TCArg("int * __restrict__", "Br"),
			TCArg("signed char *__restrict__", "Wz"),
			TCArg("int * __restrict__", "Bz"),
			TCArg("signed char *__restrict__", "Wh"),
			TCArg("int * __restrict__", "Bwh"),
			TCArg("int * __restrict__", "Brh"),
			TCArg("signed char *__restrict__", "Hout"),
			TCArg("unsigned short int", "Nout"),
			TCArg("signed char *__restrict__", "Infos"),
			TCArg("char", "FirstCell"),
			TCArg("char", "FirstOut"),
			TCArg("int", "TileOffset"),
			TCArg("char", "Reset")
		       )
	);
	LibKernel("GRU_KerB32_SQ8", CALL_PARALLEL, 0, "KerGRU_SQ8_T", 0);
	LibKernel("GRU_ParKerB32_SQ8", CALL_PARALLEL, 0, "KerGRU_SQ8_T", 0);
	LibKernel("GRU_ParKerB32_Hard_SQ8", CALL_PARALLEL, 0, "KerGRU_SQ8_T", 0);

}

int RNN_Sequence(int Nc, int K0, int K1, int *n1, int *n2, int *n3, int *n2_io)

{
	int N1 = Min(K0, Nc-K1);
	int N3 = Nc - Max(K0, Nc-K1);
	int N2 = Nc - (N1 + N3);
	int N2_IO = ((N2>0)&&(K0+K1>=Nc));

	if (n1) *n1 = N1;
	if (n2) *n2 = N2;
	if (n3) *n3 = N3;
	if (n2_io) *n2_io = N2_IO;

	return ((N1!=0) + (N2!=0) + (N3!=0));
}

static Kernel_T *RNN_Stack_Seq_SQ8_Internal(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *RNNKerName,

	int BiasDataSize,
	int FeatDataSize,


	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int Buffer,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)

{
	/*      Sequences
		In:     DimIn!=0, ExposeSequence==0
		InOut:  DimIn!=0, ExposeSequence!=0
		None:   DimIn==0, ExposeSequence==0
		Out:    DimIn==0, ExposeSequence!=0
	*/
	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int TileCons=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;
	unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
	unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);
	int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;

	if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;
	if (Ctrl) {
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	if (ParFeat) TileCons = 8;

	int Din = UseIn?DimIn:0;
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState + DimState);
	LayerBandwidth = NCells*((DimState + DimIn) + (DimState + DimIn)*DimState*1 + (DimState)*BiasDataSize + DimState);

	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
		TileOrientation,
		CArgs(9,
		      (Dynamic)?		  TCArg(CNN_ArgDataType(4,0,0),		   "NCells"):AT_NO_C_ARG,
		      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sin"):AT_NO_C_ARG,
		      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sout"):AT_NO_C_ARG,
		      (UseIn)?			  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin"):AT_NO_C_ARG,
		      				  TCArg(CNN_ArgDataType(1,1,1),            "Wf"),
		      				  TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf"),
		      (ExposeSequence)?		  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout"):AT_NO_C_ARG,
		      				  TCArg(CNN_ArgDataType(1,1,1),            "Infos"),
		      (!AlwaysReset)?		  TCArg(CNN_ArgDataType(1,0,0),            "Reset"):AT_NO_C_ARG
		),
		Calls(1,
			Call(RNNKerName, LOC_LOOP,
				Bindings(14,
					(!(FirstSeq&&AlwaysReset))?K_Arg("Sin",  KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("Sout",  KER_ARG_TILE):Imm(0)),
					K_Arg("State",  KER_ARG_TILE),
					UseIn?K_Arg("Xin", KER_ARG_TILE):Imm(0),
					Imm(DimState), Imm(DimIn),
					K_Arg("Wf",  KER_ARG_TILE), K_Arg("Bf",  KER_ARG_TILE),
					(ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
					K_Arg("Bf", KER_ARG_TILE_H),
					K_Arg("Infos", KER_ARG_TILE),
					K_ArgPred("Wf", KER_ARG_TILEFIRST, D0),
					K_ArgPred("Wf", KER_ARG_TILEFIRST, T0),
					K_Arg("Bf", KER_ARG_TILE_BASE),
					AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset")
					)
			)
		),
		KerArgs(8,
			KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,      	      DimState+DimIn, 1,        FeatDataSize, 0, 0, 0, ""),
			(!(FirstSeq&&AlwaysReset))?
			KerArg("Sin",     KerArgSpace(1,T0),    Si_Attr|O_BUFF|O_NTILED,      DimState,       1,        FeatDataSize, 0, 0, 0, "Sin"):AT_NO_KER_ARG,
			(!(LastSeq&&AlwaysReset))?
			((!(FirstSeq&&AlwaysReset))?
		 KerArgAliased("Sout",    KerArgSpace(1,T0),3,  So_Attr|O_BUFF|O_NTILED,      DimState,       1,        FeatDataSize, 0, 0, 0, "Sout"):
			KerArg("Sout",    KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,      DimState,       1,        FeatDataSize, 0, 0, 0, "Sout")):AT_NO_KER_ARG,

			(UseIn)?
			KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                    DimIn,          1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Wf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST|Buffer,     DimIn+DimState, DimState, 1,            0, 0, TileCons, "Wf"):
			KerArg("Wf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST|Buffer,     DimIn+DimState, DimState, 1,            0, 0, TileCons, "Wf"),
			(PerCell)?
			KerArg("Bf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST|Buffer,     1,              DimState, BiasDataSize, 0, 0, 0, "Bf"):
			KerArg("Bf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST|Buffer,     1,              DimState, BiasDataSize, 0, 0, 0, "Bf"),
			(ExposeSequence)?
			KerArg("Hout",    KerArgSpace(2,RD0,T0),O_OUT|O_DB,                   1,              DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Infos",   KerArgSpace(2,RD0,T0),O_IN|O_BUFF|O_NTILED|O_CONST, RNN_CELL_INFOS, DimInfos, 1,            0, 0, 0, "Infos"):
			KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, RNN_CELL_INFOS, DimInfos, 1,            0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sin",   2, DimState, FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sout",  2, DimState, FeatDataSize);
		if (UseIn)			AddKernelArgDim(Name, "Xin",   3, NCells, DimIn, FeatDataSize);
						AddKernelArgDim(Name, "Wf",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bf",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Infos", 3, DimOut, RNN_CELL_INFOS*DimInfos, 1);
		if (ExposeSequence)		AddKernelArgDim(Name, "Hout",  3, NCells, DimState, FeatDataSize);
	}

	return Kernel;
}

static Kernel_T *RNN_Stack_Seq_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *RNNKerName,

	int BiasDataSize,
	int FeatDataSize,

	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int Buffer,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)
{
	Kernel_T *Ker = 0;

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Ker = RNN_Stack_Seq_SQ8_Internal(Name, Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, Buffer, FirstSeq, LastSeq, Revert, Dynamic);
	if (Ker) return Ker;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	printf("\n\n=============================== Solution not found for %s: Trying PARALLELFEATURES=0 ===============================\n\n", Name);
	/* If solution not found try with ParallelFeature = 0 */
	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;
    	CNN_SetGenCtrl(&InternalCtrl, "PARALLELFEATURES", AT_OPT_VAL(0));
    	Ker = RNN_Stack_Seq_SQ8_Internal(Name, &InternalCtrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, Buffer, FirstSeq, LastSeq, Revert, Dynamic);
    	return Ker;
}


int RNN_Stack_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

	int NCells,
	int K0,
	int K1,
	int DimState,
	int DimIn,
	int AlwaysReset,
	int Revert
	)

{
	int Log = 1;
	if (K0<1) GenTilingError("RNN_Stack_SQ8, %s, K0, At least one input is expected\n", Name, K0);
	if (K1<1) GenTilingError("RNN_Stack_SQ8, %s, K1, At least one output is expected\n", Name, K1);
	if (K0>NCells) GenTilingError("RNN_Stack_SQ8, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
	if (K1>NCells) GenTilingError("RNN_Stack_SQ8, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

	int UseHardAct = 1;
	int SameInStateScales = 1;
	if (Ctrl) {
		if (Ctrl->RNNUseHardActivation != -1) UseHardAct = Ctrl->RNNUseHardActivation;
		if (Ctrl->RNNSameInStateScales != -1) SameInStateScales = Ctrl->RNNSameInStateScales;
	}

	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;
	unsigned S_Attr = 0 | ((!AlwaysReset)?O_IN:0) | ((!AlwaysReset)?O_OUT:0);

	char *RNNKerName = ParFeat?(UseHardAct? \
				    (SameInStateScales?"RNN_ParKerB32_Hard_SameInStateScale_SQ8":"RNN_ParKerB32_Hard_SQ8"): \
				    (SameInStateScales?"RNN_ParKerB32_SameInStateScale_SQ8":"RNN_ParKerB32_SQ8") \
				   ): \
			   "RNN_KerB32_SQ8";
	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	/*
		If dynamic cell count we accept only
	       		All IN all OUT e.g NC=K0=K1 		=> N1=N3=0,  N2=NC => Single sequence
			All IN single OUT e.g NC=K0, K1=1	=> N1=NC-1,  N2=1  => Two sequences
	*/
	int Dynamic = 0;

	if (Ctrl) {
		if (Ctrl->DynamicIter) Dynamic = 1;
	}
	if (Log) {
		printf("RNN, %d Cells%s, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d, Order: %s\n",
			NCells, Dynamic?" Dynamic":"", DimState, DimIn, K0, K1, Revert?"Reverse":"Regular");
		printf("Basic Kernel: %s\n", RNNKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	if (Dynamic && !((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("RNN with dynamic cell count is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
	OpenKernelGroup(Name);
	if (Dynamic)
		UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));
	if (N1==0 && N3==0) {
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
		Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
					     O_BUFF, 1, 1, Revert, Dynamic);
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Ok==0) {
			if (Log) printf("Failed to map with all coeffs promoted to buffer, reverting to tile based\n");
			Ok = 1;
			Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
						     0, 1, 1, Revert, Dynamic);
		}
	} else {
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
		Kernel_T *Sol1=0, *Sol2=0, *Sol3=0;
		Kernel_T *PtSol1=0, *PtSol2=0, *PtSol3=0;
		if (N1>0) {
			Sol1 = RNN_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,
						 O_BUFF, 1, 0, Revert, Dynamic);
			if (Sol1==0) Ok=0; else PtSol1 = CopyAndPopUserKernel(Sol1);
		}
		if ((N2>0) && Ok) {
			Sol2 = RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
						 N1?(O_BUFF|O_NO_LOAD):O_BUFF, (N1==0), (N3==0), Revert, Dynamic && (N1==0));
			if (Sol2==0) Ok=0; else PtSol2 = CopyAndPopUserKernel(Sol2);
		}
		if ((N3>0) && Ok) {
			Sol3 = RNN_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,
						 O_BUFF|O_NO_LOAD, 0, 1, Revert, 0);
			if (Sol3==0) Ok=0; else PtSol3 = CopyAndPopUserKernel(Sol3);
		}
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Ok) {
			if (Log) printf("Map sequence with all coeffs promoted to buffer is successfull\n");
			if (Sol1) PushBackUserKernel(PtSol1); 
			if (Sol2) PushBackUserKernel(PtSol2); 
			if (Sol3) PushBackUserKernel(PtSol3); 
		} else {
			Ok = 1;
			if (Log) printf("Failed to map sequence with all coeffs promoted to buffer, reverting to tile based\n");
			if (N1>0) Ok = Ok && RNN_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,
						       	       	0, 1, 0, Revert, Dynamic);
			if (N2>0) Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
						       		0, (N1==0), (N3==0), Revert, Dynamic && (N1==0));
			if (N3>0) Ok = Ok && RNN_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,
						       		0, 0, 1, Revert, 0);
		}
	}
	CloseKernelGroupNoMerge();
	if (Ok==0) return 0;

	CKernel_Arg_T **GroupCArgs;
	CKernelCall_T **GroupCCalls;
	Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(5 + 2*(AlwaysReset==0) + (N1!=0) + (N2!=0 && N3!=0) + (Dynamic!=0));
	int A= 0;

	if (Dynamic) 		GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0), 	   "NCells");
	if (!AlwaysReset) 	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hinout");
	if (N1!=0) 	   	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G1O");
	if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G2O");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wf");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
	if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

	GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
	A=0;
	if (Dynamic) {
		if (N1>0) {
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(8,
							BindKGExpr("CArg(NCells)-1"),
							(!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							C_Arg("G1O"),
							BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
							C_Arg("Wf"), C_Arg("Bf"), C_Arg("Infos"), (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
		}
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(9, (N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
							    (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*N1*RNN_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
							    )
						);
	} else {
		if (N1>0) {
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(7,
							(!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							C_Arg("G1O"),
							Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
							C_Arg("Wf"), C_Arg("Bf"), C_Arg("Infos"), (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
		}
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(8, (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*N1*RNN_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
							    )
						);
		if (N2>0&&!N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(6, C_Arg("G1O"),
							    C_Arg("G2O"),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Infos", '+', PerCell*N1*RNN_CELL_INFOS),
							    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
							    )
						);
		if (N3>0)
			GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
						Bindings(7, (N2)?C_Arg("G2O"):C_Arg("G1O"),
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    KG_ArgOper("Wf",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Hout",  '+', N2_IO?(N2*DimState):0),
							    KG_ArgOper("Infos", '+', PerCell*(N1+N2)*RNN_CELL_INFOS),
							    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
							    )
						);
	}

	GroupKerArgs = AllocateKerArgs(5+ (Dynamic!=0) + (N1!=0) + (N2!=0 && N3!=0)+ 2*(AlwaysReset==0));
	A = 0;
	if (Dynamic)		GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,   	     1,                                  4,            "NCells");
	if (AlwaysReset==0) 	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
	if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O");
	if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O");
				GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,   	     DimIn*K0,                           FeatDataSize, "Xin");
				GroupKerArgs[A++] = KerGroupArg("Wf",     O_IN,   	     DimOut*(DimState + DimIn)*DimState, 1,            "Wf");
				GroupKerArgs[A++] = KerGroupArg("Bf",     O_IN,   	     DimOut*DimState,                    BiasDataSize, "Bf");
				GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,  	     DimState*K1,                        FeatDataSize, "Hout");
				GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,   	     DimInfos*DimOut*RNN_CELL_INFOS,     1,            "Infos");
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Reset",  O_IN,   	     1,                                  1,            "Reset");

	KernelGroup_T *UKGroup = UserKernelGroupK(
		Name,
		1,
		GroupCArgs,
		0,
		GroupCCalls,
		GroupKerArgs
	);
	return (UKGroup!=0);
}


static int LSTM_Stack_Seq_SQ8_Internal(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *LSTMKerName,

	int BiasDataSize,
	int FeatDataSize,

	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)

{
	/*      Sequences
		In:     DimIn!=0, ExposeSequence==0
		InOut:  DimIn!=0, ExposeSequence!=0
		None:   DimIn==0, ExposeSequence==0
		Out:    DimIn==0, ExposeSequence!=0
	*/
	int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;
	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int TileCons=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
	unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);

	if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;
	if (Ctrl) {
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	if (ParFeat) TileCons = 8;

	int Din = UseIn?DimIn:0;
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState*4 + DimState*4 + DimState*4);
	LayerBandwidth = NCells*((DimState + DimIn) + 4*(DimState + DimIn)*DimState*1 + 4*(DimState)*BiasDataSize + DimState);

	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
		TileOrientation,
		CArgs(17,
		      (Dynamic)?                  TCArg(CNN_ArgDataType(4,0,0),            "NCells"):AT_NO_C_ARG,
		      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataType(FeatDataSize,1,1), "SCin"):AT_NO_C_ARG,
		      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataType(FeatDataSize,1,1), "SHin"):AT_NO_C_ARG,
		      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "SCout"):AT_NO_C_ARG,
		      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "SHout"):AT_NO_C_ARG,
		      (UseIn)?  		  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin"):AT_NO_C_ARG,

		      				  TCArg(CNN_ArgDataType(1,1,1),            "Wf"),
		      				  TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf"),

		      				  TCArg(CNN_ArgDataType(1,1,1),            "Wi"),
		      				  TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bi"),

		      				  TCArg(CNN_ArgDataType(1,1,1),            "Wg"),
		      				  TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bg"),

		      				  TCArg(CNN_ArgDataType(1,1,1),            "Wo"),
		      				  TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bo"),

		      (ExposeSequence)?		  TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout"):AT_NO_C_ARG,
		      				  TCArg(CNN_ArgDataType(1,1,1),            "Infos"),
		      (AlwaysReset==0)?		  TCArg(CNN_ArgDataType(1,0,0),            "Reset"):AT_NO_C_ARG
		),
		Calls(1,
			Call(LSTMKerName, LOC_LOOP,
				Bindings(20,
					(!(FirstSeq&&AlwaysReset))?K_Arg("SCin",  KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("SCout",  KER_ARG_TILE):Imm(0)),
					K_Arg("State",  KER_ARG_TILE),
					UseIn?K_Arg("Xin",  KER_ARG_TILE):Imm(0),
					Imm(DimState), Imm(DimIn),
					K_Arg("Wf",  KER_ARG_TILE), K_Arg("Bf",  KER_ARG_TILE),
					K_Arg("Wi",  KER_ARG_TILE), K_Arg("Bi",  KER_ARG_TILE),
					K_Arg("Wg",  KER_ARG_TILE), K_Arg("Bg",  KER_ARG_TILE),
					K_Arg("Wo",  KER_ARG_TILE), K_Arg("Bo",  KER_ARG_TILE),
					(ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
					K_Arg("Bo", KER_ARG_TILE_H),
					K_Arg("Infos", KER_ARG_TILE),
					K_ArgPred("Wf", KER_ARG_TILEFIRST, D0),
					K_ArgPred("Wf", KER_ARG_TILEFIRST, T0),
					K_Arg("Bo",     KER_ARG_TILE_BASE),
					AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset")
					)
			)
		),
		KerArgs(16,
			KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,              		   2*DimState+DimIn, 1,        FeatDataSize, 0, 0, 0, ""),

			(!(FirstSeq&&AlwaysReset))?
			KerArg("SCin",    KerArgSpace(1,T0),    Si_Attr|O_BUFF|O_NTILED,   		   DimState,         1,        FeatDataSize, 0, 0, 0, "SCin"):AT_NO_KER_ARG,
			(!(FirstSeq&&AlwaysReset))?
			KerArg("SHin",    KerArgSpace(1,T0),    Si_Attr|O_BUFF|O_NTILED|O_STACK_PRED,      DimState,         1,        FeatDataSize, 0, 0, 0, "SHin"):AT_NO_KER_ARG,

			(!(LastSeq&&AlwaysReset))?
			((!(FirstSeq&&AlwaysReset))?
		 KerArgAliased("SCout",   KerArgSpace(1,T0), 1, So_Attr|O_BUFF|O_NTILED,	   	   DimState,         1,        FeatDataSize, 0, 0, 0, "SCout"):
			KerArg("SCout",   KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,	   	   DimState,         1,        FeatDataSize, 0, 0, 0, "SCout")):AT_NO_KER_ARG,

			(!(LastSeq&&AlwaysReset))?
			((!(FirstSeq&&AlwaysReset))?
		 KerArgAliased("SHout",   KerArgSpace(1,T0), 2, So_Attr|O_BUFF|O_NTILED,	   	   DimState,         1,        FeatDataSize, 0, 0, 0, "SHout"):
			KerArg("SHout",   KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,	   	   DimState,         1,        FeatDataSize, 0, 0, 0, "SHout")):AT_NO_KER_ARG,

			(UseIn)?
			KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                    		   DimIn,            1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Wf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, TileCons, "Wf"):
			KerArg("Wf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, TileCons, "Wf"),
			(PerCell)?
			KerArg("Bf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bf"):
			KerArg("Bf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bf"),
			(PerCell)?
			KerArg("Wi",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wi"):
			KerArg("Wi",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wi"),
			(PerCell)?
			KerArg("Bi",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bi"):
			KerArg("Bi",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bi"),
			(PerCell)?
			KerArg("Wg",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wg"):
			KerArg("Wg",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wg"),
			(PerCell)?
			KerArg("Bg",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bg"):
			KerArg("Bg",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bg"),
			(PerCell)?
			KerArg("Wo",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wo"):
			KerArg("Wo",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wo"),
			(PerCell)?
			KerArg("Bo",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bo"):
			KerArg("Bo",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   1,                DimState, BiasDataSize, 0, 0, 0, "Bo"),
			(ExposeSequence)?
			KerArg("Hout",    KerArgSpace(2,RD0,T0),O_OUT|O_DB,                   		   1,                DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Infos",   KerArgSpace(2,RD0,T0),O_IN|O_BUFF|O_NTILED|O_CONST, 		   LSTM_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos"):
			KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, 		   LSTM_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "SCin",  2, DimState, FeatDataSize);
		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHin",  2, DimState, FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "SCout", 2, DimState, FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHout", 2, DimState, FeatDataSize);
		if (UseIn)			AddKernelArgDim(Name, "Xin",   3, NCells, DimIn, FeatDataSize);
						AddKernelArgDim(Name, "Wf",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bf",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Wi",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bi",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Wg",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bg",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Wo",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bo",    3, DimOut, DimState, BiasDataSize);
		if (ExposeSequence)		AddKernelArgDim(Name, "Hout",  3, NCells, DimState, FeatDataSize);
						AddKernelArgDim(Name, "Infos", 3, DimOut, LSTM_CELL_INFOS*DimInfos, 1);
	}

	return (Kernel!=0);
}

static int LSTM_Stack_Seq_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *LSTMKerName,

	int BiasDataSize,
	int FeatDataSize,

	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)
{
	int Ker = 0;

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Ker = LSTM_Stack_Seq_SQ8_Internal(Name, Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, FirstSeq, LastSeq, Revert, Dynamic);
	if (Ker) return 1;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	printf("\n\n=============================== Solution not found for %s: Trying PARALLELFEATURES=0 ===============================\n\n", Name);
	/* If solution not found try with ParallelFeature = 0 */
	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;
    	CNN_SetGenCtrl(&InternalCtrl, "PARALLELFEATURES", AT_OPT_VAL(0));
    	Ker = LSTM_Stack_Seq_SQ8_Internal(Name, &InternalCtrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, FirstSeq, LastSeq, Revert, Dynamic);
    	return Ker;
}


int LSTM_Stack_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

	int NCells,
	int K0,
	int K1,
	int DimState,
	int DimIn,
	int AlwaysReset,
	int Revert

	)

{
	int Log = 1;

	if (K0<1) GenTilingError("LSTM_Stack_SQ8, %s, K0, At least one input is expected\n", Name, K0);
	if (K1<1) GenTilingError("LSTM_Stack_SQ8, %s, K1, At least one output is expected\n", Name, K1);
	if (K0>NCells) GenTilingError("LSTM_Stack_SQ8, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
	if (K1>NCells) GenTilingError("LSTM_Stack_SQ8, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;

	int UseHardAct = 1;
	int SameInStateScales = 1;
	if (Ctrl) {
		if (Ctrl->RNNUseHardActivation != -1) UseHardAct = Ctrl->RNNUseHardActivation;
		if (Ctrl->RNNSameInStateScales != -1) SameInStateScales = Ctrl->RNNSameInStateScales;
	}

	char *LSTMKerName = ParFeat?(UseHardAct? \
				     (SameInStateScales?"LSTM_ParKerB32_Hard_SameInStateScale_SQ8":"LSTM_ParKerB32_Hard_SQ8"): \
				     (SameInStateScales?"LSTM_ParKerB32_SameInStateScale_SQ8":"LSTM_ParKerB32_SQ8") \
				    ): \
				    "LSTM_KerB32_SQ8";
	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;
	int Dynamic = 0;

	if (Ctrl) {
		if (Ctrl->DynamicIter) Dynamic = 1;
	}
	if (Log) {
		printf("LSTM, %d Cells%s, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d\n",
			NCells, Dynamic?" Dynamic":"", DimState, DimIn, K0, K1);
		printf("Basic Kernel: %s\n", LSTMKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	if (Dynamic && !((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("LSTM with dynamic cell count is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
	OpenKernelGroup(Name);
	if (Dynamic) UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));

	if (N1>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,
						1, 0, Revert, Dynamic);
	if (N2>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
						(N1==0), (N3==0), Revert, Dynamic && (N1==0));
	if (N3>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,
						0, 1, Revert, 0);
	CloseKernelGroupNoMerge();
	if (Ok==0) return 0;

	CKernel_Arg_T **GroupCArgs;
	CKernelCall_T **GroupCCalls;
	Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(11 + 3*(AlwaysReset==0)+ 2*(N1!=0) + 2*(N2!=0 && N3!=0) + (Dynamic!=0));
	int A= 0;

	if (Dynamic)			GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0),            "NCells");
	if (!AlwaysReset)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Cinout");
	if (!AlwaysReset)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hinout");
	if (N1!=0)			GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G1O0");
	if (N1!=0)			GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G1O1");
	if (N2!=0 && N3!=0)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G2O0");
	if (N2!=0 && N3!=0)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G2O1");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wf");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wi");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bi");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wg");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bg");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wo");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bo");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout");
					GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
	if (AlwaysReset==0)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

	GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
	A=0;
	if (Dynamic) {
		if (N1>0)
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(16,BindKGExpr("CArg(NCells)-1"),
							    (!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    C_Arg("G1O0"), C_Arg("G1O1"),
							    BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
							    C_Arg("Wf"), C_Arg("Bf"),
							    C_Arg("Wi"), C_Arg("Bi"),
							    C_Arg("Wg"), C_Arg("Bg"),
							    C_Arg("Wo"), C_Arg("Bo"),
							    C_Arg("Infos"),
							    (!AlwaysReset)?C_Arg("Reset"):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(17,(N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
							    (N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
							    (N1)?C_Arg("G1O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wi",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bi",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wg",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bg",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wo",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bo",    '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*LSTM_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_IGNORE_ARG_BINDING
							    )
						);
	} else {
		if (N1>0)
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(15,(!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    C_Arg("G1O0"), C_Arg("G1O1"),
							    Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
							    C_Arg("Wf"), C_Arg("Bf"),
							    C_Arg("Wi"), C_Arg("Bi"),
							    C_Arg("Wg"), C_Arg("Bg"),
							    C_Arg("Wo"), C_Arg("Bo"),
							    C_Arg("Infos"),
							    (!AlwaysReset)?C_Arg("Reset"):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(16,(N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
							    (N1)?C_Arg("G1O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wi",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bi",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wg",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bg",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wo",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bo",    '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*LSTM_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N2>0&&!N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(14,C_Arg("G1O0"), C_Arg("G1O1"),
							    C_Arg("G2O0"), C_Arg("G2O1"),
							    KG_ArgOper("Wf",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wi",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bi",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wg",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bg",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wo",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bo",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*LSTM_CELL_INFOS),
							    (AlwaysReset==0)?Imm(0):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N3>0)
			GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
						Bindings(15,(N2)?C_Arg("G2O0"):C_Arg("G1O0"),
							    (N2)?C_Arg("G2O1"):C_Arg("G1O1"),
						       	    (!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    KG_ArgOper("Wf",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bf",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Wi",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bi",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Wg",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bg",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Wo",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bo",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Hout",  '+', N2_IO?N2*DimState:0),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*(N1+N2)*LSTM_CELL_INFOS),
							    (!AlwaysReset)?Imm(0):AT_IGNORE_ARG_BINDING
							    )
						);
	}

	GroupKerArgs = AllocateKerArgs(11 + (Dynamic!=0) + 3*(AlwaysReset==0) + 2*(N1!=0) + 2*(N2!=0 && N3!=0));
	A = 0;
	if (Dynamic)            GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,              1,                                  4,            "NCells");
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Cinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Cinout");
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
	if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O0",   O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O0");
	if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O1",   O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O1");
	
	if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O0",   O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O0");
	if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O1",   O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O1");
	
				GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,              DimIn*K0,                           FeatDataSize, "Xin");
				GroupKerArgs[A++] = KerGroupArg("Wf",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wf");
				GroupKerArgs[A++] = KerGroupArg("Bf",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bf");
				GroupKerArgs[A++] = KerGroupArg("Wi",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wi");
				GroupKerArgs[A++] = KerGroupArg("Bi",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bi");
				GroupKerArgs[A++] = KerGroupArg("Wg",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wg");
				GroupKerArgs[A++] = KerGroupArg("Bg",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bg");
				GroupKerArgs[A++] = KerGroupArg("Wo",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wo");
				GroupKerArgs[A++] = KerGroupArg("Bo",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bo");
				GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,             K1*DimState,                        FeatDataSize, "Hout");
				GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,              DimOut*DimInfos*LSTM_CELL_INFOS,    1,            "Infos");
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Reset",  O_IN,              1,                                  1,            "Reset");

	KernelGroup_T *UKGroup = UserKernelGroupK(
		Name,
		1,
		GroupCArgs,
		0,
		GroupCCalls,
		GroupKerArgs
	);
	return (UKGroup!=0);
}


static int GRU_Stack_Seq_SQ8_Internal(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *GRUKerName,

	int BiasDataSize,
	int FeatDataSize,

	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)

{
	/*      Sequences
		In:     DimIn!=0, ExposeSequence==0
		InOut:  DimIn!=0, ExposeSequence!=0
		None:   DimIn==0, ExposeSequence==0
		Out:    DimIn==0, ExposeSequence!=0
	*/
	int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;
	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;
	Tile_Orientation_T TileOrientation = TILE_HOR;
	int TileCons=0;
	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
	unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);

	if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;
	if (Ctrl) {
		if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
	}
	if (ParFeat) TileCons = 8;

	int Din = UseIn?DimIn:0;
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState*3 + DimState*3 + DimState*3);
	LayerBandwidth = NCells*((DimState + DimIn) + 3*(DimState + DimIn)*DimState*1 + 3*(DimState)*BiasDataSize + DimState);

	Kernel_T *Kernel = UserKernel(Name,
		KernelIterSpace(2, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
		TileOrientation,
		CArgs(14,
		      (Dynamic)?			TCArg(CNN_ArgDataType(4,0,0),            "NCells"):AT_NO_C_ARG,
		      (!(FirstSeq&&AlwaysReset))?	TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sin"):AT_NO_C_ARG,
		      (!(LastSeq&&AlwaysReset))?	TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sout"):AT_NO_C_ARG,
		      (UseIn)?				TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin"):AT_NO_C_ARG,

							TCArg(CNN_ArgDataType(1,1,1),            "Wr"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Br"),

							TCArg(CNN_ArgDataType(1,1,1),            "Wz"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bz"),

							TCArg(CNN_ArgDataType(1,1,1),            "Wh"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bwh"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Brh"),

		      (ExposeSequence)?			TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout"):AT_NO_C_ARG,
							TCArg(CNN_ArgDataType(1,1,1),            "Infos"),
		      (!AlwaysReset)?			TCArg(CNN_ArgDataType(1,0,0),            "Reset"):AT_NO_C_ARG
		),
		Calls(1,
			Call(GRUKerName, LOC_LOOP,
				Bindings(19,
					(!(FirstSeq&&AlwaysReset))?K_Arg("Sin", KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("Sout", KER_ARG_TILE):Imm(0)),
					K_Arg("State",  KER_ARG_TILE),
					UseIn?K_Arg("Xin",  KER_ARG_TILE):Imm(0),
					Imm(DimState), Imm(DimIn),
					K_Arg("Wr",  KER_ARG_TILE), K_Arg("Br",  KER_ARG_TILE),
					K_Arg("Wz",  KER_ARG_TILE), K_Arg("Bz",  KER_ARG_TILE),
					K_Arg("Wh",  KER_ARG_TILE), K_Arg("Bwh",  KER_ARG_TILE), K_Arg("Brh",  KER_ARG_TILE),
					(ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
					K_Arg("Bwh", KER_ARG_TILE_H),
					K_Arg("Infos", KER_ARG_TILE),
					K_ArgPred("Wr", KER_ARG_TILEFIRST, D0),
					K_ArgPred("Wr", KER_ARG_TILEFIRST, T0),
					K_Arg("Bwh",     KER_ARG_TILE_BASE),
					AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset")
					)
			)
		),
		KerArgs(13,
			KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,      		   DimState+DimIn,  1,        FeatDataSize, 0, 0, 0, ""),
			(!(FirstSeq&&AlwaysReset))?
			KerArg("Sin",     KerArgSpace(1,T0),    Si_Attr|O_BUFF|O_NTILED,           DimState,        1,        FeatDataSize, 0, 0, 0, "Sin"):AT_NO_KER_ARG,
			(!(LastSeq&&AlwaysReset))?
			((!(FirstSeq&&AlwaysReset))?
		 KerArgAliased("Sout",    KerArgSpace(1,T0),1,  So_Attr|O_BUFF|O_NTILED,           DimState,        1,        FeatDataSize, 0, 0, 0, "Sout"):
			KerArg("Sout",    KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,           DimState,        1,        FeatDataSize, 0, 0, 0, "Sout")):AT_NO_KER_ARG,
			(UseIn)?
			KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                         DimIn,           1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Wr",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, TileCons, "Wr"):
			KerArg("Wr",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, TileCons, "Wr"),
			(PerCell)?
			KerArg("Br",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Br"):
			KerArg("Br",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Br"),
			(PerCell)?
			KerArg("Wz",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, 0, "Wz"):
			KerArg("Wz",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, 0, "Wz"),
			(PerCell)?
			KerArg("Bz",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bz"):
			KerArg("Bz",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bz"),
			(PerCell)?
			KerArg("Wh",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, 0, "Wh"):
			KerArg("Wh",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, 0, "Wh"),
			(PerCell)?
			KerArg("Bwh",     KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bwh"):
			KerArg("Bwh",     KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bwh"),
			(PerCell)?
			KerArg("Brh",     KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Brh"):
			KerArg("Brh",     KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Brh"),
			(ExposeSequence)?
			KerArg("Hout",    KerArgSpace(2,RD0,T0),O_OUT|O_DB,                        1,               DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
			KerArg("Infos",   KerArgSpace(2,RD0,T0),O_IN|O_BUFF|O_NTILED|O_CONST,      GRU_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos"):
			KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,      GRU_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sin",   2, DimState, FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sout",  2, DimState, FeatDataSize);
		if (UseIn)			AddKernelArgDim(Name, "Xin",   3, NCells, DimIn, FeatDataSize);
						AddKernelArgDim(Name, "Wr",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Br",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Wz",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bz",    3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Wh",    4, DimOut, DimState, DimIn+DimState, 1);
						AddKernelArgDim(Name, "Bwh",   3, DimOut, DimState, BiasDataSize);
						AddKernelArgDim(Name, "Brh",   3, DimOut, DimState, BiasDataSize);
		if (ExposeSequence)		AddKernelArgDim(Name, "Hout",  3, NCells, DimState, FeatDataSize);
						AddKernelArgDim(Name, "Infos", 3, DimOut, GRU_CELL_INFOS*DimInfos, 1);
		if (Ctrl && (Ctrl->Filter_L3)) {
			SetKerArgInL3(Name, "Wr");
			SetKerArgInL3(Name, "Wz");
			SetKerArgInL3(Name, "Wh");
		}

	}

	return (Kernel!=0);
}

static int GRU_Stack_Seq_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *GRUKerName,

	int BiasDataSize,
	int FeatDataSize,

	int AlwaysReset,
	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int FirstSeq,
	int LastSeq,
	int Revert,
	int Dynamic
	)
{
	int Ker = 0;

	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Ker = GRU_Stack_Seq_SQ8_Internal(Name, Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, FirstSeq, LastSeq, Revert, Dynamic);
	if (Ker) return 1;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);

	printf("\n\n=============================== Solution not found for %s: Trying PARALLELFEATURES=0 ===============================\n\n", Name);
	/* If solution not found try with ParallelFeature = 0 */
	CNN_GenControl_T InternalCtrl;
	if (!Ctrl) CNN_InitGenCtrl(&InternalCtrl);
    	else 	   InternalCtrl = *Ctrl;
    	CNN_SetGenCtrl(&InternalCtrl, "PARALLELFEATURES", AT_OPT_VAL(0));
    	Ker = GRU_Stack_Seq_SQ8_Internal(Name, &InternalCtrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, NCells, DimState, DimIn, UseIn, ExposeSequence, FirstSeq, LastSeq, Revert, Dynamic);
    	return Ker;
}

int GRU_Stack_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,

	int BiasDataSize,
	int FeatDataSize,

	int NCells,
	int K0,
	int K1,
	int DimState,
	int DimIn,
	int AlwaysReset,
	int Revert
	)

{
	int Log = 1;

	if (K0<1) GenTilingError("GRU_Stack_SQ8, %s, K0, At least one input is expected\n", Name, K0);
	if (K1<1) GenTilingError("GRU_Stack_SQ8, %s, K1, At least one output is expected\n", Name, K1);
	if (K0>NCells) GenTilingError("GRU_Stack_SQ8, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
	if (K1>NCells) GenTilingError("GRU_Stack_SQ8, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;

	int UseHardAct = 1;
	int SameInStateScales = 1;
	if (Ctrl) {
		if (Ctrl->RNNUseHardActivation != -1)    UseHardAct = Ctrl->RNNUseHardActivation;
	}

	char *GRUKerName;
	GRUKerName = ParFeat?(UseHardAct?"GRU_ParKerB32_Hard_SQ8":"GRU_ParKerB32_SQ8"):"GRU_KerB32_SQ8";

	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);

	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	int Dynamic = 0;

	if (Ctrl) {
		if (Ctrl->DynamicIter) Dynamic = 1;
	}
	if (Log) {
		printf("GRU, %d Cells%s, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d\n",
			NCells, Dynamic?" Dynamic":"", DimState, DimIn, K0, K1);
		printf("Basic Kernel: %s\n", GRUKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	if (Dynamic && !((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("GRU with dynamic cell count is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
	OpenKernelGroup(Name);
	if (Dynamic) UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));

	if (N1>0) Ok = Ok && GRU_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,
		   			       1, 0, Revert, Dynamic);
	if (N2>0) Ok = Ok && GRU_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
					       (N1==0), (N3==0), Revert, Dynamic && (N1==0));
	if (N3>0) Ok = Ok && GRU_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,
					       0, 1, Revert, 0);
	CloseKernelGroupNoMerge();
	if (Ok==0) return 0;

	CKernel_Arg_T **GroupCArgs;
	CKernelCall_T **GroupCCalls;
	Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(10 + (N1!=0) + (N2!=0 && N3!=0) + 2*(AlwaysReset==0) + (Dynamic!=0));
	int A= 0;

	if (Dynamic)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0),            "NCells");
	if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hinout");
	if (N1!=0)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G1O");
	if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G2O");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Xin");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wr");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Br");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wz");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bz");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wh");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bwh");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Brh");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
	if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

	GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
	A=0;
	if (Dynamic) {
		if (N1>0)
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(12,BindKGExpr("CArg(NCells)-1"),
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    C_Arg("G1O"),
							    BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
							    C_Arg("Wr"), C_Arg("Br"),
							    C_Arg("Wz"), C_Arg("Bz"),
							    C_Arg("Wh"), C_Arg("Bwh"), C_Arg("Brh"),
							    C_Arg("Infos"),
							    (AlwaysReset==0)?C_Arg("Reset"):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(13,(N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
							    (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
							    KG_ArgOper("Wr",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Br",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wz",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bz",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wh",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bh",    '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*GRU_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
							)
					);
	} else {
		if (N1>0)
			GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
						Bindings(12,(!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    C_Arg("G1O"),
							    Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
							    C_Arg("Wr"), C_Arg("Br"),
							    C_Arg("Wz"), C_Arg("Bz"),
							    C_Arg("Wh"), C_Arg("Bwh"), C_Arg("Brh"),
							    C_Arg("Infos"),
							    (AlwaysReset==0)?C_Arg("Reset"):AT_IGNORE_ARG_BINDING
							    )
						);
		if (N2>0&&N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(13,(N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
							    Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
							    KG_ArgOper("Wr",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Br",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wz",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bz",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wh",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bwh",   '+', PerCell*N1*DimState),
							    KG_ArgOper("Brh",   '+', PerCell*N1*DimState),
							    C_Arg("Hout"),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*GRU_CELL_INFOS),
							    (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
							)
					);
		if (N2>0&&!N2_IO)
			GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
						Bindings(11,C_Arg("G1O"),
							    C_Arg("G2O"),
							    KG_ArgOper("Wr",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Br",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wz",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bz",    '+', PerCell*N1*DimState),
							    KG_ArgOper("Wh",    '+', PerCell*N1*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bwh",   '+', PerCell*N1*DimState),
							    KG_ArgOper("Brh",   '+', PerCell*N1*DimState),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*GRU_CELL_INFOS),
							    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
							)
					);
		if (N3>0)
			GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
						Bindings(12,(N2)?C_Arg("G2O"):C_Arg("G1O"),
							    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
							    KG_ArgOper("Wr",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Br",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Wz",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bz",    '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Wh",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
							    KG_ArgOper("Bwh",   '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Brh",   '+', PerCell*(N1+N2)*DimState),
							    KG_ArgOper("Hout",  '+', N2_IO?N2*DimState:0),
							    KG_ArgOper("Infos", '+', PerCell*DimInfos*(N1+N2)*GRU_CELL_INFOS),
							    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
							)
					);
	}

	GroupKerArgs = AllocateKerArgs(10 + (Dynamic!=0) + (N1!=0) + (N2!=0 && N3!=0) + 2*(AlwaysReset==0));
	A = 0;
	if (Dynamic)            GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,              1,                                  4,            "NCells");
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
	if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O");
	if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O");
				GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,              DimIn*K0,                           FeatDataSize, "Xin");
				GroupKerArgs[A++] = KerGroupArg("Wr",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wr");
				GroupKerArgs[A++] = KerGroupArg("Br",     O_IN,              DimOut*DimState,                    BiasDataSize, "Br");
				GroupKerArgs[A++] = KerGroupArg("Wz",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wz");
				GroupKerArgs[A++] = KerGroupArg("Bz",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bz");
				GroupKerArgs[A++] = KerGroupArg("Wh",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wh");
				GroupKerArgs[A++] = KerGroupArg("Bwh",    O_IN,              DimOut*DimState,                    BiasDataSize, "Bwh");
				GroupKerArgs[A++] = KerGroupArg("Brh",    O_IN,              DimOut*DimState,                    BiasDataSize, "Brh");
				GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,             DimState*K1,                        FeatDataSize, "Hout");
				GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,              DimOut*DimInfos*GRU_CELL_INFOS,     1,            "Infos");
	if (!AlwaysReset)       GroupKerArgs[A++] = KerGroupArg("Reset",  O_IN,              1,                                  1,            "Reset");

	KernelGroup_T *UKGroup = UserKernelGroupK(
		Name,
		1,
		GroupCArgs,
		0,
		GroupCCalls,
		GroupKerArgs
	);
	if (UKGroup) {
		if (Ctrl && (Ctrl->Filter_L3)) {
			SetKerArgInL3(Name, "Wr");
			SetKerArgInL3(Name, "Wz");
			SetKerArgInL3(Name, "Wh");
		}
	}
	return (UKGroup!=0);
}
