#!/usr/bin/python3.6
# PYTHON_ARGCOMPLETE_OK
#
import math
import argparse
import os
import random
import importlib
import models
import input_data
import matplotlib
import matplotlib.pyplot
from six.moves import xrange  # pylint: disable=redefined-builtin

import argcomplete
import numpy as np
import tensorflow as tf

def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='save_samples')
    parser.add_argument('-d', '--directory',
                        default="./images",
                        help='directory to save samples to')
    parser.add_argument('-s', '--set',
                        help='train or test image set',
                        choices=['train', 'test'],
                        default='test')
    parser.add_argument(
        '--data_url',
        type=str,
        # pylint: disable=line-too-long
        default='http://download.tensorflow.org/data/speech_commands_v0.01.tar.gz',
        # pylint: enable=line-too-long
        help='Location of speech training data archive on the web.')
    parser.add_argument(
        '--data_dir',
        type=str,
        default='./data/speech_dataset/',
        help="""\
        Where to download the speech training data to.
        """)
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
        '--summaries_dir',
        type=str,
        default='/tmp/retrain_logs',
        help='Where to save summary logs for TensorBoard.')
    parser.add_argument(
        '--wanted_words',
        type=str,
        default='yes,no,up,down,left,right,on,off,stop,go',
        help='Words to use (others will be added to an unknown label)',)
    parser.add_argument(
        '--train_dir',
        type=str,
        default='./data/speech_commands_train',
        help='Directory to write event logs and checkpoint.')
    parser.add_argument(
        '--start_checkpoint',
        type=str,
        default='./data/speech_commands_train/conv.ckpt-18000',
        help='If specified, restore this pretrained model before any training.')
    parser.add_argument(
        '--model_architecture',
        type=str,
        default='conv',
        help='What model architecture to use')
    parser.add_argument(
        '--check_nans',
        type=bool,
        default=False,
        help='Whether to check for invalid numbers during processing')
    parser.add_argument(
        '--save_features',
        type=bool,
        default=False,
        help='Whether to check for invalid numbers during processing')
    parser.add_argument(
        '--print_outputs',
        type=bool,
        default=False,
        help='Whether to check for invalid numbers during processing')
    return parser



def main():
    parser = create_parser()
    argcomplete.autocomplete(parser)
    args = parser.parse_args()
    print_outputs = args.print_outputs

    sess = tf.InteractiveSession()

    model_settings = models.prepare_model_settings(
        len(input_data.prepare_words_list(args.wanted_words.split(','))),
        args.sample_rate, args.clip_duration_ms, args.window_size_ms,
        args.window_stride_ms, args.dct_coefficient_count)
    audio_processor = input_data.AudioProcessor(
        args.data_url, args.data_dir, args.silence_percentage,
        args.unknown_percentage,
        args.wanted_words.split(','), args.validation_percentage,
        args.testing_percentage, model_settings)

    label_count = model_settings['label_count']
    fingerprint_size = model_settings['fingerprint_size']
    fingerprint_input = tf.placeholder(
        tf.float32, [None, fingerprint_size], name='fingerprint_input')

    if print_outputs:
        logits, first_conv_val, first_weights, first_bias, second_weights, second_bias, third_weights, second_conv_val = models.create_model(
            fingerprint_input,
            model_settings,
            args.model_architecture,
            is_training=False,print_outputs=True)
    else:

        logits = models.create_model(
            fingerprint_input,
            model_settings,
            args.model_architecture,
            is_training=False,
            print_outputs=False
        )
        
    # load weights/biases from checkpoint
    models.load_variables_from_checkpoint(sess, args.start_checkpoint)

    # Define loss and optimizer
    ground_truth_input = tf.placeholder(
        tf.float32, [None, label_count], name='groundtruth_input')



    predicted_indices = tf.argmax(logits, 1)
    expected_indices = tf.argmax(ground_truth_input, 1)
    correct_prediction = tf.equal(predicted_indices, expected_indices)
    confusion_matrix = tf.confusion_matrix(expected_indices, predicted_indices)
    evaluation_step = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    tf.summary.scalar('accuracy', evaluation_step)


    
    #generate test outputs
    set_size = audio_processor.set_size('testing')
    tf.logging.info('set_size=%d', set_size)
 
    batch_size = args.batch_size
    directory = args.directory
    test_fingerprints, test_ground_truth = audio_processor.get_data(
        batch_size, 0, model_settings, 0.0, 0.0, 0, 'testing', sess, True)

    if print_outputs:
        outfc, outconv1, weights1, bias1, weights2, bias2, weights3,  outconv2, test_accuracy, expected, predicted = sess.run(
            [logits, first_conv_val, first_weights, first_bias, second_weights, second_bias, third_weights, second_conv_val, evaluation_step, expected_indices, predicted_indices],
            feed_dict={
                fingerprint_input: test_fingerprints,
                ground_truth_input: test_ground_truth
                #dropout_prob: 1.0
            })
    else:
        outfc,  test_accuracy, expected, predicted = sess.run(
            [logits,  evaluation_step, expected_indices, predicted_indices],
            feed_dict={
                fingerprint_input: test_fingerprints,
                ground_truth_input: test_ground_truth
                #dropout_prob: 1.0
            })


        
    print("expected/predicted")
    print(expected)
    print(predicted)

    # print image in a .h file, to be include
    for img_num in range(batch_size):
        in_feat = np.array(98*40)
        #print(test_fingerprints[img_num]*64)
        in_feat = np.reshape(test_fingerprints[img_num]*64+0.5,(98*40))
        for i in range(0,40*98):
            in_feat[i] = math.floor(in_feat[i])

        in_feat_int = np.array(98*40)
        in_feat_int = in_feat.astype(int)
        
        format='%d'
        np.savetxt("./data/in_feat_{}_{}.txt".format(img_num,expected[img_num]), in_feat_int, delimiter=", ",newline=",\n", fmt=format)
   
    if print_outputs:
        #outconv1_2D = np.reshape(outconv1,(batch_size*32,79*33))
        outconv1_2D = np.reshape(outconv1,(batch_size*32,39*16))
        first_weights_2D = np.reshape(weights1,(32,8*20))
        weights2=np.reshape(weights2,(10*4,32,32))
        second_weights_2D = np.reshape(weights2.transpose(),(32*32,4*10))
        weights3 = np.reshape(weights3,(13*30,32,12))
        print("SHAPE WEIGHTS3")
        print(np.shape(weights3))
        third_weights_2D = np.reshape(weights3.transpose(),(12,32*13*30))
        outconv2_2D = np.reshape(outconv2,(batch_size*32,30*13))
        print("SHAPE WEIGHTS2")
        print(np.shape(weights2))
        
        np.savetxt("./data/outFC.txt",outfc, delimiter=",")
        np.savetxt("./data/outConv1.txt", outconv1_2D, delimiter=",")
        np.savetxt("./data/weights1.txt", first_weights_2D, delimiter=",")
        np.savetxt("./data/bias1.txt", bias1, delimiter=",")
        np.savetxt("./data/weights2.txt", second_weights_2D*1024*32, delimiter=",")
        np.savetxt("./data/bias2.txt", bias2, delimiter=",")
        np.savetxt("./data/outConv2.txt", outconv2_2D, delimiter=",")
        tf.logging.info('test accuracy = %.1f%% (N=%d)' % (test_accuracy,
                                                           batch_size))
        np.savetxt("./data/weights3.txt", third_weights_2D*1024*32, delimiter=",\n")

    # dump file in a 40*98 pgm image with  16bits pixels
    s_16b = np.array([40*98], dtype=np.uint16)
    #shift left by 7 bits

    for i in range(batch_size):
        s_16b = test_fingerprints[i]*64 + 0.5
        #print(s_16b)
        with open(os.path.join(directory, "features_{}_{}.pgm".format(i,expected[i])), 'wb') as f:
            hdr =  'P5' + '\n' + str(40) + '  ' + str(98) + '  ' + str(65335) + '\n'
            f.write(hdr.encode())
            np.uint16(s_16b).tofile(f)

          
    print("finished: test accuracy = %.1f%%" % (test_accuracy*100))
    
if __name__ == "__main__":
    main()
