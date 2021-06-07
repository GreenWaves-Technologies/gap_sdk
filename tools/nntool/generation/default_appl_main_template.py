# pylint: disable=line-too-long, pointless-string-statement, unused-variable

from .templet import stringfunction

GWT_HEADER = '''
/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
'''

@stringfunction
# pylint: disable=unused-argument
def generate_main_appl_template(G, gen):
    '''
${GWT_HEADER}

/* Autotiler includes. */
#include "${gen.project_name}.h"
#include "${gen.project_name}Kernels.h"

#ifdef __EMUL__
#define pmsis_exit(n) exit(n)
#endif

#ifndef STACK_SIZE
#define STACK_SIZE      1024
#endif

${gen.flash_pointer} ${gen.project_name}_L3_Flash = 0;

${gen.generate_main_appl_inout_def()}

static void cluster()
{
    #ifdef PERF
    printf("Start timer\\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

    ${gen.project_name}CNN(${gen.gen_inout_list()});
    printf("Runner completed\\n");
}

int test_${gen.project_name}(void)
{
    printf("Entering main controller\\n");
    /* ----------------> 
     * Put here Your input settings
     * <---------------
     */
    

#ifndef __EMUL__
    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\\n");
        pmsis_exit(-4);
    }
#endif

    printf("Constructor\\n");
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    if (${gen.project_name}CNN_Construct())
    {
        printf("Graph constructor exited with an error\\n");
        pmsis_exit(-5);
    }

    printf("Call cluster\\n");
#ifndef __EMUL__
    struct pi_cluster_task task = {0};
    task.entry = cluster;
    task.arg = NULL;
    task.stack_size = (unsigned int) STACK_SIZE;
    task.slave_stack_size = (unsigned int) SLAVE_STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);
#else
    cluster();
#endif

    ${gen.project_name}CNN_Destruct();

#ifdef PERF
    {
      unsigned int TotalCycles = 0, TotalOper = 0;
      printf("\\n");
      for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
        TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
      }
      printf("\\n");
      printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
      printf("\\n");
    }
#endif

    printf("Ended\\n");
    pmsis_exit(0);
    return 0;
}

#ifndef __EMUL__
int main()
{
    printf("\\n\\n\\t *** NNTOOL ${gen.project_name} Example ***\\n\\n");
    return pmsis_kickoff((void *) test_${gen.project_name});
}
#else
int main(int argc, char *argv[])
{
    printf("\\n\\n\\t *** NNTOOL ${gen.project_name} Example ***\\n\\n");
    test_${gen.project_name}();
    return 0;
}
#endif
'''

@stringfunction
# pylint: disable=unused-argument
def generate_main_appl_header(G, gen):
    '''
#ifndef __${gen.project_name}_H__
#define __${gen.project_name}_H__

#define __PREFIX(x) ${gen.project_name} ## x

// Include basic GAP builtins defined in the Autotiler
#include "Gap.h"

#ifdef __EMUL__
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h>
#endif

extern ${gen.flash_pointer} ${gen.project_name}_L3_Flash;
#endif
'''

@stringfunction
# pylint: disable=unused-argument
def generate_main_appl_make(G, gen):
    '''
NNTOOL=nntool
${"MODEL_SQ8=1"  if any(qrec.ktype == "scaled"    for qrec in G.quantization.values()) else ""}
${"MODEL_POW2=1" if any(qrec.ktype == "symmetric" for qrec in G.quantization.values()) else ""}
${"MODEL_FP16=1" if any(qrec.ktype == "float"     for qrec in G.quantization.values()) else ""}
${"MODEL_NE16=1" if any(qrec.ktype == "ne16"      for qrec in G.quantization.values()) else ""}
MODEL_SUFFIX?=
MODEL_PREFIX?=${gen.project_name}
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)

TRAINED_MODEL = ${G.graph_identity.filename}
MODEL_PATH = $(MODEL_BUILD)/$(MODEL_PREFIX).${G.graph_identity.filename.split(".")[1]}

TENSORS_DIR = $(MODEL_BUILD)/tensors
MODEL_TENSORS = $(MODEL_BUILD)/$(MODEL_PREFIX)_L3_Flash_Const.dat

MODEL_STATE = $(MODEL_BUILD)/$(MODEL_PREFIX).json
MODEL_SRC = $(MODEL_PREFIX)Model.c
MODEL_HEADER = $(MODEL_PREFIX)Info.h
MODEL_GEN = $(MODEL_BUILD)/$(MODEL_PREFIX)Kernels $(MODEL_BUILD)/${gen.opts['basic_kernel_header_file'].split(".")[0] if G.has_expressions else ""}
MODEL_GEN_C = $(addsuffix .c, $(MODEL_GEN))
MODEL_GEN_CLEAN = $(MODEL_GEN_C) $(addsuffix .h, $(MODEL_GEN))
MODEL_GEN_EXE = $(MODEL_BUILD)/GenTile

MODEL_EXPRESSIONS = ${"$(MODEL_BUILD)/" + gen.opts['basic_kernel_source_file'] if gen.G.has_expressions else ""}

# Memory sizes for cluster L1, SoC L2 and Flash
TARGET_L1_SIZE = ${gen.opts['l1_size']}
TARGET_L2_SIZE = ${gen.opts['l2_size']}
TARGET_L3_SIZE = ${gen.opts['l3_size']}

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=${gen.opts['cluster_stack_size']}
CLUSTER_SLAVE_STACK_SIZE=${gen.opts['cluster_slave_stack_size']}
CLUSTER_NUM_CORES=${gen.opts['cluster_num_cores']}

NNTOOL_SCRIPT = nntool_script
$(info GEN ... $(CNN_GEN))

'''