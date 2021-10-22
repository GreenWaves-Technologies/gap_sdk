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

#ifndef __JOB_TRIGGER_H__
#define __JOB_TRIGGER_H__

#define MAX_NUM_TRIGGERS 5

#define TRACE 0

typedef struct JobTriggerDesc {
    unsigned int JobSem;
    unsigned int BufSem;
    char Triggers[MAX_NUM_TRIGGERS];
    int JobCount;
    int GetTrigIdx;
    int SetTrigIdx;
} JobTriggerDesc_t, *pJobTriggerDesc_t;

#include "Gap.h"

#ifdef __riscv__
#include "pmsis.h"
#else
#define pi_cl_sem_alloc()   0
#define pi_cl_sem_free
#define pi_cl_sem_set
#define pi_cl_sem_inc
#define pi_cl_sem_load_inc
#define pi_cl_sem_dec
#define pi_cl_sem_dec_cc
#define gap_waitbarrier(a)
#endif

inline static int JobTriggerAlloc(pJobTriggerDesc_t desc)
{
    if (TRACE) printf("alloc jobtrigger\n");
    if (!(desc->JobSem = pi_cl_sem_alloc())) {
        return 0;
    }
    if (!(desc->BufSem = pi_cl_sem_alloc())) {
        return 0;
    }
    return 1;
}

inline static void JobTriggerFree(pJobTriggerDesc_t desc)
{
    if (TRACE) printf("free jobtrigger\n");
    pi_cl_sem_free(desc->JobSem);
    pi_cl_sem_free(desc->BufSem);
}

inline static void JobTriggerReset(pJobTriggerDesc_t desc)
{
    for (int i=0; i<MAX_NUM_TRIGGERS; i++) desc->Triggers[i] = 0;
    desc->JobCount = 0;
    desc->GetTrigIdx = 0;
    desc->SetTrigIdx = 0;
    pi_cl_sem_set(desc->JobSem, 0);
    pi_cl_sem_set(desc->BufSem, 0);
}

inline static void JobTriggerAquireJob(pJobTriggerDesc_t desc)
{
    desc->JobCount += 1;
    if (TRACE) printf("acquire %d %d\n", desc->JobCount, desc->Triggers[desc->GetTrigIdx]);
    while (desc->Triggers[desc->GetTrigIdx]&&desc->Triggers[desc->GetTrigIdx]<=desc->JobCount) {
        if (TRACE) printf("release %d\n", desc->JobCount);
        pi_cl_sem_inc(desc->JobSem, 1);
        desc->GetTrigIdx += 1;
    }
}

inline static void JobTriggerJobQueued(pJobTriggerDesc_t desc, int Nb_KO)
{
    // Special case for 1 KO since cannot guarantee to queue 2 jobs so loks possible
    // Slightly less efficient but very small network
    if (Nb_KO == 1) {
        desc->SetTrigIdx++;
        NE16_BARRIER();
        if (TRACE) printf("release ne16 idle\n");
        pi_cl_sem_inc(desc->JobSem, 1);
    } else {
        if (TRACE) printf("queue at %d\n", desc->JobCount + 2);
        desc->Triggers[desc->SetTrigIdx++] = desc->JobCount + 2;
    }
}

inline static void JobTriggerReleaseAll(pJobTriggerDesc_t desc)
{
    if (TRACE) printf("release all %d %d\n", desc->JobCount, desc->SetTrigIdx);
    while (desc->GetTrigIdx++ <= desc->SetTrigIdx) pi_cl_sem_inc(desc->JobSem, 1);
}

inline static void JobTriggerAwaitJob(pJobTriggerDesc_t desc, int CoreId)
{
    if (CoreId == 0) {
        if (TRACE) printf("await %d\n", desc->JobCount);
        pi_cl_sem_dec(desc->JobSem);
    }
    gap_waitbarrier(0);
}

inline static void JobTriggerIndicateBuffer(pJobTriggerDesc_t desc, int CoreId)
{
    gap_waitbarrier(0);
    if (CoreId == 0) {
        if (TRACE)
            printf("indicate buffer ready %d\n", pi_cl_sem_load_inc(desc->BufSem));
        else
            pi_cl_sem_inc(desc->BufSem, 1);
    }
}

inline static void JobTriggerAwaitBufferCC(pJobTriggerDesc_t desc)
{
    if (TRACE) printf("await buffer\n");
    pi_cl_sem_dec_cc(desc->BufSem);
}
#endif