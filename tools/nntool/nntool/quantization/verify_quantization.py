# Copyright (C) 2020, 2021  GreenWaves Technologies, SAS

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




def walk_graph(G, qrecs, node, visited):
    problems = []
    if node in visited:
        return problems
    visited.add(node)
    qrec = qrecs.get(node.name)
    if qrec is None:
        problems.append(f"node {node.name} has no quantization set")
    elif qrec.out_qs is None:
        problems.append(f"node {node.name} has no output quantization set")
    elif qrec.in_qs is None:
        problems.append(f"node {node.name} has no input quantization set")
    else:
        for idx, edge_group in enumerate(G.indexed_out_edges(node.name)):
            if len(qrec.out_qs) <= idx:
                problems.append(
                    f"node {node.name} has no output quantization set on output {idx}")
                continue
            qtype = qrec.out_qs[idx]
            if qtype is None and edge_group:
                problems.append(
                    f"node {node.name} quantization on output {idx} is None")
                continue
            for edge in edge_group:
                to_qrec = qrecs.get(edge.to_node.name)
                if to_qrec is None or to_qrec.in_qs is None:
                    # error will be reported when node is visited
                    continue
                if len(to_qrec.in_qs) <= edge.to_idx:
                    problems.append(
                        f"node {edge.to_node.name} has no input quantization set on input {edge.to_idx}")
                if to_qrec.in_qs[edge.to_idx] is None:
                    problems.append(
                        f"node {edge.to_node.name} quantization set on input {edge.to_idx} is None")
                if not qtype.quantization_equal(to_qrec.in_qs[edge.to_idx]):
                    problems.append(f"node {edge.to_node.name} quantization set on input {edge.to_idx} "
                                    f"does not match node {node.name} output {idx} {qtype} -> {to_qrec.in_qs[edge.to_idx]}")
    for edge in G.out_edges(node.name):
        problems.extend(walk_graph(G, qrecs, edge.to_node, visited))
    return problems

def verify_quantization(G):
    if G.quantization is None:
        return ["quantization is not set"]
    qrecs = G.quantization
    visited = set()
    problems = []
    for node in G.inputs():
        problems.extend(walk_graph(G, qrecs, node, visited))
    return problems
