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
//#include "AutoTilerLib.h"
#include "CNN_Generator.h"

void CnnModel(unsigned int L1Memory)
{
  int i;

  SetInlineMode(SINGLE_INLINE);
  SetSymbolDynamics();
  SetUsedFilesNames("KernelLibStdTypes.h",1 , "CNN_BasicKernels.h");
  SetGeneratedFilesNames("CnnKernelsInit_hwce.c", "CnnKernelsInit_hwce.h", "CnnKernels_hwce.c", "CnnKernels_hwce.h");
  SetL1MemorySize(L1Memory);

  CNN_LoadSoftwareKernelLibrary();
  CNN_LoadHWCEKernelLibrary();


#ifdef RT_HAS_HWCE
  CNN_TiledConvNxMReLUPool2x2_HWCE_fp("Conv8x20MaxPool2x2_HWCE_0", 8, 20, 1,  32, 40, 98, 1);
  CNN_TiledConvNxMReLUPool2x2_HWCE_fp("Conv6x10_HWCE_1", 4, 10, 32,  32, 16, 39, 8);
#else

  // KWS configuration
  CNN_TiledConvNxMReLUPool2x2_SW_fp("ConvLayer1", 8, 20,  1, 32, 40, 98, 5);
  CNN_TiledConvNxMReLUPool2x2_SW_char("ConvLayer3", 4, 10, 32, 32, 16, 39, 4);
  CNN_TiledConvNxMReLUPool2x2_SW_fp("ConvLayer2", 4, 10, 32, 32, 16, 39, 4);
#endif

  
  //CNN_TiledLinearLayer("Dense2", 32, 13, 30, 12, 3, 0, 0);
  //CNN_TiledLinearLayer("Dense1", 16, 13, 30, 12, 1, 0, 0);
}

int main(int argc, char **argv)
{
	if (TilerParseOptions(argc, argv))
	{
		printf("Failed to initialize or incorrect output directory.\n"); return 1;
	}
	CnnModel(52000);
	GenerateTilingCode();
	return 0;
}

