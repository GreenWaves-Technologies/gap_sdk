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
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from nntool.generation.generators.helpers.global_names import (CUSTOM_INFOS, INFOS, MULSCALE,
                                                        MULSHIFT)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.generation.helpers.gen_scales import gen_scales
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.act_infos import gen_act_infos
from nntool.generation.generators.helpers.fusions import custom_activation_infos, decompose_fusions, gen_activation_attrs
from nntool.generation.generators.mult8.conv_pool_mult8 import SQ8ActInfos
from nntool.graph.types import LinearNode, LinearFusionNode
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from nntool.quantization.qtype import QType

LOG = logging.getLogger(__name__)


@paramstype(LinearFusionNode, LinearNode)
@ktype('scaled')
class LinearActSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        act_node, act_qrec = None, None
        if isinstance(pnode, LinearNode):
            gen_scales(gen, pnode, pnode, qrec)
            infos, infos_comment = {}, "no activation"
            linear_node = pnode
            linear_qrec = qrec
        elif isinstance(pnode, LinearFusionNode) and isinstance(fnode, LinearNode):
            cnodes, quants = decompose_fusions(
                gen.G, pnode, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (linear_qrec, act_qrec) = cnodes, quants
            gen_scales(gen, pnode, linear_node, linear_qrec)
            infos, infos_comment = gen_act_infos(act_node, act_qrec)
        else:
            return False

        infos['BIASN'] = np.int8(0)  # BiasQ
        conv_mul_bias = linear_qrec.cache.get('mul_biases_q')
        infos['PRENORM'] = np.uint8(conv_mul_bias.pre_normalization if isinstance(
            conv_mul_bias, MultMulBiasScaleQType) else 0)

        if linear_qrec.cache.get('ne16'):
            infos['NE16_PADVAL'] = np.atleast_1d(
                linear_qrec.in_qs[0].zero_point).astype(np.uint16)
            infos['NE16_WOFFSET'] = - \
                np.array(linear_qrec.in_qs[1].zero_point).astype(np.int32)
            infos_len = 'NE16_DIM'
        else:
            infos_len = 'DIM'

        infos_encoder = SQ8ActInfos()
        contents, new_comment = infos_encoder.gen_infos_array(
            infos_len, **infos)
        comment = infos_comment + new_comment

        cname, file_name = gen_constant(gen, pnode, linear_node, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=comment))
        custom_activation_infos(gen, pnode, linear_node, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, LinearNode):
            set_fc_bindings(gen, step_idx, in_eparams,
                            out_eparams, cname, node, qrec)
        elif isinstance(node, LinearFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (linear_qrec, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            set_fc_bindings(gen, step_idx, in_eparams, out_eparams,
                            cname, linear_node, linear_qrec, out_q=act_qrec, has_custom=custom_infos)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, LinearNode):
            lin_kernel = LinearActNE16Kernel if qrec.cache.get(
                "ne16") else LinearActSQ8Kernel
            gen.kernels.append(
                lin_kernel(
                    node.name, cname, node, None, qrec, None, gen.force_relu)
            )
        elif isinstance(node, LinearFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (linear_qrec, act_qrec) = cnodes, quants
            lin_kernel = LinearActNE16Kernel if linear_qrec.cache.get(
                "ne16") else LinearActSQ8Kernel
            gen.kernels.append(
                lin_kernel(
                    node.name, cname, linear_node, act_node, linear_qrec, act_qrec, gen.force_relu)
            )
        else:
            return False
        return True


def set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                    params, linear_q, out_q=None, has_custom=False):
    del step_idx
    if out_q is None:
        out_q = linear_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                           linear_q.in_qs[0], linear_q.in_qs[1], out_q.out_qs[0])
    )
    args = [GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
            GNodeArgNode(params, MULSCALE),
            GNodeArgNode(params, MULSHIFT),
            GNodeArgNode(params, INFOS)]
    if has_custom:
        args.append(GNodeArgNode(params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *args
                        ))


class LinearActKernel(NewAutoTilerKernel):

    def __init__(self, node_name, cname, linear_params, act_params, linear_qrec, act_qrec, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]
        in_qs = linear_qrec.in_qs
        mulbiases_q = linear_qrec.cache['mul_biases_q']

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl,
            force_relu=force_relu)

        if act_params is not None:
            if in_dim is None:
                in_dim = act_params.in_dims[0]
            if out_dim is None:
                out_dim = act_params.out_dims[0]
            out_qs = act_qrec.out_qs
        else:
            out_qs = linear_qrec.out_qs

        attrs.update({
            'in_size': in_qs[0].dtype_bits//8 if in_qs[0].signed else -in_qs[0].dtype_bits//8,
            'out_size': out_qs[0].dtype_bits//8 if out_qs[0].signed else -out_qs[0].dtype_bits//8,
            'bias_size': in_qs[2].dtype_bits//8,
            'scale_size': mulbiases_q.dtype_bits//8,
            'filter_bits': in_qs[1].bits,
            'in_feat': in_dim.size(),
            'out_feat': out_dim.size(),
        })

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
                  KOP_LINEAR, {kop_act});
"""


class LinearActNE16Kernel(LinearActKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_LinearAct_NE16("{cname}", {gen_ctrl},
                   {in_size}, {out_size}, {bias_size}, {scale_size}, {filter_bits},
                   {in_feat}, {out_feat}, KOP_LINEAR, {kop_act});
"""
