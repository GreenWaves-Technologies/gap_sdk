from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import argparse
import os
import sys
import sklearn
import re

import tensorflow as tf
import input_data
import input_data

def prepare_model_settings(label_count, sample_rate, clip_duration_ms,
                           window_size_ms, window_stride_ms,
                           dct_coefficient_count, preprocess='mfcc',
                           use_power_spectrogram=True):
  """Calculates common settings needed for all models.

  Args:
    label_count: How many classes are to be recognized.
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
  
  if preprocess == 'average':
    fft_bin_count = 1 + (_next_power_of_two(window_size_samples) / 2)
    average_window_width = int(math.floor(fft_bin_count / dct_coefficient_count))
    fingerprint_width = int(math.ceil(fft_bin_count / average_window_width))
  elif preprocess == 'mfcc':
    average_window_width = -1
    fingerprint_width = dct_coefficient_count
  elif preprocess == 'micro':
    average_window_width = -1
    fingerprint_width = dct_coefficient_count
  else:
    raise ValueError('Unknown preprocess mode "%s" (should be "mfcc",'
                     ' "average", or "micro")' % (preprocess))
  
  fingerprint_size = fingerprint_width * spectrogram_length
  return {
      'desired_samples': desired_samples,
      'window_size_samples': window_size_samples,
      'window_stride_samples': window_stride_samples,
      'spectrogram_length': spectrogram_length,
      
      'dct_coefficient_count': dct_coefficient_count,
      'fingerprint_width': fingerprint_width,
      
      'fingerprint_size': fingerprint_size,
      'label_count': label_count,
      'sample_rate': sample_rate,
      'preprocess': preprocess,
      'average_window_width': average_window_width,
      'use_power': use_power_spectrogram
  }


CLASS_MATCH = re.compile(r'Recognized:\t(?P<class>[0-9]+)\n')

def main(_):
  if FLAGS.exe_path is None:
    small = int("_s_" in FLAGS.tflite_model)
    medium = int("_m_" in FLAGS.tflite_model)
    large = int("_l_" in FLAGS.tflite_model)
    if not small and not medium and not large:
      raise ValueError("You must select one of the models in model dir")
    compile_command = 'make -f emul.mk clean_model clean all DUMP_TENSORS=0 SMALL={} MEDIUM={} LARGE={} WITH_MFCC={} USE_POWER={}'.format(small, medium, large, 1 if FLAGS.test_with_wav == True else 0, 1 if FLAGS.use_power_spectrogram == True else 0)
    print(compile_command)
    stream = os.popen(compile_command)
    for line in stream.readlines():
    	print(line, end='')
    if not os.path.exists('./kws_ds_cnn_emul'):
    	raise Exception("Executable not generated correctly")
    executable = "kws_ds_cnn_emul"
  else:
  	executable = FLAGS.exe_path
  
  tf.logging.set_verbosity(tf.logging.INFO)
  sess = tf.InteractiveSession()
  words_list = input_data.prepare_words_list(FLAGS.wanted_words.split(','))
  model_settings = prepare_model_settings(
      len(words_list), FLAGS.sample_rate, FLAGS.clip_duration_ms, FLAGS.window_size_ms,
      FLAGS.window_stride_ms, FLAGS.dct_coefficient_count, FLAGS.preprocess, bool(FLAGS.use_power_spectrogram))
  print(model_settings)
 
  audio_processor = input_data.AudioProcessor(
      FLAGS.data_url, FLAGS.data_dir, FLAGS.silence_percentage,
      FLAGS.unknown_percentage,
      FLAGS.wanted_words.split(','), FLAGS.validation_percentage,
      FLAGS.testing_percentage, model_settings)

  # validation set
  set_size = audio_processor.set_size('validation')
  tf.logging.info('Validation set size:%d', set_size)
  total_accuracy = 0
  total_conf_matrix = None
  corrects = 0
  for i in range(0, set_size):
    if not FLAGS.test_with_wav:
      validation_fingerprint, validation_ground_truth = audio_processor.get_data(1, i, model_settings, 0.0, 0, 0, 'validation', sess)
      # Quantize fingerprints and save it in a pgm file, then run the network with it
      in_shape = [-1, FLAGS.dct_coefficient_count]
      tf_mfccs_int8 = np.floor(validation_fingerprint.astype(np.float32) / FLAGS.nntool_input_scale + 0.5).astype(np.int8).reshape(in_shape) # Scale to int8
      with open("test.pgm", 'wb') as f:
        hdr =  'P5' + '\n' + str(tf_mfccs_int8.shape[1]) + '  ' + str(tf_mfccs_int8.shape[0]) + '  ' + str(255) + '\n'
        f.write(hdr.encode())
        np.int8(tf_mfccs_int8).tofile(f)
      ex_stream = os.popen("./{} test.pgm".format(executable))
    else:
      # Run the exe on the wav file directly
      validation_wav_file, validation_ground_truth = audio_processor.get_wav_files(1, i, model_settings, 'validation')
      ex_stream = os.popen("./{} {}".format(executable, validation_wav_file[0]))
    emul_log = ex_stream.readlines()
    emul_prediction = None
    for line in emul_log:
      m = CLASS_MATCH.search(line)
      if m:
        emul_prediction = int(m['class'])
        break
    if emul_prediction is None:
      print("Error encountered with './{} {}'".format(executable, validation_wav_file[0]))
      continue

    predicted_class = emul_prediction
    gt_class = np.argmax(validation_ground_truth)
    corrects += 1 if predicted_class == gt_class else 0
    conf_matrix = sklearn.metrics.confusion_matrix([words_list[gt_class]], [words_list[predicted_class]], labels=words_list)
    if total_conf_matrix is None:
      total_conf_matrix = conf_matrix
    else:
      total_conf_matrix += conf_matrix
    if not(i % 100) and i > 0:
      print("Pred/Tot:\t{:4d}/{:4d}\tAccuracy:\t{:.2f}%".format(corrects, i, corrects/i*100))
  print("\nFINAL VALIDATION ACCURACY:")
  print("Pred/Tot:\t{:4d}/{:4d}\tAccuracy:\t{:.2f}%\n".format(corrects, i, corrects/i*100))
  print("Confusion matrix:\n{}".format(total_conf_matrix))

  # test set
  set_size = audio_processor.set_size('testing')
  tf.logging.info('Test set size:%d', set_size)
  total_accuracy = 0
  total_conf_matrix = None
  corrects = 0
  for i in range(0, set_size):
    if not FLAGS.test_with_wav:
      testing_fingerprint, testing_ground_truth = audio_processor.get_data(1, i, model_settings, 0.0, 0, 0, 'testing', sess)
      # Quantize fingerprints and save it in a pgm file, then run the network with it
      in_shape = [-1, FLAGS.dct_coefficient_count]
      tf_mfccs_int8 = np.floor(testing_fingerprint.astype(np.float32) / FLAGS.nntool_input_scale + 0.5).astype(np.int8).reshape(in_shape) # Scale to int8
      with open("test.pgm", 'wb') as f:
        hdr =  'P5' + '\n' + str(tf_mfccs_int8.shape[1]) + '  ' + str(tf_mfccs_int8.shape[0]) + '  ' + str(255) + '\n'
        f.write(hdr.encode())
        np.int8(tf_mfccs_int8).tofile(f)
      ex_stream = os.popen("./{} test.pgm".format(executable))
    else:
      # Run the exe on the wav file directly
      testing_wav_file, testing_ground_truth = audio_processor.get_wav_files(1, i, model_settings, 'testing')
      ex_stream = os.popen("./{} {}".format(executable, testing_wav_file[0]))

    emul_log = ex_stream.readlines()
    emul_prediction = None
    for line in emul_log:
      m = CLASS_MATCH.search(line)
      if m:
        emul_prediction = int(m['class'])
        break
    if emul_prediction is None:
      print("Error encountered with './{} {}'".format(executable, testing_wav_file[0]))
      continue

    predicted_class = emul_prediction
    gt_class = np.argmax(testing_ground_truth)
    corrects += 1 if predicted_class == gt_class else 0
    conf_matrix = sklearn.metrics.confusion_matrix([words_list[gt_class]], [words_list[predicted_class]], labels=words_list)
    if total_conf_matrix is None:
      total_conf_matrix = conf_matrix
    else:
      total_conf_matrix += conf_matrix
    if not(i % 100) and i > 0:
      print("Pred/Tot:\t{:4d}/{:4d}\tAccuracy:\t{:.2f}%".format(corrects, i, corrects/i*100))
  print("\nFINAL TESTING ACCURACY:")
  print("Pred/Tot:\t{:4d}/{:4d}\tAccuracy:\t{:.2f}%\n".format(corrects, i, corrects/i*100))
  print("Confusion matrix:\n{}".format(total_conf_matrix))
  os.popen("rm test.pgm")



if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument(
      '--data_url',
      type=str,
      # pylint: disable=line-too-long
      default='http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz',
      # pylint: enable=line-too-long
      help='Location of speech training data archive on the web.')
  parser.add_argument(
      '--data_dir',
      type=str,
      default='samples/speech_dataset/',
      help="""\
      Where to download the speech training data to.
      """)
  parser.add_argument(
      '--tflite_model', type=str, default='', help='Model to use for identification.')
  parser.add_argument(
      '--silence_percentage',
      type=float,
      default=10.0,
      help="""\
      How much of the training data should be silence.
      """)
  parser.add_argument(
      '--unknown_percentage',
      type=float,
      default=10.0,
      help="""\
      How much of the training data should be unknown words.
      """)
  parser.add_argument(
      '--testing_percentage',
      type=int,
      default=10,
      help='What percentage of wavs to use as a test set.')
  parser.add_argument(
      '--validation_percentage',
      type=int,
      default=10,
      help='What percentage of wavs to use as a validation set.')
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
      help='How long each spectrogram timeslice is',)
  parser.add_argument(
      '--window_stride_ms',
      type=float,
      default=10.0,
      help='How long each spectrogram timeslice is',)
  parser.add_argument(
      '--dct_coefficient_count',
      type=int,
      default=40,
      help='How many bins to use for the MFCC fingerprint',)
  parser.add_argument(
      '--batch_size',
      type=int,
      default=100,
      help='How many items to train with at once',)
  parser.add_argument(
      '--wanted_words',
      type=str,
      default='yes,no,up,down,left,right,on,off,stop,go',
      help='Words to use (others will be added to an unknown label)',)
  parser.add_argument(
      '--preprocess',
      type=str,
      default='mfcc',
      help='Spectrogram processing mode. Can be "mfcc", "average", or "micro"')
  parser.add_argument(
      '--nntool_input_scale',
      type=float,
      default=1.9372712,
      help='Scale factor applied to the input by nntool (you can look at it \
      		importing the model in nntool and once quantized (if already tflite \
      		quantize open it with -q flag) look at the input scale with qshow 0')
  parser.add_argument(
      '--exe_path',
      type=str,
      default=None,
      help='Path to compiled emul executable')
  parser.add_argument(
      '--test_with_wav',
      type=int,
      default=0,
      help='Test with MFCC gap implementation')
  parser.add_argument(
      '--use_power_spectrogram',
      type=int,
      default=1)

  
  FLAGS, unparsed = parser.parse_known_args()
  tf.app.run(main=main, argv=[sys.argv[0]] + unparsed)
