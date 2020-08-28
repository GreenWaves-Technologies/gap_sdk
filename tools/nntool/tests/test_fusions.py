import logging
import numpy as np
from PIL import Image

from importer.tflite.new_tflite_graph_all import TfliteImporter
from graph.matches.match_gap_conv import MatchAllGapConv
from graph.matches.matches import get_pow2_match_group, get_scale8_match_group
from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode

def test_fusions1(mnist_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(mnist_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()


def test_fusions2(ir_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ir_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()

def test_fusions3(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {})
    G.add_dimensions()
    matcher = MatchAllGapConv()
    matcher.match(G)
    G.add_dimensions()

def test_fusions4(ssd_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(ssd_graph, {})
    G.add_dimensions()
    matcher = get_pow2_match_group()
    matcher.match(G)
    G.add_dimensions()

def test_external_biases_sq8(qvww_graph):
    # this model has at the end an external biases layer as constant add
    tfi = TfliteImporter()
    G = tfi.create_graph(qvww_graph, {"load_quantization": True, "load_tensors": True})
    G.add_dimensions()
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    image = 'tests/vwwimages/COCO_val2014_000000174838_1.png'
    img_in = Image.open(image)
    img_in = img_in.resize((238, 208))
    input_tensor = np.array(img_in, dtype=np.uint8)
    input_tensor = (input_tensor.astype(np.float32) - 128) / 128
    executer = GraphExecuter(G, qrecs=G.quantization)
    # check if nntool can execute
    qoutput_tensors = executer.execute([input_tensor], qmode=QuantizationMode.all_dequantize())
    foutput_tensors = executer.execute([input_tensor], qmode=None)
    diff = [q[0]-f[0] for q,f in zip(qoutput_tensors, foutput_tensors)]
    assert max([np.max(d) for d in diff]) < 2.2
