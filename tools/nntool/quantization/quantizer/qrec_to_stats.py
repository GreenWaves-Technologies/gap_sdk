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

import logging
from copy import deepcopy

import numpy as np
from graph.types import (ConstantInputParameters, ExpressionFusionParameters,
                         FusionBase, FusionInputParameters,
                         FusionOutputParameters, QuantizeParameters)
from quantization.new_qrec import QRec
from utils.node_id import NodeId

LOG = logging.getLogger('nntool.' + __name__)


def ranges_are_valid(ranges):
    return not any(rng['min'] is None or rng['max'] is None for rng in ranges if rng is not None)


def build_stat_from_qtypes(in_qs, out_qs, node=None):
    range_in = [None if qtype is None else ({'min': qtype.min_val, 'max': qtype.max_val}
                                            if qtype.has_valid_range else {'min': None, 'max': None})
                for qtype in in_qs]
    range_out = [None if qtype is None else ({'min': qtype.min_val, 'max': qtype.max_val}
                                             if qtype and qtype.has_valid_range else {'min': None, 'max': None})
                 for qtype in out_qs]
    range_in_valid = ranges_are_valid(range_in)
    range_out_valid = ranges_are_valid(range_out)
    if not range_in_valid or not range_out_valid:
        # maybe this is a quantize node with a float type one side of it
        if isinstance(node, QuantizeParameters) and (range_in_valid or range_out_valid):
            if range_in_valid:
                range_out = deepcopy(range_in)
            else:
                range_in = deepcopy(range_out)
        elif isinstance(node, ConstantInputParameters):
            range_out = [{'min': np.atleast_1d(
                node.value.min()), 'max': np.atleast_1d(node.value.max())}]
        else:
            LOG.warning(
                f'{node.name} does not have valid activation range information')
    return {
        'range_in': range_in,
        'range_out': range_out
    }


def build_stat_from_qrec(qrec, node=None):
    if qrec is None:
        return None
    if qrec.in_qs is None or qrec.out_qs is None:
        return None
    return build_stat_from_qtypes(qrec.in_qs, qrec.out_qs, node=node)


def build_fusion_stats(stats: dict, fusion: FusionBase):
    inputs = fusion.subgraph.inputs()
    in_stats = [None] * len(inputs)
    for sub_node in inputs:
        edge = fusion.subgraph.out_edges(sub_node)[0]
        stat = stats.get(NodeId(edge.to_node))
        if stat is None:
            in_stats = None
            break
        range_in = stat['range_in']
        if len(range_in) <= edge.to_idx:
            in_stats = None
            break
        in_stats[sub_node.idx] = range_in[edge.to_idx]
    outputs = fusion.subgraph.outputs()
    out_stats = [None] * len(outputs)
    for sub_node in outputs:
        edge = fusion.subgraph.in_edges(sub_node)[0]
        stat = stats.get(NodeId(edge.from_node))
        if stat is None:
            out_stats = None
            break
        range_out = stat['range_out']
        if len(range_out) <= edge.from_idx:
            out_stats = None
            break
        out_stats[sub_node.idx] = range_out[edge.from_idx]
    return {
        'range_in': in_stats,
        'range_out': out_stats
    }


def build_stat(G, nid, node=None):
    if not G.quantization:
        return None
    qrec = G.quantization.get(nid)
    return build_stat_from_qrec(qrec, node)


def set_stats(G, current_stats=None, current_options=None):
    stats = {} if current_stats is None else current_stats.copy()
    current_options = {} if current_options is None else current_options.copy()
    for node in G.nodes():
        nid = NodeId(node)
        if nid not in stats or stats[nid] is None:
            stats[nid] = build_stat(G, nid, node=node)
        if isinstance(node, FusionBase) and node.quantize_internals:
            for fnode in node.subgraph.nodes():
                nid = NodeId(node, fnode)
                if nid not in stats:
                    # if fusion input or output recs are not present build them
                    if isinstance(fnode, (FusionInputParameters, FusionOutputParameters)):
                        continue
                    else:
                        qrec = G.quantization.get(
                            nid) if G.quantization else None
                    stats[nid] = build_stat_from_qrec(qrec)
            nid = NodeId(node)
            if G.quantization and nid not in G.quantization:
                stats[nid] = build_fusion_stats(stats, node)
        elif isinstance(node, ExpressionFusionParameters):
            if stats[nid] is None or 'expression' not in stats[nid]:
                if (G.quantization is None or nid not in G.quantization or G.quantization[nid].cache is None or
                        'expression' not in G.quantization[nid].cache):
                    raise ValueError(
                        f"quantized expression for {node.name} not found in current stats")
                stats[nid]['expression'] = G.quantization[nid].cache['expression']
        elif isinstance(node, ConstantInputParameters):
            if G.quantization and nid in G.quantization:
                current_options.setdefault(
                    nid, {})['qtype_ind'] = G.quantization[nid].out_qs[0]

    return stats, current_options
