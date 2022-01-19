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

from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import GeneratorBase, paramstype
from generation.new_generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from graph.types import ResizerParameters

LOG = logging.getLogger("nntool." + __name__)


@paramstype(ResizerParameters)
class GenResizerParameters(GeneratorBase, InOutBindingsMixin):
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
        if qrec.in_qs[0].is_floating:
            kernel = FP16ResizerKernel
        elif qrec.in_qs[0].dtype_bits == 16:
            kernel = Q16ResizerKernel
        else:
            kernel = Q8ResizerKernel

        gen.kernels.append(
            kernel(cname, node, qrec))
        return True


class ResizeKernelBase(NewAutoTilerKernel):
    RESIZE_KOP = {
        "bilinear": "KOP_BILINEAR_RESIZE",
        "nearest_neighbor": "KOP_NEAREST_NEIGHBOR_RESIZE"}

    def __init__(self, cname, params, qrec):

        in_dim = params.in_dims[0]
        if in_dim.has_key('w') and in_dim.has_key('h'):
            in_dim_w, in_dim_h, in_dim_c = in_dim.w, in_dim.h, in_dim.c
        else:
            in_dim_w, in_dim_h, in_dim_c = in_dim.shape[2], in_dim.shape[1], in_dim.shape[0]

        new_shape = params.new_shape
        chin = in_dim_c
        win, wout = in_dim_w, new_shape[1]
        hin, hout = in_dim_h, new_shape[0]

        if in_dim_w == 1 and in_dim_h == 1:
            # If both are 1s the autotiler cannot solve the tiling problem
            # --> exchange channels with one of the HW dimensions (the one that does not change)
            if 1 in new_shape:
                idx_one = new_shape.index(1)
                win, wout = (in_dim_w, new_shape[1]) if idx_one == 0 else (
                    in_dim_c, in_dim_c)
                hin, hout = (in_dim_h, new_shape[0]) if idx_one == 1 else (
                    in_dim_c, in_dim_c)
                chin = 1
            else:
                # If both HW change from 1x1 to HxW this is not going to work
                LOG.warning(f"Resize Node {params.name} has 1x1xc input but resizes both "
                            "HW dimension, could not work in autotiler")

        if params.op_name not in self.RESIZE_KOP:
            raise NotImplementedError(
                f'{params.op_name} resize generator not implemented')

        attrs = {
            'win': win,
            'hin': hin,
            'chin': chin,
            'wout': wout,
            'hout': hout,
            'inout_t': "SIGNED_INOUT" if qrec.in_qs[0].signed else "UNSIGNED_INOUT",
            'resize_kop': self.RESIZE_KOP[params.op_name],
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs)


class FP16ResizerKernel(ResizeKernelBase):
    CALL_TEMPLATE = '''
// generator for {node_name}
GenerateResizeMultiChannel_fp16("{cname}", {win}, {hin}, {wout}, {hout}, {chin}, {inout_t}, {resize_kop});
'''


class Q16ResizerKernel(ResizeKernelBase):
    CALL_TEMPLATE = '''
// generator for {node_name}
GenerateResizeMultiChannelQ16("{cname}", {win}, {hin}, {wout}, {hout}, {chin}, {inout_t}, {resize_kop});
'''


class Q8ResizerKernel(ResizeKernelBase):
    CALL_TEMPLATE = '''
// generator for {node_name}
GenerateResizeMultiChannel("{cname}", {win}, {hin}, {wout}, {hout}, {chin}, {inout_t}, {resize_kop});
'''
