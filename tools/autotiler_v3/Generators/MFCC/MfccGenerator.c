/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdint.h>
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
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
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
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_Arg_T",NULL
		 );
       
        LibKernel("RadixMixedFFT_DIF_Inv_Par", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "Twiddles"),
		       TCArg("signed char * __restrict__"   , "shift_fft"),
		       TCArg("short int"   , "N_fft")
		       ),
		 "FFT_Arg_T",NULL
		 );
	
	LibKernel("SwapSamples_Par", CALL_PARALLEL,
		 CArgs(3,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("int"   , "Ni")
		       ),
		 "SwapSamples_Arg_T",NULL
		 );

	LibKernel("SwapSamples_scal", CALL_PARALLEL,
		 CArgs(4,
		       TCArg("short int * __restrict__"   , "Data"),
		       TCArg("short int * __restrict__"   , "SwapTable"),
		       TCArg("signed char * __restrict__"   , "shift_BF"),
		       TCArg("int"   , "Ni")
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

        LibKernel("MFCC_Power", CALL_PARALLEL,
		  CArgs(4,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_T",NULL
		  );

        LibKernel("MFCC_PowerV2S", CALL_PARALLEL,
		  CArgs(4,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_T",NULL
		  );

        LibKernel("MFCC_Abs", CALL_PARALLEL,
		  CArgs(4,
			TCArg("v2s * __restrict__"   , "FrameIn"),
			TCArg("int * __restrict__"   , "FrameOut"),
			TCArg("signed char * __restrict__"   , "shift_fft"),
			TCArg("int", "nfft")
			),
		  "MFCC_EP_T",NULL
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

        LibKernel("MFCC_ComputeMFCC", CALL_PARALLEL,
		  CArgs(6,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("short int","MFCC_bank_cnt"),
			TCArg("short int","MFCC_Coeff_dyn")
			),
		  "MFCC_MF_T",NULL
		  );

        LibKernel("MFCC_ComputeMFCC_BFF", CALL_PARALLEL,
		  CArgs(7,
			TCArg("unsigned int * __restrict__"   , "FramePower"),
			TCArg("unsigned int * __restrict__"   , "MFCC"),
			TCArg("fbank_type_t *__restrict__", "MFCC_FilterBank"),
			TCArg("short int *__restrict__", "MFCC_Coeffs"),
			TCArg("signed char *__restrict__", "shift_pow"),
			TCArg("signed char *__restrict__", "shift_BF"),
			TCArg("short int","MFCC_bank_cnt")
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
		  CArgs(3,
			TCArg("short int * __restrict__"   , "Out"),
			TCArg("unsigned short int", "Norm"),
			TCArg("int", "N")
			),
		  "Norm_Clip_args_T",NULL
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
	int high_prec_fft
	)
{
	// if ((!use_power) && high_prec_fft) {
	// 	printf("Error: from spectrogram MFCC does not support high precision FFT \n");
	// 	return;
	// }
	int MFCC_Coeff_Dyn = 10;
	int fft_bits = 31 - __builtin_clz(Nfft);
	Kernel_T *Kernel = UserKernel(Name,
                KernelIterSpace(2, IterFixedSpace(D0, NFrames), IterTiledSpace(T0)),
                TILE_HOR,
                CArgs((Ndct?10:9)+(LifterCoeff?1:0),
                	TCArg("int16_t * __restrict__",      "In"),
			TCArg("int16_t * __restrict__",      "Out"),
			TCArg("int16_t", "Prev"),
			TCArg("int16_t * __restrict__",      "Twiddles_fft"),
			TCArg("int16_t * __restrict__",      "SwapTable_fft"),
			TCArg("int16_t * __restrict__",      "WinTable"),
			TCArg("fbank_type_t * __restrict__", "MFCC_FilterBank"),
			TCArg("int16_t * __restrict__",      "MFCC_Coeffs"),
			TCArg("int16_t", 		     "Norm"),
			Ndct?TCArg("int16_t * __restrict__", "DCT_Coeff"):AT_NO_C_ARG,
			LifterCoeff?TCArg("int16_t * __restrict__", "lift_coeff"):AT_NO_C_ARG
                ),
                Calls((Ndct?9:8)+(LifterCoeff?1:0),
			Call("MFCC_PreEmphasis",LOC_LOOP,
			     Bindings(6,
			       K_Arg("In"   , KER_ARG_TILE),
			       K_Arg("InOut1"   , KER_ARG_TILE),
			       C_Arg("Prev"),
			       Imm(FP2FIX(PreempFactor, 15)),
			       K_Arg("Shift"   , KER_ARG_TILE),
			       Imm(FrameSize)
			       )
			     ),
			(high_prec_fft==0)?
				Call("MFCC_WindowedFrame",LOC_LOOP,
				     Bindings(5,
				       K_Arg("InOut1"  , KER_ARG_TILE),
				       K_Arg("Out_fft" , KER_ARG_TILE),
				       K_Arg("WinTable" , KER_ARG_TILE ),
				       Imm(FrameSize),
				       Imm(Nfft)
				       )
				     ):
				Call("MFCC_WindowedFrame_int",LOC_LOOP,
				     Bindings(5,
				       K_Arg("InOut1"  , KER_ARG_TILE),
				       K_Arg("Out_fft" , KER_ARG_TILE),
				       K_Arg("WinTable" , KER_ARG_TILE ),
				       Imm(FrameSize),
				       Imm(Nfft)
				       )
				     ),
			(high_prec_fft==0)?
				use_radix_4_fft?
				Call("RadixMixedFFT_DIF_Par",LOC_LOOP,
				     Bindings(4,
				       K_Arg("Out_fft"     , KER_ARG_TILE),
				       K_Arg("Twiddles_fft", KER_ARG_TILE),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       Imm(Nfft)
				       )
				):Call("Radix2FFT_DIF_Par",LOC_LOOP,
				     Bindings(4,
				       K_Arg("Out_fft",       KER_ARG_TILE),
				       K_Arg("Twiddles_fft", KER_ARG_TILE ),
				       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
				       Imm(Nfft)
				       )
				     ):
				Call("Radix2FFT_DIF_INT_Scal_Par",LOC_LOOP,
				      Bindings(4,
				       K_Arg("Out_fft",       KER_ARG_TILE),
				       K_Arg("Twiddles_fft", KER_ARG_TILE ),
				       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
				       Imm(Nfft)
				      )
				     ),
			(high_prec_fft==0)?
				Call("SwapSamples_Par",LOC_LOOP,
				     Bindings(3,
				       K_Arg("Out_fft",       KER_ARG_TILE),
				       K_Arg("SwapTable_fft", KER_ARG_TILE),
				       Imm(Nfft)
				       )
				     ):
				Call("SwapSamples_scal",LOC_LOOP,
				      Bindings(4,
				       K_Arg("Out_fft",       KER_ARG_TILE),
				       K_Arg("SwapTable_fft", KER_ARG_TILE),
				       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
				       Imm(Nfft)
				      )
				     ),
			(high_prec_fft==0)?
				use_power?Call("MFCC_PowerV2S",LOC_LOOP,
				     Bindings(4,
				       K_Arg("Out_fft",   KER_ARG_TILE),
				       K_Arg("InOut1",   KER_ARG_TILE),
				       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
				       Imm(Nfft)
				       )
				     ):
				Call("MFCC_Abs",LOC_LOOP,
				     Bindings(4,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("InOut1"   , KER_ARG_TILE),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
				       Imm(Nfft)
				       )
				     ):
				use_power?Call("MFCC_Power",LOC_LOOP,
				      Bindings(4,
				       K_Arg("Out_fft",   KER_ARG_TILE),
				       K_Arg("InOut1",   KER_ARG_TILE),
				       K_Arg("shift_fft", KER_ARG_TILE), //shift_fft buffer
				       Imm(Nfft)
				      )
				     ):
				Call("MFCC_Abs_BFF",LOC_LOOP,
				     Bindings(5,
				       K_Arg("Out_fft"   , KER_ARG_TILE),
				       K_Arg("InOut1"   , KER_ARG_TILE),
				       K_Arg("shift_fft"   , KER_ARG_TILE),
			       	       K_Arg("Shift"   , KER_ARG_TILE),
				       Imm(Nfft)
				       )
				     ),
			(high_prec_fft==0)?
				Call("MFCC_ComputeMFCC", LOC_LOOP,
				      Bindings(6,
					K_Arg("InOut1",           KER_ARG_TILE),
					K_Arg("InOut2", KER_ARG_TILE),
					K_Arg("MFCC_FilterBank",   KER_ARG_TILE),
					K_Arg("MFCC_Coeffs",       KER_ARG_TILE),
					Imm(Ndct),
					Imm(MFCC_Coeff_Dyn)
					)
				      ):
				Call("MFCC_ComputeMFCC_BFF", LOC_LOOP,
				      Bindings(7,
					       K_Arg("InOut1"   	  , KER_ARG_TILE),
					       K_Arg("InOut2"   	  , KER_ARG_TILE),
					       K_Arg("MFCC_FilterBank", KER_ARG_TILE),
					       K_Arg("MFCC_Coeffs"    , KER_ARG_TILE),
					       K_Arg("shift_fft"      , KER_ARG_TILE),
					       K_Arg("shift_bf"       , KER_ARG_TILE),
					       Imm(Ndct)
					)
				      ),

			Call("MFCC_ComputeLog",LOC_LOOP,
			     Bindings(6,
			 	K_Arg("InOut2", KER_ARG_TILE),
				Imm(Ndct),
				K_Arg("Shift",  KER_ARG_TILE), // PreEmphasis Shift
				Imm(fft_bits), // fft_bits
				Imm(MFCC_Coeff_Dyn),
			       	K_Arg("shift_bf", KER_ARG_TILE) //shift_bf buffer in case of high_prec_fft
				)
			      ),
			Call("norm_clip_16",LOC_LOOP,
			     Bindings(3,
			       K_Arg("InOut2", KER_ARG_TILE),
			       C_Arg("Norm"),
			       Imm(Ndct)
			       )
			     ),
			Ndct?
			Call("MFCC_ComputeDCT_II",LOC_LOOP,
			     Bindings(5,
			       K_Arg("InOut2", KER_ARG_TILE),
			       K_Arg("DCT_Coeff",         KER_ARG_TILE),
			       K_Arg("Out",            KER_ARG_TILE),
			       Imm(Ndct),
			       Imm(Ndct)
			       )
			     ):AT_NO_CALL,
			LifterCoeff?
			Call("MFCC_Lifter", LOC_LOOP,
			     Bindings(3,
			       K_Arg("InOut2", KER_ARG_TILE),
			       K_Arg("lift_coeff", KER_ARG_TILE),
			       Imm(Ndct)
			       )
			     ):AT_NO_CALL
		),
		KerArgs((Ndct?14:13)+(high_prec_fft?1:0)+(LifterCoeff?1:0),
			KerArg("In",              KerArgSpace(1,D0), OBJ_IN_DB,	  	  1, FrameSize, 	  	sizeof(short int),    FrameSize-FrameStride, 0, 0, "In"),
			KerArg("Out",         	  KerArgSpace(1,D0), OBJ_OUT_DB,   	  1, Ndct,          		sizeof(short int),    0, 0, 0, "Out"),
			KerArg("InOut1",          KerArgSpace(1,T0), O_BUFF,   		  1, 2*Nfft,           		sizeof(int),          0, 0, 0, ""),
			KerArg("Out_fft",         KerArgSpace(1,T0), O_BUFF,   		  1, 2*Nfft,		        sizeof(int),          0, 0, 0, ""),
			KerArg("shift_fft",       KerArgSpace(1,T0), O_BUFF,       	  1, 2*Nfft,            	sizeof(signed char),  0, 0, 0, ""),
			KerArg("InOut2",          KerArgSpace(1,T0), O_BUFF,   		  1, Ndct,	      		sizeof(int),          0, 0, 0, ""),
			KerArg("Shift",		  KerArgSpace(1,T0), O_BUFF,	          1, 1, 		     	sizeof(short int),    0, 0, 0, ""),
			KerArg("WinTable",        KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	sizeof(short int),    0, 0, 0, "WinTable"),
			use_radix_4_fft?KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(3*Nfft/4), sizeof(short int),    0, 0, 0, "Twiddles_fft"):
					KerArg("Twiddles_fft", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, 2*(Nfft),     sizeof(short int),    0, 0, 0, "Twiddles_fft"),
			KerArg("SwapTable_fft",   KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Nfft,              	sizeof(short int),    0, 0, 0, "SwapTable_fft"),
			KerArg("MFCC_FilterBank", KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Ndct,              	sizeof(fbank_type_t), 0, 0, 0, "MFCC_FilterBank"),
			KerArg("MFCC_Coeffs",     KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, NMFCCCoeff,        	sizeof(short int),    0, 0, 0, "MFCC_Coeffs"),
			KerArg("DCT_Coeff",       KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST, 1, Ndct*Ndct,         	sizeof(short int),    0, 0, 0, "DCT_Coeff"),
			KerArg("shift_bf",   	  KerArgSpace(1,T0), O_BUFF,    	  1, Ndct,              	sizeof(signed char),  0, 0, 0, ""),
			LifterCoeff?KerArg("lift_coeff",      KerArgSpace(1,T0), O_IN|O_BUFF|O_CONST|O_NTILED, 1, Ndct, 	sizeof(short int),    0, 0, 0, "lift_coeff"):0
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
#endif
