import numpy as np

FFT_TWIDDLE_DYN = 15
DCT_TWIDDLE_DYN = 7
LIFT_COEFF_DYN = 11
MFCC_COEFF_DYN = 15

def FP2FIX(Val, Prec):
    try:
        return (Val * ((1 << Prec) - 1)).astype(np.int32)
    except:
        return int(Val * ((1 << Prec) - 1))

def convert_dtype(arr, dtype="int", prec=15):
	if dtype == "int":
		return np.round(arr * ((1<<Prec)-1)).astype(np.int)
	elif dtype == "float16":
		return arr.astype(np.float16)
	else:
		return arr

def SetupTwiddlesLUT(Nfft, Inverse=False, dtype="int"):
	Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft)
	if Inverse:
		Twiddles_real = np.cos(Phi)
		Twiddles_imag = np.sin(Phi)
	else:
		Twiddles_real = np.cos(-Phi)
		Twiddles_imag = np.sin(-Phi)
	if dtype == "int":
		return np.round(Twiddles_real * ((1<<FFT_TWIDDLE_DYN)-1)).astype(np.int), np.round(Twiddles_imag * ((1<<FFT_TWIDDLE_DYN)-1)).astype(np.int)
	elif dtype == "float16":
		return Twiddles_real.astype(np.float16), Twiddles_imag.astype(np.float16)
	else:
		return Twiddles_real, Twiddles_imag

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

def SetupSwapTableR4(Ni):
	log4 = int(np.log2(Ni) / 2)
	iL = Ni / 4
	iM = 1
	SwapTable = np.zeros(Ni)
	for i in range(log4):
		for j in range(iM):
			SwapTable[j +   iM] = SwapTable[j] +   iL
			SwapTable[j + 2*iM] = SwapTable[j] + 2*iL
			SwapTable[j + 3*iM] = SwapTable[j] + 3*iL
		iL /= 4
		iM *= 4
	return SwapTable

def SetupTwiddlesRFFT(Nfft, Inverse=False, dtype="int"):
	Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft//2)
	#if Inverse:
	#	Twiddles_real = np.cos(Phi)
	#	Twiddles_imag = np.sin(Phi)
	#else:
	Twiddles_real = np.sin(Phi)
	Twiddles_imag = np.cos(Phi)
	if dtype == "int":
		return np.round(Twiddles_real * ((1<<FFT_TWIDDLE_DYN)-1)).astype(np.int), np.round(Twiddles_imag * ((1<<FFT_TWIDDLE_DYN)-1)).astype(np.int)
	elif dtype == "float16":
		return Twiddles_real.astype(np.float16), Twiddles_imag.astype(np.float16)
	else:
		return Twiddles_real, Twiddles_imag

def SetupDCTTable(Ndct, dct_type=2, dtype="int", norm=None):
	print(f"Setting up DCT table with type {dtype}")
	norm_factor = np.ones((Ndct, Ndct))
	if norm == "ortho" and dct_type == 2:
		norm_factor   *= np.sqrt(1/(2*Ndct))
		norm_factor[0] = np.sqrt(1/(4*Ndct))
	if norm == "ortho" and dct_type == 3:
		norm_factor   *= np.sqrt(1/(2*Ndct))
		norm_factor[0] = np.sqrt(1/(Ndct))
	DCT_Coeff = np.zeros((Ndct, Ndct))
	for k in range(Ndct):
		for i in range(Ndct):
			if dct_type == 2:
				coeff = 2*np.cos(np.pi / (2*Ndct) * k * (2*i + 1))
			elif dct_type == 3:
				coeff = 1 if i == 0 else 2*np.cos(np.pi / (2*Ndct) * i * (2*k + 1))
			else:
				raise NotImplementedError("DCT type 2 and 3 only supported")
			DCT_Coeff[k, i] = coeff

	if dtype == "int":
		return np.round((DCT_Coeff * norm_factor) * ((1<<(DCT_TWIDDLE_DYN))-1)).astype(np.int16)
	elif dtype == "float16":
		return (DCT_Coeff * norm_factor).astype(np.float16)
	else:
		return (DCT_Coeff * norm_factor)

def SetupLiftCoeff(L, N, dtype="int"):
	Lift_Coeff = np.zeros(N)
	for i in range(N):
		Lift_Coeff[i] = 1.0 + (L / 2.0) * np.sin(np.pi * i / L)

	if dtype == "int":
		return np.round(Lift_Coeff * ((1<<(LIFT_COEFF_DYN))-1)).astype(np.int16)
	elif dtype == "float16":
		return (Lift_Coeff).astype(np.float16)
	else:
		return Lift_Coeff

def GenMelFilterBanksCode(filters, mfcc_bank_cnt, fmin, fmax, dtype, data_type, name_suffix):
	HeadCoeff = 0
	MFCC_Coeff = []
	if dtype == "int":
		quant_filters = FP2FIX(filters, MFCC_COEFF_DYN)
	elif dtype == "float16":
		quant_filters = filters.astype(np.float16)
	else:
		quant_filters = filters.astype(np.float32)

	for i, filt in enumerate(quant_filters):
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
		print("Filter {}: Start: {} Stop: {} Base: {} Items: {}".format(i, Start, Stop, Base, Items))
		for j in range(Items):
			MFCC_Coeff.append(filt[Start+j])
		HeadCoeff += Items

	#Out_str =  "#define MFCC_COEFF_CNT\t{}\n\n".format(HeadCoeff+1)
	Out_str  = "/* Filter Bank bands:\n\n"
	Out_str += "\tMinimum Frequency: {} Hz\n".format(fmin)
	Out_str += "\tMaximum Frequency: {} Hz*/\n\n".format(fmax)

	Out_str += "PI_L2 fbank_type_t MFCC_FilterBank{}[{}] = {{\n".format(name_suffix, mfcc_bank_cnt)
	HeadCoeff = 0
	for i, filt in enumerate(quant_filters):
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

	Out_str += "PI_L2 {} MFCC_Coeffs{}[{}] = {{\n\t".format(data_type, name_suffix, HeadCoeff+1)
	for i, coeff in enumerate(MFCC_Coeff):
		Out_str += "{:>5}".format(str(coeff)) + ", "
		if (i+1) % 15 == 0:
			Out_str += "\n\t"
	# Add a last 0 coeff
	Out_str += "{:>5}\n}};\n".format(0)
	return Out_str, HeadCoeff


def GenMFCC_FB_tf(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000, dtype="int"):
	import tensorflow as tf
	# Generate Mel Filterbanks matrix with tensorflow functions:
	# https://www.tensorflow.org/api_docs/python/tf/signal/mfccs_from_log_mel_spectrograms
	print("Generating TF Mel Filter: sr = {}, n_mels = {}, n_fft = {}, fmin = {}, fmax = {}".format(sample_rate, Nbanks, Nfft, Fmin, Fmax))
	num_spectrogram_bins = Nfft//2 + 1
	lower_edge_hertz, upper_edge_hertz, num_mel_bins = Fmin, Fmax, Nbanks
	linear_to_mel_weights = tf.signal.linear_to_mel_weight_matrix(
	  num_mel_bins, num_spectrogram_bins, sample_rate, lower_edge_hertz,
	  upper_edge_hertz)

	if tf.__version__.startswith("1"):
		linear_to_mel_weights = tf.Session().run(linear_to_mel_weights)
	else:
		linear_to_mel_weights = np.array(linear_to_mel_weights)

	return linear_to_mel_weights.T


def GenMFCC_FB_librosa(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000, norm=None, dtype='int'):
	import librosa
	print("Generating LIBROSA Mel Filter: sr = {}, n_mels = {}, n_fft = {}, fmin = {}, fmax = {}, norm = {}".format(sample_rate, Nbanks, Nfft, Fmin, Fmax, norm))
	# Generate Mel Filterbanks matrix with librosa functions:
	# https://librosa.org/doc/0.6.3/generated/librosa.filters.mel.html#librosa.filters.mel
	linear_to_mel_weights = librosa.filters.mel(sample_rate, Nfft, Nbanks, Fmin, Fmax, norm=norm)
	return linear_to_mel_weights


def Mel(k):
  	return 1125 * np.log(1 + k/700.0)
def InvMel(k):
    return 700.0*(np.exp(k/1125.0) - 1.0)

def GenMFCC_FB(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000, dtype='int'):
	# Generate Mel Filterbank
	# http://practicalcryptography.com/miscellaneous/machine-learning/guide-mel-frequency-cepstral-coefficients-mfccs/
	print("Generating Mel Filter: sr = {}, n_mels = {}, n_fft = {}, fmin = {}, fmax = {}".format(sample_rate, Nbanks, Nfft, Fmin, Fmax))
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

	return filters

