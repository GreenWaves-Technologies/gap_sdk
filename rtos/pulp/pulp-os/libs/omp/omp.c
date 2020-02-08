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
#include "hal/pulp.h"

omp_t RT_L1_TINY_DATA ompData;
RT_L1_TINY_DATA int core_epoch[16];

static void handleReadyTasks(omp_team_t *team);

/*
 * OMP runtime library
 */

#if 0
static void handleReadyTasks(omp_team_t *team)
{
  criticalStart(team);
  ompTask_t *task = team->firstReadyTask;
  omp_t *_this = omp_getData();
  if (task) {
    team->firstReadyTask = task->next;
    if (team->firstReadyTask == NULL) {
      team->lastReadyTask = NULL;
      team->hasTasks = 0;
    }
    void (*func)(void *) = task->func;
    void *data = task->data;
    freeTask(rtCc, _this, task);
    criticalEnd(team);
    func(data);
  } else {
    criticalEnd(team);
  }
}
#endif

static inline void initTeam(omp_t *_this, omp_team_t *team)
{
}


extern int main();


static void __rt_omp_init(int exec_main)
{
  rt_trace(RT_TRACE_INIT, "OMP init\n");
  unsigned int cid = rt_cluster_id();
  unsigned int nbCores = rt_nb_pe();
  int coreMask = (1 << nbCores) - 1;
  omp_t *_this = omp_getData();

  _this->coreMask = coreMask;

  unsigned int stackSize = 0x400;
  //unsigned int stacks = (unsigned int)rt_alloc(RT_ALLOC_CL_DATA, stackSize*(nbCores-1));

  //plp_setSlaveStacks(coreMask, stacks, stackSize);

  // Plain team init, nested parllelism is not supported so this is the only used team
#if EU_VERSION >= 3
  eu_dispatch_team_config(coreMask);
  //_this->plainTeam.barrier = eu_bar_addr(0);
  eu_bar_setup(eu_bar_addr(0), coreMask);
#else
  pulp_barrier_setup(0, nbCores, _this->coreMask);
#endif
  _this->plainTeam.nbThreads = nbCores;
#if !defined(ARCHI_EU_HAS_DYNLOOP) && EU_VERSION == 3
  _this->plainTeam.loop_epoch = 0;
  _this->plainTeam.loop_is_setup = 0;
#endif
  //_this->plainTeam.firstReadyTask = NULL;
  //_this->taskPool = NULL;
  //_this->plainTeam.hasTasks = 0;
  #if 0
  ompTask_t *taskPool = (ompTask_t *)rt_l1_alloc(rtCc, sizeof(ompTask_t)*rtCc->nbCores*2);
  unsigned int i;
  for (i=0; i<rtCc->nbCores*2; i++) {
    ompTask_t *task = &taskPool[i];
    task->next = _this->taskPool;
    _this->taskPool = task;
  }
  #endif

  initTeam(_this, &_this->plainTeam);

#ifdef __LLVM__
  _this->numThreads = 0;
#endif

  // Set appropriate event unit masks for all cores remotly
//  unsigned int i; 
//  for (i=0; i<nbCores; i++)
//  {
//#if EU_VERSION >= 3
//    unsigned int mask = (1 << PULP_HW_BAR_EVENT) | (1 << PULP_MUTEX_EVENT) | (1 << PULP_DISPATCH_EVENT) | (1 << PULP_LOOP_EVENT);
//    eu_evt_maskSet_base(eu_evt_getClusterBase(i), mask);
//#else
//    unsigned int mask = (1 << PULP_HW_BAR_EVENT);
//    pulp_evtMask_coreSet(i, mask);
//#endif
//  }

#if EU_VERSION == 1
  plp_swMutex_init(&_this->plainTeam.mutex);
#endif

  //rt_l1_start(rtCc, NULL, NULL);
  rt_trace(RT_TRACE_INIT, "OMP init done\n");

  if (exec_main)
    main();
}

static void __rt_omp_init_no_main()
{
  __rt_omp_init(1);
}

RT_BOOT_CODE __attribute__((constructor))  void omp_constructor()
{
  int err = 0;

  __rt_cluster_entry =  __rt_omp_init_no_main;
}

int rt_omp_start() {
  __rt_omp_init(0);
  return 0;
}

void rt_omp_stop() {
}

int omp_get_thread_num(void) {
  omp_t *omp = omp_getData();
  return getThreadNum(omp);
}

int omp_get_num_threads(void) {
  omp_t *omp = omp_getData();
  omp_team_t *team = getTeam(omp);
  return team->nbThreads;
}

void partialParallelRegion(void (*fn) (void*), void *data, int num_threads)
{
  omp_t *_this = omp_getData();
  int coreMask = _this->coreMask;
  
  // We differentiate plain team and partial teams to put more optimizations on plain teams
  // as they are the most used
  omp_team_t *team = &_this->plainTeam;
  unsigned int coreSet = (1<<num_threads)-1;
  int nbCores = team->nbThreads;

  eu_bar_setup(eu_bar_addr(0), coreSet);
  eu_dispatch_team_config(coreSet);
  team->nbThreads = num_threads;

#if defined(ARCHI_HAS_CC)

  eu_bar_setup_mask(eu_bar_addr(1), coreSet | (1<<8), coreSet | (1<<8));

  if (num_threads == ARCHI_CLUSTER_NB_PE + 1)
    parallelRegionExec(data, fn);
  else
    parallelRegionExec_nocc(data, fn);

#else

  parallelRegionExec(data, fn);

#endif

  eu_bar_setup(eu_bar_addr(0), coreMask);
  eu_dispatch_team_config(coreMask);
  // After a partial team has been executed, the loop core epochs are desynchronized,
  // Realign them in order to not disturb plain teams
#ifdef ARCHI_EU_HAS_DYNLOOP
  eu_loop_initEpoch(eu_loop_addr(1), coreMask);
#else
  team->loop_epoch = 0;
  for (int i=0; i<team->nbThreads; i++)
  {
    core_epoch[i] = 0;
  }
#endif
  team->nbThreads = nbCores;
}

RT_L1_TINY_DATA unsigned int parallelTemp0;
