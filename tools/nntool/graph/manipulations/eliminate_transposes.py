# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging

from graph.types.others import ReshapeParameters
from graph.types.base import SensitiveToOrder, Transposable

LOG = logging.getLogger("nntool." + __name__)

def add_sequence(trans_seqs, trans_nodes):
    if trans_nodes and len(trans_nodes) > 1:
        trans_seq = trans_seqs.get(trans_nodes[-1])
        if not trans_seq:
            trans_seq = []
            trans_seqs[trans_nodes[-1]] = trans_seq
        trans_seq.append(trans_nodes)

def find_last_transpose(G, node, trans_seqs, trans_nodes=None):
    if isinstance(node, str):
        node = G.node(node)

    if isinstance(node, SensitiveToOrder):
        add_sequence(trans_seqs, trans_nodes)
        trans_nodes = None
    elif isinstance(node, Transposable):
        if trans_nodes is None:
            # new sequence
            trans_nodes = []
        trans_nodes.append(node)

    out_edges = G.out_edges(node.name)

    if len(out_edges) == 0:
        add_sequence(trans_seqs, trans_nodes)
        return

    # Edges are visited in a repeatable order
    out_edges.sort(key=lambda x: str(x.from_idx) + x.to_node.name + str(x.to_idx))

    for edge in out_edges:
        if trans_nodes:
            if len(out_edges) > 1:
                trans_nodes_copy = trans_nodes.copy()
            else:
                trans_nodes_copy = trans_nodes
            find_last_transpose(G, edge.to_node, trans_seqs, trans_nodes_copy)
        else:
            find_last_transpose(G, edge.to_node, trans_seqs)

def find_last_transposes(G):
    """Does a depth first search in the graph to discover transposable
    nodes with no SensitiveToOrder nodes between them"""
    LOG.info("finding transpose sequences")
    trans_seqs = {}
    for node in G.inputs():
        find_last_transpose(G, node, trans_seqs)
    return trans_seqs

def reverses_transpose(trans1, trans2):
    """Checks if one transpose reverses another"""
    if trans1 is None or trans2 is None:
        return False
    for idx, val in enumerate(trans1):
        if trans2[val] != idx:
            return False
    return True

def get_first_transposable(rseq, idx):
    """Looks back in the string of transposables for a vlid transposable. Reshapes that are
    not transposing are skipped but returned in an array"""
    reshapes = []
    while idx < len(rseq):
        node = rseq[idx]
        if isinstance(node, ReshapeParameters) and not node.has_transpose:
            reshapes.append(rseq[idx])
        elif isinstance(node, Transposable):
            return node, reshapes, idx
        idx += 1
    return None, reshapes, idx

def apply_reshape(trans, reshape):
    """Create a new transpose if there are 1 sized dimensions in the reshape"""
    if not reshape.does_nothing():
        return trans

    old_shape = reshape.old_shape.shape.copy()
    trans = trans.copy()
    while True:
        change = False
        idx = 0
        while idx < len(trans):
            dim_idx = trans[idx]
            if old_shape[dim_idx] == 1:
                change = True
                del old_shape[dim_idx]
                del trans[idx]
                for jdx, dim_jdx in enumerate(trans):
                    if dim_jdx > dim_idx:
                        trans[jdx] -= 1
                change = True
                break
            idx += 1
        if not change:
            break

    return trans

def apply_reshapes(trans, reshapes):
    for reshape in reversed(reshapes):
        trans = apply_reshape(trans, reshape)
    return trans

def process(seq, switchable):
    rseq = seq[::-1]
    idx = 0
    while idx < len(rseq) - 1:
        node = rseq[idx]
        pnode, reshapes, idx = get_first_transposable(rseq, idx + 1)
        sw_node = switchable.get(node)
        if reverses_transpose(node.transpose_in, apply_reshapes(pnode.transpose_out, reshapes)):
            if not sw_node:
                switchable[node] = {
                    'can_switch': True,
                    'segments': {pnode: reshapes}
                }
            elif sw_node['can_switch']:
                sw_node['segments'][pnode] = reshapes
        else:
            if not sw_node:
                # This node cannot be switched so all the nodes that could
                # switched cannot be
                switchable[node] = {'can_switch': False, 'segments': {}}
            elif sw_node['can_switch']:
                sw_node['can_switch'] = False
                sw_node['segments'].clear()

def process_sequences(trans_seqs):
    """Extracts nodes that are valid for transpose elimination"""
    LOG.info("processing transpose sequences")
    switchable = {}
    for seqs in trans_seqs.values():
        for seq in seqs:
            process(seq, switchable)
    return switchable

def update_switchable(switchable):
    """Updates the node transposes"""
    LOG.info("updating nodes")
    updated_reshapes = set()
    for node, switch in switchable.items():
        if not switch['can_switch']:
            continue
        for pnode, reshapes in switch['segments'].items():
            for reshape in reshapes:
                if reshape not in updated_reshapes:
                    updated_reshapes.add(reshape)
                    reshape.old_shape.transpose(pnode.transpose_out)
                    reshape.shape.transpose(node.transpose_in)
                    LOG.info("reshape %s modified", reshape.name)
            pnode.transpose_out = None
            LOG.info("transpose eliminated %s => %s", pnode.name, node.name)

        node.transpose_in = None

def eliminate_transposes(G):
    """Eliminates unnecessary transposes from the graph. Valid transposes are those that have no
    nodes that are sensitive to order between them and where one reverses the other"""
    LOG.info("eliminating unnecessary transposes")
    trans_seqs = find_last_transposes(G)
    if not trans_seqs:
        LOG.info("no transpose sequences found")
        return
    switchable = process_sequences(trans_seqs)
    update_switchable(switchable)
