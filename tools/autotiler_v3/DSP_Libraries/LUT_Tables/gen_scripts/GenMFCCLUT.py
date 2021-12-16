#!/usr/bin/python
import json
import numpy as np
import math
import sys
import argparse
from SetupLUT import FP2FIX, SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4, SetupDCTTable, SetupLiftCoeff, SetupTwiddlesRFFT, DCT_TWIDDLE_DYN, GenMelFilterBanksCode

def array_to_def_c_file(arr, name, data_type, size, elem_in_rows=2):
	Out_str = ""
	# Window
	Out_str += "PI_L2 {} {}[{}] = {{\n\t".format(data_type, name, size)
	for i, elem in enumerate(arr):
		Out_str += f"{elem:>6}, "
		if (i+1)%elem_in_rows == 0:
			Out_str += "\n\t"
	Out_str += "\n}; \n"
	return Out_str

def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='mfcc_lut_gen')

	parser.add_argument('--params_json', default=None,
						help="Path to .json file where to get parameters")
	parser.add_argument('--fft_lut_file', required="--params_json" not in sys.argv,
						help="path to fft lut file")
	parser.add_argument('--mfcc_bf_lut_file', default=None,
						help="path to fft lut file")
	parser.add_argument('--sample_rate', default=16000, type=int)
	parser.add_argument('--name_suffix', default="", type=str)
	parser.add_argument('--frame_size', required="--params_json" not in sys.argv, type=int,
						help="size in number of samples of one frame")
	parser.add_argument('--frame_step', type=int, default=0,
						help="step in number of samples between two consecutive frames")
	parser.add_argument('--win_func', default="hanning", type=str,
						help="numpy window function (e.g. hanning)")
	parser.add_argument('--gen_inv', action="store_true",
						help="generate the inverse window lut")
	parser.add_argument('--n_fft', default=None, type=int,
						help="number of fft bins")
	parser.add_argument('--fmin', default=20.0, type=float,
						help="mel spectrogram frequency min")
	parser.add_argument('--fmax', default=4000.0, type=float,
						help="mel spectrogram frequency max")
	parser.add_argument('--librosa_mel_norm', default="slaney",
						help="mel spectrogram norm function if librosa")
	parser.add_argument('--mfcc_bank_cnt', default=40 if not "--use_librosa" in sys.argv else 128, type=int,
						help="number of mel filterbanks, must be <= n_dct if n_dct!=0")
	parser.add_argument('--use_tf_mfcc', action='store_true',
						help="use tensorflow filterbank")
	parser.add_argument('--use_librosa', action='store_true',
						help="use librosa filterbank")
	parser.add_argument('--n_dct', default=0, type=int,
						help="number of dct bins, if 0 will not be generated any lut")
	parser.add_argument('--dct_type', default=2, type=int,
						help="DCT algortihm type")
	parser.add_argument('--dct_norm', default=None, type=str,
						help="DCT normalization (default: None)")
	parser.add_argument('--lifter_coeff', default=0, type=int,
						help="Lifter coefficient (default: 0)")
	parser.add_argument('--save_params_header', default=None,
						help="Writes all this parameters in a .h file")
	parser.add_argument('--mel_filterbanks_file', default=None,
						help="Path to a .npy file containing the custom melfilterbanks")
	parser.add_argument('--dct_matrix_file', default=None,
						help="Path to a .npy file containing the custom dct matrix")
	parser.add_argument('--dtype', default="fix16")
	return parser

def main():
	parser = create_parser()
	args = parser.parse_args()

	if args.params_json:
		with open(args.params_json, "r") as f:
			models_params = json.load(f)
	else:
		models_params = {}
	print(models_params)

	fft_lut_file     = args.fft_lut_file     if not "fft_lut_file"	   in models_params else models_params["fft_lut_file"]
	mfcc_bf_lut_file = args.mfcc_bf_lut_file if not "mfcc_bf_lut_file" in models_params else models_params["mfcc_bf_lut_file"]
	use_tf_mfcc      = args.use_tf_mfcc      if not "use_tf_mfcc"	   in models_params else models_params["use_tf_mfcc"]
	use_librosa      = args.use_librosa      if not "use_librosa"	   in models_params else models_params["use_librosa"]
	sample_rate      = args.sample_rate      if not "sample_rate"	   in models_params else models_params["sample_rate"]
	frame_size       = args.frame_size       if not "frame_size"	   in models_params else models_params["frame_size"]
	frame_step       = args.frame_step       if not "frame_step"	   in models_params else models_params["frame_step"]
	window_fn        = args.win_func         if not "win_func"		   in models_params else models_params["win_func"]
	gen_inv		     = args.gen_inv     	 if not "gen_inv"	 	   in models_params else models_params["gen_inv"]
	name_suffix 	 = args.name_suffix      if not "name_suffix"	   in models_params else models_params["name_suffix"]

	n_fft            = args.n_fft            if not "n_fft"		 	   in models_params else models_params["n_fft"]

	fmax             = args.fmax             if not "fmax"			   in models_params else models_params["fmax"]
	fmin             = args.fmin             if not "fmin"		 	   in models_params else models_params["fmin"]
	librosa_mel_norm = args.librosa_mel_norm if not "librosa_mel_norm" in models_params else models_params["librosa_mel_norm"]
	mfcc_bank_cnt    = args.mfcc_bank_cnt    if not "mfcc_bank_cnt"	   in models_params else models_params["mfcc_bank_cnt"]

	n_dct            = args.n_dct            if not "n_dct"			   in models_params else models_params["n_dct"]
	dct_type         = args.dct_type         if not "dct_type"		   in models_params else models_params["dct_type"]
	dct_norm         = args.dct_norm         if not "dct_norm"		   in models_params else models_params["dct_norm"]
	lifter_coeff     = args.lifter_coeff     if not "lifter_coeff"	   in models_params else models_params["lifter_coeff"]

	dtype     		 = args.dtype 		     if not "dtype"	   		   in models_params else models_params["dtype"]

	mel_filterbanks_file = args.mel_filterbanks_file if not "mel_filterbanks_file" in models_params else models_params["mel_filterbanks_file"]
	dct_matrix_file  = args.dct_matrix_file  if not "dct_matrix_file"  in models_params else models_params["dct_matrix_file"]

	lut_dtype = "int" if dtype == "fix16" or dtype == "fix32_scal" else dtype
	n_fft_int = n_fft if dtype == "fix32_scal" else n_fft // 2

	if lut_dtype == "int":
		data_type = "short int"
	elif lut_dtype == "float16":
		data_type = "F16_DSP"
	elif lut_dtype == "float32":
		data_type = "float"
	else:
		raise NotImplemented(f"lut_dtype = {lut_dtype} not implemeted, available ['fix16' 'fix32_scal' 'float32' 'float16']")

	print(lut_dtype, name_suffix, n_fft_int)
	win_func = getattr(np, window_fn)
	win_lut = win_func(frame_size)
	if lut_dtype == "int":
		Window = (win_lut * 2**(15)).astype(np.int16)
	else:
		Window = win_lut.astype(np.float32)

	if gen_inv:
		inv_win_lut = win_lut / (win_lut ** 2 + 1e-8)
		if lut_dtype == "int":
			base, exp = np.frexp(inv_win_lut)
			InvWindow = np.empty(base.size + exp.size)
			InvWindow[0::2] = np.round(base*2**8).astype(np.uint8)
			InvWindow[1::2] = exp.astype(np.uint8)
			InvWindow = InvWindow.astype(np.int32)
		else:
			InvWindow = inv_win_lut.astype(np.float32)

	Twiddles_cos, Twiddles_sin = SetupTwiddlesLUT(n_fft_int, dtype=lut_dtype)
	if round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4):
		SwapTableR4 = SetupSwapTableR4(n_fft_int)
		Twiddles_cosR4, Twiddles_sinR4 = Twiddles_cos[:int(3/4*n_fft_int)], Twiddles_sin[:int(3/4*n_fft_int)]
		print("Setting up twiddles for radix 4 ", len(Twiddles_cosR4))

	SwapTableR2 = SetupSwapTable(n_fft_int)
	Twiddles_cosR2, Twiddles_sinR2 = Twiddles_cos[:int(n_fft_int//2)], Twiddles_sin[:int(n_fft_int//2)]
	print("Setting up twiddles for radix 2 ", len(Twiddles_cosR2))

	RFFTTwiddles_real, RFFTTwiddles_imag = SetupTwiddlesRFFT(n_fft, dtype=lut_dtype)

	if n_dct > 0:
		if dct_matrix_file:
			DCT_Coeff = convert_dtype(np.load(dct_matrix_file), lut_dtype, DCT_TWIDDLE_DYN)
		else:
			DCT_Coeff = SetupDCTTable(n_dct, dct_type, lut_dtype, dct_norm)
		if lifter_coeff > 0:
			Lift_Coeff = SetupLiftCoeff(lifter_coeff, n_dct, lut_dtype)

	################################ WRITE TO FILE #######################################
	Out_str  = array_to_def_c_file(Window, f"WindowLUT{name_suffix}", data_type, frame_size, elem_in_rows=12)
	if gen_inv:
		if dtype == "int":
			Out_str += array_to_def_c_file(InvWindow, f"InvWindowLUT{name_suffix}", "unsigned char", 2*frame_size, elem_in_rows=12)
		else:
			Out_str += array_to_def_c_file(InvWindow, f"InvWindowLUT{name_suffix}", data_type, frame_size, elem_in_rows=12)

	if round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4) and not dtype == "fix32_scal":
		Out_str += array_to_def_c_file(SwapTableR4.astype(np.int16), f"SwapTable{name_suffix}", "short int", n_fft_int, elem_in_rows=2)
	else:
		Out_str += array_to_def_c_file(SwapTableR2.astype(np.int16), f"SwapTable{name_suffix}", "short int", n_fft_int, elem_in_rows=2)

	# FFT
	if dtype == "fix32_scal":
		# only rad2 for fix32_scale
		Out_str += "PI_L2 {} TwiddlesLUT{}[{}] = {{\n".format(data_type, name_suffix, 2*len(Twiddles_cosR2))
		for i in range(len(Twiddles_cosR2)):
			Out_str += "\t {:>6}, {:>6}, \n".format(Twiddles_cosR2[i], Twiddles_sinR2[i])
		Out_str += "\n};\n\n"
	else:
		if round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4):
			Out_str += "PI_L2 {} TwiddlesLUT{}[{}] = {{\n".format(data_type, name_suffix, 2*len(Twiddles_cosR4))
			for i in range(len(Twiddles_cosR4)):
				Out_str += "\t {:>6}, {:>6}, \n".format(Twiddles_cosR4[i], Twiddles_sinR4[i])
			Out_str += "\n};\n\n"
		else:
			Out_str += "PI_L2 {} TwiddlesLUT{}[{}] = {{\n".format(data_type, name_suffix, 2*len(Twiddles_cosR2))
			for i in range(len(Twiddles_cosR2)):
				Out_str += "\t {:>6}, {:>6}, \n".format(Twiddles_cosR2[i], Twiddles_sinR2[i])
			Out_str += "\n};\n\n"
		Out_str += "PI_L2 {} RFFTTwiddlesLUT{}[{}] = {{\n".format(data_type, name_suffix, 2*len(RFFTTwiddles_real))
		for i in range(len(RFFTTwiddles_real)):
			Out_str += "\t {:>6}, {:>6}, \n".format(RFFTTwiddles_real[i], RFFTTwiddles_imag[i])
		Out_str += "\n};\n\n"

	# DCT
	if n_dct > 0:
		Out_str += array_to_def_c_file(DCT_Coeff.flatten(), f"DCT_Coeff{name_suffix}", data_type, n_dct*n_dct, elem_in_rows=n_dct)
		if lifter_coeff > 0:
			Out_str += array_to_def_c_file(Lift_Coeff.flatten(), f"Lift_Coeff{name_suffix}", data_type, n_dct, elem_in_rows=n_dct)

	with open(fft_lut_file, 'w') as f:
		f.write(Out_str)

	if mfcc_bf_lut_file:
		#####################################################################################
		# MFCC
		if mel_filterbanks_file:
			filters = np.load(mel_filterbanks_file)
		elif use_tf_mfcc:
			from SetupLUT import GenMFCC_FB_tf
			filters = GenMFCC_FB_tf(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=lut_dtype)
		elif use_librosa:
			from SetupLUT import GenMFCC_FB_librosa
			filters = GenMFCC_FB_librosa(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, norm=librosa_mel_norm, dtype=lut_dtype)
		else:
			from SetupLUT import GenMFCC_FB
			filters = GenMFCC_FB(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=lut_dtype)

		MfccLUT, HeadCoeff = GenMelFilterBanksCode(filters, mfcc_bank_cnt, fmin, fmax, lut_dtype, data_type, name_suffix)

		with open(mfcc_bf_lut_file, "w") as f:
			f.write(MfccLUT)

	if args.save_params_header:
		with open(args.save_params_header, "w") as f:
			f.write("#define\t{:21}{:>10}\n".format("SAMPLERATE", sample_rate))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_SIZE", frame_size))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_STEP", frame_step))
			f.write("#define\t{:21}{:>10}\n".format("N_FFT", n_fft))
			f.write("#define\t{:21}{:>10}\n".format("DATA_TYPE", 2 if dtype=="float16" else (3 if dtype=="float32" else (1 if dtype=="fix32_scal" else 0))))
			if mfcc_bf_lut_file:
				f.write("#define\t{:21}{:>10}\n".format("MFCC_BANK_CNT", mfcc_bank_cnt))
				f.write("#define\t{:21}{:>10}\n".format("FMIN", fmin))
				f.write("#define\t{:21}{:>10}\n".format("FMAX", fmax))
				f.write("#define\t{:21}{:>10}\n".format("MFCC_COEFF_CNT", HeadCoeff+1))
				f.write("#define\t{:21}{:>10}\n".format("N_DCT", n_dct))


if __name__ == "__main__":
	main()
