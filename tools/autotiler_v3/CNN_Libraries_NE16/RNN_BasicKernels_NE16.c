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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <stdio.h>
#include <math.h>
#include "RNN_BasicKernels_NE16.h"
#include "NE16_Helpers.h"
#include "NE16_Manual_Subtile.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();
// #define DEBUG_NE16 0


void dump_u8(const char *name, int len, const unsigned char *out)
{
        pi_cl_team_critical_enter();
        printf("%s[%d] = { ", name, len);
        for (int i = 0; i < len; i++)
                printf("%x%s", out[i], (i == len - 1 ? "}\n" : ", "));
        pi_cl_team_critical_exit();
}

void dump_u16(const char *name, int len, const unsigned short *out)
{
        pi_cl_team_critical_enter();
        printf("%s[%d] = { ", name, len);
        for (int i = 0; i < len; i++)
                printf("%u%s", out[i], (i == len - 1 ? "}\n" : ", "));
        pi_cl_team_critical_exit();
}

void dump_16(const char *name, int len, const short *out)
{
        pi_cl_team_critical_enter();
        printf("%s[%d] = { ", name, len);
        for (int i = 0; i < len; i++)
                printf("%d%s", out[i], (i == len - 1 ? "}\n" : ", "));
        pi_cl_team_critical_exit();
}

void dump_i32(const char *name, int len, const int *out)
{
        pi_cl_team_critical_enter();
        printf("%s[%d] = { ", name, len);
        for (int i = 0; i < len; i++)
                printf("%d%s", out[i], (i == len - 1 ? "}\n" : ", "));
        pi_cl_team_critical_exit();
}

static inline unsigned int __attribute__((always_inline)) ChunkSizeCC(unsigned int X, int IncludeCC)

{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        if (CoreCountDynamic)
                NCore = ActiveCore;
        else
                NCore = gap_ncore();
        if (IncludeCC)
        {
                NCore += 1;
                Chunk = X / NCore;
        }
        else
        {
                Log2Core = gap_fl1(NCore);
                Chunk = (X >> Log2Core) + ((X & (NCore - 1)) != 0);
        }
        return Chunk;
}


static inline void ParSetup(int Nout, int IncCC, int * First, int * Last)
{
        int CoreId = gap_coreid();
        int ChunkCell = ChunkSizeCC(Nout, 1);
        *First = CoreId * ChunkCell;
        *Last = Nout;
}

#define B_CLR(x, bits) ((x) & (~((1 << (bits)) - 1)))

static inline void Copy(void *__restrict__ To, void *__restrict__ From, unsigned int Size, unsigned int CoreId)

{
        unsigned int Chunk = ChunkSizeCC(Size, 0), First = Chunk * CoreId, Last = Min(First + Chunk, Size);
        unsigned int Iter = Max(0, Last - First);

        int *pFrom = (int *)(From + First), *pTo = (int *)(To + First);
        for (int i = 0; i < Iter / 8; i++)
        {
                int V0 = pFrom[2 * i], V1 = pFrom[2 * i + 1];
                pTo[2 * i] = V0;
                pTo[2 * i + 1] = V1;
        }
        if (Iter & 0x4)
                *((int *)(To + First + B_CLR(Iter, 3))) = *((int *)(From + First + B_CLR(Iter, 3)));
        if (Iter & 0x2)
                *((short int *)(To + First + B_CLR(Iter, 2))) = *((short int *)(From + First + B_CLR(Iter, 2)));
        if (Iter & 0x1)
                *((signed char *)(To + First + Iter - 1)) = *((signed char *)(From + First + Iter - 1));
}

static inline void ZeroBody(void *__restrict__ To, unsigned int Cnt)
{
        int *pTo = (int *)To;
        for (int i = 0; i < Cnt / 8; i++)
        {
                pTo[2 * i] = 0;
                pTo[2 * i + 1] = 0;
        }
        if (Cnt & 0x4)
                *((int *)(To + B_CLR(Cnt, 3))) = 0;
        if (Cnt & 0x2)
                *((short int *)(To + B_CLR(Cnt, 2))) = 0;
        if (Cnt & 0x1)
                *((signed char *)(To + Cnt - 1)) = 0;
}

static inline void Zero(void *__restrict__ To, unsigned int Size, unsigned int CoreId)
{
        int Trace = 0;
        unsigned int Chunk = ChunkSizeCC(Size, 0), First = Chunk * CoreId, Last = Min(First + Chunk, Size);
        if (Trace)
                printf("zero %d %d %d\n", CoreId, First, Last);

        int Iter = Max(0, Last - First);
        ZeroBody(To + First, Iter);
}

static inline void ZeroState8Body(void *__restrict__ To, unsigned int Cnt)
{
        int *pTo = (int *)To;
        for (int i = 0; i < Cnt / 8; i++)
        {
                pTo[2 * i] = 0x80808080;
                pTo[2 * i + 1] = 0x80808080;
        }
        if (Cnt & 0x4)
                *((int *)(To + B_CLR(Cnt, 3))) = 0x80808080;
        if (Cnt & 0x2)
                *((short int *)(To + B_CLR(Cnt, 2))) = 0x8080;
        if (Cnt & 0x1)
                *((signed char *)(To + Cnt - 1)) = 0x80;
}

static inline void ZeroState8(unsigned char *__restrict__ To, unsigned int Size, unsigned int CoreId)

{
        int Trace = 0;
        unsigned int Chunk = ChunkSizeCC(Size, 0), First = Chunk * CoreId, Last = Min(First + Chunk, Size);
        if (Trace)
                printf("zero %d %d %d\n", CoreId, First, Last);

        int Iter = Max(0, Last - First);
        ZeroState8Body(To + First, Iter);
}

static inline void ZeroState16Body(void *__restrict__ To, unsigned int Cnt, unsigned int ZeroPoint)
{
        unsigned int ZeroPointInt = ZeroPoint << 16 | ZeroPoint;
        unsigned int *pTo = (int *)To;
        for (int i = 0; i < Cnt / 4; i++)
        {
                pTo[2 * i] = ZeroPointInt;
                pTo[2 * i + 1] = ZeroPointInt;
        }
        if (Cnt & 0x2)
                *((unsigned int *)(To + B_CLR(Cnt, 2))) = ZeroPointInt;
        if (Cnt & 0x1)
                *((unsigned short int *)(To + Cnt - 1)) = ZeroPoint;
}

static inline void ZeroState16(short int *__restrict__ To, unsigned int Size, unsigned int CoreId, unsigned int ZeroPoint)

{
        int Trace = 0;
        unsigned int Chunk = ChunkSizeCC(Size, 0), First = Chunk * CoreId, Last = Min(First + Chunk, Size);

        int Iter = Max(0, Last - First);

        if (Trace)
                printf("zero %d %d %d %x\n", CoreId, First, Last, ZeroPoint);
        ZeroState16Body(To + First, Iter, ZeroPoint);

}


void RNN_ParKerB32_Hard_NE16(KerRNN_NE16_T *Arg)

{
        /*	Sequences
                In:	DimIn!=0, Hout==0
                InOut:	DimIn!=0, Hout!=0
                None:	DimIn==0, Hout==0
                Out:	DimIn==0, Hout!=0

                Infos: Forget: HSigmoid:       Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN          7

                if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
        */
        int Trace = 1;
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);
        unsigned char *__restrict__ StateInOut = Arg->StateInOut;
        unsigned char *__restrict__ Xin = Arg->Xin;
        unsigned char *__restrict__ State = Arg->State;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int *volatile __restrict__ OutBuff = (int *)Arg->OutBuff;
        int TileOff = Arg->TileOffset;

        unsigned char OutZeroPoint = *((unsigned char *)&Infos[RNN_NE16_OUT_ZERO_POINT]);

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 1);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);

        if (CoreId != 8)
        {
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                if (Trace)
                                        printf("%d zero state\n", CoreId);
                                ZeroState8(State, DimState, CoreId);
                        }
                        else
                        {
                                if (Trace)
                                        printf("%d copy state\n", CoreId);
                                Copy(State, StateInOut, DimState, CoreId);
                        }
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        Copy(State + DimStateInt, Xin, DimIn, CoreId);
                        gap_waitbarrier_cc();
                }
        }
        else
        {
                if (Trace)
                        printf("NE16: FirstCell %d FirstOut %d Reset %d DimState %d DimIn %d DimStateInt %d DimInInt %d Nout %d\n",
                               Arg->FirstCell, Arg->FirstOut, Arg->Reset, DimState, DimIn, DimStateInt, DimInInt, Nout);
                Last = Nout;
                // Execute NE16 job
                unsigned char *__restrict__ Scale = Arg->ScaleNorm;
                unsigned char *__restrict__ ScaleN = &Arg->ScaleNorm[2 * Nout];
                unsigned char *__restrict__ Wf = Arg->Wf;
                unsigned char *__restrict__ Wfi = Arg->Wfi;
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                int *__restrict__ Bf = Arg->Bf;
                int *__restrict__ Bfi = &Arg->Bf[Nout];

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;

                int QuantBitsFlag = (cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;

                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {
                                if (Trace)
                                        printf("%d zero state pad %d %d\n", CoreId, DimState, DimStateInt);
                                ZeroBody(&State[DimState], DimStateInt-DimState);
                        }
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        if (Trace)
                                printf("%d zero Xin pad %d %d\n", CoreId, DimIn, DimInInt);
                        ZeroBody(&State[DimStateInt + DimIn], DimInInt-DimIn);
                        gap_waitbarrier_cc();
                }

                // bias always on when in 8 bit mode set by generator

                NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
                if (Xin)
                {
                        if (Trace)
                        {
                                dump_u8("inp_scale", Nout, &Scale[Nout]);
                                dump_u8("inp_scale_n", Nout, &ScaleN[Nout]);
                        }
                        // switch off streamin
                        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
                        TriggerNE16Job(cfg, &State[DimStateInt], OutBuff, Wfi, Bfi, 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, &Scale[Nout], &ScaleN[Nout], Infos[RNN_NE16_W_ZERO_POINT]);
                }

                if (Trace)
                {
                        dump_u8("state_scale", Nout, Scale);
                        dump_u8("state_scale_n", Nout, ScaleN);
                }
                // switch on streamin
                cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
                TriggerNE16Job(cfg, State, OutBuff, Wf, Bf, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, ScaleN, Infos[RNN_NE16_W_ZERO_POINT]);

                // wait for end of computation
                NE16_BARRIER();

                // set priority to core side
                NE16_SETPRIORITY_CORE();
        }
        gap_waitbarrier_cc();
        for (int o = First; o < Last; o++)
        {
                // Already in output scale - just max min tahn
                unsigned char Of = ((unsigned char)Max(Infos[RNN_NE16_F_A0], Min(Infos[RNN_NE16_F_B0], OutBuff[o]))) + OutZeroPoint;

                if (StateInOut)
                        StateInOut[TileOff + o] = Of;
                if (Hout)
                        Hout[o] = Of;
        }
        if (Trace)
        {
                gap_waitbarrier_cc();
                if (CoreId == 8)
                {
                        if (StateInOut)
                                dump_u8("state_out", Nout, &StateInOut[TileOff]);
                        if (Hout)
                                dump_u8("hout_out", Nout, Hout);
                }
        }
        gap_waitbarrier_cc();
}

void RNN_ParKerB32_NE16(KerRNN_NE16_T *Arg)

{
        /*	Sequences
                In:	DimIn!=0, Hout==0
                InOut:	DimIn!=0, Hout!=0
                None:	DimIn==0, Hout==0
                Out:	DimIn==0, Hout!=0

                Infos: Forget: HSigmoid:       Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN          7

                if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
        */
        int Trace = 1;
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);
        unsigned char *__restrict__ StateInOut = Arg->StateInOut;
        unsigned char *__restrict__ Xin = Arg->Xin;
        unsigned char *__restrict__ State = Arg->State;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int *volatile __restrict__ OutBuff = (int *)Arg->OutBuff;
        int TileOff = Arg->TileOffset;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 1);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);

        unsigned char OutZeroPoint = *((unsigned char *)&Infos[RNN_NE16_OUT_ZERO_POINT]);

        if (CoreId != 8)
        {
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                if (Trace)
                                        printf("%d zero state\n", CoreId);
                                ZeroState8(State, DimState, CoreId);
                        }
                        else
                        {
                                if (Trace)
                                        printf("%d copy state\n", CoreId);
                                Copy(State, StateInOut, DimState, CoreId);
                        }
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        Copy(State + DimStateInt, Xin, DimIn, CoreId);
                        gap_waitbarrier_cc();
                }
        }
        else
        {
                if (Trace)
                        printf("NE16: FirstCell %d FirstOut %d Reset %d DimState %d DimIn %d DimStateInt %d DimInInt %d Nout %d\n",
                               Arg->FirstCell, Arg->FirstOut, Arg->Reset, DimState, DimIn, DimStateInt, DimInInt, Nout);
                Last = Nout;
                // Execute NE16 job
                unsigned char *__restrict__ Scale = Arg->ScaleNorm;
                unsigned char *__restrict__ ScaleN = &Arg->ScaleNorm[2 * Nout];
                unsigned char *__restrict__ Wf = Arg->Wf;
                unsigned char *__restrict__ Wfi = Arg->Wfi;
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                int *__restrict__ Bf = Arg->Bf;
                int *__restrict__ Bfi = &Arg->Bf[Nout];
                printf("Input weights/bias=%x/%x State weights/bias=%x/%x\n", Wfi, Bfi, Wf, Bf);

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;

                int QuantBitsFlag = (cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;

                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {
                                if (Trace)
                                        printf("%d zero state pad %d %d\n", CoreId, DimState, DimStateInt);
                                ZeroBody(&State[DimState], DimStateInt-DimState);
                        }
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        if (Trace)
                                printf("%d zero Xin pad %d %d\n", CoreId, DimIn, DimInInt);
                        ZeroBody(&State[DimStateInt + DimIn], DimInInt-DimIn);
                        gap_waitbarrier_cc();
                }

                // bias always on when in 8 bit mode set by generator

                NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
                if (Xin)
                {
                        if (Trace)
                        {
                                dump_u8("inp_scale", Nout, &Scale[Nout]);
                                dump_u8("inp_scale_n", Nout, &ScaleN[Nout]);
                        }
                        // switch off streamin
                        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
                        TriggerNE16Job(cfg, &State[DimStateInt], OutBuff, Wfi, Bfi, 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, &Scale[Nout], &ScaleN[Nout], Infos[RNN_NE16_W_ZERO_POINT]);
                }

                if (Trace)
                {
                        dump_u8("state_scale", Nout, Scale);
                        dump_u8("state_scale_n", Nout, ScaleN);
                }
                // switch on streamin
                cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
                TriggerNE16Job(cfg, State, OutBuff, Wf, Bf, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, ScaleN, Infos[RNN_NE16_W_ZERO_POINT]);

                // wait for end of computation
                NE16_BARRIER();

                // set priority to core side
                NE16_SETPRIORITY_CORE();
        }
        gap_waitbarrier_cc();
        for (int o = First; o < Last; o++)
        {
                /* Scale to Output scale*/
                unsigned char Of = ((unsigned char)gap_clip(AT_SCALE(Tanh(OutBuff[o]), ((unsigned char *)Infos)[RNN_NE16_OUT_SCALE], ((unsigned char *)Infos)[RNN_NE16_OUT_SCALEN]), 7)) + OutZeroPoint;

                if (StateInOut)
                        StateInOut[TileOff + o] = Of;
                if (Hout)
                        Hout[o] = Of;
        }
        if (Trace)
        {
                gap_waitbarrier_cc();
                if (CoreId == 8)
                {
                        if (StateInOut)
                                dump_u8("state_out", Nout, &StateInOut[TileOff]);
                        if (Hout)
                                dump_u8("hout_out", Nout, Hout);
                }
        }
        gap_waitbarrier_cc();
}

void RNN_ParKerB32_NE16fp(KerRNN_NE16fp_T *Arg)
{
        /*	Sequences
                In:	DimIn!=0, Hout==0
                InOut:	DimIn!=0, Hout!=0
                None:	DimIn==0, Hout==0
                Out:	DimIn==0, Hout!=0

        */
        int Trace = 1;
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);
        unsigned short *__restrict__ StateInOut = Arg->StateInOut;
        unsigned short *__restrict__ Xin = Arg->Xin;
        unsigned short *__restrict__ State = Arg->State;

        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;

        unsigned short *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int *volatile __restrict__ OutBuff = (int *)Arg->OutBuff;
        int TileOff = Arg->TileOffset;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell;
        unsigned int First;
        unsigned int Last;

        unsigned char *__restrict__ Scale = Arg->ScaleNorm;
        unsigned char *__restrict__ ScaleN = &Arg->ScaleNorm[2 * Nout];
        int *__restrict__ Bf = Arg->Bf;
        int *__restrict__ Bfi = &Arg->Bf[Nout];
        unsigned short OutZeroPoint = *((unsigned short *)&Infos[RNN_NE16_OUT_ZERO_POINT]);

        if (CoreId != 8)
        {
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                ZeroState16(State, DimState, CoreId, OutZeroPoint);
                        }
                        else
                        {
                                Copy(State, StateInOut, DimState * 2, CoreId);
                        }
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        Copy(State + DimStateInt, Xin, DimIn * 2, CoreId);
                        Copy(&OutBuff[Nout], Bfi, Nout * 4, CoreId);
                        gap_waitbarrier_cc();
                }

                Copy(OutBuff, Bf, Nout * 4, CoreId);
                gap_waitbarrier_cc();
                ChunkCell = ChunkSizeCC(Nout, 1);
                First = CoreId * ChunkCell;
                Last = Min(First + ChunkCell, Nout);
        }
        else
        {
                if (Trace)
                        printf("NE16: FirstCell %d FirstOut %d Reset %d DimState %d DimIn %d DimStateInt %d DimInInt %d Nout %d\n",
                               Arg->FirstCell, Arg->FirstOut, Arg->Reset, DimState, DimIn, DimStateInt, DimInInt, Nout);
                Last = Nout;
                // Execute NE16 job
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                unsigned char *__restrict__ Wf = Arg->Wf;
                unsigned char *__restrict__ Wfi = Arg->Wfi;

                // Setup output size
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32;

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;

                // Zero state padding
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {
                                if (Trace)
                                        printf("%d zero state pad %d %d\n", CoreId, DimState, DimStateInt);
                                ZeroBody(&State[DimState], (DimStateInt-DimState)*2);
                        }
                        gap_waitbarrier_cc();
                }
                // Zero input padding
                if (Xin)
                {
                        if (Trace)
                                printf("%d zero Xin pad %d %d\n", CoreId, DimIn, DimInInt);

                        ZeroBody(&State[DimStateInt + DimIn], (DimInInt-DimIn)*2);
                        gap_waitbarrier_cc();
                }

                // switch on streamin
                int FiltSize;
                NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
                // printf("Nin %d, Rem_KI %d, Nb_KI %d\n", DimIn, Rem_KI, Nb_KI);

                // load configuration for the layer - input only
                if (Xin)
                {
                        NE16_Manual_SubTile_Linear(
                                cfg, &State[DimStateInt], &OutBuff[Nout], Wfi, DimInInt,
                                Nb_KO, Rem_KO, Infos[RNN_NE16_W_ZERO_POINT], FilterDataSizeBits, 1);
                }

                ChunkCell = ChunkSizeCC(Nout, 1);
                First = CoreId * ChunkCell;
                Last = Nout;

                gap_waitbarrier_cc();

                NE16_Manual_SubTile_Linear(
                        cfg, State, OutBuff, Wf, DimStateInt,
                        Nb_KO, Rem_KO, Infos[RNN_NE16_W_ZERO_POINT], FilterDataSizeBits, 0);

                // wait for end of computation
                NE16_BARRIER();

                // set priority to core side
                NE16_SETPRIORITY_CORE();
                if (Trace)
                {
                        dump_u8("inp_scale", Nout, &Scale[Nout]);
                        dump_u8("inp_scale_n", Nout, &ScaleN[Nout]);
                        dump_u8("state_scale", Nout, Scale);
                        dump_u8("state_scale_n", Nout, ScaleN);
                }
        }
        int NoOutScale = (Infos[RNN_NE16_OUT_SCALE] == 1 && Infos[RNN_NE16_OUT_SCALEN] == 0);
        gap_waitbarrier_cc();
        if (NoOutScale)
        {
                for (int o = First; o < Last; o++)
                {
                        int InputOut = AT_NORM(AT_NORM(OutBuff[o + Nout], Infos[RNN_NE16_I_PRE_N]) * *((unsigned char *)&Scale[Nout + o]), ScaleN[Nout + o]);
                        int StateOut = AT_NORM(AT_NORM(OutBuff[o], Infos[RNN_NE16_R_PRE_N]) * *((unsigned char *)&Scale[o]), ScaleN[o]);
                        unsigned short Of = ((unsigned short)gap_clip(
                                                Tanh(gap_clip(InputOut + StateOut, 15)),
                                                15)) +
                                            OutZeroPoint;

                        if (StateInOut)
                                StateInOut[TileOff + o] = Of;
                        if (Hout)
                                Hout[o] = Of;
                }
        }
        else
        {
                for (int o = First; o < Last; o++)
                {
                        int InputOut = AT_NORM(AT_NORM(OutBuff[o + Nout], Infos[RNN_NE16_I_PRE_N]) * *((unsigned char *)&Scale[Nout + o]), ScaleN[Nout + o]);
                        int StateOut = AT_NORM(AT_NORM(OutBuff[o], Infos[RNN_NE16_R_PRE_N]) * *((unsigned char *)&Scale[o]), ScaleN[o]);
                        unsigned short Of = ((unsigned short)gap_clip(
                                                AT_NORM(Tanh(gap_clip(InputOut + StateOut, 15)) * *((unsigned char *)&Infos[RNN_NE16_OUT_SCALE]), Infos[RNN_NE16_OUT_SCALEN]),
                                                15)) +
                                            OutZeroPoint;

                        if (StateInOut)
                                StateInOut[TileOff + o] = Of;
                        if (Hout)
                                Hout[o] = Of;
                }
        }
        gap_waitbarrier_cc();
}

static inline void LSTM_Queue_Jobs_UInt8(
    unsigned int CoreId,
    unsigned int cfg,
    volatile int *__restrict__ OutBuff1,
    volatile int *__restrict__ OutBuff2,
    volatile int *__restrict__ OutBuff3,
    unsigned char *__restrict__ Wf,
    unsigned char *__restrict__ Wi,
    unsigned char *__restrict__ Wg,
    unsigned char *__restrict__ Wo,
    unsigned int *__restrict__ Bf,
    unsigned int *__restrict__ Bi,
    unsigned int *__restrict__ Bg,
    unsigned int *__restrict__ Bo,
    unsigned char *__restrict__ Wfi,
    unsigned char *__restrict__ Wii,
    unsigned char *__restrict__ Wgi,
    unsigned char *__restrict__ Woi,
    unsigned char *__restrict__ State,
    unsigned char *__restrict__ Scale,
    signed char *__restrict__ Infos,
    int FilterDataSizeBits,
    int Nout,
    int Nb_KO,
    int Rem_KO,
    int DimIn,
    int DimState,
    int DimInInt,
    int DimStateInt,
    int Trace)
{
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);

        // I
        // I input
        int id_i = GetJobId();
        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState+DimStateInt], OutBuff1, Wii, &Bi[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue i INPUT %d %d\n", CoreId, id_i);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
        // NE16_BARRIER();
        // dump_i32("streamout_accum", Nout, OutBuff1);

        // I state
        Scale += 2 * Nout;
        id_i = GetJobId();
        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState], OutBuff1, Wi, Bi, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue i STATE %d %d\n", CoreId, id_i);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // G input - I input done when passed
        Scale += 2 * Nout;
        int id_g = GetJobId();

        // G

        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState+DimStateInt], OutBuff2, Wgi, &Bg[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue g INPUT %d %d\n", CoreId, id_g);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // G state - I input and start done when passed
        Scale += 2 * Nout;
        id_g = GetJobId();
        if (Trace)
                printf("Master Done i %d\n", CoreId);
        gap_waitbarrier_cc();

        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState], OutBuff2, Wg, Bg, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue g STATE %d %d\n", CoreId, id_g);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // F

        Scale += 2 * Nout;
        int id_f = GetJobId();
        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState+DimStateInt], OutBuff3, Wfi, &Bf[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue f INPUT %d %d\n", CoreId, id_f);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        Scale += 2 * Nout;
        id_f = GetJobId();
        if (Trace)
                printf("Master Done g %d\n", CoreId);
        gap_waitbarrier_cc();

        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState], OutBuff3, Wf, Bf, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue f STATE %d %d\n", CoreId, id_f);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // O

        Scale += 2 * Nout;
        int id_o = GetJobId();
        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState+DimStateInt], OutBuff2, Woi, &Bo[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue o INPUT %d %d\n", CoreId, id_o);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        Scale += 2 * Nout;
        id_o = GetJobId();
        if (Trace)
                printf("Master Done f %d\n", CoreId);
        gap_waitbarrier_cc();

        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimState], OutBuff2, Wo, Bo, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[LSTM_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue o STATE %d %d\n", CoreId, id_o);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
}

void LSTM_ParKerB32_NE16(KerLSTM_NE16_T *Arg)
{
        /*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

	*/
        int Trace = 0;
        int SingleCore = 0;

        unsigned char *__restrict__ StateInOut = Arg->StateInOut;
        unsigned char *__restrict__ State = Arg->State;
        unsigned char *__restrict__ Xin = Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;
        int *__restrict__ OutBuff1 = (int *)Arg->OutBuff1;
        int *__restrict__ OutBuff2 = (int *)Arg->OutBuff2;
        int *__restrict__ OutBuff3 = (int *)Arg->OutBuff3;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 0);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);
        if (Trace)
                printf("Entry %d\n", CoreId);
        if (Trace && SingleCore) {
                if (CoreId == 0) {
                        First = 0;
                        Last = Nout;
                } else {
                        First = Nout;
                }
        }
        if (CoreId != 8)
        {
                if (Trace)
                        printf("core %d\n", CoreId);
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                unsigned int StateChunk = ChunkSizeCC(DimState, 0);
                                unsigned int StateFirst = CoreId * StateChunk;
                                unsigned int StateLast = Min(StateFirst + StateChunk, DimState);
                                int Iter = Max(0, StateLast - StateFirst);
                                // Zero cell in - cell in is signed
                                ZeroBody(State + StateFirst, Iter);
                                // Zero h state - h state is unsigned
                                ZeroState8Body(State + DimState + StateFirst, Iter);
                        }
                        else
                        {
                                // c_state then h_state to leave zeros at end
                                Copy(State, StateInOut, DimState*2, CoreId);
                        }
                        if (Trace)
                                printf("%d wait copy/zero state\n", CoreId);
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        // copy input past c_state and h_state
                        Copy(State+DimState+DimStateInt, Xin, DimIn, CoreId);
                        if (Trace)
                                printf("%d wait copy input\n", CoreId);
                        gap_waitbarrier_cc();
                }

                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                gap_waitbarrier_cc();
                if (Trace)
                        printf("Start postprocess i %d\n", CoreId);
                for (int o = First; o < Last; o++)
                {
                        /* Oi = HSigmoid(Scaled(Oi)) */
                        OutBuff1[o] = SigmoidTable(OutBuff1[o], (unsigned short *)&Infos[0]);
                }

                // if (Trace)
                //         printf("Done i %d\n", CoreId);
                gap_waitbarrier_cc();
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("i_gate_after_act", Nout, OutBuff1);
                }
                if (Trace)
                        printf("Start postprocess g %d\n", CoreId);

                if (Trace && SingleCore && CoreId == 0)
                        printf("%s[%d] = { ", "cstate_c_i", Nout);

                for (int o = First; o < Last; o++)
                {
                        /* Og = HTanh(Scaled(Og)) */
                        /* Half of cell calculation i gate * g gate */
                        int Og = TanhTable(OutBuff2[o], (unsigned short *)&Infos[0]);
                        OutBuff1[o] = AT_NORM(OutBuff1[o] * Og, 15);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", OutBuff1[o], (o == Last - 1 ? "}\n" : ", "));
                }

                // if (Trace)
                //         printf("Done g %d\n", CoreId);
                gap_waitbarrier_cc();
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("i_gate_times_g_gate", Nout, OutBuff1);
                }
                if (Trace)
                        printf("Start postprocess f %d\n", CoreId);

                if (Trace && SingleCore && CoreId == 0) {
                        printf("CSTATE -- Scale %d ScaleN %d\n", ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALE], ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALEN]);
                        printf("%s[%d] = { ", "cstate_cbar_f_pre_post", Nout);
                }
                for (int o = First; o < Last; o++)
                {
                        /* Of = HSigmoid(Scaled(Of)) */
                        int X1 = SigmoidTable(OutBuff3[o], (unsigned short *)&Infos[0]);
                        /* Q15 * Cbar scale -> Q15 */
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d/%d/", X1, ((signed char)State[TileOff + o]));
                        X1 = ((signed char)State[TileOff + o]) * X1;
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d/", X1);
                        X1 = AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALE], ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALEN]);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", X1, (o == Last - 1 ? "}\n" : ", "));
                        // Finish compute of X1 in Q15
                        X1 += OutBuff1[o];
                        if (StateInOut)
                                *((signed char *)&StateInOut[TileOff + o]) = (signed char) gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_NE16_COUT_SCALE], ((unsigned char *)Infos)[LSTM_NE16_COUT_SCALEN]), 7);
                        /* Q15 -> Q12 -> Q15 */
                        OutBuff1[o] = TanhTable(AT_NORM(X1, 3), (unsigned short *)&Infos[0]);
                }

                if (Trace)
                        printf("Done f %d\n", CoreId);
                gap_waitbarrier_cc();
                if (Trace)
                        printf("Start postprocess o %d\n", CoreId);
                if (Trace && SingleCore && CoreId == 0)
                        printf("%s[%d] = { ", "output_before_scale", Nout);
                for (int o = First; o < Last; o++)
                {
                        /* Oo = HSigmoid(Scaled(Oo)) */
                        int Oo = SigmoidTable(OutBuff2[o], (unsigned short *)&Infos[0]);
                        Oo = AT_NORM(Oo * OutBuff1[o], 15);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", Oo, (o == Last - 1 ? "}\n" : ", "));
                        unsigned char X2 = (unsigned char)(
                                gap_clipu(
                                        AT_SCALE(
                                                Oo,
                                                ((unsigned char *)Infos)[LSTM_NE16_OUT_SCALE],
                                                ((unsigned char *)Infos)[LSTM_NE16_OUT_SCALEN]
                                        ) + (unsigned char)Infos[LSTM_NE16_OUT_ZEROPOINT],
                                        8));
                        if (StateInOut)
                                StateInOut[TileOff + DimState + o] = X2;
                        if (Hout)
                                Hout[o] = X2;
                }
                if (Trace)
                        printf("Done o %d\n", CoreId);
        }
        else
        {
                if (Trace)
                        printf("Master core %d DimState %d DimIn %d Nout %d\n", CoreId, DimState, DimIn, Nout);

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                signed char *Scale = Arg->ScaleNorm;

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;

                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {
                                ZeroBody(&State[DimState*2], DimStateInt-DimState);
                        }
                        if (Trace)
                                printf("%d wait zero state pad\n", CoreId);
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        ZeroBody(&State[DimStateInt+DimState+DimIn], DimInInt-DimIn);
                        if (Trace)
                                printf("%d wait zero input pad\n", CoreId);
                        gap_waitbarrier_cc();
                }

                LSTM_Queue_Jobs_UInt8(
                        CoreId, cfg, OutBuff1, OutBuff2, OutBuff3,
                        Arg->Wf, Arg->Wi, Arg->Wg, Arg->Wo,
                        Arg->Bf, Arg->Bi, Arg->Bg, Arg->Bo,
                        Arg->Wfi, Arg->Wii, Arg->Wgi, Arg->Woi,
                        State, Scale, Infos, FilterDataSizeBits, Nout,
                        Nb_KO, Rem_KO, DimIn, DimState, DimInInt, DimStateInt,
                        Trace);

                NE16_BARRIER();

                if (Trace)
                        printf("Master Done o %d %d\n", CoreId);
                gap_waitbarrier_cc();
                // set priority to core side
                NE16_SETPRIORITY_CORE();
        }
        if (Trace)
                printf("Final wait %d\n", CoreId);
        gap_waitbarrier_cc();
}

static inline void LSTM_Queue_Jobs_UInt16(
        unsigned int CoreId,
        unsigned int cfg,
        volatile int *__restrict__ OutBuff1,
        volatile int *__restrict__ OutBuff2,
        volatile int *__restrict__ OutBuff3,
        unsigned char *__restrict__ Wf,
        unsigned char *__restrict__ Wi,
        unsigned char *__restrict__ Wg,
        unsigned char *__restrict__ Wo,
        unsigned char *__restrict__ Wfi,
        unsigned char *__restrict__ Wii,
        unsigned char *__restrict__ Wgi,
        unsigned char *__restrict__ Woi,
        unsigned short *__restrict__ State,
        signed char *__restrict__ Infos,
        int FilterDataSizeBits,
        int Nout,
        int Nb_KO,
        int Rem_KO,
        int DimIn,
        int DimState,
        int DimInInt,
        int DimStateInt,
        pJobTriggerDesc_t pJob,
        int Trace)
{
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);

        // I
        // I input

        int Inp_Nb_KI, Inp_Rem_KI, Inp_Fs, State_Nb_KI, State_Rem_KI, State_Fs;
        NE16_Manual_SubTile_Linear_Setup(cfg, DimInInt, FilterDataSizeBits, &Inp_Nb_KI, &Inp_Rem_KI, &Inp_Fs);
        NE16_Manual_SubTile_Linear_Setup(cfg, DimStateInt, FilterDataSizeBits, &State_Nb_KI, &State_Rem_KI, &State_Fs);

        gap_waitbarrier_cc(); // Extra sync for cores to setup OutBuff1
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        if (Trace)
                printf("Master Queue i INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState+DimStateInt], &OutBuff1[Nout], Wii, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);
        // dump_i32("streamout_accum", Nout, OutBuff1);

        // State copied
        JobTriggerAwaitBufferCC(pJob);
        // I state
        if (Trace)
                printf("Master Queue i STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState], OutBuff1, Wi, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);
        // G
        // G input

        // OutBuff2 ready
        JobTriggerAwaitBufferCC(pJob); // 2

        if (Trace)
                printf("Master Queue g INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState+DimStateInt], &OutBuff2[Nout], Wgi, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);

        if (Trace)
                printf("Master Queue g STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState], OutBuff2, Wg, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);
        // OutBuff3 ready
        JobTriggerAwaitBufferCC(pJob);

        // F
        if (Trace)
                printf("Master Queue f INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState+DimStateInt], &OutBuff3[Nout], Wfi, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);

        if (Trace)
                printf("Master Queue f STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState], OutBuff3, Wf, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);
        // OutBuff 2 ready
        JobTriggerAwaitBufferCC(pJob);

        // O
        if (Trace)
                printf("Master Queue o INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState+DimStateInt], &OutBuff2[Nout], Woi, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);

        if (Trace)
                printf("Master Queue o STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimState], OutBuff2, Wo, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        NE16_BARRIER();
        if (Trace)
                printf("Master Done o %d %d\n", CoreId);

        // set priority to core side
        NE16_SETPRIORITY_CORE();
        JobTriggerReleaseAll(pJob);
}

void LSTM_ParKerB32_NE16fp(KerLSTM_NE16fp_T *Arg)
{
        /*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
			2 + 2 + 2 + 2 + 6 + 7  LSTM_INT group contains 3 shorts and 1 byte

		In total: 21	LSTM_CELL_INFOS
		if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
	*/
        int Trace = 0;
        int SingleCore = 0;

        unsigned short *__restrict__ StateInOut = Arg->StateInOut;
        unsigned short *__restrict__ State = Arg->State;
        unsigned short *__restrict__ Xin = Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned short *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;
        int *__restrict__ OutBuff1 = (int *)Arg->OutBuff1;
        int *__restrict__ OutBuff2 = (int *)Arg->OutBuff2;
        int *__restrict__ OutBuff3 = (int *)Arg->OutBuff3;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 0);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);
        if (Trace) {
                printf("Entry %d\n", CoreId);
                if (CoreId == 8) {
                        dump_16("cstate_in", DimState, StateInOut);
                        dump_u16("hstate_in", DimState, &StateInOut[DimState]);
                }
                gap_waitbarrier_cc();
                if (SingleCore) {
                        if (CoreId == 0) {
                                First = 0;
                                Last = Nout;
                        } else {
                                First = Nout;
                        }
                }
        }

        if (CoreId != 8)
        {
                unsigned char * Scalei = Arg->ScaleNorm;
                unsigned char * Scaler = &Arg->ScaleNorm[2*Nout];
                int GatePrenorm = Infos[LSTM_NE16_GATE_PRENORM];
                if (Trace)
                        printf("core %d FirstOut %d FirstCell %d Reset %d GatePrenorm %d\n", CoreId, Arg->FirstOut, Arg->FirstCell, Arg->Reset, GatePrenorm);

                if (Xin)
                {
                        // copy input past c_state and h_state
                        Copy(State+DimState+DimStateInt, Xin, DimIn * 2, CoreId);
                        if (Trace)
                                printf("%d wait copy input\n", CoreId);
                        gap_waitbarrier_cc();
                }

                for (int o = First; o < Last; o++) {
                        OutBuff1[o] = Arg->Bi[o];
                        OutBuff1[o+Nout] = Arg->Bi[o+Nout];
                }
                gap_waitbarrier_cc(); // OutBuff1 Loaded - ready for state calculation

                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                unsigned int StateChunk = ChunkSizeCC(DimState, 0);
                                unsigned int StateFirst = CoreId * StateChunk;
                                unsigned int StateLast = Min(StateFirst + StateChunk, DimState);
                                int Iter = Max(0, StateLast - StateFirst);
                                // Zero cell in - cell in is signed
                                ZeroBody(State + StateFirst, Iter * 2);
                                // Zero h state - h state is unsigned
                                ZeroState16Body(State + DimState + StateFirst, Iter, *((unsigned short *)&Infos[LSTM_NE16_OUT_ZEROPOINT]));
                                if (Trace)
                                        printf("%d wait zero state\n", CoreId);
                        }
                        else
                        {
                                // c_state then h_state to leave zeros at end
                                Copy(State, StateInOut, DimState * 4, CoreId);
                                if (Trace)
                                        printf("%d wait copy state\n", CoreId);
                        }
                }
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);

                for (int o = First; o < Last; o++) {
                        OutBuff2[o] = Arg->Bg[o];
                        OutBuff2[o+Nout] = Arg->Bg[o+Nout];
                }
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);

                for (int o = First; o < Last; o++) {
                        OutBuff3[o] = Arg->Bf[o];
                        OutBuff3[o+Nout] = Arg->Bf[o+Nout];
                }
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);

                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace)
                        printf("Start postprocess i %d\n", CoreId);
                for (int o = First; o < Last; o++)
                {
                        /* Oi = HSigmoid(Scaled(Oi)) */
                        int Oi = AT_SCALE(gap_roundnorm_reg(OutBuff1[o], GatePrenorm), Scaler[o], Scaler[o+Nout]);
                        Oi += AT_SCALE(gap_roundnorm_reg(OutBuff1[o+Nout], GatePrenorm), Scalei[o], Scalei[o+Nout]);
                        OutBuff1[o] = SigmoidTable(Oi, (unsigned short *)&Infos[0]);
                }

                // if (Trace)
                //         printf("Done i %d\n", CoreId);
                Scaler += Nout * 4;
                Scalei += Nout * 4;
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("i_gate_after_act", Nout, OutBuff1);
                }
                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace)
                        printf("Start postprocess g %d\n", CoreId);

                if (Trace && SingleCore && CoreId == 0)
                        printf("%s[%d] = { ", "c_gate_after_act", Nout);

                for (int o = First; o < Last; o++)
                {
                        /* Og = HTanh(Scaled(Og)) */
                        /* Half of cell calculation i gate * g gate */
                        int Og = AT_SCALE(gap_roundnorm_reg(OutBuff2[o], GatePrenorm), Scaler[o], Scaler[o+Nout]);
                        Og += AT_SCALE(gap_roundnorm_reg(OutBuff2[o+Nout], GatePrenorm), Scalei[o], Scalei[o+Nout]);
                        Og = TanhTable(Og, (unsigned short *)&Infos[0]);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", Og, (o == Last - 1 ? "}\n" : ", "));
                        OutBuff1[o] = AT_NORM(OutBuff1[o] * Og, 15);
                        OutBuff2[o] = Arg->Bo[o];
                        OutBuff2[o+Nout] = Arg->Bo[o+Nout];
                }

                if (Trace)
                        printf("Done g %d\n", CoreId);
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);

                Scaler += Nout * 4;
                Scalei += Nout * 4;
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("i_gate_times_g_gate", Nout, OutBuff1);
                }
                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace)
                        printf("Start postprocess f %d\n", CoreId);

                if (Trace && SingleCore && CoreId == 0) {
                        printf("CSTATE -- Scale %d ScaleN %d\n", ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALE], ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALEN]);
                        printf("%s[%d] = { ", "cstate_cbar_f_pre_post", Nout);
                }
                for (int o = First; o < Last; o++)
                {
                        /* Of = HSigmoid(Scaled(Of)) */
                        int X1 = AT_SCALE(gap_roundnorm_reg(OutBuff3[o],GatePrenorm), Scaler[o], Scaler[o+Nout]);
                        X1 += AT_SCALE(gap_roundnorm_reg(OutBuff3[o+Nout],GatePrenorm), Scalei[o], Scalei[o+Nout]);
                        X1 = SigmoidTable(X1, (unsigned short *)&Infos[0]);
                        /* Q15 * Cbar scale -> Q15 */
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d/%d/", X1, ((signed short)State[TileOff + o]));
                        X1 = ((short)State[TileOff + o]) * X1;
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d/", X1);
                        X1 = AT_SCALE(AT_NORM(X1, 8), ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALE], ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALEN]);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", X1, (o == Last - 1 ? "}\n" : ", "));
                        // Finish compute of X1 in Q15
                        X1 += OutBuff1[o];
                        if (StateInOut)
                                *((short *)&StateInOut[TileOff + o]) = (short) gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_NE16_COUT_SCALE], ((unsigned char *)Infos)[LSTM_NE16_COUT_SCALEN]), 15);
                        /* Q15 -> Q12 -> Q15 */
                        OutBuff1[o] = TanhTable(AT_NORM(X1, 3), (unsigned short *)&Infos[0]);
                }

                if (Trace)
                        printf("Done f %d\n", CoreId);

                Scaler += Nout * 4;
                Scalei += Nout * 4;
                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace)
                        printf("Start postprocess o %d\n", CoreId);
                if (Trace && SingleCore && CoreId == 0)
                        printf("%s[%d] %d = { ", "o_gate_after_act", Nout, TileOff);
                for (int o = First; o < Last; o++)
                {
                        /* Oo = HSigmoid(Scaled(Oo)) */
                        int Oo = AT_SCALE(gap_roundnorm_reg(OutBuff2[o],GatePrenorm), Scaler[o], Scaler[o+Nout]);
                        Oo += AT_SCALE(gap_roundnorm_reg(OutBuff2[o+Nout],GatePrenorm), Scalei[o], Scalei[o+Nout]);
                        Oo = SigmoidTable(Oo, (unsigned short *)&Infos[0]);
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", Oo, (o == Last - 1 ? "}\n" : ", "));
                        Oo = AT_NORM(Oo * OutBuff1[o], 15);
                        unsigned short X2 = (unsigned short)(
                                gap_clipu(
                                        AT_SCALE(
                                                Oo,
                                                ((unsigned char *)Infos)[LSTM_NE16_OUT_SCALE],
                                                ((unsigned char *)Infos)[LSTM_NE16_OUT_SCALEN]
                                        ) + *((unsigned short *)&Infos[LSTM_NE16_OUT_ZEROPOINT]),
                                        16));
                        if (StateInOut)
                                StateInOut[TileOff + DimState + o] = X2;
                        if (Hout)
                                Hout[o] = X2;
                }
                if (Trace)
                        printf("Done o %d\n", CoreId);
        }
        else
        {
                if (Trace)
                        printf("Master core %d DimStateInt %d DimInInt %d Nout %d\n", CoreId, DimStateInt, DimInInt, Nout);

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                signed char *Scale = Arg->ScaleNorm;

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;


                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {       if (Trace)
                                        printf("First out / First Cell\n");
                                ZeroBody(&State[DimState*2], (DimStateInt-DimState)*2);
                                if (!(JobTriggerAlloc(&Arg->Jobs)))
                                {
                                        if (Trace) printf("JobTrigger allocation failed\n");
                                        return;
                                }
                        }
                        if (Trace)
                                printf("%d wait zero state pad\n", CoreId);
                }
                if (Xin)
                {
                        ZeroBody(&State[DimStateInt+DimState+DimIn], (DimInInt-DimIn)*2);
                        if (Trace)
                                printf("%d wait zero input pad\n", CoreId);
                }
                JobTriggerReset(&Arg->Jobs);
                gap_waitbarrier_cc();

                LSTM_Queue_Jobs_UInt16(
                        CoreId, cfg, OutBuff1, OutBuff2, OutBuff3,
                        Arg->Wf, Arg->Wi, Arg->Wg, Arg->Wo,
                        Arg->Wfi, Arg->Wii, Arg->Wgi, Arg->Woi,
                        State, Infos, FilterDataSizeBits, Nout,
                        Nb_KO, Rem_KO, DimIn, DimState, DimInInt, DimStateInt,
                        &Arg->Jobs, Trace);

                if (Arg->LastCell && Arg->LastOut) {
                        JobTriggerFree(&Arg->Jobs);
                }
        }
        if (Trace)
                printf("Final wait %d\n", CoreId);
        gap_waitbarrier_cc();
        if (Trace) {
                if (CoreId == 8) {
                        dump_16("cstate_out", DimState, StateInOut);
                        dump_u16("hstate_out", DimState, &StateInOut[DimState]);
                }
                gap_waitbarrier_cc();
        }
}

static inline void GRU_Queue_Jobs_UInt8(
    unsigned int CoreId,
    unsigned int cfg,
    volatile int *__restrict__ OutBuff1,
    volatile int *__restrict__ OutBuff2,
    volatile int *__restrict__ OutBuff3,
    unsigned char *__restrict__ Wh,
    unsigned char *__restrict__ Wr,
    unsigned char *__restrict__ Wz,
    unsigned int *__restrict__ Bwh,
    unsigned int *__restrict__ Brh,
    unsigned int *__restrict__ Br,
    unsigned int *__restrict__ Bz,
    unsigned char *__restrict__ Whi,
    unsigned char *__restrict__ Wri,
    unsigned char *__restrict__ Wzi,
    unsigned char *__restrict__ State,
    unsigned char *__restrict__ Scale,
    signed char *__restrict__ Infos,
    int FilterDataSizeBits,
    int Nout,
    int Nb_KO,
    int Rem_KO,
    int DimIn,
    int DimState,
    int DimInInt,
    int DimStateInt,
    int Trace)
{
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);

        // R
        // R input
        int id_r = GetJobId();
        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, &State[DimStateInt], OutBuff1, Wri, &Br[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue r INPUT %d %d\n", CoreId, id_r);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // R state
        Scale += 2 * Nout;
        id_r = GetJobId();
        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, State, OutBuff1, Wr, Br, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue r STATE %d %d\n", CoreId, id_r);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        NE16_BARRIER(); // Trigger haddammard as soon as possible
        if (Trace)
                printf("Master trigger process R %d\n", CoreId);
        gap_waitbarrier_cc(); // Trigger R activation - 1

        // H
        // H state - r input done when passed
        // Note scales are reversed versus normal order
        Scale += 2 * Nout;
        int id_h = GetJobId();

        cfg &= ~(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, State, OutBuff2, Wh, Brh, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue h STATE %d %d\n", CoreId, id_h);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        NE16_BARRIER(); // Trigger haddammard as soon as possible
        if (Trace)
                printf("Master trigger process haddamard %d\n", CoreId);
        gap_waitbarrier_cc(); // Trigger H haddamard - 2

        Scale += 2 * Nout;
        id_h = GetJobId();
        if (Trace)
                printf("Master Done r %d\n", CoreId);

        SetupNE16Job(cfg, &State[DimStateInt], OutBuff3, Whi, Bwh, 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue h INPUT %d %d\n", CoreId, id_h);

        if (Trace)
                printf("Master Done h INPUT %d\n", CoreId);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
        NE16_BARRIER(); // Need to wait for haddamard to finish here to free OutBuff2. Could track Job ID to do this better or use semaphores + barriers
        if (Trace)
                printf("Master trigger h %d\n", CoreId);
        gap_waitbarrier_cc(); // Trigger H sum and activation - 3

        // Z
        Scale += 2 * Nout;
        int id_z = GetJobId();
        SetupNE16Job(cfg, &State[DimStateInt], OutBuff2, Wzi, &Bz[Nout], 4, DimInInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Done h STATE %d\n", CoreId);
        if (Trace)
                printf("Master Queue z INPUT %d %d\n", CoreId, id_z);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        // Z state
        Scale += 2 * Nout;
        id_z = GetJobId();
        cfg |= (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
        SetupNE16Job(cfg, State, OutBuff2, Wz, Bz, 4, DimStateInt, Nout, Nb_KO, Rem_KO, 1, 1, FilterDataSizeBits, Scale, &Scale[Nout], Infos[GRU_NE16_W_ZEROPOINT]);
        if (Trace)
                printf("Master Queue z STATE %d %d\n", CoreId, id_z);
        NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
        NE16_BARRIER();

        if (Trace)
                printf("Master Done z %d\n", CoreId);
        // set priority to core side
        NE16_SETPRIORITY_CORE();
        gap_waitbarrier_cc(); // - 4
}

void GRU_ParKerB32_NE16(KerGRU_NE16_T *Arg)
{
        /*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0
	*/
        int Trace = 0;
        int SingleCore = 0;

        unsigned char *__restrict__ StateInOut = Arg->StateInOut;
        unsigned char *__restrict__ State = Arg->State;
        unsigned char *__restrict__ Xin = Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;
        int *__restrict__ OutBuff1 = (int *)Arg->OutBuff1;
        int *__restrict__ OutBuff2 = (int *)Arg->OutBuff2;
        int *__restrict__ OutBuff3 = (int *)Arg->OutBuff3;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 0);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);
        if (Trace)
                printf("Entry %d\n", CoreId);
        if (Trace && SingleCore) {
                if (CoreId == 0) {
                        First = 0;
                        Last = Nout;
                } else {
                        First = Nout;
                }
        }
        if (CoreId != 8)
        {
                if (Trace)
                        printf("core %d\n", CoreId);
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                // Zero h state - h state is unsigned
                                ZeroState8(State, DimState, CoreId);
                        }
                        else
                        {
                                // c_state then h_state to leave zeros at end
                                Copy(State, StateInOut, DimState, CoreId);
                        }
                        if (Trace)
                                printf("%d wait copy/zero state\n", CoreId);
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        // copy input past c_state and h_state
                        Copy(State+DimStateInt, Xin, DimIn, CoreId);
                        if (Trace)
                                printf("%d wait copy input\n", CoreId);
                        gap_waitbarrier_cc();
                }

                if (Trace)
                        printf("%d wait start postprocess\n", CoreId);
                gap_waitbarrier_cc(); // - 1
                if (Trace)
                        printf("Start postprocess r %d\n", CoreId);
                for (int o = First; o < Last; o++)
                {
                        OutBuff1[o] = SigmoidTable(OutBuff1[o], (unsigned short *)&Infos[0]);
                }

                if (Trace)
                        printf("Done r %d\n", CoreId);
                gap_waitbarrier_cc(); // - 2
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("r_gate_after_act", Nout, OutBuff1);
                }
                if (Trace)
                        printf("Start postprocess hr haddamard %d\n", CoreId);


                for (int o = First; o < Last; o++)
                {
                        OutBuff1[o] = AT_NORM(gap_clip(OutBuff2[o], 15) * OutBuff1[o], 15);
                }
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("hr_haddamard_an", Nout, OutBuff1);
                }

                if (Trace)
                        printf("Done hr haddamard %d\n", CoreId);
                gap_waitbarrier_cc(); // - 3

                if (Trace)
                        printf("Start postprocess h %d\n", CoreId);

                for (int o = First; o < Last; o++)
                {
                        OutBuff1[o] = TanhTable(OutBuff1[o] + OutBuff3[o], (unsigned short *)&Infos[0]);
                }
                if (Trace && SingleCore && CoreId == 0) {
                        dump_i32("hr_gate_tanh", Nout, OutBuff1);
                }

                if (Trace)
                        printf("Done h %d\n", CoreId);
                gap_waitbarrier_cc(); // - 4
                if (Trace)
                        printf("Start postprocess z %d\n", CoreId);

                if (Trace && SingleCore && CoreId == 0)
                        printf("%s[%d] = { ", "h_state_out_prenorm", Nout);
                for (int o = First; o < Last; o++)
                {
                        // Compute z
                        int Out = SigmoidTable(OutBuff2[o], (unsigned short *)&Infos[0]);
                        // (1 - zt) (.) ht + zt (.) Ht-1
                        Out = (0x8000 - Out) * OutBuff1[o] + Out * (((int)State[o] - 128) << 8);
                        Out = AT_NORM(Out, (30-7));
                        if (Trace && SingleCore && CoreId == 0)
                                printf("%d%s", Out, (o == Last - 1 ? "}\n" : ", "));
                        unsigned char UCOut = gap_clipu(Out + 128, 8);

                        if (StateInOut)
                                StateInOut[TileOff + o] = UCOut;
                        if (Hout)
                                Hout[o] = UCOut;
                }
                if (Trace && SingleCore && CoreId == 0) {
                        dump_u8("h_out", Nout, Hout);
                }
                if (Trace)
                        printf("Done z %d\n", CoreId);
        }
        else
        {
                if (Trace)
                        printf("Master core %d DimState %d DimIn %d Nout %d\n", CoreId, DimState, DimIn, Nout);

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                signed char *Scale = Arg->ScaleNorm;

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;

                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {
                                ZeroBody(&State[DimState], DimStateInt-DimState);
                        }
                        if (Trace)
                                printf("%d wait zero state pad\n", CoreId);
                        gap_waitbarrier_cc();
                }
                if (Xin)
                {
                        ZeroBody(&State[DimStateInt+DimIn], DimInInt-DimIn);
                        if (Trace)
                                printf("%d wait zero input pad\n", CoreId);
                        gap_waitbarrier_cc();
                }

                GRU_Queue_Jobs_UInt8(
                        CoreId, cfg, OutBuff1, OutBuff2, OutBuff3,
                        Arg->Wh, Arg->Wr, Arg->Wz,
                        Arg->Bwh, Arg->Brh, Arg->Br, Arg->Bz,
                        Arg->Whi, Arg->Wri, Arg->Wzi,
                        State, Scale, Infos, FilterDataSizeBits, Nout,
                        Nb_KO, Rem_KO, DimIn, DimState, DimInInt, DimStateInt,
                        Trace);

        }
        if (Trace)
                printf("Final wait %d\n", CoreId);
        gap_waitbarrier_cc();
}


static inline void GRU_Queue_Jobs_UInt16(
    unsigned int CoreId,
    unsigned int cfg,
    volatile int *__restrict__ OutBuff1,
    volatile int *__restrict__ OutBuff2,
    unsigned char *__restrict__ Wh,
    unsigned char *__restrict__ Wr,
    unsigned char *__restrict__ Wz,
    unsigned char *__restrict__ Whi,
    unsigned char *__restrict__ Wri,
    unsigned char *__restrict__ Wzi,
    unsigned short *__restrict__ State,
    signed char *__restrict__ Infos,
    int FilterDataSizeBits,
    int Nout,
    int Nb_KO,
    int Rem_KO,
    int DimIn,
    int DimState,
    int DimInInt,
    int DimStateInt,
    pJobTriggerDesc_t pJob,
    int Trace)
{
        if (Trace)
                NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, 3);


        int Inp_Nb_KI, Inp_Rem_KI, Inp_Fs, State_Nb_KI, State_Rem_KI, State_Fs;
        NE16_Manual_SubTile_Linear_Setup(cfg, DimInInt, FilterDataSizeBits, &Inp_Nb_KI, &Inp_Rem_KI, &Inp_Fs);
        NE16_Manual_SubTile_Linear_Setup(cfg, DimStateInt, FilterDataSizeBits, &State_Nb_KI, &State_Rem_KI, &State_Fs);

        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        // R
        // R input

        if (Trace)
                printf("Master Queue r INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimStateInt], &OutBuff1[Nout], Wri, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[GRU_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);
        // dump_i32("streamout_accum", Nout, OutBuff1);

        JobTriggerAwaitBufferCC(pJob);
        // R state
        if (Trace)
                printf("Master Queue r STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, State, OutBuff1, Wr, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[GRU_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);

        // H
        // H state

        JobTriggerAwaitBufferCC(pJob);
        if (Trace)
                printf("Master Queue h STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, State, OutBuff2, Wh, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[GRU_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);

        if (Trace)
                printf("Master Queue h INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimStateInt], &OutBuff2[Nout], Whi, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[GRU_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);

        JobTriggerJobQueued(pJob, Nb_KO);

        // Z
        // Z input
        JobTriggerAwaitBufferCC(pJob);
        if (Trace)
                printf("Master Queue z INPUT %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, &State[DimStateInt], &OutBuff1[Nout], Wzi, DimInInt,
                Inp_Nb_KI, Inp_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, Inp_Fs, 1, -1, pJob);

        // Z state
        if (Trace)
                printf("Master Queue z STATE %d\n", CoreId);
        NE16_Manual_SubTile_Linear_Body(
                cfg, State, OutBuff1, Wz, DimStateInt,
                State_Nb_KI, State_Rem_KI, Nb_KO, Rem_KO, Infos[LSTM_NE16_W_ZEROPOINT],
                FilterDataSizeBits, State_Fs, 0, -1, pJob);

        if (Trace)
                printf("Master Done z %d\n", CoreId);
        NE16_BARRIER();
        // set priority to core side
        NE16_SETPRIORITY_CORE();
        JobTriggerReleaseAll(pJob);
}

void GRU_ParKerB32_NE16fp(KerGRU_NE16fp_T *Arg)
{
        /*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0
	*/
        int Trace = 0;
        int SingleCore = 0;

        unsigned short *__restrict__ StateInOut = Arg->StateInOut;
        unsigned short *__restrict__ State = Arg->State;
        unsigned short *__restrict__ Xin = Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        unsigned short int DimStateInt = Arg->DimStateInt;
        unsigned short int DimInInt = Arg->DimInInt;
        unsigned short *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;
        int *__restrict__ OutBuff1 = (int *)Arg->OutBuff1;
        int *__restrict__ OutBuff2 = (int *)Arg->OutBuff2;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSizeCC(Nout, 0);
        unsigned int First = CoreId * ChunkCell;
        unsigned int Last = Min(First + ChunkCell, Nout);
        if (Trace) {
                printf("Entry %d\n", CoreId);
                if (CoreId == 8) {
                        dump_u16("hstate_in", DimState, StateInOut);
                }
                gap_waitbarrier_cc();
                if (SingleCore) {
                        if (CoreId == 0) {
                                First = 0;
                                Last = Nout;
                        } else {
                                First = Nout;
                        }
                }
        }

        if (CoreId != 8)
        {
                unsigned char * Scale0 = Arg->ScaleNorm; // i for r & z, state for h
                unsigned char * Scale1 = &Arg->ScaleNorm[2*Nout]; // state for r & z, i for h
                int GatePrenorm = Infos[GRU_NE16_GATE_PRENORM];
                if (Trace)
                        printf("core %d FirstOut %d FirstCell %d Reset %d GatePrenorm %d\n", CoreId, Arg->FirstOut, Arg->FirstCell, Arg->Reset, GatePrenorm);
                if (Xin)
                {
                        // copy input past h_state
                        Copy(State+DimStateInt, Xin, DimIn * 2, CoreId);
                        if (Trace)
                                printf("%d wait copy input\n", CoreId);
                }
                for (int o = First; o < Last; o++) {
                        OutBuff1[o] = Arg->Br[o];
                        OutBuff1[o+Nout] = Arg->Br[o+Nout];
                }
                gap_waitbarrier_cc();
                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell && Arg->Reset)
                        {
                                ZeroState16(State, DimState, CoreId, 0x8000);
                                if (0)
                                        printf("%d wait zero state\n", CoreId);
                        }
                        else
                        {
                                // h_state 2 bytes
                                Copy(State, StateInOut, DimState * 2, CoreId);
                                if (0)
                                        printf("%d wait copy state\n", CoreId);
                        }
                }
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);
                for (int o = First; o < Last; o++) {
                        OutBuff2[o] = Arg->Brh[o];
                        OutBuff2[o+Nout] = Arg->Bwh[o];
                }
                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);
                if (Trace) {
                        if (CoreId==0) {
                                printf("buf ready\n");
                                printf("%d wait start postprocess\n", CoreId);
                        }
                }

                JobTriggerAwaitJob(&Arg->Jobs, CoreId);

                if (Trace) {
                        if (CoreId == 0) {
                                printf("Start postprocess r %d\n", CoreId);
                                dump_i32("r_gate_inp_before", Nout, &OutBuff1[Nout]);
                                dump_i32("r_gate_state_before", Nout, OutBuff1);
                        }
                        gap_waitbarrier(0);
                }
        
                for (int o = First; o < Last; o++)
                {
                        /* Oi = HSigmoid(Scaled(Oi)) */
                        OutBuff1[o] = AT_SCALE(gap_roundnorm_reg(OutBuff1[o], GatePrenorm), Scale1[o], Scale1[o+Nout]);
                        OutBuff1[o] += AT_SCALE(gap_roundnorm_reg(OutBuff1[o+Nout], GatePrenorm), Scale0[o], Scale0[o+Nout]);
                        OutBuff1[o] = SigmoidTable(gap_clip(OutBuff1[o], 15), (unsigned short *)&Infos[0]);
                }

                if (Trace)
                        printf("Done r %d\n", CoreId);

                JobTriggerAwaitJob(&Arg->Jobs, CoreId);

                if (Trace)
                        printf("Start postprocess hr haddamard%d\n", CoreId);

                // !!!!!!!!!!!!!!! These are swapped versus 8 bit !!!!!!!!!!!!!
                Scale0 += Nout * 4;
                Scale1 += Nout * 4;

                if (Trace) {
                        if (CoreId == 0) {
                                dump_i32("h_gate_state_before_scale", Nout, OutBuff2);

                        }
                        gap_waitbarrier(0);
                }

                for (int o = First; o < Last; o++)
                {
                        OutBuff2[o] = AT_SCALE(gap_roundnorm_reg(OutBuff2[o], GatePrenorm), Scale0[o], Scale0[o+Nout]);
                        OutBuff2[o] = AT_NORM(gap_clip(OutBuff2[o], 15) * OutBuff1[o], 15);
                        OutBuff1[o] = Arg->Bz[o];
                        OutBuff1[o+Nout] = Arg->Bz[o+Nout];
                }

                JobTriggerIndicateBuffer(&Arg->Jobs, CoreId);
                if (Trace) {
                        if (CoreId == 0) {
                                printf("Done hr haddamard %d\n", CoreId);
                                dump_i32("hr_haddamard_an", Nout, OutBuff2);
                        }
                        gap_waitbarrier(0);
                }

                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace&(CoreId==0))
                        printf("Start postprocess h %d\n", CoreId);

                for (int o = First; o < Last; o++)
                {
                        int Oih = AT_SCALE(gap_roundnorm_reg(OutBuff2[o+Nout], GatePrenorm), Scale1[o], Scale1[o+Nout]);
                        OutBuff2[o] = TanhTable(Oih + OutBuff2[o], (unsigned short *)&Infos[0]);
                }

                if (Trace&&(CoreId=0))
                        printf("Done h %d\n", CoreId);

                JobTriggerAwaitJob(&Arg->Jobs, CoreId);
                if (Trace) {
                        if (CoreId == 0) {
                               dump_i32("hr_gate_tanh", Nout, OutBuff2);
                                printf("Start postprocess z %d\n", CoreId);
                        }
                        gap_waitbarrier(0);
                }

                Scale0 += Nout * 4;
                Scale1 += Nout * 4;

                // if (Trace && SingleCore && CoreId == 0) {
                //         printf("CSTATE -- Scale %d ScaleN %d\n", ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALE], ((unsigned char *)Infos)[LSTM_NE16_CIN_SCALEN]);
                //         printf("%s[%d] = { ", "cstate_cbar_f_pre_post", Nout);
                // }

                for (int o = First; o < Last; o++)
                {
                        OutBuff1[o] = AT_SCALE(gap_roundnorm_reg(OutBuff1[o], GatePrenorm), Scale1[o], Scale1[o+Nout]);
                        OutBuff1[o] += AT_SCALE(gap_roundnorm_reg(OutBuff1[o+Nout],GatePrenorm), Scale0[o], Scale0[o+Nout]);
                        OutBuff1[o] = SigmoidTable(OutBuff1[o], (unsigned short *)&Infos[0]);
                        OutBuff1[o] = (0x8000 - OutBuff1[o]) * OutBuff2[o] + OutBuff1[o] * (((int)State[o] - 0x8000));
                        OutBuff1[o] = AT_NORM(OutBuff1[o], (30-15));
                        // if (Trace && SingleCore && CoreId == 0)
                        //         printf("%d%s", Out, (o == Last - 1 ? "}\n" : ", "));
                        unsigned short USOut = gap_clipu(OutBuff1[o] + 0x8000, 16);

                        if (StateInOut)
                                StateInOut[TileOff + o] = USOut;
                        if (Hout)
                                Hout[o] = USOut;
                }

                if (Trace) {
                        gap_waitbarrier(0);
                        if (CoreId == 0) {
                                printf("Done z %d\n", CoreId);
                                dump_u16("h_out", Nout, Hout);
                        }
                }
        }
        else
        {
                if (Trace)
                        printf("Master core %d DimStateInt %d DimInInt %d Nout %d\n", CoreId, DimStateInt, DimInInt, Nout);

                int Nb_KI, Rem_KI;
                int Nb_KO = Nout / 32 + (Nout % 32 ? 1 : 0);
                int Rem_KO = Nout % 32 ? Nout % 32 : 32; // Check different wrt simulator
                char FilterDataSizeBits = Arg->FilterDataSizeBits;
                signed char *Scale = Arg->ScaleNorm;

                unsigned int cfg = Arg->Default_NE16_Job_Cfg;


                if (Arg->FirstOut)
                {
                        if (Arg->FirstCell)
                        {       if (Trace)
                                        printf("First out / First Cell\n");
                                ZeroBody(&State[DimState], (DimStateInt-DimState)*2);
                                if (!(JobTriggerAlloc(&Arg->Jobs)))
                                {
                                        if (Trace) printf("JobTrigger allocation failed\n");
                                        return;
                                }
                        }
                        if (Trace)
                                printf("%d wait zero state pad\n", CoreId);
                }
                if (Xin)
                {
                        ZeroBody(&State[DimStateInt+DimIn], (DimInInt-DimIn)*2);
                        if (Trace)
                                printf("%d wait zero input pad\n", CoreId);
                }
                JobTriggerReset(&Arg->Jobs);
                gap_waitbarrier_cc();

                GRU_Queue_Jobs_UInt16(
                        CoreId, cfg, OutBuff1, OutBuff2,
                        Arg->Wh, Arg->Wr, Arg->Wz,
                        Arg->Whi, Arg->Wri, Arg->Wzi,
                        State, Infos, FilterDataSizeBits, Nout,
                        Nb_KO, Rem_KO, DimIn, DimState, DimInInt, DimStateInt,
                        &Arg->Jobs,
                        Trace);
                if (Arg->LastCell && Arg->LastOut) {
                        JobTriggerFree(&Arg->Jobs);
                }
        }
        if (Trace)
                printf("Final wait %d\n", CoreId);
        gap_waitbarrier_cc();
}
