#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators_SQ8.h"
#include "CNN_Generators_NE16.h"
#include "CNN_Generator_Util.h"
#include "Gap.h"
#include "hal_ne16.h"

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

#define AT_INF_NE16_PADVAL      10
#define AT_INF_NE16_WOFFSET     12
//#define AT_INF_NE16_DEF_CFG     16

#define AT_INF_DIM              16

unsigned int NE16_DefaultConfig(unsigned char Qw,
                        unsigned char Mode16,
                        unsigned char StreamoutMode,
                        unsigned char FilterMode,
                        unsigned char LinearMode,
                        unsigned char StridedMode,
                        unsigned char NormBits,
                        unsigned char Streamin,
                        unsigned char WOffsetCfg,
                        unsigned char QuantRightShift,
                        unsigned char QuantBits,
                        unsigned char QuantNoRect,
                        unsigned char NormShift,
                        unsigned char NormBias
                       )
{
        unsigned int ret = 0;
        ret = ret | ((Qw-1          & NE16_MASK_WBITS_M1)          << NE16_SHIFT_WBITS_M1);            /**< weight bits minus 1. */
        ret = ret | ((Mode16        & NE16_MASK_MODE16)            << NE16_SHIFT_MODE16);              /**< mode16 (0=8-bits mode; 1=16-bits mode) */
        ret = ret | ((StreamoutMode & NE16_MASK_OUTQUANT)          << NE16_SHIFT_OUTQUANT);            /**< quantization rect(0=rectify + consider as unsigned; 1=do not rectify, keep sign) */
        ret = ret | ((FilterMode    & NE16_MASK_FILTER_MODE)       << NE16_SHIFT_FILTER_MODE);         /**< filter mode (11=reserved, 10=1x1, 01=3x3 depthwise, 00=3x3) */
        ret = ret | ((LinearMode    & NE16_MASK_LINEAR_MODE)       << NE16_SHIFT_LINEAR_MODE);         /**< linear mode (0=normal operation, 1=linear mode) */
        ret = ret | ((StridedMode   & NE16_MASK_STRIDED_MODE)      << NE16_SHIFT_STRIDED_MODE);        /**< strided 2x2 mode (0=normal operation, 1=strided mode) */
        ret = ret | ((NormBits      & NE16_MASK_NORM_BITS)         << NE16_SHIFT_NORM_BITS);           /**< normalization bits (00=8-bits, 01=16-bits, 10=32-bits) */
        ret = ret | ((Streamin      & NE16_MASK_STREAMIN)          << NE16_SHIFT_STREAMIN);            /**< streamin mode (0=normal operation, 1=perform streamin) */
        ret = ret | ((WOffsetCfg    & NE16_MASK_WEIGHT_OFFSET_CFG) << NE16_SHIFT_WEIGHT_OFFSET_CFG);   /**< weight offset cfg (0=symmetric weights, 1=use layer-wise weight_offset) */
        ret = ret | ((QuantRightShift & NE16_MASK_QUANT_RIGHT_SHIFT) << NE16_SHIFT_QUANT_RIGHT_SHIFT); /**< quantization right shift */
        ret = ret | ((QuantBits     & NE16_MASK_QUANT_BITS)        << NE16_SHIFT_QUANT_BITS);          /**< quantization bits (00=8-bits, 01=16-bits, 10=32-bits) */
        ret = ret | ((QuantNoRect   & NE16_MASK_QUANT_NORECT)      << NE16_SHIFT_QUANT_NORECT);        /**< streamout / quantization (1=quantization+streamout, 0=streamout only) */
        ret = ret | ((NormShift     & NE16_MASK_NORM_SHIFT)        << NE16_SHIFT_NORM_SHIFT);          /**< norm option shift (0=use quantization right shift; 1=load with norm) */
        ret = ret | ((NormBias      & NE16_MASK_NORM_BIAS)         << NE16_SHIFT_NORM_BIAS);           /**< norm option bias (0=do not load bias; 1=load bias) */
        return ret;
}

void LoadCNN_NE16_SQ8_Library()

{
        LibKernelTemplate("KerConv_NE16_T",
                  CArgs(26,
                        TCArg("unsigned char * __restrict__", "In"),
                        TCArg("unsigned short * __restrict__","Filter"),
                        TCArg("int * __restrict__",           "Bias"),
                        TCArg("unsigned char * __restrict__", "Out"),
                        TCArg("unsigned char * __restrict__", "Scale"),
                        TCArg("unsigned char * __restrict__", "ScaleN"),
                        TCArg("unsigned short int",           "Tile_InFeat"),
                        TCArg("unsigned short int",           "Tile_InH"),
                        TCArg("unsigned short int",           "Tile_InW"),
                        TCArg("unsigned short int",           "Tile_OutFeat"),
                        TCArg("unsigned short int",           "Tile_OutH"),
                        TCArg("unsigned short int",           "Tile_OutW"),
                        TCArg("unsigned short int",           "Pad_Val"),
                        TCArg("v4s",                          "Pad"),
                        TCArg("unsigned char",                "LastD0"),
                        TCArg("unsigned char",                "FirstD0"),
                        TCArg("unsigned char",                "Qw"),
                        TCArg("unsigned char",                "Mode16"),
                        TCArg("unsigned int",                 "Default_NE16_Job_Cfg"),
                        TCArg("int",                          "W_Offset"),
                        TCArg("unsigned char",                "Fx"),
                        TCArg("unsigned char",                "Sx"),
                        TCArg("unsigned char",                "Dx"),
                        TCArg("unsigned char",                "Fy"),
                        TCArg("unsigned char",                "Sy"),
                        TCArg("unsigned char",                "Dy")
                        )
        );
        LibKernelTemplate("KerLinear_NE16_T",
                  CArgs(14,
                        TCArg("signed char * __restrict__",   "In"),
                        TCArg("unsigned short * __restrict__","Filter"),
                        TCArg("int * __restrict__",           "Bias"),
                        TCArg("int * __restrict__",           "Out"),
                        TCArg("unsigned char * __restrict__", "Scale"),
                        TCArg("unsigned char * __restrict__", "ScaleN"),
                        TCArg("unsigned short int",           "Tile_InFeat"),
                        TCArg("unsigned short int",           "Tile_OutFeat"),
                        TCArg("unsigned char",                "LastD0"),
                        TCArg("unsigned char",                "FirstD0"),
                        TCArg("int",                          "W_Offset"),
                        TCArg("unsigned char",                "Qw"),
                        TCArg("unsigned char",                "Mode16"),
                        TCArg("unsigned int",                 "Default_NE16_Job_Cfg")
                        )
        );
        LibKernelTemplate("WeightsExpandLastTile_T",
                  CArgs(7,
                        TCArg("unsigned short int *__restrict__", "Filter"),
                        TCArg("signed char", "IsLastTile"),
                        TCArg("unsigned short int", "OutFeat"),
                        TCArg("unsigned short int", "Chin"),
                        TCArg("unsigned short int", "Fcx"),
                        TCArg("unsigned short int", "Fcy"),
                        TCArg("unsigned char", "Nbits")
                        )
        );
        LibKernelTemplate("KerSignedUnsigned_T",
                  CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
                        TCArg("signed char * __restrict__", "Infos")
                        )
        );
        LibKernel("KerSignUnsign", 			CALL_PARALLEL|CALL_HWC_KER, 0, "KerSignedUnsigned_T", 0);

        LibKernel("WeightsExpandLastTile", 			CALL_PARALLEL|CALL_HWC_KER, 0, "WeightsExpandLastTile_T", 0);
        LibKernel("NE16_Enable",    				CALL_SEQUENTIAL, CArgs(0), "", 0);
        LibKernel("NE16_SoftReset", 				CALL_SEQUENTIAL, CArgs(0), "", 0);
        LibKernel("NE16_Disable",   				CALL_SEQUENTIAL, CArgs(0), "", 0);

        /* Convolution or Linear output reduction with per channel scaling and optional activation. Out != In and In Place (IO)  */
        /* Activation wth tensor centric scaling */

        LibKernel("KerConv3x3Stride1_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4),  3, 3, 1, 1, 1, 1));
        LibKernel("KerConv3x3Stride1_DxDy_NE16",CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4),  3, 3,-1,-1, 1, 1));
        LibKernel("KerConv3x3Stride2_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4),  3, 3, 1, 1, 2, 2));
        // LibKernel("KerConv3x3StrideSxxSy_NE16", CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4),  3, 3, 1, 1,-1,-1));
        LibKernel("KerConv1x1Stride1_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4),  1, 1, 1, 1, 1, 1));
        LibKernel("KerConvNxMStride1_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(1,1,0,0,4), -1,-1, 1, 1, 1, 1));
        LibKernel("KerConvDW3x3Stride1_NE16",   CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,0,0,4), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2_NE16",   CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T", CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, 0, CNN_Type(1,1,0,0,4), 3,3,1,1,2,2));

        LibKernel("KerLinear_NE16",             CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerLinear_NE16_T", CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_NONE), 1, CNN_Type(1,1,4,0,1), 0,0,0,0,0,0));
}

static Kernel_T *CNN_ConvolutionNE16_Internal(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

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

        KernelOper_T ActOper,

        unsigned int MinTileDim,
        int InFeatOneTile,
        unsigned int TileCons
        )

{
        if (In_DataSize!=1 && In_DataSize!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, In_DataSize);
        int Mode16 = (In_DataSize == 2);
        int Ws = Filter_DataSizeBits;
        unsigned int Wa = 0;
        // if (1 || ((Ws%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        if (((Ws%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        unsigned int Wp = SPACE_PROP_PAD2PREF;
        unsigned int Bs = Bias_DataSize;
        int ParFeat = 1;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        AT_PadType PadType = PAD_BALANCED_LEFT;
        if (PoolOper==KOP_NONE) {
                Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
        }
        if (Ctrl) {
                if (Ctrl->PadType != -1) PadType = Ctrl->PadType;
        }
        int OverlapC, OverlapP;
        // int TileCons;
        int Wo, Ho, Wc, Hc;
        int PadCw=0, PadCh=0, PadPw=0, PadPh=0;
        v4s PadInp  = (v4s){0,0,0,0}, PadInc  = (v4s){0,0,0,0}, PadIncT = (v4s){0,0,0,0};
        char *ConvKerName=0, *PoolKerName=0, *ActKerName=0, *SetBiasKerName=0, *DPReductionKerName=0;
        int DWConv=(ConvOper==KOP_CONV_DW);
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int Os=(DWConv?D0:D1);
        int UsedWidth, UsedHeight, UsedWc, UsedHc;
        // int InTileCons = 16; // InFeat<16?16:InFeat;
        unsigned int InTileCons = 16;
        unsigned int InFeatProp = (InFeatOneTile!=0)?SPACE_PROP_ONE_TILE:0;
        int OutTileCons = 1;
        int StandAloneAct = (ActOper!=KOP_NONE);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;
        int NeedConvOut = ((InTileCons < InFeat) && !InFeatOneTile) && (!DWConv);
        int NeedReduct = NeedConvOut;
        unsigned int Cos = NeedConvOut?4:1;

        if (!(ConvOper == KOP_NONE || ConvOper == KOP_CONV || ConvOper == KOP_CONV_DW))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
        if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL)) // || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

        if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

        if (DWConv) Wa |= O_NE16_DW; else Wa |= O_NE16_PW;

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
        // TileCons = (TileOrientation==TILE_HOR)?CNN_Scm(Scy, Spy):CNN_Scm(Scx, Spx);

        /* Re evaluate now that we know exactly what is used */
        PadInc[1] = Max(0, PadInc[1]-(Width-UsedWidth)); PadInc[3] = Max(0, PadInc[3]-(Height-UsedHeight));
        PadInp[1] = Max(0, PadInp[1]-(Wc-UsedWc)); PadInp[3] = Max(0, PadInp[3]-(Hc-UsedHc));
        PadIncT = (v4s) {PadInp[0]*Scx+PadInc[0], PadInp[1]*Scx+PadInc[1], PadInp[2]*Scy+PadInc[2], PadInp[3]*Scy+PadInc[3]};
        UsedWc = (Wo-1)*Spx+(Dpx*(Fpx-1)+1)-PadInp[0]-PadInp[1]; UsedHc = (Ho-1)*Spy+(Dpy*(Fpy-1)+1)-PadInp[2]-PadInp[3];
        UsedWidth  = (UsedWc-1)*Scx+(Dcx*(Fcx-1)+1) -PadInc[0]-PadInc[1]; UsedHeight = (UsedHc-1)*Scy+(Dcy*(Fcy-1)+1)-PadInc[2]-PadInc[3];
        //Wc = UsedWc; Hc = UsedHc;

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
        if (!DWConv) LayerOp *= InFeat;
        if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += OutFeat*Wo*Ho;
        LayerBandwidth += Width*Height*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Wo*Ho*1*OutFeat;
        LayerBandwidth += (Fcx*Fcy*Filter_DataSizeBits*InFeat*(DWConv?1:OutFeat)+7)/8;
        LayerBandwidth += Bias_DataSize*OutFeat;

        if (ConvOper == KOP_CONV_DW){
                if (Fcx == 3 && Fcy==3) {
                    if (Scx == 2 && Scy == 2) ConvKerName = "KerConvDW3x3Stride2_NE16";
                    else                      ConvKerName = "KerConvDW3x3Stride1_NE16";
                } else ConvKerName = 0;
        } else {
                ConvKerName = CNN_FindMatchingKernelAttr(ConvOper, KOP_NONE, ParFeat, CALL_HWC_KER, 1, 1, Bias_DataSize, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
                                                         &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
        }
        if (ConvKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Convolution basic kernel", Name);
        if (NeedReduct) {
                DPReductionKerName = CNN_FindMatchingKernelAttr(KOP_DP_REDUCT_UNSIGNED, ActOper, ParFeat, CALL_HWC_KER, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (DPReductionKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Reduction basic kernel", Name);
        }
        if (PoolOper==KOP_MAXPOOL) {
                PoolKerName = "KerParPool_MaxPoolNxMStrideSxSy__HWC_USQ8";
                NeedConvOut = 1;
        }

        if (Log) {
                printf("InFeat: %d%s, OutFeat: %d, InFeatCons: %d\n", InFeat, InFeatOneTile?" (FULL)":"(PART)", OutFeat, InTileCons);
                printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]x%d Bits\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy, Filter_DataSizeBits);
                printf("Conv => W:  %d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy);
                printf("     => H:  %d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("Pool => Wc: %d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", UsedWc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
                printf("     => Hc: %d, Pad:[%d,%d] => Ho: %d\n", UsedHc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC: %d\n", OverlapC);
                printf("OverlapP: %d\n", OverlapP);
                printf("TileCons: %d, MinTile: %d\n", TileCons, MinTileDim);
                printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
                if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
                if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                if (ActKerName) printf("%20s: %s\n", "ActKerName", ActKerName);
                printf("Nb Oper : %lld\n", LayerOp);

        }

        /* User kernel C arguments */
        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
        Kernel_T *Kernel;
        // int MinTile = (Fcx==1)?3:5;

        int StreamoutMode   = 1; // Streamout = apply *Scale >> ScaleN
        int Streamin        = 0; // Streamin initialized at 0, set to 1 in the basic kernel if multiple chin tile
        int FilterMode      = (Fcx==1)?2:(DWConv?1:0);
        int LinearMode      = 0;
        int StridedMode     = (Scx==2 && Scy==2)?1:0;
        int NormBits        = 0;
        int WOffsetCfg      = 1;
        int QuantRightShift = 0;
        int QuantBits       = (NeedReduct)?2:0; // 00: 8bit, 01: 16bit, 10: 32bit --> If tiling the channel input dimension you need to streamin (need 32 bits output)
        int QuantNoRect     = (ActOper==KOP_RELU || ActOper==KOP_RELUN)?0:1;
        int NormShift       = 1;
        int NormBias        = 1;
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig(Filter_DataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, \
                                                               WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);

        int Ca=0;
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(In_DataSize,1,1),     "In");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "Filter"); // int16_t for 16 chin contributions
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
        KCArgs[Ca++] = QuantNoRect?TCArg(CNN_ArgDataType(1,1,1),    "Out"):TCArg(CNN_ArgDataTypeUns(1,1,1),"Out");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");

        /* User kernel kernel arguments */
        Object_T **KArgs = AllocateKerArgs(NeedConvOut?8:7);
        int Ka=0;
        KArgs[Ka++] = KerArgP("In",     KerArgSpace(2,T0,D0),    O_IN|O_DB,     Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, 1,   OverlapC, 0, TileCons, "In");
        if (MinTileDim && (MinTileDim > TileCons)) SetKerArgMinTileSize(KArgs[Ka-1], MinTileDim);
        KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         Bs,         0, 0,        0, "Bias");
        KArgs[Ka++] = KerArg ("Scale",  KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         1,          0, 0,        0, "Scale");
        KArgs[Ka++] = KerArg ("ScaleN", KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         1,          0, 0,        0, "ScaleN");
        if (DWConv) {
                KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os|Wp),    O_IN|O_DB|O_CONST|Wa, Fcx, Fcy,                               Ws, 0, 0,        0, "Filter");
        } else {
                KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0|Wp), O_IN|O_DB|O_CONST|Wa, Fcx, Fcy,                               Ws, 0, 0,        0, "Filter");
        }
        if (NeedConvOut) 
        KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(2,T0,Os),    O_BUFF|O_ONETILE,  Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        Cos, OverlapP, 0,        0, "");
        KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,T0,Os),    O_OUT|O_DB,        Wo,    Ho,                                         1,          0, 0,        0, "Out");
        KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_DIM,    1,                      1,          0, 0,        0, "Infos");


        Kernel = UserKernel(Name,
                (DWConv?
                KernelIterSpace(2, IterParSpace(D0|InFeatProp, InFeat, InTileCons), IterTiledSpace(T0)):
                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0|InFeatProp, InFeat, InTileCons))),
                TILE_HOR|TILE_HWC,
                KCArgs,
                Calls(6,
                        Call("NE16_Enable", DWConv?LOC_D0_PROLOG:LOC_D1_PROLOG, Bindings(0)),
                        Call("NE16_SoftReset", DWConv?LOC_LOOP:LOC_D0, Bindings(0)),
                        /*((InFeat%16)!=0)?
                        Call("WeightsExpandLastTile", DWConv?LOC_LOOP:LOC_D0,
                                Bindings(7,
                                        K_Arg("Filter", KER_ARG_TILE),
                                        K_ArgPred("In", KER_ARG_TILELAST, D0),
                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os),
                                        Imm(InFeat%16),
                                        Imm(Fcx),
                                        Imm(Fcy),
                                        Imm(Filter_DataSizeBits)
                                )
                        ):AT_NO_CALL, Disable the runtime weights expansion for now*/
                        Call(ConvKerName, DWConv?LOC_LOOP:LOC_D0,
                                Bindings(26,
                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* Conv Bias when depth wise conv*/
                                        K_Arg(NeedConvOut?"ConvOut":"Out", KER_ARG_TILE),                       /* Conv output */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),    //Filter instead of In??   /* Number of input features in this tile */
                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
                                        K_ArgPar(NeedConvOut?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os),                          /* Number of output features in this tile */
                                        K_Arg(NeedConvOut?"ConvOut":"Out", KER_ARG_TILE_H),
                                        K_Arg(NeedConvOut?"ConvOut":"Out", KER_ARG_TILE_W),
                                        K_TileOper("Infos", "short int *", '@', AT_INF_NE16_PADVAL/2),          /* PadVal */
                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
                                        K_ArgPred("In", KER_ARG_TILELAST, D0),
                                        K_ArgPred("In", KER_ARG_TILEFIRST, D0),
                                        Imm(Filter_DataSizeBits),
                                        Imm(Mode16),
                                        Imm(DEFAULT_NE16_JOB_CFG),                                              /* MOVE TO INFOS !!!! */ 
                                        WOffsetCfg?K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4):AT_IGNORE_ARG_BINDING, /* W_Offset */
                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fx */
                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridex */
                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Dx */
                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fy */
                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridey */
                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Pooling Dy */
                                )
                        ),
                        (NeedReduct==0)?AT_NO_CALL:
                        Call(DPReductionKerName, DWConv?LOC_LOOP_EPILOG:LOC_D0_EPILOG,                                 /* DP Reduction also take care of optional activation */
                                Bindings(8,
                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* Double precision input tile */
                                        K_Arg(PoolOper?"ConvOut":"Out", KER_ARG_TILE),                          /* Single precision output tile, warning use IO kernel when In=Out */
                                        ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),           /* Input tile Number of features */
                                        K_Arg("ConvOut", KER_ARG_TILE_W),                                       /* Input tile width */
                                        K_Arg("ConvOut", KER_ARG_TILE_H),                                       /* Input tile height */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_Arg("Infos", KER_ARG_TILE)                                            /* Infos */
                                )
                        ),
                        (PoolKerName==0)?AT_NO_CALL:
                        Call(PoolKerName, DWConv?LOC_LOOP:LOC_D0_EPILOG,
                                Bindings(13,
                                        K_Arg("ConvOut", KER_ARG_TILE),
                                        K_Arg("Out", KER_ARG_TILE),
                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, Os),
                                        K_Arg("ConvOut", KER_ARG_TILE_W),
                                        K_Arg("ConvOut", KER_ARG_TILE_USEDW),
                                        K_Arg("ConvOut", KER_ARG_TILE_H),
                                        K_Arg("ConvOut", KER_ARG_TILE_USEDH),
                                        Imm(Fpx),
                                        Imm(Fpy),
                                        Imm(Spx),
                                        Imm(Spy),
                                        K_Arg("ConvOut", KER_ARG_TILE_PAD),
                                        K_Arg("Infos", KER_ARG_TILE)
                                )
                        ),
                    Call("NE16_Disable", DWConv?LOC_D0_EPILOG:LOC_D1_EPILOG, Bindings(0))
                ),
                KArgs
        );
        if (Kernel) {
                int InFeatPadded = (InFeat%16)?16*(InFeat/16+1):InFeat;
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, 1);
                if (DWConv) AddKernelArgDim(Name, "Filter", 4, Fcx, Fcy, InFeatPadded, 1);
                else AddKernelArgDim(Name, "Filter", 5, OutFeat, Fcx, Fcy, InFeatPadded, 1);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, 1);
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

                AT_PrepareForTest_SQ8(Name, InFeat, OutFeat, Width, Height, Bias_DataSize,
                                      ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, PadInc,
                                      PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp,
                                      ActOper);
        }
        return Kernel;
}

int CNN_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

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
        unsigned int MinTile;
        unsigned int InTileCons;
        if (PoolOper==KOP_NONE) {
                Fpx=1; Dpx=1; Spx=1; Fpy=1; Dpy=1; Spy=1;
        }
        unsigned int TileCons = CNN_Scm(Scy, Spy);
        unsigned int OrgTileCons = TileCons;

        Kernel_T *Ker, *Sol1 = 0, *Sol2 = 0;
        float K = 0.7;
        int DW = (ConvOper==KOP_CONV_DW);

        if (TileCons == 1) TileCons = 3; else TileCons = CNN_Scm(TileCons, 3);
        unsigned int Sol1TileCons = TileCons, Sol2TileCons = TileCons;

        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
        printf("----------------------------------------------------------CNN_ConvolutionNE16------------------------------------------------------------------------------\n");
        Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                    In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                    ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                    0, DW?0:1, TileCons);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        else if (OrgTileCons != TileCons) {
                Sol1TileCons = OrgTileCons;
                Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                                In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                                ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                                0, DW?0:1, OrgTileCons);
                if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        }
        if (InFeat>16 && !DW) {
                Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                            In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                        ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                        0, 0, TileCons);
                if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
                else if (OrgTileCons != TileCons) {
                    Sol2TileCons = OrgTileCons;
                    Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                                In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                            ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                            0, 0, OrgTileCons);
                    if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
                }
        }
        if (Sol1 && Sol2) {
                int TakeSol1 = 1; //((K*Sol1->Cost->TileOverhead) < Sol2->Cost->TileOverhead);
                Kernel_T *Sol = TakeSol1?Sol1:Sol2;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InFeat);

                printf("Eval [%s] CNN_ConvolutionNE16, %35s: %s InFeat: %4d/%4d, Tile: %4d/%4d, Tiling Overhead: %f (%s overhead is %f)\n",
                        DW?"DW":"PW", Name, Full?"FULL ":"TILED", InFeat, GetKernelTileSize(Sol, "In", D0),
                         GetKernelTileSize(Sol, "In", T0), TakeSol1?Sol1TileCons:Sol2TileCons, Sol->Cost->TileOverhead,
                        (!TakeSol1)?"FULL ":"TILED", (!TakeSol1)?Sol1->Cost->TileOverhead:Sol2->Cost->TileOverhead);
                if (TakeSol1) {
                    PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
                } else {
                    PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
                }
        } else if (Sol1) {
                Kernel_T *Sol = Sol1;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InFeat);
                printf("Eval [%s] CNN_ConvolutionNE16, %35s: %s InFeat: %4d/%4d, Tile: %4d/%4d, Tiling Overhead: %f\n", DW?"DW":"PW",
                    Name, Full?"FULL ":"TILED", InFeat, GetKernelTileSize(Sol, "In", D0), GetKernelTileSize(Sol1, "In", T0), Sol1TileCons, Sol1->Cost->TileOverhead);
                PushBackUserKernel(Sol1);
        } else if (Sol2) {
                Kernel_T *Sol = Sol2;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InFeat);
                printf("Eval [%s] CNN_ConvolutionNE16, %35s: %s InFeat: %4d/%4d, Tile: %4d/%4d, Tiling Overhead: %f\n", DW?"DW":"PW",
                    Name, Full?"FULL ":"TILED", InFeat, GetKernelTileSize(Sol, "In", D0), GetKernelTileSize(Sol2, "In", T0), Sol2TileCons, Sol2->Cost->TileOverhead);
                PushBackUserKernel(Sol2);
        } else GenTilingError("CNN_ConvolutionNE16, %s: can't find any possible mapping");
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        return 1;
}

/*********************************************************************************************************************************************************************
        Generator for Linear layers followed wth channel centric scaling followed by an optional activation

        Template:
                Name:           Name of the generated user kernel

                Ctrl:           Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 0)

                Bias_DataSize:  1: byte, 2: half word, 4: word
                Scale_DataSize: 1: byte, 2: half word, 4: word

                InDim:          Number of inputs
                OutDim:         Number of outputs

                LinearOper      KOP_LINEAR
                ActOper         Optional activation function: KOP_NONE, KOP_RELU, KOP_RELUN, KOP_HSWISH, KOP_HSIGMOID, KOP_LEAKYRELU

                Signature:      Name(In, Filter, Bias, Out, Scale, ScaleN, Infos)

        CNN_LinearAct_NE16
        
*********************************************************************************************************************************************************************/

static Kernel_T *CNN_LinearAct_NE16_Internal(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InDim,
        int OutDim,

        KernelOper_T LinearOper,
        KernelOper_T ActOper,

        int InFeatOneTile,
        int OutTileCons
        )

{
        if (In_DataSize!=1 && In_DataSize!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, In_DataSize);
        int Mode16 = (In_DataSize == 2);
        int Ws = Filter_DataSizeBits;
        unsigned int Wp = SPACE_PROP_PAD2PREF;
        unsigned int Bs = Bias_DataSize;
        unsigned int Wa = 0;
        if (((Ws%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_LIN;
        unsigned int InTileCons = 16;
        unsigned int InFeatProp = (InFeatOneTile!=0)?SPACE_PROP_ONE_TILE:0;

        int Log = 1;
        int Iter;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        int ParFeat = 1;
        char *LinearKerName=0, *ActKerName=0;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;

        /* First try to map on Linear Kernel without reduction, for that In and one full line of Weights must fit into L1 */
        if (LinearOper != KOP_LINEAR) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, only KOP_LINEAR should be used as LinearOper argument", Name);

        LinearKerName = "KerLinear_NE16";
        if (ActOper!=KOP_NONE) {
                ActKerName = CNN_FindMatchingKernel(ActOper, KOP_NONE, ParFeat, 1, 1, Bs, 0, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (ActKerName==0) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, Can't find a matching Activation basic kernel", Name);
        }
        int NeedLinOut = (InTileCons < InDim) && !InFeatOneTile;
        unsigned int Ls = NeedLinOut?4:1;

        LayerOp += InDim*OutDim;
        if (ActOper) LayerOp += OutDim;
        LayerBandwidth += InDim*OutDim*1;
        LayerBandwidth += OutDim*1;
        LayerBandwidth += InDim*OutDim*1;
        LayerBandwidth += Bs*OutDim;

        int StreamoutMode   = 1;
        int Streamin        = 0; // Streamin initialized at 0, set to 1 in the basic kernel if multiple chin tile
        int FilterMode      = 3;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0;
        int WOffsetCfg      = 1;
        int QuantRightShift = 0;
        int QuantBits       = 0; // 00: 8bit, 01: 16bit, 10: 32bit
        int QuantNoRect     = 0; //ActOper==KOP_RELU?0:1;
        int NormBias        = 1;
        int NormShift       = 1;
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig(Filter_DataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, \
                                                               WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);

        if (Log) {
                printf("Linear Layer %s, %s: InDim: %d%s, Cons: %d, OutDim: %d, Cons: %d, Activation: %s\n",
                       Name, CNN_KernelOperImage(LinearOper), InDim, InFeatOneTile?" (FULL)":"(PART)", InTileCons, OutDim, OutTileCons, CNN_KernelOperImage(ActOper));
                if (LinearKerName) printf("Linear Kernel: %s\n", LinearKerName);
                if (ActKerName)    printf("Act Kernel: %s\n", ActKerName);
        }
        Kernel_T *Kernel;

        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
        int Ca=0;
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(In_DataSize,1,1), "In");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "Filter");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bs,1,1),   "Bias");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "Out");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "Scale");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "ScaleN");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),    "Infos");

        Object_T **KArgs = AllocateKerArgs(7+(NeedLinOut||ActKerName));
        int Ka=0;
        KArgs[Ka++] = KerArg("In",      KerArgSpace(1,D0),   OBJ_IN_DB,            1, 1,  1,            0, 0, 0, "In");
        KArgs[Ka++] = KerArg("Filter",  KerArgSpace(2,D1,D0|Wp), OBJ_IN_DB|O_CONST|Wa, 1, 1,  Ws,           0, 0, 0, "Filter");
        KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  Bs,       0, 0, 0, "Bias");
        if (NeedLinOut||ActKerName)
        KArgs[Ka++] = KerArg("LinOut",  KerArgSpace(1,D1),       O_BUFF|O_ONETILE,     1, 1,  Ls,           0, 0, 0, "");
        KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D1),   OBJ_OUT_DB,           1, 1,  1,            0, 0, 0, "Out");
        KArgs[Ka++] = KerArg("Scale",   KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  1,            0, 0, 0, "Scale");
        KArgs[Ka++] = KerArg("ScaleN",  KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  1,            0, 0, 0, "ScaleN");
        KArgs[Ka++] = KerArg("Infos",   KerArgSpace(1,T0),   O_IN|O_BUFF|O_NTILED, 1, 1,  AT_INF_DIM*1, 0, 0, 0, "Infos");

        Kernel = UserKernel(Name,
                KernelIterSpace(3, IterTiledSpace(T0), IterParSpace(D1, OutDim, OutTileCons), IterParSpace(D0|InFeatProp, InDim, InTileCons)),
                TileOrientation,
                KCArgs,
                Calls(5,
                        Call("NE16_Enable", LOC_PROLOG, Bindings(0)),
                        Call("NE16_SoftReset", LOC_D0, Bindings(0)),
                        Call(LinearKerName, LOC_D0,
                                Bindings(14,
                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* Conv Bias when depth wise conv*/
                                        K_Arg(NeedLinOut?"LinOut":"Out", KER_ARG_TILE),                     /* Conv output */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),                   /* Number of input features in this tile */
                                        K_ArgPar(NeedLinOut?"LinOut":"Out", KER_ARG_PARTILE_SIZE, D1),      /* Number of output features in this tile */
                                        K_ArgPred(NeedLinOut?"LinOut":"Out", KER_ARG_TILELAST, D1),     /* First Out Tile */
                                        K_ArgPred(NeedLinOut?"LinOut":"Out", KER_ARG_TILEFIRST, D1),        /* Last Out Tile */
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),               /* W_Offset */
                                        Imm(Filter_DataSizeBits),
                                        Imm(Mode16),
                                        Imm(DEFAULT_NE16_JOB_CFG)                                               /* MOVE TO INFOS !!!! */ 
                                        //K_TileOper("Infos", "unsigned int *", '@', AT_INF_NE16_DEF_CFG),      /* Default NE16 config */
                                )
                        ),
                        (ActKerName==0)?AT_NO_CALL:
                        Call(ActKerName, LOC_D0_EPILOG,
                                Bindings(6,
                                        K_Arg(NeedLinOut?"LinOut":"Out", KER_ARG_TILE),             /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                     /* Output tile */
                                        K_ArgPar("Out", KER_ARG_PARTILE_SIZE, D1),                      /* Number of features in this tile */
                                        Imm(1),                                                         /* Tile width */
                                        Imm(1),                                                         /* Tile height */
                                        K_Arg("Infos", KER_ARG_TILE)                                    /* Infos */
                                )
                        ),
                        Call("NE16_Disable", LOC_EPILOG, Bindings(0))
                ),
                KArgs
        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 2, InDim, 1);
                AddKernelArgDim(Name, "Filter", 3, OutDim, InDim, 1);
                AddKernelArgDim(Name, "Bias", 2, OutDim, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 2, OutDim, 1);
                AddKernelArgDim(Name, "Scale", 2, OutDim, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutDim, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_DIM, 1);

                AT_PrepareForTest_SQ8(Name, InDim,OutDim,1,1, Bias_DataSize, LinearOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
        }
        return Kernel;
}



int CNN_LinearAct_NE16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Bias_DataSize,
        int Scale_DataSize,
        int Filter_DataSizeBits,

        int InDim,
        int OutDim,

        KernelOper_T LinearOper,
        KernelOper_T ActOper
        )

{
        Kernel_T *Ker = 0, *Sol1 = 0, *Sol2 = 0;
        float K = 0.7;

        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);

        Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 1, 32);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        else {
                Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 0, 32);
                if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        }
        Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 1, 1);
        if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
        else {
                Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 0, 1);
                if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
        }

        if (Sol1 && Sol2) {
                int TakeSol1 = ((K*Sol1->Cost->TileOverhead) < Sol2->Cost->TileOverhead);
                Kernel_T *Sol = TakeSol1?Sol1:Sol2;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InDim);
                printf("Eval CNN_LinearAct_NE16, %35s: %s InDim: %4d/%4d, OutDim: %4d/%4d, Tiling Overhead: %f (%s overhead is %f)\n",
                       Name, Full?"FULL ":"TILED", InDim, GetKernelTileSize(Sol, "In", D0), OutDim, GetKernelTileSize(Sol, "Out", D1),
                       Sol->Cost->TileOverhead, (!TakeSol1)?"FULL ":"TILED", (!TakeSol1)?Sol1->Cost->TileOverhead:Sol2->Cost->TileOverhead);
                if (TakeSol1) {
                        PushBackUserKernel(Sol1); ReleaseUserKerne(Sol2);
                } else {
                        PushBackUserKernel(Sol2); ReleaseUserKerne(Sol1);
                }
        } else if (Sol1) {
                Kernel_T *Sol = Sol1;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InDim);
                printf("Eval CNN_LinearAct_NE16, %35s: %s InDim: %4d/%4d, OutDim: %4d/%4d, Tiling Overhead: %f\n",
                        Name, Full?"FULL ":"TILED", InDim, GetKernelTileSize(Sol, "In", D0), OutDim, GetKernelTileSize(Sol, "Out", D1), Sol->Cost->TileOverhead);
                PushBackUserKernel(Sol1);
        } else if (Sol2) {
                Kernel_T *Sol = Sol2;
                int Full = (GetKernelTileSize(Sol, "In", D0) == InDim);
                printf("Eval CNN_LinearAct_NE16, %35s: %s InDim: %4d/%4d, OutDim: %4d/%4d, Tiling Overhead: %f\n",
                        Name, Full?"FULL ":"TILED", InDim, GetKernelTileSize(Sol, "In", D0), OutDim, GetKernelTileSize(Sol, "Out", D1), Sol->Cost->TileOverhead);
                PushBackUserKernel(Sol2);
        } else GenTilingError("CNN_LinearAct_NE16, %s: can't find any possible mapping");

        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        return 1;

}
