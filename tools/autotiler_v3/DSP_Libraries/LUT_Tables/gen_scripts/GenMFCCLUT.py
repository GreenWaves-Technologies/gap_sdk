#!/usr/bin/python
import json
import numpy as np
import math
import sys
import argparse
from SetupLUT import FP2FIX, SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4, SetupDCTTable, SetupLiftCoeff, SetupTwiddlesRFFT, MFCC_COEFF_DYN

def array_to_def_c_file(arr, name, data_type, size, elem_in_rows=2):
	Out_str = ""
	# Window
	Out_str += "PI_L2 {} {}[{}] = {{\n\t".format(data_type, name, size)
	for i, elem in enumerate(arr):
		Out_str += str(elem) + ", "
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
	parser.add_argument('--mfcc_bf_lut_file', required="--params_json" not in sys.argv,
						help="path to fft lut file")
	parser.add_argument('--sample_rate', required="--params_json" not in sys.argv, type=int)
	parser.add_argument('--frame_size', required="--params_json" not in sys.argv, type=int,
						help="size in number of samples of one frame")
	parser.add_argument('--frame_step', required="--params_json" not in sys.argv, type=int,
						help="step in number of samples between two consecutive frames")
	parser.add_argument('--win_func', default="hanning", type=str,
						help="numpy window function (e.g. hanning)")
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
	parser.add_argument('--lifter_coeff', default=0, type=int,
						help="Lifter coefficient (default: 0)")
	parser.add_argument('--save_params_header', default=None,
						help="Writes all this parameters in a .h file")
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

	n_fft            = args.n_fft            if not "n_fft"		 	   in models_params else models_params["n_fft"]

	fmax             = args.fmax             if not "fmax"			   in models_params else models_params["fmax"]
	fmin             = args.fmin             if not "fmin"		 	   in models_params else models_params["fmin"]
	librosa_mel_norm = args.librosa_mel_norm if not "librosa_mel_norm" in models_params else models_params["librosa_mel_norm"]
	mfcc_bank_cnt    = args.mfcc_bank_cnt    if not "mfcc_bank_cnt"	   in models_params else models_params["mfcc_bank_cnt"]

	n_dct            = args.n_dct            if not "n_dct"			   in models_params else models_params["n_dct"]
	dct_type         = args.dct_type         if not "dct_type"		   in models_params else models_params["dct_type"]
	lifter_coeff     = args.lifter_coeff     if not "lifter_coeff"	   in models_params else models_params["lifter_coeff"]

	dtype     		 = args.dtype 		     if not "dtype"	   		   in models_params else models_params["dtype"]

	lut_dtype = "int" if dtype == "fix16" or dtype == "fix32_scal" else dtype
	n_fft_int = n_fft if dtype == "fix32_scal" else n_fft // 2

	if lut_dtype == "int":
		data_type = "short int"
	elif lut_dtype == "float16":
		data_type = "f16"
	elif lut_dtype == "float32":
		data_type = "float"
	else:
		raise NotImplemented(f"lut_dtype = {lut_dtype} not implemeted, available 'int' 'float32' 'float16'")

	print(lut_dtype)
	win_func = getattr(np, window_fn)
	if lut_dtype == "int":
		Window = (win_func(frame_size) * 2**(15)).astype(np.int16)
	else:
		Window = win_func(frame_size).astype(np.float16)

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
		DCT_Coeff = SetupDCTTable(n_dct, dct_type, lut_dtype)
		if lifter_coeff > 0:
			Lift_Coeff = SetupLiftCoeff(lifter_coeff, n_dct)

	################################ WRITE TO FILE #######################################
	Out_str  = array_to_def_c_file(Window, "WindowLUT", data_type, frame_size, elem_in_rows=12)
	Out_str += array_to_def_c_file(SwapTableR2.astype(np.int16), "SwapTableR2", "short int", n_fft_int, elem_in_rows=2)
	if round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4):
		Out_str += array_to_def_c_file(SwapTableR4.astype(np.int16), "SwapTableR4", "short int", n_fft_int, elem_in_rows=2)

	# FFT
	Out_str += "PI_L2 {} TwiddlesLUTR2[{}] = {{\n".format(data_type, 2*len(Twiddles_cosR2))
	for i in range(len(Twiddles_cosR2)):
		Out_str += "\t {}, {}, \n".format(Twiddles_cosR2[i], Twiddles_sinR2[i])
	Out_str += "\n};\n\n"
	if not dtype == "fix32_scal":
		if round(math.log(n_fft_int, 4)) == math.log(n_fft_int, 4):
			Out_str += "PI_L2 {} TwiddlesLUTR4[{}] = {{\n".format(data_type, 2*len(Twiddles_cosR2))
			for i in range(len(Twiddles_cosR4)):
				Out_str += "\t {}, {}, \n".format(Twiddles_cosR4[i], Twiddles_sinR4[i])
			Out_str += "\n};\n\n"
		Out_str += "PI_L2 {} RFFTTwiddlesLUT[{}] = {{\n".format(data_type, 2*len(RFFTTwiddles_real))
		for i in range(len(RFFTTwiddles_real)):
			Out_str += "\t {}, {}, \n".format(RFFTTwiddles_real[i], RFFTTwiddles_imag[i])
		Out_str += "\n};\n\n"

	# DCT
	if n_dct > 0:
		Out_str += array_to_def_c_file(DCT_Coeff.flatten(), "DCT_Coeff", data_type, n_dct*n_dct, elem_in_rows=n_dct)
		if lifter_coeff > 0:
			Out_str += array_to_def_c_file(Lift_Coeff.flatten(), "Lift_Coeff", data_type, n_dct, elem_in_rows=6)

	with open(fft_lut_file, 'w') as f:
		f.write(Out_str)

	#####################################################################################
	# MFCC
	if use_tf_mfcc:
		from SetupLUT import GenMFCC_FB_tf
		filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB_tf(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=lut_dtype)
	elif use_librosa:
		from SetupLUT import GenMFCC_FB_librosa
		filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB_librosa(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, norm=librosa_mel_norm, dtype=lut_dtype)
	else:
		from SetupLUT import GenMFCC_FB
		filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=lut_dtype)

	Out_str =  "#define MFCC_COEFF_CNT\t{}\n\n".format(HeadCoeff+1)
	Out_str += "/* Filter Bank bands:\n\n"
	Out_str += "\tMinimum Frequency: {} Hz\n".format(fmin)
	Out_str += "\tMaximum Frequency: {} Hz*/\n\n".format(fmax)

	Out_str += "PI_L2 fbank_type_t MFCC_FilterBank[{}] = {{\n".format(mfcc_bank_cnt)
	HeadCoeff = 0
	for i, filt in enumerate(filters):
		if np.all(filt == 0):
			Start = 0
			Stop = 0
			Base = HeadCoeff
			Items = 0
		else:
			Start = np.argmax(filt!=0)
			Stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
			Base = HeadCoeff
			Items = Stop - Start + 1

		Out_str += "\t{{{:>4},{:>4},{:>4}}},\n".format(Start, Items, Base)
		HeadCoeff += Items
	Out_str += "};\n\n"

	Out_str += "PI_L2 {} MFCC_Coeffs[{}] = {{\n\t".format(data_type, HeadCoeff+1)
	for i, coeff in enumerate(MFCC_Coeff):
		Out_str += "{:>5}".format(str(coeff)) + ", "
		if (i+1) % 15 == 0:
			Out_str += "\n\t"
	# Add a last 0 coeff
	Out_str += "{:>5}\n}};\n".format(0)

	with open(mfcc_bf_lut_file, "w") as f:
		f.write(Out_str)

	if args.save_params_header:
		with open(args.save_params_header, "w") as f:
			f.write("#define\t{:21}{:>10}\n".format("SAMPLERATE", sample_rate))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_SIZE", frame_size))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_STEP", frame_step))
			f.write("#define\t{:21}{:>10}\n".format("N_FFT", n_fft))
			f.write("#define\t{:21}{:>10}\n".format("DATA_TYPE", 2 if dtype=="float16" else (3 if dtype=="float32" else (1 if dtype=="fix32_scal" else 0))))
			f.write("#define\t{:21}{:>10}\n".format("MFCC_BANK_CNT", mfcc_bank_cnt))
			f.write("#define\t{:21}{:>10}\n".format("FMIN", fmin))
			f.write("#define\t{:21}{:>10}\n".format("FMAX", fmax))
			f.write("#define\t{:21}{:>10}\n".format("MFCC_COEFF_CNT", HeadCoeff+1))
			f.write("#define\t{:21}{:>10}\n".format("N_DCT", n_dct))


if __name__ == "__main__":
	main()
