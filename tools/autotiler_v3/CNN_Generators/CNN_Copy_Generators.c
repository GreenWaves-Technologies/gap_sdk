/*
 * Copyright (C) 2018 GreenWaves Technologies
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
#include "AutoTilerLib.h"
#include "CNN_Copy_Generators.h"
#include "Gap.h"

#define MaxS(a, b) (((int)(a)>(int)(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
#define Abs(x) (((x)<0)?-(x):(x))

#define D0	KER_ITER_D0
#define D1	KER_ITER_D1
#define D2	KER_ITER_D2
#define D3	KER_ITER_D3
#define T0	KER_ITER_TILE0
#define T1	KER_ITER_TILE1
#define T2	KER_ITER_TILE2

typedef void (*add_kernel_arg_func_t) (char *Name, char *ArgName, int Dim, ...);
typedef char *(*cnn_kernel_arg_datatype_func_t) (int DataSize, int Pointer, int Restrict);

static int copy_library_loaded = 0;

// Returns floor of square root of x
int floorSqrt(int x)
{
    // Base cases
    if (x == 0 || x == 1)
    return x;
 
    // Staring from 1, try all numbers until
    // i*i is greater than or equal to x.
    int i = 1, result = 1;
    while (result <= x)
    {
      i++;
      result = i * i;
    }
    return i - 1;
}

int primeFactors(int n) 
{
	int original_n = n;
	int W = 1;
    // Print the number of 2s that divide n 
    while (n % 2 == 0 && W < n) 
    { 
        W *= 2;
        n = n/2; 
    } 
  
    // n must be odd at this point. So we can skip 
    // one element (Note i = i +2) 
    for (int i = 3; i <= floorSqrt(n); i = i + 2) 
    { 
        // While i divides n, print i and divide n 
        while (n % i == 0 && W < n) 
        { 
            W *= i;
            n = n/i; 
        } 
    } 
  
    // This condition is to handle the case when n 
    // is a prime number greater than 2 
    if (n > 2 && W < n) 
        W = n;

	return Max(W, n);
}

int largest_factor(int sz)
{
	int limit = floorSqrt(sz);
	for (int i=2; i<=limit; i++)
    {
        int c=0;
        for (int j=1; j<=i; j++)
        {
            if (i%j==0)
            {
                c++;
            }
        }
	    if (c==2 && sz%i==0) return sz/i;
    }
	return 1;
}

void LoadCNN_Copy_Library()

{
        if (copy_library_loaded) return;
        copy_library_loaded = 1;

	LibKernelTemplate("KerCopy_void_T",
		  CArgs(3,
		  	TCArg("void *__restrict__", "In"),
		  	TCArg("void *__restrict__", "Out"),
		  	TCArg("unsigned int", "Size")
		       )
	);
	LibKernelTemplate("CNN_FpUFp_T",
			CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_UFpFp_T",
                        CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpFps_T",
			CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_FpUFps_T",
                        CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_UFpFps_T",
			CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_UFpUFps_T",
                        CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_UFpUFp_T",
                        CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );


	LibKernelTemplate("CNN_FpsUFps_T",
			CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_UFpsFps_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_UFpsUFps_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpsFps_T",
                        CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpFp_T",
                        CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpsFp_T",
			CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_UFpsFp_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_FpsUFp_T",
			CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
			)
        );
	LibKernelTemplate("CNN_UFpsUFp_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_Float16Fp_T",
                        CArgs(5,
                        TCArg("F16 *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_Float16UFp_T",
                        CArgs(5,
                        TCArg("F16 *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_Float16Fps_T",
                        CArgs(5,
                        TCArg("F16 *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_Float16UFps_T",
                        CArgs(5,
                        TCArg("F16 *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpFloat16_T",
                        CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("F16 *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_UFpFloat16_T",
                        CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("F16 *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_FpsFloat16_T",
                        CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("F16 *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("CNN_UFpsFloat16_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("F16 *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_Float32Fp_T",
                        CArgs(5,
                        TCArg("float *__restrict__", "In"),
                        TCArg("signed short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_Float32UFp_T",
                        CArgs(5,
                        TCArg("float *__restrict__", "In"),
                        TCArg("unsigned short *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_Float32Fps_T",
                        CArgs(5,
                        TCArg("float *__restrict__", "In"),
                        TCArg("signed char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_Float32UFps_T",
                        CArgs(5,
                        TCArg("float *__restrict__", "In"),
                        TCArg("unsigned char *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_FpFloat32_T",
                        CArgs(5,
                        TCArg("signed short *__restrict__", "In"),
                        TCArg("float *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_UFpFloat32_T",
                        CArgs(5,
                        TCArg("unsigned short *__restrict__", "In"),
                        TCArg("float *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_FpsFloat32_T",
                        CArgs(5,
                        TCArg("signed char *__restrict__", "In"),
                        TCArg("float *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
        LibKernelTemplate("CNN_UFpsFloat32_T",
                        CArgs(5,
                        TCArg("unsigned char *__restrict__", "In"),
                        TCArg("float *__restrict__", "Out"),
                        TCArg("unsigned short int", "W"),
                        TCArg("unsigned short int", "H"),
                        TCArg("signed char *__restrict__", "Infos")
                        )
        );
	LibKernelTemplate("KerMatTranspose_fps_T",
		CArgs(7,
			TCArg("signed char *__restrict__", "In"),
			TCArg("signed char *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy")
			)
	);
	LibKernelTemplate("KerMatTranspose_fp_T",
			CArgs(7,
			TCArg("short int *__restrict__", "In"),
			TCArg("short int *__restrict__", "Out"),
			TCArg("unsigned short int", "Feat"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H"),
			TCArg("unsigned char", "Sx"),
			TCArg("unsigned char", "Sy")
			)
	);


	/****************************************************************************************************************/
	/* Kernels generic copy  */
	/****************************************************************************************************************/

	LibKernel("CNN_Copy_void", CALL_PARALLEL,
		  CArgs(3,
		  	TCArg("void *__restrict__", "In"),
		  	TCArg("void *__restrict__", "Out"),
		  	TCArg("unsigned int", "N")
		       ),
		  "KerCopy_void_T", NULL
	);
	LibKernel("CNN_NormRGB565_offset_fps", CALL_PARALLEL,
		CArgs(6,
			TCArg("unsigned short *__restrict__", "In"),
			TCArg("signed char *__restrict__", "Out0"),
			TCArg("signed char *__restrict__", "Out1"),
			TCArg("signed char *__restrict__", "Out2"),
			TCArg("unsigned short int", "W"),
			TCArg("unsigned short int", "H")
		     ),
		"KerNormRGB565_fps_T", NULL
	);

	LibKernel("CNN_NormRGB565_shift_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned short *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB565_fps_T", NULL
		 );

	LibKernel("CNN_NormRGB888_offset_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB888_fps_T", NULL
		 );

	LibKernel("CNN_NormRGB888_shift_fps", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out0"),
				TCArg("signed char *__restrict__", "Out1"),
				TCArg("signed char *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB888_fps_T", NULL
		 );
	
	LibKernel("CNN_NormRGB16_fp", CALL_PARALLEL,
			CArgs(6,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed short int *__restrict__", "Out0"),
				TCArg("signed short int *__restrict__", "Out1"),
				TCArg("signed short int *__restrict__", "Out2"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormRGB16_fp_T", NULL
		 );

	LibKernel("CNN_NormBW_offset_fps", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fps_T", NULL
		 );

	LibKernel("CNN_NormBW_shift_fps", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed char *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fps_T", NULL
		 );

	LibKernel("CNN_NormBW_fp", CALL_PARALLEL,
			CArgs(4,
				TCArg("unsigned char *__restrict__", "In"),
				TCArg("signed short int *__restrict__", "Out"),
				TCArg("unsigned short int", "W"),
				TCArg("unsigned short int", "H")
			     ),
			"KerNormBW_fp_T", NULL
		 );

	/****************************************************************************************************************/
	/* Kernels tensor type, scale and zeropoint conversions  */
	/****************************************************************************************************************/

	LibKernel("CNN_FpsUFps", CALL_PARALLEL, 0, "CNN_FpsUFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_ZEROPOINT),        0, 1, CNN_Type(1,0,0,0,-1), 0,0,0,0,1,1));
	LibKernel("CNN_FpsFp", CALL_PARALLEL, 0, "CNN_FpsFp_T",	                CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(1,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_FpsFpScaled", CALL_PARALLEL, 0, "CNN_FpsFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(1,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_FpsUFpScaled", CALL_PARALLEL, 0, "CNN_FpsUFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(1,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_FpsUFpsScaled", CALL_PARALLEL, 0, "CNN_FpsUFps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(1,0,0,0,-1), 0,0,0,0,1,1));
	LibKernel("CNN_FpsFpsScaled", CALL_PARALLEL, 0, "CNN_FpsFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsUFpsScaled", CALL_PARALLEL, 0, "CNN_UFpsUFps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-1,0,0,0,-1), 0,0,0,0,1,1));

	LibKernel("CNN_UFpsFps", CALL_PARALLEL, 0, "CNN_UFpsFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_ZEROPOINT),        0, 1, CNN_Type(-1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsFp", CALL_PARALLEL, 0, "CNN_FpsFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(-1,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsFpScaled", CALL_PARALLEL, 0, "CNN_UFpsFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-1,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsUFpScaled", CALL_PARALLEL, 0, "CNN_FpsUFp_T",        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-1,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsUFp", CALL_PARALLEL, 0, "CNN_UFpsUFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(-1,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsFpsScaled", CALL_PARALLEL, 0, "CNN_UFpsFps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-1,0,0,0,1), 0,0,0,0,1,1));

	LibKernel("CNN_FpUFp", CALL_PARALLEL, 0, "CNN_FpUFp_T",	                CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_ZEROPOINT),        0, 1, CNN_Type(2,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_FpFps", CALL_PARALLEL, 0, "CNN_FpFps_T",	                CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(2,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_FpFpsScaled", CALL_PARALLEL, 0, "CNN_FpFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(2,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_FpUFpsScaled", CALL_PARALLEL, 0, "CNN_FpUFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(2,0,0,0,-1), 0,0,0,0,1,1));
	LibKernel("CNN_FpUFpScaled", CALL_PARALLEL, 0, "CNN_FpUFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(2,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_FpFpScaled", CALL_PARALLEL, 0, "CNN_FpFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));

	LibKernel("CNN_UFpFp", CALL_PARALLEL, 0, "CNN_UFpFp_T",	                CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_ZEROPOINT),        0, 1, CNN_Type(-2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpFps", CALL_PARALLEL, 0, "CNN_UFpFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(-2,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpUFps", CALL_PARALLEL, 0, "CNN_UFpUFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP),                  0, 1, CNN_Type(-2,0,0,0,-1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpFpsScaled", CALL_PARALLEL, 0, "CNN_UFpFps_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-2,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpUFpsScaled", CALL_PARALLEL, 0, "CNN_UFpUFps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-2,0,0,0,-1), 0,0,0,0,1,1));
	LibKernel("CNN_UFpUFpScaled", CALL_PARALLEL, 0, "CNN_UFpUFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-2,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpFpScaled", CALL_PARALLEL, 0, "CNN_UFpFp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FP_SCALE),            0, 1, CNN_Type(-2,0,0,0,2), 0,0,0,0,1,1));

	LibKernel("CNN_Float16Fp", CALL_PARALLEL, 0, "CNN_Float16Fp_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_Float16UFp", CALL_PARALLEL, 0, "CNN_Float16UFp_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(2,0,0,0,-2), 0,0,0,0,1,1));
	LibKernel("CNN_Float16Fps", CALL_PARALLEL, 0, "CNN_Float16Fps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(2,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_Float16UFps", CALL_PARALLEL, 0, "CNN_Float16UFps_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(2,0,0,0,-1), 0,0,0,0,1,1));

	LibKernel("CNN_FpFloat16", CALL_PARALLEL, 0, "CNN_FpFloat16_T",	        CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpFloat16", CALL_PARALLEL, 0, "CNN_UFpFloat16_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(-2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_FpsFloat16", CALL_PARALLEL, 0, "CNN_FpsFloat16_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(1,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_UFpsFloat16", CALL_PARALLEL, 0, "CNN_UFpsFloat16_T",	CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(-1,0,0,0,2), 0,0,0,0,1,1));

        LibKernel("CNN_Float32Fp", CALL_PARALLEL, 0, "CNN_Float32Fp_T",         CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(4,0,0,0,2), 0,0,0,0,1,1));
        LibKernel("CNN_Float32UFp", CALL_PARALLEL, 0, "CNN_Float32UFp_T",       CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(4,0,0,0,-2), 0,0,0,0,1,1));
        LibKernel("CNN_Float32Fps", CALL_PARALLEL, 0, "CNN_Float32Fps_T",       CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(4,0,0,0,1), 0,0,0,0,1,1));
        LibKernel("CNN_Float32UFps", CALL_PARALLEL, 0, "CNN_Float32UFps_T",     CNN_Match(CNN_OperList(1, KOP_CONVERT_FL_FP),                  0, 1, CNN_Type(4,0,0,0,-1), 0,0,0,0,1,1));

        LibKernel("CNN_FpFloat32", CALL_PARALLEL, 0, "CNN_FpFloat32_T",         CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(2,0,0,0,4), 0,0,0,0,1,1));
        LibKernel("CNN_UFpFloat32", CALL_PARALLEL, 0, "CNN_UFpFloat32_T",       CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(-2,0,0,0,4), 0,0,0,0,1,1));
        LibKernel("CNN_FpsFloat32", CALL_PARALLEL, 0, "CNN_FpsFloat32_T",       CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(1,0,0,0,4), 0,0,0,0,1,1));
        LibKernel("CNN_UFpsFloat32", CALL_PARALLEL, 0, "CNN_UFpsFloat32_T",     CNN_Match(CNN_OperList(1, KOP_CONVERT_FP_FL),                  0, 1, CNN_Type(-1,0,0,0,4), 0,0,0,0,1,1));

	/****************************************************************************************************************/
	/* Kernels tensor/matrix transposes and permutations  */
	/****************************************************************************************************************/

	/* Matrix Transposition, byte, no scaling */
	LibKernel("CNN_ParTranspose_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_ParTransposeSxSy_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,-1,-1));
	LibKernel("CNN_Transpose_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_TransposeSxSy_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(1,0,0,0,1), 0,0,0,0,-1,-1));

	/* Tensor Permutation (CHW), byte, no scaling */
	LibKernel("CNN_MatPermCHW2CWH_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2CWH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HWC_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HWC), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WHC_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WHC), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WCH_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WCH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HCW_fps", CALL_PARALLEL, 0, "KerMatTranspose_fps_T",	CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HCW), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));

	/* Tensor Permutation (HWC), byte, no scaling */
        LibKernel("CNN_MatPermHWC2HCW_fps", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fps_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2HCW), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2WCH_fps", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fps_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2WCH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2WHC_fps", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fps_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2WHC), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2CHW_fps", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fps_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2CHW), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2CWH_fps", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fps_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2CWH), 0, 1, CNN_Type(1,0,0,0,1), 0,0,0,0,1,1));

	/* Matrix Transposition, short */
	LibKernel("CNN_ParTranspose_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_ParTransposeSxSy_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,-1,-1));
	LibKernel("CNN_Transpose_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",			CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_TransposeSxSy_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATTRANSP), 0, 0, CNN_Type(2,0,0,0,2), 0,0,0,0,-1,-1));

	/* Tensor Permutation (CHW), short */
	LibKernel("CNN_MatPermCHW2CWH_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2CWH), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HWC_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HWC), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WHC_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WHC), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2WCH_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2WCH), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
	LibKernel("CNN_MatPermCHW2HCW_fp", CALL_PARALLEL, 0, "KerMatTranspose_fp_T",		CNN_Match(CNN_OperList(1, KOP_MATPERM_CHW2HCW), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));

	/* Tensor Permutation (HCW), short, no scaling */
        LibKernel("CNN_MatPermHWC2HCW_fp", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fp_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2HCW), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2WCH_fp", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fp_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2WCH), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2WHC_fp", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fp_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2WHC), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2CHW_fp", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fp_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2CHW), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
        LibKernel("CNN_MatPermHWC2CWH_fp", CALL_PARALLEL|CALL_HWC_KER, 0, "KerMatTranspose_fp_T",
                                                                                        CNN_Match(CNN_OperList(1, KOP_MATPERM_HWC2CWH), 0, 1, CNN_Type(2,0,0,0,2), 0,0,0,0,1,1));
}

int CNN_Copy(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int Sz,
	int FeatureSize
)

{
	int Log = 1;

	unsigned long long int LayerOp = 0;
	unsigned long long int LayerBandwidth = 0;

	add_kernel_arg_func_t AddKArgDimFunc = AddKernelArgDim;

        if (Ctrl) {
		if (Ctrl->FloatDump != -1&&Ctrl->FloatDump) AddKArgDimFunc = AddKernelFloatArgDim;
        }

	LayerBandwidth += Sz*2;

	if (Log) {
		printf("CNN_Copy: %s\n", Name);
		printf("In  => Feat: 1  Sz: %4d, FeatureSize: %4d\n", Sz, Abs(FeatureSize));
		printf("Out => Feat: 1, Sz: %4d\n", Sz);
		printf("Nb Oper : %lld\n", LayerOp);
	}
	Kernel_T *Kernel = 0;
	Object_T **PKerArgs = AllocateKerArgs(2);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,D0), O_IN|O_DB,  1, 1, 1,  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out",  KerArgSpace(1,D0), O_OUT|O_DB, 1, 1, 1,  0, 0, 0, "Out");
	Kernel = UserKernel(
                Name,
                KernelIterSpace(1, IterParSpace(D0, Sz * Abs(FeatureSize), 4)),
                TILE_HOR,
                CArgs(2, TCArg(CNN_ArgDataType(1,1,1),  "In"), TCArg(CNN_ArgDataType(1,1,1), "Out")),
                Calls(1,
                        Call("CNN_Copy_void", LOC_D0,
                                Bindings(3,
                                        K_Arg("In", KER_ARG_TILE),      /* Input tile */
                                        K_Arg("Out", KER_ARG_TILE),    	/* Output tile */
                                        K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0)    /* Input tile size */
                                )
                        )
                ),
                PKerArgs
        );
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
		AddKArgDimFunc(Name, "In", 2, Sz, Abs(FeatureSize));
		AddKArgDimFunc(Name, "Out", 2, Sz, Abs(FeatureSize));
	}
	return (Kernel!=0);
}

static int CNN_MatTranspose_Internal(
        char *Name,

        CNN_GenControl_T *Ctrl,

	int Size,

        int Feat,
        int Width,
        int Height,

        KernelOper_T MatTransOper
)

{
        int Log = 1;
        Tile_Orientation_T TileOrientation = TILE_HOR;
        unsigned int OutTileOrientation;
        int ParFeat = 1, HWC = 0;
	add_kernel_arg_func_t AddKArgDimFunc = AddKernelArgDim;
        cnn_kernel_arg_datatype_func_t CNN_ArgDtype = CNN_ArgDataType;

        if (Ctrl) {
                if (Ctrl->TileOrientation != -1) TileOrientation = (Ctrl->TileOrientation==0)?TILE_HOR:TILE_VER;
                if (Ctrl->ParallelFeatures != -1) ParFeat = Ctrl->ParallelFeatures;
                if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
		if (Ctrl->FloatDump != -1&&Ctrl->FloatDump) AddKArgDimFunc = AddKernelFloatArgDim;
                if (Ctrl->FloatDump != -1&&Ctrl->FloatDump) CNN_ArgDtype = CNN_ArgDataTypeF;
        }
        if (HWC) {
                return CNN_3DTensorPermute(Name, Ctrl, Feat, Size, Width, Height, KOP_MATPERM_HWC2WHC);
        }
        unsigned long long int LayerOp = Width*Height*Feat*Size;
        unsigned long long int LayerBandwidth = 0;

        if (!(MatTransOper == KOP_MATTRANSP)) GenTilingError("CNN_MatTranspose_Internal Kernel: %s, MatTransOper should be KOP_MATTRANSP", Name);

        char *MatTransKerName = CNN_FindMatchingKernel(MatTransOper, KOP_NONE, ParFeat, Size, 0, 0, 0, Size, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
        if (MatTransKerName==0) GenTilingError("CNN_MatTranspose_Internal Kernel: %s, Can't find a matching basic kernel for Matrix Transpose", Name);

        if (TileOrientation==TILE_HOR) OutTileOrientation = OBJ_CONSTRAINTS_TILE_VER; else OutTileOrientation = OBJ_CONSTRAINTS_TILE_HOR;
        LayerBandwidth += (int64_t) Width*Height*Size;
        LayerBandwidth += (int64_t) Width*Height*Size;
        if (Log) {
                printf("CNN_MatTranspose: %s %s%s, %d\n", Name, ParFeat?"Par Feat":"", HWC?", HWC":", CHW", Size);
                printf("In  => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
                printf("Out => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
                if (MatTransKerName) printf("%20s: %s\n", "MatTransKerName", MatTransKerName);
                printf("Nb Oper : %lld\n", LayerOp);
        }

        Kernel_T *Kernel =
                UserKernel(Name,
                        (ParFeat)?
                        KernelIterSpace(2, IterParSpace(D0, Feat, 8), IterTiledSpace(T0)):
                        KernelIterSpace(2, IterFixedSpace(D0, Feat), IterTiledSpace(T0)),
                        TileOrientation,
                        CArgs(2,
                                TCArg(CNN_ArgDtype(Size,1,1),  "In"),
                                TCArg(CNN_ArgDtype(Size,1,1), "Out")
                        ),
                        Calls(1,
                                Call(MatTransKerName, LOC_LOOP,
                                        Bindings(7,
                                                K_Arg("In", KER_ARG_TILE),                      /* Input tile */
                                                K_Arg("Out", KER_ARG_TILE),                     /* Output tile */
                                                (ParFeat)?
                                                K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0):       /* Number of Matrices involved */
                                                Imm(1),                                         /* A single matrix */
                                                K_Arg("In", KER_ARG_TILE_W),                    /* Input tile width */
                                                K_Arg("In", KER_ARG_TILE_H),                    /* Input tile height */
                                                AT_IGNORE_ARG_BINDING,                          /* StrideX */
                                                AT_IGNORE_ARG_BINDING                           /* StrideY */
                                        )
                                )
                        ),
                        KerArgs(2,
                                KerArg("In",   KerArgSpace(2,D0,T0), O_IN|O_DB,  Width, Height, Size, 0, 0, 0, "In"),
                                KerArg("Out",  KerArgSpace(2,D0,T0), O_OUT|O_DB, Height, Width, Size, 0, OutTileOrientation, 0, "Out")
                        )
                );
        if (Kernel) {
                AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
                AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

                AddKArgDimFunc(Name, "In",  4, Feat, Height, Width, Size);
                AddKArgDimFunc(Name, "Out", 4, Feat, Width, Height, Size);
		AT_PrepareForTest(Name,
                                  (v4s) {Size, 0, 0, Size},
                                  (v4s) {0,0,0,0},
                                  Feat, Feat, Width, Height,
                                  0,0, 0,0, 0,0, (v4s) 0,
                                  0,0, 0,0, 0,0, (v4s) 0,
                                  MatTransOper,
                                  0, 0);
        }
        return (Kernel!=0);

}

/*********************************************************************************************************************************************************************
 	Generator for Matrix Transposition

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)

		In_DataSize:	1: byte, 2: half word,
		Out_DataSize:	1: byte, 2: half word

		In_Q:		In fixed point format
		Out_Q:		Out fixed point format

		In_InL3:	0: In is in L2, 1: In is in L3 memory
		Out_InL3:	0: Out is in L2, 1: Out is in L3 memory

		InFeat		Number of matrices
		Width		For 1x1 convolution, width of an input feature map
		Height		For 1x1 convolution, height of an input feature map

		Signature:	Name(In, Out)

	CNN_MatTranspose
	
*********************************************************************************************************************************************************************/

int CNN_MatTranspose(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Size,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatTransOper
)

{
	if (Ctrl && Ctrl->HWC) return CNN_3DTensorPermute(Name, Ctrl, Size, Feat, Width, Height, KOP_MATPERM_HWC2WHC);
	else     	       return CNN_MatTranspose_Internal(Name, Ctrl, Size, Feat, Width, Height, MatTransOper);
}

/*********************************************************************************************************************************************************************
 	Generator for 3D Tensor permutations:  CHW => {CWH, HWC, WHC, WCH, HCW}, HWC => {HCW, WCH, CWH, CHW, WHC}

	Template:
		Name:		Name of the generated user kernel

		Ctrl:		Overide generator default options (TileOrientation, Parallel Features), Def=(TILE_HOR, 1)
		Size		Size in bytes of In/Out tensors

		Feat		Number of channels of the tensor
		Width		Tensor width
		Height		Tensor height

		MatPermOper	Permutation oper:  KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH, KOP_MATPERM_CHW2HCW
						   KOP_MATPERM_HWC2HCW, KOP_MATPERM_HWC2WCH, KOP_MATPERM_HWC2CWH, KOP_MATPERM_HWC2CHW, KOP_MATPERM_HWC2WHC

		Signature:	Name(In, Out)

	CNN_3DTensorPermute
	
*********************************************************************************************************************************************************************/

int CNN_3DTensorPermute(
	char *Name,

	CNN_GenControl_T *Ctrl,

	int Size,

	int Feat,
	int Width,
	int Height,

	KernelOper_T MatPermOper
)

{
	int Log = 1;
	int ParFeat = 1, HWC = 0;
	unsigned long long int LayerOp = (int64_t) Width*Height*Feat;
	unsigned long long int LayerBandwidth = 0;

	add_kernel_arg_func_t AddKArgDimFunc = AddKernelArgDim;
        cnn_kernel_arg_datatype_func_t CNN_ArgDtype = CNN_ArgDataType;

	if (Ctrl) {
		if (Ctrl->HWC != -1) HWC = Ctrl->HWC;
		if (Ctrl->FloatDump != -1&&Ctrl->FloatDump) AddKArgDimFunc = AddKernelFloatArgDim;
                if (Ctrl->FloatDump != -1&&Ctrl->FloatDump) CNN_ArgDtype = CNN_ArgDataTypeF;
	}
	int KerLayout = HWC?CALL_HWC_KER:0;

	if (HWC==0) {
		if (!(MatPermOper == KOP_MATPERM_CHW2CWH || MatPermOper == KOP_MATPERM_CHW2HWC || MatPermOper == KOP_MATPERM_CHW2WHC ||
	      	      MatPermOper == KOP_MATPERM_CHW2WCH || MatPermOper == KOP_MATPERM_CHW2HCW))
			GenTilingError("CNN_3DTensorPermute Kernel: %s, CHW MatTransOper should be KOP_MATPERM_CHW2CWH, KOP_MATPERM_CHW2HWC, KOP_MATPERM_CHW2WHC, KOP_MATPERM_CHW2WCH or KOP_MATPERM_CHW2HCW", Name);
	} else {
		if (!(MatPermOper == KOP_MATPERM_HWC2CWH || MatPermOper == KOP_MATPERM_HWC2CHW || MatPermOper == KOP_MATPERM_HWC2WHC ||
	      	      MatPermOper == KOP_MATPERM_HWC2WCH || MatPermOper == KOP_MATPERM_HWC2HCW))
			GenTilingError("CNN_3DTensorPermute Kernel: %s, HWC MatTransOper should be KOP_MATPERM_HWC2CWH, KOP_MATPERM_HWC2HWC, KOP_MATPERM_HWC2WHC, KOP_MATPERM_HWC2WCH or KOP_MATPERM_HWC2HCW", Name);
	}
	char *MatPermKerName = CNN_FindMatchingKernelAttr(MatPermOper, KOP_NONE, ParFeat, KerLayout, Size, 0, 0, 0, Size, 0,0,0,0,0,0, 0,0,0,0, 0, 0, 0);
	if (MatPermKerName==0) GenTilingError("CNN_3DTensorPermute Kernel: %s, Can't find a matching basic kernel for 3D Tensor Permutation %d", Name, Size);

	LayerBandwidth += (int64_t) Feat*Width*Height*1;
	if (Log) {
		printf("CNN_MatPermute: %s %s%s, %d\n", Name, ParFeat?"Par Feat":"", HWC?", HWC":", CHW", Size);
		printf("In  => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		printf("Out => Feat: %4d, W: %4d, H: %4d\n", Feat, Width, Height);
		if (MatPermKerName) printf("%20s: %s\n", "MatPermKerName", MatPermKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(2);
	if (HWC==0) PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width, Height*Feat, Size,  0, 0, 0, "In");
	else        PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Feat, Width*Height, Size,  0, 0, 0, "In");
	switch (MatPermOper) {
		case KOP_MATPERM_HWC2HCW:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Feat*Width, Height, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_CHW2CWH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Height, Feat, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_HWC2WCH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Feat*Height, Width, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_CHW2HWC:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Feat, Height, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_HWC2CWH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Height, Feat, Size, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_CHW2WHC:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Height*Feat, Width, Size, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_HWC2CHW:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width*Height, Feat, Size, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_CHW2WCH:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Height*Feat, Width, Size, 0, OBJ_CONSTRAINTS_TILE_HOR, 0, "Out");
			break;
		case KOP_MATPERM_HWC2WHC:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Feat, Width*Height, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
		case KOP_MATPERM_CHW2HCW:
			PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width, Height*Feat, Size, 0, OBJ_CONSTRAINTS_TILE_VER, 0, "Out");
			break;
	}
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_OFF);
	Kernel_T *Kernel = UserKernel(Name,
			KernelIterSpace(1, IterTiledSpace(T0)),
	        	TILE_VER,
                	CArgs(2, TCArg(CNN_ArgDtype(Size,1,1),  "In"), TCArg(CNN_ArgDtype(Size,1,1), "Out")),
			Calls(1,
				Call(MatPermKerName, LOC_LOOP,
					Bindings(7,
						K_Arg("In", KER_ARG_TILE),	/* Input tile */
						K_Arg("Out", KER_ARG_TILE),	/* Output tile */
						Imm(Feat), 			/* Number of Channels */
						K_Arg("In", KER_ARG_TILE_W),	/* Input tile width */
						Imm(Height),			/* Input tile height */
						AT_IGNORE_ARG_BINDING,		/* StrideX */
						AT_IGNORE_ARG_BINDING		/* StrideY */
					)
				)
			),
			PKerArgs
		);
	AT_SetKernelCtrl(AT_KERNEL_NOSOLUTION_ERROR, AT_OPT_ON);
	if (Kernel==0) {
		printf("NEW SCHEME\n");
		PKerArgs[0] = KerArg("In", KerArgSpace(3,D2,D1,D0), O_IN|O_DB,  1, 1,  Size,  0, 0, 0, "In");
		switch (MatPermOper) {
			case KOP_MATPERM_CHW2CWH:
			case KOP_MATPERM_HWC2HCW:
				PKerArgs[1] = KerArg("Out", KerArgSpace(3,D2,D0,D1), O_OUT|O_DB, 1, 1,  Size,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2HWC:
			case KOP_MATPERM_HWC2WCH:
				PKerArgs[1] = KerArg("Out", KerArgSpace(3,D1,D0,D2), O_OUT|O_DB, 1, 1,  Size,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2WHC:
			case KOP_MATPERM_HWC2CWH:
				PKerArgs[1] = KerArg("Out", KerArgSpace(3,D0,D1,D2), O_OUT|O_DB, 1, 1,  Size,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2WCH:
			case KOP_MATPERM_HWC2CHW:
				PKerArgs[1] = KerArg("Out", KerArgSpace(3,D0,D2,D1), O_OUT|O_DB, 1, 1,  Size,  0, 0, 0, "Out");
				break;
			case KOP_MATPERM_CHW2HCW:
			case KOP_MATPERM_HWC2WHC:
				PKerArgs[1] = KerArg("Out", KerArgSpace(3,D1,D2,D0), O_OUT|O_DB, 1, 1,  Size,  0, 0, 0, "Out");
				break;
		}
		Kernel = UserKernel(Name,
				(HWC==0)?
				KernelIterSpace(3, IterParSpace(D2, Feat, 1), IterParSpace(D1, Height, 1), IterParSpace(D0, Width, Width)):
				KernelIterSpace(3, IterParSpace(D2, Height, 1), IterParSpace(D1, Width, Width), IterParSpace(D0, Feat, 1)),
	        		TILE_HOR,
                		CArgs(2, TCArg(CNN_ArgDtype(Size,1,1),  "In"), TCArg(CNN_ArgDtype(Size,1,1), "Out")),
				Calls(1,
					Call(MatPermKerName, LOC_LOOP,
						Bindings(7,
							K_Arg("In", KER_ARG_TILE),			/* Input tile */
							K_Arg("Out", KER_ARG_TILE),			/* Output tile */
							K_ArgPar("In", KER_ARG_PARTILE_SIZE, D2),	/* Number of Channels */
							K_Arg("In", KER_ARG_TILE_W),			/* Input tile width */
							K_Arg("In", KER_ARG_TILE_H),			/* Input tile height */
							AT_IGNORE_ARG_BINDING,				/* StrideX */
							AT_IGNORE_ARG_BINDING				/* StrideY */
						)
					)
				),
				PKerArgs
		);
	}
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);

		AddKArgDimFunc(Name, "In", 4, Feat, Height, Width, Size);
		switch (MatPermOper) {
			case KOP_MATPERM_CHW2CWH:
				AddKArgDimFunc(Name, "Out", 4, Feat, Width, Height, Size);
				break;
			case KOP_MATPERM_CHW2HWC:
				AddKArgDimFunc(Name, "Out", 4, Height, Width, Feat, Size);
				break;
			case KOP_MATPERM_CHW2WHC:
				AddKArgDimFunc(Name, "Out", 4, Width, Height, Feat, Size);
				break;
			case KOP_MATPERM_CHW2WCH:
				AddKArgDimFunc(Name, "Out", 4, Width, Feat, Height, Size);
				break;
			case KOP_MATPERM_CHW2HCW:
				AddKArgDimFunc(Name, "Out", 4, Height, Feat, Width, Size);
				break;

			case KOP_MATPERM_HWC2HCW:
				AddKArgDimFunc(Name, "Out", 4, Height, Feat, Width, Size);
				break;
			case KOP_MATPERM_HWC2WCH:
				AddKArgDimFunc(Name, "Out", 4, Width, Feat, Height, Size);
				break;
			case KOP_MATPERM_HWC2WHC:
				AddKArgDimFunc(Name, "Out", 4, Width, Height, Feat, Size);
				break;
			case KOP_MATPERM_HWC2CHW:
				AddKArgDimFunc(Name, "Out", 4, Feat, Height, Width, Size);
				break;
			case KOP_MATPERM_HWC2CWH:
				AddKArgDimFunc(Name, "Out", 4, Feat, Width, Height, Size);
				break;
		}
		AT_PrepareForTest(Name,
                                  (v4s) {Size, 0, 0, Size},
                                  (v4s) {0,0,0,0},
                                  Feat, Feat, Width, Height,
                                  0,0, 0,0, 0,0, (v4s) 0,
                                  0,0, 0,0, 0,0, (v4s) 0,
                                  MatPermOper,
                                  0, 0);
	}
	return (Kernel!=0);
}


/*********************************************************************************************************************************************************************
  Generator for RGB565 image preprocessing:

Template:
Name:		Name of the generated user kernel

Width		Image width
Height		Image height

DoOffset    If true offset pixel by -128

Signature:	Name(In, Out)

Kop: KOP_NORM_RGB16, KOP_NORM_RGB565, KOP_NORM_RGB888

 *********************************************************************************************************************************************************************/

int CNN_NormRGB(
	char *Name,
	int Width,
	int Height,
	int DoOffset,
	KernelOper_T kop
       )

{
	int Log = 1;
	char *BodyName = AppendNames(Name, "Body");
	unsigned long long int LayerOp = (Width*Height*4) + (Width*Height*(DoOffset?2:1))/4;
	unsigned long long int LayerBandwidth = 0;

	char *NormRGBKerName = (kop==KOP_NORM_RGB16?"CNN_NormRGB16_fp":(kop==KOP_NORM_RGB565?(DoOffset?"CNN_NormRGB565_offset_fps":"CNN_NormRGB565_shift_fps"):(DoOffset?"CNN_NormRGB888_offset_fps":"CNN_NormRGB888_shift_fps")));

	LayerBandwidth += 2*Width*Height*1;
	LayerBandwidth += 3*Width*Height*1;

	if (Log) {
		printf("CNN_NormRGB%d: %s\n", (kop==KOP_NORM_RGB16?16:(kop==KOP_NORM_RGB565?565:888)), Name);
		printf("In  => Feat: %d W: %4d, H: %4d\n", (kop==KOP_NORM_RGB565?1:3), Width, Height);
		printf("Out => Feat: 3, W: %4d, H: %4d\n", Width, Height);
		if (NormRGBKerName) printf("%20s: %s\n", "KerName", NormRGBKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(4);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width*(kop==KOP_NORM_RGB565?1:3), Height, (kop==KOP_NORM_RGB565?2:1),  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out0", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==KOP_NORM_RGB16?2:1),  0, 0, 0, "Out0");
	PKerArgs[2] = KerArg("Out1", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==KOP_NORM_RGB16?2:1),  0, 0, 0, "Out1");
	PKerArgs[3] = KerArg("Out2", KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==KOP_NORM_RGB16?2:1),  0, 0, 0, "Out2");
	OpenKernelGroup(Name);
	UserKernel(BodyName,
			KernelIterSpace(1, IterTiledSpace(T0)),
			TILE_HOR,
			CArgs(4, TCArg(CNN_ArgDataTypeUns((kop==KOP_NORM_RGB565?2:1),1,1),  "In"),
					 TCArg(CNN_ArgDataType((kop==KOP_NORM_RGB16?2:1),1,1), "Out0"),
					 TCArg(CNN_ArgDataType((kop==KOP_NORM_RGB16?2:1),1,1), "Out1"),
					 TCArg(CNN_ArgDataType((kop==KOP_NORM_RGB16?2:1),1,1), "Out2")
				 ),
			Calls(1,
				Call(NormRGBKerName, LOC_LOOP,
					Bindings(6,
						K_Arg("In", KER_ARG_TILE),	/* Input tile */
						K_Arg("Out0", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out1", KER_ARG_TILE),	/* Output tile */
						K_Arg("Out2", KER_ARG_TILE),	/* Output tile */
						K_Arg((kop==KOP_NORM_RGB565?"In":"Out0"), KER_ARG_TILE_W),	/* tile width */
						K_Arg((kop==KOP_NORM_RGB565?"In":"Out0"), KER_ARG_TILE_H)	/* tile height */
						)
				    )
			     ),
			PKerArgs
		  );
	AddKernelInfos(BodyName, AT_KERINFO_OPER, LayerOp, 0);
	AddKernelInfos(BodyName, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
	AddKernelArgDim(BodyName, "In",   4, (kop==KOP_NORM_RGB565?1:3), Height, Width, (kop==KOP_NORM_RGB565?2:1));
	AddKernelArgDim(BodyName, "Out0", 4, 1, Height, Width, (kop==KOP_NORM_RGB16?2:1));
	AddKernelArgDim(BodyName, "Out1", 4, 1, Height, Width, (kop==KOP_NORM_RGB16?2:1));
	AddKernelArgDim(BodyName, "Out2", 4, 1, Height, Width, (kop==KOP_NORM_RGB16?2:1));
	CloseKernelGroup();
	CKernel_Arg_T **KCArgs = AllocateCArgs(2);
	int Ca=0;
	KCArgs[Ca++] = TCArg(CNN_ArgDataTypeUns((kop==KOP_NORM_RGB565?2:1),1,1),   "In");
	KCArgs[Ca++] = TCArg(CNN_ArgDataType(   (kop==KOP_NORM_RGB16?2:1), 1,1),  "Out");
	Object_T **KArgs = AllocateKerArgs(7);
	int Ka=0;
	KArgs[Ka++] = KerGroupArg("In",     O_IN,  Width*Height*(kop==KOP_NORM_RGB565?1:3), (kop==KOP_NORM_RGB565?2:1), "In");
	KArgs[Ka++] = KerGroupArg("Out",    O_OUT, Width*Height*3,                            (kop==KOP_NORM_RGB16?2:1), "Out");
	UserKernelGroup(Name,
			KCArgs,
			Calls(1,
				UserKernelCall(BodyName, LOC_GROUP,
					Bindings(4,
						C_Arg("In"),
						C_ArgPlusImmOffset("Out", 0),
						C_ArgPlusImmOffset("Out", Height * Width),
						C_ArgPlusImmOffset("Out", Height * Width * 2)
						)
					)
			     )
		       );
	return 0;

}

int CNN_NormBW(
	char *Name,
	int Width,
	int Height,
	int DoOffset,
	KernelOper_T kop
       )
{
	int Log = 1;
	unsigned long long int LayerOp = Width*Height;
	unsigned long long int LayerBandwidth = 0;
	char *NormBWKerName = (kop==KOP_NORM_BW16?"CNN_NormBW_fp":(DoOffset?"CNN_NormBW_offset_fps":"CNN_NormBW_shift_fps"));

	LayerBandwidth += Width*Height*1;
	LayerBandwidth += Width*Height*1;

	if (Log) {
		printf("CNN_NormBW: %s\n", Name);
		printf("In  => Feat: 1 W: %4d, H: %4d\n", Width, Height);
		printf("Out => Feat: 1, W: %4d, H: %4d\n", Width, Height);
		if (NormBWKerName) printf("%20s: %s\n", "KerName", NormBWKerName);
		printf("Nb Oper : %lld\n", LayerOp);
	}

	Object_T **PKerArgs = AllocateKerArgs(2);
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,T0), O_IN|O_DB,  Width, Height, 1,  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out",  KerArgSpace(1,T0), O_OUT|O_DB, Width, Height, (kop==KOP_NORM_BW16?2:1),  0, 0, 0, "Out");
        UserKernel(Name,
                        KernelIterSpace(1, IterTiledSpace(T0)),
                        TILE_HOR,
                        CArgs(2, TCArg(CNN_ArgDataTypeUns(1,1,1),  "In"), TCArg(CNN_ArgDataType((kop==KOP_NORM_BW16?2:1),1,1), "Out")),
                        Calls(1,
                                Call(NormBWKerName, LOC_LOOP,
                                        Bindings(4,
                                                K_Arg("In", KER_ARG_TILE),      /* Input tile */
                                                K_Arg("Out", KER_ARG_TILE),    /* Output tile */
                                                K_Arg("In", KER_ARG_TILE_W),    /* Input tile width */
                                                K_Arg("In", KER_ARG_TILE_H)     /* Input tile width */
                                                )
                                    )
                             ),
                        PKerArgs
                  );
        AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
        AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
        AddKernelArgDim(Name, "In", 4, 1, Height, Width, 1);
        AddKernelArgDim(Name, "Out", 4, 1, Height, Width, 1);
	return 0;
}

int CNN_Norm(
	char *Name,
	int Width,
	int Height,
	int DoOffset,
	KernelOper_T kop
       )
{
	if (kop == KOP_NORM_BW || kop == KOP_NORM_BW16) {
		return CNN_NormBW(Name, Width, Height, DoOffset, kop);
	} else {
		return CNN_NormRGB(Name, Width, Height, DoOffset, kop);
	}
}

int CNN_Convert(
	char *Name,
	int In_DataSize,
	int Out_DataSize,
	int Sz,
	KernelOper_T kop
)
{
	int Log = 1;
	unsigned long long int LayerOp = 1 * Sz;
	unsigned long long int LayerBandwidth = 0;
	char *ConvertKerName = CNN_FindMatchingKernel(
                kop, KOP_NONE, 1,
                In_DataSize, 0, 0, 0, Out_DataSize, 0,0,0,0,1,1, 0,0,0,0, 0, 0, 0);
	if (ConvertKerName==0) GenTilingError("CNN_Convert Kernel: %s, Can't find a matching basic kernel for tensor conversion", Name);

        int In_Unsigned = In_DataSize < 0, Out_Unsigned = Out_DataSize < 0;
        In_DataSize = Abs(In_DataSize); Out_DataSize = Abs(Out_DataSize);
        int In_Float = kop == KOP_CONVERT_FL_FP;
        int Out_Float = kop == KOP_CONVERT_FP_FL;
#ifdef __EMUL__
        if (In_Float) In_DataSize = 4;
        if (Out_Float) Out_DataSize = 4;
#endif
	LayerBandwidth += Sz*(In_DataSize + Out_DataSize);

	if (Log) {
		printf("CNN_Convert: %s\n", Name);
		printf("Kernel: %s\n", ConvertKerName);
		printf("In  => Feat: 1 N: %4d\n", Sz);
		printf("Out => Feat: 1, N: %4d\n", Sz);
		printf("Nb Oper : %lld\n", LayerOp);
	}

        char *In_DataType = (In_Float?CNN_ArgDataTypeF(In_DataSize, 1, 1):(In_Unsigned?CNN_ArgDataTypeUns(In_DataSize, 1, 1):CNN_ArgDataType(In_DataSize, 1, 1)));
        char *Out_DataType = (Out_Float?CNN_ArgDataTypeF(Out_DataSize, 1, 1):(Out_Unsigned?CNN_ArgDataTypeUns(Out_DataSize, 1, 1):CNN_ArgDataType(Out_DataSize, 1, 1)));

	Object_T **PKerArgs = AllocateKerArgs((kop==KOP_CONVERT_FP_FP?2:3));
	PKerArgs[0] = KerArg("In",   KerArgSpace(1,D0), O_IN|O_DB,            1, 1, In_DataSize,  0, 0, 0, "In");
	PKerArgs[1] = KerArg("Out",  KerArgSpace(1,D0), O_OUT|O_DB,           1, 1, Out_DataSize,  0, 0, 0, "Out");
        if (kop!=KOP_CONVERT_FP_FP)
        	PKerArgs[2] = KerArg("Infos",KerArgSpace(1,T0), O_IN|O_BUFF|O_NTILED, 1, 1, 8,  0, 0, 0, "Infos");
	Kernel_T *Kernel = UserKernel(Name,
				KernelIterSpace(2, IterTiledSpace(T0), IterParSpace(D0, Sz, 8)),
				TILE_HOR,
				CArgs(3,
					TCArg(In_DataType, "In"),
					TCArg(Out_DataType, "Out"),
					(kop==KOP_CONVERT_FP_FP?AT_NO_C_ARG:TCArg(CNN_ArgDataType(1,1,1), "Infos"))
				),
				Calls(1,
					Call(ConvertKerName, LOC_D0,
						Bindings(5,
							K_Arg("In", KER_ARG_TILE),      /* Input tile */
							K_Arg("Out", KER_ARG_TILE),    	/* Output tile */
							K_ArgPar("In", KER_ARG_PARTILE_SIZE, D0),    /* Input tile width */
							Imm(1),    /* Input tile height */
				                        (kop==KOP_CONVERT_FP_FP?AT_IGNORE_ARG_BINDING:K_Arg("Infos", KER_ARG_TILE))
						)
					)
				),
				PKerArgs
			);
	if (Kernel) {
		AddKernelInfos(Name, AT_KERINFO_OPER, LayerOp, 0);
		AddKernelInfos(Name, AT_KERINFO_BANDWIDTH, LayerBandwidth, 0);
		In_Float? AddKernelFloatArgDim(Name, "In",    3, 1, Sz, In_DataSize) :AddKernelArgDim(Name, "In",    3, 1, Sz, In_DataSize);
                Out_Float?AddKernelFloatArgDim(Name, "Out",   3, 1, Sz, Out_DataSize):AddKernelArgDim(Name, "Out",   3, 1, Sz, Out_DataSize);
                if (kop!=KOP_CONVERT_FP_FP)
	        	AddKernelArgDim(Name, "Infos", 2, 8, 1);
	}
	return (Kernel!=0);
}

