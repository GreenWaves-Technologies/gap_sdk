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
from graph.types import ImageFormatParameters

LOG = logging.getLogger("nntool." + __name__)


@paramstype(ImageFormatParameters)
class GenImageFormat(GeneratorBase, InOutBindingsMixin):

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
        gen.kernels.append(ImageFormatKernel(cname, node, qrec))
        return True


KOP_NORM = {
    "RGB565_RGB888": "KOP_NORM_RGB565",
    "RGB888": "KOP_NORM_RGB888",
    "RGB16": "KOP_NORM_RGB16",
    "BW8": "KOP_NORM_BW",
    "BW16": "KOP_NORM_BW16"
}


class ImageFormatKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_Norm("{cname}", {width}, {height}, {do_offset}, {kop_norm});
'''

    def __init__(self, cname, params, qrec):

        if params.format_change not in KOP_NORM:
            raise NotImplementedError(
                f'{params.format_change} not implemented')

        attrs = {
            'do_offset': 1 if params.norm_func == "OFFSET_INT8" else 0,
            'kop_norm': KOP_NORM[params.format_change],
        }

        in_dim = params.in_dims[0]
        if in_dim.has_key('w') and in_dim.has_key('h'):
            attrs['width'] = in_dim.w
            attrs['height'] = in_dim.h
        else:
            if len(in_dim.shape) > 2:
                LOG.warning(
                    "Input Dim has no hints -> we are assuming HxWxC order in this case "
                    f"-> {in_dim.shape[0]}x{in_dim.shape[1]}x{in_dim.shape[2]}, for .onnx graphs may not be the case")
            attrs['width'] = in_dim.shape[1]
            attrs['height'] = in_dim.shape[0]

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs)
