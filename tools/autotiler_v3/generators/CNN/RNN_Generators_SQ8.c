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
        LibKernelTemplate("KerRNN_SQ8_T",
                  CArgs(13,
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
			TCArg("char", "Reset")
		       )
        );
        LibKernel("RNN_KerB32_SQ8", CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);
        LibKernel("RNN_ParKerB32_SQ8", CALL_PARALLEL, 0, "KerRNN_SQ8_T", 0);

        LibKernelTemplate("KerLSTM_SQ8_T",
                  CArgs(19,
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
			TCArg("char", "Reset")
		       )
        );
        LibKernel("LSTM_KerB32_SQ8", CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);
        LibKernel("LSTM_ParKerB32_SQ8", CALL_PARALLEL, 0, "KerLSTM_SQ8_T", 0);


        LibKernelTemplate("KerGRU_SQ8_T",
                  CArgs(18,
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
			TCArg("int * __restrict__", "Bh"),
			TCArg("signed char *__restrict__", "Sbuff"),
			TCArg("signed char *__restrict__", "Hout"),
			TCArg("unsigned short int", "Nout"),
			TCArg("signed char *__restrict__", "Infos"),
			TCArg("char", "FirstCell"),
			TCArg("char", "FirstOut"),
			TCArg("char", "Reset")
		       )
	);
        LibKernel("GRU_KerB32_SQ8", CALL_PARALLEL, 0, "KerGRU_SQ8_T", 0);
        LibKernel("GRU_ParKerB32_SQ8", CALL_PARALLEL, 0, "KerGRU_SQ8_T", 0);

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

static int RNN_Stack_RevertedSeq_SQ8(
	char *Name,
	CNN_GenControl_T *Ctrl,
	char *RNNKerName,

	int NCells,
	int DimState,
	int DimIn,
	int UseIn,
	int ExposeSequence,
	int Buffer
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
	int BiasDataSize = 4;
	int TileCons=0;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
	int RD0 = SPACE_PROP(D0, SPACE_PROP_REVERT);


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
                KernelIterSpace(2, IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(7,
                      TCArg(CNN_ArgDataType(1,1,1),            "Hinout"),
		      (UseIn)?
                      TCArg(CNN_ArgDataType(1,1,1),            "Xin"):AT_NO_C_ARG,

                      TCArg(CNN_ArgDataType(1,1,1),            "Wf"),
                      TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf"),

		      (ExposeSequence)?
                      TCArg(CNN_ArgDataType(1,1,1),            "Hout"):AT_NO_C_ARG,
		      TCArg(CNN_ArgDataType(1,1,1),            "Infos"),
		      TCArg(CNN_ArgDataType(1,0,0),            "Reset")
                ),
		Calls(1,
			Call(RNNKerName, LOC_LOOP,
				Bindings(12,
					K_Arg("Hinout",  KER_ARG_TILE),
					K_Arg("State",  KER_ARG_TILE),
					UseIn?K_Arg("Xin", KER_ARG_TILE):Imm(0),
					Imm(DimState), Imm(DimIn),
					K_Arg("Wf",  KER_ARG_TILE), K_Arg("Bf",  KER_ARG_TILE),
					(ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
					K_Arg("Bf", KER_ARG_TILE_H),
					K_Arg("Infos", KER_ARG_TILE),
					K_ArgPredList(BIND_PRED_AND, 2, K_ArgPred("Wf", KER_ARG_TILEFIRST, T0), K_ArgPred("Wf", KER_ARG_TILEFIRST, D0)),
					C_Arg("Reset")
					)
			)
		),
		KerArgs(7,
                        KerArg("State",   KerArgSpace(1,T0),     O_BUFF|O_NTILED,      		    DimState+Din,   1,        1,            0, 0, 0, ""),
                        KerArg("Hinout",  KerArgSpace(1,T0),     O_IN|O_OUT|O_BUFF|O_NTILED,        DimState,       1,        1,            0, 0, 0, "Hinout"),
			(UseIn)?
                        KerArg("Xin",     KerArgSpace(1,RD0),    O_IN|O_DB,                         DimIn,          1,        1,            0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Wf",      KerArgSpace(2,D0,T0),  O_IN|O_DB|O_CONST|Buffer,          DimIn+DimState, DimState, 1,            0, 0, TileCons, "Wf"):
                        KerArg("Wf",      KerArgSpace(1,T0),     O_IN|O_DB|O_CONST|Buffer,          DimIn+DimState, DimState, 1,            0, 0, 0, "Wf"),
			(PerCell)?
                        KerArg("Bf",      KerArgSpace(2,D0,T0),  O_IN|O_DB|O_CONST|Buffer,          1,              DimState, BiasDataSize, 0, 0, 0, "Bf"):
                        KerArg("Bf",      KerArgSpace(1,T0),     O_IN|O_DB|O_CONST|Buffer,          1,              DimState, BiasDataSize, 0, 0, 0, "Bf"),
			(ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,RD0,T0), O_OUT|O_DB,                        1,              DimState, 1,            0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Infos",   KerArgSpace(2,RD0,T0), O_IN|O_BUFF|O_NTILED|O_CONST,      RNN_CELL_INFOS, DimInfos, 1,            0, 0, 0, "Infos"):
                        KerArg("Infos",   KerArgSpace(1,T0),     O_IN|O_BUFF|O_NTILED|O_CONST,      RNN_CELL_INFOS, DimInfos, 1,            0, 0, 0, "Infos")
		)
	);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKernelArgDim(Name, "Hinout", 2, DimState, 1);
		if (UseIn) AddKernelArgDim(Name, "Xin", 3, NCells, DimIn, 1);
		AddKernelArgDim(Name, "Wf",    4, DimOut, DimState, DimIn+DimState, 1);
		AddKernelArgDim(Name, "Bf",    3, DimOut, DimState, BiasDataSize);
		AddKernelArgDim(Name, "Infos", 3, DimOut, RNN_CELL_INFOS*DimInfos, 1);
		if (ExposeSequence) AddKernelArgDim(Name, "Hout",  3, NCells, DimState, 1);
	}

	return (Kernel!=0);
}

static int RNN_Stack_Seq_SQ8(
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
	int LastSeq
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
                KernelIterSpace(2, IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(8,
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
				Bindings(13,
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
                 KerArgAliased("Sout",    KerArgSpace(1,T0),1,  So_Attr|O_BUFF|O_NTILED,      DimState,       1,        FeatDataSize, 0, 0, 0, "Sout"):
                        KerArg("Sout",    KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,      DimState,       1,        FeatDataSize, 0, 0, 0, "Sout")):AT_NO_KER_ARG,

			(UseIn)?
                        KerArg("Xin",     KerArgSpace(1,D0),    O_IN|O_DB,                    DimIn,          1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Wf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST|Buffer,     DimIn+DimState, DimState, 1,            0, 0, TileCons, "Wf"):
                        KerArg("Wf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST|Buffer,     DimIn+DimState, DimState, 1,            0, 0, 0, "Wf"),
			(PerCell)?
                        KerArg("Bf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST|Buffer,     1,              DimState, BiasDataSize, 0, 0, 0, "Bf"):
                        KerArg("Bf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST|Buffer,     1,              DimState, BiasDataSize, 0, 0, 0, "Bf"),
			(ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,D0,T0), O_OUT|O_DB,                   1,              DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Infos",   KerArgSpace(2,D0,T0), O_IN|O_BUFF|O_NTILED|O_CONST, RNN_CELL_INFOS, DimInfos, 1,            0, 0, 0, "Infos"):
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

	return (Kernel!=0);
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
	int BiDir
	)

{
	int Log = 1;
	if (K0<1) GenTilingError("RNN_Stack_SQ8, %s, K0, At least one input is expected\n", Name, K0);
	if (K1<1) GenTilingError("RNN_Stack_SQ8, %s, K1, At least one output is expected\n", Name, K1);
	if (K0>NCells) GenTilingError("RNN_Stack_SQ8, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
	if (K1>NCells) GenTilingError("RNN_Stack_SQ8, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

	int ParFeat = 1;
	int PerCell = 0;
	int PerChanQuant = 0;
	unsigned S_Attr = 0 | ((!AlwaysReset)?O_IN:0) | ((!AlwaysReset)?O_OUT:0);

	char *RNNKerName = ParFeat?"RNN_ParKerB32_SQ8":"RNN_KerB32_SQ8";
	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	if (Log) {
		printf("RNN, %d Cells, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d\n", NCells, DimState, DimIn, K0, K1);
		printf("Basic Kernel: %s\n", RNNKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	OpenKernelGroup(Name);
	if (N1==0 && N3==0) {
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
		Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO, O_BUFF, 1, 1);
		AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		if (Ok==0) {
			if (Log) printf("Failed to map with all coeffs promoted to buffer, reverting to tile based\n");
			Ok = 1;
			Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO, 0, 1, 1);
		}
	} else {
		if (N1>0) Ok = Ok && RNN_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,     0, 1, 0);
		if (N2>0) Ok = Ok && RNN_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO, 0, (N1==0), (N3==0));
		if (N3>0) Ok = Ok && RNN_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, RNNKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,     0, 0, 1);
	}
	CloseKernelGroup();
	if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(5 + 2*(AlwaysReset==0) + (N1!=0) + (N2!=0 && N3!=0));
	int A= 0;

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
	if (N1>0) {
		GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
					Bindings(7,
						(!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
						C_Arg("G1O"),
						C_Arg("Xin"),
						C_Arg("Wf"), C_Arg("Bf"), C_Arg("Infos"), (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
	}
	if (N2>0&&N2_IO)
		GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
					Bindings(8, (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
						    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
						    KG_ArgOper("Xin",   '+', N1*DimIn),
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
						    KG_ArgOper("Hout",  '+', N2_IO?N2*DimState:0),
						    KG_ArgOper("Infos", '+', PerCell*(N1+N2)*RNN_CELL_INFOS),
						    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
						    )
					);

	GroupKerArgs = AllocateKerArgs(5+ (N1!=0) + (N2!=0 && N3!=0)+ 2*(AlwaysReset==0));
	A = 0;
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
	int LastSeq
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
                KernelIterSpace(2, IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(16,
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
				Bindings(19,
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
                        KerArg("Xin",     KerArgSpace(1,D0),    O_IN|O_DB,                    		   DimIn,            1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Wf",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, TileCons, "Wf"):
                        KerArg("Wf",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,            		   DimIn+DimState,   DimState, 1,            0, 0, 0, "Wf"),
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
                        KerArg("Hout",    KerArgSpace(2,D0,T0), O_OUT|O_DB,                   		   1,                DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Infos",   KerArgSpace(2,D0,T0), O_IN|O_BUFF|O_NTILED|O_CONST, 		   LSTM_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos"):
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
	int BiDir

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

	char *LSTMKerName = ParFeat?"LSTM_ParKerB32_SQ8":"LSTM_KerB32_SQ8";
	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);
	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	if (Log) {
		printf("LSTM, %d Cells, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d\n", NCells, DimState, DimIn, K0, K1);
		printf("Basic Kernel: %s\n", LSTMKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	OpenKernelGroup(Name);
	if (N1>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,     1, 0);
	if (N2>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO, (N1==0), (N3==0));
	if (N3>0) Ok = Ok && LSTM_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,     0, 1);
	CloseKernelGroup();
	if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(11 + 3*(AlwaysReset==0)+ 2*(N1!=0) + 2*(N2!=0 && N3!=0));
	int A= 0;

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
	if (N1>0)
		GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
					Bindings(15,(!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
						    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
						    C_Arg("G1O0"), C_Arg("G1O1"),
						    C_Arg("Xin"),
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
						    KG_ArgOper("Xin",   '+', N1*DimIn),
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

	GroupKerArgs = AllocateKerArgs(11 + 3*(AlwaysReset==0) + 2*(N1!=0) + 2*(N2!=0 && N3!=0));
	A = 0;
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
	int LastSeq
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
                KernelIterSpace(2, IterFixedSpace(D0, NCells), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(12,
                      (!(FirstSeq&&AlwaysReset))?	TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sin"):AT_NO_C_ARG,
                      (!(LastSeq&&AlwaysReset))?	TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Sout"):AT_NO_C_ARG,
		      (UseIn)?				TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Xin"):AT_NO_C_ARG,

							TCArg(CNN_ArgDataType(1,1,1),            "Wr"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Br"),

							TCArg(CNN_ArgDataType(1,1,1),            "Wz"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bz"),

							TCArg(CNN_ArgDataType(1,1,1),            "Wh"),
       							TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bh"),

		      (ExposeSequence)?			TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout"):AT_NO_C_ARG,
							TCArg(CNN_ArgDataType(1,1,1),            "Infos"),
		      (!AlwaysReset)?			TCArg(CNN_ArgDataType(1,0,0),            "Reset"):AT_NO_C_ARG
                ),
		Calls(1,
			Call(GRUKerName, LOC_LOOP,
				Bindings(18,
					(!(FirstSeq&&AlwaysReset))?K_Arg("Sin", KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("Sout", KER_ARG_TILE):Imm(0)),
					K_Arg("State",  KER_ARG_TILE),
					UseIn?K_Arg("Xin",  KER_ARG_TILE):Imm(0),
					Imm(DimState), Imm(DimIn),
					K_Arg("Wr",  KER_ARG_TILE), K_Arg("Br",  KER_ARG_TILE),
					K_Arg("Wz",  KER_ARG_TILE), K_Arg("Bz",  KER_ARG_TILE),
					K_Arg("Wh",  KER_ARG_TILE), K_Arg("Bh",  KER_ARG_TILE),
					K_Arg("Sbuff", KER_ARG_TILE),
					(ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
					K_Arg("Bh", KER_ARG_TILE_H),
					K_Arg("Infos", KER_ARG_TILE),
					K_ArgPred("Wr", KER_ARG_TILEFIRST, D0),
					K_ArgPred("Wr", KER_ARG_TILEFIRST, T0),
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
                        KerArg("Xin",     KerArgSpace(1,D0),    O_IN|O_DB,                         DimIn,           1,        FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Wr",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, TileCons, "Wr"):
                        KerArg("Wr",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 DimIn+DimState,  DimState, 1,            0, 0, 0, "Wr"),
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
                        KerArg("Bh",      KerArgSpace(2,D0,T0), O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bh"):
                        KerArg("Bh",      KerArgSpace(1,T0),    O_IN|O_DB|O_CONST,                 1,               DimState, BiasDataSize, 0, 0, 0, "Bh"),
                        KerArg("Sbuff",   KerArgSpace(2,D0,T0), O_BUFF|O_ONETILE,                  1,               DimState, BiasDataSize, 0, 0, 0, ""),
			(ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,D0,T0), O_OUT|O_DB,                        1,               DimState, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
			(PerCell)?
                        KerArg("Infos",   KerArgSpace(2,D0,T0), O_IN|O_BUFF|O_NTILED|O_CONST,      GRU_CELL_INFOS,  DimInfos, 1,            0, 0, 0, "Infos"):
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
						AddKernelArgDim(Name, "Bh",    3, DimOut, DimState, BiasDataSize);
		if (ExposeSequence)		AddKernelArgDim(Name, "Hout",  3, NCells, DimState, FeatDataSize);
						AddKernelArgDim(Name, "Infos", 3, DimOut, GRU_CELL_INFOS*DimInfos, 1);
	}

	return (Kernel!=0);
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
	int BiDir
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

	char *GRUKerName = ParFeat?"GRU_ParKerB32_SQ8":"GRU_KerB32_SQ8";
	char *G1_Name=0, *G2_Name=0, *G3_Name=0;
	int N1, N2, N3, N2_IO, Seq = RNN_Sequence(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);

	int DimOut = PerCell?NCells:1;
	int DimInfos = PerChanQuant?DimState:1;

	if (Log) {
		printf("GRU, %d Cells, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d\n", NCells, DimState, DimIn, K0, K1);
		printf("Basic Kernel: %s\n", GRUKerName);
		printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
	}
	int Ok = 1;
	OpenKernelGroup(Name);
	if (N1>0) Ok = Ok && GRU_Stack_Seq_SQ8(G1_Name = AppendNames(Name, "G1"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N1, DimState, DimIn, 1,     0,     1, 0);
	if (N2>0) Ok = Ok && GRU_Stack_Seq_SQ8(G2_Name = AppendNames(Name, "G2"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO, (N1==0), (N3==0));
	if (N3>0) Ok = Ok && GRU_Stack_Seq_SQ8(G3_Name = AppendNames(Name, "G3"), Ctrl, GRUKerName, BiasDataSize, FeatDataSize, AlwaysReset, N3, DimState, DimIn, 0,     1,     0, 1);
	CloseKernelGroup();
	if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;

	GroupCArgs = AllocateCArgs(9 + (N1!=0) + (N2!=0 && N3!=0) + 2*(AlwaysReset==0));
	int A= 0;

	if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hinout");
        if (N1!=0)		GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G1O");
        if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "G2O");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Xin");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wr");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Br");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wz");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bz");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Wh");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bh");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Hout");
				GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
	if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

	GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
	A=0;
	if (N1>0)
		GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
					Bindings(11,(!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
						    C_Arg("G1O"),
						    C_Arg("Xin"),
						    C_Arg("Wr"), C_Arg("Br"),
						    C_Arg("Wz"), C_Arg("Bz"),
						    C_Arg("Wh"), C_Arg("Bh"),
						    C_Arg("Infos"),
						    (AlwaysReset==0)?C_Arg("Reset"):AT_IGNORE_ARG_BINDING
						    )
					);
	if (N2>0&&N2_IO)
		GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
					Bindings(12,(N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                    (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
						    KG_ArgOper("Xin",   '+', N1*DimIn),
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
	if (N2>0&&!N2_IO)
		GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
					Bindings(10,C_Arg("G1O"),
                                                    C_Arg("G2O"),
						    KG_ArgOper("Wr",    '+', PerCell*N1*(DimState+DimIn)*DimState),
						    KG_ArgOper("Br",    '+', PerCell*N1*DimState),
						    KG_ArgOper("Wz",    '+', PerCell*N1*(DimState+DimIn)*DimState),
						    KG_ArgOper("Bz",    '+', PerCell*N1*DimState),
						    KG_ArgOper("Wh",    '+', PerCell*N1*(DimState+DimIn)*DimState),
						    KG_ArgOper("Bh",    '+', PerCell*N1*DimState),
						    KG_ArgOper("Infos", '+', PerCell*DimInfos*N1*GRU_CELL_INFOS),
						    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
						)
				);
	if (N3>0)
		GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
					Bindings(11,(N2)?C_Arg("G2O"):C_Arg("G1O"),
						    (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
						    KG_ArgOper("Wr",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
						    KG_ArgOper("Br",    '+', PerCell*(N1+N2)*DimState),
						    KG_ArgOper("Wz",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
						    KG_ArgOper("Bz",    '+', PerCell*(N1+N2)*DimState),
						    KG_ArgOper("Wh",    '+', PerCell*(N1+N2)*(DimState+DimIn)*DimState),
						    KG_ArgOper("Bh",    '+', PerCell*(N1+N2)*DimState),
						    KG_ArgOper("Hout",  '+', N2_IO?N2*DimState:0),
						    KG_ArgOper("Infos", '+', PerCell*DimInfos*(N1+N2)*GRU_CELL_INFOS),
						    (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
						)
				);

	GroupKerArgs = AllocateKerArgs(9 + (N1!=0) + (N2!=0 && N3!=0) + 2*(AlwaysReset==0));
	A = 0;
	if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
        if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O");
        if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O");
				GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,              DimIn*K0,                           FeatDataSize, "Xin");
				GroupKerArgs[A++] = KerGroupArg("Wr",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wr");
				GroupKerArgs[A++] = KerGroupArg("Br",     O_IN,              DimOut*DimState,                    BiasDataSize, "Br");
				GroupKerArgs[A++] = KerGroupArg("Wz",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wz");
				GroupKerArgs[A++] = KerGroupArg("Bz",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bz");
				GroupKerArgs[A++] = KerGroupArg("Wh",     O_IN,              DimOut*(DimState + DimIn)*DimState, 1,            "Wh");
				GroupKerArgs[A++] = KerGroupArg("Bh",     O_IN,              DimOut*DimState,                    BiasDataSize, "Bh");
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
	return (UKGroup!=0);
}
