import sounddevice as sd
from scipy.io.wavfile import write
import os
import re

from tensorflow.contrib.framework.python.ops import audio_ops
from tensorflow.python.ops import io_ops
import tensorflow as tf
import sys
import numpy as np

LABELS = ["silence", "unknown", "yes","no","up","down","left","right","on","off","stop","go"]
CLASS_MATCH = re.compile(r'Recognized:\t(?P<class>[0-9]+)\n')

if len(sys.argv) < 2:
	raise ValueError("give me a path to model")
else:
	model_path = sys.argv[1]

interpreter = tf.lite.Interpreter(model_path)
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()
print("Input Details: \n", input_details)
print("Output Details: \n", output_details)
scale, zero_point = input_details[0]['quantization']

FRAME_SIZE = 640
FRAME_STRIDE = 320
fs = DESIRED_SAMPLES = 16000
NUM_CEP = 10

my_records = [None]*2
my_records[0] = sd.rec(DESIRED_SAMPLES, samplerate=fs, channels=1, dtype='int16')
idx = False
count = 0
while True:
	sd.wait()  # Wait until recording is finished
	idx = not idx
	my_records[int(idx)] = sd.rec(DESIRED_SAMPLES, samplerate=fs, channels=1, dtype='int16')

	# Run the spectrogram and MFCC ops to get a 2D 'fingerprint' of the audio.
	spectrograms_power = audio_ops.audio_spectrogram(my_records[int(not idx)]/32678,
											         window_size=FRAME_SIZE,
											         stride=FRAME_STRIDE,
											         magnitude_squared=True)
	USE_POWER="power" in model_path
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
	else:
		mfccs = audio_ops.mfcc(
			 spectrograms_power,
			 wav_decoder.sample_rate,
			 dct_coefficient_count=NUM_CEP)
	mfccs = tf.Session().run(mfccs)

	input_array = np.array(np.floor(mfccs / scale + 0.5) + zero_point).astype(np.uint8)
	interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
	interpreter.invoke()
	output = interpreter.get_tensor(output_details[0]['index'])
	prediction = LABELS[np.argmax(output)]

	filename = 'from_pc/for_gap_{}.wav'.format(count)
	write(filename, fs, my_records[int(not idx)])
	ex_stream = os.popen("./kws_ds_cnn_emul {}".format(filename))
	emul_log = ex_stream.readlines()
	emul_prediction = None
	for line in emul_log:
		m = CLASS_MATCH.search(line)
		if m:
			emul_prediction = int(m['class'])
			break
	if emul_prediction is None:
		print("Error encountered with './kws_ds_cnn_emul {}'".format(filename))
		continue
	print("Predicted: TFLITE/GAP: {}/{} with confidence: {}".format(prediction, LABELS[emul_prediction], np.max(output)*output_details[0]['quantization'][0]))

	count += 1
