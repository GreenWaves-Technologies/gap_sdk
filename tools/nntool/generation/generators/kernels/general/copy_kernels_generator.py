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

from generation.at_types.gen_ctrl import GenCtrl
from generation.generator_decorators import generation_function
from graph.types import CopyParameters, StridedSliceParameters

from ..autotiler_kernel import NewAutoTilerKernel

LOG = logging.getLogger("nntool." + __name__)

@generation_function("kernels", (CopyParameters, StridedSliceParameters))
def copy_kernel_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    if isinstance(node, StridedSliceParameters):
        if not all([act_slice[2] == 1 for act_slice in node.act_slice]):
            raise ValueError(f"Cannot generate strided slice layer {node.name} with a copy if stride != 1")
        if not all([act_slice[0] == 0 for act_slice in node.act_slice]):
            raise ValueError(f"Cannot generate strided slice layer {node.name} with a copy if start != 0")
    gen.kernels.append(CopyKernel(cname, node, qrec))
    return True

class CopyKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_Copy("{cname}", {gen_ctrl}, {size});
'''

    def __init__(self, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        attrs = {
            'size': params.out_dims[0].size() * (qrec.out_qs[0].bits//8)
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
 