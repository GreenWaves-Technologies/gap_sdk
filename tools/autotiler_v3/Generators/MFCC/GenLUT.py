#!/usr/bin/python
import json
import numpy as np
import sys
import argparse
from SetupLUT import FP2FIX, SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4, SetupDCTTable, SetupLiftCoeff, MFCC_COEFF_DYN


def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='mfcc_lut_gen')

	parser.add_argument('--params_json', default=None,
						help="Path to .json file where to get parameters")
	parser.add_argument('--fft_lut_file', required="--params_json" not in sys.argv,
						help="path to fft lut file")
	parser.add_argument('--mfcc_bf_lut_file', required="--params_json" not in sys.argv and "--only_stft" not in sys.argv,
						help="path to fft lut file")
	parser.add_argument('--sample_rate', required="--params_json" not in sys.argv, type=int)
	parser.add_argument('--frame_size', required="--params_json" not in sys.argv, type=int,
						help="size in number of samples of one frame")
	parser.add_argument('--frame_step', required="--params_json" not in sys.argv, type=int,
						help="step in number of samples between two consecutive frames")
	parser.add_argument('--n_frame', type=int, default=1,
						help="How many frame the kernel must process, default=1")
	parser.add_argument('--preempfactor', default=0.0, type=float,
						help="preemphasis factor, default=0")
	parser.add_argument('--n_fft', default=512 if not "--use_librosa" in sys.argv else 2048, type=int,
						help="number of fft bins")
	parser.add_argument('--use_radix_4', action='store_true',
						help="use radix 4 fft")
	parser.add_argument('--use_high_prec', action='store_true',
						help="use fake float fft and mfcc")
	parser.add_argument('--use_power', action='store_true',
						help="If MFCC computes over |STFT|^2, else over |STFT|")
	parser.add_argument('--n_dct', default=0, type=int,
						help="number of dct bins, if 0 will not be generated any lut")
	parser.add_argument('--dct_type', default=2, type=int,
						help="DCT algortihm type")
	parser.add_argument('--lifter_coeff', default=0, type=int,
						help="Lifter coefficient (default: 0)")
	parser.add_argument('--win_func', default="hanning", type=str,
						help="numpy window function (e.g. hanning)")
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
	parser.add_argument('--use_db', action='store_true',
						help="Compute 10*log10(mel_spectrogram) instead of ln(mel_spectrogram)")
	parser.add_argument('--out_melspectr', action='store_true',
						help="Get the melspectrogram output (no log), n_dct must be =0")
	parser.add_argument('--only_stft', action='store_true',
						help="Generate LUT only for STFT computation")
	parser.add_argument('--out_fft', action='store_true',
						help="Output only fft without any spectrum function")
	parser.add_argument('--save_params_header', default=None,
						help="Writes all this parameters in a .h file")
	parser.add_argument('--dtype', default="int")
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
	only_stft        = args.only_stft        if not "only_stft"        in models_params else models_params["only_stft"]
	mfcc_bf_lut_file = args.mfcc_bf_lut_file if not "mfcc_bf_lut_file" in models_params else models_params["mfcc_bf_lut_file"]
	use_tf_mfcc      = args.use_tf_mfcc      if not "use_tf_mfcc"	   in models_params else models_params["use_tf_mfcc"]
	use_librosa      = args.use_librosa      if not "use_librosa"	   in models_params else models_params["use_librosa"]
	use_db		     = args.use_db 		     if not "use_db"		   in models_params else models_params["use_db"]
	sample_rate      = args.sample_rate      if not "sample_rate"	   in models_params else models_params["sample_rate"]
	frame_size       = args.frame_size       if not "frame_size"	   in models_params else models_params["frame_size"]
	frame_step       = args.frame_step       if not "frame_step"	   in models_params else models_params["frame_step"]
	n_frame          = args.n_frame          if not "n_frame"		   in models_params else models_params["n_frame"]

	preempfactor     = args.preempfactor     if not "preempfactor"	   in models_params else models_params["preempfactor"]
	window_fn        = args.win_func         if not "win_func"		   in models_params else models_params["win_func"]

	n_fft            = args.n_fft            if not "n_fft"		 	   in models_params else models_params["n_fft"]
	out_fft		     = args.out_fft          if not "out_fft"   	   in models_params else models_params["out_fft"]
	use_power        = args.use_power        if not "use_power"		   in models_params else models_params["use_power"]
	use_radix_4      = args.use_radix_4      if not "use_radix_4"	   in models_params else models_params["use_radix_4"]
	use_high_prec    = args.use_high_prec    if not "use_high_prec"	   in models_params else models_params["use_high_prec"]

	fmax             = args.fmax             if not "fmax"			   in models_params else models_params["fmax"]
	fmin             = args.fmin             if not "fmin"		 	   in models_params else models_params["fmin"]
	librosa_mel_norm = args.librosa_mel_norm if not "librosa_mel_norm" in models_params else models_params["librosa_mel_norm"]
	mfcc_bank_cnt    = args.mfcc_bank_cnt    if not "mfcc_bank_cnt"	   in models_params else models_params["mfcc_bank_cnt"]

	out_melspectr    = args.out_melspectr    if not "out_melspectr"	   in models_params else models_params["out_melspectr"]
	n_dct            = args.n_dct            if not "n_dct"			   in models_params else models_params["n_dct"]
	dct_type         = args.dct_type         if not "dct_type"		   in models_params else models_params["dct_type"]
	lifter_coeff     = args.lifter_coeff     if not "lifter_coeff"	   in models_params else models_params["lifter_coeff"]

	dtype     		 = args.dtype 		     if not "dtype"	   		   in models_params else models_params["dtype"]

	if dtype == "int":
		data_type = "short int"
	elif dtype == "float16":
		data_type = "f16"
	elif dtype == "float32":
		data_type = "float"
	else:
		raise NotImplemented()

	print(dtype)
	win_func = getattr(np, window_fn)
	if dtype == "int":
		Window = (win_func(frame_size) * 2**(15)).astype(np.int16)
	else:
		Window = win_func(frame_size).astype(np.float16)

	Twiddles_cos, Twiddles_sin = SetupTwiddlesLUT(n_fft, dtype=dtype)
	if use_radix_4:
		SwapTable = SetupSwapTableR4(n_fft)
		Twiddles_cos, Twiddles_sin = Twiddles_cos[:int(3/4*n_fft)], Twiddles_sin[:int(3/4*n_fft)]
		print("Setting up twiddles for radix 4 ", len(Twiddles_cos))
	else:
		SwapTable = SetupSwapTable(n_fft)
		Twiddles_cos, Twiddles_sin = Twiddles_cos[:int(n_fft//2)], Twiddles_sin[:int(n_fft//2)]
		print("Setting up twiddles for radix 2 ", len(Twiddles_cos))

	print(SwapTable.shape)
	if n_dct > 0:
		DCT_Coeff = SetupDCTTable(n_dct, dct_type, dtype)
		if lifter_coeff > 0:
			Lift_Coeff = SetupLiftCoeff(lifter_coeff, n_dct)

	################################ WRITE TO FILE #######################################
	Out_str = ""
	# Window
	Out_str += "PI_L2 {} WindowLUT[{}] = {{\n\t".format(data_type, frame_size)
	for i, elem in enumerate(Window):
		Out_str += str(elem) + ", "
		if (i+1)%12 == 0:
			Out_str += "\n\t"
	Out_str += "\n}; \n"

	# FFT 
	Out_str += "PI_L2 {} TwiddlesLUT[{}] = {{\n".format(data_type, 2*len(Twiddles_cos))
	for i in range(len(Twiddles_cos)):
		Out_str += "\t {}, {}, \n".format(Twiddles_cos[i], Twiddles_sin[i])
	Out_str += "\n};\n\n"

	Out_str += "PI_L2 short int SwapLUT[{}] = {{\n\t".format(n_fft)
	for i, swap in enumerate(SwapTable):
		Out_str += str(int(swap)) + ", "
		if (i+1)%13 == 0:
			Out_str += "\n\t"
	Out_str += "\n};\n "

	# DCT
	if n_dct > 0:
		Out_str += "PI_L2 {} DCT_Coeff[{}*{}] = {{\n\t".format(data_type, n_dct, n_dct)
		for k in range(n_dct):
			for i in range(n_dct):
				Out_str += "{}, ".format(DCT_Coeff[k, i])
			Out_str += "\n\t"
		Out_str += "};\n"
		if lifter_coeff > 0:
			Out_str += "PI_L2 short int Lift_Coeff[{}] = {{\n\t".format(n_dct)
			for i in range(n_dct):
				Out_str += "{}, ".format(int(Lift_Coeff[i]))
				if (i+1)%6 == 0:
					Out_str += "\n\t"
			Out_str += "};\n"

	with open(fft_lut_file, 'w') as f:
		f.write(Out_str)
	#####################################################################################

	if not only_stft:
		# MFCC
		if use_tf_mfcc:
			from SetupLUT import GenMFCC_FB_tf
			filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB_tf(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=dtype)
		elif use_librosa:
			from SetupLUT import GenMFCC_FB_librosa
			filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB_librosa(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, norm=librosa_mel_norm, dtype=dtype)
		else:
			from SetupLUT import GenMFCC_FB
			filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB(n_fft, mfcc_bank_cnt, Fmin=fmin, Fmax=fmax, sample_rate=sample_rate, dtype=dtype)

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
			f.write("#define\t{:21}{:>10}\n".format("PREEMP_FACTOR", preempfactor))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_SIZE", frame_size))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_STEP", frame_step))
			f.write("#define\t{:21}{:>10}\n".format("N_FRAME", n_frame))
			f.write("#define\t{:21}{:>10}\n".format("N_FFT", n_fft))
			f.write("#define\t{:21}{:>10}\n".format("USE_RADIX_4", 1 if use_radix_4 else 0))
			f.write("#define\t{:21}{:>10}\n".format("DATA_TYPE", 1 if use_high_prec else (2 if dtype=="float16" else (3 if dtype=="float32" else 0))))
			f.write("#define\t{:21}{:>10}\n".format("USE_POWER", 1 if use_power else 0))
			f.write("#define\t{:21}{:>10}\n".format("OUT_FFT", 1 if out_fft else 0))
			if not only_stft:
				f.write("#define\t{:21}{:>10}\n".format("MFCC_BANK_CNT", mfcc_bank_cnt))
				f.write("#define\t{:21}{:>10}\n".format("FMIN", fmin))
				f.write("#define\t{:21}{:>10}\n".format("FMAX", fmax))
				f.write("#define\t{:21}{:>10}\n".format("MFCC_COEFF_CNT", HeadCoeff+1))
				f.write("#define\t{:21}{:>10}\n".format("N_DCT", n_dct))
				f.write("#define\t{:21}{:>10}\n".format("OUTPUT_MELSPECTROGRAM", 1 if out_melspectr else 0))
				f.write("#define\t{:21}{:>10}\n".format("LIFTER_COEFF", lifter_coeff))
				f.write("#define\t{:21}{:>10}\n".format("USE_DB", 1 if use_db else 0))
			

if __name__ == "__main__":
	main()
