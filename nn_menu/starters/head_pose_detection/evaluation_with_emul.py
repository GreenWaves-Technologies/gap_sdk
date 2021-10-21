#!/usr/bin/env python3
import re
import os
import argparse
from sklearn.metrics import confusion_matrix

HEAD_THR = -44 # 0.1
LOOK_THR = 13 # 0.68
LOG_MATCH = re.compile(r'input:\t(?P<img>.+)\tHEAD\((?P<head_score>-?[0-9]+)\)\tLOOK\((?P<look_score>-?[0-9]+)\)\n')

parser = argparse.ArgumentParser(description='Evaluate in emul mode for some images')
parser.add_argument('exe_file', help='path to executable of emul mode')
parser.add_argument('--dataset_path', default='images/ofilm',
                    help='path to the dataset to test (default: images/ofilm)')
parser.add_argument('--already_resized', action='store_true',
					help='operates on images which are already resized to the AT input sizes')
parser.add_argument('--print_wrong', action='store_true',
					help='print wrong predicted imgs')

def main():
	args = parser.parse_args()
	exe_file = args.exe_file
	dataset_dir = args.dataset_path

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
			stream = os.popen('./{} {}'.format(exe_file, img_path))
			try:
				out_log = stream.readlines()[0]
			except:
				continue
			match = LOG_MATCH.search(out_log)
			if match:
				HEAD['pred'].append((int(match['head_score']) > HEAD_THR))
				HEAD['true'].append(head_label)
				if (int(match['head_score']) > HEAD_THR) != head_label and args.print_wrong:
					print('\twrong head/no head:\t', img_path)
				if 'cm' in file:
					distance = file[0:2]
					HEAD_DISTANCE_EVAL[distance].append((int(match['head_score']) > HEAD_THR) != head_label)
				if head_label:
					LOOK['pred'].append(int(match['look_score']) > LOOK_THR)
					LOOK['true'].append(look_label)
					if (int(match['look_score']) > LOOK_THR) != look_label and args.print_wrong:
						print('\twrong look/no look:\t', img_path)
					if 'cm' in file:
						LOOK_DISTANCE_EVAL[distance].append((int(match['look_score']) > LOOK_THR) != look_label)

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
