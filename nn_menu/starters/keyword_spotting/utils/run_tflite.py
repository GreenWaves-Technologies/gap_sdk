import tensorflow as tf
import sys
import numpy as np

if len(sys.argv) < 3:
	raise ValueError("give me a path to model and to a file float32 .dat")
else:
	model_path = sys.argv[1]
	file_path = sys.argv[2]


interpreter = tf.lite.Interpreter(model_path)
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

print("Input Details: \n", input_details)
print("Output Details: \n", output_details)
scale, zero_point = input_details[0]['quantization']

tf_mfccs = np.fromfile(file_path, dtype=np.float32)
print(tf_mfccs.shape)
input_array = np.array(np.floor(tf_mfccs / scale + 0.5) + zero_point).astype(np.uint8)
interpreter.set_tensor(input_details[0]['index'], input_array.reshape(input_details[0]['shape']))
interpreter.invoke()
output = interpreter.get_tensor(output_details[0]['index'])
print(output)
print("Predicted: ", np.argmax(output))
