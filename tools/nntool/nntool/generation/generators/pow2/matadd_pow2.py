# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import gen_activation_attrs
from nntool.graph.types import (MatrixAddNode)

LOG = logging.getLogger(__name__)


@paramstype(MatrixAddNode)
@ktype("symmetric")
class MatAddPOW2Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList("Node {} inq1 {} inq2 {} outq {}", node.name,
                               qrec.in_qs[0].q, qrec.in_qs[1].q, qrec.out_qs[0].q)
        )
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))

        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if qrec.in_qs[0].q == qrec.in_qs[1].q and qrec.in_qs[0].q == qrec.out_qs[0].q:
            gen.kernels.append(MatrixAddKernel(cname, node, qrec, None, None,
                                               gen_ctrl=node.get_gen_ctrl()))
        else:
            gen.kernels.append(MatrixAddDynKernel(cname, node, qrec, None, None,
                                                  gen_ctrl=node.get_gen_ctrl()))
        return True


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

    def __init__(self, cname, params, matrixadd_q, act_params, act_q, gen_ctrl=None):
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


class MatrixAddKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatAdd("{cname}", {gen_ctrl}, {at_bits(in_q1)}, {at_bits(in_q2)}, {at_bits(out_q)},
        {in_q1.q}, {in_q2.q}, {out_q.q}, 1, 1, 1, {in_dim.shape[0]}, {out_dim.shape[0]},
        {in_dim.shape[1]}, {in_dim.shape[2]}, {kop_add});',
"""

    def __init__(self, cname, params, matrixadd_qrec, act_params, act_qrec, gen_ctrl=None, force_relu=False):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        attrs = {
            "in_dim": params.in_dims[0],
            "out_dim": params.out_dims[0],
            "in_q1": matrixadd_qrec.in_qs[0],
            "in_q2": matrixadd_qrec.in_qs[1],
            "out_q": matrixadd_qrec.out_qs[0],
            "kop_add": "KOP_ADD"
        }

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl,
            force_relu=force_relu)

        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
