# pylint: disable=line-too-long, pointless-string-statement, unused-variable

from .templet import stringfunction
import os
import numpy as np

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
def generate_main_appl_template(G, gen, test_inputs=None, test_outputs=None, tolerance=0.0):
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

${gen.generate_main_appl_inout_def(test_inputs, test_outputs)}

static void cluster()
{
    #ifdef PERF
    printf("Start timer\\n");
    gap_cl_starttimer();
    gap_cl_resethwtimer();
    #endif

    ${gen.project_name}CNN(${gen.gen_inout_list()});
    printf("Runner completed\\n");
${gen.generate_output_check(tolerance, indent=1) if test_outputs else ""}
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
    int cur_fc_freq = pi_freq_set(PI_FREQ_DOMAIN_FC, ${gen.opts['fc_freq']});
    if (cur_fc_freq == -1)
    {
        printf("Error changing frequency !\\nTest failed...\\n");
        pmsis_exit(-4);
    }

    int cur_cl_freq = pi_freq_set(PI_FREQ_DOMAIN_CL, ${gen.opts['cl_freq']});
    if (cur_cl_freq == -1)
    {
        printf("Error changing frequency !\\nTest failed...\\n");
        pmsis_exit(-5);
    }
#ifdef __GAP9__
    pi_freq_set(PI_FREQ_DOMAIN_PERIPH, 250000000);
#endif
#endif
    // IMPORTANT - MUST BE CALLED AFTER THE CLUSTER IS SWITCHED ON!!!!
    printf("Constructor\\n");
    int ConstructorErr = ${gen.project_name}CNN_Construct();
    if (ConstructorErr)
    {
        printf("Graph constructor exited with error: %d\\n(check the generated file ${gen.project_name}Kernels.c to see which memory have failed to be allocated)\\n", ConstructorErr);
        pmsis_exit(-6);
    }


    printf("Call cluster\\n");
#ifndef __EMUL__
    struct pi_cluster_task task;
    pi_cluster_task(&task,NULL,NULL);
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
      for (unsigned int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
        printf("%45s: Cycles: %10u, Operations: %10u, Operations/Cycle: %f\\n", AT_GraphNodeNames[i], AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
        TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
      }
      printf("\\n");
      printf("%45s: Cycles: %10u, Operations: %10u, Operations/Cycle: %f\\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
      printf("\\n");
    }
#endif

    printf("Ended\\n");
    pmsis_exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    printf("\\n\\n\\t *** NNTOOL ${gen.project_name} Example ***\\n\\n");
    #ifdef __EMUL__
    test_${gen.project_name}();
    #else
    return pmsis_kickoff((void *) test_${gen.project_name});
    #endif
    return 0;
}

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
def generate_main_appl_make(G, gen, quantized, open_args=""):
    '''
NNTOOL=nntool
${"MODEL_SQ8=1"  if gen.G.has_expressions or "SQ8" in gen.G.quantization.schemes_present or any(qrec.cache.get("ne16")  for qrec in G.quantization.values()) else "# MODEL_SQ8=1"}
${"MODEL_POW2=1" if "POW2" in gen.G.quantization.schemes_present else "# MODEL_POW2=1"}
${"MODEL_FP16=1" if any(qrec.ktype == "float"  for qrec in G.quantization.values()) else "# MODEL_FP16=1"}
${"MODEL_NE16=1" if any(qrec.cache.get("ne16")  for qrec in G.quantization.values()) else "# MODEL_NE16=1"}

MODEL_SUFFIX?=
MODEL_PREFIX?=${gen.project_name}
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)

TRAINED_MODEL = ${os.path.split(G.graph_identity.filename)[1]}

MODEL_EXPRESSIONS = ${"$(MODEL_BUILD)/" + gen.opts['basic_kernel_source_file']}

NNTOOL_EXTRA_FLAGS += ${open_args}
${"MODEL_QUANTIZED=1" if quantized else ""}

# Memory sizes for cluster L1, SoC L2 and Flash
TARGET_L1_SIZE = ${gen.opts['l1_size']}
TARGET_L2_SIZE = ${gen.opts['l2_size']}
TARGET_L3_SIZE = ${gen.opts['l3_size']}

# Cluster stack size for master core and other cores
CLUSTER_STACK_SIZE=${gen.opts['cluster_stack_size']}
CLUSTER_SLAVE_STACK_SIZE=${gen.opts['cluster_slave_stack_size']}
CLUSTER_NUM_CORES=${gen.opts['cluster_num_cores']}

NNTOOL_SCRIPT = nntool_script
${"APP_CFLAGS += -DSTD_FLOAT" if any(qrec[1].out_qs[0].dtype == np.float16 for qrec in G.quantization.sorted_iterator(G)) else ""}
${"APP_LDFLAGS += -lm" if gen.G.has_expressions and "FLOAT" in gen.G.quantization.schemes_present else ""}
$(info GEN ... $(CNN_GEN))

'''

@stringfunction
# pylint: disable=unused-argument
def generate_main_appl_make_atproject(G, gen, quantized, model_path):
    '''
NNTOOL=nntool
${"MODEL_SQ8=1"  if gen.G.has_expressions or "SQ8" in G.quantization.schemes_present or any(qrec.cache.get("ne16")  for qrec in G.quantization.values()) else "# MODEL_SQ8=1"}
${"MODEL_POW2=1" if "POW2" in G.quantization.schemes_present else "# MODEL_POW2=1"}
${"MODEL_FP16=1" if "FLOAT" in G.quantization.schemes_present else "# MODEL_FP16=1"}
${"MODEL_NE16=1" if any(qrec.cache.get("ne16") for qrec in G.quantization.values()) else "# MODEL_NE16=1"}

MODEL_SUFFIX?=
MODEL_PREFIX?=${gen.project_name}
MODEL_PYTHON=python3
MODEL_BUILD=BUILD_MODEL$(MODEL_SUFFIX)

AT_MODEL_PATH=${model_path}

MODEL_EXPRESSIONS = ${gen.opts['basic_kernel_source_file']}

${"MODEL_QUANTIZED=1" if quantized else ""}

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
