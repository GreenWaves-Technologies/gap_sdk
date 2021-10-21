#!/usr/bin/env python3
import os
import argparse
import numpy as np
import tensorflow as tf
from PIL import Image
from sklearn.metrics import confusion_matrix

HEAD_THR = 0
LOOK_THR = 0
INTERMEDIATE_RESIZE = (220, 165)

parser = argparse.ArgumentParser(description='Evaluate in emul mode for some images')
parser.add_argument('tflite_path', help='path to executable of emul mode')
parser.add_argument('--dataset_path', default='images/ofilm',
					help='path to the dataset to test (default: images/ofilm)')
parser.add_argument('--already_resized', action='store_true',
					help='operates on images which are already resized to the AT input sizes')
parser.add_argument('--print_wrong', action='store_true',
					help='print wrong predicted imgs')

def main():
	args = parser.parse_args()
	model_tflite = args.tflite_path
	dataset_dir = args.dataset_path
	interpreter = tf.lite.Interpreter(model_tflite)
	interpreter.allocate_tensors()

	# Get input and output tensors.
	input_details = interpreter.get_input_details()
	output_details = interpreter.get_output_details()

	# Test the model on random input data.
	input_shape = input_details[0]['shape']

	HEAD = {'pred': [], 'true': []}
	LOOK = {'pred': [], 'true': []}
	distance = None
	HEAD_DISTANCE_EVAL = {'20': [], '30': [], '40': [], '50': [], '60': [], '70': [], '80': []}
	LOOK_DISTANCE_EVAL = {'20': [], '30': [], '40': [], '50': [], '60': [], '70': [], '80': []}
	for directory in os.listdir(dataset_dir):
		if not os.path.isdir(os.path.join(dataset_dir, directory)):
			continue
		print(directory)
		head_label = 'looking' in directory
		look_label = not 'away' in directory and head_label
		for file in os.listdir(os.path.join(dataset_dir, directory)):
			if os.path.splitext(file)[1] != '.ppm':
				continue
			if args.already_resized:
				if not 'resized' in file:
					continue
			else:
				if 'resized' in file:
					continue

			img_path = os.path.join(dataset_dir, directory, file)
			img = Image.open(img_path).convert('L').resize(INTERMEDIATE_RESIZE)
			img = img.crop(((img.size[0]-input_shape[2])//2, (img.size[1]-input_shape[1])//2, \
						    input_shape[2] + (img.size[0]-input_shape[2])//2, input_shape[1] + (img.size[1]-input_shape[1])//2))
			input_data = (np.array(img).astype(np.int32).flatten().reshape(input_shape) - 128).astype(np.int8)

			interpreter.set_tensor(input_details[0]['index'], input_data)
			interpreter.invoke()

			predicted_head = interpreter.get_tensor(output_details[0]['index']).flatten()
			predicted_look = interpreter.get_tensor(output_details[1]['index']).flatten()

			HEAD['pred'].append((int(predicted_head) > HEAD_THR))
			HEAD['true'].append(head_label)
			if (int(predicted_head) > HEAD_THR) != head_label and args.print_wrong:
				print('\twrong head/no head:\t', img_path)
			if 'cm' in file:
				distance = file[0:2]
				HEAD_DISTANCE_EVAL[distance].append((int(predicted_head) > HEAD_THR) != head_label)
			if head_label:
				LOOK['pred'].append(int(predicted_look) > LOOK_THR)
				LOOK['true'].append(look_label)
				if (int(predicted_look) > LOOK_THR) != look_label and args.print_wrong:
					print('\twrong look/no look:\t', img_path)
				if 'cm' in file:
					LOOK_DISTANCE_EVAL[distance].append((int(predicted_look) > LOOK_THR) != look_label)

	print('Face / No face')
	print('\tAccuracy: ', sum([a == b for a, b in zip(HEAD['pred'], HEAD['true'])]) / len(HEAD['pred']))
	print('\tConfusion Matrix: ')
	print(confusion_matrix(HEAD['true'], HEAD['pred']))
	print('Look / No look')
	print('\tAccuracy: ', sum([a == b for a, b in zip(LOOK['pred'], LOOK['true'])]) / len(LOOK['pred']))
	print('\tConfusion Matrix: ')
	print(confusion_matrix(LOOK['true'], LOOK['pred']))
	if distance:
		print('Distance:')
		for (kh, vh), (kl, vl) in zip(HEAD_DISTANCE_EVAL.items(), LOOK_DISTANCE_EVAL.items()):
			if len(vh) > 0:
				print('\t@{}cm\tHead:\t{}/{}\tLook:\t{}/{}'.format(kh, len(vh)-sum(vh), len(vh), len(vl)-sum(vl), len(vl)))

if __name__ == "__main__":
	main()
