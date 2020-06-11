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

from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import MatrixMulParameters, ActivationFusion
from utils.node_id import NodeId


@generation_function("bindings", (MatrixMulParameters, ActivationFusion), qrec_types=(QREC_MULT8,))
def mat_vect_mul_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    step_idx = node.step_idx
    if isinstance(node, ActivationFusion):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if isinstance(cnodes[0], MatrixMulParameters):
            set_mat_vect_mul_bindings(gen, cnodes[0], step_idx, in_eparams, out_eparams,
                                      cname, quants[0], out_q=quants[1])
            return True
        return False
    set_mat_vect_mul_bindings(gen, node, step_idx, in_eparams, out_eparams, cname, qrec)
    return True


def set_mat_vect_mul_bindings(gen, node, step_idx, in_eparams, out_eparams, cname, qrec, out_q=None):
    del step_idx
    if out_q is None:
        out_q = qrec
    gen.bindings.append(
        CommentBindingList("Node {} in1q {} in2q {} outq {}", cname,
                           qrec.in_qs[0], qrec.in_qs[1], out_q.out_qs[0])
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[1]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(node, 'infos')
                        ))
