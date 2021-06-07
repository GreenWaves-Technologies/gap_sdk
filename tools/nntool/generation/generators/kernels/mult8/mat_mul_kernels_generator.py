# Copyright (C) 2020, 2021  GreenWaves Technologies, SAS

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

from generation.at_generators.utils import at_bits
from generation.at_types.at_params import gen_activation_op
from generation.at_types.gen_ctrl import GenCtrl
from generation.generator_decorators import QREC_MULT8, generation_function
from graph.types import (ActivationParameters, MatMulOpFusionParameters,
                         MatMulOpParameters)

from ..autotiler_kernel import NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)

MAT_MUL_OPER = "KOP_MATMUL"


@generation_function(
    "kernels",
    (MatMulOpParameters, MatMulOpFusionParameters),
    qrec_types=(QREC_MULT8, ))
def mat_mul_kernel_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, MatMulOpFusionParameters):
        cnodes = node.contained_nodes()
        act_node = cnodes[-1] if isinstance(cnodes[-1],
                                            ActivationParameters) else None
    else:
        act_node = None

    gen.kernels.append(
        MatMulKernel(
            node.name, cname, node, qrec, act_node, force_relu=gen.force_relu))
    return True

class MatMulKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_MatMulAct_SQ8("{cname}", {gen_ctrl}, {bias_datasize}, 1, {width_1},
                  {height_1}, {width_2}, {height_2}, 0, 0, 1, 1, {matmul_op}, {act_op});
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

        if len(matmul_params.in_dims) == 3:
            bias_datasize = at_bits(matmul_qrec.in_qs[2])
            if len(matmul_qrec.in_qs[0].scale) == 1:
                matmul_op = 'KOP_MATMUL_SCALE_SCALAR'
            else:
                matmul_op = 'KOP_MATMUL'
        else:
            bias_datasize = 0
            matmul_op = 'KOP_MATMUL_NOBIAS'

        height_1 = matmul_params.in_dims[0][0]
        width_1 = matmul_params.in_dims[0][1]
        height_2 = matmul_params.in_dims[1][0]
        width_2 = matmul_params.in_dims[1][1]

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
