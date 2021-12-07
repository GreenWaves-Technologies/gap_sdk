
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

from graph.types.tensor_arithmetic import MatMulTransposedParameters
import logging

from generation.at_generators.cnn_convolution_pool_relu import \
    gen_activation_op
from generation.at_generators.utils import at_bits
from generation.at_types.gen_ctrl import GenCtrl
from generation.generator_decorators import QREC_POW2, generation_function
from graph.types import (ActivationParameters, MatMulOpFusionParameters,
                         MatMulOpParameters)
from utils.node_id import NodeId

from ..autotiler_kernel import NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (MatMulOpParameters, MatMulOpFusionParameters), qrec_types=(QREC_POW2, ))
def matmul_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams

    if len(node.in_dims[0].shape) == 2 and len(node.in_dims[1].shape) == 2:
        if isinstance(node, MatMulOpFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            matmul_node = cnodes[0]
            act_node = cnodes[-1] if isinstance(cnodes[-1],
                                                ActivationParameters) else None
            mul_qrec = quants[0]
            act_qrec = quants[1]
        else:
            matmul_node = node
            act_node = None
            mul_qrec = qrec
            act_qrec = None
        gen.kernels.append(MatMulReluKernel(
            cname, node, matmul_node, mul_qrec, act_node, act_qrec, gen_ctrl=node.get_gen_ctrl()))
        return True
    return False


class MatMulReluKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_MatMul("{cname}", {gen_ctrl}, {at_bits(in1_qtype)}, {at_bits(in2_qtype)},
           {bias_bits}, {at_bits(out_qtype)}, {in1_qtype.q}, {in2_qtype.q}, {bias_q}, {out_qtype.q},
           1, 1, 1, 1,
           {in1_shape[1]}, {in1_shape[0]}, {width_2}, {height_2}, 0, 0, 1, 1,
           {relu_lower}, {relu_upper}, {mult_op}, {act_op});
'''

    def __init__(self, cname, params, matmul_params, matmul_qrec, act_params, act_qrec, gen_ctrl=None, out_qtype=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if len(params.in_dims[0]) != 2 or len(params.in_dims[1]) != 2:
            raise ValueError(f'Matmul {params.name} has inputs of rank {len(params.in_dims[0])} and {len(params.in_dims[1])}'
                             f'which are not supported by the matmul kernel')
        in1_shape = params.in_dims[0].shape
        in2_shape = params.in_dims[1].shape
        height_2 = in2_shape[0]
        width_2 = in2_shape[1]
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

        if isinstance(matmul_params, MatMulTransposedParameters):
            matmul_op += '_TRANSPOSED'
            height_2 = in2_shape[1]
            width_2 = in2_shape[0]

        if act_params is not None:
            act_op = gen_activation_op(act_params.activation)
            out_qtype = act_qrec.out_qs[0]
            relu_lower = 0
            if act_params.activation == "relu6" and out_qtype.q != 0:
                relu_upper = 6 << out_qtype.q
            else:
                relu_upper = 0
        else:
            out_qtype = matmul_qrec.out_qs[0]
            relu_upper = relu_lower = 0
            act_op = "KOP_NONE"

        # attributes used to test equality - i.e. this kernel can be reused
        attrs = {
            'in1_qtype': in1_qtype,
            'in2_qtype': in2_qtype,
            'bias_q': bias_q,
            'bias_bits': bias_bits,
            'out_qtype': out_qtype,
            'in1_shape': in1_shape,
            'height_2': height_2,
            'width_2': width_2,
            'out_shape': out_shape,
            'relu_lower': relu_lower,
            'relu_upper': relu_upper,
            'mult_op': matmul_op,
            'act_op': act_op
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
