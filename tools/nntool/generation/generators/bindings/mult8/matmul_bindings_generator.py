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
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generator_decorators import QREC_MULT8, generation_function
from generation.generators.globals.global_names import (INFOS, MULSCALE,
                                                        MULSHIFT)
from graph.types import MatMulOpParameters
from graph.types.fusions import MatMulOpFusionParameters


@generation_function("bindings", (MatMulOpFusionParameters, MatMulOpParameters), qrec_types=(QREC_MULT8, ))
def matmul_mult_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    if isinstance(node, MatMulOpFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(
            node, fnode)] for fnode in cnodes]
        set_in_out_bindings(gen, in_eparams, out_eparams,
                            cname, cnodes[0], quants[0], out_q=quants[-1])
        return True
    if isinstance(node, MatMulOpParameters):
        set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
    else:
        return False
    return True


def set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, node_q, out_q=None):
    if out_q is None:
        out_q = node_q
    gen.bindings.append(
        CommentBindingList(
            "Node {} inq {} outq {}", node.name,
            str(node_q.in_qs[0]), str(out_q.out_qs[0]))
    )
    if len(node.in_dims) == 3:
        if len(node_q.in_qs[0].scale) > 1:
            gen.bindings.append(
                NodeBindingList(
                    cname,
                    GNodeArgEdge(in_eparams[1]),
                    GNodeArgEdge(in_eparams[0]),
                    GNodeArgEdge(in_eparams[2]),
                    GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                    GNodeArgNode(node, MULSCALE),
                    GNodeArgNode(node, MULSHIFT),
                    GNodeArgNode(node, INFOS)))
        else:
            gen.bindings.append(
                NodeBindingList(
                    cname,
                    GNodeArgEdge(in_eparams[1]),
                    GNodeArgEdge(in_eparams[0]),
                    GNodeArgEdge(in_eparams[2]),
                    GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                    GNodeArgNode(node, INFOS)))
    else:
        gen.bindings.append(
            NodeBindingList(
                cname,
                GNodeArgEdge(in_eparams[1]),
                GNodeArgEdge(in_eparams[0]),
                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                GNodeArgNode(node, INFOS)))
