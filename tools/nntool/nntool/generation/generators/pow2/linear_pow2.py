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
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge, GNodeArgNode,
                                 NodeBindingList)
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import (custom_activation_infos, decompose_fusions,
                                                       gen_activation_attrs)
from nntool.generation.generators.pow2.matmul_pow2 import MatMulActKernel
from nntool.graph.types import LinearNode, LinearFusionNode
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode

LOG = logging.getLogger(__name__)


@paramstype(LinearFusionNode, LinearNode)
@ktype('symmetric')
class LinearActPOW2Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        # if isinstance(node, MultiplicativeBiasParameters) and node.has_mul_bias:
        #     mul_biases_q = qrec.mul_biases_q

        #     cname = gen.naming_convension.get_global_name(pnode.name, pnode.step_idx,
        #                                                   pnode, MULSCALE)
        #     file_name = os.path.join(gen.opts['tensor_directory'],
        #                              cname+".tensor")
        #     gen.name_cache.set(node, MULSCALE, cname)

        #     contents = mul_biases_q.quantize(node.mul_biases).astype(mul_biases_q.dtype,
        #                                                              order='C',
        #                                                              casting='no',
        #                                                              copy=True)
        #     const_info = ConstantInfo(
        #         file_name, mul_biases_q, contents=contents)

        #     gen.globals.append(GlobalArgInfo(mul_biases_q.ctype, cname,
        #                                      gen.opts['default_global_home_location'],
        #                                      gen.opts['default_global_exec_location'],
        #                                      const_info=const_info))
        if isinstance(node, LinearFusionNode) and fnode is None:
            cnodes, quants = decompose_fusions(
                gen.G, pnode, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (_, act_node), (_, act_qrec) = cnodes, quants
            custom_activation_infos(
                gen, pnode, fnode, act_node, act_qrec)
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
                            cname, linear_node, linear_qrec, out_q=quants[1],
                            has_custom=custom_infos)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        if isinstance(node, LinearNode):
            if node.batch_size > 1:
                if node.batch_minor:
                    raise ValueError(
                        f'linear node {node.name} is set as match minor output. Please run fusion match_batch_minor_linear.')
                gen.kernels.append(MatMulActKernel(cname, node, node, qrec,
                                                   None, None,
                                                   gen_ctrl=node.get_gen_ctrl()))
            else:
                gen.kernels.append(LinearActKernel(node.name, cname, node, qrec, None, None,
                                                   gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, LinearFusionNode) and node.fusion_type == "linear_active":
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(
                node.name, fnode.name)] for fnode in cnodes]
            linear = cnodes[0]
            if linear.batch_size > 1:
                if linear.batch_minor:
                    raise ValueError(
                        f'linear node {linear.name} is set as match minor output. Please run fusion match_batch_minor_linear.')
                gen.kernels.append(MatMulActKernel(cname, node, cnodes[0], quants[0],
                                                   cnodes[1], quants[1],
                                                   gen_ctrl=node.get_gen_ctrl()))
            else:
                gen.kernels.append(LinearActKernel(node.name, cname, cnodes[0], quants[0],
                                                   cnodes[1], quants[1],
                                                   gen_ctrl=node.get_gen_ctrl()))
        else:
            return False
        return True


def set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                    params, linear_q, out_q=None, has_custom=False):
    if out_q is None:
        out_q = linear_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {}", params.name,
                           linear_q.in_qs[0].q, linear_q.in_qs[1].q, out_q.out_qs[0].q)
    )
    bindings = [
        GNodeArgEdge(in_eparams[0]),
        GNodeArgEdge(in_eparams[1]),
        GNodeArgEdge(in_eparams[2]),
        GNodeArgEdge(out_eparams[0], "GNA_OUT")
    ]
    if has_custom:
        bindings.append(GNodeArgNode(params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *bindings))


class LinearActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_LinearReLU("{cname}", {gen_ctrl}, {at_bits(in_q)}, {at_bits(filter_q)},
                {at_bits(bias_q)}, {at_bits(out_q)}, {in_q.q}, {filter_q.q}, {bias_q.q},
                {out_q.q}, 1, 1, 1, 1, {in_size}, {out_size}, KOP_LINEAR, {kop_act});
"""

    def __init__(self, node_name, cname, linear_params, linear_qrec, act_params, act_qrec, gen_ctrl=None, force_relu=False):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]
        in_qs = linear_qrec.in_qs
        out_qs = linear_qrec.out_qs

        filter_q = in_qs[1]
        in_q = in_qs[0]
        out_q = out_qs[0]
        bias_q = in_qs[2]

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
            # Expression is expecting output Q of convolution
            if not isinstance(act_params, ExpressionFusionNode):
                out_q = act_qrec.out_qs[0]
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params

        attrs.update({
            'in_q': in_q,
            'filter_q': filter_q,
            'bias_q': bias_q,
            'out_q': out_q,
            'in_size': in_dim.size(),
            'out_size': out_dim.size(),
        })

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
