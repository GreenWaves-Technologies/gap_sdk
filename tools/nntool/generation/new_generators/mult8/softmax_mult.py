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
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.globals.global_names import (INFOS)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.helpers.gen_scales import gen_scales
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from generation.new_generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from generation.new_generators.mult8.conv_pool_mult8 import SQ8ActInfos
from graph.types import SoftMaxParameters
from quantization.qtype import QType
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

@paramstype(SoftMaxParameters)
@ktype('scaled')
class SoftMaxGenerator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        assert qrec.in_qs[0].zero_point == 0 and qrec.out_qs[0].zero_point == 0, "asymmetric not supported"
        norm = 15 + np.ceil(np.log2(qrec.in_qs[0].scale))
        infos = { 
            'BIASL_SM': np.uint8(15 + np.ceil(np.log2(qrec.in_qs[0].scale)))
        }
        comment = f"in: {qrec.in_qs[0].scale[0]:.5f} out: {qrec.out_qs[0].scale[0]:.5f} "

        infos_encoder = SQ8ActInfos()
        contents, new_comment = infos_encoder.gen_infos_array('DIM', **infos)
        comment += new_comment

        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                           gen.opts['default_global_home_location'],
                           gen.opts['default_global_exec_location'],
                           const_info=const_info,
                           comment=comment))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cls.set_in_out_infos_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
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

        if (not qrec.out_qs[0].dtype_bits == 16) or (not qrec.out_qs[0].signed):
            self.gen_ctrl.output_datasize = qrec.out_qs[0].dtype_bits//8 if qrec.out_qs[0].signed else -qrec.out_qs[0].dtype_bits//8

        in_dim = params.in_dims[0]
        axis = params.axis

        softmax_op = 'KOP_SOFTMAX'

        # attributes affecting generation
        attrs = {
            'size': in_dim.size(),
            'width': in_dim.size()//in_dim.shape[axis],
            'height': in_dim.shape[axis],
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
CNN_SoftMax_SQ8("{cname}", {gen_ctrl}, {size}, {softmax_op});
'''

class SoftMax2DKernel(SoftMaxKernelBase):
    CALL_TEMPLATE = '''// generator for {node_name}
CNN_SoftMax2D_SQ8("{cname}", {gen_ctrl}, {height}, {width}, {softmax_op});
'''
