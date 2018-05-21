/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/**
 * @defgroup Example Cluster Performance Monitoring
 *
 * This test monitor the cycles and instructions taken to execute functions that does n printf on 8 cores of the cluster. This is only for cluster, not for the Fabric Controller.
 *
 * The perf driver use HW performance counter to perform measurements using a structure of type rt_perf_t.
 * This structure needs to
 *   be initialized rt_perf_init() and configured rt_perf_conf()
 *   explicitely requested to sample the HW counter through rt_perf_save()
 *   read to get the measurement by rt_perf_get()
 * The HW counter usage is the following:
 *   rt_perf_reset() to set its value to 0
 *   rt_perf_start() to make it start counting
 *   rt_perf_stop() to make it stop counting
 * The driver uses 1 32bits counter.
 *
 * GAP8 specificities
 * The counter used is 1 32bits among 2 32bits that could also be managed as 1 64bits.
 * There is no FPU so the following events to measure are not available:
 * RT_PERF_CYCLES         Total number of cycles (also includes the cycles where the core is sleeping).
 * RT_PERF_ACTIVE_CYCLES  Counts the number of cycles the core was active (not sleeping).
 * RT_PERF_INSTR          Counts the number of instructions executed.
 * RT_PERF_LD_STALL       Number of load data hazards.
 * RT_PERF_JR_STALL       Number of jump register data hazards.
 * RT_PERF_IMISS          Cycles waiting for instruction fetches, i.e. number of instructions wasted due to non-ideal caching.
 * RT_PERF_LD             Number of data memory loads executed. Misaligned accesses are counted twice.
 * RT_PERF_ST             Number of data memory stores executed. Misaligned accesses are counted twice.
 * RT_PERF_JUMP           Number of unconditional jumps (j, jal, jr, jalr).
 * RT_PERF_BRANCH         Number of branches. Counts both taken and not taken branches.
 * RT_PERF_BTAKEN         Number of taken branches.
 * RT_PERF_RVC            Number of compressed instructions executed.
 * RT_PERF_LD_EXT         Number of memory loads to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external (cluster only).
 * RT_PERF_ST_EXT         Number of memory stores to EXT executed. Misaligned accesses are counted twice. Every non-TCDM access is considered external (cluster only).
 * RT_PERF_LD_EXT_CYC     Cycles used for memory loads to EXT. Every non-TCDM access is considered external (cluster only).
 * RT_PERF_ST_EXT_CYC     Cycles used for memory stores to EXT. Every non-TCDM access is considered external (cluster only).
 * RT_PERF_TCDM_CONT      Cycles wasted due to TCDM/log-interconnect contention (cluster only).
 * RT_PERF_CSR_HAZARD     Cycles wasted due to CSR access.
 */


#include "rt/rt_api.h"

#define STACK_SIZE           2048
#define CLUSTER_PWON         1
#define CLUSTER_PWDOWN       0
#define CLUSTER_GAP          0
#define CLUSTER_FLAG_DEFAULT 0

#define TAB_SIZE 100

static rt_perf_t *cluster_perf;

/** \brief A function to monitor
 * \param nbrLoop The number of printf's to display
 */
void funct2monitor(unsigned int nbrLoop)
{
  unsigned i;
  for(i=0; i<nbrLoop; i++)
  {
    printf("loop iteration n=%d\n", i);
  }
}

/** \brief Measure the number of Cycles and Instructions to execute funct2monitor
 *
 * \param perf The necessary structure to do measurement
 * \param nbrPrint Number of printf's to execute in the function to monitor
 *
 * Procedure to do the measurement is as follow :
 * I) Initialize and configure the measurement
 * Initialize the rt_perf_t structure performance counter to 0
 * Configure the events on which to do the measurement
 * Reset the HW performance counter to 0
 * II) Perform the measurements
 * Start the measurement
 * Execute the code to measure
 * Stop the HW performances counters
 * Save the HW performances counters in the rt_perf_t structure
 * Read each monitored measurement
 */
void measure(rt_perf_t *perf, unsigned int nbrPrint)
{
  unsigned int instr, cycles;

  rt_perf_init(perf);
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  rt_perf_reset(perf);

  rt_perf_start(perf);

  rt_team_fork(0, (void *) funct2monitor, (void *) nbrPrint);

  rt_perf_stop(perf);
  rt_perf_save(perf);

  instr = rt_perf_get(perf, RT_PERF_INSTR);
  cycles = rt_perf_get(perf, RT_PERF_CYCLES);

  printf("\tPerformance of funct2monitor: cycles %d instructions %d\n", cycles, instr);
}

void test_on_cluster()
{
  rt_perf_t *perf = cluster_perf;

  //function use of printf to explicitly show the function duration
  measure(perf, 10);
  measure(perf, 20);
}

int main()
{
  if (rt_event_alloc(NULL, 4)) goto err;

  //power on the cluster
  rt_cluster_mount(CLUSTER_PWON, CLUSTER_GAP, CLUSTER_FLAG_DEFAULT, NULL);


  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) goto err;

  //allocate stacks for all the cluster core
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) goto err;

  //run test_on_cluster
  rt_cluster_call(NULL, CLUSTER_GAP, test_on_cluster, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);

  //power off the cluster
  rt_cluster_mount(CLUSTER_PWDOWN, CLUSTER_GAP, 0, NULL);
  printf("Test success\n");

  return 0;
err:
  printf("Test failed\n");
  return -1;
}

