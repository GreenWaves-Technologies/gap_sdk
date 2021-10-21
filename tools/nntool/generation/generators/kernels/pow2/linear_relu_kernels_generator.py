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

from generation.at_generators.cnn_convolution_pool_relu import \
    gen_activation_op
from generation.at_types.gen_ctrl import GenCtrl
from generation.generator_decorators import QREC_POW2, generation_function
from generation.generators.kernels.pow2.matmul_kernels_generator import \
    MatMulReluKernel
from graph.types import LinearFusionParameters, FcParameters
from utils.node_id import NodeId

from ..autotiler_kernel import NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (LinearFusionParameters, FcParameters), qrec_types=(QREC_POW2, ))
def linear_relu_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, FcParameters):
        if node.batch_size > 1:
            if node.batch_minor:
                raise ValueError(f'linear node {node.name} is set as match minor output. Please run fusion match_batch_minor_linear.')
            gen.kernels.append(MatMulReluKernel(cname, node, qrec,
                                                None, None,
                                                gen_ctrl=node.get_gen_ctrl()))
        else:
            gen.kernels.append(LinearReluKernel(node.name, cname, node, qrec, None, None,
                                                at_ver=gen.opts['at_ver'], gen_ctrl=node.get_gen_ctrl()))
    elif isinstance(node, LinearFusionParameters) and node.fusion_type == "linear_active":
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        linear = cnodes[0]
        if linear.batch_size > 1:
            if linear.batch_minor:
                raise ValueError(f'linear node {linear.name} is set as match minor output. Please run fusion match_batch_minor_linear.')
            gen.kernels.append(MatMulReluKernel(cname, cnodes[0], quants[0],
                                                cnodes[1], quants[1],
                                                gen_ctrl=node.get_gen_ctrl()))
        else:
            gen.kernels.append(LinearReluKernel(node.name, cname, cnodes[0], quants[0],
                                                cnodes[1], quants[1], at_ver=gen.opts['at_ver'],
                                                gen_ctrl=node.get_gen_ctrl()))
    else:
        return False
    return True

class LinearReluKernel(NewAutoTilerKernel):
    TEMPLATE_VER0 = '''
// generator for {node_name}
CNN_LinearReLU("{cname}", {gen_ctrl}, {at_bits(in_q)}, {at_bits(filter_q)},
                {at_bits(bias_q)}, {at_bits(out_q)}, 1, 1, 1, 1, {in_size},
                {out_size}, KOP_LINEAR, {act_op});
'''
    TEMPLATE_VER3 = '''
// generator for {node_name}
CNN_LinearReLU("{cname}", {gen_ctrl}, {at_bits(in_q)}, {at_bits(filter_q)},
                {at_bits(bias_q)}, {at_bits(out_q)}, {in_q.q}, {filter_q.q}, {bias_q.q},
                {out_q.q}, 1, 1, 1, 1, {in_size}, {out_size}, KOP_LINEAR, {act_op});
'''
    def __init__(self, node_name, cname, params, linear_q, act_params, act_q, at_ver=3, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        assert params is not None, "linear should always be included"
        in_size = params.in_dims[0].size()
        out_size = params.out_dims[0].size()
        filter_q = linear_q.in_qs[1]
        in_q = linear_q.in_qs[0]
        out_q = linear_q.out_qs[0]
        bias_q = linear_q.in_qs[2]

        if act_params is not None:
            act_op = gen_activation_op(act_params.activation)
            out_q = act_q.out_qs[0]
            if at_ver < 3:
                if act_params.activation == "relu6" and out_q.q != 0:
                    gen_ctrl.ReluN = 6 << out_q.q
                    gen_ctrl.ReluNNoNorm = 1
            else:
                if act_params.activation == "relun":
                    gen_ctrl.ReluN = act_params.activation_params
        else:
            act_op = "KOP_NONE"

        # attributes used to test equality - i.e. this kernel can be reused
        attrs = {
            'in_q': in_q,
            'filter_q': filter_q,
            'bias_q': bias_q,
            'out_q': out_q,
            'in_size': in_size,
            'out_size': out_size,
            'act_op': act_op
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
        self.at_ver = at_ver

    def get_template(self):
        if self.at_ver < 3:
            return self.TEMPLATE_VER0
        return self.TEMPLATE_VER3
