#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import argparse

import argcomplete
from tensorflow import lite


def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='h5_to_tflite')

    parser.add_argument('h5_file',
                        help='Input - Trained model in h5 format')
    parser.add_argument('tflite_file',
                        help='Output - Trained model in TFLITE format')
    return parser

def main():
    parser = create_parser()
    argcomplete.autocomplete(parser)
    args = parser.parse_args()

    converter = lite.TFLiteConverter.from_keras_model_file(args.h5_file)
    converter.target_ops = [lite.OpsSet.TFLITE_BUILTINS,
                            lite.OpsSet.SELECT_TF_OPS]
    print("converting model")
    model = converter.convert()
    print("convert completed - writing file")
    open(args.tflite_file, "wb").write(model)

if __name__ == '__main__':
    main()
