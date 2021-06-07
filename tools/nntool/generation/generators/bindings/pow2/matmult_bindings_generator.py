# Copyright (C) 2020  GreenWaves Technologies, SAS

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


from utils.node_id import NodeId
from graph.types import MatMulOpParameters, MatMulOpFusionParameters
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from generation.generator_decorators import QREC_POW2, generation_function


@generation_function("bindings", (MatMulOpParameters, MatMulOpFusionParameters), qrec_types=(QREC_POW2, ))
def matmult_pow2_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    if isinstance(node, MatMulOpFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        set_matmul_bindings(gen, in_eparams, out_eparams, cname, cnodes[0], quants[0], out_q=quants[1].out_qs[0])
    else:
        set_matmul_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
    return True


def set_matmul_bindings(gen, in_eparams, out_eparams, cname, params, node_q, out_q=None):
    if out_q is None:
        out_q = node_q.out_qs[0]
    gen.bindings.append(
        CommentBindingList(f"Node {params.name} inq1 {node_q.in_qs[0].q} inq2 {node_q.in_qs[1].q} outq {out_q.q}")
    )
    if len(in_eparams) > 2:
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[1]), GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[2]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))
    else:
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[1]), GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))
