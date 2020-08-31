import os
from glob import glob
from shutil import rmtree
from tempfile import mkdtemp

import numpy as np
import pytest

from graph.dim import Conv2DFilterDim, Dim, PadDim, StrideDim
from graph.matches.matches import get_pow2_match_group
from graph.nngraph import NNGraph
from graph.types import (Conv2DParameters, NNEdge, MatrixAddParameters,
                         ReluActivationParameters, ConcatParameters, SplitParameters,
                         ReshapeParameters, StridedSliceParameters)
from importer.importer import create_graph
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.filter_stats_collector import FilterStatsCollector
from utils.data_importer import import_data
from utils.new_param_state import dump_state
from utils.sparse_list import SparseList
from utils.node_id import NodeId
from quantization.quantization_set import QuantizationSet
from quantization.multiplicative.mult_quantization import (MultScalableFilterQuantizationRecord,
                                                           MultAddQuantizationRecord)
from quantization.multiplicative.symmetric.symmetric_mult_biases_qtype import SymmetricMultBiasesQType
from quantization.multiplicative.symmetric.symmetric_mult_qtype import SymmetricMultQType

MNIST_GRAPH = 'tests/graph/mnist_model.tflite'
IR_GRAPH = 'tests/graph/ir_model.tflite'
SSD_GRAPH = 'tests/graph/ssd_model.tflite'
CIFAR10_GRAPH = 'tests/graph/cifar10_model.tflite'
KWS_GRAPH = 'tests/graph/kws.tflite'
VISUAL_GRAPH = 'tests/graph/visual_wake.tflite'
CONCAT_TEST_GRAPH = 'tests/graph/concat_test.tflite'
QVISUAL_GRAPH = 'tests/graph/model_quantized.tflite'
MN3_GRAPH = 'tests/graph/v3-large_224_1.0_float.tflite'
MN2_VWW_SYM_Q_GRAPH = "tests/mobv2_valid/mobv2_vww_quant_sym.tflite"
MN3Q_GRAPH = 'tests/graph/v3-large_224_1.0_uint8.tflite'
MN3Q2_GRAPH = 'tests/graph/mn3_large_quant_tf2_no_train.tflite'
MN2_GRAPH = 'tests/graph/vergesense_mnv2.tflite'
MN1Q_GRAPH = 'tests/graph/mobv1_quant.tflite'
MN1F_GRAPH = 'tests/graph/mobv1_float.tflite'
LPRNET_Q_GRAPH = 'tests/graph/lprnet.tflite'
SQUEEZE_GRAPH = 'tests/graph/squeezenet.tflite'
QSSD_MBV1_GRAPH = 'tests/graph/ssd_mobv1_quant.tflite'
CONV1D_GRAPH = "tests/graph/conv1d.tflite"

MNIST_IMAGES = glob('tests/images/136.pgm')
VWW_IMAGES = glob('tests/vwwimages/*.png')


def save_state(temp_dir, width, fusions=False, adjust=False):
    file_name = os.path.join(temp_dir, "state_file")
    G = create_graph(MNIST_GRAPH, opts={"load_tensors": True})
    G.add_dimensions()
    if adjust:
        G.adjust_order()
    if fusions:
        get_pow2_match_group().match(G)
        G.add_dimensions()
    stats_collector = ActivationStatsCollector()
    for input_file in MNIST_IMAGES:
        data = import_data(input_file, offset=0, divisor=255)
        if not adjust:
            data = data.reshape((28, 28, 1))
        stats_collector.collect_stats(G, [data])
    astats = stats_collector.reduce_stats()
    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)
    quantizer = SymmetricQuantizer(astats, fstats, force_width=width)
    qrecs = quantizer.quantize(G)
    G.quantization = qrecs
    dump_state(G, include_parameters=True, state_path=file_name)
    return file_name


@pytest.fixture()
def two_conv_graph():
    G = NNGraph(name='two_conv_graph')
    ti = G.add_input(Dim.unnamed([10, 10, 2])).name
    c1filt = Conv2DFilterDim(3, 3, 2, in_c=2)
    c1filt.impose_order(['out_c', 'h', 'w', 'in_c'])
    n1 = Conv2DParameters("node1",
                          filt=c1filt,
                          stride=StrideDim(1, 1),
                          padding=PadDim(0),
                          in_dims_hint=SparseList([['h', 'w', 'c']]),
                          out_dims_hint=SparseList([['h', 'w', 'c']]))
    G.add_node(n1)
    w1 = [[0.25, 0.25], [0.25, 0.25], [0.25, 0.25]]
    w1 = [w1, w1, w1]
    w2 = [[0.75, 0.75], [0.75, 0.75], [0.75, 0.75]]
    w2 = [w2, w2, w2]
    n1.weights = np.array([w1, w2])
    c2filt = Conv2DFilterDim(3, 3, 2, in_c=2)
    c2filt.impose_order(['out_c', 'h', 'w', 'in_c'])
    n2 = Conv2DParameters("node2",
                          filt=c2filt,
                          stride=StrideDim(1, 1),
                          padding=PadDim(0),
                          in_dims_hint=SparseList([['h', 'w', 'c']]),
                          out_dims_hint=SparseList([['h', 'w', 'c']]))
    G.add_node(n2)
    w3 = [[0.75, 0.25], [0.75, 0.25], [0.75, 0.25]]
    w3 = [w3, w3, w3]
    n2.weights = np.array([w3, w3])
    to = G.add_output()
    G.add_edge(NNEdge(ti, n1))
    G.add_edge(NNEdge(n1, n2))
    G.add_edge(NNEdge(n2, to))
    G.add_dimensions()
    yield G


@pytest.fixture()
def actfusion_graph():
    G = NNGraph(name='actfusion_graph')
    ti1 = G.add_input(Dim.unnamed([10, 10, 2])).name
    ti2 = G.add_input(Dim.unnamed([10, 10, 2])).name
    c1filt = Conv2DFilterDim(3, 3, 2, in_c=2)
    c1filt.impose_order(['out_c', 'h', 'w', 'in_c'])
    n1 = Conv2DParameters("node1",
                          filt=c1filt,
                          stride=StrideDim(1, 1),
                          padding=PadDim(0),
                          in_dims_hint=SparseList([['h', 'w', 'c']]),
                          out_dims_hint=SparseList([['h', 'w', 'c']]))
    G.add_node(n1)
    w1 = [[0.25, 0.25], [0.25, 0.25], [0.25, 0.25]]
    w1 = [w1, w1, w1]
    w2 = [[0.75, 0.75], [0.75, 0.75], [0.75, 0.75]]
    w2 = [w2, w2, w2]
    n1.weights = np.array([w1, w2])
    n1a = ReluActivationParameters("node1a")
    G.add_node(n1a)
    c2filt = Conv2DFilterDim(3, 3, 2, in_c=2)
    c2filt.impose_order(['out_c', 'h', 'w', 'in_c'])
    n2 = Conv2DParameters("node2",
                          filt=c2filt,
                          stride=StrideDim(1, 1),
                          padding=PadDim(0),
                          in_dims_hint=SparseList([['h', 'w', 'c']]),
                          out_dims_hint=SparseList([['h', 'w', 'c']]))
    G.add_node(n2)
    w3 = [[0.75, 0.25], [0.75, 0.25], [0.75, 0.25]]
    w3 = [w3, w3, w3]
    n2.weights = np.array([w3, w3])
    n3 = MatrixAddParameters("node3")
    G.add_node(n3)
    n4 = ReluActivationParameters("node4")
    G.add_node(n4)
    to = G.add_output()
    G.add_edge(NNEdge(ti1, n1))
    G.add_edge(NNEdge(n1, n1a))
    G.add_edge(NNEdge(ti2, n2))
    G.add_edge(NNEdge(n1a, n3, to_idx=0))
    G.add_edge(NNEdge(n2, n3, to_idx=1))
    G.add_edge(NNEdge(n3, n4))
    G.add_edge(NNEdge(n4, to))
    G.add_dimensions()
    yield G


@pytest.fixture()
def split_concat_graph():
    G = NNGraph(name='split_concat_graph')
    inp1 = G.add_input(Dim.unnamed([12, 10, 10]))
    split = SplitParameters(
        "split1",
        act_slices=[[(0, 2, 1), (0, 10, 1), (0, 10, 1)],
                    [(2, 4, 1), (0, 10, 1), (0, 10, 1)],
                    [(4, 6, 1), (0, 10, 1), (0, 10, 1)],
                    [(6, 8, 1), (0, 10, 1), (0, 10, 1)],
                    [(8, 10, 1), (0, 10, 1), (0, 10, 1)],
                    [(10, 12, 1), (0, 10, 1), (0, 10, 1)]],
        out_shapes=[(2, 10, 10), (2, 10, 10), (2, 10, 10), (2, 10, 10), (2, 10, 10), (2, 10, 10)],
        axis=0
    )
    add1 = MatrixAddParameters("add1")
    add2 = MatrixAddParameters("add2")
    concat = ConcatParameters("concat1", axis=0)
    out1 = G.add_output()
    G.add_edge(NNEdge(inp1, split))
    G.add_edge(NNEdge(split, add1, from_idx=0, to_idx=0))
    G.add_edge(NNEdge(split, add1, from_idx=1, to_idx=1))
    G.add_edge(NNEdge(split, concat, from_idx=2, to_idx=0))
    G.add_edge(NNEdge(split, add2, from_idx=3, to_idx=0))
    G.add_edge(NNEdge(split, add2, from_idx=4, to_idx=1))
    G.add_edge(NNEdge(split, concat, from_idx=5, to_idx=2))

    # in_idx=0 above from split idx 2
    G.add_edge(NNEdge(add1, concat, to_idx=1))
    # in_idx=2 above from split idx 5
    G.add_edge(NNEdge(add2, concat, to_idx=3))

    G.add_edge(NNEdge(concat, out1))
    G.add_dimensions()
    yield G

@pytest.fixture()
def pack_stridedslice():
    G = NNGraph(name='crazy_graph')
    inp1 = G.add_input(Dim.unnamed([10, 10]))
    inp2 = G.add_input(Dim.unnamed([10, 10]))
    res1 = ReshapeParameters("res1", old_shape=[10, 10], shape=[1, 10, 10])
    res2 = ReshapeParameters("res2", old_shape=[10, 10], shape=[1, 10, 10])
    pack1 = ConcatParameters("pack1", axis=0)
    slice1 = StridedSliceParameters("slice1", act_slice=[(0, 1, 1), (0, 10, 1), (0, 10, 1)], out_shape=(1, 10, 10))
    slice2 = StridedSliceParameters("slice2", act_slice=[(1, 2, 1), (0, 10, 1), (0, 10, 1)], out_shape=(1, 10, 10))
    add1 = MatrixAddParameters("add1")
    out1 = G.add_output()
    G.add_edge(NNEdge(inp1, res1))
    G.add_edge(NNEdge(inp2, res2))
    G.add_edge(NNEdge(res1, pack1))
    G.add_edge(NNEdge(res2, pack1, to_idx=1))
    G.add_edge(NNEdge(pack1, slice1))
    G.add_edge(NNEdge(pack1, slice2))
    G.add_edge(NNEdge(slice1, add1, from_idx=0, to_idx=0))
    G.add_edge(NNEdge(slice2, add1, from_idx=0, to_idx=1))
    G.add_edge(NNEdge(add1, out1))
    G.add_dimensions()
    yield G


@pytest.fixture(scope="session")
def mnist_unfused_16bit_state():
    temp_dir = mkdtemp()
    yield save_state(temp_dir, 16)
    rmtree(temp_dir)


@pytest.fixture(scope="session")
def mnist_unfused_8bit_state():
    temp_dir = mkdtemp()
    yield save_state(temp_dir, 8)
    rmtree(temp_dir)


@pytest.fixture(scope="session")
def mnist_fused_8bit_state():
    temp_dir = mkdtemp()
    yield save_state(temp_dir, 8, True, True)
    rmtree(temp_dir)


@pytest.fixture(scope="session")
def vww_graph():
    yield VISUAL_GRAPH


@pytest.fixture(scope="session")
def qvww_graph():
    yield QVISUAL_GRAPH


@pytest.fixture(scope="session")
def mobv2_symq_graph():
    yield MN2_VWW_SYM_Q_GRAPH


@pytest.fixture(scope="session")
def mnist_graph():
    yield MNIST_GRAPH


@pytest.fixture(scope="session")
def ir_graph():
    yield IR_GRAPH


@pytest.fixture(scope="session")
def ssd_graph():
    yield SSD_GRAPH


@pytest.fixture(scope="session")
def cifar10_graph():
    yield CIFAR10_GRAPH


@pytest.fixture(scope="session")
def kws_graph():
    yield KWS_GRAPH


@pytest.fixture(scope="session")
def concat_test_graph():
    yield CONCAT_TEST_GRAPH


@pytest.fixture(scope="session")
def mn3_graph():
    yield MN3_GRAPH


@pytest.fixture(scope="session")
def mn3q_graph():
    yield MN3Q_GRAPH


@pytest.fixture(scope="session")
def mn3q2_graph():
    yield MN3Q2_GRAPH


@pytest.fixture(scope="session")
def mn2_graph():
    yield MN2_GRAPH


@pytest.fixture(scope="session")
def mn1q_graph():
    yield MN1Q_GRAPH


@pytest.fixture(scope="session")
def mn1f_graph():
    yield MN1F_GRAPH


@pytest.fixture(scope="session")
def lprnet_graph():
    yield LPRNET_Q_GRAPH


@pytest.fixture(scope="session")
def squeezenet_graph():
    yield SQUEEZE_GRAPH


@pytest.fixture(scope="session")
def ssd_mobv1_quant_graph():
    yield QSSD_MBV1_GRAPH


@pytest.fixture(scope="session")
def conv1d_graph():
    yield CONV1D_GRAPH

@pytest.fixture(scope="session")
def ir_images():
    yield [
        "tests/images/im1.png",
        "tests/images/im2.png",
        "tests/images/im3.png",
        "tests/images/im4.png",
    ]


@pytest.fixture(scope="session")
def kws_sounds():
    yield [
        "tests/sounds/feature.txt_475.pgm",
        "tests/sounds/feature.txt_737.pgm",
        "tests/sounds/feature.txt_99.pgm",
        "tests/sounds/feature.txt_476.pgm",
        "tests/sounds/feature.txt_738.pgm",
        "tests/sounds/feature.txt_9.pgm"
    ]


@pytest.fixture(scope="session")
def mnist_images():
    yield MNIST_IMAGES


@pytest.fixture(scope="session")
def vww_images():
    yield VWW_IMAGES
