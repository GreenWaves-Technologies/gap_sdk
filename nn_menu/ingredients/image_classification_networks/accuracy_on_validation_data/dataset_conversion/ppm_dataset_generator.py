#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK
import os
from PIL import Image
from collections import Counter, OrderedDict
import argparse, argcomplete

def create_parser():
	# create the top-level parser
	parser = argparse.ArgumentParser(prog='generate_synset')

	parser.add_argument('out_header',
						default="../ordered_synset.h",
						nargs=argparse.OPTIONAL,
						help='Output - Trained model in h5 format')
	parser.add_argument('-D', '--create_dataset',
						action='store_true',
						help='if you want to recreate the dataset to ppm format')
	parser.add_argument('-H', '--new_height',
						type=int,
						default=224)
	parser.add_argument('-W', '--new_width',
						type=int,
						default=224)
	parser.add_argument('--original_dataset_path',
						help='where is stored the original dataset')
	parser.add_argument('--converted_dataset_path',
						help='where to store the converted dataset')
	return parser

def create_dataset(ordered_synset_dict, args):
	for i, class_syn in enumerate(ordered_synset_dict.keys()):
		out_folder = os.path.join(args.converted_dataset_path, class_syn)
		if not os.path.exists(out_folder):
			os.mkdir(out_folder)
		counter = 0
		for j, file in enumerate(os.listdir(os.path.join(args.original_dataset_path, class_syn))):
			filepath = os.path.join(os.path.join(args.original_dataset_path, class_syn), file)
			if os.path.isdir(filepath):
				continue
			filename = os.path.splitext(file)[0]
			out_filepath = os.path.join(out_folder, filename) + '.ppm'
			if os.path.exists(out_filepath):
				continue
			img = Image.open(filepath)

			new_WH_crop = args.new_width # FIXME: I consider equal to args.new_height
			new_WH = int(new_WH_crop /0.875)

			(orig_W, orig_H) = img.size

			if orig_W < orig_H:
			    new_W = new_WH
			    new_H = int(new_WH*orig_H/orig_W)
			else:
			    new_H = new_WH
			    new_W = int(new_WH*orig_W/orig_H)    
			img = img.resize((new_W, new_H),Image.BILINEAR)

			left = (new_W - new_WH_crop)/2
			top = (new_H - new_WH_crop)/2
			right = (new_W + new_WH_crop)/2
			bottom = (new_H + new_WH_crop)/2
			# Crop the center of the image

			img = img.crop((left, top, right, bottom))

			if img.mode == 'L':
				img = img.convert('RGB')
			try:
				img.save(out_filepath)
			except:
				print('Failed to save ', out_filepath)
				continue
			counter += 1
		print("{} class {}: saved {}/{} in folder {}".format(i, class_syn, counter, j+1, out_folder))

def main():
	parser = create_parser()
	argcomplete.autocomplete(parser)
	args = parser.parse_args()

	ordered_synset_dict = OrderedDict()
	for i, line in enumerate(open('tf_synset_name.txt', 'r')):
		if len(line.split(':')) < 2:
			continue
		ordered_synset_dict.update({line.split(':')[0]: line.split(':')[1]})

	print('check there are no repetitions in synset names')
	print([(item, count) for item, count in Counter(ordered_synset_dict.keys()).items() if count > 1])
	print(len(set(ordered_synset_dict.keys())), len(ordered_synset_dict.keys()))

	str_out = "static char *ORDERED_SYNSET[1001] = {\t\\\n"
	str_out += '\t\t\"\",\t\\\n'
	for synset, name in ordered_synset_dict.items():
		if not os.path.exists('./val/' + synset):
			print(synset)
		str_out += '\t\t\"' + synset + '\"' + ',\t\\\n'
	str_out = str_out[:-4] + "};"
	with open(args.out_header, 'w') as f:
		f.write(str_out)

	if args.create_dataset:
		print("Resize images to: H={}, W={}".format(args.new_height,args.new_width))
		print("Saving the converted dataset to: ", args.original_dataset_path)
		create_dataset(ordered_synset_dict, args)

if __name__ == '__main__':
	main()
