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

import numpy as np
from bfloat16 import bfloat16

from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                        GNodeArgNode, NodeBindingList)
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS
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
from nntool.graph.types.expression_fusion import ExpressionFusionNode

LOG = logging.getLogger(__name__)


@paramstype(Conv2DNode, ConvFusionNode, ActivationNodeBase)
@ktype('float')
class ConvActGeneratorFP16(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, ConvFusionNode):
            act_node, act_qrec = None, None
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[(
                pnode.name, fnode.name)] for fnode in cnodes]
            if pnode.fusion_type in ("conv_active_pool", "conv_active"):
                act_node, act_qrec = cnodes[1], quants[1]
            elif pnode.fusion_type == "conv_pool_active":
                act_node, act_qrec = cnodes[2], quants[2]
            custom_activation_infos(gen, pnode, fnode, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, ActivationNodeBase):
            set_conv_bindings(gen, step_idx, in_eparams,
                              out_eparams, cname, node, just_act=True)
        elif isinstance(node, Conv2DNode):
            set_conv_bindings(gen, step_idx, in_eparams,
                              out_eparams, cname, node)
        elif isinstance(node, ConvFusionNode):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(
                node.name, fnode.name)] for fnode in cnodes]
            if node.fusion_type in ("conv_active_pool", "conv_active"):
                if isinstance(cnodes[1], ExpressionFusionNode):
                    infos = quants[1].cache['qfunc_col'].infos
                    custom_infos = infos and infos[0].values
                else:
                    custom_infos = None
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0], has_custom=custom_infos)
            elif node.fusion_type == "conv_pool":
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0])
            elif node.fusion_type == "conv_pool_active":
                if isinstance(cnodes[2], ExpressionFusionNode):
                    infos = quants[2].cache['qfunc_col'].infos
                    custom_infos = infos and infos[0].values
                else:
                    custom_infos = None
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                  cname, cnodes[0], has_custom=custom_infos)
            else:
                return False
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, Conv2DNode):
            conv_kernel = cls.get_kernel(node)
            if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(conv_kernel(node.name, cname, node, None,
                                           None, None, gen_ctrl=node.get_gen_ctrl()))
        # We want to match the pool_act generator for PoolingParameters
        # elif isinstance(node, PoolingParameters):
        #     gen.kernels.append(ConvPoolReluKernel(node.name, cname, None, None,
        #                                           node, qrec, None, None, at_ver=gen.opts['at_ver'],
        #                                           gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationNodeBase):
            # self.set_in_out_bindings(in_eparams, out_eparams, cname, node, qrec)
            if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(ConvPoolReluKernelFP16(node.name, cname, None, None,
                                                      node, qrec, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ConvFusionNode):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(node.name, fnode.name)]
                      for fnode in cnodes]
            if quants[0].in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            conv_kernel = cls.get_kernel(cnodes[0])
            if node.fusion_type == "conv_active_pool":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], cnodes[2],
                                               cnodes[1], quants[1], gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool_active":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], cnodes[1],
                                               cnodes[2], quants[2], gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_active":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], None,
                                               cnodes[1], quants[1], gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], cnodes[1],
                                               None, None, gen_ctrl=node.get_gen_ctrl()))
            else:
                return False
        else:
            return False
        return True

    @classmethod
    def get_kernel(cls, conv_params):
        is_grouped = not conv_params.is_depthwise_conv() and conv_params.is_grouped_conv()
        conv_kernel = ConvPoolReluKernelGroupedFP16 if is_grouped else ConvPoolReluKernelFP16
        return conv_kernel


def set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                      conv_params, has_custom=False, just_act=False):
    del step_idx
    gen.bindings.append(
        CommentBindingList(f"Node {cname}")
    )
    args = [GNodeArgEdge(in_eparams[0])]
    if not just_act:
        args.extend([GNodeArgEdge(in_eparams[1]),
                     GNodeArgEdge(in_eparams[2])])
    args.append(GNodeArgEdge(out_eparams[0], "GNA_OUT"))

    if has_custom:
        args.append(GNodeArgNode(conv_params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *args))


class ConvPoolReluKernel(NewAutoTilerKernel):
    def __init__(self, node_name, cname, conv_params, pool_params,
                 act_params, act_q, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if conv_params:
            if conv_params.ker_in_order and conv_params.ker_in_order[0] == ["h", "w", "c"]:
                gen_ctrl.hwc = 1

            if conv_params.filter_dim.h == 1 and conv_params.filter_dim.w == 1 and gen_ctrl.enableim2col is None:
                gen_ctrl.enableim2col = 1

        pad_compatibilities = []

        in_dim = out_dim = None

        attrs = {}

        attrs.update(gen_conv_attrs(conv_params, pad_compatibilities))
        if conv_params:
            in_dim = conv_params.in_dims[0]
            out_dim = conv_params.out_dims[0]

        attrs.update(gen_pool_attrs(pool_params, pad_compatibilities))
        if pool_params:
            if in_dim is None:
                in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
        attrs.update(
            gen_activation_attrs(
                act_params,
                act_q,
                gen_ctrl))
        if act_params:
            if in_dim is None:
                in_dim = act_params.in_dims[0].expand_to_chw()
            out_dim = act_params.out_dims[0].expand_to_chw()

        attrs.update({
            'in_feat': in_dim.c,
            'out_feat': out_dim.c,
            'in_width': in_dim.w,
            'in_height': in_dim.h,
        })

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                gen_ctrl.PadType = at_pad_ctrl

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


class ConvPoolReluKernelFP16(ConvPoolReluKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_ConvolutionPoolAct_fp16("{cname}", {gen_ctrl},
                             {in_feat}, {out_feat}, {in_width}, {in_height},
                             {kop_conv}, {fcx}, {fcy}, {dcx}, {
                                 dcy}, {scx}, {scy}, {conv_pad},
                             {kop_pool}, {fpx}, {fpy}, {dpx}, {
                                 dpy}, {spx}, {spy}, {pool_pad},
                             {kop_act});
"""


class ConvPoolReluKernelGroupedFP16(ConvPoolReluKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_GroupedConvolutionPoolAct_fp16("{cname}", {gen_ctrl},
                                    {group_in}, {group_out},
                                    {in_feat}, {out_feat}, {
                                        in_width}, {in_height},
                                    {kop_conv}, {fcx}, {fcy}, {dcx}, {
                                        dcy}, {scx}, {scy}, {conv_pad},
                                    {kop_pool}, {fpx}, {fpy}, {dpx}, {
                                        dpy}, {spx}, {spy}, {pool_pad},
                                    {kop_act});
"""
