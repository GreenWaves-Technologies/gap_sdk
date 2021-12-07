# Copyright (C) 2021  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from graph.types import (ConstantInputParameters, InputParameters,
                         OutputParameters)


class GraphIntegrityError(Exception):
    pass


def verify_inputs_outputs(G):
    """ All input and output nodes should be the correct class"""
    problems = []
    for node in G.inputs():
        if not isinstance(node, (InputParameters, ConstantInputParameters)):
            problems.append(
                f"{node.name} is a {node.__class__.__name__} but is a graph input")
    for node in G.outputs():
        if not isinstance(node, (OutputParameters)):
            problems.append(
                f"{node.name} is a {node.__class__.__name__} but is a graph output")
    return problems


def verify_edges(G, check_connected=True):
    """ Their must only be one input on each index and everything must be connected """
    problems = []
    for node_name in G:
        in_idxes = {}
        for edge in G.in_edges(node_name):
            if edge.to_idx in in_idxes:
                problems.append(
                    f"edge {in_idxes[edge.to_idx]} connects to the same input as edge {edge}")
        if check_connected and G.num_in_edges(node_name) == 0 and G.num_out_edges(node_name) == 0:
            problems.append(f"{node_name} isn't connected to any other")
    return problems


def verify_edge_connections(G):
    problems = []
    for edge in G.edges():
        if edge.from_node.name not in G:
            problems.append(f"edge {edge} source node is not in graph")
        if edge.to_node.name not in G:
            problems.append(f"edge {edge} destination node is not in graph")
    return problems


def visit(G, in_edge, visited_edges, visited_nodes):
    visited_edges.add(in_edge)
    node = in_edge.to_node
    if not all((edge in visited_edges) for edge in G.in_edges(node.name)):
        return []
    visited_nodes.add(node)
    problems = []
    for edge in G.out_edges(node.name):
        if edge.to_node in visited_nodes:
            problems.append(f'cycle in graph from node {node.name}:{edge.from_idx} '
                            f'to {edge.to_node.name}:{edge.to_idx}')
        else:
            problems.extend(visit(G, edge, visited_edges, visited_nodes))
    return problems


def verify_no_cycles(G):
    """ Their must not be any cycles in the graph """
    visited_edges = set()
    visited_nodes = set()
    problems = []
    for node in G.inputs():
        for edge in G.out_edges(node.name):
            problems.extend(visit(G, edge, visited_edges, visited_nodes))
    return problems


def visit_all_edges(G, node, visited_edges, all_nodes):
    if node not in all_nodes:
        return
    all_nodes.remove(node)
    for edge in G.in_edges(node.name):
        if edge not in visited_edges:
            visited_edges.add(edge)
            visit_all_edges(G, edge.from_node, visited_edges, all_nodes)
    for edge in G.out_edges(node.name):
        if edge not in visited_edges:
            visited_edges.add(edge)
            visit_all_edges(G, edge.to_node, visited_edges, all_nodes)


def verify_weakly_connected(G):
    """ DAG needs to be weakly connected. i.e. Not 2 graphs """
    start = G.inputs()[0]
    visited_edges = set()
    all_nodes = set(G.nodes())
    visit_all_edges(G, start, visited_edges, all_nodes)
    if all_nodes:
        return (['there is more than one graph'] +
                [f'{node.name} is not connected to {start.name}' for node in all_nodes])
    return []


def verify_node_verifiers(G):
    """ Node classes can define their own verifiers """
    problems = []
    for node in G.nodes():
        problems.extend(node.verify(G))
    return problems


VERIFIERS = [
    verify_inputs_outputs,
    verify_edges,
    verify_edge_connections,
    verify_no_cycles,
    verify_weakly_connected,
    verify_node_verifiers
]


def verify_graph(G, throw_exception=False):
    problems = []
    for verifier in VERIFIERS:
        print(f'running verifier {verifier.__name__}')
        edge_problems = verifier(G)
        problems.extend(edge_problems)
        if edge_problems:
            if throw_exception:
                raise GraphIntegrityError('\n'.join(problems))
    return problems


def graph_statistics(G):
    node_counts = {}
    edge_count = 0
    for node in G.nodes():
        node_counts[node.__class__] = node_counts.setdefault(
            node.__class__, 0) + 1
        edge_count += len(G.out_edges(node.name))
    for node in node_counts:
        if node.CLS_OP_NAME is None:
            raise ValueError(f'node class {node.__name__} has no CLS_OP_NAME')
    counts = [f"{node.CLS_OP_NAME}: {node_counts[node]}" for node in node_counts]
    counts += [f"{sum(node_counts.values())} nodes"]
    counts += [f"{edge_count} edges"]
    return counts
