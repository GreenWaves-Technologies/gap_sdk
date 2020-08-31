# pylint: disable=line-too-long
import logging
import os
import tempfile
import numpy as np

from generation.code_generator import CodeGenerator
# from generation.code_generators import gen_conv_pool_relu
from generation.default_template import default_template
from generation.naming_convension import DefaultNamingConvension
from utils.new_param_state import load_state
from importer.tflite.new_tflite_graph_all import TfliteImporter
from graph.matches.matches import get_fusion, get_scale8_match_group
from quantization.multiplicative.mult_quantizer import MultQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.activation_ranges_collector import ActivationRangesCollector
from graph.types import InputParameters, OutputParameters
from importer.tflite.remove_concats import remove_concats

# def test_conv_pool_relu_kernel_gen(mnist_unfused_8bit_state):
#     G = load_state(mnist_unfused_8bit_state)
#     conv_params = G.graph_state.steps[1]['node']
#     relu_params = G.graph_state.steps[2]['node']
#     pool_params = G.graph_state.steps[3]['node']
#     conv_q = G.quantization[NodeId(conv_params)]
#     pool_q = G.quantization[NodeId(pool_params)]
#     relu_q = G.quantization[NodeId(relu_params)]
#     code_block = gen_conv_pool_relu("Test", conv_params, conv_q, None, None, None, None)
#     assert str(code_block) ==\
# 'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_NONE);'
#     code_block = gen_conv_pool_relu("Test", conv_params, conv_q, pool_params, pool_q, relu_params, relu_q)
#     assert str(code_block) ==\
# 'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);'
#     code_block = gen_conv_pool_relu("Test", conv_params, conv_q, None, None, relu_params, relu_q)
#     assert str(code_block) ==\
# 'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_RELU);'
#     code_block = gen_conv_pool_relu("Test", conv_params, conv_q, pool_params, pool_q, None, None)
#     assert str(code_block) ==\
# 'CNN_ConvolutionPoolReLU("Test", 0, 1, 1, 1, 1, 7, 7, 7, 6, 1, 1, 1, 1, 1, 32, 28, 28,\n    KOP_CONV_DP, 5, 5, 1, 1, 1, 1, 0,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_NONE);'
#     code_block = gen_conv_pool_relu("Test", None, None, pool_params, pool_q, relu_params, relu_q)
#     assert str(code_block) ==\
# 'CNN_PoolReLU("Test", 0, 1, 1, 6, 6, 1, 1, 32, 32, 24, 24,\n    KOP_MAXPOOL, 2, 2, 1, 1, 2, 2, 0, KOP_RELU);'
#     code_block = gen_conv_pool_relu("Test", None, None, None, None, relu_params, relu_q)
#     assert str(code_block) ==\
# 'CNN_PoolReLU("Test", 0, 1, 1, 6, 6, 1, 1, 32, 32, 24, 24,\n    KOP_NONE, 0, 0, 0, 0, 0, 0, 0, KOP_RELU);'

def test_unfused_operational(caplog, mnist_unfused_8bit_state):
    caplog.set_level(logging.DEBUG)
    G = load_state(mnist_unfused_8bit_state)
    opts = {
        'default_input_location': 'ARG_LOC_L2',
        'default_output_location': 'ARG_LOC_L2',
        'default_global_location': 'ARG_LOC_L3_HFLASH',
        'default_local_location': 'AT_MEM_UNDEF',
    }
    code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
    print(default_template(G, code_generator=code_gen))

def test_fused_operational(caplog, mnist_fused_8bit_state):
    caplog.set_level(logging.INFO)
    G = load_state(mnist_fused_8bit_state)
    opts = {
        'default_input_location': 'ARG_LOC_L2',
        'default_output_location': 'ARG_LOC_L2',
        'default_global_location': 'ARG_LOC_L3_HFLASH',
        'default_local_location': 'AT_MEM_UNDEF',
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
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()
        files_list = [f for f in os.listdir(tempdir) if os.path.isfile(os.path.join(tempdir, f))]
        assert set(files_list) == set(['S2_Weights.tensor',
                                       'S1_Weights.tensor',
                                       'S1_Biases.tensor',
                                       'S3_Weights.tensor',
                                       'S2_Biases.tensor',
                                       'S3_Biases.tensor'])


def test_gen_vergesense(caplog):
    caplog.set_level(logging.DEBUG)
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/marco_17_04.tflite", {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()

def test_gen_mobv2_quant_from_keras(caplog):
    caplog.set_level(logging.DEBUG)
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/model_quantized.tflite", {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()

def test_gen_ssdlite(caplog):
    caplog.set_level(logging.DEBUG)
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/graph/ssdlite_v2_quant_ocr_nopostprocess.tflite", {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()

def test_gen_mobv2_pool_with_actfusions(caplog):
    caplog.set_level(logging.DEBUG)
    tfi = TfliteImporter()
    G = tfi.create_graph("tests/mobv2_valid/mobv2_vwwvehicle_quant_asym.tflite", {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    G.adjust_order()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        default_template(G, code_generator=code_gen)
        code_gen.write_constants()

def test_activatiofusion(actfusion_graph):
    G = actfusion_graph
    matcher = get_fusion('scale8_match_group')
    matcher.match(G)
    G.add_dimensions()
    astat_col = ActivationRangesCollector()
    astats = astat_col.collect_stats(G, [np.full([10, 10, 2], 1), np.full([10, 10, 2], 1)])
    quantizer = MultQuantizer(astats, force_width=8, quantized_dimension="channel")
    G.quantization = quantizer.quantize(G)
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        ATModel_code = default_template(G, code_generator=code_gen)
        #code_gen.write_constants()

def test_gen_split_concat(split_concat_graph):
    G = split_concat_graph
    G.add_dimensions()
    # for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
    #     node.fixed_order = 1
    G.adjust_order()
    matcher = get_fusion('scale8_match_group')
    matcher.match(G)
    G.add_dimensions()
    astat_col = ActivationRangesCollector()
    astats = astat_col.collect_stats(G, [np.full([12, 10, 10], 1.0)])
    quantizer = MultQuantizer(astats, force_width=8, quantized_dimension="channel")
    G.quantization = quantizer.quantize(G)
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        ATModel_code = default_template(G, code_generator=code_gen)
        X=0
    assert G

def test_gen_pack_stridedslice(pack_stridedslice, caplog):
    caplog.set_level(logging.INFO)
    G = pack_stridedslice
    G.add_dimensions()
    (strided_slices_removed, concats_removed) = remove_concats(G)
    assert len(strided_slices_removed) == 2
    assert len(concats_removed) == 1
    # for node in [n for n in G.nodes() if isinstance(n, (InputParameters, OutputParameters))]:
    #     node.fixed_order = 1
    G.add_dimensions()
    G.adjust_order()
    matcher = get_fusion('scale8_match_group')
    matcher.match(G)
    G.add_dimensions()
    astat_col = ActivationRangesCollector()
    astats = astat_col.collect_stats(G, [np.full([10, 10], 1.0), np.full([10, 10], 1.0)])
    quantizer = MultQuantizer(astats, force_width=8, quantized_dimension="channel")
    G.quantization = quantizer.quantize(G)
    with tempfile.TemporaryDirectory() as tempdir:
        opts = {
            'default_input_location': 'ARG_LOC_L2',
            'default_output_location': 'ARG_LOC_L2',
            'default_global_location': 'ARG_LOC_L3_HFLASH',
            'default_local_location': 'AT_MEM_UNDEF',
            'tensor_directory': tempdir
        }
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
        ATModel_code = default_template(G, code_generator=code_gen)
        X=0
    assert G