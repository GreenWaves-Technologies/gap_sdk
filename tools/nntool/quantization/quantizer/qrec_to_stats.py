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


def build_stat_from_qrec(qrec, node=None):
    if qrec is None:
        return None
    if qrec.in_qs is None or qrec.out_qs is None:
        return None
    range_in = [None if qtype is None else ({'min': qtype.min_val, 'max': qtype.max_val} if qtype.has_valid_range else {'min': None, 'max': None})
                for qtype in qrec.in_qs]
    range_out = [None if qtype is None else ({'min': qtype.min_val, 'max': qtype.max_val} if qtype and qtype.has_valid_range else {'min': None, 'max': None})
                 for qtype in qrec.out_qs]
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
            LOG.warning(f'{node.name} does not have valid activation range information')
    return {
        'range_in': range_in,
        'range_out': range_out
    }


def build_stat(G, nid, node=None):
    if not G.quantization:
        return None
    qrec = G.quantization.get(nid)
    return build_stat_from_qrec(qrec, node)


def set_stats(G, current_stats=None):
    stats = {} if current_stats is None else current_stats.copy()
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
        elif isinstance(node, ExpressionFusionParameters):
            if 'expression' not in stats[nid]:
                if 'expression' not in G.quantization[nid].cache:
                    raise ValueError(f"quantized expression for {node.name} not found in current stats")
                stats[nid]['expression'] = G.quantization[nid].cache['expression']
    return stats
