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

#ifndef __STAGE_DESC_H__
#define __STAGE_DESC_H__

#if defined(__riscv__)
#include "pmsis.h"
#endif

// #define __STAGE_TRACE
#define NUM_STAGES 3

typedef struct StageDesc {
        unsigned int Sem[NUM_STAGES];
        int NumIters;
        int CurIters;
        int Stage;
} StageDesc_t, *pStageDesc_t;

inline static void StageDescAlloc(pStageDesc_t pStageDesc, int NumIters)
{
    for (int i = 0; i < NUM_STAGES; i++)
        pStageDesc->Sem[i] = pi_cl_sem_alloc();
    pStageDesc->Stage = 0;
    pStageDesc->NumIters = NumIters;
    // CurIters starts at 2 more to cover the 2 jobs that can be queued
    pStageDesc->CurIters = NumIters + 2;
}

inline static void StageDescFree(pStageDesc_t pStageDesc)
{
    for (int i = 0; i < NUM_STAGES; i++)
        pi_cl_sem_free(pStageDesc->Sem[i]);
}

inline static void StageDescTestCompleted(pStageDesc_t pStageDesc, int Stage)
{
#ifdef __STAGE_TRACE
    printf("await %d\n", Stage);
#endif
    pi_cl_sem_dec(pStageDesc->Sem[Stage - 1]);
}

inline static void StageDescIterJobAquired(pStageDesc_t pStageDesc)
{
    pStageDesc->CurIters--;
    if (pStageDesc->CurIters == 0)
    {
        pStageDesc->Stage++;
#ifdef __STAGE_TRACE
        printf("signal %d\n", pStageDesc->Stage);
#endif
        pStageDesc->CurIters = pStageDesc->NumIters;
        pi_cl_sem_inc(pStageDesc->Sem[pStageDesc->Stage - 1], 8);
    }
}

inline static void StageDescSignalStage(pStageDesc_t pStageDesc, int Stage)
{
#ifdef __STAGE_TRACE
    printf("signal %d\n", Stage);
#endif
    pi_cl_sem_inc(pStageDesc->Sem[Stage - 1], 8);
}

inline static void StageDescReset(pStageDesc_t pStageDesc, int NumIters)
{
    for (int i = 0; i < NUM_STAGES; i++) pi_cl_sem_set(pStageDesc->Sem[i], 0);
    pStageDesc->Stage = 0;
    pStageDesc->NumIters = NumIters;
    // CurIters starts at 2 more to cover the 2 jobs that can be queued
    pStageDesc->CurIters = NumIters + 2;
}
#endif
