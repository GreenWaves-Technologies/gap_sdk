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

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import GeneratorBase, paramstype
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.types import TransposeNode

LOG = logging.getLogger(__name__)


@paramstype(TransposeNode)
class GenTransposeParameters(GeneratorBase, InOutBindingsMixin):
    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_in_out_bindings(
            gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        real_in_shape, real_transpose = node.real_shape()
        if len(real_transpose) <= 1:
            return True
        if len(real_transpose) == 2:
            gen.kernels.append(
                MatTransposeKernel(
                    cname, node, (1, real_in_shape[0], real_in_shape[1]), real_transpose, qrec))
        elif len(real_transpose) == 3:
            dim_names = ['C', 'H', 'W']
            perm = ''.join([dim_names[i] for i in real_transpose])
            gen.kernels.append(
                TensorPermuteKernel(
                    cname, node, real_in_shape, real_transpose, qrec,
                    perm_op=f"KOP_MATPERM_CHW2{perm}"))
        else:
            raise NotImplementedError(
                f"Error generating {node.name}: only 2D or 3D transposes are currently supported")
        LOG.debug("generating for transpose in %s out %s trans %s",
                 node.in_dims[0], node.out_dims[0], node.transpose)
        return True


class TransposeKernelBase(NewAutoTilerKernel):
    def __init__(self, cname, params, in_shape, real_transpose, qrec, perm_op=None, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if qrec.out_qs[0].is_floating:
            gen_ctrl.float_dump = 1

        datasize = qrec.out_qs[0].dtype_bits//8
        if not qrec.out_qs[0].signed:
            datasize = -datasize

        attrs = {
            'in_dim': params.in_dims[0],
            'out_dim': params.out_dims[0],
            'real_transpose': real_transpose,
            'size': params.out_dims[0].size(),
            'features': in_shape[0],
            'height': in_shape[1],
            'width': in_shape[2],
            'perm_op': perm_op,
            'datasize': datasize
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


class MatTransposeKernel(TransposeKernelBase):
    CALL_TEMPLATE = '''
// generator for {node_name} Transpose {in_dim} -> {out_dim} ({real_transpose})
CNN_MatTranspose("{cname}", {gen_ctrl}, {datasize},
                  {features}, {width}, {height}, KOP_MATTRANSP);
'''


class TensorPermuteKernel(TransposeKernelBase):
    CALL_TEMPLATE = '''
// generator for {node_name} Transpose {in_dim} -> {out_dim} ({real_transpose})
CNN_3DTensorPermute("{cname}", {gen_ctrl}, {datasize},
                     {features}, {width}, {height}, {perm_op});
'''
