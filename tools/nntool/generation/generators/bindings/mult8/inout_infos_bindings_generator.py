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

from generation.bindings import (CommentBindingList, GNodeArgEdge, GNodeArgNode,
                                 NodeBindingList)
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import (ActivationParameters, ConvFusionParameters,
                         GlobalPoolParameters, PoolingParameters, ActivationFusion)


@generation_function("bindings", (PoolingParameters, ConvFusionParameters, ActivationParameters,
                                  GlobalPoolParameters, ActivationFusion), qrec_types=(QREC_MULT8, ))
def in_out_infos_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    if isinstance(node, ActivationFusion):
        cnodes = node.contained_nodes()
        if isinstance(cnodes[0], (GlobalPoolParameters, PoolingParameters)):
            set_in_out_bindings(gen, in_eparams, out_eparams, cname, cnodes[0], qrec)
            return True
        return False
    if isinstance(node, (GlobalPoolParameters, PoolingParameters)):
        set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
    else:
        return False
    return True


def set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, node_q, out_q=None):
    if out_q is None:
        out_q = node_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} outq {}", node.name,
                           str(node_q.in_qs[0]), str(out_q.out_qs[0]))
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"), GNodeArgNode(node, "infos")))
