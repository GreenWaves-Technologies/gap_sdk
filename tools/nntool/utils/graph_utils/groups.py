from functools import reduce
from graph.types.base import NNEdge

from graph.types import FusionInputParameters
from graph.types.fusions import FusionOutputParameters
from utils.graph import GraphView


def match_connected_nodes(G, start_node, match_fn, visited=None, up=False):
    if visited is None:
        visited = set()
    if not match_fn(start_node):
        return []
    in_reses = []
    in_edges = G.in_edges(start_node.name)
    for edge in in_edges:
        if edge in visited:
            continue
        visited.add(edge)
        in_reses.extend(match_connected_nodes(
            G, edge.from_node, match_fn, visited=visited, up=True))
    out_edges = G.out_edges(start_node.name)
    out_reses = []
    for edge in out_edges:
        if edge in visited:
            continue
        visited.add(edge)
        res = match_connected_nodes(G, edge.to_node, match_fn, visited=visited)
        if not res:
            out_reses = None
            break
        out_reses.extend(res)
    # if all the out edges are the right class then we can proceed
    if out_reses is not None:
        return in_reses + [start_node] + out_reses
    # if going up any node that has a output that is not correct class is not usable
    if up:
        return []
    # if going down then cannot go further than here
    return in_reses + [start_node]


def construct_subgraph(G, nodes):
    subg = GraphView()
    nodes = set(nodes)
    for node in nodes:
        for edge in G.out_edges(node.name):
            # only add internal edges
            if edge.to_node in nodes:
                subg.add_edge(NNEdge(from_node=edge.from_node, to_node=edge.to_node,
                                   from_idx=edge.from_idx, to_idx=edge.to_idx))

    def red_fn(state, edge):
        state.setdefault((edge.from_node, edge.from_idx), []
                         ).append((edge.to_node, edge.to_idx))
        return state

    inputs = reduce(red_fn, set([edge for node in subg.inputs()
                                 for edge in G.in_edges(node.name)]), {})
    inputs_map = []
    for (fnode, fidx), outs in inputs.items():
        inp = FusionInputParameters(
            f'{fnode.name}_{fidx}_in', dims=fnode.out_dims[fidx])
        inputs_map.append((fnode, fidx))
        for (tnode, tidx) in outs:
            subg.add_edge(NNEdge(from_node=inp, to_node=tnode, to_idx=tidx))
    outputs = [(node, set(edge.from_idx for edge in G.out_edges(node.name)))
               for node in subg.outputs()]
    outputs_map = []
    for (node, fidxes) in outputs:
        output_map = []
        outputs_map.append(output_map)
        for fidx in fidxes:
            output_map.append((edge.to_node, edge.to_idx)
                              for edge in G.out_edges(node.name) if edge.from_idx == fidx)
            outp = FusionOutputParameters(
                f'{node.name}_{fidx}_out', dims=node.out_dims[fidx])
            subg.add_edge(NNEdge(from_node=node, to_node=outp, from_idx=fidx))
    return (subg, inputs_map, outputs_map)


def find_connected_groups_of_class(G, must_classes, may_classes, min_len=2):
    classes = tuple(list(must_classes) + list(may_classes))
    candidates = set([node for node in G.nodes() if isinstance(node, classes)])
    start_nodes = set([node for node in candidates if not set(
        [edge.from_node for edge in G.in_edges(node.name)]) & candidates])
    groups = []
    match_classes = set(must_classes) | set(may_classes)
    while start_nodes:
        start_node = start_nodes.pop()
        connected_nodes = match_connected_nodes(
            G, start_node, lambda x: x.__class__ in match_classes)
        start_nodes -= set(connected_nodes)
        if len(connected_nodes) >= min_len and set(must_classes) & set([node.__class__ for node in connected_nodes]):
            groups.append(construct_subgraph(G, connected_nodes))
    return groups

def find_connected_groups_of_nodes(G, nodes):
    groups = []
    nodes = set(nodes)
    while nodes:
        start_node = next(iter(nodes))
        node_group = set(match_connected_nodes(
            G, start_node, lambda x: x in nodes))
        groups.append(node_group)
        nodes = nodes - node_group
    return groups


def find_in_out_nodes_in_group(G, nodes):
    in_edges = set()
    out_edges = set()
    input_nodes = set([(node, None)
                       for node in nodes if not G.in_edges(node.name)])
    output_nodes = set([(node, None)
                        for node in nodes if not G.out_edges(node.name)])
    in_edges.update(*[set(G.in_edges(node.name)) for node in nodes])
    out_edges.update(*[set(G.out_edges(node.name)) for node in nodes])
    input_edges = in_edges - out_edges
    input_nodes |= set((edge.to_node, edge.to_idx) for edge in input_edges)
    output_edges = out_edges - in_edges
    output_nodes |= set((edge.from_node, edge.from_idx)
                        for edge in output_edges)
    return input_nodes, output_nodes
