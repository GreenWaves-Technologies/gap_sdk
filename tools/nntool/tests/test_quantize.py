import numpy as np
from utils.data_importer import import_data
from utils.stats_funcs import bits

IMAGE_FILE = "examples/0/136.pgm"

# def test_quantization():
#     arr = np.array([0.5])
#     qarr = NumpyFloatToFixConverter(True, 16, 13)(arr)
#     dqarr = NumpyFixToFloatConverter(13)(qarr)
#     assert dqarr == arr

# def test_image_scale():
#     input_tensor = import_data(IMAGE_FILE, height=28, width=28, divisor=128, offset=-1)
#     qinput_tensor = quantize(input_tensor, QType(bits=8, q=7, signed=True))
#     print(qinput_tensor[0][5])

def test_bits():
    assert bits(0.081599854, -0.07628916) == 1
    assert bits(1.081599854, -0.07628916) == 2
    assert bits(2.081599854, -0.07628916) == 3
    assert bits(256.081599854, -0.07628916) == 10
    assert bits(0, 0) == 1
