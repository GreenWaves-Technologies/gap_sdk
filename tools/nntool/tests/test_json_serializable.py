import json
from graph.matches.matches import get_scale8_match_group
from generation.autotiler_options import DEFAULT_GEN_OPTS
from importer.tflite.new_tflite_graph_all import TfliteImporter
from interpreter.commands.imageformat import insert_formatter
from utils.json_serializable import JsonSerializable, JsonSerializableStateDecoder, JsonSerializableStateEncoder
from utils.new_param_state import dump_state, load_state
from utils.node_id import NodeId

class TestClass(JsonSerializable):
    def __init__(self, param):
        super(TestClass, self).__init__()
        self._param = param

    def _encapsulate(self):
        return self._param

    @classmethod
    def _dencapsulate(cls, val):
        return TestClass(val)

    def __eq__(self, value):
        return self._param == value._param

def test_json_serializer():
    val1 = TestClass("test")
    json_string = json.dumps(val1, cls=JsonSerializableStateEncoder)
    val2 = json.loads(json_string, cls=JsonSerializableStateDecoder)
    assert val1 == val2

def test_load_imageformat(qvww_graph):
    tfi = TfliteImporter()
    G = tfi.create_graph(qvww_graph, {'load_tensors': True, 'load_quantization': True})
    G.add_dimensions()
    # adjust
    G.adjust_order()
    # fusions --scale8
    matcher = get_scale8_match_group()
    matcher.match(G)
    G.add_dimensions()
    # imageformat input_1 rgb888 offset_int8
    input_node = G['input_1']
    out_edge = G.out_edges(input_node.name)[0]
    G.changes.image_format(input_node.name, "rgb888", "offset_int8")
    insert_formatter(G, out_edge, "rgb888", "offset_int8")
    G.add_dimensions()
    # nodeoption 0 EXTERN_INPUT_POINTER 1
    node = G.graph_state.steps[0]['node']
    node_options = node.at_options
    setattr(node_options, "EXTERN_INPUT_POINTER", 1)
    G.node_options[NodeId(node)] = node_options
    # save_state
    dump_state(G, state_path="/tmp/test_qvww", extra=DEFAULT_GEN_OPTS)
    # load_state
    G_loaded = load_state("/tmp/test_qvww")
    assert len(G.nodes()) == len(G_loaded.nodes())
