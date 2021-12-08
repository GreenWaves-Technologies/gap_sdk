# Copyright (C) 2021  GreenWaves Technologies, SAS

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

from generation.new_generators.mult8.conv_pool_mult8 import verify_scalar
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from generation.new_generators.helpers.act_infos import gen_act_infos
from generation.helpers.gen_scales import gen_scales
from generation.at_types.at_params import gen_activation_op
import logging
from utils.node_id import NodeId

import numpy as np
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.globals.global_names import INFOS, MULSCALE, MULSHIFT
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import (GeneratorBase,
                                                      paramstype, ktype)
from graph.types import LinearFusionParameters, FcParameters
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

@paramstype(LinearFusionParameters, FcParameters)
@ktype('scaled')
class LinearActSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, FcParameters):
            gen_scales(gen, pnode, pnode, qrec)
            infos, infos_comment = np.array([0, 0, 0, 0, 0]), "no activation"
            fnode = pnode
            filt_q = qrec
        elif isinstance(pnode, LinearFusionParameters) and isinstance(fnode, FcParameters) and pnode.fusion_type == "linear_active":
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[NodeId(pnode, fnode)] for fnode in cnodes]
            filt_q = quants[0]
            gen_scales(gen, pnode, cnodes[0], quants[0])
            infos, infos_comment = gen_act_infos(cnodes[1], quants[1])
        else:
            return False
        infos = np.append(infos, [0, 0, 0, 0])
        comment = str.format("BiasQ: {}", 0) + infos_comment
        infos[5] = 0 # BiasQ

        if filt_q.cache.get('ne16'):
            conv_mul_bias = filt_q.cache.get('mul_biases_q')
            prenorm = conv_mul_bias.pre_normalization if isinstance(conv_mul_bias, MultMulBiasScaleQType) else 0
            pad_value = np.array(filt_q.in_qs[0].zero_point).astype(np.int16)
            pad_value1 = np.bitwise_and(pad_value, 0xFF)
            pad_value2 = np.bitwise_and(pad_value, 0xFF00) >> 8
            w_offset = -np.array(filt_q.in_qs[1].zero_point).astype(np.int32)
            w_offset1 = np.bitwise_and(w_offset, 0xFF)
            w_offset2 = np.bitwise_and(w_offset, 0xFF00) >> 8
            w_offset3 = np.bitwise_and(w_offset, 0xFF0000) >> 16
            w_offset4 = np.bitwise_and(w_offset, 0xFF000000) >> 24

            infos = np.append(
                infos, verify_scalar([prenorm if prenorm else 0, pad_value1, pad_value2, w_offset1, w_offset2, w_offset3, w_offset4]))

        cname, file_name = gen_constant(gen, pnode, fnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(bits=8, q=0, signed=True), contents=infos)
        gen.globals.append(GlobalArgInfo("int8", cname,
                           gen.opts['default_global_home_location'],
                           gen.opts['default_global_exec_location'],
                           const_info=const_info,
                           comment=comment))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, FcParameters):
            set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname, node, qrec)
        elif isinstance(node, LinearFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname, cnodes[0], quants[0], out_q=quants[1])
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, FcParameters):
            lin_kernel = LinearActNE16Kernel if qrec.cache.get("ne16") else LinearActSQ8Kernel
            gen.kernels.append(
                lin_kernel(
                    node.name, cname, node, None, qrec, None, gen.force_relu)
            )
        elif isinstance(node, LinearFusionParameters) and node.fusion_type == "linear_active":
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            lin_kernel = LinearActNE16Kernel if quants[0].cache.get("ne16") else LinearActSQ8Kernel
            gen.kernels.append(
                lin_kernel(
                    node.name, cname, cnodes[0], cnodes[1], quants[0], quants[1], gen.force_relu)
            )
        else:
            return False
        return True


def set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                    params, linear_q, out_q=None):
    del step_idx
    if out_q is None:
        out_q = linear_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                           linear_q.in_qs[0], linear_q.in_qs[1], out_q.out_qs[0])
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[1]),
                        GNodeArgEdge(in_eparams[2]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(params, MULSCALE),
                        GNodeArgNode(params, MULSHIFT),
                        GNodeArgNode(params, INFOS)
                        ))

class LinearActKernel(NewAutoTilerKernel):

    def __init__(self, node_name, cname, linear_params, act_params, linear_q, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]
        in_qs = linear_q.in_qs
        mulbiases_q = linear_q.cache['mul_biases_q']

        if act_params is not None:
            at_act_params = gen_activation_op(act_params.activation, force_relu=force_relu,
                                              asymmetric=act_q.in_qs[0].zero_point != 0)
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            out_qs = act_q.out_qs
        else:
            at_act_params = "KOP_NONE"
            out_qs = linear_q.out_qs

        attrs = {
            'in_size': in_qs[0].dtype_bits//8 if in_qs[0].signed else -in_qs[0].dtype_bits//8,
            'out_size': out_qs[0].dtype_bits//8 if out_qs[0].signed else -out_qs[0].dtype_bits//8,
            'bias_size': in_qs[2].dtype_bits//8,
            'scale_size': mulbiases_q.dtype_bits//8,
            'filter_bits': in_qs[1].bits,
            'in_feat': in_dim.size(),
            'out_feat': out_dim.size(),
            'act_op': at_act_params
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)

class LinearActSQ8Kernel(LinearActKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_LinearAct_SQ8("{cname}", {gen_ctrl},
                  {bias_size}, {scale_size},
                  {in_feat}, {out_feat},
                  KOP_LINEAR, {act_op});
"""

class LinearActNE16Kernel(LinearActKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_LinearAct_NE16("{cname}", {gen_ctrl},
                   {in_size}, {out_size}, {bias_size}, {scale_size}, {filter_bits},
                   {in_feat}, {out_feat}, KOP_LINEAR, {act_op});
"""
