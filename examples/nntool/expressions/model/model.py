# coding: utf-8
import tensorflow as tf
import numpy as np
SHAPES = [
    (1, 12, 1, 80),
    (1, 12, 1, 80),
    (1, 12, 1, 80),
    (1, 12, 1, 80),
    (12, 1, 1),
    (1, 12, 1, 80),
    (1, 12, 1, 80),
    (80),
    (1, 12, 1, 1),
]
if tf.__version__.startswith('1.'):
    inputs = [tf.placeholder(name='input_%s'%idx, dtype=tf.float32, shape=shape) for idx, shape in enumerate(SHAPES[0:4:])]
    constants = [tf.constant(np.full(shape, idx), name='const_%s'%idx, dtype=tf.float32) for idx, shape in enumerate(SHAPES[4::])]
    all_ts = inputs + constants
    x = tf.math.tanh(tf.math.add(all_ts[0], all_ts[1]))  # tanh(in1+in2)
    y = tf.math.maximum(tf.math.minimum(tf.add(tf.math.multiply(
        tf.math.add(all_ts[2], all_ts[3]), 5), 10), 6), 0)  # max(min((y1+10), 6), 0)
    t1 = tf.math.multiply(y, x)
    w = tf.math.maximum(tf.math.minimum(
        tf.add(tf.math.multiply(tf.math.add(all_ts[5], all_ts[6]), 5), 10), 6), 0)
    t2 = tf.math.multiply(all_ts[4], w)
    out_temp_1 = tf.math.add(t1, t2)
    out_1 = tf.identity(out_temp_1, name="output_1")
    z = tf.math.maximum(tf.math.minimum(
        tf.add(tf.math.multiply(tf.math.add(all_ts[7], all_ts[8]), 5), 10), 6), 0)
    out_temp_2 = tf.math.multiply(tf.math.tanh(out_temp_1), z)
    out_2 = tf.identity(out_temp_2, name="output_2")
    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer())
        converter = tf.lite.TFLiteConverter.from_session(
            sess, inputs, [out_1, out_2])
        tflite_model = converter.convert()
        open("model/expressions.tflite", "wb").write(tflite_model)
else:
    @tf.function
    def my_subgraph(in0, in1, in2, in3):
        const = [tf.constant(np.full(shape, idx), dtype=tf.float32) for idx, shape in enumerate(SHAPES[4::])]
        x = tf.math.tanh(tf.math.add(in0, in1))  # tanh(in1+in2)
        y = tf.math.maximum(tf.math.minimum(tf.add(tf.math.multiply(
            tf.math.add(in2, in3), 5), 10), 6), 0)  # max(min((y1+10), 6), 0)
        t1 = tf.math.multiply(y, x)
        w = tf.math.maximum(tf.math.minimum(
            tf.add(tf.math.multiply(tf.math.add(const[1], const[2]), 5), 10), 6), 0)
        t2 = tf.math.multiply(const[0], w)
        out_temp_1 = tf.math.add(t1, t2)
        out_1 = tf.identity(out_temp_1, name="output_1")
        z = tf.math.maximum(tf.math.minimum(
            tf.add(tf.math.multiply(tf.math.add(const[3], const[4]), 5), 10), 6), 0)
        out_temp_2 = tf.math.multiply(tf.math.tanh(out_temp_1), z)
        out_2 = tf.identity(out_temp_2, name="output_2")
        return out_1, out_2
    input_data = [tf.ones(shape) for shape in SHAPES[0:4:]]
    # Convert the model.
    #
    # `from_concrete_function` takes in a list of concrete functions, however,
    # currently only supports converting one function at a time. Converting multiple
    # functions is under development.
    converter = tf.lite.TFLiteConverter.from_concrete_functions([my_subgraph.get_concrete_function(*input_data)])
    tflite_model = converter.convert()
    # Save the TF Lite model.
    with tf.io.gfile.GFile('model/expressions.tflite', 'wb') as f:
        f.write(tflite_model)
