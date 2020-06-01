import tempfile
import shutil
import os

from importer.importer import create_graph
from utils.data_importer import import_data
from utils.new_param_state import load_state, dump_state
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from stats.activation_stats_collector import ActivationStatsCollector
from stats.filter_stats_collector import FilterStatsCollector

def create_temporary_copy(path):
    basename = os.path.basename(path)
    temp_dir = tempfile.gettempdir()
    temp_path = os.path.join(temp_dir, basename)
    shutil.copy2(path, temp_path)
    return temp_path

def test_graph_calc(mnist_graph, mnist_images):
    temp_graph = create_temporary_copy(mnist_graph)
    G = create_graph(temp_graph, opts={"load_tensors":True})
    G.add_dimensions()
    input_tensor = import_data(mnist_images[0], height=28, width=28, divisor=128, offset=-1)
    input_tensor = input_tensor.reshape(28, 28, 1)
    # import data always returns C, H, W. We need H, W, C.

    stats_collector = ActivationStatsCollector()
    stats_collector.collect_stats(G, [input_tensor])
    astats = stats_collector.reduce_stats()

    stats_collector = FilterStatsCollector()
    fstats = stats_collector.collect_stats(G)

    quantizer = SymmetricQuantizer(astats, fstats, force_width=8)
    qrecs = quantizer.quantize(G)

    G.quantization = qrecs

    dump_state(G)

    G = load_state(temp_graph)

    for k, v in G.quantization.items():
        assert v == qrecs[k], "problem with " + str(k)


    assert G.quantization == qrecs
