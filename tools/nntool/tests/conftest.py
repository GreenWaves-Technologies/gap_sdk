from glob import glob
from shutil import rmtree
from tempfile import mkdtemp

import os

import pytest

from graph.matches.matches import get_std_match_group
from importer.importer import create_graph
from quantization.simple_quantizer import SimpleQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.filter_stats_collector import FilterStatsCollector
from utils.data_importer import import_data
from utils.intermediate_cache import IntermediateCache
from utils.new_param_state import dump_state

MNIST_GRAPH = 'tests/graph/mnist_model.tflite'
IR_GRAPH = 'tests/graph/ir_model.tflite'
SSD_GRAPH = 'tests/graph/ssd_model.tflite'
CIFAR10_GRAPH = 'tests/graph/cifar10_model.tflite'
MNIST_IMAGES = glob('tests/images/*.pgm')

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
def ir_images():
    yield [
        "tests/images/im1.png",
        "tests/images/im2.png",
        "tests/images/im3.png",
        "tests/images/im4.png",
    ]

@pytest.fixture(scope="session")
def mnist_images():
    yield MNIST_IMAGES
