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

import logging

from generation.at_types.at_params import NO_ACTIVATION, gen_activation_op
from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from generation.new_generators.mult8.matadd_mult8 import make_three_dims
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from graph.types import MatrixAddParameters, PaddedAddFusionParameters
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

@paramstype(PaddedAddFusionParameters)
@ktype("float")
class PaddedMatAddFloatGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        add_node = [node for node in cnodes if isinstance(
            node, MatrixAddParameters)]
        if add_node:
            quants = [gen.G.quantization[NodeId(
                node, fnode)] for fnode in cnodes]

        gen.bindings.append(
            CommentBindingList("Node {} in1q {} in2q {} outq {}", cname,
                            quants[1].in_qs[0], quants[1].in_qs[1], quants[-1].out_qs[0])
        )
        gen.bindings.append(
            NodeBindingList(cname,
                            GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cnodes = node.contained_nodes()
        if len(cnodes) > 2:
            act_node = cnodes[2]
        else:
            act_node = None
        pad_node = cnodes[0]
        gen.kernels.append(PaddedMatAddKernelFloat(node.name, cname, node, pad_node, act_node, at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
        return True


class PaddedMatAddKernelFloat(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatAddPaddedAct_fp16("{cname}", {gen_ctrl}, {feat}, {width}, {height}, {padtop}, {padbot}, {padded_idx}, KOP_MATADD, {act_oper});
"""

    def __init__(self, node_name, cname, matrixadd_params, pad_params, act_params, at_ver=3, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if act_params is not None:
            at_act_params = gen_activation_op(act_params.activation, force_relu=force_relu)
        else:
            at_act_params = NO_ACTIVATION

        padtop = pad_params.padding[0][0]
        padbot = pad_params.padding[0][1]
        padded_idx = 0 if matrixadd_params.in_dims[0].size() > matrixadd_params.in_dims[1].size() else 1
        dimensions0 = make_three_dims(matrixadd_params.in_dims[0])
        dimensions1 = make_three_dims(matrixadd_params.in_dims[1])

        attrs = {
            'feat': max(dimensions0[0], dimensions1[0]),
            'width': dimensions0[1],
            'height': dimensions0[2],
            'padded_idx': padded_idx,
            'padtop': padtop,
            'padbot': padbot,
            'act_oper': at_act_params
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
