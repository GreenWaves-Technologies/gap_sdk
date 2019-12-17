/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

// Custom template file

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"

void ${gen.project_name}Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    KernelOper_T Cop = KOP_CONV;

    SetSymbolDynamics();

    AT_SetGraphCtrl(AT_GRAPH_MONITOR_CYCLES, AT_OPT_ON);
    AT_SetGraphCtrl(AT_GRAPH_MONITOR_CVAR_NAME, "MnistPerf");
    AT_SetGraphCtrl(AT_GRAPH_PRODUCE_NODE_NAMES, AT_OPT_ON);
    AT_SetGraphCtrl(AT_GRAPH_PRODUCE_NODE_CVAR_NAME, "MnistLName");
    AT_SetGraphCtrl(AT_GRAPH_NOINLINE_NODE, AT_OPT_ON);

    AT_SetGraphCtrl(AT_GRAPH_PRODUCE_OPERINFOS, AT_OPT_ON);
    AT_SetGraphCtrl(AT_GRAPH_PRODUCE_OPERINFOS_CVAR_NAME, "MnistOperCount");

//    AT_SetGraphCtrl(AT_GRAPH_REORDER_CONSTANT_IN, AT_OPT_ON);

    SetUsedFilesNames(0, 2, "CNN_BasicKernels.h", "${gen.project_name}.h");
    SetGeneratedFilesNames("${gen.project_name}Kernels.c", "${gen.project_name}Kernels.h");

${gen.memory_device_generator(indent=1,L3RamUserManaged=0,L3FlashUserManaged=0)}

    SetAT_TestFile("AT_${gen.project_name}Test.inc");

    LoadCNNLibrary();

${gen.kernel_generator(indent=1)}

#define GRAPH
#ifdef GRAPH
    CreateGraph("${gen.project_name}CNN",
        /* Arguments either passed or globals */
${gen.global_generator(indent=2,forceinputLocation='',forceoutputLocation='')},
        /* Locals, allocated dynamically */
${gen.local_generator(indent=2)}
    );

${gen.binding_generator(indent=1)}
    CloseGraph();
#endif
}

int main(int argc, char **argv)

{
    if (TilerParseOptions(argc, argv)) {
            printf("Failed to initialize or incorrect output arguments directory.\\n"); return 1;
    }
    ${gen.project_name}Model(52000, 300*1024, 8*1024*1024, 20*1024*1024);
    GenerateTilingCode();
    return 0;
}