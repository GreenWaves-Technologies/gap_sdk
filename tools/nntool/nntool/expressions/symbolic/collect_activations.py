from ast import Constant
from itertools import chain, groupby
from typing import cast
from nntool.expressions.symbolic.assignments import Assignments
from nntool.expressions.symbolic.symbol import QuantizedConstant
from nntool.graph.nngraph import NNGraph
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import FusionNodeBase

def collect_constants(qfuncs):
    assert all(qfuncs[0].__class__ == other .__class__ for other in qfuncs[1:])
    if isinstance(qfuncs[0], (Constant, QuantizedConstant)):
        if all(qfuncs[0].value == other.value for other in qfuncs[1:]):
            return []
        return [[qfuncs]]
    if isinstance(qfuncs[0], Assignments):
        contents = [[func[0][1] for func in qfuncs]]
    else:
        contents = list(zip(*[func.contents for func in qfuncs]))
    return list(chain(collect_constants(qfuncs) for qfuncs in contents))

def collect_activations(G: NNGraph):
    expressions = []
    for node in G.nodes(node_classes=FusionNodeBase):
        node = cast(FusionNodeBase, node)
        if not node.quantize_internals:
            continue
        int_node = node.subgraph.nodes(node_classes=ExpressionFusionNode)
        if len(int_node) != 1:
            continue
        expressions.append((node, int_node[0]))
    grouped = {}
    for pnode, fnode in expressions:
        qrec = G.quantization[(pnode.name, fnode.name)]
        qfunc = qrec.cache['qfunc_col']
        fnode = cast(ExpressionFusionNode, fnode)
        for other, fusions in grouped.items():
            if qfunc.equivalent(other):
                fusions.append((pnode, fnode, qfunc))
                break
        else:
            grouped[qfunc] = [(pnode, fnode, qfunc)]
    for funcs in grouped.values():
        constants = collect_constants([func[2][0][1] for func in funcs])
        x = 0
