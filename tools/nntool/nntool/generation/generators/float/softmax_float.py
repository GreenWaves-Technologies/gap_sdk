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

import numpy as np
from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.generators.helpers.global_names import (INFOS)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.generation.generators.mult8.conv_pool_mult8 import SQ8ActInfos
from nntool.graph.types import SoftMaxNode
from nntool.quantization.qtype import QType

LOG = logging.getLogger(__name__)

@paramstype(SoftMaxNode)
@ktype('float')
class SoftMaxFP16Generator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams

        is_2d = (node.in_dims[0].size() // node.in_dims[0].shape[node.axis]) != 1
        if is_2d:
            kernel = SoftMax2DKernel(node.name, cname, node, qrec)
        else:
            kernel = SoftMaxKernel(node.name, cname, node, qrec)
        gen.kernels.append(kernel)
        return True

class SoftMaxKernelBase(NewAutoTilerKernel):
    def __init__(self, node_name, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        in_dim = params.in_dims[0]
        axis = params.axis

        softmax_op = 'KOP_SOFTMAX'

        # attributes affecting generation
        attrs = {
            'feat': in_dim.size()//in_dim.shape[axis],
            'dim': in_dim.shape[axis],
            'softmax_op': softmax_op
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


class SoftMaxKernel(SoftMaxKernelBase):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_SoftMax_fp16("{cname}", {gen_ctrl}, {dim}, {softmax_op});
'''

class SoftMax2DKernel(SoftMaxKernelBase):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_SoftMax2D_fp16("{cname}", {gen_ctrl}, {feat}, {dim}, {softmax_op});
'''
