import os
import json
import numpy as np
import math
from bfloat16 import bfloat16
import sys
import argparse
from SetupLUT import SetupTwiddlesLUT, SetupSwapTable, SetupSwapTableR4, SetupDCTTable, SetupLiftCoeff, SetupTwiddlesRFFT, SparseMelFilterBanksCode, FP2FIX

WIN_LUT_Q = 15
FFT_TWIDDLE_Q = 15
MEL_COEFF_Q = 15
DCT_TWIDDLE_Q = 7
LIFT_COEFF_Q = 11
TYPE_TO_CDEF = {
	"int": "signed short int",
	"fix16": "signed short int",
	"fix32_scal": "signed short int",
	"float16": "F16_DSP",
	"bfloat16": "F16_DSP",
	"float32": "float"
}
TYPE_TO_NP = {
	"int": np.int16,
	"fix16": np.int16,
	"fix32_scal": np.int16,
	"float16": np.float16,
	"bfloat16": bfloat16,
	"float32": np.float64
}

def next_pow2(x):
	return 2**math.ceil(math.log2(x))

def WriteToFile(array, filepath, as_text=False, dtype="float32", quantize=False, q_precision=None, new_line=10):
	if quantize:
		array = FP2FIX(array, q_precision)

	array = array.astype(TYPE_TO_NP[dtype])
	if as_text:
		array = array.flatten()
		name = os.path.split(filepath)[1]
		Out_str = "PI_L2 {} {}[{}] = {{\n\t".format(TYPE_TO_CDEF[dtype], name, array.size)
		for i, elem in enumerate(array):
			Out_str += f"{elem:>8}, "
			if (i+1)%new_line == 0:
				Out_str += "\n\t"
		Out_str += "\n}; \n"
		with open((filepath + ".def"), "w") as f:
			f.write(Out_str)
	else:
		array.tofile(filepath + ".bin")

def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='mfcc_lut_gen')

	parser.add_argument('params_json', default=None,
						help="Path to .json file where to get parameters")
	parser.add_argument('--build_dir', default=None, required=True,
						help="Path to directory where to store luts")
	parser.add_argument('--save_text', action="store_true",
						help="Save luts in .def file as text --> Data in L2 always")
	parser.add_argument('--save_params_header', default=None,
						help="Writes all this parameters in a .h file")
	return parser

def main():
	parser = create_parser()
	args = parser.parse_args()

	#######################################################################################

	build_dir = args.build_dir
	with open(args.params_json, "r") as f:
		models_params = json.load(f)
	assert "frame_size" in list(models_params.keys())
	# Get basic sizes information
	frame_size = models_params["frame_size"]
	window_size = models_params.get("window_size", frame_size)

	dtype = models_params.get("dtype", "float32")

	n_fft = models_params.get("n_fft", int(next_pow2(frame_size)))
	real_n_fft = n_fft if dtype == "fix32_scal" else n_fft // 2

	sample_rate = models_params.get("sample_rate", 16000)
	mel_filterbanks_file = models_params.get("mel_filterbanks_file", None)
	fbank_type = models_params.get("fbank_type", None)
	use_tf_mel = fbank_type == "tensorflow" or models_params.get("use_tf_mel", False)
	use_librosa_mel = fbank_type == "librosa" or models_params.get("use_librosa", False)
	if use_tf_mel:
		fmax = models_params.get("fmax", 3800.0)
		fmin = models_params.get("fmin", 125.0)
		n_mels = models_params.get("n_mels", models_params.get("mfcc_bank_cnt", 20))
	elif use_librosa_mel:
		fmax = models_params.get("fmax", None)
		fmin = models_params.get("fmin", 0.0)
		n_mels = models_params.get("n_mels", models_params.get("mfcc_bank_cnt", 128))
		librosa_mel_norm = models_params.get("librosa_mel_norm", "slaney")
	else:
		fmax = models_params.get("fmax", sample_rate//2)
		fmin = models_params.get("fmin", 20.0)
		n_mels = models_params.get("n_mels", models_params.get("mfcc_bank_cnt", 0))

	n_dct = models_params.get("n_dct", 0)
	dct_type = models_params.get("dct_type", 2)
	dct_norm = models_params.get("dct_norm", None)
	lifter_coeff = models_params.get("lifter_coeff", 0)

	gen_inv_win = models_params.get("gen_inv_lut")
	gen_inv_mel = models_params.get("gen_inv_mel")

	#######################################################################################

	# Frames Windowing
	window_fn = models_params.get("win_func", None)
	if window_fn is not None:
		win_func = getattr(np, window_fn)
		win_lut = win_func(window_size)
		if window_size < frame_size:
			pad_before = int((frame_size - window_size) // 2)
			pad_after = int((frame_size - window_size) - pad_before)
			win_lut = np.pad(win_lut, (pad_before, pad_after))
		WriteToFile(win_lut, build_dir+"/WindowLUT", args.save_text, dtype,
					quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=WIN_LUT_Q)
		if gen_inv_win:
			inv_win_lut = win_lut / (win_lut ** 2 + 1e-8)
			if dtype in ["int", "fix16", "fix32_scal"]:
				base, exp = np.frexp(inv_win_lut)
				inv_win_lut = np.empty(base.size + exp.size)
				inv_win_lut[0::2] = np.round(base*2**8).astype(np.uint8)
				inv_win_lut[1::2] = exp.astype(np.uint8)
				inv_win_lut = inv_win_lut.astype(np.uint8)
			WriteToFile(inv_win_lut, build_dir+"/InvWindowLUT", args.save_text, dtype, quantize=False)

	# FFT Twiddles
	Twiddles_real, Twiddles_imag = SetupTwiddlesLUT(real_n_fft)
	if round(math.log(real_n_fft, 4)) == math.log(real_n_fft, 4) and dtype != "fix32_scal":
		SwapTable = SetupSwapTableR4(real_n_fft)
		Twiddles_real, Twiddles_imag = Twiddles_real[:int(3/4*real_n_fft)], Twiddles_imag[:int(3/4*real_n_fft)]
		print("Setting up twiddles for radix 4 ", len(Twiddles_real))
	else:
		SwapTable = SetupSwapTable(real_n_fft)
		Twiddles_real, Twiddles_imag = Twiddles_real[:int(real_n_fft//2)], Twiddles_imag[:int(real_n_fft//2)]
		print("Setting up twiddles for radix 2 ", len(Twiddles_real))

	WriteToFile(SwapTable.astype(np.int16), build_dir+"/SwapTable", args.save_text, dtype="int", quantize=False)

	Twiddles = np.empty(len(Twiddles_real) + len(Twiddles_imag))
	Twiddles[::2] = Twiddles_real
	Twiddles[1::2] = Twiddles_imag
	WriteToFile(Twiddles, build_dir+"/FFTTwiddles", args.save_text, dtype,
				quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=FFT_TWIDDLE_Q)

	# Real FFT Twiddles
	if dtype != "fix32_scal":
		RFFTTwiddles_real, RFFTTwiddles_imag = SetupTwiddlesRFFT(n_fft)
		RFFTTwiddles = np.empty(len(RFFTTwiddles_real) + len(RFFTTwiddles_imag))
		RFFTTwiddles[::2] = RFFTTwiddles_real
		RFFTTwiddles[1::2] = RFFTTwiddles_imag
		WriteToFile(RFFTTwiddles, build_dir+"/RFFTTwiddles", args.save_text, dtype,
					quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=FFT_TWIDDLE_Q)

	# Mel Filterbanks
	if n_mels > 0 or mel_filterbanks_file is not None:
		if mel_filterbanks_file:
			filters = np.load(mel_filterbanks_file)
			n_mels = filters.shape[0]
		elif use_tf_mel:
			from SetupLUT import SetupMelFBanksTf
			filters = SetupMelFBanksTf(n_fft, n_mels, sample_rate=sample_rate, Fmin=fmin, Fmax=fmax)
		elif use_librosa_mel:
			from SetupLUT import SetupMelFBanksLibrosa
			filters = SetupMelFBanksLibrosa(n_fft, n_mels, sample_rate=sample_rate, Fmin=fmin, Fmax=fmax, norm=librosa_mel_norm)
		else:
			from SetupLUT import SetupMelFBanks
			filters = SetupMelFBanks(n_fft, n_mels, sample_rate=sample_rate, Fmin=fmin, Fmax=fmax)
		sparse_filters, sparsity_fb, NCoeffMEL = SparseMelFilterBanksCode(filters, quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=MEL_COEFF_Q)
		WriteToFile(sparse_filters, build_dir+"/MelFBCoeff", args.save_text, dtype, quantize=False)
		WriteToFile(sparsity_fb, build_dir+"/MelFBSparsity", args.save_text, "fix16", quantize=False, new_line=3)

		if gen_inv_mel:
			# Inverse matrix of filterbank generated with least squares algorithm
			# A.T*b = A.T*A*x^
			# x^ = (A.T*A)^-1 * A.T * b
			inverse_mel_fb = np.matmul(np.linalg.inv(np.matmul(filters, filters.T)), filters)
			imel_min, imel_max = np.abs(np.min(inverse_mel_fb)), np.abs(np.max(inverse_mel_fb))
			IMEL_COEFF_Q = 15 - int(np.ceil(np.log2(np.maximum(imel_min, imel_max))))
			WriteToFile(inverse_mel_fb, build_dir+"/IMelFBCoeff", args.save_text, dtype, quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=IMEL_COEFF_Q, new_line=inverse_mel_fb.shape[1])

	# DCT Matrix and Lifter
	if n_dct > 0:
		DCTCoeff = SetupDCTTable(n_dct, dct_type, dct_norm)
		WriteToFile(DCTCoeff, build_dir+"/DCTTwiddles", args.save_text, dtype,
					quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=DCT_TWIDDLE_Q)
		if lifter_coeff > 0:
			LiftCoeff = SetupLiftCoeff(lifter_coeff, n_dct)
			WriteToFile(LiftCoeff, build_dir+"/LiftCoeff", args.save_text, dtype,
						quantize=dtype in ["int", "fix16", "fix32_scal"], q_precision=LIFT_COEFF_Q)

	if args.save_params_header:
		with open(args.save_params_header, "w") as f:
			f.write("#define\t{:21}{:>10}\n".format("SAMPLERATE", sample_rate))
			f.write("#define\t{:21}{:>10}\n".format("FRAME_SIZE", frame_size))
			if models_params.get("frame_step"):
				f.write("#define\t{:21}{:>10}\n".format("FRAME_STEP", models_params["frame_step"]))
			f.write("#define\t{:21}{:>10}\n".format("N_FFT", n_fft))
			f.write("#define\t{:21}{:>10}\n".format("DATA_TYPE", 2 if dtype=="float16" else (3 if dtype=="float32" else (1 if dtype=="fix32_scal" else 0))))
			if n_mels > 0:
				f.write("#define\t{:21}{:>10}\n".format("N_MELS", n_mels))
				f.write("#define\t{:21}{:>10}\n".format("FMIN", fmin))
				f.write("#define\t{:21}{:>10}\n".format("FMAX", fmax))
				f.write("#define\t{:21}{:>10}\n".format("MEL_COEFF_CNT", NCoeffMEL+1))
				f.write("#define\t{:21}{:>10}\n".format("N_DCT", n_dct))
				if gen_inv_mel and dtype in ["int", "fix16", "fix32_scal"]:
					f.write("#define\t{:21}{:>10}\n".format("IMEL_COEFF_Q", IMEL_COEFF_Q))

if __name__ == "__main__":
	main()
