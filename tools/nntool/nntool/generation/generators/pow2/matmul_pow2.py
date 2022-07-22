# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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
from nntool.generation.gen_utils import at_bits
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import (custom_activation_infos,
                                                       decompose_fusions,
                                                       gen_activation_attrs)
from nntool.graph.types import (ActivationNodeBase, MatMulOpFusionNode,
                         MatMulOpNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import ActivationFusionNode
from nntool.graph.types.tensor_arithmetic import MatMulTransposedNode


LOG = logging.getLogger(__name__)

def check_shape(shape):
    if len(shape) < 2:
        return False
    if len(shape) > 2:
        return np.prod(shape[0:-2]) == 1
    return True

@paramstype(MatMulOpNode, MatMulOpFusionNode, ActivationFusionNode)
@ktype('symmetric')
class MatMulPOW2Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(node, (MatMulOpFusionNode, ActivationFusionNode)) and fnode is None:
            cnodes, quants = decompose_fusions(
                gen.G, pnode, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (_, act_node), (_, act_qrec) = cnodes, quants
            custom_activation_infos(
                gen, pnode, fnode, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (MatMulOpFusionNode, ActivationFusionNode)):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (matmul_node, act_node), (matmul_qrec, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            set_matmul_bindings(gen, in_eparams, out_eparams, cname,
                                matmul_node, matmul_qrec, out_qrec=act_qrec,
                                has_custom=custom_infos)
        else:
            set_matmul_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (MatMulOpFusionNode, ActivationFusionNode)):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (matmul_node, act_node), (mul_qrec, act_qrec) = cnodes, quants
        else:
            matmul_node = node
            act_node = None
            mul_qrec = qrec
            act_qrec = None

        if not (check_shape(node.in_dims[0].shape) and check_shape(node.in_dims[1].shape)):
            LOG.warning(f'matmul {node.name} has batch dimension that is not supported by POW2 kernels')
            return False
        
        gen.kernels.append(MatMulActKernel(
            cname, node, matmul_node, mul_qrec, act_node, act_qrec, gen_ctrl=node.get_gen_ctrl()))
        return True


class MatMulActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_MatMul("{cname}", {gen_ctrl}, {at_bits(in1_qtype)}, {at_bits(in2_qtype)},
           {bias_bits}, {at_bits(out_qtype)}, {in1_qtype.q}, {in2_qtype.q}, {bias_q}, {out_qtype.q},
           1, 1, 1, 1,
           {width_1}, {height_1}, {width_2}, {height_2}, 0, 0, 1, 1,
           {relu_lower}, {relu_upper}, {kop_mult}, {kop_act});
'''

    def __init__(self, cname, params, matmul_params, matmul_qrec, act_params, act_qrec, gen_ctrl=None, out_qtype=None, force_relu=False):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in1_shape = params.in_dims[0].shape
        height_1 = in1_shape[-2]
        width_1 = in1_shape[-1]
        in2_shape = params.in_dims[1].shape
        height_2 = in2_shape[-2]
        width_2 = in2_shape[-1]
        out_shape = params.out_dims[0].shape

        in1_qtype = matmul_qrec.in_qs[0]
        in2_qtype = matmul_qrec.in_qs[1]
        if len(matmul_params.in_dims) == 3:
            bias_bits = at_bits(matmul_qrec.in_qs[2])
            bias_q = matmul_qrec.in_qs[2].q
            matmul_op = 'KOP_MATMUL'
        else:
            bias_q = 0
            bias_bits = 0
            matmul_op = 'KOP_MATMUL_NOBIAS'

        if isinstance(matmul_params, MatMulTransposedNode):
            matmul_op += '_TRANSPOSED'
            height_2 = in2_shape[1]
            width_2 = in2_shape[0]

        out_qtype = matmul_qrec.out_qs[0]
        relu_upper = relu_lower = 0
        attrs = {}

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl,
            force_relu=force_relu)

        if act_params is not None:
            # Expression is expecting output Q of matmul
            if not isinstance(act_params, ExpressionFusionNode):
                out_qtype = act_qrec.out_qs[0]
            relu_lower = 0
            if act_params.activation == "relu6" and out_qtype.q != 0:
                relu_upper = 6 << out_qtype.q
            else:
                relu_upper = 0


        # attributes used to test equality - i.e. this kernel can be reused
        attrs.update({
            'in1_qtype': in1_qtype,
            'in2_qtype': in2_qtype,
            'bias_q': bias_q,
            'bias_bits': bias_bits,
            'out_qtype': out_qtype,
            'height_1': height_1,
            'width_1': width_1,
            'height_2': height_2,
            'width_2': width_2,
            'out_shape': out_shape,
            'relu_lower': relu_lower,
            'relu_upper': relu_upper,
            'kop_mult': matmul_op,
        })

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


def set_matmul_bindings(gen, in_eparams, out_eparams, cname, params, node_qrec, out_qrec=None, has_custom=False):
    if out_qrec is None:
        out_q = node_qrec.out_qs[0]
    else:
        out_q = out_qrec.out_qs[0]

    gen.bindings.append(
        CommentBindingList(
            f"Node {params.name} inq1 {node_qrec.in_qs[0].q} inq2 {node_qrec.in_qs[1].q} outq {out_q.q}")
    )
    if len(in_eparams) > 2:
        bindings = [
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")
        ]
    else:
        bindings = [
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")
        ]
    if has_custom:
        bindings.append(GNodeArgNode(params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *bindings))
