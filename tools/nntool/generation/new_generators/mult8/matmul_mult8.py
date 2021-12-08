# Copyright (C) 2021  GreenWaves Technologies, SAS

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

from generation.new_generators.mult8.conv_pool_mult8 import verify_scalar
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from graph.types.tensor_arithmetic import MatMulTransposedParameters
from generation.bindings import CommentBindingList, GNodeArgEdge, GNodeArgNode, NodeBindingList
from generation.helpers.gen_scales import gen_scales
from generation.at_generators.utils import at_bits
from generation.new_generators.helpers.in_out_bindings_mixin import InOutBindingsMixin
from generation.new_generators.helpers.act_infos import gen_act_infos
from generation.at_types.at_params import NO_POOL, gen_activation_op
import logging
from utils.node_id import NodeId

import numpy as np
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.globals.global_names import INFOS, MULSCALE, MULSHIFT
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import (GeneratorBase,
                                                      paramstype, ktype)
from graph.types import MatMulOpParameters, MatMulOpFusionParameters, ActivationParameters
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

@paramstype(MatMulOpParameters, MatMulOpFusionParameters)
@ktype('scaled')
class MatMulGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, MatMulOpParameters):
            mul_node = pnode
            mul_qrec = qrec
            fnode = pnode
            infos, comment = np.array([0, 0, 0, 0, 0]), "no activation"
        elif isinstance(pnode, MatMulOpFusionParameters) and isinstance(fnode, MatMulOpParameters):
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[NodeId(
                pnode, fnode)] for fnode in cnodes]
            mul_node = cnodes[0]
            mul_qrec = quants[0]
            infos, comment = gen_act_infos(cnodes[1], quants[1])
        else:
            return False

        if len(mul_qrec.in_qs[1].scale) > 1:
            gen_scales(gen, pnode, mul_node, mul_qrec)
            pl_scale = 0
            pl_scalen = 0
        else:
            pl_scale = mul_qrec.cache['mul_biases_q'].qbiases[0]
            pl_scalen = mul_qrec.cache['mul_biases_q'].qnorms[0]

        infos = np.append(infos, [0, 0, pl_scale, pl_scalen])

        if mul_qrec.cache.get('ne16'):
            conv_mul_bias = mul_qrec.cache.get('mul_biases_q')
            prenorm = conv_mul_bias.pre_normalization if isinstance(conv_mul_bias, MultMulBiasScaleQType) else 0
            pad_value = np.array(mul_qrec.in_qs[0].zero_point).astype(np.int16)
            pad_value1 = np.bitwise_and(pad_value, 0xFF)
            pad_value2 = np.bitwise_and(pad_value, 0xFF00) >> 8
            w_offset = -np.array(mul_qrec.in_qs[1].zero_point).astype(np.int32)
            w_offset1 = np.bitwise_and(w_offset, 0xFF)
            w_offset2 = np.bitwise_and(w_offset, 0xFF00) >> 8
            w_offset3 = np.bitwise_and(w_offset, 0xFF0000) >> 16
            w_offset4 = np.bitwise_and(w_offset, 0xFF000000) >> 24

            infos = np.append(
                infos, verify_scalar([prenorm if prenorm else 0, pad_value1, pad_value2, w_offset1, w_offset2, w_offset3, w_offset4]))

        cname, file_name = gen_constant(gen, pnode, fnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(bits=8, q=0, signed=True), contents=infos)
        gen.globals.append(GlobalArgInfo("int8", cname,
                           gen.opts['default_global_home_location'],
                           gen.opts['default_global_exec_location'],
                           const_info=const_info,
                           comment=comment))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, MatMulOpFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(
                node, fnode)] for fnode in cnodes]
            set_matmul_bindings(gen, in_eparams, out_eparams,
                                cname, cnodes[0], quants[0], out_q=quants[-1])
            return True
        if isinstance(node, MatMulOpParameters):
            set_matmul_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True


    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        if isinstance(node, MatMulOpFusionParameters):
            cnodes = node.contained_nodes()
            matmul_node = cnodes[0]
            act_node = cnodes[-1] if isinstance(cnodes[-1],
                                                ActivationParameters) else None
        else:
            matmul_node = node
            act_node = None

        matmul_ker_gen = MatMulKernelNE16 if qrec.cache.get("ne16") else MatMulKernel
        gen.kernels.append(
            matmul_ker_gen(
                node.name, cname, matmul_node, qrec, act_node, force_relu=gen.force_relu))
        return True

def set_matmul_bindings(gen, in_eparams, out_eparams, cname, node, node_q, out_q=None):
    if out_q is None:
        out_q = node_q
    gen.bindings.append(
        CommentBindingList(
            "Node {} inq {} outq {}", node.name,
            str(node_q.in_qs[0]), str(out_q.out_qs[0]))
    )
    if len(node.in_dims) == 3:
        if len(node_q.in_qs[1].scale) > 1:
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
        if len(node_q.in_qs[1].scale) > 1:
            gen.bindings.append(
                NodeBindingList(
                    cname,
                    GNodeArgEdge(in_eparams[1]),
                    GNodeArgEdge(in_eparams[0]),
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
                    GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                    GNodeArgNode(node, INFOS)))


class MatMulKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_MatMulAct_SQ8("{cname}", {gen_ctrl}, {bias_datasize}, 1, 
                  {width_1}, {height_1}, {width_2}, {height_2},
                  0, 0, 1, 1, {matmul_op}, {act_op});
'''

    def __init__(self, node_name, cname, matmul_params, matmul_qrec, act_params, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        if act_params is not None:
            act_op = gen_activation_op(
                act_params.activation, force_relu=force_relu)
        else:
            act_op = 'KOP_NONE'

        height_1 = matmul_params.in_dims[0][0]
        width_1 = matmul_params.in_dims[0][1]
        height_2 = matmul_params.in_dims[1][0]
        width_2 = matmul_params.in_dims[1][1]

        if len(matmul_params.in_dims) == 3:
            bias_datasize = at_bits(matmul_qrec.in_qs[2])
            matmul_op = 'KOP_MATMUL'
        else:
            bias_datasize = 0
            matmul_op = 'KOP_MATMUL_NOBIAS'

        if len(matmul_qrec.in_qs[1].scale) == 1:
            matmul_op += '_SCALE_SCALAR'

        if isinstance(matmul_params, MatMulTransposedParameters):
            matmul_op += '_TRANSPOSED'
            height_2 = matmul_params.in_dims[1][1]
            width_2 = matmul_params.in_dims[1][0]

        # attributes affecting generation
        attrs = {
            'height_1': height_1,
            'width_1': width_1,
            'height_2': height_2,
            'width_2': width_2,
            'bias_datasize': bias_datasize,
            'matmul_op': matmul_op,
            'act_op': act_op
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)

class MatMulKernelNE16(NewAutoTilerKernel):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_MatMulAct_NE16("{cname}", {gen_ctrl}, {in1_datasize}, {out_datasize}, {bias_datasize}, {in2_datasize_bits}, 
                   {width_1}, {height_1}, {width_2}, {height_2},
                   {matmul_op}, {act_op});
'''

    def __init__(self, node_name, cname, matmul_params, matmul_qrec, act_params, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        if act_params is not None:
            act_op = gen_activation_op(
                act_params.activation, force_relu=force_relu)
        else:
            act_op = 'KOP_NONE'

        height_1 = matmul_params.in_dims[0][0]
        width_1 = matmul_params.in_dims[0][1]
        height_2 = matmul_params.in_dims[1][1]
        width_2 = matmul_params.in_dims[1][0]
        bias_datasize = at_bits(matmul_qrec.in_qs[2])
        in1_datasize = at_bits(matmul_qrec.in_qs[0])
        in2_datasize_bits = matmul_qrec.in_qs[1].bits
        out_datasize = at_bits(matmul_qrec.out_qs[0])
        matmul_op = 'KOP_MATMUL_TRANSPOSED'

        # attributes affecting generation
        attrs = {
            'height_1': height_1,
            'width_1': width_1,
            'height_2': height_2,
            'width_2': width_2,
            'bias_datasize': bias_datasize,
            'in1_datasize': in1_datasize,
            'in2_datasize_bits': in2_datasize_bits,
            'out_datasize': out_datasize,
            'matmul_op': matmul_op,
            'act_op': act_op
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
