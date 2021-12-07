/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "ompRt.h"
#include "omp.c"

/*
 * GCC wrapper
 */

#if !defined(__riscv__) && !defined(PLP_EU_HAS_DISPATCH)
void GOMP_parallel (void (*fn) (void*), void *data, int num_threads, unsigned int flags) {
  parallelRegion(data, fn, num_threads);
}
#endif

void GOMP_barrier() {
  omp_t *_this = omp_getData();
  userBarrier(getTeam(_this));
}

void GOMP_critical_start()
{
  userCriticalStart(getCurrentTeam());
}

void GOMP_critical_end()
{
  userCriticalEnd(getCurrentTeam());
}

int GOMP_sections_start (int count)
{

  return sectionInit(count);
}

void GOMP_sections_end()
{
  doBarrier(getCurrentTeam());
}

void GOMP_sections_end_nowait()
{
}

int GOMP_sections_next()
{
  return sectionGet();
}

void GOMP_atomic_start (void)
{
  criticalStart(getCurrentTeam());
}

void GOMP_atomic_end (void)
{
  criticalEnd(getCurrentTeam());
}

int GOMP_loop_dynamic_start(int start, int end, int incr, int chunk_size,
                        int *istart, int *iend)
{
  omp_team_t *team = getCurrentTeam();
  int isLast;

  return dynLoopInit(team, start, end, incr, chunk_size, istart, iend);
}



int GOMP_loop_dynamic_next (int *istart, int *iend)
{
  omp_team_t *team = getCurrentTeam();
  int isLast;
  int ret = dynLoopIter(team, istart, iend, &isLast);
  return ret;
}


void GOMP_loop_end()
{
  omp_team_t *team = getCurrentTeam();
  doBarrier(team);
}

int GOMP_loop_guided_start(int start, int end, int incr, int chunk_size,
                        int *istart, int *iend)
{
  return GOMP_loop_dynamic_start(start, end, incr, chunk_size, istart, iend);
}

int GOMP_loop_guided_next (int *istart, int *iend)
{
#if EU_VERSION == 3
  return 0;
#else
  return GOMP_loop_dynamic_next(istart, iend);
#endif
}

int GOMP_loop_static_start(int start, int end, int incr, int chunk_size,
                        int *istart, int *iend)
{
  return GOMP_loop_dynamic_start(start, end, incr, chunk_size, istart, iend);
}

int GOMP_loop_static_next (int *istart, int *iend)
{
#if EU_VERSION == 3
  return 0;
#else
  omp_team_t *team = getCurrentTeam();
  int isLast;
  int ret = dynLoopIter(team, istart, iend, &isLast);
  return ret;
#endif
}

#if 0
void GOMP_task(void (*func)(void *), void *data, void (*copy_func)(void *, void *),
	       long arg_size, long arg_align, int if_cond, unsigned gomp_flags, void **depend) 
{
  omp_team_t *team = getCurrentTeam();
  omp_t *_this = omp_getData();
  criticalStart(team);
  ompTask_t *task = allocTask(rtCc, _this, arg_size);
  if (task == NULL) {
    criticalEnd(team);
    //printf("Direct exec\n");
    func(data);
  } else {
    //printf("Enqueue %x %x %x %x %x\n", (int)func, (int)data, (int)copy_func, (int)arg_size, (int)arg_align);
    task->func = func;
    void *args = (void *)((unsigned int)task + sizeof(ompTask_t));
    rt_memcpy(args, data, arg_size);
    task->data = args;
    team->hasTasks = 1;
    if (team->firstReadyTask == NULL) team->firstReadyTask = task;
    else team->lastReadyTask->next = task;
    team->lastReadyTask = task;
    task->next = NULL;
    criticalEnd(team);
  }


#if 0
  int nbdepend = (uintptr_t) depend[0];
  int i;
  printf("%x %x %x %x %x %x %x %d\n", (int)func, (int)data, (int)copy_func, (int)arg_size, (int)arg_align, (int)if_cond, (int)gomp_flags, nbdepend);
  for (i = 0; i < nbdepend; i++)
    printf("%x\n", (int)depend[i+2]);
  func(data);
#endif
}
  #endif

int GOMP_single_start(void)
{ 
  return singleStart();
}

#if 0
void
GOMP_parallel_loop_dynamic_start (void (*fn) (void *), void *data,
                                  unsigned num_threads, long start, long end,
                                  long incr, long chunk_size)
{
  omp_team_t *team = getCurrentTeam();
  int isLast;
  rt_cc_t *rtCc = rt_getCcData(rt_clusterId());
  omp_t *_this = omp_getData();

  dynLoopInit(team, start, end, incr, chunk_size);
  parallelRegion(data, fn, num_threads, _this, rtCc);
}
#endif

void
GOMP_parallel_loop_dynamic (void (*fn) (void *), void *data,
                            unsigned num_threads, long start, long end,
                            long incr, long chunk_size, unsigned flags)
{
  omp_team_t *team = getCurrentTeam();
  int isLast;

  dynLoopInitSingle(team, start, end, incr, chunk_size, num_threads);
  parallelRegion(data, fn, num_threads);
}

void GOMP_loop_end_nowait()
{
}
