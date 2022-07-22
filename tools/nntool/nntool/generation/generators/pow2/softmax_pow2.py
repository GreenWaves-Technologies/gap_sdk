# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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
from nntool.generation.gen_utils import at_bits
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.types import SoftMaxNode

LOG = logging.getLogger(__name__)

@paramstype(SoftMaxNode)
@ktype('symmetric')
class SoftMaxPOW2Generator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams

        is_2d = (node.in_dims[0].size() // node.in_dims[0].shape[node.axis]) != 1
        if is_2d:
            return False
        else:
            kernel = SoftMaxKernel(node.name, cname, node, qrec)
        gen.kernels.append(kernel)
        return True

class SoftMaxKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_SoftMax("{cname}", {gen_ctrl}, {in_bits}, {out_bits}, {in_q}, {out_q}, 1, 1, {size}, {softmax_op});
'''
    def __init__(self, node_name, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]

        in_dim = params.in_dims[0]
        axis = params.axis

        # attributes affecting generation
        attrs = {
            'in_bits': at_bits(in_q),
            'out_bits': at_bits(out_q),
            'in_q': in_q.q,
            'out_q': out_q.q, 
            'size': in_dim.size(),
            'width': in_dim.size()//in_dim.shape[axis],
            'height': in_dim.shape[axis],
            'softmax_op': 'KOP_SOFTMAX'
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
