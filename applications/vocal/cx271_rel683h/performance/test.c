/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

/**
 * @defgroup Example Cluster Performance Monitoring
 *
 * This test monitor the cycles and instructions taken to execute functions that does n printf or n sprintf on one core of the cluster. This is only for cluster, not for the Fabric Controller.
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


#include <stdio.h>
#include "rt/rt_api.h"


extern int sprintf(char *str, const char *fmt, ...);

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

/** \brief A function to monitor
 * \param nbrLoop The number of sprintf's used to modulate execution time
 */
void funct2monitor2(unsigned int nbrLoop)
{
  unsigned i;
  char tab[TAB_SIZE];

  if (nbrLoop <= TAB_SIZE)
  {
    for(i=0; i<nbrLoop; i++)
    {
      sprintf(tab+i, "%d", i);
    }
  }
  else
    printf("%s: Incorrect parameters\n", __func__);
}

typedef signed short fract;

volatile signed short output = 0;
//void *vocal_allocate_data(void);
//void *vocal_data=NULL;
void init_dsp_process(void);
int dsp_process_block(fract *input_mics, fract *input_spk, fract *output, int mics, int samples);
volatile signed short output_buff[128] = {0};
signed short input_samples[] = {
#include "4mics_samples.h"
};

//signed short input_samples[16000] = {
//0
//};


void funct2monitor3(unsigned int nbrLoop)
{
	unsigned i, j, k;
	signed short input = 1;
	j = 0;

	k = sizeof (input_samples)/sizeof(signed short);
	while (k > j) {
	  fract input[4*128];
		for (i = 0; i < 128; i++) {
			input[4*i + 0] = input_samples[j++];
			input[4*i + 1] = input_samples[j++];
			input[4*i + 2] = input_samples[j++];
			input[4*i + 3] = input_samples[j++];
		}
		dsp_process_block(input, NULL, output_buff, 4, 128);
		//	printf("you %d\n",j);
		if (0) for (i = 0; i < 128; i++) {
			printf("%d, ", output_buff[i]);		
		}
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
//  vocal_data = vocal_allocate_data();
  init_dsp_process();

  funct2monitor3(nbrPrint);

  rt_perf_stop(perf);
  rt_perf_save(perf);

  instr = rt_perf_get(perf, RT_PERF_INSTR);
  cycles = rt_perf_get(perf, RT_PERF_CYCLES);

  printf("\tPerformance of funct2monitor3: cycles %d instructions %d\n", cycles, instr);
}

/** \brief Measure the number of Cycles and Instructions to execute funct2monitor2, nbrLoop times and the total execution
 * \param nbrPrint Allow to modulate duration of the function to monitor
 * \param nbrLoop Number of execution of the monitored function
 */
void parallel_measure(unsigned int nbrPrint, unsigned int nbrLoop)
{
  rt_perf_t *perf = cluster_perf;
  unsigned int i;
  //instr[0] and nbrLoop[0] set to 0 as initial measurement value
  // 0 < i < nbrLoop+2, instr[i] nbrLoop[i] measure one execution of funct2monitor2
  //instr[nbrLoop+2] cycles[nbrLoop+2] are the total measurement value
  unsigned int instr[nbrLoop+2], cycles[nbrLoop+2];

  rt_perf_init(perf);
  rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  rt_perf_reset(perf);
  instr[0] = rt_perf_get(perf, RT_PERF_INSTR);
  cycles[0] = rt_perf_get(perf, RT_PERF_CYCLES);
//  vocal_data = vocal_allocate_data();
  init_dsp_process();

  //start the monitoring
  rt_perf_start(perf);
  for(i=1; i<nbrLoop+1;i++)
  {
//    funct2monitor2(nbrPrint);
    funct2monitor3(nbrPrint);
    //save the monitoring parameters, RT_PERF_CYCLES and RT_PERF_INSTR, in the perf structure
    rt_perf_save(perf);
    //save these values in a table
    instr[i] = rt_perf_get(perf, RT_PERF_INSTR);
    cycles[i] = rt_perf_get(perf, RT_PERF_CYCLES);
    //Initialize again the perf structure in order to avoid cumulative counting for next measurement
    rt_perf_init(perf);
    rt_perf_conf(perf, (1<<RT_PERF_CYCLES) | (1<<RT_PERF_INSTR));
  }
  //get the total measurement
  rt_perf_save(perf);
  instr[nbrLoop+2] = rt_perf_get(perf, RT_PERF_INSTR);
  cycles[nbrLoop+2] = rt_perf_get(perf, RT_PERF_CYCLES);
  //stop the HW counter used for monitoring
  rt_perf_stop(perf);
  for(i=1; i<nbrLoop+1;i++)
  {
    printf("\tPerf of funct2monitor2: loop_n=%d cycles %d instructions %d\n", i, cycles[i]-cycles[i-1], instr[i]-instr[i-1]);
  }
  printf("\tPerf of funct2monitor2: total for all loop: cycles %d instructions %d\n", cycles[nbrLoop+2]-cycles[0], instr[nbrLoop+2]-instr[0]);
}

void test_on_cluster(void * args)
{
  rt_perf_t *perf = cluster_perf;

  printf("* 1st monitoring test: Call a function several times\n");
  perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (NULL == perf) return;
  //function use of printf to explicitly show the function duration
  measure(perf, 10);
  measure(perf, 20);

  printf("** 2nd monitoring test: call same function several times. Monitor individual and total execution time of the function\n");
  //rt_perf_t instantiated inside parallel_measure
  //use of sprintf this time to avoid polluting the output and focus on measurements
  parallel_measure(20, 1);

}

int main()
{
  rt_event_t *event;

  event = NULL;
  //power on the cluster
  rt_cluster_mount(CLUSTER_PWON, CLUSTER_GAP, CLUSTER_FLAG_DEFAULT, event);

  cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
  if (cluster_perf == NULL) return -1;

  //allocate stacks for all the cluster core
  void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
  if (stacks == NULL) return -1;

  //run test_on_cluster
  rt_cluster_call(NULL, CLUSTER_GAP, test_on_cluster, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), event);
  //test_on_cluster(NULL);
  //power off the cluster
  rt_cluster_mount(CLUSTER_PWDOWN, CLUSTER_GAP, 0, event);

  return 0;
}

