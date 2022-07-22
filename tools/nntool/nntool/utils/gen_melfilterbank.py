import numpy as np


def GenMelFilters_FB_tf(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000):
    import tensorflow as tf
    # Generate Mel Filterbanks matrix with tensorflow functions:
    # https://www.tensorflow.org/api_docs/python/tf/signal/mfccs_from_log_mel_spectrograms
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


def GenMelFilters_FB_librosa(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000, norm="slaney"):
    import librosa
    # Generate Mel Filterbanks matrix with librosa functions:
    # https://librosa.org/doc/0.6.3/generated/librosa.filters.mel.html#librosa.filters.mel
    linear_to_mel_weights = librosa.filters.mel(sample_rate, Nfft, Nbanks, Fmin, Fmax, norm=norm)
    return linear_to_mel_weights


def Mel(k):
      return 1125 * np.log(1 + k/700.0)


def InvMel(k):
    return 700.0*(np.exp(k/1125.0) - 1.0)


def GenMelFilters_FB(Nfft, Nbanks, sample_rate=16000, Fmin=20, Fmax=4000):
    # Generate Mel Filterbank
    # http://practicalcryptography.com/miscellaneous/machine-learning/guide-mel-frequency-cepstral-coefficients-mfccs/

    SamplePeriod = 1.0/sample_rate
    Step = ((float) (Mel(Fmax)-Mel(Fmin)))/(Nbanks+1)
    Fmel0 = Mel(Fmin)

    f = [None] * (Nbanks+2)
    for i in range(Nbanks+2):
        f[i] = int(((Nfft+1)*InvMel(Fmel0+i*Step))//sample_rate)
        # print( "f[%d] = %d" % (i, f[i]))

    filters = np.zeros((Nbanks, Nfft // 2))
    for i in range(1, Nbanks+1):
        for k in range(f[i-1], f[i]):
            filters[i-1][k] = np.float(k-f[i-1])/(f[i]-f[i-1])
        for k in range(f[i], f[i+1]):
            filters[i-1][k] = np.float(f[i+1]-k)/(f[i+1]-f[i])

    return np.array(filters)

def GenMelFilterbank_Sparse(filters):
    ncoeff = 0
    MelFilt_Sparse_Coeff = []
    MelFilt_Sparsity_Matrix = []
    for i, filt in enumerate(filters):
        if np.all(filt == 0):
            start = 0
            stop = 0
            base = ncoeff
            items = 0
        else:
            start = np.argmax(filt!=0)
            stop = len(filt) - np.argmax(filt[::-1]!=0) - 1
            base = ncoeff
            items = stop - start + 1
        for j in range(items):
            MelFilt_Sparse_Coeff.append(filt[start+j])
        MelFilt_Sparsity_Matrix.append([start, items, base])
        ncoeff += items

    return np.array(MelFilt_Sparse_Coeff), np.array(MelFilt_Sparsity_Matrix)
