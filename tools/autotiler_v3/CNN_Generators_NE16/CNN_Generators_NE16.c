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
#include "CNN_Generators_SQ8.h"
#include "CNN_Generators_NE16.h"
#include "RNN_Generators_NE16.h"
#include "CNN_Generator_Util.h"
#include "Gap.h"
#include "hal_ne16.h"

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

#define AT_INF_NE16_PADVAL      10
#define AT_INF_NE16_WOFFSET     12

#define AT_INF_NE16_DIM         16

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

static int Loaded = 0;

void LoadCNN_NE16_SQ8_Library()

{
        if (Loaded) return;
        Loaded = 1;
        Load_RNN_NE16_Library();
        LibKernelTemplate("Ker_NE16_Job_T",
                  CArgs(21,
                        TCArg("unsigned char * __restrict__", "In"),
                        TCArg("unsigned short int",           "Tile_InW"),
                        TCArg("unsigned short int",           "Tile_InH"),
                        TCArg("v4s",                          "Pad"),
                        TCArg("unsigned short * __restrict__","Filter"),
                        TCArg("int * __restrict__",           "Bias"),
                        TCArg("unsigned char * __restrict__", "Out"),
                        TCArg("unsigned char * __restrict__", "Scale"),
                        TCArg("unsigned char * __restrict__", "ScaleN"),
                        TCArg("unsigned short int",           "Tile_OutW"),
                        TCArg("unsigned short int",           "Tile_OutH"),
                        TCArg("unsigned short int",           "Tile_InFeat"),
                        TCArg("unsigned short int",           "Tile_OutFeat"),
                        TCArg("unsigned char",                "Qw"),
                        TCArg("unsigned int",                 "Default_NE16_Job_Cfg"),
                        TCArg("int",                          "W_Offset"),
                        TCArg("unsigned short int",           "Pad_Val"),
                        TCArg("char",                         "FirstD0"),
                        TCArg("char",                         "LastD0"),
                        TCArg("char",                         "LastT0"),
                        TCArg("void *",                       "NE16JobEvt")
                        )
        );
        LibKernelTemplate("KerConv_NE16_T",
                  CArgs(26,
                        TCArg("unsigned char * __restrict__", "In"),
                        TCArg("unsigned short * __restrict__","Filter"),
                        TCArg("int * __restrict__",           "Bias"),
                        TCArg("unsigned char * __restrict__", "Out"),
                        TCArg("unsigned char * __restrict__", "Scale"),
                        TCArg("unsigned char * __restrict__", "ScaleN"),
                        TCArg("unsigned short int",           "Tile_InFeat"),
                        TCArg("unsigned short int",           "TotalInFeatures"),
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
        LibKernelTemplate("Ker_MM_Conv_NE16_T",
                  CArgs(28,
                        TCArg("unsigned char * __restrict__", "In"),
                        TCArg("unsigned char * __restrict__", "ColBuff"),
                        TCArg("unsigned short * __restrict__","Filter"),
                        TCArg("int * __restrict__",           "Bias"),
                        TCArg("unsigned char * __restrict__", "Out"),
                        TCArg("unsigned char * __restrict__", "Scale"),
                        TCArg("unsigned char * __restrict__", "ScaleN"),
                        TCArg("unsigned short int",           "Tile_InFeat"),
                        TCArg("unsigned short int",           "TotalInFeatures"),
                        TCArg("unsigned short int",           "Tile_InH"),
                        TCArg("unsigned short int",           "Tile_InW"),
                        TCArg("unsigned short int",           "Tile_OutFeat"),
                        TCArg("unsigned short int",           "Tile_OutH"),
                        TCArg("unsigned short int",           "Tile_OutW"),
                        TCArg("unsigned short int",           "Pad_Val"),
                        TCArg("v4s",                          "Pad"),
                        TCArg("unsigned char",                "LastD0"),
                        TCArg("unsigned char",                "FirstD0"),
                        TCArg("unsigned char",                "FirstTile"),
                        TCArg("unsigned char",                "Qw"),
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
                  CArgs(13,
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

        LibKernel("NE16_PrepareJob", CALL_SEQUENTIAL_STRUCT, 0, "Ker_NE16_Job_T", 0);
        LibKernel("NE16_Enable",     CALL_SEQUENTIAL, CArgs(0), "", 0);
        LibKernel("NE16_SoftReset",  CALL_SEQUENTIAL, CArgs(0), "", 0);
        LibKernel("NE16_Disable",    CALL_SEQUENTIAL, CArgs(0), "", 0);
        LibKernel("NE16_FireJob",    CALL_SEQUENTIAL, CArgs(0), "", 0);

        /* Convolution or Linear output reduction with per channel scaling and optional activation. Out != In and In Place (IO)  */
        /* Activation wth tensor centric scaling */

        LibKernel("KerConv3x3Stride1_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4),  3, 3, 1, 1, 1, 1));
        LibKernel("KerConv3x3Stride2_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4),  3, 3, 1, 1, 2, 2));
        LibKernel("KerConv1x1Stride1_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4),  1, 1, 1, 1, 1, 1));
        LibKernel("KerConv1x1StrideS_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4),  1, 1, 1, 1,-1,-1));
        LibKernel("KerConv1x1_SmallHW_StrideS_NE16",CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4),  1, 1, 1, 1,-1,-1));
        LibKernel("KerConv1D_StrideS_NE16",     CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER,0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV1D),0,-1, CNN_Type(0,0,0,0,4), -1, 1,-1, 1,-1, 1));
        LibKernel("KerConvNxMDxDy_StrideSxSy_NE16",CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV), 0, -1, CNN_Type(0,0,0,0,4), -1,-1,-1,-1,-1,-1));
        LibKernel("KerConvDW3x3Stride1_NE16",   CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, -1, CNN_Type(0,0,0,0,4), 3,3,1,1,1,1));
        LibKernel("KerConvDW3x3Stride2_NE16",   CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerConv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_CONV_DW), 0, -1, CNN_Type(0,0,0,0,4), 3,3,1,1,2,2));

        LibKernel("KerLinear_NE16",             CALL_SEQUENTIAL_STRUCT|CALL_HWC_KER, 0, "KerLinear_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_LINEAR), CNN_OperList(1, KOP_NONE), -1, CNN_Type(0,0,4,0,1), 0,0,0,0,0,0));
        LibKernel("Ker_MM_Conv2D_NE16",         CALL_PARALLEL_CC|CALL_HWC_KER,       0, "Ker_MM_Conv_NE16_T",
                CNN_Match(CNN_OperList(1, KOP_MM_CONV), 0, -1, CNN_Type(1,1,0,0,4), -1,-1,1,1,-1,-1));
}


int CNN_MM_ConvolutionNE16(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
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
        int PadValue,

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
        if (Abs(In_DataSize)!=1 && Abs(In_DataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, In_DataSize);
        int Ws = Filter_DataSizeBits;
        unsigned int Wa = 0;
        if (((Ws%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        unsigned int Wp = SPACE_PROP_PAD2PREF;
        unsigned int Bs = Bias_DataSize;
        int ParFeat = 0;
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
        int NeedFcx, NeedFcy, NeedDcx, NeedDcy, NeedScx, NeedScy, NeedFpx, NeedFpy, NeedDpx, NeedDpy, NeedSpx, NeedSpy;
        int UsedWidth, UsedHeight, UsedWc, UsedHc;

        unsigned int InTileCons = 16;
        int OutTileCons = 32;
        int StandAloneAct = (ActOper!=KOP_NONE);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;

        if (!(ConvOper == KOP_CONV))
                GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, ConvOper, expecting KOP_CONV", Name);
        if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL)) // || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL, KOP_AVGPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_RELUM, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

        Wa |= O_NE16_RNN;
        /* When there is a special activation (not supported by the accelerator itself), you need to streamout 32bits and do the act in the cluster but the ((*S) >> N) is done in the accelerator (KOP_DP_REDUCT_NOSCALE) */
        int NeedReductNoScale = !(ActOper == KOP_RELU || ActOper == KOP_NONE);
        /* Also when in/out are 16bits you need to streamout 32bits but here the reduction step will be done in the cluster (KOP_DP_REDUCT) */
        int NeedReductScale = Abs(In_DataSize) == 2;
        int NeedReduct = NeedReductNoScale || NeedReductScale;

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

        /* Im2Col Size is aligned to 16bits for linear padding */
        int Im2ColSize = InFeat*Fcx*Fcy;
        int WBuffSize = ALIGN(Im2ColSize, 4);
        int BuffS = 2*ALIGN(Im2ColSize, 4);

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
        LayerOp *= InFeat;
        if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += OutFeat*Wo*Ho;
        LayerBandwidth += Width*Height*1*InFeat*(OutFeat);
        LayerBandwidth += Wo*Ho*1*OutFeat;
        LayerBandwidth += (Fcx*Fcy*Filter_DataSizeBits*InFeat*(OutFeat)+7)/8;
        LayerBandwidth += Bias_DataSize*OutFeat;

        ConvKerName = CNN_FindMatchingKernelAttr(KOP_MM_CONV, KOP_NONE, ParFeat, CALL_HWC_KER, Abs(In_DataSize), Abs(Out_DataSize), Bias_DataSize, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
                                                 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
        if (ConvKerName==0) GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, Can't find a matching Convolution basic kernel", Name);
        if (PoolOper==KOP_MAXPOOL) {
                PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, 1, CALL_HWC_KER, In_DataSize, 0, 0, 0, Out_DataSize, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
                                                         &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
                if (PoolKerName==0) GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, Can't find a matching Pooling basic kernel", Name);
        }
        if (NeedReduct) {
                DPReductionKerName = CNN_FindMatchingKernelAttr(NeedReductNoScale?KOP_DP_REDUCT_NOSCALE:KOP_DP_REDUCT, ActOper, 1, CALL_HWC_KER, 4, 0, 0, 0, Out_DataSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (DPReductionKerName==0) GenTilingError("CNN_MM_ConvolutionNE16 Kernel: %s, Can't find a matching Reduction basic kernel %d", Name, Out_DataSize);
        }
        // If pooling you need an extra buffer for convout but reduction can be done in the accelerator
        int NeedConvout = NeedReduct || PoolKerName;
        unsigned int Cos = NeedConvout?4:1;

        if (Log) {
                printf("InFeat: %d%s, OutFeat: %d, InFeatCons: %d\n", InFeat, " Im2Col", OutFeat, InTileCons);
                printf("Conv => W:  %4d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]x%d Bits\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy, Filter_DataSizeBits);
                printf("     => H:  %4d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("     ConvOut_DataSize: %d\n", Cos);
                printf("Pool => Wc: %4d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d]\n", UsedWc, PadInp[0], PadInp[1], Wo, Fpx, Fpy);
                printf("     => Hc: %4d, Pad:[%d,%d] => Ho: %d\n", UsedHc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC: %d\n", OverlapC);
                printf("OverlapP: %d\n", OverlapP);
                printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
                if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
                if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
                if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                printf("Nb Oper : %lld\n", LayerOp);
                printf("NeedConvout: %d\n", NeedConvout);
        }
        /* User kernel C arguments */
        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
        Kernel_T *Kernel;

        int StreamoutMode   = 1; // Streamout = apply *Scale >> ScaleN
        int Mode16          = (Abs(In_DataSize) == 2);
        int Streamin        = 0; // Streamin initialized at 0, set to 1 in the basic kernel if multiple chin tile
        int FilterMode      = 3;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0;
        int WOffsetCfg      = 1;
        int QuantRightShift = 0;
        int QuantBits       = (NeedReduct)?2:(Abs(Out_DataSize)==2?1:0); // 00: 8bit, 01: 16bit, 10: 32bit --> If tiling the channel input dimension you need to streamin (need 32 bits output)
        int QuantNoRect     = (Out_DataSize>0)?1:0;
        int NormShift       = 1;
        int NormBias        = 1;
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig(Filter_DataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, \
                                                               WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);

        int Ca=0;
        KCArgs[Ca++] = TCArg(In_DataSize>0?CNN_ArgDataType(In_DataSize,1,1):CNN_ArgDataTypeUns(-In_DataSize,1,1), "In");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "Filter"); // int16_t for 16 chin contributions
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
        KCArgs[Ca++] = TCArg(Out_DataSize>0?CNN_ArgDataType(Out_DataSize,1,1):CNN_ArgDataTypeUns(-Out_DataSize,1,1), "Out");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");

        Kernel = UserKernel(Name,
                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0, InFeat, InTileCons)),
                TILE_HOR|TILE_HWC,
                KCArgs,
                Calls(6,
                        Call("NE16_Enable", LOC_D1_PROLOG, Bindings(0)),
                        Call("NE16_SoftReset", LOC_D0, Bindings(0)),
                        Call(ConvKerName, LOC_D0,
                                Bindings(28,
                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
                                        K_Arg("ColBuff", KER_ARG_TILE),                                         /* Im2Col Buffer */
                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* Conv Bias when depth wise conv*/
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE),                       /* Conv output */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),                           /* Number of input features in this tile */
                                        K_ArgPar("In", KER_ARG_LOADEDPARTILE_SIZE, D0),                     /* Total Number of loaded input features in case of promotion */
                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
                                        K_ArgPar(NeedConvout?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, D1),                          /* Number of output features in this tile */
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE_H),
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE_W),
                                        Imm(PadValue),
                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
                                        K_ArgPred("In", KER_ARG_TILELAST, D0),
                                        K_ArgPred("In", KER_ARG_TILEFIRST, D0),
                                        K_ArgPred("In", KER_ARG_TILEFIRST, T0),                                 /* First Tile */
                                        Imm(Filter_DataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG),                                              /* MOVE TO INFOS !!!! */
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),               /* W_Offset */
                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fx */
                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridex */
                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Dx */
                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fy */
                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridey */
                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Pooling Dy */
                                )
                        ),
                        (NeedReduct==0)?AT_NO_CALL:
                        Call(DPReductionKerName, LOC_D0_EPILOG,                                             /* DP Reduction also take care of optional activation */
                                Bindings(8,
                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* Double precision input tile */
                                        K_Arg(PoolOper?"ConvOut":"Out", KER_ARG_TILE),                          /* Single precision output tile, warning use IO kernel when In=Out */
                                        ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D1):Imm(1),           /* Input tile Number of features */
                                        K_Arg("ConvOut", KER_ARG_TILE_W),                                       /* Input tile width */
                                        K_Arg("ConvOut", KER_ARG_TILE_H),                                       /* Input tile height */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_Arg("Infos", KER_ARG_TILE)                                            /* Infos */
                                )
                        ),
                        (PoolKerName==0)?AT_NO_CALL:
                        Call(PoolKerName, LOC_D0_EPILOG,
                                Bindings(13,
                                        K_Arg("ConvOut", KER_ARG_TILE),
                                        K_Arg("ConvOut", KER_ARG_TILE_W),
                                        K_Arg("ConvOut", KER_ARG_TILE_H),
                                        NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,                 /* Pool Fx */
                                        NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,                 /* Pool Fy */
                                        NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,                 /* Pool Stridex */
                                        NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,                 /* Pool Stridey */
                                        K_ArgPred("ConvOut", KER_ARG_TILEFIRST, T0),            /* First Tile */
                                        K_Arg("ConvOut", KER_ARG_TILE_PAD),                     /* Pool Padding */
                                        K_Arg("Out", KER_ARG_TILE),                             /* Pooling output tile */
                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D1),          /* In Features */
                                        K_Arg("Out", KER_ARG_TILE_W),                           /* Output tile width */
                                        K_Arg("Out", KER_ARG_TILE_H)                            /* Output tile height */
                                )
                        ),
                        Call("NE16_Disable", LOC_D1_EPILOG, Bindings(0))
                ),
                KerArgs(9,
                        KerArgPV("In",    KerArgSpace(2,T0,D0),    O_IN|O_DB|O_HWC,  Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, PadValue, Abs(In_DataSize),   OverlapC, 0, 0, "In"),
                        KerArg ("ColBuff",KerArgSpace(1,T0),       O_BUFF|O_NTILED,    BuffS,   1,                 1, 0, 0, 0, 0),
                        KerArg ("Bias",   KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,      1,   1,                Bs, 0, 0, 0, "Bias"),
                        KerArg ("Scale",  KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,      1,   1,                 1, 0, 0, 0, "Scale"),
                        KerArg ("ScaleN", KerArgSpace(1,D1),       O_IN|O_DB|O_CONST,      1,   1,                 1, 0, 0, 0, "ScaleN"),
                        KerArg ("Filter", KerArgSpace(1,D1),       O_IN|O_DB|O_CONST|Wa, 1, WBuffSize,            Ws, 0, 0, 0, "Filter"),
                        NeedConvout?
                        KerArgP("ConvOut",KerArgSpace(2,T0,D1),    O_BUFF|O_ONETILE|O_HWC,Wc,  Hc,  UsedWc, UsedHc, PadInp, PadInp,        Cos, OverlapP, 0,        0, ""):AT_NO_KER_ARG,
                        KerArg ("Out",    KerArgSpace(2,T0,D1),    O_OUT|O_DB|O_HWC,      Wo,  Ho, Abs(Out_DataSize), 0, 0, 0, "Out"),
                        KerArg ("Infos",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_NE16_DIM, 1,   1, 0, 0, 0, "Infos")
                )
        );
        if (Kernel) {
                int InFeatPadded = (InFeat%16)?16*(InFeat/16+1):InFeat;
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, Abs(In_DataSize));
                AddKernelArgDim(Name, "Filter", 5, OutFeat, Fcx, Fcy, InFeatPadded, 1);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, Abs(Out_DataSize));
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_NE16_DIM, 1);

                AT_PrepareForTest_SQ8(Name, InFeat, OutFeat, Width, Height, Bias_DataSize,
                                      ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, PadInc,
                                      PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PadInp,
                                      ActOper);
        }
        return (Kernel!=0);
}

static Kernel_T *CNN_ConvolutionNE16_Internal(
        char         *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
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
        int PadValue,

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
        if (Abs(In_DataSize)!=1 && Abs(In_DataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, In_DataSize);
        int Ws = Filter_DataSizeBits;
        unsigned long int Wa = 0;
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
        unsigned int InFeatProp = (InFeatOneTile!=0)?SPACE_PROP_ONE_TILE:0;
        int OutTileCons = 32;
        int StandAloneAct = (ActOper!=KOP_NONE);
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        int Log=1;

        if (!(ConvOper == KOP_NONE || ConvOper == KOP_CONV || ConvOper == KOP_CONV_DW))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, ConvOper, expecting KOP_NONE, KOP_CONV or KOP_CONV_DW", Name);
        if (!(PoolOper == KOP_NONE || PoolOper == KOP_MAXPOOL || PoolOper == KOP_AVGPOOL)) // || PoolOper == KOP_AVGPOOL))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, PoolOper, expecting KOP_NONE, KOP_MAXPOOL, KOP_AVGPOOL", Name);
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_RELUM, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

        if (DWConv && (InFeat != OutFeat)) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Depth wise convolution requested with InFeat:%d != OutFeat:%d", Name, InFeat, OutFeat);

        if (DWConv) Wa |= O_NE16_DW; else Wa |= O_NE16_PW;
        if (Fcx==3 && Fcy==3 && ((Scx==1 && Scy==1) || (Scx==2 && Scy==2)) && Dcx == 1 && Dcy == 1) Wa |= O_NE16_3X3;

        int Mode16 = (Abs(In_DataSize) == 2);
        if (Mode16) {
            Wa |= O_NE16_MODE16;
            printf("HERE %ld\n", O_IN|O_DB|O_CONST|Wa);
        }

        unsigned int InTileCons = Mode16?8:16;
        int NeedSetBias = Mode16;
        /* When there is a special activation (not supported by the accelerator itself), you need to streamout 32bits and do the act in the cluster but the ((*S) >> N) is done in the accelerator (KOP_DP_REDUCT_NOSCALE) */
        int NeedReductNoScale = (((InTileCons < InFeat) && !InFeatOneTile) && (!DWConv)) || (!(Wa&O_NE16_3X3) && !(Fcx == 1 && Fcy == 1)) || (!(ActOper == KOP_RELU || ActOper == KOP_NONE));
        /* Also when in/out are 16bits you need to streamout 32bits but here the reduction step will be done in the cluster (KOP_DP_REDUCT) */
        int NeedReductScale = Mode16;
        int NeedReduct = NeedReductNoScale || NeedReductScale;

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

        /* Layer number of operations and memory bandwidth requirements */
        LayerOp += Wc*Hc*Fcx*Fcy*OutFeat;
        if (!DWConv) LayerOp *= InFeat;
        if (PoolOper) LayerOp += OutFeat*Wo*Ho*Fpx*Fpy;
        if (ActOper) LayerOp += OutFeat*Wo*Ho;
        LayerBandwidth += Width*Height*1*InFeat*(DWConv?1:OutFeat);
        LayerBandwidth += Wo*Ho*1*OutFeat;
        LayerBandwidth += (Fcx*Fcy*Filter_DataSizeBits*InFeat*(DWConv?1:OutFeat)+7)/8;
        LayerBandwidth += Bias_DataSize*OutFeat;

        if (ConvOper == KOP_CONV && Height == 1 && Fcx != 1 && Fcy == 1) ConvOper = KOP_CONV1D;
        ConvKerName = CNN_FindMatchingKernelAttr(ConvOper, KOP_NONE, ParFeat, CALL_HWC_KER, Abs(In_DataSize), Abs(Out_DataSize), Bias_DataSize, 0, 4, Fcx, Fcy, Dcx, Dcy, Scx, Scy,
                                                 &NeedFcx, &NeedFcy, &NeedDcx, &NeedDcy, &NeedScx, &NeedScy, 0);
        if (ConvKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Convolution basic kernel", Name);
        int BasicNE16Mode = 0;
        // if ((Fcx==1 && Fcy==1) || (Fcx==3 && Fcy==3 && Scx==1 && Scy==1)) {BasicNE16Mode = 1; printf("BASIC MODE\n");}
        if (PoolOper==KOP_MAXPOOL) {
                PoolKerName = CNN_FindMatchingKernelAttr(PoolOper, KOP_NONE, 1, CALL_HWC_KER, In_DataSize, 0, 0, 0, Out_DataSize, Fpx, Fpy, Dpx, Dpy, Spx, Spy,
                                                         &NeedFpx, &NeedFpy, &NeedDpx, &NeedDpy, &NeedSpx, &NeedSpy, 0);
                if (PoolKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Pooling basic kernel", Name);
        }
        if (NeedReduct) {
                DPReductionKerName = CNN_FindMatchingKernelAttr(NeedReductScale?KOP_DP_REDUCT:KOP_DP_REDUCT_NOSCALE, ActOper, 1, CALL_HWC_KER,
                                                                4, 0, 0, 0, Out_DataSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (DPReductionKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Reduction basic kernel %d %s", Name, Out_DataSize, NeedReductNoScale?"NoScale":"Scale");
        }
        if (NeedSetBias) {
                SetBiasKerName = CNN_FindMatchingKernelAttr(KOP_SETBIAS, KOP_NONE, ParFeat, CALL_HWC_KER, Bias_DataSize,0,0,0,4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching SetBias basic kernel", Name);
        }

        // If pooling you need an extra buffer for convout but reduction can be done in the accelerator
        int NeedConvout = NeedReduct || PoolKerName || NeedSetBias;
        unsigned int Cos = NeedConvout?4:1;

        if (Log) {
                printf("InFeat: %d%s, OutFeat: %d, InFeatCons: %d\n", InFeat, InFeatOneTile?" (FULL)":"(PART)", OutFeat, InTileCons);
                printf("Conv => W:  %4d, Pad:[%d,%d] PadT:[%d,%d] => Wc: %d, Filter:[%d,%d]x%d Bits\n", Width,  PadInc[0], PadInc[1], PadIncT[0], PadIncT[1], Wc, Fcx, Fcy, Filter_DataSizeBits);
                printf("     => H:  %4d, Pad:[%d,%d] PadT:[%d,%d] => Hc: %d\n", Height, PadInc[2], PadInc[3], PadIncT[2], PadIncT[3], Hc);
                printf("     ConvOut_DataSize: %d\n", Cos);
                printf("Pool => Wc: %4d, Pad:[%d,%d] => Wo: %d, Filter:[%d,%d] %d\n", UsedWc, PadInp[0], PadInp[1], Wo, Fpx, Fpy, Mode16);
                printf("     => Hc: %4d, Pad:[%d,%d] => Ho: %d\n", UsedHc, PadInp[2], PadInp[3], Ho);
                printf("OverlapC: %d\n", OverlapC);
                printf("OverlapP: %d\n", OverlapP);
                printf("TileCons: %d, MinTile: %d\n", TileCons, MinTileDim);
                printf("UsedIn  : [%d x %d]\n", UsedWidth, UsedHeight);
                printf("UsedC   : [%d x %d]\n", UsedWc, UsedHc);
                if (SetBiasKerName) printf("%20s: %s\n", "SetBiasKerName", SetBiasKerName);
                if (ConvKerName) printf("%20s: %s\n", "ConvKerName", ConvKerName);
                if (DPReductionKerName) printf("%20s: %s\n", "DPReductionKerName", DPReductionKerName);
                if (PoolKerName) printf("%20s: %s\n", "PoolKerName", PoolKerName);
                printf("Nb Oper : %lld\n", LayerOp);

        }
        /* User kernel C arguments */
        CKernel_Arg_T **KCArgs = AllocateCArgs(7);
        Kernel_T *Kernel;
        // int MinTile = (Fcx==1)?3:5;

        int StreamoutMode   = !Mode16; // Streamout = apply *Scale >> ScaleN
        int Streamin        = Mode16; // Streamin initialized at 0, set to 1 in the basic kernel if multiple chin tile
        int FilterMode      = (Fcx==3 && Fcy==3 && ((Scx==1 && Scy==1) || (Scx==2 && Scy==2)) && Dcx == 1 && Dcy == 1)?(DWConv?1:0):2;
        int LinearMode      = 0;
        int StridedMode     = (Fcx==3 && Fcy==3 && Scx==2 && Scy==2)?1:0;
        int NormBits        = 0;
        int WOffsetCfg      = 1;
        int QuantRightShift = 0;
        int QuantBits       = (NeedReduct)?2:(Abs(Out_DataSize)==2?1:0); // 00: 8bit, 01: 16bit, 10: 32bit --> If tiling the channel input dimension you need to streamin (need 32 bits output)
        int QuantNoRect     = (Out_DataSize>0 || Mode16)?1:0;
        int NormShift       = 1;
        int NormBias        = !Mode16;
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig(Filter_DataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, \
                                                               WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);

        int Ca=0;
        KCArgs[Ca++] = TCArg(In_DataSize>0?CNN_ArgDataType(In_DataSize,1,1):CNN_ArgDataTypeUns(-In_DataSize,1,1), "In");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "Filter"); // int16_t for 16 chin contributions
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bias_DataSize,1,1),   "Bias");
        KCArgs[Ca++] = TCArg(Out_DataSize>0?CNN_ArgDataType(Out_DataSize,1,1):CNN_ArgDataTypeUns(-Out_DataSize,1,1), "Out");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1),  "Scale");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,         1,1), "ScaleN");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,            1,1),  "Infos");

        /* User kernel kernel arguments */
        Object_T **KArgs = AllocateKerArgs(NeedConvout?8:7);
        int Ka=0;
        KArgs[Ka++] = KerArgPV("In",    KerArgSpace(2,T0,D0),    O_IN|O_DB|O_HWC,  Width, Height, UsedWidth, UsedHeight, PadIncT, PadInc, PadValue, Abs(In_DataSize),   OverlapC, 0, TileCons, "In");
        if (MinTileDim && (MinTileDim > TileCons)) SetKerArgMinTileSize(KArgs[Ka-1], MinTileDim);
        KArgs[Ka++] = KerArg ("Bias",   KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         Bs,         0, 0,        0, "Bias");
        KArgs[Ka++] = KerArg ("Scale",  KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         1,          0, 0,        0, "Scale");
        KArgs[Ka++] = KerArg ("ScaleN", KerArgSpace(1,Os),       O_IN|O_DB|O_CONST,  1,     1,                                         1,          0, 0,        0, "ScaleN");
        if (DWConv) {
                KArgs[Ka++] = KerArg ("Filter", KerArgSpace(1,Os|Wp),    O_IN|O_DB|O_CONST|Wa, Fcx, Fcy,                               Ws, 0, 0,        0, "Filter");
        } else {
                KArgs[Ka++] = KerArg ("Filter", KerArgSpace(2,Os,D0|Wp), O_IN|O_DB|O_CONST|Wa, Fcx, Fcy,                               Ws, 0, 0,        0, "Filter");
        }
        if (NeedConvout) 
        KArgs[Ka++] = KerArgP("ConvOut",KerArgSpace(2,T0,Os),    O_BUFF|O_ONETILE|O_HWC,  Wc,    Hc,  UsedWc, UsedHc, PadInp, PadInp,        Cos, OverlapP, 0,        0, "");
        KArgs[Ka++] = KerArg ("Out",    KerArgSpace(2,T0,Os),    O_OUT|O_DB|O_HWC,        Wo,    Ho,                                         Abs(Out_DataSize),0,0,        0, "Out");
        KArgs[Ka++] = KerArg ("Infos",  KerArgSpace(1,T0),       O_IN|O_BUFF|O_NTILED|O_CONST,  AT_INF_NE16_DIM,    1,                      1,          0, 0,        0, "Infos");


        Kernel = UserKernel(Name,
                (DWConv?
                KernelIterSpace(2, IterParSpace(D0|InFeatProp, InFeat, InTileCons), IterTiledSpace(T0)):
                KernelIterSpace(3, IterParSpace(D1, OutFeat, OutTileCons), IterTiledSpace(T0), IterParSpace(D0|InFeatProp, InFeat, InTileCons))),
                TILE_HOR|TILE_HWC,
                KCArgs,
                Calls(10,
                        Call("NE16_Enable", DWConv?LOC_D0_PROLOG:LOC_D1_PROLOG, Bindings(0)),
                        BasicNE16Mode?Call("NE16_SoftReset", DWConv?LOC_D0_PROLOG:LOC_D1_PROLOG, Bindings(0)):AT_NO_CALL,
                        BasicNE16Mode?Call("NE16_PrepareJob", DWConv?LOC_D0_PROLOG:LOC_D1_PROLOG,
                                Bindings(21,
                                        K_Arg("In", KER_ARG_FIRST_TILE),
                                        K_Arg("In", KER_ARG_FIRST_TILE_W),
                                        K_Arg("In", KER_ARG_FIRST_TILE_H),
                                        K_Arg("In", KER_ARG_FIRST_TILE_PAD),
                                        K_Arg("Filter", KER_ARG_FIRST_TILE),
                                        K_Arg("Bias", KER_ARG_FIRST_TILE),
                                        K_Arg("Out", KER_ARG_FIRST_TILE),
                                        K_Arg("Scale", KER_ARG_FIRST_TILE),
                                        K_Arg("ScaleN", KER_ARG_FIRST_TILE),
                                        K_Arg("Out", KER_ARG_FIRST_TILE_W),
                                        K_Arg("Out", KER_ARG_FIRST_TILE_H),
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),
                                        K_ArgPar(NeedConvout?"ConvOut":"Out", KER_ARG_FIRST_PARTILE_SIZE, Os),
                                        Imm(Filter_DataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG),
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),
                                        Imm(PadValue),
                                        Imm(1),
                                        K_ArgParOper("In", KER_ARG_PARTILE_DIM, D0, '=', 1),
                                        Imm(0),
                                        Imm(0)
                                )
                        ):AT_NO_CALL,
                        SetBiasKerName?Call(SetBiasKerName, DWConv?LOC_D0_PROLOG:LOC_D1_PROLOG, 
                                Bindings(6,
                                        K_Arg("ConvOut", KER_ARG_TILE),                                         /* SetBias output tile */
                                        K_Arg("ConvOut", KER_ARG_TILE_W),                                       /* SetBias output tile width */
                                        K_Arg("ConvOut", KER_ARG_TILE_H),                                       /* SetBias output tile height */
                                        ParFeat?K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, Os):Imm(1),           /* Number of output features in this tile */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* SetBias Bias tile */
                                        K_TileOper("Infos", "char *", '@', AT_INF_BIASN)                        /* Bias Norm */
                                )
                        ):AT_NO_CALL,
                        Call("NE16_SoftReset", DWConv?LOC_LOOP:LOC_D0, Bindings(0)),
                        Call(BasicNE16Mode?"NE16_FireJob":ConvKerName, DWConv?LOC_LOOP:LOC_D0,
                                Bindings(BasicNE16Mode?0:26,
                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* Conv Bias when depth wise conv*/
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE),                       /* Conv output */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_ArgPar("Filter", KER_ARG_PARTILE_SIZE, D0),                           /* Number of input features in this tile */
                                        K_ArgPar("Filter", KER_ARG_LOADEDPARTILE_SIZE, D0),                     /* Total Number of loaded input features in case of promotion */
                                        K_Arg("In", KER_ARG_TILE_H),                                            /* Conv input tile height */
                                        K_Arg("In", KER_ARG_TILE_W),                                            /* Conv input tile width */
                                        K_ArgPar(NeedConvout?"ConvOut":"Out", KER_ARG_PARTILE_SIZE, Os),                          /* Number of output features in this tile */
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE_H),
                                        K_Arg(NeedConvout?"ConvOut":"Out", KER_ARG_TILE_W),
                                        Imm(PadValue),
                                        K_Arg("In", KER_ARG_TILE_PAD),                                          /* Conv Padding */
                                        K_ArgPred("In", KER_ARG_TILELAST, D0),
                                        K_ArgPred("In", KER_ARG_TILEFIRST, D0),
                                        Imm(Filter_DataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG),                                              /* MOVE TO INFOS !!!! */ 
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),               /* W_Offset */
                                        NeedFcx?Imm(Fcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fx */
                                        NeedScx?Imm(Scx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridex */
                                        NeedDcx?Imm(Dcx):AT_IGNORE_ARG_BINDING,                                 /* Pooling Dx */
                                        NeedFcy?Imm(Fcy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Fy */
                                        NeedScy?Imm(Scy):AT_IGNORE_ARG_BINDING,                                 /* Pooling Stridey */
                                        NeedDcy?Imm(Dcy):AT_IGNORE_ARG_BINDING                                  /* Pooling Dy */
                                )
                        ),
                        BasicNE16Mode?Call("NE16_PrepareJob", DWConv?LOC_LOOP:LOC_D0,
                                Bindings(21,
                                        K_Arg("In", KER_ARG_NEXT_TILE),
                                        K_Arg("In", KER_ARG_NEXT_TILE_W),
                                        K_Arg("In", KER_ARG_NEXT_TILE_H),
                                        K_Arg("In", KER_ARG_NEXT_TILE_PAD),
                                        K_Arg("Filter", KER_ARG_NEXT_TILE),
                                        K_Arg("Bias", KER_ARG_NEXT_TILE),
                                        K_Arg("Out", KER_ARG_NEXT_TILE),
                                        K_Arg("Scale", KER_ARG_NEXT_TILE),
                                        K_Arg("ScaleN", KER_ARG_NEXT_TILE),
                                        K_Arg("Out", KER_ARG_NEXT_TILE_W),
                                        K_Arg("Out", KER_ARG_NEXT_TILE_H),
                                        K_ArgPar("In", KER_ARG_NEXT_PARTILE_SIZE, D0),
                                        K_ArgPar(NeedConvout?"ConvOut":"Out", KER_ARG_NEXT_PARTILE_SIZE, Os),
                                        Imm(Filter_DataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG),
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),
                                        Imm(PadValue),
                                        K_ArgPred("In", KER_ARG_TILEFIRST, D0),
                                        K_ArgPred("In", KER_ARG_NEXT_TILELAST, D0),
                                        K_ArgPred("In", KER_ARG_NEXT_TILELAST, T0),
                                        Imm(0)
                                )
                        ):AT_NO_CALL,
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
                                        K_Arg("ConvOut", KER_ARG_TILE_W),
                                        K_Arg("ConvOut", KER_ARG_TILE_H),
                                        NeedFpx?Imm(Fpx):AT_IGNORE_ARG_BINDING,                 /* Pool Fx */
                                        NeedFpy?Imm(Fpy):AT_IGNORE_ARG_BINDING,                 /* Pool Fy */
                                        NeedSpx?Imm(Spx):AT_IGNORE_ARG_BINDING,                 /* Pool Stridex */
                                        NeedSpy?Imm(Spy):AT_IGNORE_ARG_BINDING,                 /* Pool Stridey */
                                        K_ArgPred("ConvOut", KER_ARG_TILEFIRST, T0),            /* First Tile */   
                                        K_Arg("ConvOut", KER_ARG_TILE_PAD),                     /* Pool Padding */
                                        K_Arg("Out", KER_ARG_TILE),                             /* Pooling output tile */
                                        K_ArgPar("ConvOut", KER_ARG_PARTILE_SIZE, D1),          /* In Features */
                                        K_Arg("Out", KER_ARG_TILE_W),                           /* Output tile width */
                                        K_Arg("Out", KER_ARG_TILE_H)                            /* Output tile height */
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

                AddKernelArgDim(Name, "In", 4, Height, Width, InFeat, Abs(In_DataSize));
                if (DWConv) AddKernelArgDim(Name, "Filter", 4, Fcx, Fcy, InFeatPadded, 1);
                else AddKernelArgDim(Name, "Filter", 5, OutFeat, Fcx, Fcy, InFeatPadded, 1);
                AddKernelArgDim(Name, "Bias", 2, OutFeat, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 4, Ho, Wo, OutFeat, Abs(Out_DataSize));
                AddKernelArgDim(Name, "Scale", 2, OutFeat, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutFeat, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_NE16_DIM, 1);

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
        int Out_DataSize,
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
        int PadValue,

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
        if (Fcx==1 && Fcy==1 && Scx==1 && Scy==1 && Dcx==1 && Dcy==1 && Height==1 && Width==1) {
                printf("This is a pointwise on 1x1 input --> Mapping to CNN_Linear_NE16\n");
                CNN_LinearAct_NE16(Name, Ctrl, In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, KOP_LINEAR, ActOper);
                return 1;
        }
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
                    In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                    ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PadValue, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                    0, DW?0:1, TileCons);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        else if (OrgTileCons != TileCons) {
                Sol1TileCons = OrgTileCons;
                Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                                In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                                ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PadValue, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                                0, DW?0:1, OrgTileCons);
                if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        }
        if (InFeat>16 && !DW) {
                Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                            In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                        ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PadValue, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
                        0, 0, TileCons);
                if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
                else if (OrgTileCons != TileCons) {
                    Sol2TileCons = OrgTileCons;
                    Ker = CNN_ConvolutionNE16_Internal(Name, Ctrl,
                                In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InFeat, OutFeat, Width, Height,
                            ConvOper, Fcx, Fcy, Dcx, Dcy, Scx, Scy, ConvPad, PadValue, PoolOper, Fpx, Fpy, Dpx, Dpy, Spx, Spy, PoolPad, ActOper,
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
        int Out_DataSize,
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
        if (Abs(In_DataSize)!=1 && Abs(In_DataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, In_DataSize);
        int Ws = Filter_DataSizeBits;
        unsigned int Wp = 0; //SPACE_PROP_PAD2PREF;
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
        char *LinearKerName=0, *ActKerName=0, *SetBiasKerName=0;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;

        /* First try to map on Linear Kernel without reduction, for that In and one full line of Weights must fit into L1 */
        if (LinearOper != KOP_LINEAR) GenTilingError("CNN_LinearAct_SQ8 Kernel: %s, only KOP_LINEAR should be used as LinearOper argument", Name);

        LinearKerName = "KerLinear_NE16";
        if (!(ActOper == KOP_NONE || ActOper == KOP_RELU || ActOper == KOP_RELUN || ActOper == KOP_RELUM || ActOper == KOP_HSIGMOID || ActOper == KOP_HSWISH || ActOper == KOP_LEAKYRELU))
                GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, ActOper, expecting KOP_NONE, KOP_RELU, KOP_RELUN, KOP_RELUM, KOP_HSIGMOID, KOP_HSWISH or KOP_LEAKYRELU", Name);

        /* Also when in/out are 16bits you need to streamout 32bits but here the reduction step will be done in the cluster (KOP_DP_REDUCT) */
        int NeedReductScale = Abs(In_DataSize) == 2; //Abs(Out_DataSize) == 2;
        int NeedSetBias = Abs(In_DataSize) == 2;
        int NeedReductNoScale = ((InTileCons < InDim) && !InFeatOneTile) || NeedSetBias;
        int NeedReduct = NeedReductNoScale || NeedReductScale;

        int NeedLinOut = NeedReduct || NeedSetBias;
        unsigned int Ls = NeedLinOut?4:1;
        if (NeedReduct) {
                ActKerName = CNN_FindMatchingKernelAttr(NeedReductScale?KOP_DP_REDUCT:KOP_DP_REDUCT_NOSCALE, ActOper, 1, CALL_HWC_KER, 4, 0, 0, 0, Out_DataSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                if (ActKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching Reduction basic kernel %d", Name, Out_DataSize);
        }
        if (NeedSetBias) {
                SetBiasKerName = CNN_FindMatchingKernelAttr(KOP_SETBIAS, KOP_NONE, ParFeat, CALL_HWC_KER, Bias_DataSize,0,0,0,4, 0,0,0,0,0,0, 0,0,0,0,0,0, 0);
                if (SetBiasKerName==0) GenTilingError("CNN_ConvolutionPoolAct_NE16 Kernel: %s, Can't find a matching SetBias basic kernel", Name);
        }

        LayerOp += InDim*OutDim;
        if (ActOper) LayerOp += OutDim;
        LayerBandwidth += InDim*OutDim*1;
        LayerBandwidth += OutDim*1;
        LayerBandwidth += InDim*OutDim*1;
        LayerBandwidth += Bs*OutDim;

        int StreamoutMode   = 1;
        int Mode16          = (Abs(In_DataSize) == 2);
        int Streamin        = 0; // Streamin initialized at 0, set to 1 in the basic kernel if multiple chin tile
        int FilterMode      = 3;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0;
        int WOffsetCfg      = 1;
        int QuantRightShift = 0;
        int QuantBits       = (NeedLinOut||ActKerName)?2:((Abs(Out_DataSize)==1)?0:1); // 00: 8bit, 01: 16bit, 10: 32bit
        int QuantNoRect     = (Out_DataSize>0)?1:0;
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
        KCArgs[Ca++] = TCArg(In_DataSize>0?CNN_ArgDataType(In_DataSize,1,1):CNN_ArgDataTypeUns(-In_DataSize,1,1), "In");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "Filter");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(Bs,1,1),   "Bias");
        KCArgs[Ca++] = TCArg(Out_DataSize>0?CNN_ArgDataType(Out_DataSize,1,1):CNN_ArgDataTypeUns(-Out_DataSize,1,1), "Out");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "Scale");
        KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "ScaleN");
        KCArgs[Ca++] = TCArg(CNN_ArgDataType(1,1,1),    "Infos");

        Object_T **KArgs = AllocateKerArgs(7+(NeedLinOut||ActKerName));
        int Ka=0;
        KArgs[Ka++] = KerArg("In",      KerArgSpace(1,D0),   OBJ_IN_DB,            1, 1,  Abs(In_DataSize), 0, 0, 0, "In");
        KArgs[Ka++] = KerArg("Filter",  KerArgSpace(2,D1,D0|Wp), OBJ_IN_DB|O_CONST|Wa, 1, 1,  Ws,           0, 0, 0, "Filter");
        KArgs[Ka++] = KerArg("Bias",    KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  Bs,       0, 0, 0, "Bias");
        if (NeedLinOut||ActKerName)
        KArgs[Ka++] = KerArg("LinOut",  KerArgSpace(1,D1),   O_BUFF|O_ONETILE,     1, 1,  Ls,           0, 0, 0, "");
        KArgs[Ka++] = KerArg("Out",     KerArgSpace(1,D1),   OBJ_OUT_DB,           1, 1,  Abs(Out_DataSize), 0, 0, 0, "Out");
        KArgs[Ka++] = KerArg("Scale",   KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  1,            0, 0, 0, "Scale");
        KArgs[Ka++] = KerArg("ScaleN",  KerArgSpace(1,D1),   OBJ_IN_DB|O_CONST,    1, 1,  1,            0, 0, 0, "ScaleN");
        KArgs[Ka++] = KerArg("Infos",   KerArgSpace(1,T0),   O_IN|O_BUFF|O_NTILED, 1, 1,  AT_INF_NE16_DIM*1, 0, 0, 0, "Infos");

        Kernel = UserKernel(Name,
                KernelIterSpace(3, IterTiledSpace(T0), IterParSpace(D1, OutDim, OutTileCons), IterParSpace(D0|InFeatProp, InDim, InTileCons)),
                TileOrientation,
                KCArgs,
                Calls(6,
                        Call("NE16_Enable", LOC_PROLOG, Bindings(0)),
                        SetBiasKerName?Call(SetBiasKerName, LOC_PROLOG,
                                Bindings(6,
                                        K_Arg("LinOut", KER_ARG_TILE),                                         /* SetBias output tile */
                                        K_Arg("LinOut", KER_ARG_TILE_W),                                       /* SetBias output tile width */
                                        K_Arg("LinOut", KER_ARG_TILE_H),                                       /* SetBias output tile height */
                                        ParFeat?K_ArgPar("LinOut", KER_ARG_PARTILE_SIZE, D1):Imm(1),           /* Number of output features in this tile */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* SetBias Bias tile */
                                        K_TileOper("Infos", "char *", '@', AT_INF_BIASN)                        /* Bias Norm */
                                )
                        ):AT_NO_CALL,
                        Call("NE16_SoftReset", LOC_D0, Bindings(0)),
                        Call(LinearKerName, LOC_D0,
                                Bindings(13,
                                        K_Arg("In", KER_ARG_TILE),                                              /* Conv input tile */
                                        K_Arg("Filter", KER_ARG_TILE),                                          /* Conv filter */
                                        K_Arg("Bias", KER_ARG_TILE),                                            /* Conv Bias when depth wise conv*/
                                        K_Arg(NeedLinOut?"LinOut":"Out", KER_ARG_TILE),                     /* Conv output */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),                   /* Number of input features in this tile */
                                        K_ArgPar(NeedLinOut?"LinOut":"Out", KER_ARG_PARTILE_SIZE, D1),      /* Number of output features in this tile */
                                        K_ArgPred("In", KER_ARG_TILELAST, D0),     /* First In Tile */
                                        K_ArgPred("In", KER_ARG_TILEFIRST, D0),        /* Last In Tile */
                                        K_TileOper("Infos", "int *", '@', AT_INF_NE16_WOFFSET/4),               /* W_Offset */
                                        Imm(Filter_DataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG)                                               /* MOVE TO INFOS !!!! */ 
                                        //K_TileOper("Infos", "unsigned int *", '@', AT_INF_NE16_DEF_CFG),      /* Default NE16 config */
                                )
                        ),
                        (ActKerName==0)?AT_NO_CALL:
                        Call(ActKerName, LOC_D0_EPILOG,                                 /* DP Reduction also take care of optional activation */
                                Bindings(8,
                                        K_Arg("LinOut", KER_ARG_TILE),                                         /* Double precision input tile */
                                        K_Arg("Out", KER_ARG_TILE),                                            /* Single precision output tile, warning use IO kernel when In=Out */
                                        ParFeat?K_ArgPar("LinOut", KER_ARG_PARTILE_SIZE, D1):Imm(1),           /* Input tile Number of features */
                                        Imm(1),                                       /* Input tile width */
                                        Imm(1),                                       /* Input tile height */
                                        K_Arg("Scale", KER_ARG_TILE),                                           /* Per channel scale tile */
                                        K_Arg("ScaleN", KER_ARG_TILE),                                          /* Per channel scale normalization tile */
                                        K_Arg("Infos", KER_ARG_TILE)                                            /* Infos */
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
                AddKernelArgDim(Name, "Filter", 3, OutDim, InDim, Abs(In_DataSize));
                AddKernelArgDim(Name, "Bias", 2, OutDim, Bias_DataSize);
                AddKernelArgDim(Name, "Out", 2, OutDim, Abs(Out_DataSize));
                AddKernelArgDim(Name, "Scale", 2, OutDim, 1);
                AddKernelArgDim(Name, "ScaleN", 2, OutDim, 1);
                AddKernelArgDim(Name, "Infos", 2, AT_INF_NE16_DIM, 1);

                AT_PrepareForTest_SQ8(Name, InDim,OutDim,1,1, Bias_DataSize, LinearOper, 0,0,0,0,0,0,(v4s)0, 0, 0,0,0,0,0,0,(v4s)0, ActOper);
        }
        return Kernel;
}



int CNN_LinearAct_NE16(
        char *Name,

        CNN_GenControl_T *Ctrl,

        int In_DataSize,
        int Out_DataSize,
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

        Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 1, 32);
        if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        else {
                Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 0, 32);
                if (Ker) Sol1 = CopyAndPopUserKernel(Ker);
        }
        Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 1, 1);
        if (Ker) Sol2 = CopyAndPopUserKernel(Ker);
        else {
                Ker = CNN_LinearAct_NE16_Internal(Name, Ctrl, In_DataSize, Out_DataSize, Bias_DataSize, Scale_DataSize, Filter_DataSizeBits, InDim, OutDim, LinearOper, ActOper, 0, 1);
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
