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

from generation.templet import stringfunction
from generation.at_generators.utils import at_bits
import logging

from generation.at_generators import (NO_ACTIVATION, gen_active_at_params,
                                      gen_at_matrixadd, gen_at_matrixadddyn,
                                      gen_matrixadd_at_params,
                                      gen_matrixadddyn_at_params)
from generation.at_types.gen_ctrl import GenCtrl
from generation.code_block import CodeBlock
from generation.generator_decorators import generation_function, QREC_POW2
from graph.types import MatrixAddParameters

from ..autotiler_kernel import AutotilerKernel, NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)


@generation_function("kernels", (MatrixAddParameters, ), qrec_types=(QREC_POW2, ))
def matadd_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if qrec.in_qs[0].q == qrec.in_qs[1].q and qrec.in_qs[0].q == qrec.out_qs[0].q:
        gen.kernels.append(MatrixAddKernel(cname, node, qrec, None, None,
                                           gen_ctrl=node.get_gen_ctrl(),
                                           at_ver=gen.opts['at_ver']))
    else:
        gen.kernels.append(MatrixAddDynKernel(cname, node, qrec, None, None,
                                              gen_ctrl=node.get_gen_ctrl(),
                                              at_ver=gen.opts['at_ver']))
    return True


class MatrixAddKernel(AutotilerKernel):
    def __init__(self, cname, matrixadd_params, matrixadd_q, act_params, act_q, gen_ctrl=None, at_ver=3):
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        at_matrixadd_params = gen_matrixadd_at_params(matrixadd_params)
        in_dim = matrixadd_params.in_dims[0]
        out_dim = matrixadd_params.out_dims[0]
        in_q1 = matrixadd_q.in_qs[0]
        in_q2 = matrixadd_q.in_qs[1]
        out_q = matrixadd_q.out_qs[0]

        if act_params is not None:
            at_act_params = gen_active_at_params(act_params)
            out_q = act_q.out_qs[0]
            if at_ver < 3:
                if act_params.activation == "relu6" and out_q.q != 0:
                    self.gen_ctrl.ReluN = 6 << out_q.q
                    self.gen_ctrl.ReluNNoNorm = 1
            else:
                if act_params.activation == "relun":
                    self.gen_ctrl.ReluN = act_params.activation_params
        else:
            at_act_params = NO_ACTIVATION

        self.at_matrixadd_params = at_matrixadd_params
        self.in_dim = in_dim
        self.out_dim = out_dim
        self.in_q1 = in_q1
        self.in_q2 = in_q2
        self.out_q = out_q
        self.at_act_params = at_act_params
        self.cname = cname
        self.node_name = matrixadd_params.name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)

        gen_at_matrixadd(code_block, self.cname, self.in_q1, self.in_q2, self.out_q,
                         self.in_dim, self.out_dim, self.at_matrixadd_params,
                         at_ver=self.at_ver, gen_ctrl=self.gen_ctrl)

        return code_block


def insert_axis(shape, make_len):
    if len(shape) > make_len:
        raise ValueError(f'shape {shape} exceeds maximum length {make_len}')
    return tuple(([1] * (make_len - len(shape))) + shape)


class MatrixAddDynKernel(NewAutoTilerKernel):

    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_MatAddDynAdjust("{cname}", {gen_ctrl}, {at_bits(in_q1)}, {at_bits(in_q2)}, {at_bits(out_q)},
                    {in_q1.q}, {in_q2.q}, {out_q.q}, 1, 1, 1, 
                    {in_shape[0]}, {out_shape[0]}, 
                    {in_shape[1]}, {in_shape[2]}, KOP_MATADD_DYNADJUST);
'''
    def __init__(self, cname, params, matrixadd_q, act_params, act_q, gen_ctrl=None, at_ver=3):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_shape1 = insert_axis(params.in_dims[0].shape, 3)
        in_shape2 = insert_axis(params.in_dims[1].shape, 3)
        out_shape = insert_axis(params.out_dims[0].shape, 3)
        if in_shape1 != in_shape2 or in_shape1 != out_shape:
            raise ValueError(f'CNN_MatAddDynAdjust has been selected for {params.name} but '
                             f'has shape {in_shape1} {in_shape2} -> {out_shape}')

        in_q1 = matrixadd_q.in_qs[0]
        in_q2 = matrixadd_q.in_qs[1]
        out_q = matrixadd_q.out_qs[0]

        if act_params is not None:
            raise ValueError(f'CNN_MatAddDynAdjust has been selected for {params.name} but '
                             f'it has a fused activation which is not compatible')

        attrs = {
            'in_q1': in_q1,
            'in_q2': in_q2,
            'out_q': out_q,
            'in_shape': in_shape1,
            'out_shape': out_shape
        }

        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
