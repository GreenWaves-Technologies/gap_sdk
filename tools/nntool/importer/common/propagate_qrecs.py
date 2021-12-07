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

from functools import reduce
from itertools import zip_longest

import numpy as np
from graph.types import (MaxPoolParameters, OutputParameters, FcParameters, Conv2DParameters,
                         ReluActivationParameters, ReshapeParameters,
                         SoftMaxParameters, TransposeParameters)
from quantization.new_qrec import QRec
from quantization.qtype import QType
from utils.node_id import NodeId

# Handlers for different node types


def overwrite_range(in_ranges, out_ranges, in_dtypes=None, out_dtypes=None):
    if in_dtypes is None:
        in_dtypes = [np.int8] * len(in_ranges)
    if out_dtypes is None:
        out_dtypes = [np.int8] * len(out_ranges)
    proto_in_qs = [QType.from_min_max_sq(
        *min_max, dtype=dtype) for min_max, dtype in zip(in_ranges, in_dtypes)]
    proto_out_qs = [QType.from_min_max_sq(
        *min_max, dtype=dtype) for min_max, dtype in zip(out_ranges, out_dtypes)]

    def handler(G, qrec, node, in_qs=None, out_qs=None):
        nonlocal proto_in_qs, proto_out_qs
        if qrec is None:
            return QRec.scaled(in_qs=list(proto_in_qs), out_qs=list(proto_out_qs))
        new_in_qs = [q1 if q1 else q2 for q1, q2 in zip_longest(
            qrec.in_qs, proto_in_qs)] if qrec.in_qs else in_qs
        new_out_qs = [q1 if q1 else q2 for q1, q2 in zip_longest(
            qrec.out_qs, proto_out_qs)] if qrec.out_qs else out_qs
        return QRec.scaled(in_qs=new_in_qs, out_qs=new_out_qs)
    return handler


def pass_qtype(G, qrec, node, in_qs=None, out_qs=None):
    copy_qs = in_qs if in_qs is not None else out_qs
    if not copy_qs:
        return qrec
    if qrec is None:
        if copy_qs:
            return QRec.scaled(in_qs=copy_qs, out_qs=copy_qs)
        return None
    in_qs = copy_qs if not qrec.in_qs or qrec.in_qs[0] is None else qrec.in_qs
    out_qs = copy_qs if not qrec.out_qs or qrec.out_qs[0] is None else qrec.out_qs
    return QRec.scaled(in_qs=in_qs, out_qs=out_qs)


def pass_qtype_up(G, qrec, node, in_qs=None, out_qs=None):
    if out_qs is None or not out_qs:
        return qrec
    if qrec is None or not qrec.in_qs or qrec.in_qs[0] is None:
        return QRec.scaled(in_qs=out_qs, out_qs=out_qs)
    return qrec


def merge_qtypes(x, y):
    if x is None:
        return y
    if y is None:
        return x
    return [x_elem if x_elem else y_elem for x_elem, y_elem in zip_longest(x, y, fillvalue=None)]


def merge_qtypes_to_qrec(G, qrec, node, in_qs=None, out_qs=None):
    in_qs = merge_qtypes(qrec.in_qs if qrec else None, in_qs)
    out_qs = merge_qtypes(qrec.out_qs if qrec else None, out_qs)
    if in_qs is None and out_qs is None:
        return None
    return QRec.scaled(in_qs=in_qs, out_qs=out_qs)


def pass_filter(G, qrec, node, in_qs=None, out_qs=None):
    if in_qs and len(in_qs) == 2 and G.num_in_edges(node) == 3:
        in_qs.append(None)
    return merge_qtypes_to_qrec(G, qrec, node, in_qs=in_qs, out_qs=out_qs)


INFERRED_NODES = {
    SoftMaxParameters: overwrite_range(((-16.0, 16.0),), ((0.0, 1.0),)),
    OutputParameters: pass_qtype,
    ReluActivationParameters: pass_qtype_up,
    MaxPoolParameters: pass_qtype,
    TransposeParameters: pass_qtype,
    ReshapeParameters: pass_qtype,
    Conv2DParameters: pass_filter,
    FcParameters: pass_filter,
    # These need more work since they have multi in out and/or can change quantized channel position
    # SplitParameters: pass_qtype,
    # ConcatParameters: pass_qtype,
    # StridedSliceParameters: pass_qtype,
    # GatherParameters: pass_qtype,
}


def extend_insert(l, idx, val, add_if_exists=True):
    l += [None] * (idx + 1 - len(l))
    if l[idx] is None or add_if_exists:
        l[idx] = val
    return l


def get_out_qtypes(qrecs, qrec, out_edges):
    def reduction(qtypes, edge):
        nid = NodeId(edge.to_node)
        qrec = qrecs.get(nid)
        if not qrec or len(qrec.in_qs) <= edge.to_idx or qrec.in_qs[edge.to_idx] is None:
            return qtypes
        qtypes = extend_insert(qtypes, edge.from_idx, qrec.in_qs[edge.to_idx])
        return qtypes
    return reduce(reduction, out_edges, qrec.out_qs.copy() if qrec is not None else [])


def get_in_qtypes(qrecs, qrec, in_edges):
    def reduction(qtypes, edge):
        nid = NodeId(edge.from_node)
        qrec = qrecs.get(nid)
        if not qrec or len(qrec.out_qs) <= edge.from_idx or qrec.out_qs[edge.from_idx] is None:
            return qtypes
        qtypes = extend_insert(qtypes, edge.to_idx, qrec.out_qs[edge.from_idx])
        return qtypes
    return reduce(reduction, in_edges, qrec.in_qs.copy() if qrec is not None else [None] * len(in_edges))


def get_qrec_pass(G, qrec, node, copy_qs):
    if qrec is None:
        if copy_qs:
            return QRec.scaled(in_qs=copy_qs, out_qs=copy_qs)
        return None
    in_qs = copy_qs if not qrec.in_qs or qrec.in_qs is None else qrec.in_qs
    out_qs = copy_qs if not qrec.out_qs or qrec.out_qs is None else qrec.out_qs
    return QRec.scaled(in_qs=in_qs, out_qs=out_qs)


def propagate_qrecs_up(G, node, qrecs, visited_edges):
    out_edges = G.out_edges(node)
    if out_edges and not all(edge in visited_edges for edge in out_edges):
        return
    in_edges = G.in_edges(node)
    if not in_edges:
        return
    nid = NodeId(node)
    qrec = qrecs.get(nid)
    out_qtypes = get_out_qtypes(qrecs, qrec, out_edges)
    if node.__class__ in INFERRED_NODES:
        qrec = INFERRED_NODES[node.__class__](G, qrec, node, out_qs=out_qtypes)
        if qrec is not None:
            qrecs[nid] = qrec
    elif out_qtypes:
        qrecs[nid] = merge_qtypes_to_qrec(G, qrec, node, out_qs=out_qtypes)
    for edge in in_edges:
        visited_edges.add(edge)
        propagate_qrecs_up(G, edge.from_node, qrecs, visited_edges)


def propagate_qrecs_down(G, node, qrecs, visited_edges):
    in_edges = G.in_edges(node)
    if in_edges and not all(edge in visited_edges for edge in in_edges):
        return
    out_edges = G.out_edges(node)
    nid = NodeId(node)
    qrec = qrecs.get(nid)
    in_qtypes = get_in_qtypes(qrecs, qrec, in_edges)
    if node.__class__ in INFERRED_NODES:
        qrec = INFERRED_NODES[node.__class__](G, qrec, node, in_qs=in_qtypes)
        if qrec is not None:
            qrecs[nid] = qrec
    elif in_qtypes:
        qrecs[nid] = merge_qtypes_to_qrec(G, qrec, node, in_qs=in_qtypes)

    for edge in out_edges:
        visited_edges.add(edge)
        propagate_qrecs_down(G, edge.to_node, qrecs, visited_edges)


def propagate_qrecs(G, qrecs):
    visited_edges = set()
    for node in G.outputs():
        propagate_qrecs_up(G, node, qrecs, visited_edges)
    visited_edges = set()
    for node in G.inputs():
        propagate_qrecs_down(G, node, qrecs, visited_edges)
