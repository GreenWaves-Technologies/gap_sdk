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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"
#include "MobileNetCNN.c"


int main(int argc, char **argv)

{
        if (TilerParseOptions(argc, argv)) {
                printf("Failed to initialize or incorrect output arguments directory.\n"); return 1;
        }
        unsigned int L1Memory =  50000 ; 
        unsigned int L2Memory = 350000 ;
        unsigned int L3Memory = 8*1024*1024 ;

        SetSymbolDynamics();

        SetUsedFilesNames(0, 1, "CNN_BasicKernels.h");
        SetGeneratedFilesNames("MN_Kernels.c", "MN_Kernels.h");

        SetMemoryDeviceInfos(4,
            AT_MEM_L1, L1Memory, "L1_Memory", 0, 0,
            AT_MEM_L2, L2Memory, "L2_Memory", 0, 0,
            AT_MEM_L3_HRAM, L3Memory, "L3_Memory", 0, 1,
            AT_MEM_L3_HFLASH, 20*1024*1024, "0", "MN_L3_Flash_Const.bin", 0
        );

        AT_SetGraphCtrl(AT_GRAPH_MONITOR_CYCLES, AT_OPT_ON);
        AT_SetGraphCtrl(AT_GRAPH_MONITOR_CVAR_NAME, "MNPerf");
        AT_SetGraphCtrl(AT_GRAPH_PRODUCE_NODE_NAMES, AT_OPT_ON);
        AT_SetGraphCtrl(AT_GRAPH_PRODUCE_NODE_CVAR_NAME, "MNLName");
        AT_SetGraphCtrl(AT_GRAPH_NOINLINE_NODE, AT_OPT_ON);

        AT_SetGraphCtrl(AT_GRAPH_PRODUCE_OPERINFOS, AT_OPT_ON);
        AT_SetGraphCtrl(AT_GRAPH_PRODUCE_OPERINFOS_CVAR_NAME, "MNOperCount");
//        AT_SetGraphCtrl(AT_GRAPH_TRACE_EXEC, AT_OPT_ON);
//        AT_SetGraphCtrl(AT_GRAPH_DUMP_TENSOR, AT_OPT_VAL(4));


        AT_SetGraphCtrl(AT_GRAPH_REORDER_CONSTANT_IN, AT_OPT_ON);


        LoadCNNLibrary();
        MobileNetCNN();
        GenerateTilingCode();
        return 0;
}

