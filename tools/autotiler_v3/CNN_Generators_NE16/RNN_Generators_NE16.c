/*
 * Copyright (C) 2021 GreenWaves Technologies
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

#include "Gap.h"
#include "RNN_Generators_NE16.h"
#include "CNN_Generators_NE16.h"

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

unsigned int NE16_DefaultConfig_RNN(unsigned char Qw,
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

void Load_RNN_NE16_Library()

{
        if (Loaded) return;
        Loaded = 1;
        LibKernelTemplate("KerRNN_NE16_T",
                  CArgs(21,
                        TCArg("unsigned char *__restrict__", "StateInOut"),
                        TCArg("unsigned char *__restrict__", "State"),
                        TCArg("unsigned char *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("void * __restrict__", "OutBuff"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wf"),
                        TCArg("unsigned char *__restrict__", "Wfi"),
                        TCArg("int * __restrict__", "Bf"),
                        TCArg("unsigned char *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );
        LibKernelTemplate("KerRNN_NE16fp_T",
                  CArgs(21,
                        TCArg("unsigned short *__restrict__", "StateInOut"),
                        TCArg("unsigned short *__restrict__", "State"),
                        TCArg("unsigned short *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("void *__restrict__", "OutBuff"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wf"),
                        TCArg("unsigned char *__restrict__", "Wfi"),
                        TCArg("int * __restrict__", "Bf"),
                        TCArg("unsigned short *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );
        LibKernel("RNN_ParKerB32_NE16", CALL_PARALLEL_CC, 0, "KerRNN_NE16_T",
                        CNN_Match(CNN_OperList(1, KOP_RNN), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-1,0,0,0,-1), 0,0,0,0,1,1));
        LibKernel("RNN_ParKerB32_Hard_NE16", CALL_PARALLEL_CC, 0, "KerRNN_NE16_T",
                        CNN_Match(CNN_OperList(1, KOP_RNN), CNN_OperList(1, KOP_HSIGMOID), CALL_PARALLEL, CNN_Type(-1,0,0,0,-1), 0,0,0,0,1,1));
        LibKernel("RNN_ParKerB32_NE16fp", CALL_PARALLEL_CC, 0, "KerRNN_NE16fp_T",
                        CNN_Match(CNN_OperList(1, KOP_RNN), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-2,0,0,0,-2), 0,0,0,0,1,1));

        LibKernelTemplate("KerLSTM_NE16_T",
                  CArgs(34,
                        TCArg("unsigned char *__restrict__", "StateInOut"),
                        TCArg("unsigned char *__restrict__", "State"),
                        TCArg("unsigned char *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("int *__restrict__", "OutBuff1"),
                        TCArg("int *__restrict__", "OutBuff2"),
                        TCArg("int *__restrict__", "OutBuff3"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wf"),
                        TCArg("unsigned char *__restrict__", "Wfi"),
                        TCArg("int * __restrict__", "Bf"),
                        TCArg("unsigned char *__restrict__", "Wi"),
                        TCArg("unsigned char *__restrict__", "Wii"),
                        TCArg("int * __restrict__", "Bi"),
                        TCArg("unsigned char *__restrict__", "Wg"),
                        TCArg("unsigned char *__restrict__", "Wgi"),
                        TCArg("int * __restrict__", "Bg"),
                        TCArg("unsigned char *__restrict__", "Wo"),
                        TCArg("unsigned char *__restrict__", "Woi"),
                        TCArg("int * __restrict__", "Bo"),
                        TCArg("unsigned char *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "LastCell"),
                        TCArg("char", "LastOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );

        LibKernelTemplate("KerLSTM_NE16fp_T",
                  CArgs(34,
                        TCArg("unsigned short *__restrict__", "StateInOut"),
                        TCArg("unsigned short *__restrict__", "State"),
                        TCArg("unsigned short *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("int *__restrict__", "OutBuff1"),
                        TCArg("int *__restrict__", "OutBuff2"),
                        TCArg("int *__restrict__", "OutBuff3"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wf"),
                        TCArg("unsigned char *__restrict__", "Wfi"),
                        TCArg("int * __restrict__", "Bf"),
                        TCArg("unsigned char *__restrict__", "Wi"),
                        TCArg("unsigned char *__restrict__", "Wii"),
                        TCArg("int * __restrict__", "Bi"),
                        TCArg("unsigned char *__restrict__", "Wg"),
                        TCArg("unsigned char *__restrict__", "Wgi"),
                        TCArg("int * __restrict__", "Bg"),
                        TCArg("unsigned char *__restrict__", "Wo"),
                        TCArg("unsigned char *__restrict__", "Woi"),
                        TCArg("int * __restrict__", "Bo"),
                        TCArg("unsigned short *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "LastCell"),
                        TCArg("char", "LastOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );

        LibKernel("LSTM_ParKerB32_NE16", CALL_PARALLEL_CC, 0, "KerLSTM_NE16_T",
                        CNN_Match(CNN_OperList(1, KOP_LSTM), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-1,0,0,0,-1), 0,0,0,0,1,1));
        LibKernel("LSTM_ParKerB32_NE16fp", CALL_PARALLEL_CC, 0, "KerLSTM_NE16fp_T",
                        CNN_Match(CNN_OperList(1, KOP_LSTM), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-2,0,0,0,-2), 0,0,0,0,1,1));

        LibKernelTemplate("KerGRU_NE16_T",
                  CArgs(32,
                        TCArg("unsigned char *__restrict__", "StateInOut"),
                        TCArg("unsigned char *__restrict__", "State"),
                        TCArg("unsigned char *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("int *__restrict__", "OutBuff1"),
                        TCArg("int *__restrict__", "OutBuff2"),
                        TCArg("int *__restrict__", "OutBuff3"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wr"),
                        TCArg("unsigned char *__restrict__", "Wri"),
                        TCArg("int * __restrict__", "Br"),
                        TCArg("unsigned char *__restrict__", "Wz"),
                        TCArg("unsigned char *__restrict__", "Wzi"),
                        TCArg("int * __restrict__", "Bz"),
                        TCArg("unsigned char *__restrict__", "Wh"),
                        TCArg("unsigned char *__restrict__", "Whi"),
                        TCArg("int * __restrict__", "Bwh"),
                        TCArg("int * __restrict__", "Brh"),
                        TCArg("unsigned char *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "LastCell"),
                        TCArg("char", "LastOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );

        LibKernelTemplate("KerGRU_NE16fp_T",
                  CArgs(32,
                        TCArg("unsigned short *__restrict__", "StateInOut"),
                        TCArg("unsigned short *__restrict__", "State"),
                        TCArg("unsigned short *__restrict__", "Xin"),
                        TCArg("unsigned char *__restrict__", "ScaleNorm"),
                        TCArg("int *__restrict__", "OutBuff1"),
                        TCArg("int *__restrict__", "OutBuff2"),
                        TCArg("int *__restrict__", "OutBuff3"),
                        TCArg("unsigned short int", "DimState"),
                        TCArg("unsigned short int", "DimIn"),
                        TCArg("unsigned short int", "DimStateInt"),
                        TCArg("unsigned short int", "DimInInt"),
                        TCArg("unsigned char *__restrict__", "Wr"),
                        TCArg("unsigned char *__restrict__", "Wri"),
                        TCArg("int * __restrict__", "Br"),
                        TCArg("unsigned char *__restrict__", "Wz"),
                        TCArg("unsigned char *__restrict__", "Wzi"),
                        TCArg("int * __restrict__", "Bz"),
                        TCArg("unsigned char *__restrict__", "Wh"),
                        TCArg("unsigned char *__restrict__", "Whi"),
                        TCArg("int * __restrict__", "Bwh"),
                        TCArg("int * __restrict__", "Brh"),
                        TCArg("unsigned short *__restrict__", "Hout"),
                        TCArg("unsigned short int", "Nout"),
                        TCArg("signed char *__restrict__", "Infos"),
                        TCArg("char", "FirstCell"),
                        TCArg("char", "FirstOut"),
                        TCArg("char", "LastCell"),
                        TCArg("char", "LastOut"),
                        TCArg("char", "FilterDataSizeBits"),
                        TCArg("int", "Default_NE16_Job_Cfg"),
                        TCArg("char", "Reset"),
                        TCArg("int", "TileOffset")
                       )
        );

        LibKernel("GRU_ParKerB32_NE16", CALL_PARALLEL_CC, 0, "KerGRU_NE16_T",
                        CNN_Match(CNN_OperList(1, KOP_GRU), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-1,0,0,0,-1), 0,0,0,0,1,1));
        LibKernel("GRU_ParKerB32_NE16fp", CALL_PARALLEL_CC, 0, "KerGRU_NE16fp_T",
                        CNN_Match(CNN_OperList(1, KOP_GRU), CNN_OperList(1, KOP_SIGMOID), CALL_PARALLEL, CNN_Type(-2,0,0,0,-2), 0,0,0,0,1,1));

}

int RNN_Sequence_NE16(int Nc, int K0, int K1, int *n1, int *n2, int *n3, int *n2_io)

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

typedef Kernel_T* (*SequenceFunction_T)(
        char *,                 // Name,
        CNN_GenControl_T *,     // Ctrl,
        char *,                 // RNNKerName,

        int,                    // BiasDataSize,
        int,                    // FeatDataSize,
        int,                    // FilterDataSizeBits,

        int,                    // UseHardAct,
        int,                    // AlwaysReset,
        int,                    // NCells,
        int,                    // DimState,
        int,                    // DimIn,
        int,                    // UseIn,
        int,                    // ExposeSequence,
        int,                    // Buffer,
        int,                    // OutSizeConstraint
        int,                    // FirstSeq,
        int,                    // LastSeq,
        int,                    // Revert,
        int                     // Dynamic
        );

int TryKernelSolution(
        SequenceFunction_T SequenceFunction,
        CNN_GenControl_T *Ctrl,
        char *KerName,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,
        int UseHardAct,
        int AlwaysReset,
        int DimState,
        int DimIn,
        int DoBuffer,
        int MinOutputs,
        int Revert,
        int Dynamic,
        char * G1_Name,
        char * G2_Name,
        char * G3_Name,
        int N1,
        int N2,
        int N3,
        int N2_IO
) 
{
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
        int Ok = 1;
        Kernel_T *Sol1=0, *Sol2=0, *Sol3=0;
        Kernel_T *PtSol1=0, *PtSol2=0, *PtSol3=0;
        if (N1>0) {
                Sol1 = SequenceFunction(
                        G1_Name, Ctrl, KerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, N1, DimState, DimIn, 1,     0,
                        (DoBuffer)?O_BUFF:0, MinOutputs, 1, 0, Revert, Dynamic);
                if (Sol1==0) Ok=0; else PtSol1 = CopyAndPopUserKernel(Sol1);
        }
                
        if (Ok&&N2>0) {
                Sol2 = SequenceFunction(
                        G2_Name, Ctrl, KerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, N2, DimState, DimIn, N2_IO, N2_IO,
                        (DoBuffer)?(N1?(O_BUFF|O_NO_LOAD):O_BUFF):0, MinOutputs, (N1==0), (N3==0), Revert, Dynamic && (N1==0));
                if (Sol2==0) Ok=0; else PtSol2 = CopyAndPopUserKernel(Sol2);
        }
        if (Ok&&N3>0) {
                Sol3 = SequenceFunction(
                        G3_Name, Ctrl, KerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, N3, DimState, DimIn, 0,     1,
                        (DoBuffer)?((N1||N2)?(O_BUFF|O_NO_LOAD):O_BUFF):0, MinOutputs, 0, 1, Revert, 0);
                if (Sol3==0) Ok=0; else PtSol3 = CopyAndPopUserKernel(Sol3);
        }
        if (Ok) {
                if (Sol1) PushBackUserKernel(PtSol1); 
                if (Sol2) PushBackUserKernel(PtSol2); 
                if (Sol3) PushBackUserKernel(PtSol3); 
        }
        AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
        return Ok;
}

#define ROUND_UP_N(__x, __n) ((((__x)/(__n))*(__n))+((__x)%(__n)?(__n):0))

static Kernel_T *RNN_Stack_Seq_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,
        char *RNNKerName,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int UseHardAct,
        int AlwaysReset,
        int NCells,
        int DimState,
        int DimIn,
        int UseIn,
        int ExposeSequence,
        int Buffer,
        int OutSizeConstraint,
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
        printf("RNN Segment: %s NC: %d, First: %d, Last: %d, UseIn: %d, UseHardAct: %d Qw: %d\n",
                Name, NCells, FirstSeq, LastSeq, UseIn, UseHardAct, FilterDataSizeBits);
        if (Abs(FeatDataSize)!=1 && Abs(FeatDataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, FeatDataSize);

        // Set bit mode if != 8 databits
        int Ws = FilterDataSizeBits;
        unsigned int Wa = 0;
        if (Ws%8) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_RNN;

        // setup default job
        int Mode16          = (Abs(FeatDataSize) == 2);
        int StreamoutMode   = (Mode16?0:1);
        int Streamin        = (Mode16?1:0); // 16 bit always streams in
        int FilterMode      = 2;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0; // Byte norms and scales
        int WOffsetCfg      = 1; // Weight 0 point dynamic
        int QuantRightShift = 0;
        int QuantBits       = 2; // 00: 8bit, 01: 16bit, 10: 32bit
        int QuantNoRect     = 1;
        int NormBias        = (Mode16?0:1); // Bias on by default - switch off if not needed
        int NormShift       = (Mode16?0:1);
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig_RNN(
                FilterDataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin,
                WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);
        int ParFeat = 1;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
        unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);
        int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;

        if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;

        LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState + DimState);
        LayerBandwidth = NCells*((DimState + DimIn)*FeatDataSize + (DimState + DimIn)*DimState*1 + (DimState)*BiasDataSize + DimState*FeatDataSize);

        int IntKi = ROUND_UP_N(DimState, (Mode16?32:16));
        int IntKiInput = (UseIn?ROUND_UP_N(DimIn, (Mode16?32:16)):0);
        int IntStateSize = IntKi + IntKiInput;
        printf("IntKi %d IntKiInput %d IntStateSize %d dimin %d bias_ds %d\n", IntKi, IntKiInput, IntStateSize, DimIn, BiasDataSize);
        Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(3, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterParSpace(D1, DimState, OutSizeConstraint), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(11,
                      (Dynamic)?		  TCArg(CNN_ArgDataType(4,0,0),		        "NCells"):AT_NO_C_ARG,
                      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Sin"):AT_NO_C_ARG,
                      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Sout"):AT_NO_C_ARG,
                      (UseIn)?			  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Xin"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wf"),
                      (UseIn)?                    TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wfi"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bf"),
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "ScaleNorm"),
                      (ExposeSequence)?		  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Hout"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(1,1,1),                 "Infos"),
                      (!AlwaysReset)?		  TCArg(CNN_ArgDataType(1,0,0),                 "Reset"):AT_NO_C_ARG
                ),
                Calls(4,
                        FirstSeq?Call("NE16_Enable", LOC_D0_PROLOG, Bindings(0)):AT_NO_CALL,
                        Call("NE16_SoftReset", LOC_D1, Bindings(0)),
                        Call(RNNKerName, LOC_D1,
                                Bindings(21,
                                        (!(FirstSeq&&AlwaysReset))?K_Arg("Sin",  KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("Sout",  KER_ARG_TILE):Imm(0)),
                                        K_Arg("State",  KER_ARG_TILE),
                                        UseIn?K_Arg("Xin", KER_ARG_TILE):Imm(0),
                                        K_Arg("ScaleNorm",  KER_ARG_TILE),
                                        K_Arg("OutBuff",  KER_ARG_TILE),
                                        Imm(DimState),
                                        Imm(DimIn),
                                        Imm(IntKi),
                                        Imm(IntKiInput),
                                        K_Arg("Wf",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wfi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bf",  KER_ARG_TILE),
                                        (ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
                                        K_ArgPar("Bf", KER_ARG_PARTILE_SIZE, D1),
                                        K_Arg("Infos", KER_ARG_TILE),
                                        K_ArgPred("Wf", KER_ARG_TILEFIRST, D0),
                                        K_ArgPred("Wf", KER_ARG_TILEFIRST, D1),
                                        Imm(FilterDataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG), 
                                        AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset"),
                                        BindKExpr("KArg(Bf, TileBase, D1)")
                                        )
                        ),
                        LastSeq?Call("NE16_Disable", LOC_D0_EPILOG, Bindings(0)):AT_NO_CALL

                ),
                KerArgs(11,
                        KerArg("Wf",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,  IntKi,                                        1, Ws,           0, 0, 0, "Wf"),
                        (UseIn)?
                        KerArg("Wfi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,  IntKiInput,                                   1, Ws,           0, 0, 0, "Wfi"):AT_NO_KER_ARG,
                        // When in SeparateInAndState input bias is interleaved with state bias
                        KerArg("Bf",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,
                                                                                              2,                                            1, BiasDataSize, 0, 0, OutSizeConstraint, "Bf"),
                        // When in SeparateInAndState input Scale and ScaleN is interleaved with state Scale and ScaleN
                        // Otherwise interleaved Scale and ScaleN for sum of input and state
                        KerArg("ScaleNorm",KerArgSpace(1,D1),   O_IN|O_DB|O_CONST|O_NE16_INTER|Buffer,
                                                                                              2*(UseIn?2:1),                                1, 1,            0, 0, 0, "ScaleNorm"),
                        KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,      	      IntStateSize,                                 1, FeatDataSize, 0, 0, 0, ""),
                        (!(FirstSeq&&AlwaysReset))?
                        KerArg("Sin",     KerArgSpace(1,T0),    Si_Attr|O_BUFF|O_NTILED,      DimState,                                     1, FeatDataSize, 0, 0, 0, "Sin"):AT_NO_KER_ARG,
                        (!(LastSeq&&AlwaysReset))?
                        ((!(FirstSeq&&AlwaysReset))?
                        KerArgAliased("Sout", KerArgSpace(1,T0),1,So_Attr|O_BUFF|O_NTILED,    DimState,                                     1, FeatDataSize, 0, 0, 0, "Sout"):
                        KerArg("Sout",    KerArgSpace(1,T0),    So_Attr|O_BUFF|O_NTILED,      DimState,                                     1, FeatDataSize, 0, 0, 0, "Sout")):AT_NO_KER_ARG,
                        (UseIn)?
                        KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                    DimIn,                                        1, FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
                        KerArg("OutBuff", KerArgSpace(1,T0),    O_BUFF|O_ONETILE,             DimState*(Mode16?2:1),                        1, 4, 0, 0, 0, ""),
                        (ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,RD0,D1),O_OUT|O_DB,                   1,                                            1, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
                        KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST, RNN_NE16_CELL_INFOS,                          1, 1,            0, 0, 0, "Infos")
                )
        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sin",   2,    DimState, FeatDataSize);
                if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "Sout",  2,    DimState, FeatDataSize);
                if (UseIn)			AddKernelArgDim(Name, "Xin",   3,    NCells, DimIn, FeatDataSize);
                                                AddKernelArgDim(Name, "ScaleNorm",   3, 2*(UseIn?2:1), DimState, 1);
                                                AddKernelArgDim(Name, "Wf",    3,    DimState, FilterDataSizeBits*IntKi/8, 1);
                if (UseIn)                      AddKernelArgDim(Name, "Wfi",   3,    DimState, FilterDataSizeBits*IntKiInput/8, 1);
                                                AddKernelArgDim(Name, "Bf",    3,    2, DimState, BiasDataSize);
                                                AddKernelArgDim(Name, "Infos", 3,    1, RNN_NE16_CELL_INFOS, 1);
                if (ExposeSequence)		AddKernelArgDim(Name, "Hout",  3,    NCells, DimState, FeatDataSize);
        }

        return Kernel;
}

int RNN_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

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
        unsigned int Wa = 0;
        int Ws = FilterDataSizeBits;
        if (((FilterDataSizeBits%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_LIN;
        if (K0<1) GenTilingError("RNN_Stack_NE16, %s, K0, At least one input is expected\n", Name, K0);
        if (K1<1) GenTilingError("RNN_Stack_NE16, %s, K1, At least one output is expected\n", Name, K1);
        if (K0>NCells) GenTilingError("RNN_Stack_NE16, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
        if (K1>NCells) GenTilingError("RNN_Stack_NE16, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

        int UseHardAct = 0;
        if (Ctrl) {
                if (Ctrl->RNNUseHardActivation != -1) UseHardAct = Ctrl->RNNUseHardActivation;
        }

        int ParFeat = 1;

        unsigned S_Attr = 0 | ((!AlwaysReset)?O_IN:0) | ((!AlwaysReset)?O_OUT:0);
        char *RNNKerName = CNN_FindMatchingKernel(
                KOP_RNN, (UseHardAct?KOP_HSIGMOID:KOP_SIGMOID), CALL_PARALLEL,
                FeatDataSize, 0, 0, 0, FeatDataSize, 0,0,0,0,1,1, 0,0,0,0, 0, 0, 0);

        if (!RNNKerName) GenTilingError("RNN_Stack_NE16 Kernel: %s, Can't find a matching basic kernel", Name);

        FeatDataSize = Abs(FeatDataSize);

        char *G1_Name=0, *G2_Name=0, *G3_Name=0;
        int N1, N2, N3, N2_IO, Seq = RNN_Sequence_NE16(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);

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
                printf("Use hard act: %d\n", UseHardAct);
        }

        if (!((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("RNN_NE16 is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
        OpenKernelGroup(Name);
        if (Dynamic)
                UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));
        int Ok = 0;
        G1_Name = AppendNames(Name, "G1");
        G2_Name = AppendNames(Name, "G2");
        G3_Name = AppendNames(Name, "G3");

        int DoBuffer = 1, DoConstraint = 32;
        while (Ok==0) {
                Ok = TryKernelSolution(
                        RNN_Stack_Seq_NE16, Ctrl, RNNKerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, DimState, DimIn, DoBuffer, DoConstraint, Revert, Dynamic,
                        G1_Name, G2_Name, G3_Name, N1, N2, N3, N2_IO);
                if (DoBuffer) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence with all coeffs promoted to buffer\n");
                        } else {
                                if (Log) printf("Failed to map sequence with all coeffs promoted to buffer, reverting to tile based\n");
                                DoBuffer = 0;
                        }
                } else if (DoConstraint) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with %d output size constraint\n", DoConstraint);
                        } else {
                                if (Log) printf("Failed to map sequence tile based with %d output size constraint, relaxing constraint\n", DoConstraint);
                                DoConstraint = (DoConstraint>16)?DoConstraint/2:0;
                        }
                } else {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with no output size constraint\n");
                        } else {
                                if (Log) printf("No solution found\n");
                                break;
                        }
                }
        }
        CloseKernelGroupNoMerge();
        if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;
        int NumCArgs = 7 + 2*(AlwaysReset==0) + (N1!=0) + (N2!=0 && N3!=0) + (Dynamic!=0);
        GroupCArgs = AllocateCArgs(NumCArgs);
        int A= 0;

        if (Dynamic) 		GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0), 	        "NCells");
        if (!AlwaysReset) 	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Hinout");
        if (N1!=0) 	   	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "G1O");
        if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "G2O");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Xin");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),              "ScaleNorm");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wf");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wfi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bf");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Hout");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),                 "Infos");
        if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),                 "Reset");

        GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
        A=0;
        if (Dynamic) {
                if (N1>0) {
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(10,
                                                        BindKGExpr("CArg(NCells)-1"),
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O"),
                                                        BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(11, (N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
                                                            (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                            (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                            BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
                                                            C_Arg("Wf"),
                                                            C_Arg("Wfi"),
                                                            C_Arg("Bf"),
                                                            C_Arg("ScaleNorm"),
                                                            C_Arg("Hout"),
                                                            C_Arg("Infos"),
                                                            (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                            )
                                                );
        } else {
                printf("--- %d, %d %d %d %d, %d\n", Revert, N1, N2, N3, N2_IO, DimIn);
                if (N1>0) {
                        printf("do N1\n");
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(9,
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O"),
                                                        Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO) {
                        printf("do N2 IO\n");
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(10, (N1)?C_Arg("G1O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                            (N3)?C_Arg("G2O"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                            Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
                                                            C_Arg("Wf"),
                                                            C_Arg("Wfi"),
                                                            C_Arg("Bf"),
                                                            C_Arg("ScaleNorm"),
                                                            C_Arg("Hout"),
                                                            C_Arg("Infos"),
                                                            (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                            )
                                                );
                }
                if (N2>0&&!N2_IO) {
                        printf("do N2 !IO\n");
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(7, C_Arg("G1O"),
                                                            C_Arg("G2O"),
                                                            C_Arg("Wf"),
                                                            C_Arg("Bf"),
                                                            C_Arg("ScaleNorm"),
                                                            C_Arg("Infos"),
                                                            (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                            )
                                                );
                }
                if (N3>0) {
                        printf("do N3\n");
                        GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
                                                Bindings(8, (N2)?C_Arg("G2O"):C_Arg("G1O"),
                                                            (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                            C_Arg("Wf"),
                                                            C_Arg("Bf"),
                                                            C_Arg("ScaleNorm"),
                                                            C_Arg("Hout"),
                                                            C_Arg("Infos"),
                                                            (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                            )
                                                );
                }
        }
        GroupKerArgs = AllocateKerArgs(7+ (Dynamic!=0) + (N1!=0) + (N2!=0 && N3!=0)+ 2*(AlwaysReset==0));
        A = 0;
        if (Dynamic)		GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,   	     1,                                  4,            "NCells");
        if (AlwaysReset==0) 	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
        if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O");
        if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O");
                                GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,   	     DimIn*K0,                           FeatDataSize, "Xin");
                                GroupKerArgs[A++] = KerGroupArg("ScaleNorm",     O_IN,       4*DimState,                         1,            "ScaleNorm");
                                GroupKerArgs[A++] = KerGroupArg("Wf",     O_IN|Wa,   	     DimState*DimState,                  Ws,           "Wf");
                                GroupKerArgs[A++] = KerGroupArg("Wfi",     O_IN|Wa,   	     DimIn*DimState,                     Ws,           "Wfi");
                                GroupKerArgs[A++] = KerGroupArg("Bf",     O_IN,   	     DimState,                           BiasDataSize, "Bf");
                                GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,  	     DimState*K1,                        FeatDataSize, "Hout");
                                GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,   	     RNN_CELL_INFOS,                     1,            "Infos");
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


static Kernel_T *LSTM_Stack_Seq_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,
        char *LSTMKerName,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int HardAct,
        int AlwaysReset,
        int NCells,
        int DimState,
        int DimIn,
        int UseIn,
        int ExposeSequence,
        int Buffer,
        int OutSizeConstraint,
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
        printf("LSTM Segment: %s NC: %d, First: %d, Last: %d, UseIn: %d, Hard Act: %d Always Reset: %d Dynamic: %d ExposeSeq: %d Feat: %d Buffer: %d\n",
                Name, NCells, FirstSeq, LastSeq, UseIn, HardAct, AlwaysReset, Dynamic, ExposeSequence, FeatDataSize, Buffer);
        if (Abs(FeatDataSize)!=1 && Abs(FeatDataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, FeatDataSize);

        int Mode16          = (Abs(FeatDataSize) == 2);
        int GatePrenorm = (Mode16?8:0);

        if (Ctrl) {
                // Not used currently
                if (Ctrl->GatePrenorm != -1 && Mode16) GatePrenorm = Ctrl->GatePrenorm;
        }

        // Set bit mode if != 8 databits
        int Ws = FilterDataSizeBits;
        unsigned int Wa = 0;
        if (Ws%8) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_RNN;

        // setup default job
        int StreamoutMode   = (Mode16?0:1);
        int Streamin        = (Mode16?1:0); // 16 bit always streams in
        int FilterMode      = 2;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0; // Byte norms and scales
        int WOffsetCfg      = 1; // Weight 0 point dynamic
        int QuantRightShift = 0;
        int QuantBits       = 2; // 00: 8bit, 01: 16bit, 10: 32bit
        int QuantNoRect     = 1;
        int NormBias        = (Mode16?0:1); // Bias on by default - switch off if not needed
        int NormShift       = (Mode16?0:1);
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig_RNN(
                FilterDataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin,
                WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);
        Tile_Orientation_T TileOrientation = TILE_HOR;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
        unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);
        int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;
        if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;

        int Din = UseIn?DimIn:0;

        LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState*4 + DimState*4 + DimState*4);
        LayerBandwidth = NCells*((DimState * 2 + DimIn)*FeatDataSize + 4 * (DimState + DimIn)*DimState*1 + 4 * (DimState)*BiasDataSize + DimState*FeatDataSize);

        int DimStateInt = ROUND_UP_N(DimState, (Mode16?32:16));
        int DimInInt = (UseIn?ROUND_UP_N(DimIn, (Mode16?32:16)):0);
        printf("DimInInt %d DimStateInt %d DimIn %d DimState %d bias_ds %d\n", DimInInt, DimStateInt, DimIn, DimState, BiasDataSize);

        Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(3, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterParSpace(D1, DimState, OutSizeConstraint), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(22,
                      (Dynamic)?		  TCArg(CNN_ArgDataType(4,0,0),		        "NCells"):AT_NO_C_ARG,
                      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataType(FeatDataSize,1,1),   "SCin"):AT_NO_C_ARG,
                      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "SHin"):AT_NO_C_ARG,
                      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataType(FeatDataSize,1,1),   "SCout"):AT_NO_C_ARG,
                      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "SHout"):AT_NO_C_ARG,
                      (UseIn)?			  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Xin"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wf"),
                      (UseIn)? TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wfi"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bf"),
                                                        TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wi"),
                      (UseIn)? TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wii"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bi"),
                                                        TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wg"),
                      (UseIn)? TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wgi"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bg"),
                                                        TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wo"),
                      (UseIn)? TCArg(CNN_ArgDataTypeUns(1,1,1),              "Woi"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bo"),
                                                        TCArg(CNN_ArgDataTypeUns(1,1,1),              "ScaleNorm"),
                      (ExposeSequence)?		  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Hout"):AT_NO_C_ARG,
                                                        TCArg(CNN_ArgDataType(1,1,1),                 "Infos"),
                      (!AlwaysReset)?		  TCArg(CNN_ArgDataType(1,0,0),                 "Reset"):AT_NO_C_ARG
                ),
                Calls(4,
                        FirstSeq?Call("NE16_Enable", LOC_D0_PROLOG, Bindings(0)):AT_NO_CALL,
                        Call("NE16_SoftReset", LOC_LOOP, Bindings(0)),
                        Call(LSTMKerName, LOC_LOOP,
                                Bindings(34,
                                        (!(FirstSeq&&AlwaysReset))?K_Arg("SCin",  KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("SCout",  KER_ARG_TILE):Imm(0)),
                                        K_Arg("State",  KER_ARG_TILE),
                                        UseIn?K_Arg("Xin", KER_ARG_TILE):Imm(0),
                                        K_Arg("ScaleNorm",  KER_ARG_TILE),
                                        K_Arg("OutBuff1",  KER_ARG_TILE),
                                        K_Arg("OutBuff2",  KER_ARG_TILE),
                                        K_Arg("OutBuff3",  KER_ARG_TILE),
                                        Imm(DimState),
                                        Imm(DimIn),
                                        Imm(DimStateInt),
                                        Imm(DimInInt),
                                        K_Arg("Wf",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wfi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bf",  KER_ARG_TILE),
                                        K_Arg("Wi",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wii",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bi",  KER_ARG_TILE),
                                        K_Arg("Wg",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wgi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bg",  KER_ARG_TILE),
                                        K_Arg("Wo",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Woi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bo",  KER_ARG_TILE),
                                        (ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
                                        K_ArgPar("Bf", KER_ARG_PARTILE_SIZE, D1),
                                        K_Arg("Infos", KER_ARG_TILE),
                                        K_ArgPred("Wf", KER_ARG_TILEFIRST, D0),
                                        K_ArgPred("Wf", KER_ARG_TILEFIRST, D1),
                                        K_ArgPred("Wf", KER_ARG_TILELAST, D0),
                                        K_ArgPred("Wf", KER_ARG_TILELAST, D1),
                                        Imm(FilterDataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG), 
                                        AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset"),
                                        BindKExpr("KArg(Bf, TileBase, D1)")
                                        )
                        ),
                        LastSeq?Call("NE16_Disable", LOC_D0_EPILOG, Bindings(0)):AT_NO_CALL

                ),
                KerArgs(24,
                        KerArg("Wf",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wf"),
                        (UseIn)?
                        KerArg("Wfi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Wfi"):AT_NO_KER_ARG,
                        KerArg("Bf",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Bf"),
                        KerArg("Wi",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wi"),
                        (UseIn)?
                        KerArg("Wii",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Wii"):AT_NO_KER_ARG,
                        KerArg("Bi",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Bi"),
                        KerArg("Wg",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wg"),
                        (UseIn)?
                        KerArg("Wgi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Wgi"):AT_NO_KER_ARG,
                        KerArg("Bg",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Bg"),
                        KerArg("Wo",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,                                    1, Ws,            0, 0, 0, "Wo"),
                        (UseIn)?
                        KerArg("Woi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Woi"):AT_NO_KER_ARG,
                        KerArg("Bo",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Bo"),
                        KerArg("ScaleNorm", KerArgSpace(1,D1),  O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  8 * (UseIn?2:1),  1, 1,            0, 0, 0, "ScaleNorm"),

                        KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,      	                DimStateInt+DimState+DimInInt,
                                                                                                                          1, FeatDataSize, 0, 0, 0, ""),
                        (!(FirstSeq&&AlwaysReset))?
                        KerArg("SCin",     KerArgSpace(1,T0),   Si_Attr|O_BUFF|O_NTILED,                DimState,         1, FeatDataSize, 0, 0, 0, "SCin"):AT_NO_KER_ARG,
                        (!(FirstSeq&&AlwaysReset))?
                        KerArg("SHin",     KerArgSpace(1,T0),   Si_Attr|O_BUFF|O_NTILED|O_STACK_PRED,   DimState,         1, FeatDataSize, 0, 0, 0, "SHin"):AT_NO_KER_ARG,

                        (!(LastSeq&&AlwaysReset))?
                        ((!(FirstSeq&&AlwaysReset))?
                        KerArgAliased("SCout", KerArgSpace(1,T0), 1, So_Attr|O_BUFF|O_NTILED,           DimState,         1, FeatDataSize, 0, 0, 0, "SCout"):
                        KerArg("SCout",    KerArgSpace(1,T0),   So_Attr|O_BUFF|O_NTILED,                DimState,         1, FeatDataSize, 0, 0, 0, "SCout")):AT_NO_KER_ARG,

                        (!(LastSeq&&AlwaysReset))?
                        ((!(FirstSeq&&AlwaysReset))?
                        KerArgAliased("SHout", KerArgSpace(1,T0), 1, So_Attr|O_BUFF|O_NTILED,           DimState,         1, FeatDataSize, 0, 0, 0, "SHout"):
                        KerArg("SHout",    KerArgSpace(1,T0),   So_Attr|O_BUFF|O_NTILED,                DimState,         1, FeatDataSize, 0, 0, 0, "SHout")):AT_NO_KER_ARG,

                        (UseIn)?
                        KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                              DimIn,            1, FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
                        KerArg("OutBuff1", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       (Mode16?2:1),     1, 4,            0, 0, 0, ""),
                        KerArg("OutBuff2", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       (Mode16?2:1),     1, 4,            0, 0, 0, ""),
                        KerArg("OutBuff3", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       (Mode16?2:1),     1, 4,            0, 0, 0, ""),
                        (ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,RD0,D1),O_OUT|O_DB,                             1,                1, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
                        KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,           (Mode16?LSTM_NE16_CELL_INFOS_SHORT:LSTM_NE16_CELL_INFOS),
                                                                                                                          1, 1,        0, 0, 0, "Infos")
                )
        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "SCin",   2, DimState,                    FeatDataSize);
		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHin",  2, DimState, FeatDataSize);
                if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "SCout",  2, DimState,                    FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHout", 2, DimState, FeatDataSize);
                                                AddKernelArgDim(Name, "ScaleNorm",   3, 8*(UseIn?2:1), DimState, 1);
                if (UseIn)			AddKernelArgDim(Name, "Xin",    3, NCells,      DimIn,          FeatDataSize);
                                                AddKernelArgDim(Name, "Wf",     3, DimState,    FilterDataSizeBits*DimStateInt/8,          1);
                if (UseIn)                      AddKernelArgDim(Name, "Wfi",    3, DimState,    FilterDataSizeBits*DimInInt/8,     1);
                                                AddKernelArgDim(Name, "Bf",     3, 2,           DimState,       BiasDataSize);
                                                AddKernelArgDim(Name, "Wo",     3, DimState,    FilterDataSizeBits*DimStateInt/8,          1);
                if (UseIn)                      AddKernelArgDim(Name, "Woi",    3, DimState,    FilterDataSizeBits*DimInInt/8,     1);
                                                AddKernelArgDim(Name, "Bo",     3, 2,           DimState,       BiasDataSize);
                                                AddKernelArgDim(Name, "Wi",     3, DimState,    FilterDataSizeBits*DimStateInt/8,          1);
                if (UseIn)                      AddKernelArgDim(Name, "Wii",    3, DimState,    FilterDataSizeBits*DimInInt/8,     1);
                                                AddKernelArgDim(Name, "Bi",     3, 2,           DimState,       BiasDataSize);
                                                AddKernelArgDim(Name, "Wg",     3, DimState,    FilterDataSizeBits*DimStateInt/8,          1);
                if (UseIn)                      AddKernelArgDim(Name, "Wgi",    3, DimState,    FilterDataSizeBits*DimInInt/8,     1);
                                                AddKernelArgDim(Name, "Bg",     3, 2,           DimState,       BiasDataSize);
                                                AddKernelArgDim(Name, "Infos",  3, 1,           (Mode16?LSTM_NE16_CELL_INFOS_SHORT:LSTM_NE16_CELL_INFOS), 1);
                if (ExposeSequence)		AddKernelArgDim(Name, "Hout",   3, NCells,      DimState,       FeatDataSize);
        }

        return Kernel;
}

int LSTM_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

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
        unsigned int Wa = 0;
        int Ws = FilterDataSizeBits;
        if (((FilterDataSizeBits%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_LIN;
        if (K0<1) GenTilingError("LSTM_Stack_NE16, %s, K0, At least one input is expected\n", Name, K0);
        if (K1<1) GenTilingError("LSTM_Stack_NE16, %s, K1, At least one output is expected\n", Name, K1);
        if (K0>NCells) GenTilingError("LSTM_Stack_NE16, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
        if (K1>NCells) GenTilingError("LSTM_Stack_NE16, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

        int UseHardAct = 0;

        if (Ctrl) {
                if (Ctrl->RNNUseHardActivation != -1) UseHardAct = Ctrl->RNNUseHardActivation;
        }

        int ParFeat = 1;

        unsigned S_Attr = 0 | ((!AlwaysReset)?O_IN:0) | ((!AlwaysReset)?O_OUT:0);
        printf("LSTM_Stack_NE16: KOP_LSTM, %s - Feature Size %d\n", (UseHardAct?"KOP_HSIGMOID":"KOP_SIGMOID"), FeatDataSize);
        char *LSTMKerName = CNN_FindMatchingKernel(
                KOP_LSTM, (UseHardAct?KOP_HSIGMOID:KOP_SIGMOID), CALL_PARALLEL,
                FeatDataSize, 0, 0, 0, FeatDataSize, 0,0,0,0,1,1, 0,0,0,0, 0, 0, 0);

        if (!LSTMKerName) GenTilingError("LSTM_Stack_NE16 Kernel: %s, Can't find a matching basic kernel", Name);

        FeatDataSize = Abs(FeatDataSize);

        char *G1_Name=0, *G2_Name=0, *G3_Name=0;
        int N1, N2, N3, N2_IO, Seq = RNN_Sequence_NE16(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);

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
                printf("LSTM, %d Cells%s, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d, Order: %s\n",
                        NCells, Dynamic?" Dynamic":"", DimState, DimIn, K0, K1, Revert?"Reverse":"Regular");
                printf("Basic Kernel: %s\n", LSTMKerName);
                printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
                printf("Use hard act: %d\n", UseHardAct);
        }

        if (Dynamic && !((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("RNN with dynamic cell count is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
        OpenKernelGroup(Name);

        if (Dynamic)
                UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));
        int Ok = 0;
        G1_Name = AppendNames(Name, "G1");
        G2_Name = AppendNames(Name, "G2");
        G3_Name = AppendNames(Name, "G3");

        int DoBuffer = 1, DoConstraint = 32;
        while (Ok==0) {
                Ok = TryKernelSolution(
                        LSTM_Stack_Seq_NE16, Ctrl, LSTMKerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, DimState, DimIn, DoBuffer, DoConstraint, Revert, Dynamic,
                        G1_Name, G2_Name, G3_Name, N1, N2, N3, N2_IO);
                if (DoBuffer) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence with all coeffs promoted to buffer\n");
                        } else {
                                if (Log) printf("Failed to map sequence with all coeffs promoted to buffer, reverting to tile based\n");
                                DoBuffer = 0;
                        }
                } else if (DoConstraint) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with %d output size constraint\n", DoConstraint);
                        } else {
                                if (Log) printf("Failed to map sequence tile based with %d output size constraint, relaxing constraint\n", DoConstraint);
                                DoConstraint = (DoConstraint>16)?DoConstraint-8:0;
                        }
                } else {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with no output size constraint\n");
                        } else {
                                if (Log) printf("No solution found\n");
                                break;
                        }
                }
        }
        CloseKernelGroupNoMerge();
        if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;
        int NumCArgs = 16 + 3*(AlwaysReset==0) + 2*(N1!=0) + 2*(N2!=0 && N3!=0) + (Dynamic!=0);
        GroupCArgs = AllocateCArgs(NumCArgs);
        int A= 0;

        if (Dynamic) 		GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0), 	   "NCells");
        if (!AlwaysReset) 	GroupCArgs[A++] = TCArg(CNN_ArgDataType(FeatDataSize,1,1), "Cinout");
        if (!AlwaysReset) 	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Hinout");
        if (N1!=0) 	   	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G1O0");
        if (N1!=0) 	   	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G1O1");
        if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G2O0");
        if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G2O1");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Xin");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "ScaleNorm");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wf");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wfi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bf");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wii");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wg");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wgi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bg");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wo");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Woi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bo");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Hout");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
        if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

        GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
        A=0;
        if (Dynamic) {
                if (N1>0) {
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(21,
                                                        BindKGExpr("CArg(NCells)-1"),
                                                        (!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O0"),
                                                        C_Arg("G1O1"),
                                                        BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Wii"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Wgi"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Woi"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(22,
                                                        (N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
                                                        (N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
                                                        (N1)?C_Arg("G1O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Wii"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Wgi"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Woi"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                        )
                                                );
        } else {
                if (N1>0) {
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(20,
                                                        (!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O0"),
                                                        C_Arg("G1O1"),
                                                        Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Wii"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Wgi"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Woi"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(21,
                                                        (N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
                                                        (N1)?C_Arg("G1O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O1"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
                                                        C_Arg("Wf"),
                                                        C_Arg("Wfi"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Wii"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Wgi"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Woi"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                        )
                                                );
                if (N2>0&&!N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(15,
                                                        C_Arg("G1O0"),
                                                        C_Arg("G1O1"),
                                                        C_Arg("G2O0"),
                                                        C_Arg("G2O1"),
                                                        C_Arg("Wf"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                        )
                                                );
                if (N3>0)
                        GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
                                                Bindings(16,
                                                        (N2)?C_Arg("G2O0"):C_Arg("G1O0"),
                                                        (N2)?C_Arg("G2O1"):C_Arg("G1O1"),
                                                        (!AlwaysReset)?C_Arg("Cinout"):AT_NO_ARG_BINDING,
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("Wf"),
                                                        C_Arg("Bf"),
                                                        C_Arg("Wi"),
                                                        C_Arg("Bi"),
                                                        C_Arg("Wg"),
                                                        C_Arg("Bg"),
                                                        C_Arg("Wo"),
                                                        C_Arg("Bo"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                        )
                                                );
        }
        GroupKerArgs = AllocateKerArgs(12+ (Dynamic!=0) + 2*(N1!=0) + 2*(N2!=0 && N3!=0)+ 3*(AlwaysReset==0) + 4);
        A = 0;
        if (Dynamic)		GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,   	     1,                                  4,            "NCells");
        if (AlwaysReset==0) 	GroupKerArgs[A++] = KerGroupArg("Cinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Cinout");
        if (AlwaysReset==0) 	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                           FeatDataSize, "Hinout");
        if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O0",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O0");
        if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O1",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G1O1");
        if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O0",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O0");
        if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O1",    O_IN|O_OUT|O_BUFF, DimState,                           FeatDataSize, "G2O1");
                                GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,   	     DimIn*K0,                           FeatDataSize, "Xin");
                                GroupKerArgs[A++] = KerGroupArg("Wf",     O_IN|Wa,   	     DimState*DimState, Ws,            "Wf");
        			GroupKerArgs[A++] = KerGroupArg("Wfi",     O_IN|Wa,   	     DimIn*DimState, Ws,            "Wfi");
                                GroupKerArgs[A++] = KerGroupArg("Bf",     O_IN,   	     DimState,                    BiasDataSize, "Bf");
                                GroupKerArgs[A++] = KerGroupArg("Wi",     O_IN|Wa,   	     DimState*DimState, Ws,            "Wi");
        			GroupKerArgs[A++] = KerGroupArg("Wii",     O_IN|Wa,   	     DimIn*DimState, Ws,            "Wii");
                                GroupKerArgs[A++] = KerGroupArg("Bi",     O_IN,   	     DimState,                    BiasDataSize, "Bi");
                                GroupKerArgs[A++] = KerGroupArg("Wg",     O_IN|Wa,   	     DimState*DimState, Ws,            "Wg");
        			GroupKerArgs[A++] = KerGroupArg("Wgi",     O_IN|Wa,   	     DimIn*DimState, Ws,            "Wgi");
                                GroupKerArgs[A++] = KerGroupArg("Bg",     O_IN,   	     DimState,                    BiasDataSize, "Bg");
                                GroupKerArgs[A++] = KerGroupArg("ScaleNorm",     O_IN,       8 * DimState * 2,                    1,             "ScaleNorm");
                                GroupKerArgs[A++] = KerGroupArg("Wo",     O_IN|Wa,   	     DimState*DimState, Ws,            "Wo");
        			GroupKerArgs[A++] = KerGroupArg("Woi",     O_IN|Wa,   	     DimIn*DimState, Ws,            "Woi");
                                GroupKerArgs[A++] = KerGroupArg("Bo",     O_IN,   	     DimState,                    BiasDataSize, "Bo");
                                GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,  	     DimState*K1,                        FeatDataSize, "Hout");
                                GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,   	     LSTM_NE16_CELL_INFOS,     1,            "Infos");
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

static Kernel_T *GRU_Stack_Seq_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,
        char *GRUKerName,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

        int HardAct,
        int AlwaysReset,
        int NCells,
        int DimState,
        int DimIn,
        int UseIn,
        int ExposeSequence,
        int Buffer,
        int OutSizeConstraint,
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
        printf("GRU Segment: %s NC: %d, First: %d, Last: %d, UseIn: %d, Hard Act: %d Always Reset: %d Dynamic: %d ExposeSeq: %d Feat: %d Buffer: %d\n",
                Name, NCells, FirstSeq, LastSeq, UseIn, HardAct, AlwaysReset, Dynamic, ExposeSequence, FeatDataSize, Buffer);
        if (Abs(FeatDataSize)!=1 && Abs(FeatDataSize)!=2) GenTilingError("Node: %s Input DataSize %d not supported in NE16", Name, FeatDataSize);

        int Mode16          = (Abs(FeatDataSize) == 2);
        int GatePrenorm = (Mode16?8:0);

        if (Ctrl) {
                // Not used currently
                if (Ctrl->GatePrenorm != -1 && Mode16) GatePrenorm = Ctrl->GatePrenorm;
        }

        // Set bit mode if != 8 databits
        int Ws = FilterDataSizeBits;
        unsigned int Wa = 0;
        if (Ws%8) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_RNN;

        // setup default job
        int StreamoutMode   = (Mode16?0:1);
        int Streamin        = (Mode16?1:0); // 16 bit always streams in
        int FilterMode      = 2;
        int LinearMode      = 1;
        int StridedMode     = 0;
        int NormBits        = 0; // Byte norms and scales
        int WOffsetCfg      = 1; // Weight 0 point dynamic
        int QuantRightShift = 0;
        int QuantBits       = 2; // 00: 8bit, 01: 16bit, 10: 32bit
        int QuantNoRect     = 1;
        int NormBias        = (Mode16?0:1); // Bias on by default - switch off if not needed
        int NormShift       = (Mode16?0:1);
        unsigned int DEFAULT_NE16_JOB_CFG = NE16_DefaultConfig_RNN(
                FilterDataSizeBits, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin,
                WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);
        Tile_Orientation_T TileOrientation = TILE_HOR;
        unsigned long long int LayerOp = 0;
        unsigned long long int LayerBandwidth = 0;
        unsigned int Si_Attr = O_IN|((!FirstSeq||AlwaysReset)?O_NO_LOAD:0);
        unsigned int So_Attr = O_OUT|((!LastSeq||AlwaysReset)?O_NO_STORE:0);
        int RD0 = Revert?SPACE_PROP(D0, SPACE_PROP_REVERT):D0;
        if (Dynamic && (Si_Attr==O_IN)) Si_Attr |= O_ALWAYS_LOAD;

        int Din = UseIn?DimIn:0;

	LayerOp = NCells*((DimState + (UseIn?DimIn:0))*DimState*3 + DimState*3 + DimState*3);
	LayerBandwidth = NCells*((DimState + DimIn) + 3*(DimState + DimIn)*DimState*1 + 3*(DimState)*BiasDataSize + DimState);

        int DimStateInt = ROUND_UP_N(DimState, (Mode16?32:16));
        int DimInInt = (UseIn?ROUND_UP_N(DimIn, (Mode16?32:16)):0);
        printf("DimInInt %d DimStateInt %d DimIn %d DimState %d bias_ds %d\n", DimInInt, DimStateInt, DimIn, DimState, BiasDataSize);

        Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(3, Dynamic?IterFixedSpaceDynBound(D0, NCells, "NCells"):IterFixedSpace(D0, NCells), IterParSpace(D1, DimState, OutSizeConstraint), IterTiledSpace(T0)),
                TileOrientation,
                CArgs(18,
                      (Dynamic)?		  TCArg(CNN_ArgDataType(4,0,0),		        "NCells"):AT_NO_C_ARG,
                      (!(FirstSeq&&AlwaysReset))? TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "SHin"):AT_NO_C_ARG,
                      (!(LastSeq&&AlwaysReset))?  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "SHout"):AT_NO_C_ARG,
                      (UseIn)?			  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Xin"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wr"),
                      (UseIn)?                    TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wri"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Br"),
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wz"),
                      (UseIn)?                    TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wzi"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bz"),
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "Wh"),
                      (UseIn)?                    TCArg(CNN_ArgDataTypeUns(1,1,1),              "Whi"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Bwh"),
                                                  TCArg(CNN_ArgDataType(BiasDataSize,1,1),      "Brh"),
                                                  TCArg(CNN_ArgDataTypeUns(1,1,1),              "ScaleNorm"),
                      (ExposeSequence)?		  TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1),   "Hout"):AT_NO_C_ARG,
                                                  TCArg(CNN_ArgDataType(1,1,1),                 "Infos"),
                      (!AlwaysReset)?		  TCArg(CNN_ArgDataType(1,0,0),                 "Reset"):AT_NO_C_ARG
                ),
                Calls(4,
                        FirstSeq?Call("NE16_Enable", LOC_D0_PROLOG, Bindings(0)):AT_NO_CALL,
                        Call("NE16_SoftReset", LOC_LOOP, Bindings(0)),
                        Call(GRUKerName, LOC_LOOP,
                                Bindings(32,
                                        (!(FirstSeq&&AlwaysReset))?K_Arg("SHin",  KER_ARG_TILE):((!(LastSeq&&AlwaysReset))?K_Arg("SHout",  KER_ARG_TILE):Imm(0)),
                                        K_Arg("State",  KER_ARG_TILE),
                                        UseIn?K_Arg("Xin", KER_ARG_TILE):Imm(0),
                                        K_Arg("ScaleNorm",  KER_ARG_TILE),
                                        K_Arg("OutBuff1",  KER_ARG_TILE),
                                        K_Arg("OutBuff2",  KER_ARG_TILE),
                                        (!Mode16)?K_Arg("OutBuff3",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING,
                                        Imm(DimState),
                                        Imm(DimIn),
                                        Imm(DimStateInt),
                                        Imm(DimInInt),
                                        K_Arg("Wr",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wri",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Br",  KER_ARG_TILE),
                                        K_Arg("Wz",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Wzi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bz",  KER_ARG_TILE),
                                        K_Arg("Wh",  KER_ARG_TILE),
                                        (UseIn?K_Arg("Whi",  KER_ARG_TILE):AT_IGNORE_ARG_BINDING),
                                        K_Arg("Bwh",  KER_ARG_TILE),
                                        K_Arg("Brh",  KER_ARG_TILE),
                                        (ExposeSequence)?K_Arg("Hout", KER_ARG_TILE):Imm(0),
                                        K_ArgPar("Br", KER_ARG_PARTILE_SIZE, D1),
                                        K_Arg("Infos", KER_ARG_TILE),
                                        K_ArgPred("Wr", KER_ARG_TILEFIRST, D0),
                                        K_ArgPred("Wr", KER_ARG_TILEFIRST, D1),
                                        K_ArgPred("Wr", KER_ARG_TILELAST, D0),
                                        K_ArgPred("Wr", KER_ARG_TILELAST, D1),
                                        Imm(FilterDataSizeBits),
                                        Imm(DEFAULT_NE16_JOB_CFG), 
                                        AlwaysReset?(FirstSeq?Imm(1):Imm(0)):C_Arg("Reset"),
                                        BindKExpr("KArg(Br, TileBase, D1)")
                                        )
                        ),
                        LastSeq?Call("NE16_Disable", LOC_D0_EPILOG, Bindings(0)):AT_NO_CALL

                ),
                KerArgs(20,
                        KerArg("Wr",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wr"),
                        (UseIn)?
                        KerArg("Wri",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Wri"):AT_NO_KER_ARG,
                        KerArg("Br",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Br"),
                        KerArg("Wz",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wz"),
                        (UseIn)?
                        KerArg("Wzi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Wzi"):AT_NO_KER_ARG,
                        KerArg("Bz",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  2,                1, BiasDataSize, 0, 0, 0, "Bz"),
                        KerArg("Wh",      KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimStateInt,      1, Ws,           0, 0, 0, "Wh"),
                        (UseIn)?
                        KerArg("Whi",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer|Wa,            DimInInt,         1, Ws,           0, 0, 0, "Whi"):AT_NO_KER_ARG,
                        KerArg("Bwh",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer,               1,                1, BiasDataSize, 0, 0, 0, "Bwh"),
                        KerArg("Brh",     KerArgSpace(1,D1),    O_IN|O_DB|O_CONST|Buffer,               1,                1, BiasDataSize, 0, 0, 0, "Brh"),
                        KerArg("ScaleNorm", KerArgSpace(1,D1),  O_IN|O_DB|O_CONST|Buffer|O_NE16_INTER,  6 * (UseIn?2:1),  1, 1,            0, 0, 0, "ScaleNorm"),

                        KerArg("State",   KerArgSpace(1,T0),    O_BUFF|O_NTILED,      	                DimStateInt+DimState+DimInInt,
                                                                                                                          1, FeatDataSize, 0, 0, 0, ""),
                        (!(FirstSeq&&AlwaysReset))?
                        KerArg("SHin",     KerArgSpace(1,T0),   Si_Attr|O_BUFF|O_NTILED,                DimState,         1, FeatDataSize, 0, 0, 0, "SHin"):AT_NO_KER_ARG,

                        (!(LastSeq&&AlwaysReset))?
                        ((!(FirstSeq&&AlwaysReset))?
                        KerArgAliased("SHout", KerArgSpace(1,T0), 1, So_Attr|O_BUFF|O_NTILED,           DimState,         1, FeatDataSize, 0, 0, 0, "SHout"):
                        KerArg("SHout",    KerArgSpace(1,T0),   So_Attr|O_BUFF|O_NTILED,                DimState,         1, FeatDataSize, 0, 0, 0, "SHout")):AT_NO_KER_ARG,

                        (UseIn)?
                        KerArg("Xin",     KerArgSpace(1,RD0),   O_IN|O_DB,                              DimIn,            1, FeatDataSize, 0, 0, 0, "Xin"):AT_NO_KER_ARG,
                        KerArg("OutBuff1", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       (Mode16?2:1),     1, 4,            0, 0, 0, ""),
                        KerArg("OutBuff2", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       (Mode16?2:1),     1, 4,            0, 0, 0, ""),
                        (!Mode16)?
                        KerArg("OutBuff3", KerArgSpace(1,D1),   O_BUFF|O_ONETILE,                       1,                1, 4,            0, 0, 0, ""):AT_NO_KER_ARG,
                        (ExposeSequence)?
                        KerArg("Hout",    KerArgSpace(2,RD0,D1),O_OUT|O_DB,                             1,                1, FeatDataSize, 0, 0, 0, "Hout"):AT_NO_KER_ARG,
                        KerArg("Infos",   KerArgSpace(1,T0),    O_IN|O_BUFF|O_NTILED|O_CONST,           GRU_NE16_CELL_INFOS,1, 1,        0, 0, 0, "Infos")
                )
        );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		if (!(FirstSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHin",     2, DimState,      FeatDataSize);
		if (!(LastSeq&&AlwaysReset))	AddKernelArgDim(Name, "SHout",    2, DimState,      FeatDataSize);
                                                AddKernelArgDim(Name, "ScaleNorm",3, 6*(UseIn?2:1), DimState, 1);
                if (UseIn)			AddKernelArgDim(Name, "Xin",      3, NCells,        DimIn,                    FeatDataSize);
                                                AddKernelArgDim(Name, "Wr",       3, DimState,      FilterDataSizeBits*DimStateInt/8,      1);
                if (UseIn)                      AddKernelArgDim(Name, "Wri",      3, DimState,      FilterDataSizeBits*DimInInt/8,         1);
                                                AddKernelArgDim(Name, "Br",       3, 2,             DimState,                 BiasDataSize);
                                                AddKernelArgDim(Name, "Wz",       3, DimState,      FilterDataSizeBits*DimStateInt/8,      1);
                if (UseIn)                      AddKernelArgDim(Name, "Wzi",      3, DimState,      FilterDataSizeBits*DimInInt/8, 1);
                                                AddKernelArgDim(Name, "Bz",       3, 2,             DimState,                 BiasDataSize);
                                                AddKernelArgDim(Name, "Wh",       3, DimState,      FilterDataSizeBits*DimStateInt/8,      1);
                if (UseIn)                      AddKernelArgDim(Name, "Whi",      3, DimState,      FilterDataSizeBits*DimInInt/8,         1);
                                                AddKernelArgDim(Name, "Bwh",      3, 1,             DimState,                 BiasDataSize);
                                                AddKernelArgDim(Name, "Brh",      3, 1,             DimState,                 BiasDataSize);
                                                AddKernelArgDim(Name, "Infos",    3, 1,             GRU_NE16_CELL_INFOS,      1);
                if (ExposeSequence)		AddKernelArgDim(Name, "Hout",     3, NCells,        DimState,                 FeatDataSize);
        }

        return Kernel;
}

int GRU_Stack_NE16(
        char *Name,
        CNN_GenControl_T *Ctrl,

        int BiasDataSize,
        int FeatDataSize,
        int FilterDataSizeBits,

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
        unsigned int Wa = 0;
        int Ws = FilterDataSizeBits;
        if (((FilterDataSizeBits%8)!=0)) Wa = O_BIT; else Ws = Ws/8;
        Wa |= O_NE16_LIN;
        if (K0<1) GenTilingError("GRU_Stack_NE16, %s, K0, At least one input is expected\n", Name, K0);
        if (K1<1) GenTilingError("GRU_Stack_NE16, %s, K1, At least one output is expected\n", Name, K1);
        if (K0>NCells) GenTilingError("GRU_Stack_NE16, %s, K0, Number of input should be in [1,NCells]\n", Name, K0);
        if (K1>NCells) GenTilingError("GRU_Stack_NE16, %s, K1, Number of ouput should be in [1,NCells]\n", Name, K1);

        int UseHardAct = 0;

        if (Ctrl) {
                if (Ctrl->RNNUseHardActivation != -1) UseHardAct = Ctrl->RNNUseHardActivation;
        }

        int ParFeat = 1;

        unsigned S_Attr = 0 | ((!AlwaysReset)?O_IN:0) | ((!AlwaysReset)?O_OUT:0);
        printf("GRU_Stack_NE16: KOP_GRU, %s - Feature Size %d\n", (UseHardAct?"KOP_HSIGMOID":"KOP_SIGMOID"), FeatDataSize);
        char *GRUKerName = CNN_FindMatchingKernel(
                KOP_GRU, (UseHardAct?KOP_HSIGMOID:KOP_SIGMOID), CALL_PARALLEL,
                FeatDataSize, 0, 0, 0, FeatDataSize, 0,0,0,0,1,1, 0,0,0,0, 0, 0, 0);

        if (!GRUKerName) GenTilingError("GRU_Stack_NE16 Kernel: %s, Can't find a matching basic kernel", Name);

        FeatDataSize = Abs(FeatDataSize);

        char *G1_Name=0, *G2_Name=0, *G3_Name=0;
        int N1, N2, N3, N2_IO, Seq = RNN_Sequence_NE16(NCells, K0, K1, &N1, &N2, &N3, &N2_IO);

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
                printf("GRU, %d Cells%s, DimState: %d, DimIn: %d, Input Cells: %d, Output Cells: %d, Order: %s\n",
                        NCells, Dynamic?" Dynamic":"", DimState, DimIn, K0, K1, Revert?"Reverse":"Regular");
                printf("Basic Kernel: %s\n", GRUKerName);
                printf("In Seq: %d, %s Seq: %d, Out Seq: %d\n", N1, N2_IO?"In/Out":"void", N2, N3);
                printf("Use hard act: %d\n", UseHardAct);
        }

        if (Dynamic && !((NCells==K0 && NCells==K1) || (NCells==K0 && K1==1))) GenTilingError("RNN with dynamic cell count is valid only for NC=K0=K1 (all in and out) or NC=K0,K1=1 (all in, single out)");
        OpenKernelGroup(Name);

        if (Dynamic)
                UserSymbols(3, US_Int("Revert", Revert), US_Int("DimIn", DimIn), US_Type("pFeatType", CNN_ArgDataType(FeatDataSize,1,1)));
        int Ok = 0;
        G1_Name = AppendNames(Name, "G1");
        G2_Name = AppendNames(Name, "G2");
        G3_Name = AppendNames(Name, "G3");

        int DoBuffer = 1, DoConstraint = 32;
        while (Ok==0) {
                Ok = TryKernelSolution(
                        GRU_Stack_Seq_NE16, Ctrl, GRUKerName, BiasDataSize, FeatDataSize, FilterDataSizeBits,
                        UseHardAct, AlwaysReset, DimState, DimIn, DoBuffer, DoConstraint, Revert, Dynamic,
                        G1_Name, G2_Name, G3_Name, N1, N2, N3, N2_IO);
                if (DoBuffer) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence with all coeffs promoted to buffer\n");
                        } else {
                                if (Log) printf("Failed to map sequence with all coeffs promoted to buffer, reverting to tile based\n");
                                DoBuffer = 0;
                        }
                } else if (DoConstraint) {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with %d output size constraint\n", DoConstraint);
                        } else {
                                if (Log) printf("Failed to map sequence tile based with %d output size constraint, relaxing constraint\n", DoConstraint);
                                DoConstraint = (DoConstraint>16)?DoConstraint-8:0;
                        }
                } else {
                        if (Ok) {
                                if (Log) printf("Mapped sequence tile based with no output size constraint\n");
                        } else {
                                if (Log) printf("No solution found\n");
                                break;
                        }
                }
        }
        CloseKernelGroupNoMerge();
        if (Ok==0) return 0;

        CKernel_Arg_T **GroupCArgs;
        CKernelCall_T **GroupCCalls;
        Object_T **GroupKerArgs;
        int NumCArgs = 14 + 2*(AlwaysReset==0) + 1*(N1!=0) + 1*(N2!=0 && N3!=0) + (Dynamic!=0);
        GroupCArgs = AllocateCArgs(NumCArgs);
        int A= 0;

        if (Dynamic) 		GroupCArgs[A++] = TCArg(CNN_ArgDataType(4,0,0), 	   "NCells");
        if (!AlwaysReset) 	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Hinout");
        if (N1!=0) 	   	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G1O0");
        if (N2!=0 && N3!=0)	GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "G2O0");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Xin");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1), "ScaleNorm");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wr");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wri");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Br");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wz");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wzi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bz");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Wh");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(1,1,1),            "Whi");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Bwh");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(BiasDataSize,1,1), "Brh");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataTypeUns(FeatDataSize,1,1), "Hout");
                                GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,1,1),            "Infos");
        if (!AlwaysReset)	GroupCArgs[A++] = TCArg(CNN_ArgDataType(1,0,0),            "Reset");

        GroupCCalls = AllocateCalls((N1!=0)+(N2!=0)+(N3!=0));
        A=0;
        if (Dynamic) {
                if (N1>0) {
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(17,
                                                        BindKGExpr("CArg(NCells)-1"),
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O0"),
                                                        BindKGExpr("((pFeatType)CArg(Xin))+(Revert?(DimIn*(CArg(NCells)-1)):0)"),
                                                        C_Arg("Wr"),
                                                        C_Arg("Wri"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Wzi"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Whi"),
                                                        C_Arg("Bwh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(18,
                                                        (N1==0)?BindKGExpr("CArg(NCells)"):AT_NO_ARG_BINDING,
                                                        (N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        BindKGExpr("((pFeatType)CArg(Xin))+(Revert?0:(DimIn*(CArg(NCells)-1)))"),
                                                        C_Arg("Wr"),
                                                        C_Arg("Wri"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Wzi"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Whi"),
                                                        C_Arg("Bwh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                        )
                                                );
        } else {
                if (N1>0) {
                        GroupCCalls[A++] = UserKernelCall(G1_Name, LOC_GROUP,
                                                Bindings(16,
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("G1O0"),
                                                        Revert?KG_ArgOper("Xin",   '+', (N2_IO?N2:0)*DimIn):C_Arg("Xin"),
                                                        C_Arg("Wr"),
                                                        C_Arg("Wri"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Wzi"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Whi"),
                                                        C_Arg("Bwh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (AlwaysReset==0)?C_Arg("Reset"):AT_NO_ARG_BINDING));
                }
                if (N2>0&&N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(17,
                                                        (N1)?C_Arg("G1O0"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        (N3)?C_Arg("G2O0"):((!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING),
                                                        Revert?C_Arg("Xin"):KG_ArgOper("Xin",   '+', N1*DimIn),
                                                        C_Arg("Wr"),
                                                        C_Arg("Wri"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Wzi"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Whi"),
                                                        C_Arg("Bwh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?((N1==0)?C_Arg("Reset"):Imm(0)):AT_NO_ARG_BINDING
                                                        )
                                                );
                if (N2>0&&!N2_IO)
                        GroupCCalls[A++] = UserKernelCall(G2_Name, LOC_GROUP,
                                                Bindings(11,
                                                        C_Arg("G1O0"),
                                                        C_Arg("G2O0"),
                                                        C_Arg("Wr"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                        )
                                                );
                if (N3>0)
                        GroupCCalls[A++] = UserKernelCall(G3_Name, LOC_GROUP,
                                                Bindings(12,
                                                        (N2)?C_Arg("G2O0"):C_Arg("G1O0"),
                                                        (!AlwaysReset)?C_Arg("Hinout"):AT_NO_ARG_BINDING,
                                                        C_Arg("Wr"),
                                                        C_Arg("Br"),
                                                        C_Arg("Wz"),
                                                        C_Arg("Bz"),
                                                        C_Arg("Wh"),
                                                        C_Arg("Brh"),
                                                        C_Arg("ScaleNorm"),
                                                        C_Arg("Hout"),
                                                        C_Arg("Infos"),
                                                        (!AlwaysReset)?Imm(0):AT_NO_ARG_BINDING
                                                        )
                                                );
        }
        GroupKerArgs = AllocateKerArgs(14+ (Dynamic!=0) + 1*(N1!=0) + 1*(N2!=0 && N3!=0)+ 2*(AlwaysReset==0));
        A = 0;
        if (Dynamic)		GroupKerArgs[A++] = KerGroupArg("NCells", O_IN,   	     1,                       4,            "NCells");
        if (AlwaysReset==0) 	GroupKerArgs[A++] = KerGroupArg("Hinout", O_IN|O_OUT,        DimState,                FeatDataSize, "Hinout");
        if (N1!=0)		GroupKerArgs[A++] = KerGroupArg("G1O0",    O_IN|O_OUT|O_BUFF, DimState,               FeatDataSize, "G1O0");
        if (N2!=0 && N3!=0)	GroupKerArgs[A++] = KerGroupArg("G2O0",    O_IN|O_OUT|O_BUFF, DimState,               FeatDataSize, "G2O0");
                                GroupKerArgs[A++] = KerGroupArg("Xin",    O_IN,   	     DimIn*K0,                FeatDataSize, "Xin");
                                GroupKerArgs[A++] = KerGroupArg("Wr",     O_IN|Wa,   	     DimState*DimState,       Ws,           "Wr");
        			GroupKerArgs[A++] = KerGroupArg("Wri",     O_IN|Wa,   	     DimIn*DimState,          Ws,           "Wri");
                                GroupKerArgs[A++] = KerGroupArg("Br",     O_IN,   	     2*DimState,              BiasDataSize, "Br");
                                GroupKerArgs[A++] = KerGroupArg("Wz",     O_IN|Wa,   	     DimState*DimState,       Ws,           "Wz");
        			GroupKerArgs[A++] = KerGroupArg("Wzi",     O_IN|Wa,   	     DimIn*DimState,          Ws,           "Wzi");
                                GroupKerArgs[A++] = KerGroupArg("Bz",     O_IN,   	     2*DimState,              BiasDataSize, "Bz");
                                GroupKerArgs[A++] = KerGroupArg("Wh",     O_IN|Wa,   	     DimState*DimState,       Ws,           "Wh");
        			GroupKerArgs[A++] = KerGroupArg("Whi",     O_IN|Wa,   	     DimIn*DimState,          Ws,           "Whi");
                                GroupKerArgs[A++] = KerGroupArg("Bwh",     O_IN,   	     DimState,                BiasDataSize, "Bwh");
                                GroupKerArgs[A++] = KerGroupArg("Brh",     O_IN,   	     DimState,                BiasDataSize, "Brh");
                                GroupKerArgs[A++] = KerGroupArg("ScaleNorm",     O_IN,       8 * DimState * 2,        1,            "ScaleNorm");
                                GroupKerArgs[A++] = KerGroupArg("Hout",   O_OUT,  	     DimState*K1,             FeatDataSize, "Hout");
                                GroupKerArgs[A++] = KerGroupArg("Infos",  O_IN,   	     GRU_NE16_CELL_INFOS,     1,            "Infos");
        if (!AlwaysReset)	GroupKerArgs[A++] = KerGroupArg("Reset",  O_IN,   	     1,                       1,            "Reset");

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
