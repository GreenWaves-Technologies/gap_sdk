import sys
import os
import argparse
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import glob

from tensorflow.contrib.framework.python.ops import audio_ops
from tensorflow.python.ops import io_ops

FLAGS = None
NNTOOL_INPUT_SCALE = 1.9372712

def filename(path):
	return os.path.splitext(os.path.split(path)[1])[0]

def prepare_model_settings(sample_rate, clip_duration_ms,
                           window_size_ms, window_stride_ms,
                           dct_coefficient_count):
  """Calculates common settings needed for all models.

  Args:
    sample_rate: Number of audio samples per second.
    clip_duration_ms: Length of each audio clip to be analyzed.
    window_size_ms: Duration of frequency analysis window.
    window_stride_ms: How far to move in time between frequency windows.
    dct_coefficient_count: Number of frequency bins to use for analysis.

  Returns:
    Dictionary containing common settings.
  """
  desired_samples = int(sample_rate * clip_duration_ms / 1000)
  window_size_samples = int(sample_rate * window_size_ms / 1000)
  window_stride_samples = int(sample_rate * window_stride_ms / 1000)
  length_minus_window = (desired_samples - window_size_samples)
  if length_minus_window < 0:
    spectrogram_length = 0
  else:
    spectrogram_length = 1 + int(length_minus_window / window_stride_samples)
  fingerprint_size = dct_coefficient_count * spectrogram_length
  return {
      'desired_samples': desired_samples,
      'window_size_samples': window_size_samples,
      'window_stride_samples': window_stride_samples,
      'spectrogram_length': spectrogram_length,
      'dct_coefficient_count': dct_coefficient_count,
      'fingerprint_size': fingerprint_size,
      'sample_rate': sample_rate,
  }

def get_mfcc_graph(model_settings):
	g = tf.Graph()
	with g.as_default():
		input_file_placeholder = tf.compat.v1.placeholder(tf.string, [], name='wav_filename')
		wav_loader = io_ops.read_file(input_file_placeholder)
		wav_decoder = audio_ops.decode_wav(wav_loader, desired_channels=1, desired_samples=model_settings['desired_samples'])
		# Run the spectrogram and MFCC ops to get a 2D 'fingerprint' of the audio.
		spectrograms_power = audio_ops.audio_spectrogram(wav_decoder.audio,
												  window_size=model_settings['window_size_samples'],
												  stride=model_settings['window_stride_samples'],
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
				log_mel_spectrograms)[..., :model_settings['dct_coefficient_count']]
			#output = tf.expand_dims(mfccs, axis=0)
			output = mfccs
		else:
			output = audio_ops.mfcc(
				 spectrograms_power,
				 wav_decoder.sample_rate,
				 dct_coefficient_count=model_settings['dct_coefficient_count'])
	return g, input_file_placeholder, output, wav_decoder.audio

def main(_):
	model_settings = prepare_model_settings(
    	  FLAGS.sample_rate, FLAGS.clip_duration_ms, FLAGS.window_size_ms,
	  	  FLAGS.window_stride_ms, FLAGS.dct_coefficient_count)

	for input_file in glob.glob(FLAGS.input_file):
		g, input_file_placeholder, tf_mfccs, signal = get_mfcc_graph(model_settings)
		with tf.compat.v1.Session(graph=g) as sess:
			tf.initialize_all_variables().run()
			tf_mfccs, signal = sess.run([tf_mfccs, signal], feed_dict={input_file_placeholder: input_file})

		print("\n")
		print(input_file)
			
		print("Float MFCC: [{}..{}] with shape [{}]".format(np.min(tf_mfccs), np.max(tf_mfccs), tf_mfccs.shape))
		tf_mfccs.astype(np.float32).tofile("samples/{}_features_float32.dat".format(filename(input_file)))
		tf_mfccs_int8 = np.floor(tf_mfccs.astype(np.float32) / NNTOOL_INPUT_SCALE + 0.5).astype(np.int8) # Scale to int8
		np.array(tf_mfccs_int8).tofile("samples/{}_features_int8.dat".format(filename(input_file)))
		with open("samples/{}_features_int8.pgm".format(filename(input_file)), 'wb') as f:
			hdr =  'P5' + '\n' + str(tf_mfccs.shape[2]) + '  ' + str(tf_mfccs.shape[1]) + '  ' + str(255) + '\n'
			f.write(hdr.encode())
			np.int8(tf_mfccs_int8).tofile(f)
		print("Integer MFCC: [{}..{}] with shape [{}]".format(np.min(tf_mfccs_int8), np.max(tf_mfccs_int8), tf_mfccs_int8.shape))

		if FLAGS.tflite_model is not None:
			interpreter = tf.lite.Interpreter(model_path=FLAGS.tflite_model)
			interpreter.allocate_tensors()
			
			# Get input and output tensors.
			input_details = interpreter.get_input_details()
			output_details = interpreter.get_output_details()
			print('Input details: ', input_details)
			print('Output details: ', output_details)
			scale, zero_point = input_details[0]['quantization']

			if scale != 0.0:
				input_array = np.array(np.floor(tf_mfccs / scale + 0.5) + zero_point).astype(np.uint8)
			else:
				input_array = np.array(tf_mfccs).astype(np.float32)

			interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
			interpreter.invoke()
			output = interpreter.get_tensor(output_details[0]['index'])
			print("Predicted class:\t{}\nWith confidence:\t{}".format(np.argmax(output), output[0, np.argmax(output)]))
			np.set_printoptions(threshold=sys.maxsize)
			print("Predictions: ")
			print(output)
			print("\n\n")

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument(
	  '--input_file',
	  type=str,
	  default='samples/*.wav',
	  help='Input wav file',)
	parser.add_argument(
	  '--tflite_model',
	  type=str,
	  help='Model to test',)
	parser.add_argument(
	  '--print_output',
	  type=bool,
	  default=True,
	  help='Model to test',)
	parser.add_argument(
	  '--sample_rate',
	  type=int,
	  default=16000,
	  help='Expected sample rate of the wavs',)
	parser.add_argument(
	  '--clip_duration_ms',
	  type=int,
	  default=1000,
	  help='Expected duration in milliseconds of the wavs',)
	parser.add_argument(
	  '--window_size_ms',
	  type=float,
	  default=30.0,
	  help='How long each spectrogram timeslice is.',)
	parser.add_argument(
	  '--window_stride_ms',
	  type=float,
	  default=10.0,
	  help='How far to move in time between spectrogram timeslices.',
	)
	parser.add_argument(
	  '--dct_coefficient_count',
	  type=int,
	  default=40,
	  help='How many bins to use for the MFCC fingerprint',
	)
	FLAGS, unparsed = parser.parse_known_args()
	tf.compat.v1.app.run(main=main, argv=[sys.argv[0]] + unparsed)
