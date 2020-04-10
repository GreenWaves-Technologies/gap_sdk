import os
from glob import glob
from shutil import rmtree
from tempfile import mkdtemp

import numpy as np
import pytest

from graph.dim import Conv2DFilterDim, Dim, FcFilterDim, PadDim, StrideDim
from graph.matches.matches import get_std_match_group
from graph.nngraph import NNGraph
from graph.types import Conv2DParameters, FcParameters, NNEdge
from importer.importer import create_graph
from quantization.simple_auto_quantify import SimpleQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.filter_stats_collector import FilterStatsCollector
from utils.data_importer import import_data
from utils.intermediate_cache import IntermediateCache
from utils.new_param_state import dump_state
from utils.sparse_list import SparseList

MNIST_GRAPH = 'tests/graph/mnist_model.tflite'
IR_GRAPH = 'tests/graph/ir_model.tflite'
SSD_GRAPH = 'tests/graph/ssd_model.tflite'
CIFAR10_GRAPH = 'tests/graph/cifar10_model.tflite'
KWS_GRAPH = 'tests/graph/kws.tflite'
VISUAL_GRAPH = 'tests/graph/visual_wake.tflite'
CONCAT_TEST_GRAPH = 'tests/graph/concat_test.tflite'
QVISUAL_GRAPH = 'tests/graph/model_quantized.tflite'
MN3_GRAPH = 'tests/graph/v3-large_224_1.0_float.tflite'

MNIST_IMAGES = glob('tests/images/*.pgm')
VWW_IMAGES = glob('tests/vwwimages/*.png')

def save_state(temp_dir, width, fusions=False, adjust=False):
    file_name = os.path.join(temp_dir, "state_file")
    G = create_graph(MNIST_GRAPH, opts={"load_tensors":True})
    G.add_dimensions()
    if adjust:
        G.adjust_order()
    if fusions:
        get_std_match_group().match(G)
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
    quantizer = SimpleQuantizer(astats, fstats, force_width=width)
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
def value_cache():
    temp_dir = mkdtemp()
    yield IntermediateCache(temp_dir)
    rmtree(temp_dir)

@pytest.fixture(scope="session")
def vww_graph():
    yield VISUAL_GRAPH

@pytest.fixture(scope="session")
def qvww_graph():
    yield QVISUAL_GRAPH

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
