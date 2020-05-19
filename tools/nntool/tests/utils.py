from execution.executer import Executer
from utils.node_id import NodeId
from collections import namedtuple
from utils.graph import Edge

def generate_header():
    pass

def fake_execute_iterator(G, input_tensors, disable_cache=None, limit=None, qrecs=None):
    del disable_cache, limit
    for idx, node in enumerate(G.nodes()):
        if qrecs:
            qrec = qrecs[NodeId(node, None)]
        else:
            qrec = None
        input_tensors, details = Executer.execute(node, input_tensors, qrec=qrec)
        yield idx, None, node, input_tensors, None, None, details

class FakeGraph():
    def __init__(self, nodes):
        self._nodes = nodes
        self._in_edges = {}
        self._out_edges = {}
        last_edge = None
        for idx, node in enumerate(nodes):
            node.step_idx = idx
            if last_edge:
                last_edge.to_idx = 0
                last_edge.to_node = node
                last_edge.params = node
                self._in_edges[node.name] = [last_edge]
                self._out_edges[last_edge.from_node.name] = [[last_edge]]
            last_edge = Edge(from_node=node, to_node=None)
        self._out_edges[self._nodes[-1].name] = []

    def nodes(self):
        return self._nodes

    def nodes_iterator(self, _):
        return [(node.step_idx, node, None, None) for node in self._nodes]

    def execute_all(self, input_tensors, qrecs=None, dequantize=False):
        for _, _, _, output_tensors, _, _, _ in fake_execute_iterator(self, input_tensors, qrecs=qrecs):
            result = output_tensors[0]
        if qrecs and dequantize:
            result = qrecs[NodeId(self._nodes[-1])].out_qs[0].dequantize(result)
        return result

    def indexed_out_edges(self, name):
        return self._out_edges[name]

    def indexed_in_edges(self, name):
        return self._in_edges[name]

    @property
    def graph_state(self):
        return namedtuple('GraphState', ['steps'])(steps=[{'node': node} for node in self._nodes])
