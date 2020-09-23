#!/usr/bin/env python
# coding: utf-8
import tensorflow as tf
import numpy as np

def main():
	interpreter = tf.lite.Interpreter(model_path='model/expressions.tflite')
	interpreter.allocate_tensors()
	inputs_details = interpreter.get_input_details()
	output_details = interpreter.get_output_details()
	for i, in_detail in enumerate(inputs_details):
		np.random.seed(i)
		input_array = np.random.randint(-128, 127, in_detail['shape']).astype(np.int8)
		input_array.tofile('samples/' + in_detail['name'] + '.dat')
		input_array = input_array.astype(np.float32) / 128
		interpreter.set_tensor(in_detail['index'], input_array)
	interpreter.invoke()
	for out_detail in output_details:
		output_array = interpreter.get_tensor(out_detail['index']).astype(np.float32) / 0.0486875
		output_array.tofile('samples/' + out_detail['name'] + '.dat')

if __name__ == "__main__":
	main()