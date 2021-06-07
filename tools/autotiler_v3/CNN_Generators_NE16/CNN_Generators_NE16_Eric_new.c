#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators_SQ8.h"
#include "CNN_Generator_Util.h"
#include "Gap.h"

#define MaxS(a, b) (((int)(a)>(int)(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
#define Abs(x) (((x)<0)?-(x):(x))

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

#define AT_INF_BIASL_SM         0
#define AT_INF_ACTSCALE         0
#define AT_INF_ACTSCALEN        1
#define AT_INF_A0               2
#define AT_INF_B0               3
#define AT_INF_C0               4

#define AT_INF_BIASN            5
#define AT_INF_IN1SCALE         5
#define AT_INF_SCALEN           5

#define AT_INF_IN1SCALEN        6
#define AT_INF_OUTSCALE         7
#define AT_INF_OUTSCALEN        8

#define AT_INF_DIM              9

void LoadCNN_NE16_SQ8_Library()

{
        LibKernelTemplate("_KerSetBias_SQ8_T",
                  CArgs(6,
                        TCArg("int * __restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("void * __restrict__", "Bias"),
                        TCArg("unsigned char", "NormBias")
                        )
        );
        LibKernelTemplate("_KerConv_SQ8_T",
                  CArgs(22,
                        TCArg("signed char * __restrict__", "In"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "UsedW"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned short int", "UsedH"),
                        TCArg("unsigned short int", "WFilt"),
                        TCArg("unsigned short int", "HFilt"),
                        TCArg("unsigned short int", "InFeatures"),
                        TCArg("unsigned short int", "OutFeatures"),
                        TCArg("unsigned short int", "TotalInFeatures"),
                        TCArg("signed char * __restrict__", "Filter"),
                        TCArg("signed char * __restrict__", "Bias"),
                        TCArg("signed char * __restrict__", "Out"),
                        TCArg("v4s", "Pad"),
                        TCArg("unsigned char", "NormBias"),
                        TCArg("unsigned char", "Orientation"),
                        TCArg("unsigned char", "N"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "Ny"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
                        )
        );
        LibKernelTemplate("_KerConvDP_SQ8_T",
                  CArgs(22,
                        TCArg("signed char * __restrict__", "In"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "UsedW"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned short int", "UsedH"),
                        TCArg("unsigned short int", "WFilt"),
                        TCArg("unsigned short int", "HFilt"),
                        TCArg("unsigned short int", "InFeatures"),
                        TCArg("unsigned short int", "OutFeatures"),
                        TCArg("unsigned short int", "TotalInFeatures"),
                        TCArg("signed char * __restrict__", "Filter"),
                        TCArg("signed char * __restrict__", "Bias"),
                        TCArg("int * __restrict__", "Out"),
                        TCArg("v4s", "Pad"),
                        TCArg("unsigned char", "NormBias"),
                        TCArg("unsigned char", "Orientation"),
                        TCArg("unsigned char", "N"),
                        TCArg("unsigned char", "S"),
                        TCArg("unsigned char", "D"),
                        TCArg("unsigned char", "Ny"),
                        TCArg("unsigned char", "Sy"),
                        TCArg("unsigned char", "Dy")
                        )
        );
        LibKernelTemplate("_KerConvLinReduct_SQ8_T",
                  CArgs(8,
                        TCArg("int *__restrict__", "In"),
                        TCArg("void *__restrict__", "Out"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("unsigned char *__restrict__", "Scale"),
                        TCArg("unsigned char *__restrict__", "ScaleN"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("_KerActivation_HWC_SQ8_T",
                  CArgs(6,
                        TCArg("void *__restrict__", "In"),
                        TCArg("void *__restrict__", "Out"),
                        TCArg("unsigned short int", "Feat"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                       )
        );


        LibKernel("MySetBiasKer", CALL_PARALLEL|CALL_HWC_KER, 0, "_KerSetBias_SQ8_T",       	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));
	LibKernel("MyConvKerDP",  CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConvDP_SQ8_T",         	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));
	LibKernel("MyConvKerDWDP",CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConvDP_SQ8_T",         	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));
	LibKernel("MyConvKer",    CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConv_SQ8_T",         	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(1,1,1,0,1), -1,-1,-1,-1,-1,-1));
	LibKernel("MyConvKerDW",  CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConv_SQ8_T",         	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(1,1,1,0,1), -1,-1,-1,-1,-1,-1));
	LibKernel("MyConvKerPW",  CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConv_SQ8_T",         	CNN_Match(CNN_OperList(1, KOP_NONE), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));
	LibKernel("MyReductKer",  CALL_PARALLEL|CALL_HWC_KER, 0, "_KerConvLinReduct_SQ8_T",	CNN_Match(CNN_OperList(1, KOP_NONE), CNN_OperList(1, KOP_NONE), 1,
                                                                                                  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_ReLU_HWC_SQ8",                        CALL_PARALLEL|CALL_HWC_KER, 0, "_KerActivation_HWC_SQ8_T",
                CNN_Match(CNN_OperList(1, KOP_RELU),      0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_ReLUN_HWC_SQ8",                       CALL_PARALLEL|CALL_HWC_KER, 0, "_KerActivation_HWC_SQ8_T",
                CNN_Match(CNN_OperList(1, KOP_RELUN),     0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_HSigmoid_HWC_SQ8",                    CALL_PARALLEL|CALL_HWC_KER, 0, "_KerActivation_HWC_SQ8_T",
                CNN_Match(CNN_OperList(1, KOP_HSIGMOID),  0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_HSwish_HWC_SQ8",                      CALL_PARALLEL|CALL_HWC_KER, 0, "_KerActivation_HWC_SQ8_T",
                CNN_Match(CNN_OperList(1, KOP_HSWISH),    0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));
        LibKernel("KerPar_LeakyReLU_HWC_SQ8",                   CALL_PARALLEL|CALL_HWC_KER, 0, "_KerActivation_HWC_SQ8_T",
                CNN_Match(CNN_OperList(1, KOP_LEAKYRELU), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,0,0));

        /*
        LibKernel("MySetBiasKer", CALL_PARALLEL, 0, "_KerSetBias_SQ8_T",                CNN_Match(CNN_OperList(1, KOP_SETBIAS), 0, 1, CNN_Type(4,0,0,0,4), 0,0,0,0,0,0));
	LibKernel("MyConvKer", CALL_PARALLEL, 0, "_KerConv_SQ8_T",         		CNN_Match(CNN_OperList(1, KOP_CONV), 0, 1, CNN_Type(1,1,1,0,4), -1,-1,-1,-1,-1,-1));
	LibKernel("MyReductKer", CALL_PARALLEL, 0, "_KerConvLinReduct_SQ8_T",           CNN_Match(CNN_OperList(1, KOP_DP_REDUCT), CNN_OperList(1, KOP_NONE), 1,
                                                                                                  CNN_Type(4,0,0,0,1), 0,0,0,0,0,0));
	*/


}

Kernel_T *CNN_MergedConvolutionNE16_Internal(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int Bias1_DataSize,
        int Bias2_DataSize,
	int Filter1_DataSize_Bit,
	int Filter2_DataSize_Bit,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ConvOper1,
        int Fcx,
        int Fcy,
        int Dcx,
        int Dcy,
        int Scx,
        int Scy,
        int Conv1Pad,
        KernelOper_T ActOper1,

        KernelOper_T ConvOper2,
        int Fpx,
        int Fpy,
        int Dpx,
        int Dpy,
        int Spx,
        int Spy,
        int Conv2Pad,
        KernelOper_T ActOper2,

	unsigned int TileCons
        )

{
	unsigned int InTileCons = InFeat;
	int DW_PW;
	if (ConvOper1 == KOP_CONV_DW && ConvOper2 == KOP_CONV) DW_PW = 1;
	else if (ConvOper1 == KOP_CONV && ConvOper2 == KOP_CONV_DW) DW_PW = 0;
	else GenTilingError("CNN_MergedConvolutionNE16: Point wise convolution followed by depth wise convolution or depth wise convolution followed by point wise convolution only");

        int Scale_DataSize = 1;
	Tile_Orientation_T TileOrientation = TILE_HOR;
        unsigned int Wa1 = 0, Wa2 = 0;

	unsigned int Ws1 = Filter1_DataSize_Bit, Ws2 = Filter2_DataSize_Bit;

        if (((Ws1%8)!=0)) Wa1 = O_BIT; else Ws1 = Ws1/8;
        if (((Ws2%8)!=0)) Wa2 = O_BIT; else Ws2 = Ws2/8;
        unsigned int Wp = SPACE_PROP_PAD2PREF;
        unsigned int Bs1 = Bias1_DataSize, Bs2 = Bias2_DataSize;
        int OverlapC1, OverlapC2;
        int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp = (v4s){0,0,0,0}, PadInc = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
        char *Conv1KerName=0, *Conv2KerName=0, *Act1KerName=0, *Act2KerName=0;
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int UsedWidth, UsedHeight, UsedWc, UsedHc;
        int OutTileCons = 1;
        int StandAloneAct = (ActOper2!=KOP_NONE);
	AT_PadType PadType = PAD_BALANCED_LEFT;
        CNN_LayerOutputDim(Width, Height, ConvOper1, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Conv1Pad, ConvOper2, Fpx, Fpy, Dpx, Dpy, Spx, Spy, Conv2Pad,
                           &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
        PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
        PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

        CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC1, &OverlapC2);
        UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
        UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
        UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);

        // TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);

        /* Re evaluate now that we know exactly what is used */
        PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
        PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
        UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
        UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];

        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*InFeat + Wo*Ho*Fpx*Fpy*InFeat*OutFeat;
        if (ActOper2) LayerOp += OutFeat*Wo*Ho;

	Conv1KerName = DW_PW?"MyConvKerDW":"MyConvKerPW";
	Conv2KerName = DW_PW?"MyConvKerPW":"MyConvKerDW";

        LayerBandwidth += Width*Height*1*InFeat + Wc*Hc*1*InFeat*OutFeat;
        LayerBandwidth += (Fcx*Fcy*Filter1_DataSize_Bit*InFeat         + 7)/8;
        LayerBandwidth += (Fpx*Fpy*Filter2_DataSize_Bit*InFeat*OutFeat + 7)/8;
        LayerBandwidth += Bias1_DataSize*InFeat + Bias2_DataSize*OutFeat;

        if (Log) {
                printf("InFeat: %d, OutFeat: %d", InFeat, OutFeat);
		if (Wa1==O_BIT) printf(", Filter1 in Bits %d bits", Ws1);
		if (Wa2==O_BIT) printf(", Filter2 in Bits %d bits", Ws2);
	       	printf("\n");
                printf("Conv1 => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
                printf("      => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("Conv2 => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
                printf("      => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC1: %d\n", OverlapC1);
                printf("OverlapC2: %d\n", OverlapC2);
                printf("TileCons : %d\n", TileCons);
                printf("UsedIn   : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC    : [%d x %d]\n", UsedWc, UsedHc);
                if (Conv1KerName) printf("%20s: %s\n", "Conv1KerName", Conv1KerName);
                if (Conv2KerName) printf("%20s: %s\n", "Conv2KerName", Conv2KerName);
                if (Act1KerName) printf("%20s: %s\n", "Act1KerName", Act1KerName);
                if (Act2KerName) printf("%20s: %s\n", "Act2KerName", Act2KerName);
                printf("Nb Oper : %lld\n", LayerOp);
        }

        CKernel_Arg_T **KCArgs = AllocateCArgs(12);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1),      "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1), "Filter1");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias1_DataSize,1,1),   "Bias1");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,          1,1),  "Scale1");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,          1,1), "Scale1N");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1),  "Infos1");

	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1), "Filter2");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias2_DataSize,1,1),   "Bias2");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1),     "Out");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,          1,1),  "Scale2");
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,          1,1), "Scale2N");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,             1,1),  "Infos2");

	Object_T **KArgs = AllocateKerArgs(13);
        int Ka=0;
	if (DW_PW) {
	        KArgs[Ka++] = KerArgP("In",      KerArgSpace(2,T0,D0),    O_IN|O_DB,         Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC1, 0, TileCons, "In");
	        KArgs[Ka++] = KerArg ("Bias1",   KerArgSpace(1,D0),       O_IN|O_DB|O_CONST,     1,      1,                                         Bs1,         0, 0,        0, "Bias1");
	        KArgs[Ka++] = KerArg ("Scale1",  KerArgSpace(1,D0),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale1");
	        KArgs[Ka++] = KerArg ("Scale1N", KerArgSpace(1,D0),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale1N");
	        KArgs[Ka++] = KerArg ("Filter1", KerArgSpace(1,D0|Wp),    O_IN|O_DB|O_CONST|Wa1, 1,      1,                                         Fcx*Fcy*Ws1, 0, 0,        0, "Filter1");
	        KArgs[Ka++] = KerArgP("In2",     KerArgSpace(2,T0,D0),    O_BUFF|O_ONETILE,     Wc,     Hc,    UsedWc,     UsedHc,  PadInp, PadInp, 1,   OverlapC2, 0,        0, "");
	        KArgs[Ka++] = KerArg ("Bias2",   KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         Bs2,         0, 0,        0, "Bias2");
	        KArgs[Ka++] = KerArg ("Scale2",  KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale2");
	        KArgs[Ka++] = KerArg ("Scale2N", KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale2N");
	        KArgs[Ka++] = KerArg ("Filter2", KerArgSpace(2,D1,D0|Wp), O_IN|O_DB|O_CONST|Wa2, 1,      1,                                         Fpx*Fpy*Ws2, 0, 0,        0, "Filter2");
	        KArgs[Ka++] = KerArg ("Out",     KerArgSpace(2,T0,D1),    O_OUT|O_DB,           Wo,     Ho,                                         1,           0, 0,        0, "Out");
	        KArgs[Ka++] = KerArg ("Infos1",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                          1,           0, 0,        0, "Infos1");
	        KArgs[Ka++] = KerArg ("Infos2",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                          1,           0, 0,        0, "Infos2");
	} else {
	        KArgs[Ka++] = KerArgP("In",      KerArgSpace(2,T0,D0),    O_IN|O_DB,         Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC1, 0, TileCons, "In");
	        KArgs[Ka++] = KerArg ("Bias1",   KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         Bs1,         0, 0,        0, "Bias1");
	        KArgs[Ka++] = KerArg ("Scale1",  KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale1");
	        KArgs[Ka++] = KerArg ("Scale1N", KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale1N");
	        KArgs[Ka++] = KerArg ("Filter1", KerArgSpace(2,D1,D0|Wp), O_IN|O_DB|O_CONST|Wa1, 1,      1,                                         Fcx*Fcy*Ws1, 0, 0,        0, "Filter1");
	        KArgs[Ka++] = KerArgP("In2",     KerArgSpace(2,T0,D1),    O_BUFF|O_ONETILE,     Wc,     Hc,    UsedWc,     UsedHc,  PadInp, PadInp, 1,   OverlapC2, 0,        0, "");
	        KArgs[Ka++] = KerArg ("Bias2",   KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         Bs2,         0, 0,        0, "Bias2");
	        KArgs[Ka++] = KerArg ("Scale2",  KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale2");
	        KArgs[Ka++] = KerArg ("Scale2N", KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,     1,      1,                                         1,           0, 0,        0, "Scale2N");
	        KArgs[Ka++] = KerArg ("Filter2", KerArgSpace(1,D1|Wp), 	  O_IN|O_DB|O_CONST|Wa2, 1,      1,                                         Fpx*Fpy*Ws2, 0, 0,        0, "Filter2");
	        KArgs[Ka++] = KerArg ("Out",     KerArgSpace(2,T0,D1),    O_OUT|O_DB,           Wo,     Ho,                                         1,           0, 0,        0, "Out");
	        KArgs[Ka++] = KerArg ("Infos1",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                          1,           0, 0,        0, "Infos1");
	        KArgs[Ka++] = KerArg ("Infos2",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                          1,           0, 0,        0, "Infos2");
	}


	Kernel_T *Kernel;
	Kernel = UserKernel(
			Name,
	                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons)),
	                TILE_HOR|TILE_HWC,
	                KCArgs,
			Calls(4,
				Call(Conv1KerName, LOC_D0,
	                                Bindings(22,
	                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
	                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
	                                        K_Arg("In", KER_ARG_TILE_USEDW),                                        /* Conv input tile width, used part of it */
	                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
	                                        K_Arg("In", KER_ARG_TILE_USEDH),                                        /* Conv input tile height, used part of it */
	                                        K_Arg("Filter1", KER_ARG_TILE_W),                                       /* Filter input tile width */
	                                        K_Arg("Filter1", KER_ARG_TILE_H),                                       /* Filter input tile height */
	                                        K_ArgPar("Filter1", KER_ARG_PARTILE_SIZE, D0),                          /* Number of input features in this tile */
	                                        K_ArgPar("In2", KER_ARG_PARTILE_SIZE, DW_PW?D0:D1),			/* Number of output features in this tile */
	                                        K_ArgPar("Filter1", KER_ARG_LOADEDPARTILE_SIZE, D0),                    /* Total number of input features currently in L1 memory, argument promotion */
	                                        K_Arg("Filter1", KER_ARG_TILE),                                         /* Conv filter */
	                                        K_Arg("Bias1", KER_ARG_TILE),               				/* Conv Bias when depth wise conv*/
	                                        K_Arg("In2", KER_ARG_TILE),                                         	/* Conv output */
	                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
	                                        K_TileOper("Infos1", "char *", '@', AT_INF_BIASN),  			/* NormBias is depth wise conv */
	                                        AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
	                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Fx */
	                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridex */
	                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Dx */
	                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Conv Fy */
	                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridey */
	                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Conv Dy */
	                                        )
				),
				(Act1KerName==0)?AT_NO_CALL:
				Call(Act1KerName, LOC_D0,
					Bindings(6,
						K_Arg("In2", KER_ARG_TILE),						/* Input tile */
						K_Arg("In2", KER_ARG_TILE),						/* Output tile */
	                                        K_ArgPar("In2", KER_ARG_PARTILE_SIZE, DW_PW?D0:D1),			/* Number of input/output features in this tile */
	                                        K_Arg("In2", KER_ARG_TILE_W),                                           /* Input tile width */
	                                        K_Arg("In2", KER_ARG_TILE_H),                                           /* Output tile width */
						K_Arg("Infos1", KER_ARG_TILE)						/* Infos */
					)
			     	),
				Call(Conv2KerName, LOC_D0,
	                                Bindings(22,
	                                        K_Arg("In2", KER_ARG_TILE),                                             /* Conv input tile */
	                                        K_Arg("In2", KER_ARG_TILE_W),                                           /* Conv input tile width */
	                                        K_Arg("In2", KER_ARG_TILE_USEDW),                                       /* Conv input tile width, used part of it */
	                                        K_Arg("In2", KER_ARG_TILE_H),                                           /* Conv input tile height */
	                                        K_Arg("In2", KER_ARG_TILE_USEDH),                                       /* Conv input tile height, used part of it */
	                                        K_Arg("Filter2", KER_ARG_TILE_W),                                   	/* Filter input tile width */
	                                        K_Arg("Filter2", KER_ARG_TILE_H),                                       /* Filter input tile height */
	                                        K_ArgPar("Filter2", KER_ARG_PARTILE_SIZE, DW_PW?D0:D1),                 /* Number of input features in this tile */
	                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D1),				/* Number of output features in this tile */
	                                        K_ArgPar("Filter2", KER_ARG_LOADEDPARTILE_SIZE, DW_PW?D0:D1),           /* Total number of input features currently in L1 memory, argument promotion */
	                                        K_Arg("Filter2", KER_ARG_TILE),                                         /* Conv filter */
	                                        K_Arg("Bias2", KER_ARG_TILE),               				/* Conv Bias when depth wise conv*/
	                                        K_Arg("Out", KER_ARG_TILE),                                         	/* Conv output */
	                                        K_Arg("In2", KER_ARG_TILE_PAD),                                         /* Conv Padding */
	                                        AT_IGNORE_ARG_BINDING, 							/* Useless here (point wise), NormBias is depth wise conv */
	                                        AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
	                                        NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,                                 /* Conv Fx */
	                                        NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridex */
	                                        NeedDpx?Imm(Dpx):AT_IGNORE_ARG_BINDING,                                 /* Conv Dx */
	                                        NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,                                 /* Conv Fy */
	                                        NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridey */
	                                        NeedDpy?Imm(Dpy):AT_IGNORE_ARG_BINDING                                  /* Conv Dy */
	                                        )
				),
				(Act2KerName==0)?AT_NO_CALL:
				Call(Act2KerName, LOC_D0,
					Bindings(6,
						K_Arg("Out", KER_ARG_TILE),						/* Input tile */
						K_Arg("Out", KER_ARG_TILE),						/* Output tile */
	                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D1),				/* Number of input/output features in this tile */
	                                        K_Arg("In2", KER_ARG_TILE_W),                                           /* Input tile width */
	                                        K_Arg("In2", KER_ARG_TILE_H),                                           /* Output tile width */
						K_Arg("Infos2", KER_ARG_TILE)						/* Infos */
					)
			     	)
			),
			KArgs
	        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                AddKernelArgDim(Name, "Filter1", 4, InFeat, Fcx, Fcy, Filter1_DataSize_Bit);
                AddKernelArgDim(Name, "Filter2", 5, OutFeat, InFeat, Fpx, Fpy, Filter2_DataSize_Bit);
                AddKernelArgDim(Name, "Bias1", 2, InFeat, Bias1_DataSize);
                AddKernelArgDim(Name, "Bias2", 2, OutFeat, Bias2_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 1);
                AddKernelArgDim(Name, "Scale1", 2, InFeat, 1);
                AddKernelArgDim(Name, "Scale1N", 2, InFeat, 1);
                AddKernelArgDim(Name, "Scale2", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Scale2N", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos1", 2, AT_INF_DIM, 1);
                AddKernelArgDim(Name, "Infos2", 2, AT_INF_DIM, 1);

        }
        return Kernel;
}

int CNN_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int Bias_DataSize,
        int Scale_DataSize,
	int Filter_DataSize,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ConvOper,
        int Fcx,
        int Fcy,
        int Dcx,
        int Dcy,
        int Scx,
        int Scy,
        int ConvPad,

        KernelOper_T PoolOper,
        int Fpx,
        int Fpy,
        int Dpx,
        int Dpy,
        int Spx,
        int Spy,
        int PoolPad,

        KernelOper_T ActOper
        )

{
        int ParFeat = 1;
	int B = 0, Bs=1;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        AT_PadType PadType = PAD_BALANCED_LEFT;
        if (PoolOper==KOP_NONE) {
                Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
        }
        if (Ctrl) {
                if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
        }
	if (Filter_DataSize<0) {
		B = O_BIT; Bs = -Filter_DataSize;
	} else Bs = Filter_DataSize;
        int OverlapC, OverlapP;
        int TileCons;
        int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
        char *ConvKerName=0, *PoolKerName=0, *ActKerName=0, *SetBiasKerName=0, *DPReductionKerName=0;
        int DWConv=(ConvOper==KOP_CONV_DW);
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int Os=(DWConv?D0:D1);
        int UsedWidth, UsedHeight, UsedWc, UsedHc;
        int InTileCons = 16; // 16;
        int OutTileCons = 1; // 8;
        int StandAloneAct = (ActOper!=KOP_NONE);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;


	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
                           &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
        PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
        PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

        CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC, &OverlapP);
        UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
        UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
        UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);
        TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);

        /* Re evaluate now that we know exactly what is used */
        PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
        PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
        UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
        UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
        Wc = UsedWc; Hc = UsedHc;

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
        if (!DWConv) LayerOp *= InFeat;
        if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += OutFeat*Wo*Ho;
        LayerBandwidth += Width*Height*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Wo*Ho*1*OutFeat;
        LayerBandwidth += Fcx*Fcy*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Bias_DataSize*OutFeat;

	SetBiasKerName = "MySetBiasKer";
	ConvKerName = "MyConvKer";
	DPReductionKerName = "MyReductKer";

        if (Log) {
                printf("InFeat: %d, OutFeat: %d", InFeat, OutFeat);
		if (B==O_BIT) printf(", Filter in Bits %d bits\n", Bs); else printf("\n");
                printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
                printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
                printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC: %d\n", OverlapC);
                printf("OverlapP: %d\n", OverlapP);
                printf("TileCons: %d\n", TileCons);
                printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
                if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
                if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
                if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
                printf("Nb Oper : %lld\n", LayerOp);
        }

        /* User kernel C arguments */
        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	Kernel_T *Kernel;
	int OrgTileCons=0;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	int MinTile = 3;
	for (int Iter=0; Iter<2; Iter++) {
	        int Ca=0;
	
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),     "In");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1), "Filter");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),    "Out");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");
	
	        /* User kernel kernel arguments */
	        Object_T **KArgs = AllocateKerArgs(8);
	        int Ka=0;
	        KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,     Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,      OverlapC, 0, TileCons, "In");
	        KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         Bias_DataSize, 0, 0,        0, "Bias");
	        KArgs[Ka++] = KerArg ("Scale",  KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         1,             0, 0,        0, "Scale");
	        KArgs[Ka++] = KerArg ("ScaleN", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         1,             0, 0,        0, "ScaleN");
	        if (DWConv)
	        KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST|B, 1,     1,                                         Fcx*Fcy*Bs,    0, 0,        0, "Filter");
	        else
	        KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0), O_IN|O_DB|O_CONST|B, 1,     1,                                         Fcx*Fcy*Bs,    0, 0,        0, "Filter");
	        KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,T0,Os), O_OUT|O_DB,          Wo,    Ho,                                        1,             0, 0,        0, "Out");
	        KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(2,T0,Os), O_BUFF|O_ONETILE,    Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,       4,      OverlapP, 0,        0, "");
	
	        KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                         1,             0, 0,        0, "Infos");
	        Kernel = UserKernel(Name,
	                (DWConv?
	                KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
	                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons))),
	                TILE_HOR|TILE_HWC,
	                KCArgs,
			Calls(3,
				Call(SetBiasKerName, LOC_D0_PROLOG,
	                                Bindings(6,
	                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* SetBias output tile */
	                                        K_Arg("ConvOut", KER_ARG_TILE_W),                                       /* SetBias output tile width */
	                                        K_Arg("ConvOut", KER_ARG_TILE_H),                                       /* SetBias output tile height */
	                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),				/* Number of output features in this tile */
	                                        K_Arg("Bias", KER_ARG_TILE),                                            /* SetBias Bias tile */
	                                        K_TileOper("Infos", "char  *", '@', AT_INF_BIASN)
						)),
				Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
	                                Bindings(22,
	                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
	                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
	                                        K_Arg("In", KER_ARG_TILE_USEDW),                                        /* Conv input tile width, used part of it */
	                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
	                                        K_Arg("In", KER_ARG_TILE_USEDH),                                        /* Conv input tile height, used part of it */
	                                        K_Arg("Filter", KER_ARG_TILE_W),                                        /* Filter input tile width */
	                                        K_Arg("Filter", KER_ARG_TILE_H),                                        /* Filter input tile height */
	                                        K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),                           /* Number of input features in this tile */
	                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),				/* Number of output features in this tile */
	                                        K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),                     /* Total number of input features currently in L1 memory, argument promotion */
	                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
	                                        DWConv?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,               /* Conv Bias when depth wise conv*/
	                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* Conv output */
	                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
	                                        DWConv?K_TileOper("Infos", "char *", '@', AT_INF_BIASN):AT_IGNORE_ARG_BINDING,  /* NormBias is depth wise conv */
	                                        AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
	                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Fx */
	                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridex */
	                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Dx */
	                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Conv Fy */
	                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridey */
	                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Conv Dy */
	                                        )
				),
	                        Call(DPReductionKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,                                 /* DP Reduction also take care of optional activation */
	                                Bindings(8,
	                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* Double precision input tile */
	                                        K_Arg(PoolOper?"ConvOut":"Out", KER_ARG_TILE),                          /* Single precision output tile, warning use IO kernel when In=Out */
	                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),				/* Input tile Number of features */
	                                        K_Arg("ConvOut", KER_ARG_TILE_W),                                       /* Input tile width */
	                                        K_Arg("ConvOut", KER_ARG_TILE_H),                                       /* Input tile height */
	                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
	                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
	                                        K_Arg("Infos", KER_ARG_TILE)                                            /* Infos */
	                                )
	                        )
			),
			KArgs
	        );
		if (Iter==0) {
			if (Kernel) {
				if (GetKernelTileSize(Kernel, "In", T0)<MinTile && TileCons<=1) {
					OrgTileCons = TileCons; TileCons = MinTile; PopUserKernel();
					printf("Trying to enlarge outer dim of In from %d to %d\n", GetKernelTileSize(Kernel, "In", T0), MinTile);
				} else {
					AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON); break;
				}
			} else {
				AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
				GenTilingError("Failed to generate a tiling solution for %s", Name);
			}
		} else {
			if (!Kernel) {
				Iter--;
				TileCons = OrgTileCons;
				printf("Enlarging outer dim of In tile failed, rolling back to %d\n", OrgTileCons);
			}
			AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		}
	}
        if (Kernel) {
		// printf("Tile Height: %d, Channel: %d", GetKernelTileSize(Kernel, "In", T0), GetKernelTileSize(Kernel, "In", D0));
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                if (DWConv) AddKernelArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 1);
                else AddKernelArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, Filter_DataSize);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 1);
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

        }
        return (Kernel!=0);
}


int CNN_ConvolutionNE16_Bis(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int Bias_DataSize,
        int Scale_DataSize,
	int Filter_DataSize,

        int InFeat,
        int OutFeat,
        int Width,
        int Height,

        KernelOper_T ConvOper,
        int Fcx,
        int Fcy,
        int Dcx,
        int Dcy,
        int Scx,
        int Scy,
        int ConvPad,

        KernelOper_T PoolOper,
        int Fpx,
        int Fpy,
        int Dpx,
        int Dpy,
        int Spx,
        int Spy,
        int PoolPad,

        KernelOper_T ActOper
        )

{
        int ParFeat = 1;
	int B = 0, Bs=1;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        AT_PadType PadType = PAD_BALANCED_LEFT;
        if (PoolOper==KOP_NONE) {
                Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
        }
        if (Ctrl) {
                if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
        }
	if (Filter_DataSize<0) {
		B = O_BIT; Bs = -Filter_DataSize;
	} else Bs = Filter_DataSize;
        int OverlapC, OverlapP;
        int TileCons;
        int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
        char *ConvKerName=0, *PoolKerName=0, *ActKerName=0, *SetBiasKerName=0, *DPReductionKerName=0;
        int DWConv=(ConvOper==KOP_CONV_DW);
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int Os=(DWConv?D0:D1);
        int UsedWidth, UsedHeight, UsedWc, UsedHc;
        int InTileCons = 16; // 16;
        int OutTileCons = 1; // 8;
        int StandAloneAct = (ActOper!=KOP_NONE);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;


	CNN_LayerOutputDim(Width, Height, ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad,
                           &Wc, &Hc, &Wo, &Ho, &PadCw, &PadCh, &PadPw, &PadPh);
        PadInc = CNN_EdgePaddingValue(PadType, PadCw, PadCh);
        PadInp = CNN_EdgePaddingValue(PadType, PadPw, PadPh);
        /* Pad value for tiling, need to accrue phantom values created for Pool padding */
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};

        CNN_TileOverlap(TileOrientation, Fcx, Fcy, Dcx, Dcy, Scx, Scy, Fpx, Fpy, Dpx, Dpy, Spx, Spy, &OverlapC, &OverlapP);
        UsedWc = CNN_UsedInputDimension(Wo, Fpx, Spx, Dpx, PadPw);
        UsedHc = CNN_UsedInputDimension(Ho, Fpy, Spy, Dpy, PadPh);
        UsedWidth  = CNN_UsedInputDimension(UsedWc, Fcx, Scx, Dcx, PadCw);
        UsedHeight = CNN_UsedInputDimension(UsedHc, Fcy, Scy, Dcy, PadCh);
        TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);

        /* Re evaluate now that we know exactly what is used */
        PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
        PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
        UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
        UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
        Wc = UsedWc; Hc = UsedHc;

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
        if (!DWConv) LayerOp *= InFeat;
        if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += OutFeat*Wo*Ho;
        LayerBandwidth += Width*Height*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Wo*Ho*1*OutFeat;
        LayerBandwidth += Fcx*Fcy*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Bias_DataSize*OutFeat;

	SetBiasKerName = "MySetBiasKer";
	ConvKerName = "MyConvKer";
	DPReductionKerName = "MyReductKer";

        if (Log) {
                printf("InFeat: %d, OutFeat: %d", InFeat, OutFeat);
		if (B==O_BIT) printf(", Filter in Bits %d bits\n", Bs); else printf("\n");
                printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
                printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", Wc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
                printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", Hc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC: %d\n", OverlapC);
                printf("OverlapP: %d\n", OverlapP);
                printf("TileCons: %d\n", TileCons);
                printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
                if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
                if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
                if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
                printf("Nb Oper : %lld\n", LayerOp);

        }

        /* User kernel C arguments */
        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
	Kernel_T *Kernel;
	int OrgTileCons=0;
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	int MinTile = 3;
	for (int Iter=0; Iter<2; Iter++) {
	        int Ca=0;
	
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),     "In");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1), "Filter");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),    "Out");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
	        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");
	
	        /* User kernel kernel arguments */
	        Object_T **KArgs = AllocateKerArgs(7);
	        int Ka=0;
	        KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,T0,D0), O_IN|O_DB,     Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,      OverlapC, 0, TileCons, "In");
	        KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         Bias_DataSize, 0, 0,        0, "Bias");
	        KArgs[Ka++] = KerArg ("Scale",  KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         1,             0, 0,        0, "Scale");
	        KArgs[Ka++] = KerArg ("ScaleN", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST,   1,     1,                                         1,             0, 0,        0, "ScaleN");
	        if (DWConv)
	        KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os),    O_IN|O_DB|O_CONST|B, 1,     1,                                         Fcx*Fcy*Bs,    0, 0,        0, "Filter");
	        else
	        KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0), O_IN|O_DB|O_CONST|B, 1,     1,                                         Fcx*Fcy*Bs,    0, 0,        0, "Filter");
	        KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,T0,Os), O_OUT|O_DB,          Wo,    Ho,                                        1,             0, 0,        0, "Out");
	
	        KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                         1,             0, 0,        0, "Infos");
	        Kernel = UserKernel(Name,
	                (DWConv?
	                KernelIterSpace(2, IterParSpace(D0, InFeat, 8), IterTiledSpace(T0)):
	                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons))),
	                TILE_HOR|TILE_HWC,
	                KCArgs,
			Calls(1,
				Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
	                                Bindings(22,
	                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
	                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
	                                        K_Arg("In", KER_ARG_TILE_USEDW),                                        /* Conv input tile width, used part of it */
	                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
	                                        K_Arg("In", KER_ARG_TILE_USEDH),                                        /* Conv input tile height, used part of it */
	                                        K_Arg("Filter", KER_ARG_TILE_W),                                        /* Filter input tile width */
	                                        K_Arg("Filter", KER_ARG_TILE_H),                                        /* Filter input tile height */
	                                        K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),                           /* Number of input features in this tile */
	                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, Os),				/* Number of output features in this tile */
	                                        K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),                     /* Total number of input features currently in L1 memory, argument promotion */
	                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
	                                        DWConv?K_Arg("Bias", KER_ARG_TILE):AT_IGNORE_ARG_BINDING,               /* Conv Bias when depth wise conv*/
	                                        K_Arg("Out", KER_ARG_TILE),                                         /* Conv output */
	                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
	                                        DWConv?K_TileOper("Infos", "char *", '@', AT_INF_BIASN):AT_IGNORE_ARG_BINDING,  /* NormBias is depth wise conv */
	                                        AT_IGNORE_ARG_BINDING,							/* Orientation when feature parallel */
	                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Fx */
	                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridex */
	                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Conv Dx */
	                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Conv Fy */
	                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Conv Stridey */
	                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Conv Dy */
	                                        )
				)
			),
			KArgs
	        );
		if (Iter==0) {
			if (Kernel) {
				if (GetKernelTileSize(Kernel, "In", T0)<MinTile && TileCons<=1) {
					OrgTileCons = TileCons; TileCons = MinTile; PopUserKernel();
					printf("Trying to enlarge outer dim of In from %d to %d\n", GetKernelTileSize(Kernel, "In", T0), MinTile);
				} else {
					AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON); break;
				}
			} else {
				AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
				GenTilingError("Failed to generate a tiling solution for %s", Name);
			}
		} else {
			if (!Kernel) {
				Iter--;
				TileCons = OrgTileCons;
				printf("Enlarging outer dim of In tile failed, rolling back to %d\n", OrgTileCons);
			}
			AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
		}
	}
        if (Kernel) {
		// printf("Tile Height: %d, Channel: %d", GetKernelTileSize(Kernel, "In", T0), GetKernelTileSize(Kernel, "In", D0));
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                if (DWConv) AddKernelArgDim(Name, "Filter", 4, InFeat, Fcx, Fcy, 1);
                else AddKernelArgDim(Name, "Filter", 5, OutFeat, InFeat, Fcx, Fcy, Filter_DataSize);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 1);
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

        }
        return (Kernel!=0);
}

