# Autotiler MFCC

This folder contains the function to generate and run MFCC on GAP. The reference algorithm cna be found [here](http://practicalcryptography.com/miscellaneous/machine-learning/guide-mel-frequency-cepstral-coefficients-mfccs/). The [tensorflow](https://www.tensorflow.org/api_docs/python/tf/signal/mfccs_from_log_mel_spectrograms) implementation is also supported. Setting _USE_TF_FB=True_ in the GenLUT script will generate the tensorflow mel filterbank.

Algorithm:
	- (optional) PreEmphasis Y[n] = X[n] - PREEMP_FACTOR\*X[n-1]
	- (optional) Windowing  (default: Hanning)
	- FFT (default: radix2)
	- Power or Magnitude (default: power)
	- MelFilterBank
	- Natural Logarithm
	- (optional) DCT
	- (optional) Lifter

In MfccModel.c you will find 2 examples of code generation for the MFCC execution over N_FRAME of size FRAME_SIZE stacked together with stride FRAME_STEP and over a single frame. The MFCC parameters are set in MFCC_params.json and the GenLUT.py script will generate for you the look up tables necessary for the several steps of the preprocessing function.

Several implementations are available, in particular the user can:
	- Apply a PreEmphasis to the input frame: Y[n] = X[n] - PREEMP_FACTOR\*X[n-1]
	- Apply a Window to the input, can be changed in the GenLUT script (default: HANNING window)
	- The number of FFT bins must be specified by the user (must be power of 2 and > than FRAME_SIZE)
	- A high precision FFT can be used instead of the default quantized version. It uses greater fixed point structures and achieve better results compared to the original floating point FFT but requires more computational effort
	- Apply the MelFilterBank to the Spectrogram (|STFT|^2) or the magnitude of STFT directly (|STFT|). The second approach requires float emulated functions (sqrt), hence has a significative drop in performance
	- The number of MFCC filter banks can be set with MFCC_COEFF_CNT
	- An optional DCT can be applied after the log, N_DCT must be specified by the user. The default DCT algorithm is the [type II](https://docs.scipy.org/doc/scipy/reference/generated/scipy.fftpack.dct.html) with fixed point arithmetic and without any normalization
	- A lifter coeff can be applied after the DCT if present

## Run example

To generate the code with the default settings in MFCC_params.json:
	make clean_model mfcc_model 

To run the generated code:
	make clean all run
