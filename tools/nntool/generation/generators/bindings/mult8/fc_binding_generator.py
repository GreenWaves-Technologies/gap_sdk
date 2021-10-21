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
from generation.generator_decorators import QREC_MULT8, generation_function
from generation.generators.globals.global_names import (INFOS, MULSCALE,
                                                        MULSHIFT)
from graph.types import LinearFusionParameters, FcParameters
from utils.node_id import NodeId


@generation_function("bindings", (LinearFusionParameters, FcParameters), qrec_types=(QREC_MULT8, ))
def fc_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    step_idx = node.step_idx
    if isinstance(node, FcParameters):
        set_fc_bindings(gen, step_idx, in_eparams,
                        out_eparams, cname, node, qrec)
    elif isinstance(node, LinearFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        set_fc_bindings(gen, step_idx, in_eparams, out_eparams,
                        cname, cnodes[0], quants[0], out_q=quants[1])
    else:
        return False
    return True


def set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                    params, linear_q, out_q=None):
    del step_idx
    if out_q is None:
        out_q = linear_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                           linear_q.in_qs[0], linear_q.in_qs[1], out_q.out_qs[0])
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[1]),
                        GNodeArgEdge(in_eparams[2]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(params, MULSCALE),
                        GNodeArgNode(params, MULSHIFT),
                        GNodeArgNode(params, INFOS)
                        ))
