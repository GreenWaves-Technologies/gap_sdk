# pylint: disable=line-too-long
import logging
import os
import tempfile

from generation.code_generator import CodeGenerator
from generation.code_generators import gen_conv_pool_relu
from generation.default_template import default_template
from generation.naming_convension import DefaultNamingConvension
from utils.new_param_state import load_state
from utils.node_id import NodeId

def test_conv_pool_relu_kernel_gen(mnist_unfused_8bit_state):
    G = load_state(mnist_unfused_8bit_state)
    conv_params = G.graph_state.steps[1]['node']
    relu_params = G.graph_state.steps[2]['node']
    pool_params = G.graph_state.steps[3]['node']
    conv_q = G.quantization[NodeId(conv_params)]
    pool_q = G.quantization[NodeId(pool_params)]
    relu_q = G.quantization[NodeId(relu_params)]
    code_block = gen_conv_pool_relu("Test", conv_params, conv_q, None, None, None, None)
    assert str(code_block) ==\
'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_NONE);'
    code_block = gen_conv_pool_relu("Test", conv_params, conv_q, pool_params, pool_q, relu_params, relu_q)
    assert str(code_block) ==\
'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);'
    code_block = gen_conv_pool_relu("Test", conv_params, conv_q, None, None, relu_params, relu_q)
    assert str(code_block) ==\
'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_RELU);'
    code_block = gen_conv_pool_relu("Test", conv_params, conv_q, pool_params, pool_q, None, None)
    assert str(code_block) ==\
'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_NONE);'
    code_block = gen_conv_pool_relu("Test", None, None, pool_params, pool_q, relu_params, relu_q)
    assert str(code_block) ==\
'CNN_PoolReLU("Test", 0, 1, 1, 6, 6, 1, 1, 32, 32, 24, 24,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);'
    code_block = gen_conv_pool_relu("Test", None, None, None, None, relu_params, relu_q)
    assert str(code_block) ==\
'CNN_PoolReLU("Test", 0, 1, 1, 6, 6, 1, 1, 32, 32, 24, 24,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_RELU);'

def test_unfused_operational(caplog, mnist_unfused_8bit_state):
    caplog.set_level(logging.INFO)
    G = load_state(mnist_unfused_8bit_state)
    opts = {
        'default_input_location': 'ARG_LOC_L2',
        'default_output_location': 'ARG_LOC_L2',
        'default_global_location': 'ARG_LOC_L3_HFLASH',
        'default_local_location': '0',
    }
    code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
    default_template(G, code_generator=code_gen)

def test_fused_operational(caplog, mnist_fused_8bit_state):
    caplog.set_level(logging.INFO)
    G = load_state(mnist_fused_8bit_state)
    opts = {
        'default_input_location': 'ARG_LOC_L2',
        'default_output_location': 'ARG_LOC_L2',
        'default_global_location': 'ARG_LOC_L3_HFLASH',
        'default_local_location': '0',
    }
    code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
    default_template(G, code_generator=code_gen)

def test_tensor_dump(mnist_fused_8bit_state):
    with tempfile.TemporaryDirectory() as tempdir:
        G = load_state(mnist_fused_8bit_state)
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': '0',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()
        files_list = [f for f in os.listdir(tempdir) if os.path.isfile(os.path.join(tempdir, f))]
        assert set(files_list) == set(['Step2Weights.tensor',
                                       'Step1Weights.tensor',
                                       'Step1Biases.tensor',
                                       'Step3Weights.tensor',
                                       'Step2Biases.tensor',
                                       'Step3Biases.tensor'])
