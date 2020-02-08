#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import argparse
import os
import pickle

import argcomplete
import numpy as np
from keras.models import Model, load_model
from keras.preprocessing import image
from keras import backend as K
from keras.layers.convolutional import Conv2D
from keras.layers.core import Dense, Flatten

def load_image(img_path):
    img = image.load_img(img_path, target_size=(28, 28), color_mode="grayscale")
     # (height, width, channels)
    img_tensor = image.img_to_array(img)
    # (1, height, width, channels), add a dimension because the model
    # expects this shape: (batch_size, height, width, channels)
    img_tensor = np.expand_dims(img_tensor, axis=0)
    img_tensor /= 255.

    return img_tensor

def create_parser():
    # create the top-level parser
    parser = argparse.ArgumentParser(prog='run')

    parser.add_argument('h5_file',
                        default="sample_project/BUILD_TRAIN/mnist_model.h5",
                        nargs=1,
                        help='Trained model in h5 format')

    parser.add_argument('-i', '--image_file',
                        type=str,
                        default="examples/1/1037.pgm",
                        help='Image file')

    parser.add_argument('-D', '--directory',
                        type=str,
                        default=".",
                        help='directory to store pickles in')

    operat = parser.add_mutually_exclusive_group(required=True)

    operat.add_argument('-l', '--list',
                        help='list layer names',
                        action='store_true')
    operat.add_argument('-p', '--print',
                        nargs=1,
                        type=int,
                        help='print the output of a layer number')
    operat.add_argument('-d', '--dump_layers',
                        action='store_true',
                        help='pickles an array of the outputs of each layer')
    operat.add_argument('-P', '--predict',
                        action='store_true',
                        help='print the output of a layer number')
    operat.add_argument('-w', '--dump_weights',
                        type=str,
                        help='pickles an array of the weights of each Conv2D or Dense layer')
    return parser

def list_layers(model):
    model.summary()
    for idx, layer in enumerate(model.layers):
        print("{idx:3d}) {layer.name}".format(idx=idx, layer=layer))

def print_layer(model, layer_num, input_image):
    name = model.layers[layer_num].name
    int_model = Model(inputs=model.input,
                      outputs=model.get_layer(name).output)
    int_output = int_model.predict(input_image)
    print(int_output)

def predict(model, input_image):
    output = model.predict(input_image)
    print(np.argmax(output))

def reorganize_dimensions(output):
    # remove batch dimension and order channel first
    # Also store outputs in an array to be compatible with nntool outputs
    output = output[0]
    if K.image_data_format() == 'channels_last' and len(output.shape) > 1:
        current = [idx for idx in range(len(output.shape))]
        first = current.pop()
        current.insert(0, first)
        output = output.transpose(current)
    return [output]

def dump_layers(model, input_image, filename):
    outputs = [reorganize_dimensions(input_image)]
    for idx in range(len(model.layers)):
        name = model.layers[idx].name
        int_model = Model(inputs=model.input,
                          outputs=model.get_layer(name).output)
        int_output = int_model.predict(input_image)
        outputs.append(reorganize_dimensions(int_output))

    with open(filename, 'wb') as pickle_fp:
        pickle.dump(outputs, pickle_fp)

def dump_weights(model, filename):
    outputs = []
    for layer_idx, layer in enumerate(model.layers):
        if isinstance(layer, Conv2D):
            params = layer.get_weights()
            outputs.append({
                'name': layer.name,
                'weights': params[0].transpose((3, 2, 0, 1)),
                'biases': params[1]
            })
        elif isinstance(layer, Dense):
            params = layer.get_weights()
            # The weights of a Dense layer are in the wrong order to validate against an adjusted
            # graph ready for the AutoTiler. We need to reorder them.
            # The shape of the input to the dense layer has already been flattened.
            # The previous layer should be a flatten which we can use to discover the original shape
            assert isinstance(model.layers[layer_idx - 1], Flatten)
            # use the input shape of the flatten layer to figure out the full shape
            shape = model.layers[layer_idx - 1].input_shape
            in_c = shape[3]
            h = shape[2]
            w = shape[1]
            out_c = layer.output_shape[1]
            # expand the weights to the full shape
            weights = params[0].reshape((h, w, in_c, out_c))
            # AutoTiler kernels expect Activations in c, h, w order
            # The AutoTiler linear layer expects c_out, in_sz
            # So the order of the weights should be c_out, c_in, h, w
            weights = weights.transpose((3, 2, 0, 1))
            # now reshape to c_out, in_sz
            weights = weights.reshape((out_c, in_c * h * w))
            outputs.append({'name': layer.name, 'weights': weights, 'biases': params[1]})
        else:
            outputs.append({'name': layer.name})

    with open(filename, 'wb') as pickle_fp:
        pickle.dump(outputs, pickle_fp)

def main():
    parser = create_parser()
    argcomplete.autocomplete(parser)
    args = parser.parse_args()

    model = load_model(args.h5_file[0])

    if args.list:
        list_layers(model)
        parser.exit(0)
        return
    elif args.dump_weights:
        filename = os.path.join(args.directory,
                                args.dump_weights)
        print("dumping weights to {} ...".format(filename))
        dump_weights(model, filename)
        parser.exit(0)
        return

    if not args.image_file:
        parser.error("you need to specify an image")
        parser.exit(1)
        return

    input_image = load_image(args.image_file)

    if args.print:
        print_layer(model, args.print, input_image)
    elif args.predict:
        predict(model, input_image)
    elif args.dump_layers:
        filename = os.path.join(args.directory,
                                os.path.basename(args.image_file)+'.pickle')
        print("dumping layers to {} ...".format(filename))
        dump_layers(model, input_image, filename)
    parser.exit(0)

if __name__ == "__main__":
    main()
