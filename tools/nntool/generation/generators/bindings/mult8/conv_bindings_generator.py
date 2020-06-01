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
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from generation.generators.globals.global_names import (BIASES, INFOS,
                                                        MULSCALE, MULSHIFT,
                                                        WEIGHTS)
from graph.types import Conv2DParameters, ConvFusionParameters, ActivationParameters
from utils.node_id import NodeId


@generation_function("bindings", (Conv2DParameters, ConvFusionParameters, ActivationParameters), qrec_types=(QREC_MULT8,))
def conv_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    step_idx = node.step_idx
    if isinstance(node, ActivationParameters):
        set_act_bindings(gen, step_idx, in_eparams, out_eparams, cname, node, qrec)
    elif isinstance(node, Conv2DParameters):
        set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname, node, qrec)
    elif isinstance(node, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type in ("conv_active_pool", "conv_active", "conv_pool"):
            set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                              cname, cnodes[0], quants[0], out_q=quants[1])
        elif node.fusion_type == "conv_pool_active":
            set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                              cname, cnodes[0], quants[0], out_q=quants[2])
        else:
            return False
    else:
        return False
    return True


def set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                      conv_params, conv_q, out_q=None):
    del step_idx
    if out_q is None:
        out_q = conv_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                           conv_q.in_qs[0], conv_q.weights_q, out_q.out_qs[0], conv_q.biases_q)
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgNode(conv_params, WEIGHTS),
                        GNodeArgNode(conv_params, BIASES),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(conv_params, MULSCALE),
                        GNodeArgNode(conv_params, MULSHIFT),
                        GNodeArgNode(conv_params, INFOS)
                        ))

def set_act_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                     act_params, act_qrec):
    del step_idx
    gen.bindings.append(
        CommentBindingList("Node {} inq {} outq {}", cname,
                           act_qrec.in_qs[0], act_qrec.out_qs[0])
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(act_params, INFOS)
                        ))
