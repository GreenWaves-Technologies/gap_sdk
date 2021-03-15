import tensorflow as tf
assert tf.__version__.startswith("2")

import os
import numpy as np
import tensorflow as tf
import tensorflow.keras as keras
import tensorflow.keras.layers as layers
from tensorflow.keras.layers import Dense, Input, LSTM, Cropping1D, Flatten, Lambda

# BUILD AND TRAIN LSTM MODEL

inputs = Input(shape=(28, 28), name='input')
x = LSTM(28, return_sequences=True)(inputs)
x = LSTM(64, return_sequences=True)(x)
splits = Lambda(lambda x: tf.split(x, num_or_size_splits=(27, 1), axis=1))(x)
x = Flatten()(x)
outputs = Dense(10, activation="softmax", name='output')(x)

model = tf.keras.Model(inputs=inputs, outputs=outputs, name="mnist_model")
model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])
model.summary()


(x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0
x_train = x_train.astype(np.float32)
x_test = x_test.astype(np.float32)
model.fit(x_train, y_train, epochs=1)
model.evaluate(x_test, y_test)


# EXPORT TO TFLITE
run_model = tf.function(lambda x: model(x))
BATCH_SIZE = 1
STEPS = 28
INPUT_SIZE = 28
concrete_func = run_model.get_concrete_function(
tf.TensorSpec([BATCH_SIZE, STEPS, INPUT_SIZE], model.inputs[0].dtype))
MODEL_DIR = "./saved_model"
model.save(MODEL_DIR, save_format="tf", signatures=concrete_func)
converter = tf.lite.TFLiteConverter.from_saved_model(MODEL_DIR)
converter.target_ops = [tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
tflite = converter.convert()

with open("model/lstm.tflite", 'wb') as f:
    f.write(tflite)
print('Model converted successfully!')