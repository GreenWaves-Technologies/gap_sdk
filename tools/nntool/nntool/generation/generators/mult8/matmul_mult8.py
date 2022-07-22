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
from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from nntool.generation.gen_utils import ModelGenerationInternalError
from nntool.generation.generators.helpers.global_names import (CUSTOM_INFOS, INFOS,
                                                        MULSCALE, MULSHIFT)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.generation.helpers.gen_scales import gen_scales
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.act_infos import gen_act_infos
from nntool.generation.generators.helpers.fusions import (SQ8ActInfos,
                                                       custom_activation_infos,
                                                       decompose_fusions,
                                                       gen_activation_attrs)
from nntool.graph.types import (ActivationNodeBase, MatMulOpFusionNode,
                         MatMulOpNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.tensor_arithmetic import MatMulTransposedNode
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.qtype import QType

LOG = logging.getLogger(__name__)

@paramstype(MatMulOpNode, MatMulOpFusionNode)
@ktype('scaled')
class MatMulGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        # Only handle outer fusion
        if pnode and fnode:
            return True
        act_node, act_qrec = None, None
        if isinstance(pnode, MatMulOpNode):
            mul_node = pnode
            mul_qrec = qrec
            infos, comment = {}, "no activation"
        elif isinstance(pnode, MatMulOpFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, pnode, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (mul_node, act_node), (mul_qrec, act_qrec) = cnodes, quants
            infos, comment = gen_act_infos(act_node, act_qrec)
        else:
            return False

        if len(mul_qrec.in_qs[1].scale) > 1:
            gen_scales(gen, pnode, mul_node, mul_qrec)
            infos['OUTSCALE'] = np.uint8(0)
            infos['OUTSCALEN'] = np.uint8(0)
        else:
            infos['OUTSCALE'] = mul_qrec.cache['mul_biases_q'].qbiases[0]
            infos['OUTSCALEN'] = mul_qrec.cache['mul_biases_q'].qnorms[0]

        conv_mul_bias = mul_qrec.cache.get('mul_biases_q')
        infos['PRENORM'] = np.uint8(conv_mul_bias.pre_normalization if isinstance(
            conv_mul_bias, MultMulBiasScaleQType) else 0)

        if mul_qrec.cache.get('ne16'):
            infos['NE16_PADVAL'] = np.atleast_1d(
                mul_qrec.in_qs[0].zero_point).astype(mul_qrec.in_qs[0].dtype)
            infos['NE16_WOFFSET'] = - \
                np.array(mul_qrec.in_qs[1].zero_point).astype(np.int32)
            infos_len = 'NE16_DIM'
        else:
            infos_len = 'DIM'

        infos_encoder = SQ8ActInfos()
        contents, new_comment = infos_encoder.gen_infos_array(
            infos_len, **infos)
        comment += new_comment

        cname, file_name = gen_constant(gen, pnode, mul_node, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=comment))
        custom_activation_infos(gen, pnode, fnode, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, MatMulOpFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (mul_node, act_node), (mul_qrec, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            set_matmul_bindings(gen, in_eparams, out_eparams,
                                cname, mul_node, mul_qrec, out_q=quants[-1],
                                has_custom=custom_infos)
            return True
        if isinstance(node, MatMulOpNode):
            set_matmul_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        if isinstance(node, MatMulOpFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, MatMulOpNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (mul_node, act_node), (mul_qrec, act_qrec) = cnodes, quants
        else:
            mul_qrec = qrec
            mul_node = node
            act_node = None
            act_qrec = None

        matmul_ker_gen = MatMulKernelNE16 if mul_qrec.cache.get("ne16") else MatMulKernel
        gen.kernels.append(
            matmul_ker_gen(
                node.name, cname, mul_node, mul_qrec, act_node, act_qrec, force_relu=gen.force_relu))
        return True


def set_matmul_bindings(gen, in_eparams, out_eparams, cname, node, node_qrec, out_q=None, has_custom=False):
    if out_q is None:
        out_q = node_qrec
    gen.bindings.append(
        CommentBindingList(
            "Node {} inq {} outq {}", node.name,
            str(node_qrec.in_qs[0]), str(out_q.out_qs[0]))
    )
    idx_0 = 0 if node_qrec.cache.get("ne16") else 1
    idx_1 = 1 if node_qrec.cache.get("ne16") else 0
    if len(node.in_dims) == 3:
        if len(node_qrec.in_qs[1].scale) > 1:
            bindings = [
                GNodeArgEdge(in_eparams[idx_0]),
                GNodeArgEdge(in_eparams[idx_1]),
                GNodeArgEdge(in_eparams[2]),
                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                GNodeArgNode(node, MULSCALE),
                GNodeArgNode(node, MULSHIFT),
                GNodeArgNode(node, INFOS)
            ]
        else:
            bindings = [
                GNodeArgEdge(in_eparams[idx_0]),
                GNodeArgEdge(in_eparams[idx_1]),
                GNodeArgEdge(in_eparams[2]),
                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                GNodeArgNode(node, INFOS)
            ]
    else:
        if len(node_qrec.in_qs[1].scale) > 1:
            bindings = [
                GNodeArgEdge(in_eparams[idx_0]),
                GNodeArgEdge(in_eparams[idx_1]),
                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                GNodeArgNode(node, MULSCALE),
                GNodeArgNode(node, MULSHIFT),
                GNodeArgNode(node, INFOS)]
        else:
            bindings = [
                GNodeArgEdge(in_eparams[idx_0]),
                GNodeArgEdge(in_eparams[idx_1]),
                GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                GNodeArgNode(node, INFOS)]
    if has_custom:
        bindings.append(GNodeArgNode(node, CUSTOM_INFOS))

    gen.bindings.append(
        NodeBindingList(
            cname,
            *bindings
        )
    )


def calculate_dimensions(node_name, matmul_params):
    in1_shape = tuple(matmul_params.in_dims[0].shape)
    in2_shape = tuple(matmul_params.in_dims[1].shape)

    rank1 = len(in1_shape)
    rank2 = len(in2_shape)

    if rank2 <= 2:
        if rank2 == 1:
            # TODO - is this correct if transposed?
            in2_shape = (in2_shape[0], 1)
        channels = 1
        if rank1 > 2 and not all(dim == 1 for dim in in1_shape[:-2]):
            in1_shape = (int(np.prod(in1_shape[:-1])), in1_shape[-1])
        elif rank1 == 1:
            in1_shape = (1, in1_shape[0])
    elif rank1 == rank2 and in1_shape[:-2] == in2_shape[:-2]:
        channels = np.prod(in1_shape[:-2])
        LOG.warning(
            f'Matmul over batches is not yet properly generated - output will not be correct')
    else:
        raise ModelGenerationInternalError(
            f'{node_name} Invalid dimensions for matmul kernel {in1_shape} {in2_shape}')

    height_1 = in1_shape[-2]
    width_1 = in1_shape[-1]
    height_2 = in2_shape[-2]
    width_2 = in2_shape[-1]
    return height_1, width_1, height_2, width_2, channels


class MatMulKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_BatchedMatMulAct_SQ8("{cname}", {gen_ctrl}, {bias_datasize}, 1, 
                         {batch_size}, {width_1}, {height_1}, {width_2}, {height_2},
                         0, 0, 1, 1, {kop_matmul}, {kop_act});
'''

    def __init__(self, node_name, cname, matmul_params, matmul_qrec, act_params, act_qrec, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl,
            force_relu=force_relu)

        height_1, width_1, height_2, width_2, batch_size = calculate_dimensions(
            node_name, matmul_params)

        if len(matmul_params.in_dims) == 3:
            bias_datasize = at_bits(matmul_qrec.in_qs[2])
            matmul_op = 'KOP_MATMUL'
        else:
            bias_datasize = 0
            matmul_op = 'KOP_MATMUL_NOBIAS'

        if len(matmul_qrec.in_qs[1].scale) == 1:
            matmul_op += '_SCALE_SCALAR'

        if isinstance(matmul_params, MatMulTransposedNode):
            matmul_op += '_TRANSPOSED'
            height_2, width_2 = width_2, height_2

        # attributes affecting generation
        attrs.update({
            'height_1': height_1,
            'width_1': width_1,
            'height_2': height_2,
            'width_2': width_2,
            'batch_size': batch_size,
            'bias_datasize': bias_datasize,
            'kop_matmul': matmul_op,
        })

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


class MatMulKernelNE16(NewAutoTilerKernel):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_BatchedMatMulAct_NE16("{cname}", {gen_ctrl}, {in1_datasize}, {out_datasize}, {bias_datasize}, {in2_datasize_bits}, {pad_val}, 
                          {batch_size}, {width_1}, {height_1}, {width_2}, {height_2},
                          {kop_matmul}, {kop_act});
'''

    def __init__(self, node_name, cname, matmul_params, matmul_qrec, act_params, act_qrec, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl,
            force_relu=force_relu)

        height_1, width_1, width_2, height_2, batch_size = calculate_dimensions(
            node_name, matmul_params)
        bias_datasize = at_bits(matmul_qrec.in_qs[2])
        in1_datasize = at_bits(matmul_qrec.in_qs[0])
        in2_datasize_bits = matmul_qrec.in_qs[1].bits
        out_datasize = at_bits(matmul_qrec.out_qs[0]) if not act_qrec else at_bits(act_qrec.out_qs[0])
        pad_val = matmul_qrec.in_qs[0].zero_point[0]
        matmul_op = 'KOP_MATMUL_TRANSPOSED'

        # attributes affecting generation
        attrs.update({
            'height_1': height_1,
            'width_1': width_1,
            'height_2': height_2,
            'width_2': width_2,
            'pad_val': pad_val,
            'batch_size': batch_size,
            'bias_datasize': bias_datasize,
            'in1_datasize': in1_datasize,
            'in2_datasize_bits': in2_datasize_bits,
            'out_datasize': out_datasize,
            'kop_matmul': matmul_op,
        })

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
