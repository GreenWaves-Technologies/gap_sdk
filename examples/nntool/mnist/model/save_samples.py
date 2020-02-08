#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK
#

import argparse
import os
import random
import importlib

import argcomplete
import numpy as np

def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='save_samples')
    parser.add_argument('-d', '--directory',
                        default="sample_project/images",
                        help='directory to save samples to')
    parser.add_argument('-n', '--number_of_examples',
                        default=1,
                        type=int,
                        help='number of examples per number')
    parser.add_argument('-s', '--set',
                        help='train or test image set',
                        choices=['train', 'test'],
                        default='test')
    return parser

def write_image(directory, num, img_idx, sample):
    image = importlib.import_module('keras.preprocessing.image')
    sample = np.expand_dims(sample, axis=-1)
    image.save_img(os.path.join(directory, "{}_{}.pgm".format(img_idx, num)), sample)

def save_samples(per_number, directory, use_set='test'):
    datasets = importlib.import_module('keras.datasets')
    (x_train, y_train), (x_test, y_test) = datasets.mnist.load_data()
    (x, y) = (x_train, y_train) if use_set == 'train' else (x_test, y_test)
    index = {k: [] for k in range(10)}
    for idx, value in enumerate(y):
        index[value].append(idx)
    random.seed()
    for idx in range(10):
        sample_set = index[idx]
        for _ in range(per_number):
            img_idx = random.randrange(0, len(sample_set))
            write_image(directory, idx, sample_set[img_idx], x[sample_set[img_idx]])
            del sample_set[img_idx]

def main():
    parser = create_parser()
    argcomplete.autocomplete(parser)
    args = parser.parse_args()

    save_samples(args.number_of_examples, args.directory, args.set)

if __name__ == "__main__":
    main()

