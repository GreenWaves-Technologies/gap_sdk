import numpy as np

def FP2FIX(Val, Prec):
    try:
        return (Val * ((1 << Prec) - 1)).astype(np.int32)
    except:
        return int(Val * ((1 << Prec) - 1))

def SetupTwiddlesLUT(Nfft, Inverse=False):
	Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft)
	if Inverse:
		Twiddles_real = np.cos(Phi)
		Twiddles_imag = np.sin(Phi)
	else:
		Twiddles_real = np.cos(-Phi)
		Twiddles_imag = np.sin(-Phi)
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

def SetupTwiddlesRFFT(Nfft, Inverse=False):
	Phi = (np.pi * 2 / Nfft) * np.arange(0, Nfft//2)
	Twiddles_real = np.sin(Phi)
	Twiddles_imag = np.cos(Phi)
	return Twiddles_real, Twiddles_imag

def SetupDCTTable(Ndct, dct_type=2, norm=None):
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
	return (DCT_Coeff * norm_factor)

def SetupLiftCoeff(L, N):
	Lift_Coeff = np.zeros(N)
	for i in range(N):
		Lift_Coeff[i] = 1.0 + (L / 2.0) * np.sin(np.pi * i / L)
	return Lift_Coeff

def SetupMelFBanksTf(Nfft, Nbanks, sample_rate, Fmin, Fmax):
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


def SetupMelFBanksLibrosa(Nfft, Nbanks, sample_rate, Fmin, Fmax, norm=None):
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

def SetupMelFBanks(Nfft, Nbanks, sample_rate, Fmin, Fmax):
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
		#print( "f[%d] = %d" % (i, f[i]))

	filters = np.zeros((Nbanks, Nfft // 2))
	for i in range(1, Nbanks+1):
		for k in range(f[i-1], f[i]):
			filters[i-1][k] = np.float(k-f[i-1])/(f[i]-f[i-1])
		for k in range(f[i], f[i+1]):
			filters[i-1][k] = np.float(f[i+1]-k)/(f[i+1]-f[i])

	return filters

def SparseMelFilterBanksCode(filters, quantize=False, q_precision=None):
	if quantize:
		filters = FP2FIX(filters, q_precision).astype(np.int16)
	Base = 0
	MelCoeffs = np.empty(0)
	SparsityMat = np.empty(0)
	for i, filt in enumerate(filters):
		if np.all(filt == 0):
			Start = 0
			Stop = 0
			Items = 0
		else:
			Start = np.argmax(filt!=0)
			Stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
			Items = Stop - Start + 1
		#print("Filter {}: Start: {} Stop: {} Base: {} Items: {}".format(i, Start, Stop, Base, Items))
		MelCoeffs = np.append(MelCoeffs, filt[Start:Start+Items])
		SparsityMat = np.append(SparsityMat, [Start, Items, Base])

		Base += Items

	return np.array(MelCoeffs, dtype=filters.dtype), np.array(SparsityMat, dtype=np.int16), Base