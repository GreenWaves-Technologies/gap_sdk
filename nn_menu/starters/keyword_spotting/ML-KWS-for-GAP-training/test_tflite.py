from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import argparse
import os.path
import sys
import sklearn

import tensorflow as tf
import input_data
import models

def main(_):
  
  tf.logging.set_verbosity(tf.logging.INFO)
  sess = tf.InteractiveSession()
  words_list = input_data.prepare_words_list(FLAGS.wanted_words.split(','))
  model_settings = models.prepare_model_settings(
      len(words_list), FLAGS.sample_rate, FLAGS.clip_duration_ms, FLAGS.window_size_ms,
      FLAGS.window_stride_ms, FLAGS.dct_coefficient_count, FLAGS.preprocess)
  print(model_settings)
 
  audio_processor = input_data.AudioProcessor(
      FLAGS.data_url, FLAGS.data_dir, FLAGS.silence_percentage,
      FLAGS.unknown_percentage,
      FLAGS.wanted_words.split(','), FLAGS.validation_percentage,
      FLAGS.testing_percentage, model_settings)

  interpreter = tf.lite.Interpreter(model_path=FLAGS.tflite_model)
  interpreter.allocate_tensors()

  # Get input and output tensors.
  input_details = interpreter.get_input_details()
  output_details = interpreter.get_output_details()
  print('Input details: ', input_details)
  print('Output details: ', output_details)
  scale, zero_point = input_details[0]['quantization']
		
  # validation set
  set_size = audio_processor.set_size('validation')
  tf.logging.info('Validation set size:%d', set_size)
  total_accuracy = 0
  total_conf_matrix = None
  corrects = 0
  for i in range(0, set_size):
    validation_fingerprint, validation_ground_truth = audio_processor.get_data(1, i, model_settings, 0.0, 0, 0, 'validation', sess)
    if scale != 0.0:
      input_array = np.array(np.floor(validation_fingerprint / scale + 0.5) + zero_point).astype(np.uint8)
    else:
      input_array = np.array(validation_fingerprint).astype(np.float32)
    interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
    interpreter.invoke()
    output = interpreter.get_tensor(output_details[0]['index'])
    predicted_class = np.argmax(output)
    gt_class = np.argmax(validation_ground_truth)
    corrects += 1 if predicted_class == gt_class else 0
    conf_matrix = sklearn.metrics.confusion_matrix([words_list[gt_class]], [words_list[predicted_class]], labels=words_list)
    if total_conf_matrix is None:
      total_conf_matrix = conf_matrix
    else:
      total_conf_matrix += conf_matrix
    if not(i % 100) and i > 0:
      print("Pred/Tot: {}/{} Accuracy: {}%\n".format(corrects, i, corrects/i*100))
  print("Pred/Tot: {}/{} Accuracy: {}%\n".format(corrects, i, corrects/i*100))
  print("Confusion matrix:\n{}".format(total_conf_matrix))

  # test set
  set_size = audio_processor.set_size('testing')
  tf.logging.info('Test set size:%d', set_size)
  total_accuracy = 0
  total_conf_matrix = None
  corrects = 0
  for i in range(0, set_size):
    testing_fingerprint, testing_ground_truth = audio_processor.get_data(1, i, model_settings, 0.0, 0, 0, 'testing', sess)
    if scale != 0.0:
      input_array = np.array(np.floor(testing_fingerprint / scale + 0.5) + zero_point).astype(np.uint8)
    else:
      input_array = np.array(testing_fingerprint).astype(np.float32)
    interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
    interpreter.invoke()
    output = interpreter.get_tensor(output_details[0]['index'])
    predicted_class = np.argmax(output)
    gt_class = np.argmax(testing_ground_truth)
    corrects += 1 if predicted_class == gt_class else 0
    conf_matrix = sklearn.metrics.confusion_matrix([words_list[gt_class]], [words_list[predicted_class]], labels=words_list)
    if total_conf_matrix is None:
      total_conf_matrix = conf_matrix
    else:
      total_conf_matrix += conf_matrix
    if not(i % 100) and i > 0:
      print("Pred/Tot: {}/{} Accuracy: {}%\n".format(corrects, i, corrects/i*100))
  print("Pred/Tot: {}/{} Accuracy: {}%\n".format(corrects, i, corrects/i*100))
  print("Confusion matrix:\n{}".format(total_conf_matrix))



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
      default='../speech_commands/speech_dataset/',
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

  
  FLAGS, unparsed = parser.parse_known_args()
  tf.app.run(main=main, argv=[sys.argv[0]] + unparsed)
