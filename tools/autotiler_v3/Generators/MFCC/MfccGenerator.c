/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "AutoTilerLibTypes.h"
#include "MfccGenerator.h"

#define D0      KER_ITER_D0
#define D1      KER_ITER_D1
#define D2      KER_ITER_D2
#define D3      KER_ITER_D3
#define T0      KER_ITER_TILE0
#define T1      KER_ITER_TILE1
#define T2      KER_ITER_TILE2

void LoadMFCCLibrary()

{

       LibKernel("Radix2FFT_DIF_Par", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );
       LibKernel("Radix2FFT_DIF_Inv_Par", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_INT_Scal_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_scal_Arg_T",NULL
		 );
       
       LibKernel("RadixMixedFFT_DIF_Par", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );
       
        LibKernel("RadixMixedFFT_DIF_Inv_Par", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_Par_Fix16", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_Par_Fix32", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("int * __restrict__"   , "Data"),
		       TCArg("int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_Par_Fix16_Fast", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_Par_f16", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("float16 * __restrict__"   , "Data"),
		       TCArg("float16 * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix2FFT_DIF_Par_f32", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("float * __restrict__"   , "Data"),
		       TCArg("float * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix4FFT_DIF_Par_Fix16", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix4FFT_DIF_Par_Fix32", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("int * __restrict__"   , "Data"),
		       TCArg("int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix4FFT_DIF_Par_f16", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("float16 * __restrict__"   , "Data"),
		       TCArg("float16 * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );

       LibKernel("Radix4FFT_DIF_Par_f32", CALL_PARALLEL,
		 CArgs(5,
		       TCArg("float * __restrict__"   , "Data"),
		       TCArg("float * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft"),
		       TCArg("unsigned char", "Inverse")
		       ),
		 "FFT_Arg_T",NULL
		 );
	
	LibKernel("SwapSamples_Par", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "FFT_SwapSamples_T",NULL
		 );

	LibKernel("SwapSamples_Par_Fix32", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "FFT_SwapSamples_T",NULL
		 );

	LibKernel("SwapSamples_Par_f16", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("v2h * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "FFT_SwapSamples_T",NULL
		 );

	LibKernel("SwapSamples_Par_f32", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("float * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "FFT_SwapSamples_T",NULL
		 );

	LibKernel("SwapSamples_scal", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni"),
		       TCArg("signed char * __restrict__"   , "shift_fft")
		       ),
		 "SwapSamples_scal_Arg_T",NULL
		 );

        LibKernel("MFCC_Step_args", CALL_PARALLEL,
		  CArgs(8,
			TCArg("short int * __restrict__"   , "Insignal"),
			TCArg("short int * __restrict__"   , "FEAT_LIST"),
			TCArg("short int * __restrict__"   , "out_fft"),
			TCArg("int16_t * __restrict__", "Twiddles_fft"),
			TCArg("int16_t * __restrict__", "SwapTable_fft"),
			TCArg("int16_t * __restrict__", "WinTable"),
			TCArg("fbank_type_t * __restrict__", "filterbank"),
			TCArg("int16_t * __restrict__", "MFCC_Coeffs")
			),
		  "MFCC_Args_T",NULL
	);

        LibKernel("MFCC_WindowedFrame", CALL_PARALLEL,
		  CArgs(5,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("short int * __restrict__"   , "OutFrame"),
			TCArg("short int * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WF_T",NULL
	);

        LibKernel("PreempWindowing", CALL_PARALLEL,
		  CArgs(8,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("short int * __restrict__"   , "OutFrame"),
			TCArg("short int * __restrict__"   , "Window"),
			TCArg("short int *", "Shift"),
			TCArg("short int", "Prev"),
			TCArg("short int", "PreempFactor"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "PreempWin_Args_T",NULL
	);


        LibKernel("MFCC_WindowedFrame_f16", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2h * __restrict__"   , "Frame"),
			TCArg("v2h * __restrict__"   , "OutFrame"),
			TCArg("v2h * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WF_T",NULL
	);

        LibKernel("MFCC_WindowedFrame_f32", CALL_PARALLEL,
		  CArgs(5,
			TCArg("float * __restrict__"   , "Frame"),
			TCArg("float * __restrict__"   , "OutFrame"),
			TCArg("float * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WF_T",NULL
	);

        LibKernel("MFCC_WindowedFrame_int", CALL_PARALLEL,
		  CArgs(5,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("int       * __restrict__"   , "OutFrame"),
			TCArg("short int * __restrict__"   , "Window"),
			TCArg("int16_t", "FrameSize"),
			TCArg("int16_t", "FFT_Dim")
			),
		  "MFCC_WFINT_T",NULL
		  );

        LibKernel("MFCC_PreEmphasis", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__"   , "Frame"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("short int", "Prev"),
			TCArg("short int", "PreempFactor"),
			TCArg("short int *", "Shift"),
			TCArg("short int", "FrameSize")
			),
		  "MFCC_PreEmphasis_T",NULL
		  );

        LibKernel("MFCC_PreEmphasis_f16", CALL_PARALLEL,
		  CArgs(6,
			TCArg("f16 * __restrict__"   , "Frame"),
			TCArg("f16 * __restrict__"   , "Out"),
			TCArg("f16", "Prev"),
			TCArg("f16", "PreempFactor"),
			TCArg("short int *", "Shift"),
			TCArg("short int", "FrameSize")
			),
		  "MFCC_PreEmphasis_T",NULL
		  );

        LibKernel("MFCC_PreEmphasis_f32", CALL_PARALLEL,
		  CArgs(6,
			TCArg("float * __restrict__"   , "Frame"),
			TCArg("float * __restrict__"   , "Out"),
			TCArg("float", "Prev"),
			TCArg("float", "PreempFactor"),
			TCArg("short int *", "Shift"),
			TCArg("short int", "FrameSize")
			),
		  "MFCC_PreEmphasis_T",NULL
		  );   

        LibKernel("MFCC_Power", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_PowerV2S", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Power_f16", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2h * __restrict__"   , "FrameIn"),
			TCArg("f16 * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Power_f32", CALL_PARALLEL,
		  CArgs(5,
			TCArg("float * __restrict__"   , "FrameIn"),
			TCArg("float * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Abs", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Abs_BFF", CALL_PARALLEL,
		  CArgs(5,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Abs_f16", CALL_PARALLEL,
		  CArgs(5,
			TCArg("f16 * __restrict__"   , "FrameIn"),
			TCArg("f16 * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );

        LibKernel("MFCC_Abs_f32", CALL_PARALLEL,
		  CArgs(5,
			TCArg("float * __restrict__"   , "FrameIn"),
			TCArg("float * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("short int *"   , "Shift"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_BFF_T",NULL
		  );
        LibKernel("MFCC_ComputeLog", CALL_PARALLEL,
		  CArgs(6,
			TCArg("unsigned int * __restrict__"   , "FrameIn"),
			TCArg("unsigned int"   , "FrameSize"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("short int"   , "MFCC_Coeff_dyn"),
			TCArg("signed char * __restrict__"   , "shift_BF")
			),
		  "MFCC_Log_T",NULL
		  );

        LibKernel("MFCC_ComputeLog_f16", CALL_PARALLEL,
		  CArgs(6,
			TCArg("f16 * __restrict__"   , "FrameIn"),
			TCArg("unsigned int"   , "FrameSize"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("short int"   , "MFCC_Coeff_dyn"),
			TCArg("signed char * __restrict__"   , "shift_BF")
			),
		  "MFCC_Log_T",NULL
		  );

        LibKernel("MFCC_ComputeLog_f32", CALL_PARALLEL,
		  CArgs(6,
			TCArg("float * __restrict__"   , "FrameIn"),
			TCArg("unsigned int"   , "FrameSize"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("short int"   , "MFCC_Coeff_dyn"),
			TCArg("signed char * __restrict__"   , "shift_BF")
			),
		  "MFCC_Log_T",NULL
		  );

        LibKernel("MFCC_ComputeDB", CALL_PARALLEL,
		  CArgs(6,
			TCArg("unsigned int * __restrict__"   , "FrameIn"),
			TCArg("unsigned int"   , "FrameSize"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("short int"   , "MFCC_Coeff_dyn"),
			TCArg("signed char * __restrict__"   , "shift_BF")
			),
		  "MFCC_Log_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC", CALL_PARALLEL,
		  CArgs(8,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt"),
			TCArg("short int","MFCC_Coeff_dyn")
			),
		  "MFCC_MF_New_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC_BFF", CALL_PARALLEL,
		  CArgs(8,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt"),
			TCArg("short int","MFCC_Coeff_dyn")
			),
		  "MFCC_MF_New_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC_f16", CALL_PARALLEL,
		  CArgs(8,
			TCArg("f16 * __restrict__"   , "FramePower"),
			TCArg("f16 * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("f16 *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt"),
			TCArg("short int","MFCC_Coeff_dyn")
			),
		  "MFCC_MF_New_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC_f32", CALL_PARALLEL,
		  CArgs(8,
			TCArg("float * __restrict__"   , "FramePower"),
			TCArg("float * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("float *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt"),
			TCArg("short int","MFCC_Coeff_dyn")
			),
		  "MFCC_MF_New_T",NULL
		  );
	
        LibKernel("MFCC_ComputeDCT", CALL_PARALLEL,
		  CArgs(8,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "Twiddles"),	
			TCArg("void * __restrict__ ", "Twidend"),	
			TCArg("short int * __restrict__", "SwapLuts"),	
			TCArg("short int * __restrict__", "Lift_coeff"),	
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_Arg_T",NULL
		  );

        LibKernel("MFCC_ComputeDCT_II", CALL_PARALLEL,
		  CArgs(5,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "DCTCoeff"),
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_II_Arg_T",NULL
		  );

        LibKernel("MFCC_ComputeDCT_II_f16", CALL_PARALLEL,
		  CArgs(5,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "DCTCoeff"),
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_II_Arg_T",NULL
		  );

        LibKernel("MFCC_ComputeDCT_II_f32", CALL_PARALLEL,
		  CArgs(5,
			TCArg("void * __restrict__ ", "Data"),	
			TCArg("void * __restrict__ ", "DCTCoeff"),
			TCArg("short int * __restrict__", "FeatList"),
			TCArg("short int", "n_dct"),
			TCArg("short int", "numcep")
			),
		  "DCT_II_Arg_T",NULL
		  );

        LibKernel("MFCC_Lifter", CALL_PARALLEL,
        	  CArgs(3,
			TCArg("short int * __restrict__ ", "FeatList"),	
			TCArg("short int * __restrict__ ", "LiftCoeff"),
			TCArg("int", "FrameSize")
        	  	), 
        	  "Lifter_Arg_T",NULL
        	  );
		
        LibKernel("get_max", CALL_PARALLEL,
		  CArgs(4,
			TCArg("int * __restrict__"   , "Frame"),
			TCArg("int * __restrict__"   , "maxin"),
			TCArg("int", "FrameSize"),
			TCArg("int", "shift")
			),
		  "get_max_T",NULL
		  );

        LibKernel("Coeff_mult", CALL_PARALLEL,
		  CArgs(4,
			TCArg("unsigned int * __restrict__"   , "In1"),
			TCArg("short int * __restrict__"   , "In2"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("int", "FrameSize")
			),
		  "COEFF_mult_args_T",NULL
		  );
        LibKernel("IMEL", CALL_PARALLEL,
		  CArgs(6,
			TCArg("short int * __restrict__"   , "In"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("short int * __restrict__"   , "IMFCC_FilterBank"),
			TCArg("short int * __restrict__"   , "IMFCC_Coeffs"),
			TCArg("int", "nbin"),
			TCArg("int", "nbout")
			),
		  "IMEL_args_T",NULL
	);
        LibKernel("norm_clip_16", CALL_PARALLEL,
		  CArgs(4,
		  	TCArg("int * __restrict__"   , "In"),
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("unsigned short int", "Norm"),
			TCArg("int", "N")
			),
		  "Norm_Clip_args_T",NULL
	);
        LibKernel("norm_clip_32_melspect", CALL_PARALLEL,
		  CArgs(7,
		  	TCArg("unsigned int * __restrict__", "In"),
			TCArg("unsigned int * __restrict__", "Out"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("short int"   , "MFCC_Coeff_dyn"),
			TCArg("signed char * __restrict__"   , "shift_BF"),
			TCArg("unsigned int"   , "N")
			),
		  "MFCC_Clip_32_T",NULL
		  );
        LibKernel("norm_clip_32_spect", CALL_PARALLEL,
		  CArgs(6,
		  	TCArg("unsigned int * __restrict__", "In"),
			TCArg("unsigned int * __restrict__", "Out"),
			TCArg("short int *"   , "Shift"),
			TCArg("short int"   , "fft_bits"),
			TCArg("signed char * __restrict__"   , "shift_BF"),
			TCArg("unsigned int"   , "N")
			),
		  "MFCC_Clip_32_T",NULL
		  );
        LibKernel("Conjugate_Fix16_Par",   CALL_PARALLEL, CArgs(2, TCArg("v2s   * __restrict__", "Data"), TCArg("int", "Ni")), "FFT_SwapSamples_T", NULL);
        LibKernel("Conjugate_Fix32_Par",   CALL_PARALLEL, CArgs(2, TCArg("int   * __restrict__", "Data"), TCArg("int", "Ni")), "FFT_SwapSamples_T", NULL);
        LibKernel("Conjugate_Float16_Par", CALL_PARALLEL, CArgs(2, TCArg("v2h   * __restrict__", "Data"), TCArg("int", "Ni")), "FFT_SwapSamples_T", NULL);
        LibKernel("Conjugate_Float32_Par", CALL_PARALLEL, CArgs(2, TCArg("float * __restrict__", "Data"), TCArg("int", "Ni")), "FFT_SwapSamples_T", NULL);
}

void PieceWiseGenerator(char *Name, CNN_GenControl_T *Ctrl, char *FunName, int Dim, int DataType, int Inplace)
{
	char * InPointer;
	switch (DataType){
		case FIX16:
			InPointer   = "int16_t * __restrict__"; break;
		case FIX32:
			InPointer   = "int * __restrict__"; break;
		case FLOAT16:
			InPointer   = "f16 * __restrict__"; break;
		case FLOAT32:
			InPointer   = "float * __restrict__"; break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}
	if (Inplace) LibKernel(FunName, CALL_PARALLEL, CArgs(2, TCArg(InPointer, "In"), TCArg("short int", "N")), "Arg_PieceWise_T", NULL);
	else         LibKernel(FunName, CALL_PARALLEL, CArgs(3, TCArg(InPointer, "In"), TCArg("short int", "N"), TCArg(InPointer, "Out")), "Arg_PieceWise_T", NULL);

	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(1, IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(2,
                	TCArg(InPointer, "In"),
			(Inplace)?TCArg(InPointer, "Out"):AT_NO_C_ARG
                     ),
                Calls(1,
			Call(FunName,LOC_LOOP,
			      Bindings(Inplace?2:3,
			       K_Arg("In",       KER_ARG_TILE),
			       Imm(Dim),
			       Inplace?K_Arg("Out", KER_ARG_TILE):AT_IGNORE_ARG_BINDING)
			     )
		),
		KerArgs(2,
			KerArg("In",  KerArgSpace(1,T0), OBJ_IN_OUT, 1, Dim, DataType==FLOAT32||DataType==FIX32?4:2,  0, 0, 0, "In"),
		Inplace?KerArg("Out", KerArgSpace(1,T0), OBJ_IN_OUT, 1, Dim, DataType==FLOAT32||DataType==FIX32?4:2,  0, 0, 0, "Out"):AT_NO_KER_ARG
		)
	);
}

void FFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int Dim,
	int ForceRadix2,
	int DataType,
	int IntScal
	)
{
	if (__builtin_popcount(Dim) != 1) GenTilingError("FFT: %s, Incorrect Dim: %d, it has to be a a power of 2", Name, Dim);
	int UseRadix4 = (!ForceRadix2 && ((__builtin_ctz(Dim)%2)==0) && (Dim>64));
	char *FFTKernel=0, *SwapKernel=0, *InPointer=0, *TwidPointer=0;

	switch (DataType){
		case FIX16:
			if (UseRadix4) FFTKernel = "Radix4FFT_DIF_Par_Fix16";
			else           FFTKernel = "Radix2FFT_DIF_Par_Fix16";
			SwapKernel  = "SwapSamples_Par";
			InPointer   = "int16_t * __restrict__";
			TwidPointer = "int16_t * __restrict__";
			break;
		case FIX32:
			FFTKernel   = IntScal?"Radix2FFT_DIF_INT_Scal_Par":"Radix2FFT_DIF_Par_Fix32";
			SwapKernel  = IntScal?"SwapSamples_scal":"SwapSamples_Par_Fix32";
			InPointer   = "int * __restrict__";
			TwidPointer = "int16_t * __restrict__";
			break;
		case FLOAT16:
			if (UseRadix4) FFTKernel = "Radix4FFT_DIF_Par_f16";
			else  	       FFTKernel = "Radix2FFT_DIF_Par_f16";
			SwapKernel  = "SwapSamples_Par_f16";
			InPointer   = "f16 * __restrict__";
			TwidPointer = "f16 * __restrict__";
			break;
		case FLOAT32:
			if (UseRadix4) FFTKernel = "Radix4FFT_DIF_Par_f32";
			else  	       FFTKernel = "Radix2FFT_DIF_Par_f32";
			SwapKernel  = "SwapSamples_Par_f32";
			InPointer   = "float * __restrict__";
			TwidPointer = "float * __restrict__";
			break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}
	int ncalls = 10;
	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(1, IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(4,
                	TCArg(InPointer, "In"),
			TCArg(TwidPointer, "Twiddles_fft"),
			TCArg("short int *",   "SwapTable_fft"),
			(DataType==FIX32 && IntScal)?TCArg("char *", "FFT_Shift_Buff"):AT_NO_C_ARG
                     ),
                Calls(2,
			Call(FFTKernel,LOC_LOOP,
			      Bindings((DataType==FIX32 && IntScal)?4:5,
			       K_Arg("In",       KER_ARG_TILE),
			       K_Arg("Twiddles_fft", KER_ARG_TILE ),
			       (DataType==FIX32 && IntScal)?K_Arg("shift_fft", KER_ARG_TILE):AT_IGNORE_ARG_BINDING, //shift_fft buffer
			       Imm(Dim),
			       Imm(0))
			     ),
			Call(SwapKernel,LOC_LOOP,
			      Bindings((DataType==FIX32 && IntScal)?4:3,
			       K_Arg("In",       KER_ARG_TILE),
			       K_Arg("SwapTable_fft", KER_ARG_TILE),
			       Imm(Dim),
			       (DataType==FIX32 && IntScal)?K_Arg("shift_fft", KER_ARG_TILE):AT_IGNORE_ARG_BINDING)
			     )
		),
		KerArgs(4,
			KerArg("In",              KerArgSpace(1,T0), OBJ_IN_OUT,	  1, 2*Dim, 	  	    DataType==FLOAT32||DataType==FIX32?4:2,    0, 0, 0, "In"),
			(DataType==FIX32 && IntScal)?
			KerArg("shift_fft",       KerArgSpace(1,T0), OBJ_IN_OUT,	  1, Dim,			  		1,    0, 0, 0, "FFT_Shift_Buff"):AT_NO_KER_ARG,
			UseRadix4?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Dim/4),   DataType==FLOAT32?4:2,    0, 0, 0, "Twiddles_fft"):
				  KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(Dim),       DataType==FLOAT32?4:2,    0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Dim,              	                2,    0, 0, 0, "SwapTable_fft")
		)
	);
}


void MFCC_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	int NMFCCCoeff,
	int Ndct,
	float PreempFactor,
	int LifterCoeff,
	int use_radix_4_fft,
	int use_power,
	int DataType,
	int MfccBankCnt,
	int OutMelspectrogram,
	int OutFFT
	)
{
	int UseDB;
	if (Ctrl) UseDB = Ctrl->MFCCDbInsteadOfLog;
	else 	  UseDB = 0;
	int MFCC_Coeff_Dyn = 15;
	int fft_bits = 31 - __builtin_clz(Nfft);
	printf("MFCC_COEF_DYN = %d\nFFT_BITS = %d\nUSE_DB = %d\nDATA_TYPE = %d\n", MFCC_Coeff_Dyn, fft_bits, UseDB, DataType);

	char *PreEmpKernel=0, *WinKernel=0, *FFTKernel=0, *SwapKernel=0, *MfccKernel=0, *SpectKernel=0, *LogKernel=0, *DCTKernel=0, *UserKernType=0, *UserKernPointer=0;      

	switch (DataType){
		case FIX16:
			PreEmpKernel = "MFCC_PreEmphasis";
			WinKernel = "MFCC_WindowedFrame";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_Fix16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par";
			SwapKernel = "SwapSamples_Par";
			SpectKernel = use_power?"MFCC_PowerV2S":"MFCC_Abs";
			MfccKernel = "MFCC_ComputeMFCC";
			LogKernel = UseDB?"MFCC_ComputeDB":"MFCC_ComputeLog";
			DCTKernel = "MFCC_ComputeDCT_II";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FIX32:
			PreEmpKernel = "MFCC_PreEmphasis";
			WinKernel = "MFCC_WindowedFrame_int";
			if (use_radix_4_fft) return GenTilingError("Radix4 FFT Not implemented for FIX32");
			else  		     FFTKernel = "Radix2FFT_DIF_INT_Scal_Par";
			SwapKernel = "SwapSamples_scal";
			SpectKernel = use_power?"MFCC_Power":"MFCC_Abs_BFF";
			MfccKernel = "MFCC_ComputeMFCC_BFF";
			LogKernel = UseDB?"MFCC_ComputeDB":"MFCC_ComputeLog";
			DCTKernel = "MFCC_ComputeDCT_II";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FLOAT16:
			PreEmpKernel = "MFCC_PreEmphasis_f16";
			WinKernel = "MFCC_WindowedFrame_f16";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f16";
			SwapKernel = "SwapSamples_Par_f16";
			SpectKernel = use_power?"MFCC_Power_f16":"MFCC_Abs_f16";
			MfccKernel = "MFCC_ComputeMFCC_f16";
			LogKernel = "MFCC_ComputeLog_f16";
			DCTKernel = "MFCC_ComputeDCT_II_f16";
			UserKernType = "f16";
			UserKernPointer = "f16 * __restrict__";
			break;
		case FLOAT32:
			PreEmpKernel = "MFCC_PreEmphasis_f32";
			WinKernel = "MFCC_WindowedFrame_f32";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f32";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f32";
			SwapKernel = "SwapSamples_Par_f32";
			SpectKernel = use_power?"MFCC_Power_f32":"MFCC_Abs_f32";
			MfccKernel = "MFCC_ComputeMFCC_f32";
			LogKernel = "MFCC_ComputeLog_f32";
			DCTKernel = "MFCC_ComputeDCT_II_f32";
			UserKernType = "float";
			UserKernPointer = "float * __restrict__";
			break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}

	int ncalls = 10;
	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(2, IterFixedSpace(D0, NFrames), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(14,
                	TCArg(UserKernPointer,      "In"),
			(OutMelspectrogram && DataType != FLOAT16 && DataType != FLOAT32)?TCArg("int * __restrict__", "Out"):TCArg(UserKernPointer, "Out"),
			TCArg(UserKernType,    "Prev"),
			TCArg(UserKernPointer, "Twiddles_fft"),
			TCArg("short int *", "SwapTable_fft"),
			TCArg(UserKernPointer, "WinTable"),
			TCArg("fbank_type_t *", "MFCC_FilterBank"),
			TCArg(UserKernPointer, "MFCC_Coeffs"),
			TCArg("int",    "Norm"),
			Ndct?TCArg(UserKernPointer, "DCT_Coeff"):AT_NO_C_ARG,
			LifterCoeff?TCArg(UserKernPointer, "lift_coeff"):AT_NO_C_ARG,
			OutFFT?TCArg(UserKernPointer, "FFT_Out"):AT_NO_C_ARG,
			OutFFT&&(DataType==FIX32)?TCArg("signed char *", "FFT_Shift_Buff"):AT_NO_C_ARG,
			OutFFT&&(DataType==FIX16||DataType==FIX32)?TCArg("short int *", "PreempShift"):AT_NO_C_ARG
                ),
                Calls(10,
			Call(PreEmpKernel,LOC_LOOP,
			     Bindings(6,
			       K_Arg("In"   , KER_ARG_TILE),
			       K_Arg("InOut1"   , KER_ARG_TILE),
			       C_Arg("Prev"),
			       (DataType==FIX16 || DataType==FIX32)?Imm(FP2FIX(PreempFactor, 15)):Imm(PreempFactor),
			       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(FrameSize))
			     ),
			Call(WinKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("InOut1"  , KER_ARG_TILE),
			       K_Arg("Out_fft" , KER_ARG_TILE),
			       K_Arg("WinTable" , KER_ARG_TILE ),
			       Imm(FrameSize),
			       Imm(Nfft))
			     ),
			Call(FFTKernel,LOC_LOOP,
			      Bindings(DataType==FIX32?4:5,
			       K_Arg("Out_fft",       KER_ARG_TILE),
			       K_Arg("Twiddles_fft", KER_ARG_TILE ),
			       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
			       Imm(Nfft),
			       Imm(0))
			     ),
			Call(SwapKernel,LOC_LOOP,
			      Bindings(DataType==FIX32?4:3,
			       K_Arg("Out_fft",       KER_ARG_TILE),
			       K_Arg("SwapTable_fft", KER_ARG_TILE),
			       Imm(Nfft),
			       K_Arg("shift_fft", KER_ARG_TILE)) //shift_fft buffer
			     ),
			Call(SpectKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("Out_fft"   , KER_ARG_TILE),
			       K_Arg("InOut1"   , KER_ARG_TILE),
			       K_Arg("shift_fft"   , KER_ARG_TILE),
		       	       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(Nfft))
			     ),
			Call(MfccKernel, LOC_LOOP,
			     Bindings(8,
			       K_Arg("InOut1"  	      , KER_ARG_TILE),
			       (Ndct==0 && (DataType==FLOAT16 || DataType==FLOAT32))?
				       K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2" , KER_ARG_TILE),
			       K_Arg("MFCC_FilterBank", KER_ARG_TILE),
			       K_Arg("MFCC_Coeffs"    , KER_ARG_TILE),
			       K_Arg("shift_fft"      , KER_ARG_TILE),
			       K_Arg("shift_bf"       , KER_ARG_TILE),
			       Imm(MfccBankCnt),
			       Imm(MFCC_Coeff_Dyn))
			     ),
			(OutMelspectrogram==0)?
			Call(LogKernel,LOC_LOOP,
			     Bindings(6,
			 	(Ndct==0 && (DataType==FLOAT16 || DataType==FLOAT32))?
			 		K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2", KER_ARG_TILE),
				Imm(MfccBankCnt),
				K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
				Imm(fft_bits), // fft_bits
				Imm(MFCC_Coeff_Dyn),
			       	K_Arg("shift_bf", KER_ARG_TILE)) //shift_bf buffer in case of DataType==FIX32
			      ):AT_NO_CALL,
			(DataType==FIX32 || DataType==FIX16)?((OutMelspectrogram==0)?
			Call("norm_clip_16",LOC_LOOP,
			     Bindings(4,
			       K_Arg("InOut2", KER_ARG_TILE),
			       (Ndct==0)?K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2", KER_ARG_TILE),
			       C_Arg("Norm"),
			       Imm(MfccBankCnt))):
			Call("norm_clip_32_melspect", LOC_LOOP,
			     Bindings(7,
			        K_Arg("InOut2", KER_ARG_TILE),
			 	K_Arg("Out",    KER_ARG_TILE),
				K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
				Imm(fft_bits), // fft_bits
				Imm(MFCC_Coeff_Dyn),
			       	K_Arg("shift_bf", KER_ARG_TILE), //shift_bf buffer in case of DataType==FIX32
				Imm(MfccBankCnt))
			     )):AT_NO_CALL,
			((Ndct>0) && (OutMelspectrogram==0))?
			Call(DCTKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("InOut2",    KER_ARG_TILE),
			       K_Arg("DCT_Coeff", KER_ARG_TILE),
			       K_Arg("Out",       KER_ARG_TILE),
			       Imm(Ndct),
			       Imm(MfccBankCnt))
			     ):AT_NO_CALL,
			(LifterCoeff && (OutMelspectrogram==0))?
			Call("MFCC_Lifter", LOC_LOOP,
			     Bindings(3,
			       K_Arg("InOut2",     KER_ARG_TILE),
			       K_Arg("lift_coeff", KER_ARG_TILE),
			       Imm(Ndct?Ndct:MfccBankCnt))
			     ):AT_NO_CALL
		),
		KerArgs((Ndct?14:13)+(DataType==FIX32?1:0)+(LifterCoeff?1:0),
			KerArg("In",              KerArgSpace(1,D0), OBJ_IN_DB,	  	  1, FrameSize, 	  	  DataType==FLOAT32?4:2,FrameSize-FrameStride, 0, 0, "In"),
			KerArg("Out",         	  KerArgSpace(1,D0), OBJ_OUT_DB,   	  1, MfccBankCnt,      		  DataType==FLOAT32||OutMelspectrogram?4:2,    0, 0, 0, "Out"),
			KerArg("InOut1",          KerArgSpace(1,T0), O_BUFF,   		  1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,   sizeof(int),          0, 0, 0, ""),
			KerArg("Out_fft",         KerArgSpace(1,T0), OutFFT?OBJ_OUT_DB:O_BUFF, 1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,   sizeof(int),          0, 0, 0, OutFFT?"FFT_Out":""),
			KerArg("shift_fft",       KerArgSpace(1,T0), OutFFT&&(DataType==FIX32)?OBJ_OUT_DB:O_BUFF,1, Nfft,    			  sizeof(signed char), 0, 0, 0, OutFFT&&(DataType==FIX32)?"FFT_Shift_Buff":""),
			(Ndct||DataType==FIX32||DataType==FIX16)?KerArg("InOut2", KerArgSpace(1,T0), O_BUFF, 1, MfccBankCnt, sizeof(int), 0, 0, 0, ""):AT_NO_KER_ARG,
			KerArg("Shift",		  KerArgSpace(1,T0), OutFFT&&(DataType==FIX16||DataType==FIX32)?OBJ_OUT_DB:O_BUFF, 1, 1, sizeof(short int),    0, 0, 0, OutFFT&&(DataType==FIX16||DataType==FIX32)?"PreempShift":""),
			KerArg("WinTable",        KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, FrameSize,              	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "WinTable"),
			use_radix_4_fft?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Nfft/4),   DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "Twiddles_fft"):
					KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, (Nfft),         DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	  sizeof(short int),    0, 0, 0, "SwapTable_fft"),
			KerArg("MFCC_FilterBank", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, MfccBankCnt,          	  sizeof(fbank_type_t), 0, 0, 0, "MFCC_FilterBank"),
			KerArg("MFCC_Coeffs",     KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, NMFCCCoeff,        	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "MFCC_Coeffs"),
			KerArg("shift_bf",   	  KerArgSpace(1,T0), O_BUFF,    	  1, MfccBankCnt,          	  sizeof(signed char),  0, 0, 0, ""),
			Ndct?KerArg("DCT_Coeff",  KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Ndct*Ndct,         	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "DCT_Coeff"):0,
			LifterCoeff?KerArg("lift_coeff", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST|O_NTILED, 1, MfccBankCnt, sizeof(short int),    0, 0, 0, "lift_coeff"):0
		)
	);
}

void STFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	float PreempFactor,
	int use_radix_4_fft,
	int out_fft,
	int use_power,
	int DataType
	)
{
	int fft_bits = 31 - __builtin_clz(Nfft);
	char *PreEmpKernel=0, *WinKernel=0, *FFTKernel=0, *SwapKernel=0, *MfccKernel=0, *SpectKernel=0, *LogKernel=0, *DCTKernel=0, *UserKernType=0, *UserKernPointer=0;

	switch (DataType){
		case FIX16:
			PreEmpKernel = "MFCC_PreEmphasis";
			WinKernel = "MFCC_WindowedFrame";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_Fix16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par";
			SwapKernel = "SwapSamples_Par";
			SpectKernel = use_power?"MFCC_PowerV2S":"MFCC_Abs";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FIX32:
			PreEmpKernel = "MFCC_PreEmphasis";
			WinKernel = "MFCC_WindowedFrame_int";
			if (use_radix_4_fft) return GenTilingError("Radix4 FFT Not implemented for FIX32");
			else  		     FFTKernel = "Radix2FFT_DIF_INT_Scal_Par";
			SwapKernel = "SwapSamples_scal";
			SpectKernel = use_power?"MFCC_Power":"MFCC_Abs_BFF";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FLOAT16:
			PreEmpKernel = "MFCC_PreEmphasis_f16";
			WinKernel = "MFCC_WindowedFrame_f16";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f16";
			SwapKernel = "SwapSamples_Par_f16";
			SpectKernel = use_power?"MFCC_Power_f16":"MFCC_Abs_f16";
			UserKernType = "f16";
			UserKernPointer = "f16 * __restrict__";
			break;
		case FLOAT32:
			PreEmpKernel = "MFCC_PreEmphasis_f32";
			WinKernel = "MFCC_WindowedFrame_f32";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f32";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f32";
			SwapKernel = "SwapSamples_Par_f32";
			SpectKernel = use_power?"MFCC_Power_f32":"MFCC_Abs_f32";
			UserKernType = "float";
			UserKernPointer = "float * __restrict__";
			break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}

	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(2, IterFixedSpace(D0, NFrames), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(6,
                	TCArg(UserKernPointer, "In"),
			TCArg(DataType==FIX16||DataType==FIX32?"int * __restrict__":UserKernPointer, "Out"),
			TCArg(UserKernType,    "Prev"),
			TCArg(UserKernPointer, "Twiddles_fft"),
			TCArg("short int *", "SwapTable_fft"),
			TCArg(UserKernPointer, "WinTable")
                ),
                Calls(6,
			Call(PreEmpKernel,LOC_LOOP,
			     Bindings(6,
			       K_Arg("In"   , KER_ARG_TILE),
			       K_Arg("InOut1"   , KER_ARG_TILE),
			       C_Arg("Prev"),
			       (DataType==FIX16 || DataType==FIX32)?Imm(FP2FIX(PreempFactor, 15)):Imm(PreempFactor),
			       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(FrameSize)
			       )
			     ),
			Call(WinKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("InOut1"  , KER_ARG_TILE),
			       out_fft?K_Arg("Out", KER_ARG_TILE):K_Arg("Out_fft", KER_ARG_TILE),
			       K_Arg("WinTable" , KER_ARG_TILE ),
			       Imm(FrameSize),
			       Imm(Nfft))
			     ),
			Call(FFTKernel,LOC_LOOP,
			      Bindings(DataType==FIX32?4:5,
			       out_fft?K_Arg("Out", KER_ARG_TILE):K_Arg("Out_fft", KER_ARG_TILE),
			       K_Arg("Twiddles_fft", KER_ARG_TILE ),
			       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
			       Imm(Nfft),
			       Imm(0))
			     ),
			Call(SwapKernel,LOC_LOOP,
			      Bindings(DataType==FIX32?4:3,
			       out_fft?K_Arg("Out", KER_ARG_TILE):K_Arg("Out_fft", KER_ARG_TILE),
			       K_Arg("SwapTable_fft", KER_ARG_TILE),
			       Imm(Nfft),
			       K_Arg("shift_fft", KER_ARG_TILE)) //shift_fft buffer
			     ),
			out_fft==0?Call(SpectKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("Out_fft"   , KER_ARG_TILE),
			       (DataType==FLOAT32||DataType==FLOAT16)?K_Arg("Out", KER_ARG_TILE):K_Arg("InOut1", KER_ARG_TILE),
			       K_Arg("shift_fft"   , KER_ARG_TILE),
		       	       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(Nfft))
			     ):AT_NO_CALL,
			out_fft==0&&(DataType==FIX16||DataType==FIX32)?
				Call("norm_clip_32_spect", LOC_LOOP,
				     Bindings(6,
				        K_Arg("InOut1",    KER_ARG_TILE),
				 	K_Arg("Out",    KER_ARG_TILE),
					K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
					Imm(fft_bits), // fft_bits
				       	K_Arg("shift_fft", KER_ARG_TILE), //shift_bf buffer in case of high_prec_fft
					Imm(Nfft)
					)
				):AT_NO_CALL
		),
		KerArgs(9,
			KerArg("In",              KerArgSpace(1,D0), OBJ_IN_DB,	  	  1, FrameSize, DataType==FLOAT32?4:2,    FrameSize-FrameStride, 0, 0, "In"),
			out_fft==0?
				KerArg("Out",     KerArgSpace(1,D0), OBJ_OUT_DB,   	  1, DataType==FIX32?2*(Nfft/2+1):(Nfft/2+1),		DataType==FLOAT16?2:4, 0, 0, 0, "Out"):
				KerArg("Out",     KerArgSpace(1,D0), OBJ_OUT_DB,	  1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,  DataType==FLOAT16?2:4, 0, 0, 0, "Out"),
			KerArg("InOut1",          KerArgSpace(1,T0), O_BUFF,   		  1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,  DataType==FLOAT16?2:4, 0, 0, 0, ""),
			out_fft==0?
				KerArg("Out_fft", KerArgSpace(1,T0), O_BUFF,	   	  1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,  DataType==FLOAT16?2:4, 0, 0, 0, ""):AT_NO_KER_ARG,
			KerArg("shift_fft",       KerArgSpace(1,T0), O_BUFF,       	  1, Nfft,						sizeof(signed char),   0, 0, 0, ""),
			KerArg("Shift",		  KerArgSpace(1,T0), O_BUFF,	          1, 1, 						sizeof(short int),     0, 0, 0, ""),
			KerArg("WinTable",        KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,						DataType==FLOAT32?4:2,    0, 0, 0, "WinTable"),
			use_radix_4_fft?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Nfft/4),   DataType==FLOAT32?4:2,    0, 0, 0, "Twiddles_fft"):
					KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(Nfft),       DataType==FLOAT32?4:2,    0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	  sizeof(short int),        0, 0, 0, "SwapTable_fft")
		)
	);
}

void ISTFT_Generator(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	int use_radix_4_fft,
	int DataType
	)
{
	int fft_bits = 31 - __builtin_clz(Nfft);
	char *FFTKernel=0, *SwapKernel=0, *UserKernType=0, *UserKernPointer=0, *ConjugateKernel=0;

	switch (DataType){
		case FIX16:
			ConjugateKernel = "Conjugate_Fix16_Par";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_Fix16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par";
			SwapKernel = "SwapSamples_Par";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FIX32:
			ConjugateKernel = "Conjugate_Fix32_Par";
			if (use_radix_4_fft) return GenTilingError("Radix4 FFT Not implemented for FIX32");
			else  		     FFTKernel = "Radix2FFT_DIF_INT_Scal_Par";
			SwapKernel = "SwapSamples_scal";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FLOAT16:
			ConjugateKernel = "Conjugate_Float16_Par";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f16";
			SwapKernel = "SwapSamples_Par_f16";
			UserKernType = "f16";
			UserKernPointer = "f16 * __restrict__";
			break;
		case FLOAT32:
			ConjugateKernel = "Conjugate_Float32_Par";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f32";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f32";
			SwapKernel = "SwapSamples_Par_f32";
			UserKernType = "float";
			UserKernPointer = "float * __restrict__";
			break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}

	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(2, IterFixedSpace(D0, NFrames), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(3,
                	TCArg(UserKernPointer, "In"),
			TCArg(UserKernPointer, "Twiddles_fft"),
			TCArg("short int *", "SwapTable_fft")
                ),
                Calls(4,
                	Call(ConjugateKernel, LOC_LOOP, Bindings(2, K_Arg("In", KER_ARG_TILE), Imm(Nfft))),
                	Call(FFTKernel,LOC_LOOP,
			     Bindings(DataType==FIX32?4:5,
			       K_Arg("In",       KER_ARG_TILE),
			       K_Arg("Twiddles_fft", KER_ARG_TILE ),
			       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
			       Imm(Nfft),
			       Imm(0)
			       )
			     ),
			Call(SwapKernel,LOC_LOOP,
			     Bindings(DataType==FIX32?4:3,
			       K_Arg("In",       KER_ARG_TILE),
			       K_Arg("SwapTable_fft", KER_ARG_TILE),
			       Imm(Nfft),
			       K_Arg("shift_fft", KER_ARG_TILE)) //shift_fft buffer
			     ),
                	Call(ConjugateKernel, LOC_LOOP, Bindings(2, K_Arg("In", KER_ARG_TILE), Imm(Nfft)))
		),
		KerArgs(5,
			KerArg("In",              KerArgSpace(1,D0), OBJ_IN_DB,	  	  1, 2*FrameSize, 	  	  DataType==FLOAT32?4:2, 0, 0, 0, "In"),
			KerArg("shift_fft",       KerArgSpace(1,T0), O_BUFF,       	  1, Nfft, 			  sizeof(signed char),   0, 0, 0, ""),
			KerArg("Shift",		  KerArgSpace(1,T0), O_BUFF,	          1, 1, 		     	  sizeof(short int),     0, 0, 0, ""),
			use_radix_4_fft?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Nfft/4),   DataType==FLOAT32?4:2, 0, 0, 0, "Twiddles_fft"):
					KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(Nfft),       DataType==FLOAT32?4:2, 0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	  sizeof(short int),     0, 0, 0, "SwapTable_fft")
		)
	);
}

#if 0
void IMFCC_parallel(char *Name, int nbin, int n_fft,  int imfcc_coeff_cnt) {

  int nb_cargs = 10;
  int nbcalls = 4;
  CKernel_Arg_T **KCArgs = AllocateCArgs(nb_cargs);
  int Ca=0;

  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "In");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Out");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "In_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "Twiddles_inv_fft");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "SwapTable_fft");
  KCArgs[Ca++] =  TCArg("fbank_type_t * __restrict__", "IMFCC_FilterBank");
  KCArgs[Ca++] =  TCArg("int16_t * __restrict__", "IMFCC_Coeffs");
  KCArgs[Ca++] =  TCArg("int16_t", "nbin");
  KCArgs[Ca++] =  TCArg("int16_t", "nfft");
  KCArgs[Ca++] =  TCArg("int16_t", "numcep");


	UserKernel(Name,
		   KernelIterSpace(1, IterTiledSpace(KER_TILE)),
		   TILE_HOR,
		   KCArgs,
		   Calls(nbcalls,
			 Call("IMEL",LOC_LOOP,
			      Bindings(6,
				       K_Arg("In"   , KER_ARG_TILE),
				       K_Arg("OMEL"   , KER_ARG_TILE),
				       C_Arg("IMFCC_FilterBank" ),
				       C_Arg("IMFCC_Coeffs" ),
				       C_Arg("nbin"),
				       C_Arg("nfft")
				       )
			      ),
			 Call("Coeff_mult",LOC_LOOP,
			      Bindings(4,
				       K_Arg("OMEL"   , KER_ARG_TILE),
				       K_Arg("In_fft"   , KER_ARG_TILE),
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("nfft" )
				       )
			      ),
			 Call("RadixMixedFFT_DIF_Par",LOC_LOOP,
			      Bindings(3,
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("Twiddles_inv_fft" ),
				       C_Arg("nfft")
				       )
			      ),
			 Call("SwapSamples_Par",LOC_LOOP,
			      Bindings(3,
				       K_Arg("Out"   , KER_ARG_TILE),
				       C_Arg("SwapTable_fft" ),
				       C_Arg("nfft" )
				       )
			      )
			 ),
		   KerArgs(8,

			   KerArg("In", KerArgSpace(1,KER_TILE), O_BUFF|O_IN, 1, nbin, sizeof(short int), 0, 0, 0, "In"),
			   KerArg("In_fft", KerArgSpace(1,KER_TILE), O_BUFF|O_IN, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "In_fft"),
			   KerArg("Out", KerArgSpace(1,KER_TILE), O_BUFF|O_OUT, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "Out"),
			   KerArg("OMEL", KerArgSpace(1,KER_TILE), O_BUFF, 1, n_fft, sizeof(short int), 0, 0, 0, ""),
			   KerArg("Twiddles_inv_fft", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, 2*n_fft, sizeof(short int), 0, 0, 0, "Twiddles_inv_fft"),
			   KerArg("SwapTable_fft", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, n_fft, sizeof(short int), 0, 0, 0, "SwapTable_fft"),
			   KerArg("IMFCC_FilterBank", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, nbin, sizeof(fbank_type_t), 0, 0, 0, "IMFCC_FilterBank"),
			   KerArg("IMFCC_Coeffs", KerArgSpace(1,KER_TILE), O_IN|O_DB|O_CONST, 1, imfcc_coeff_cnt, sizeof(short int), 0, 0, 0, "IMFCC_Coeffs")
			   )
		   );
}

void MFCC_GeneratorV2(
	char *Name,
	CNN_GenControl_T *Ctrl,
	int NFrames,
	int FrameSize,
	int FrameStride,
	int Nfft,
	int NMFCCCoeff,
	int Ndct,
	float PreempFactor,
	int use_radix_4_fft,
	int Power,
	int UseDB,
	int DataType,
	int MfccBankCnt,
	int OutMelspectrogram,
	int OutFFT
	)
{
	int MFCC_Coeff_Dyn = 15;
	int fft_bits = 31 - __builtin_clz(Nfft);
	printf("MFCC_COEF_DYN = %d\nFFT_BITS = %d\nUSE_DB = %d\nDATA_TYPE = %d\n", MFCC_Coeff_Dyn, fft_bits, UseDB, DataType);

	char *PreEmpKernel=0, *WinKernel=0, *FFTKernel=0, *SwapKernel=0, *MfccKernel=0, *SpectKernel=0, *LogKernel=0, *DCTKernel=0, *UserKernType=0, *UserKernPointer=0;      

	switch (DataType){
		case FIX16:
			PreEmpKernel = PreempFactor?"MFCC_PreEmphasis":0;
			WinKernel    = "MFCC_WindowedFrame";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_Fix16";
			else  		     	 FFTKernel = "Radix2FFT_DIF_Par_Fix16";
			SwapKernel = "SwapSamples_Par";
			SpectKernel = Power?"MFCC_PowerV2S":"MFCC_Abs";
			MfccKernel = "MFCC_ComputeMFCC";
			LogKernel = UseDB?"MFCC_ComputeDB":"MFCC_ComputeLog";
			DCTKernel = "MFCC_ComputeDCT_II";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FIX32:
			PreEmpKernel = PreempFactor?"MFCC_PreEmphasis":0;
			WinKernel    = "MFCC_WindowedFrame_int";
			if (use_radix_4_fft) return GenTilingError("Radix4 FFT Not implemented for FIX32");
			else  		     FFTKernel = "Radix2FFT_DIF_Par_Fix32";
			SwapKernel = "SwapSamples_Par_Fix32";
			SpectKernel = Power?"MFCC_Power":"MFCC_Abs_BFF";
			MfccKernel = "MFCC_ComputeMFCC";
			LogKernel = UseDB?"MFCC_ComputeDB":"MFCC_ComputeLog";
			DCTKernel = "MFCC_ComputeDCT_II";
			UserKernType = "int16_t";
			UserKernPointer = "int16_t * __restrict__";
			break;
		case FLOAT16:
			PreEmpKernel = PreempFactor?"MFCC_PreEmphasis_f16":0;
			WinKernel    = "MFCC_WindowedFrame_f16";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f16";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f16";
			SwapKernel = "SwapSamples_Par_f16";
			SpectKernel = Power?"MFCC_Power_f16":"MFCC_Abs_f16";
			MfccKernel = "MFCC_ComputeMFCC_f16";
			LogKernel = "MFCC_ComputeLog_f16";
			DCTKernel = "MFCC_ComputeDCT_II_f16";
			UserKernType = "f16";
			UserKernPointer = "f16 * __restrict__";
			break;
		case FLOAT32:
			PreEmpKernel = PreempFactor?"MFCC_PreEmphasis_f32":0;
			WinKernel    = "MFCC_WindowedFrame_f32";
			if (use_radix_4_fft) FFTKernel = "Radix4FFT_DIF_Par_f32";
			else  		     FFTKernel = "Radix2FFT_DIF_Par_f32";
			SwapKernel = "SwapSamples_Par_f32";
			SpectKernel = Power?"MFCC_Power_f32":"MFCC_Abs_f32";
			MfccKernel = "MFCC_ComputeMFCC_f32";
			LogKernel = "MFCC_ComputeLog_f32";
			DCTKernel = "MFCC_ComputeDCT_II_f32";
			UserKernType = "float";
			UserKernPointer = "float * __restrict__";
			break;
		default:
			return GenTilingError("Data Type %d not known", DataType);
	}

	int ncalls = 10;
	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(2, IterFixedSpace(D0, NFrames), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs(9,
                	TCArg(UserKernPointer, "In"),
			TCArg(UserKernPointer, "Out"),
			PreempFactor?TCArg(UserKernType, "Prev"):AT_NO_C_ARG,
			TCArg(UserKernPointer, "Twiddles_fft"),
			TCArg("short int *", "SwapTable_fft"),
			TCArg(UserKernPointer, "WinTable"),
			TCArg("fbank_type_t *", "MFCC_FilterBank"),
			TCArg(UserKernPointer, "MFCC_Coeffs"),
			Ndct?TCArg(UserKernPointer, "DCT_Coeff"):AT_NO_C_ARG
                ),
                Calls(9,
			(PreEmpKernel==0)?AT_NO_CALL:Call(PreEmpKernel,LOC_LOOP,
			     Bindings(6,
			       K_Arg("In", KER_ARG_TILE),
			       K_Arg("InOut1", KER_ARG_TILE),
			       C_Arg("Prev"),
			       (DataType==FIX16 || DataType==FIX32)?Imm(FP2FIX(PreempFactor, 15)):Imm(PreempFactor),
			       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(FrameSize))
			     ),
			Call(WinKernel,LOC_LOOP,
			     Bindings(5,
			       PreEmpKernel==0?K_Arg("In", KER_ARG_TILE):K_Arg("InOut1", KER_ARG_TILE),
			       K_Arg("Out_fft" , KER_ARG_TILE),
			       K_Arg("WinTable" , KER_ARG_TILE ),
			       Imm(FrameSize),
			       Imm(Nfft))
			     ),
			Call(FFTKernel,LOC_LOOP,
			      Bindings(5,
			       K_Arg("Out_fft", KER_ARG_TILE),
			       K_Arg("Twiddles_fft", KER_ARG_TILE ),
			       AT_IGNORE_ARG_BINDING, // K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
			       Imm(Nfft),
			       Imm(0))
			     ),
			Call(SwapKernel,LOC_LOOP,
			      Bindings(3,
			       K_Arg("Out_fft",       KER_ARG_TILE),
			       K_Arg("SwapTable_fft", KER_ARG_TILE),
			       Imm(Nfft))
			     ),
			Call(SpectKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("Out_fft"   , KER_ARG_TILE),
			       K_Arg("InOut1"   , KER_ARG_TILE),
			       Imm(0), //K_Arg("shift_fft"   , KER_ARG_TILE),
		       	       Imm(0), //K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(Nfft))
			     ),
			Call(MfccKernel, LOC_LOOP,
			     Bindings(8,
			       K_Arg("InOut1"  	      , KER_ARG_TILE),
			       (Ndct==0 && (DataType==FLOAT16 || DataType==FLOAT32))?
				       K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2" , KER_ARG_TILE),
			       K_Arg("MFCC_FilterBank", KER_ARG_TILE),
			       K_Arg("MFCC_Coeffs"    , KER_ARG_TILE),
			       Imm(0), //K_Arg("shift_fft"      , KER_ARG_TILE),
			       K_Arg("shift_bf"       , KER_ARG_TILE),
			       Imm(MfccBankCnt),
			       Imm(MFCC_Coeff_Dyn))
			     ),
			(OutMelspectrogram==0)?
			Call(LogKernel,LOC_LOOP,
			     Bindings(6,
			 	(Ndct==0 && (DataType==FLOAT16 || DataType==FLOAT32))?
			 		K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2", KER_ARG_TILE),
				Imm(MfccBankCnt),
				Imm(0), //K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
				Imm(fft_bits), // fft_bits
				Imm(MFCC_Coeff_Dyn),
			       	K_Arg("shift_bf", KER_ARG_TILE)) //shift_bf buffer in case of DataType==FIX32
			      ):AT_NO_CALL,
			(DataType==FIX32 || DataType==FIX16)?((OutMelspectrogram==0)?
			Call("norm_clip_16",LOC_LOOP,
			     Bindings(4,
			       K_Arg("InOut2", KER_ARG_TILE),
			       (Ndct==0)?K_Arg("Out", KER_ARG_TILE):K_Arg("InOut2", KER_ARG_TILE),
			       Imm(5),
			       Imm(MfccBankCnt))):
			Call("norm_clip_32_melspect", LOC_LOOP,
			     Bindings(7,
			        K_Arg("InOut2", KER_ARG_TILE),
			 	K_Arg("Out",    KER_ARG_TILE),
				Imm(0), //K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
				Imm(fft_bits), // fft_bits
				Imm(MFCC_Coeff_Dyn),
			       	K_Arg("shift_bf", KER_ARG_TILE), //shift_bf buffer in case of DataType==FIX32
				Imm(MfccBankCnt))
			     )):AT_NO_CALL,
			((Ndct>0) && (OutMelspectrogram==0))?
			Call(DCTKernel,LOC_LOOP,
			     Bindings(5,
			       K_Arg("InOut2",    KER_ARG_TILE),
			       K_Arg("DCT_Coeff", KER_ARG_TILE),
			       K_Arg("Out",       KER_ARG_TILE),
			       Imm(Ndct),
			       Imm(MfccBankCnt))
			     ):AT_NO_CALL
		),
		KerArgs(13-(Ndct==0?1:0),
			KerArg("In",              KerArgSpace(1,D0), OBJ_IN_DB,	  	  1, FrameSize, 	  	  DataType==FLOAT32?4:2,FrameSize-FrameStride, 0, 0, "In"),
			KerArg("Out",         	  KerArgSpace(1,D0), OBJ_OUT_DB,   	  1, MfccBankCnt,      		  DataType==FLOAT32||OutMelspectrogram?4:2,    0, 0, 0, "Out"),
			KerArg("InOut1",          KerArgSpace(1,T0), O_BUFF,   		  1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,   sizeof(int),          0, 0, 0, ""),
			KerArg("Out_fft",         KerArgSpace(1,T0), OutFFT?OBJ_OUT_DB:O_BUFF, 1, (DataType==FLOAT32||DataType==FIX32)?2*Nfft:Nfft,   sizeof(int),          0, 0, 0, OutFFT?"FFT_Out":""),
			(Ndct||DataType==FIX32||DataType==FIX16)?KerArg("InOut2", KerArgSpace(1,T0), O_BUFF, 1, MfccBankCnt, sizeof(int), 0, 0, 0, ""):AT_NO_KER_ARG,
			KerArg("WinTable",        KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, FrameSize,              	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "WinTable"),
			use_radix_4_fft?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Nfft/4),   DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "Twiddles_fft"):
					KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, (Nfft),         DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	  sizeof(short int),    0, 0, 0, "SwapTable_fft"),
			KerArg("MFCC_FilterBank", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, MfccBankCnt,          	  sizeof(fbank_type_t), 0, 0, 0, "MFCC_FilterBank"),
			KerArg("MFCC_Coeffs",     KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, NMFCCCoeff,        	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "MFCC_Coeffs"),
			KerArg("shift_bf",   	  KerArgSpace(1,T0), O_BUFF,    	  1, MfccBankCnt,          	  sizeof(signed char),  0, 0, 0, ""),
			Ndct?KerArg("DCT_Coeff",  KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Ndct*Ndct,         	  DataType==FLOAT32?4:sizeof(short int),    0, 0, 0, "DCT_Coeff"):0
			)
		);
}
#endif
