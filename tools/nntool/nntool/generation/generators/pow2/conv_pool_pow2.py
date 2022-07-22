# Copyright (C) 2022  GreenWaves Technologies, SAS

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
import os

from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS, MULSCALE
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import (custom_activation_infos,
                                                       gen_activation_attrs,
                                                       gen_conv_attrs,
                                                       gen_pool_attrs)
from nntool.graph.dim import PadDim
from nntool.graph.types import Conv2DNode, ConvFusionNode
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.base import MultiplicativeBiasNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.pooling import PoolingNodeBase

LOG = logging.getLogger(__name__)


@paramstype(Conv2DNode, ConvFusionNode)
@ktype('symmetric')
class ConvActPOW2Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(node, ConvFusionNode):
            if fnode is None and node.fusion_type in [
                "conv_pool_active",
                "conv_active",
                "conv_active_pool",
                "conv_pool"
            ]:
                cnodes = node.contained_nodes()
                quants = [gen.G.quantization[(
                    node.name, fnode.name)] for fnode in cnodes]
                conv_node = cnodes[0]
                conv_qrec = quants[0]
                if len(cnodes) > 1 and isinstance(cnodes[1], ExpressionFusionNode):
                    custom_activation_infos(
                        gen, pnode, fnode, cnodes[1], quants[1])
                elif len(cnodes) > 2 and isinstance(cnodes[2], ExpressionFusionNode):
                    custom_activation_infos(
                        gen, pnode, fnode, cnodes[2], quants[2])
            else:
                return False
        else:
            conv_node = node
            conv_qrec = qrec
        if isinstance(conv_node, MultiplicativeBiasNodeBase) and conv_node.has_mul_bias:
            mul_biases_q = conv_qrec.mul_biases_q

            cname = gen.naming_convension.get_global_name(pnode.name, pnode.step_idx,
                                                          pnode, MULSCALE)
            file_name = os.path.join(gen.opts['tensor_directory'],
                                     cname+".tensor")
            gen.name_cache.set(node, MULSCALE, cname)

            contents = mul_biases_q.quantize(conv_node.mul_biases).astype(mul_biases_q.dtype,
                                                                          order='C',
                                                                          casting='no',
                                                                          copy=True)
            const_info = ConstantInfo(
                file_name, mul_biases_q, contents=contents)

            gen.globals.append(GlobalArgInfo(mul_biases_q.ctype, cname,
                                             gen.opts['default_global_home_location'],
                                             gen.opts['default_global_exec_location'],
                                             const_info=const_info))
        return True


    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, Conv2DNode):
            set_conv_bindings(gen, step_idx, in_eparams,
                              out_eparams, cname, node, qrec)
        elif isinstance(node, ConvFusionNode):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(
                node.name, fnode.name)] for fnode in cnodes]
            if node.fusion_type == "conv_pool_active":
                if isinstance(cnodes[2], ExpressionFusionNode):
                    infos = quants[2].cache['qfunc_col'].infos
                    custom_infos = infos and infos[0].values
                else:
                    custom_infos = None
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0], quants[0], out_q=quants[2], has_custom=custom_infos)
            elif node.fusion_type in ["conv_active", "conv_active_pool"]:
                if isinstance(cnodes[1], ExpressionFusionNode):
                    infos = quants[1].cache['qfunc_col'].infos
                    custom_infos = infos and infos[0].values
                else:
                    custom_infos = None
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0], quants[0], out_q=quants[1], has_custom=custom_infos)
            elif node.fusion_type == "conv_pool":
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0], quants[0], out_q=quants[1])
            else:
                return False
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, Conv2DNode):
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, node, qrec, None,
                                                  None, None, None,
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, PoolingNodeBase):
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                                  node, qrec, None, None,
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationNodeBase):
            gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                                  None, None, node, qrec,
                                                  gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ConvFusionNode):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(
                node.name, fnode.name)] for fnode in cnodes]
            if node.fusion_type == "conv_active_pool":
                gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0],
                                                      cnodes[2], quants[2],
                                                      cnodes[1], quants[1],
                                                      gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool_active":
                gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0],
                                                      cnodes[1], quants[1],
                                                      cnodes[2], quants[2],
                                                      gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_active":
                gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0],
                                                      None, None, cnodes[1],
                                                      quants[1],
                                                      gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool":
                gen.kernels.append(ConvPoolReluKernel(node.name, cname, cnodes[0], quants[0],
                                                      cnodes[1], quants[1], None,
                                                      None, gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "pool_active":
                gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
                                                      cnodes[0], quants[0], cnodes[1], quants[1],
                                                      gen_ctrl=node.get_gen_ctrl()))
            else:
                return False
        else:
            return False
        return True


def set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname, params, conv_q,
                      out_q=None, has_custom=False):
    if out_q is None:
        out_q = conv_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                           conv_q.in_qs[0].q, conv_q.in_qs[1].q, out_q.out_qs[0].q, conv_q.in_qs[2].q)
    )
    if params.has_mul_bias:
        bindings = [
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgNode(params, MULSCALE),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")]
    else:
        bindings = [
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")]
    if has_custom:
        bindings.append(GNodeArgNode(params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *bindings))


class ConvPoolReluKernel(NewAutoTilerKernel):
    CALL_TEMPLATE_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_ConvolutionPoolReLU("{cname}", {gen_ctrl},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad},
                        {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                        {kop_act});
'''
    CALL_TEMPLATE_MULBIAS_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_ConvolutionMulBiasPoolReLU("{cname}", {gen_ctrl},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)},
                        {at_bits(mulbias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {mulbias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad},
                        {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                        {kop_act});
'''
    CALL_TEMPLATE_GROUPED_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_GroupedConvolutionPoolReLU("{cname}", {gen_ctrl},
                        {group_in}, {group_out},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad},
                        {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                        {kop_act});
'''
    CALL_TEMPLATE_GROUPED_MULBIAS_CONV_POOL_RELU = '''
// generator for {node_name}
CNN_GroupedConvolutionMulBiasPoolReLU("{cname}", {gen_ctrl},
                        {group_in}, {group_out},
                        {at_bits(in_qtype)}, {at_bits(filter_qtype)}, {at_bits(bias_qtype)},
                        {at_bits(mulbias_qtype)}, {at_bits(out_qtype)},
                        {in_qtype.q}, {filter_qtype.q}, {bias_qtype.q}, {mulbias_qtype.q}, {out_qtype.q},
                        1, 1, 1, 1, 1, {in_c}, {out_c}, {in_w}, {in_h},
                        {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad},
                        {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                        {kop_act});
'''

    def __init__(self, node_name, cname, conv_params, conv_qrec,
                 pool_params, pool_qrec, act_params, act_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_q = filter_q = out_q = bias_q = mul_biases_q = None
        in_dim = out_dim = None
        pad_compatibilities = []

        attrs = {}
        attrs.update(gen_conv_attrs(conv_params, pad_compatibilities, pow2_dp=True))
        if conv_params is not None:
            in_dim = conv_params.in_dims[0]
            out_dim = conv_params.out_dims[0]
            filter_q = conv_qrec.in_qs[1]
            in_q = conv_qrec.in_qs[0]
            out_q = conv_qrec.out_qs[0]
            bias_q = conv_qrec.in_qs[2]
            if conv_params.has_mul_bias:
                mul_biases_q = conv_qrec.mul_biases_q

        attrs.update(gen_pool_attrs(pool_params, pad_compatibilities))
        if pool_params is not None:
            if in_dim is None:
                in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            if in_q is None:
                in_q = pool_qrec.in_qs[0]
            out_q = pool_qrec.out_qs[0]

        attrs.update(
            gen_activation_attrs(
                act_params,
                act_qrec,
                gen_ctrl))

        if act_params is not None:
            if in_dim is None:
                in_dim = act_params.in_dims[0].expand_to_chw()
            if out_dim is None:
                out_dim = act_params.out_dims[0].expand_to_chw()
            if in_q is None:
                in_q = act_qrec.in_qs[0]
            # Expression is expecting output Q of convolution or pool
            if not isinstance(act_params, ExpressionFusionNode):
                out_q = act_qrec.out_qs[0]
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params

        if conv_params:
            if conv_params.is_grouped_conv() and not conv_params.is_depthwise_conv():
                if mul_biases_q is not None:
                    self.template = 'CALL_TEMPLATE_GROUPED_MULBIAS_CONV_POOL_RELU'
                else:
                    self.template = 'CALL_TEMPLATE_GROUPED_CONV_POOL_RELU'
            else:
                if mul_biases_q is not None:
                    self.template = 'CALL_TEMPLATE_MULBIAS_CONV_POOL_RELU'
                else:
                    self.template = 'CALL_TEMPLATE_CONV_POOL_RELU'
        elif pool_params:
            if in_q.dtype_bits != out_q.dtype_bits:
                raise NotImplementedError(
                    "only homogenious operations are supported at present")
            self.template = 'CALL_TEMPLATE_POOL_RELU'

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                gen_ctrl.PadType = at_pad_ctrl

        attrs.update({
            'in_w': in_dim.w,
            'in_h': in_dim.h,
            'in_c': in_dim.c,
            'out_c': out_dim.c,
            'in_qtype': in_q,
            'out_qtype': out_q,
            'filter_qtype': filter_q,
            'bias_qtype': bias_q,
            'mul_biases_qtype': mul_biases_q,
        })

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)

    def get_template(self):
        return getattr(self, self.template)
