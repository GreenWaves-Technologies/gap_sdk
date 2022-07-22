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

import numpy as np
from bfloat16 import bfloat16

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge, GNodeArgNode,
                                 NodeBindingList)
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import custom_activation_infos, decompose_fusions, gen_activation_attrs
from nntool.graph.types import MatMulOpFusionNode, MatMulOpNode, MatMulTransposedNode
from nntool.graph.types.activations import (ActivationNodeBase,
                                     ReluNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode

LOG = logging.getLogger(__name__)


@paramstype(MatMulOpFusionNode, MatMulOpNode)
@ktype("float")
class MatMulFloatGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, MatMulOpFusionNode) and isinstance(fnode, MatMulOpNode):
            cnodes, quants = decompose_fusions(
                gen.G, pnode, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (_, act_qrec) = cnodes, quants
            custom_activation_infos(
                gen, pnode, linear_node, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, MatMulOpFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (_, act_node), (_, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
        else:
            custom_infos = None

        if len(node.in_dims) == 3:
            gen.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} outq {} biasesq {}", cname,
                                   qrec.in_qs[0], qrec.in_qs[1], qrec.out_qs[0], qrec.in_qs[2])
            )
            bindings = [
                GNodeArgEdge(in_eparams[1]),
                GNodeArgEdge(in_eparams[0]),
                GNodeArgEdge(in_eparams[2]),
                GNodeArgEdge(out_eparams[0], direction="GNA_OUT")
            ]
        else:
            gen.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} outq {}", cname,
                                   qrec.in_qs[0], qrec.in_qs[1], qrec.out_qs[0])
            )
            bindings = [
                GNodeArgEdge(in_eparams[1]),
                GNodeArgEdge(in_eparams[0]),
                GNodeArgEdge(out_eparams[0], direction="GNA_OUT")
            ]
        if custom_infos:
            bindings.append(GNodeArgNode(node, CUSTOM_INFOS))
        gen.bindings.append(
            NodeBindingList(cname,
                            *bindings
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, MatMulOpNode):
            if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(
                MatMulFloatKernel(
                    node.name, cname, node, None, None, gen_ctrl=node.get_gen_ctrl())
            )
        elif isinstance(node, MatMulOpFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (matmul_node, act_node), (matmul_qrec, act_qrec) = cnodes, quants
            if matmul_qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(
                MatMulFloatKernel(
                    node.name, cname, matmul_node, act_node, act_qrec, gen_ctrl=node.get_gen_ctrl())
            )
        else:
            return False
        return True

def reduce2d(shape):
    while len(shape) > 2 and shape[0] == 1:
        shape = shape[1:]
    return shape


class MatMulFloatKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatMulAct_fp16("{cname}", {gen_ctrl}, {ColM1}, {LineM1}, {ColM2},
                   {LineM2}, {Width}, {Height}, {Scx}, {Scy}, {kop_matmul}, {kop_act});
"""

    def __init__(self, node_name, cname, params, act_params, act_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if isinstance(act_params, ReluNode) and act_params.upper_bound:
            gen_ctrl.ReluN = act_params.upper_bound

        in_dims = params.in_dims
        matop = "KOP_MATMUL" if len(in_dims) > 2 else "KOP_MATMUL_NOBIAS"
        if isinstance(params, MatMulTransposedNode):
            matop += "_TRANSPOSED"

        shape0 = reduce2d(in_dims[0].shape)
        shape1 = reduce2d(in_dims[1].shape)

        if len(shape0) > 2 or len(shape1) > 2:
            LOG.error(f'{params.name} FP16 matmul kernel does not support batches')

        attrs = {
            'ColM1': shape0[1],
            'LineM1': shape0[0],
            'ColM2': shape1[0] if isinstance(params, MatMulTransposedNode) else shape1[1],
            'LineM2': shape1[1] if isinstance(params, MatMulTransposedNode) else shape1[0],
            'Width': 0,
            'Height': 0,
            'Scx': 1,
            'Scy': 1,
            'kop_matmul': matop,
        }

        attrs.update(gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl))

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
