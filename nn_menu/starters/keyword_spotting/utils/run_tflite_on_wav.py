import sys
from scipy.io.wavfile import read
from tensorflow.contrib.framework.python.ops import audio_ops
from tensorflow.python.ops import io_ops
import tensorflow as tf
import numpy as np


if len(sys.argv) < 3:
	raise ValueError("give me a path to model and to a file float32 .dat")
else:
	model_path = sys.argv[1]
	file_path = sys.argv[2]

FRAME_SIZE = 640
FRAME_STRIDE = 320
SAMPLE_RATE = DESIRED_SAMPLES = 16000
NUM_CEP = 10

wav_loader = io_ops.read_file(file_path)
wav_decoder = audio_ops.decode_wav(wav_loader, desired_channels=1, desired_samples=DESIRED_SAMPLES)
# Run the spectrogram and MFCC ops to get a 2D 'fingerprint' of the audio.
spectrograms_power = audio_ops.audio_spectrogram(wav_decoder.audio,
										  window_size=FRAME_SIZE,
										  stride=FRAME_STRIDE,
										  magnitude_squared=True)
USE_POWER=True
if USE_POWER:
	# Warp the linear scale spectrograms into the mel-scale.
	num_spectrogram_bins = spectrograms_power.shape[-1].value
	lower_edge_hertz, upper_edge_hertz, num_mel_bins = 20.0, 4000.0, 40
	linear_to_mel_weight_matrix = tf.signal.linear_to_mel_weight_matrix(
		num_mel_bins, num_spectrogram_bins, 16000.0, lower_edge_hertz,
		upper_edge_hertz)
	mel_spectrograms = tf.tensordot(
		spectrograms_power, linear_to_mel_weight_matrix, 1)
	mel_spectrograms.set_shape(spectrograms_power.shape[:-1].concatenate(
		linear_to_mel_weight_matrix.shape[-1:]))

	# Compute a stabilized log to get log-magnitude mel-scale spectrograms.
	log_mel_spectrograms = tf.math.log(mel_spectrograms + 1e-6)

	# Compute MFCCs from log_mel_spectrograms and take the first NDCT.
	mfccs = tf.signal.mfccs_from_log_mel_spectrograms(
		log_mel_spectrograms)[..., :NUM_CEP]
	#output = tf.expand_dims(mfccs, axis=0)
else:
	mfccs = audio_ops.mfcc(
		 spectrograms_power,
		 wav_decoder.sample_rate,
		 dct_coefficient_count=NUM_CEP)

print(tf.Session().run(wav_decoder.audio).max())
mfccs = tf.Session().run(mfccs)
interpreter = tf.lite.Interpreter(model_path)
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print("Input Details: \n", input_details)
print("Output Details: \n", output_details)
scale, zero_point = input_details[0]['quantization']

print(mfccs.shape)
input_array = np.array(np.floor(mfccs / scale + 0.5) + zero_point).astype(np.uint8)
interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
interpreter.invoke()
output = interpreter.get_tensor(output_details[0]['index'])
print(output)
print("Predicted: ", np.argmax(output))
