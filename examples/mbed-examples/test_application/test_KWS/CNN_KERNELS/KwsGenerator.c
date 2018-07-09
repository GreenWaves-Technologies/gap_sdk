/*
 * Copyright (c) 2017 GreenWaves Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of GreenWaves Technologies SAS nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
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

