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

#ifndef __NE16_MANUAL_SUBTILE_H__
#define __NE16_MANUAL_SUBTILE_H__

#include "Gap.h"
#include "NE16_Job_Trigger.h"
#include "NE16_Helpers.h"

inline static void NE16_Manual_SubTile_Linear_Start(unsigned int Cfg, void *InPointer, int NumIn, int Nb_KI, int WOff, int Qw)
{
        SetNE16_InPointer(InPointer);
        SetNE16_Dim(Nb_KI, 1, 1, 1);
        SetNE16_ConfigPad((v4s){0, 0, 0, 0}, 0);
        SetNE16_ConfigFMask((v4s){0, 0, 0, 0});
        SetNE16_GenConfig(Cfg);
        SetNE16_WOffset(WOff);
        SetNE16_Strides(16, 0, 0,    // In_D0, In_D1 - unused, In_D2 - unused
                        4 * 8, 0, 0, // Out_D0, Out_D1 - unused, Out_D2 - unused
                        // OutBytes * 8, 0, 0,                              // Out_D0, Out_D1 - unused, Out_D2 - unused
                        NumIn * 2 / 16, Qw * NumIn * 2 / 16,
                        Qw * NumIn * 2); // Weights_D0, Weights_D1, Weights_D2
}

inline static void NE16_Manual_SubTile_Linear_iter(void *OutPointer, void *WeightsPointer, int Rem_KI, int Rem_KO)
{
        SetNE16_OutPointer(OutPointer);
        SetNE16_WeightsPointer(WeightsPointer);
        SetNE16_Reminders(0, 0, Rem_KI, Rem_KO, 0, 0);
}

inline static void NE16_Manual_SubTile_Linear_Setup(
    unsigned int Cfg,
    int NumIn,
    int Qw,
    int *Nb_KI,
    int *Rem_KI,
    int *FiltSize)
{
        CalcNumIn(Cfg, NumIn, Rem_KI, Nb_KI);
        *FiltSize = Qw * (NumIn >> 3);
}

inline static void NE16_Manual_SubTile_Linear_Body(
    unsigned int Cfg,
    void *InPointer,
    int *OutPointer,
    unsigned char *WeightsPointer,
    int NumIn,
    int Nb_KI,
    int Rem_KI,
    int Nb_KO,
    int Rem_KO,
    int WOff,
    int Qw,
    int FiltSize,
    int IsInput,
    int JobId,
    pJobTriggerDesc_t pJobs)
{
        int Trace = 0;
        for (int Ko = 0; Ko < Nb_KO; Ko++)
        {
                int Off = Ko * 32;
                int IsLastKo = (Ko == (Nb_KO - 1));
                int NumOutC = (IsLastKo ? Rem_KO : 32);
                if (JobId == -1)
                        JobId = GetJobId();
                if (pJobs)
                        JobTriggerAquireJob(pJobs);
                if (Trace)
                {
                        printf(
                                "  State START_JOB\n    input=%d\n    k_out_major=%d\n    off=%d\n    is_last_ko=%d\n    job_id=%d\n",
                                IsInput, Ko, Off, IsLastKo, JobId);
                }
                JobId = -1;
                // Initialize both shadow register sets
                if (Ko < 2)
                {
                        NE16_Manual_SubTile_Linear_Start(Cfg, InPointer, NumIn, Nb_KI, WOff, Qw);
                }
                NE16_Manual_SubTile_Linear_iter(&OutPointer[Off], &WeightsPointer[Off * FiltSize], Rem_KI, NumOutC);
                NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
        }
}

inline static void NE16_Manual_SubTile_Linear(
    unsigned int Cfg,
    void *InPointer,
    int *OutPointer,
    unsigned char *WeightsPointer,
    int NumIn,
    int Nb_KO,
    int Rem_KO,
    int WOff,
    int Qw,
    int IsInput)
{
        int Nb_KI;
        int Rem_KI;
        int FiltSize;
        NE16_Manual_SubTile_Linear_Setup(Cfg, NumIn, Qw, &Nb_KI, &Rem_KI, &FiltSize);
        NE16_Manual_SubTile_Linear_Body(Cfg, InPointer, OutPointer, WeightsPointer, NumIn, Nb_KI, Rem_KI,
                                 Nb_KO, Rem_KO, WOff, Qw, FiltSize, IsInput, -1, 0);
}

#endif
