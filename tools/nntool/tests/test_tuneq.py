from utils.new_param_state import load_state
from quantization.tuneq import tuneq

def test_tune_state(mnist_unfused_16bit_state):
    G = load_state(mnist_unfused_16bit_state)
    tuneq(G, G.quantization, 1, 'acc', 3, 13)
