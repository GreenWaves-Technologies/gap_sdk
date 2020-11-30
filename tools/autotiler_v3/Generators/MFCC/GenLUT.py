#!/usr/bin/python
import numpy as np
import sys
import json
USE_TF_FB = False
if USE_TF_FB:
	import tensorflow as tf

with open("MFCC_params.json", "r") as f:
	params = json.load(f)
with open("MFCC_params.h", "w") as f:
	for k, v in params.items():
		f.write("#define\t{:16}\t{}\n".format(k, int(v) if k != "PREEMP_FACTOR" else float(v)))

LUT_FILE = "BUILD_MODEL/LUT.def"
FB_FILE = "BUILD_MODEL/MFCC_FB.def"
WINDOW = "HANNING"
FFT_TWIDDLE_DYN = 15
MFCC_COEFF_DYN = 10

def FP2FIX(Val, Prec):
    try:
        return (Val * ((1 << Prec) - 1)).astype(np.int32)
    except:
        return int(Val * ((1 << Prec) - 1))

def SetupTwiddlesLUT(Nfft, Inverse=False):
	Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft)
	if Inverse:
		Twiddles_cos = np.round(np.cos(Phi) * ((1<<FFT_TWIDDLE_DYN)-1))
		Twiddles_sin = np.round(np.sin(Phi) * ((1<<FFT_TWIDDLE_DYN)-1))
	else:
		Twiddles_cos = np.round(np.cos(-Phi) * ((1<<FFT_TWIDDLE_DYN)-1))
		Twiddles_sin = np.round(np.sin(-Phi) * ((1<<FFT_TWIDDLE_DYN)-1))
	return Twiddles_cos, Twiddles_sin

def SetupSwapTable(Ni):
	log2 = int(np.log2(Ni))
	iL = Ni / 2
	iM = 1
	SwapTable = np.zeros(Ni)
	for i in range(log2):
		for j in range(iM):
			SwapTable[j + iM] = SwapTable[j] + iL
		iL /= 2
		iM *= 2
	return SwapTable

def SetupDCTTable(Ndct):
	DCT_Coeff = np.zeros((Ndct, Ndct))
	for k in range(Ndct):
		for i in range(Ndct):
			DCT_Coeff[k, i] = np.round(np.cos(np.pi / Ndct * k * (i + 0.5)) * ((1<<FFT_TWIDDLE_DYN)-1))
	return DCT_Coeff

def SetupLiftCoeff(L, N, Qdyn=11):
	Lift_Coeff = np.zeros(N)
	for i in range(N):
		Lift_Coeff[i] = ((1.0 + (L / 2.0) * np.sin(np.pi * i / L)) * (1 << Qdyn)).astype(np.int16)
	return Lift_Coeff


def GenMFCC_FB_tf(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000):
	# Generate Mel Filterbanks matrix with tensorflow functions:
	# https://www.tensorflow.org/api_docs/python/tf/signal/mfccs_from_log_mel_spectrograms
	num_spectrogram_bins = Nfft//2 + 1
	lower_edge_hertz, upper_edge_hertz, num_mel_bins = Fmin, Fmax, Nbanks
	linear_to_mel_weight_matrix = tf.signal.linear_to_mel_weight_matrix(
	  num_mel_bins, num_spectrogram_bins, sample_rate, lower_edge_hertz,
	  upper_edge_hertz)

	linear_to_mel_weights = tf.Session().run(linear_to_mel_weight_matrix)

	HeadCoeff = 0
	MFCC_Coeff = []
	for i, filt in enumerate(linear_to_mel_weights.T):
		Start = np.argmax(filt!=0)
		Stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
		Base = HeadCoeff
		Items = Stop - Start + 1
		print("Filter {}: Start: {} Stop: {} Base: {} Items: {}".format(i, Start, Stop, Base, Items))
		for j in range(Items):
			MFCC_Coeff.append(FP2FIX(filt[Start+j], MFCC_COEFF_DYN))
		HeadCoeff += Items
	return linear_to_mel_weights.T, np.array(MFCC_Coeff), HeadCoeff


def Mel(k):
  	return 1125 * np.log(1 + k/700.0)
def InvMel(k):
    return 700.0*(np.exp(k/1125.0) - 1.0)

def GenMFCC_FB(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000):
	# Generate Mel Filterbank
	# http://practicalcryptography.com/miscellaneous/machine-learning/guide-mel-frequency-cepstral-coefficients-mfccs/
	SamplePeriod = 1.0/sample_rate;
	FreqRes = (SamplePeriod*Nfft*700.0);
	Step = ((float) (Mel(Fmax)-Mel(Fmin)))/(Nbanks+1);
	Fmel0 = Mel(Fmin);

	f = [None] * (Nbanks+2)
	for i in range(Nbanks+2):
		f[i] = int(((Nfft+1)*InvMel(Fmel0+i*Step))//sample_rate)
		print( "f[%d] = %d" % (i, f[i]))

	filters = np.zeros((Nbanks, Nfft // 2))
	for i in range(1, Nbanks+1):
		for k in range(f[i-1], f[i]):
			filters[i-1][k] = np.float(k-f[i-1])/(f[i]-f[i-1])
		for k in range(f[i], f[i+1]):
			filters[i-1][k] = np.float(f[i+1]-k)/(f[i+1]-f[i])

	HeadCoeff = 0
	MFCC_Coeff = []
	for i, filt in enumerate(filters):
		Start = np.argmax(filt!=0)
		Stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
		Base = HeadCoeff
		Items = Stop - Start + 1
		print("Filter {}: Start: {} Stop: {} Base: {} Items: {}".format(i, Start, Stop, Base, Items))
		for j in range(Items):
			MFCC_Coeff.append(FP2FIX(filt[Start+j], MFCC_COEFF_DYN))
		HeadCoeff += Items

	return filters, MFCC_Coeff, HeadCoeff


def main():
	Window = (np.hanning(params['FRAME_SIZE']) * 2**(15)).astype(np.int16)
	if params['USE_RADIX_4']:
		Twiddles_cos, Twiddles_sin = SetupTwiddlesLUT(3/4 * params['N_FFT'])
	else:
		Twiddles_cos, Twiddles_sin = SetupTwiddlesLUT(params['N_FFT'])
	SwapTable = SetupSwapTable(params['N_FFT'])
	print(SwapTable.shape)
	if "N_DCT" in params.keys() and params["N_DCT"] > 0:
		DCT_Coeff = SetupDCTTable(params['N_DCT'])
		if "LIFTER_COEFF" in params.keys() and params["LIFTER_COEFF"] > 0:
			Lift_Coeff = SetupLiftCoeff(params["LIFTER_COEFF"], params["N_DCT"])


	Out_str = ""

	# Window
	Out_str += " short int WindowLUT[{}] = {{\n\t".format(params['FRAME_SIZE'])
	for i, elem in enumerate(Window):
		Out_str += str(elem) + ", "
		if (i+1)%12 == 0:
			Out_str += "\n\t"
	Out_str += "\n}; \n"

	# FFT 
	Out_str += " short int TwiddlesLUT[{}] = {{\n".format(params['N_FFT'])
	for i in range(params['N_FFT'] // 2):
		Out_str += "\t {}, {}, \n".format(int(Twiddles_cos[i]), int(Twiddles_sin[i]))
	Out_str += "\n};\n\n"

	Out_str += " short int SwapLUT[{}] = {{\n\t".format(params['N_FFT'])
	for i, swap in enumerate(SwapTable):
		Out_str += str(int(swap)) + ", "
		if (i+1)%13 == 0:
			Out_str += "\n\t"
	Out_str += "\n};\n "

	# DCT
	if "N_DCT" in params.keys() and params["N_DCT"] > 0:
		Out_str += " short int DCT_Coeff[{}*{}] = {{\n\t".format(params['N_DCT'], params['N_DCT'])
		for k in range(params['N_DCT']):
			for i in range(params['N_DCT']):
				Out_str += "{}, ".format(int(DCT_Coeff[k, i]))
			Out_str += "\n\t"
		Out_str += "};\n"
		if "LIFTER_COEFF" in params.keys() and params["LIFTER_COEFF"] > 0:
			Out_str += " short int Lift_Coeff[{}] = {{\n\t".format(params['N_DCT'])
			for i in range(params['N_DCT']):
				Out_str += "{}, ".format(int(Lift_Coeff[i]))
				if (i+1)%6 == 0:
					Out_str += "\n\t"
			Out_str += "};\n"

	with open(LUT_FILE, 'w') as f:
		f.write(Out_str)


	# MFCC
	if USE_TF_FB:
		filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB_tf(params['N_FFT'], params['MFCC_BANK_CNT'], Fmin=params['FMIN'], Fmax=params['FMAX'], sample_rate=params['SAMPLERATE'])
	else:
		filters, MFCC_Coeff, HeadCoeff = GenMFCC_FB(params['N_FFT'], params['MFCC_BANK_CNT'], Fmin=params['FMIN'], Fmax=params['FMAX'], sample_rate=params['SAMPLERATE'])

	Out_str =  "#define MFCC_COEFF_CNT\t{}\n\n".format(HeadCoeff+1)
	Out_str += "typedef struct {\n"
	Out_str += "\tint Start;\n"
	Out_str += "\tint Stop;\n"
	Out_str += "\tint Base;\n"
	Out_str += "\tshort int Norm;\n"
	Out_str += "} FbankType;\n\n"
	Out_str += "/* Filter Bank bands:\n\n"
	Out_str += "\tMinimum Frequency: {} Hz\n".format(params['FMIN'])
	Out_str += "\tMaximum Frequency: {} Hz*/\n\n".format(params['FMAX'])

	Out_str += "fbank_type_t MFCC_FilterBank[{}] = {{\n".format(params['MFCC_BANK_CNT'])
	HeadCoeff = 0
	for i, filt in enumerate(filters):
		Start = np.argmax(filt!=0)
		Stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
		Base = HeadCoeff
		Items = Stop - Start + 1
		Sum = sum(MFCC_Coeff[Base:Base+(Stop-Start+1)])
		Norm = FP2FIX(1 / Sum, MFCC_COEFF_DYN)

		Out_str += "\t{{{:>4},{:>4},{:>4},{:>4}}},\n".format(Start, Stop, Base, Norm)
		HeadCoeff += Items
	Out_str += "};\n\n"
	    
	Out_str += "short int MFCC_Coeffs[{}] = {{\n\t".format(HeadCoeff+1)
	for i, coeff in enumerate(MFCC_Coeff):
		Out_str += "{:>5}".format(str(coeff)) + ", "
		if (i+1) % 15 == 0:
			Out_str += "\n\t"
	# Add a last 0 coeff
	Out_str += "{:>5}\n}};\n".format(0)

	with open(FB_FILE, "w") as f:
		f.write(Out_str)



if __name__ == "__main__":
	main()
