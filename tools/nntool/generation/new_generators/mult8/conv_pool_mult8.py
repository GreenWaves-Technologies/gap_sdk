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

import logging
import os

import numpy as np
from generation.at_types.at_params import (NO_POOL, gen_activation_op,
                                           gen_conv_at_params,
                                           gen_pool_at_params)
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.globals.global_names import (INFOS, MULSCALE,
                                                        MULSHIFT)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.helpers.gen_scales import gen_scales
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from generation.new_generators.helpers.act_infos import gen_act_infos
from graph.dim import PadDim
from graph.types import Conv2DParameters, ConvFusionParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.qtype import QType
from utils.diag_collector import thread_singleton
from utils.node_id import NodeId
from utils.process_header import InfosBase

LOG = logging.getLogger("nntool." + __name__)

def verify_scalar(arr):
    return [item.item() if isinstance(item, np.ndarray) else item for item in arr]

@thread_singleton
class SQ8ActInfos(InfosBase):
    def __init__(self) -> None:
        super().__init__('CNN_Infos_SQ8.h', [os.environ.get('TILER_CNN_KERNEL_PATH_SQ8')])

@paramstype(Conv2DParameters, ConvFusionParameters)
@ktype('scaled')
class ConvActGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, Conv2DParameters):
            gen_scales(gen, pnode, pnode, qrec)
            infos, infos_comment = {}, "no activation"
            filt_q = qrec
            fnode = pnode
        elif isinstance(pnode, ConvFusionParameters) and isinstance(fnode, Conv2DParameters):
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[NodeId(pnode, fnode)] for fnode in cnodes]
            filt_q = quants[0]
            gen_scales(gen, pnode, cnodes[0], quants[0])
            if pnode.fusion_type in ("conv_active_pool", "conv_active"):
                infos, infos_comment = gen_act_infos(cnodes[1], quants[1])
            elif pnode.fusion_type == "conv_pool_active":
                infos, infos_comment = gen_act_infos(cnodes[2], quants[2])
            elif pnode.fusion_type == "conv_pool":
                infos, infos_comment = {}, "no activation"
        else:
            return False


        comment = f"BiasQ: {0}" + infos_comment
        infos['BIASN'] = np.int8(0)  # BiasQ

        if filt_q.cache.get('ne16'):
            conv_mul_bias = filt_q.cache.get('mul_biases_q')
            infos['PRENORM'] = np.uint8(conv_mul_bias.pre_normalization if isinstance(conv_mul_bias, MultMulBiasScaleQType) else 0)
            infos['NE16_PADVAL'] = np.atleast_1d(filt_q.in_qs[0].zero_point).astype(filt_q.in_qs[0].dtype)
            infos['NE16_WOFFSET'] =  -np.array(filt_q.in_qs[1].zero_point).astype(np.int32)
            infos_len = 'NE16_DIM'
        else:
            infos_len = 'DIM'


        infos_encoder = SQ8ActInfos()
        contents = infos_encoder.gen_infos_array(infos_len, **infos)

        cname, file_name = gen_constant(gen, pnode, fnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                           gen.opts['default_global_home_location'],
                           gen.opts['default_global_exec_location'],
                           const_info=const_info,
                           comment=comment))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, Conv2DParameters):
            set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname, node, qrec)
        elif isinstance(node, ConvFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            if node.fusion_type in ("conv_active_pool", "conv_active", "conv_pool"):
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                cname, cnodes[0], quants[0], out_q=quants[1])
            elif node.fusion_type == "conv_pool_active":
                set_conv_bindings(gen, step_idx, in_eparams, out_eparams,
                                cname, cnodes[0], quants[0], out_q=quants[2])
            else:
                return False
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, Conv2DParameters):
            conv_kernel = ConvPoolReluKernelNE16 if qrec.cache.get("ne16") else ConvPoolReluKernelSQ8
            gen.kernels.append(conv_kernel(node.name, cname, node, qrec, None, None, None, None,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ConvFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            conv_kernel = ConvPoolReluKernelNE16 if quants[0].cache.get("ne16") else ConvPoolReluKernelSQ8
            if node.fusion_type == "conv_active_pool":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], quants[0], cnodes[2], quants[2], cnodes[1], quants[1],
                                               force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool_active":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1], cnodes[2], quants[2],
                                               force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_active":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], quants[0], None, None, cnodes[1], quants[1],
                                               force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
            elif node.fusion_type == "conv_pool":
                gen.kernels.append(conv_kernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1], None, None,
                                               force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
            else:
                return False
        return True


def set_conv_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                      conv_params, conv_q, out_q=None):
    del step_idx
    if out_q is None:
        out_q = conv_q
    gen.bindings.append(
        CommentBindingList("Node {} inq {} weightsq {} outq {} biasesq {}", cname,
                           conv_q.in_qs[0], conv_q.in_qs[1], out_q.out_qs[0], conv_q.in_qs[2])
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(in_eparams[1]),
                        GNodeArgEdge(in_eparams[2]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(conv_params, MULSCALE),
                        GNodeArgNode(conv_params, MULSHIFT),
                        GNodeArgNode(conv_params, INFOS)
                        ))


class ConvPoolReluKernel(NewAutoTilerKernel):

    def __init__(self, node_name, cname, conv_params, conv_q, pool_params, pool_q, act_params, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        is_ne16 = conv_q.cache.get('ne16')
        hwc = False
        if not is_ne16 and conv_params.ker_in_order and conv_params.ker_in_order[0] == ["h", "w", "c"]:
            hwc = True
            gen_ctrl.hwc = 1
        if not is_ne16 and not hwc and conv_params.filter.h == 1 and conv_params.filter.w == 1 and gen_ctrl.enableim2col is None:
            gen_ctrl.enableim2col = 1

        in_q = filter_q = out_q = bias_q = None
        in_dim = out_dim = None
        pad_compatibilities = []
        at_conv_params = gen_conv_at_params(
            conv_params, pad_compatibilities)
        in_dim = conv_params.in_dims[0]
        out_dim = conv_params.out_dims[0]
        filter_q = conv_q.in_qs[1]
        in_q = conv_q.in_qs[0]
        out_q = conv_q.out_qs[0]
        bias_q = conv_q.in_qs[2]
        pad_val = in_q.zero_point[0]

        if pool_params is not None:
            at_pool_params = gen_pool_at_params(
                pool_params, pad_compatibilities)
            out_dim = pool_params.out_dims[0]
            out_q = pool_q.out_qs[0]
        else:
            at_pool_params = NO_POOL

        if act_params is not None:
            act_op = gen_activation_op(
                act_params.activation, force_relu=force_relu, asymmetric=act_q.in_qs[0].zero_point != 0)
            if out_dim is None:
                out_dim = act_params.out_dims[0].expand_to_chw()
            out_q = act_q.out_qs[0]
        else:
            act_op = "KOP_NONE"

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                self.gen_ctrl.PadType = at_pad_ctrl
        # if conv_params is not None and conv_params.padding != 0:
        #     self.gen_ctrl.explicit_pad_conv = (conv_params.padding.l) | (conv_params.padding.r << 8) | (conv_params.padding.t << 16) | (conv_params.padding.b << 24)
        # if pool_params is not None and pool_params.padding != 0:
        #     self.gen_ctrl.explicit_pad_pool = (pool_params.padding.l) | (pool_params.padding.r << 8) | (pool_params.padding.t << 16) | (pool_params.padding.b << 24)

        attrs = {
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'bias_size': bias_q.dtype_bits//8,
            'filter_bits': filter_q.bits,
            'in_feat': in_dim.c,
            'out_feat': out_dim.c,
            'in_width': in_dim.w,
            'in_height': in_dim.h,
            'kop_conv': at_conv_params.ConvOper,
            'fcx': at_conv_params.Fcx,
            'fcy': at_conv_params.Fcy,
            'dcx': at_conv_params.Dcx,
            'dcy': at_conv_params.Dcy,
            'scx': at_conv_params.Scx,
            'scy': at_conv_params.Scy,
            'conv_pad': at_conv_params.ConvPad,
            'pad_value': pad_val,
            'kop_pool': at_pool_params.PoolOper,
            'fpx': at_pool_params.Fpx,
            'fpy': at_pool_params.Fpy,
            'dpx': at_pool_params.Dpx,
            'dpy': at_pool_params.Dpy,
            'spx': at_pool_params.Spx,
            'spy': at_pool_params.Spy,
            'pool_pad': at_pool_params.PoolPad,
            'kop_act': act_op
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)

class ConvPoolReluKernelNE16(ConvPoolReluKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_ConvolutionNE16("{cname}", {gen_ctrl},
                    {in_size}, {out_size}, {bias_size}, 1, {filter_bits},
                    {in_feat}, {out_feat}, {in_width}, {in_height},
                    {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad}, {pad_value},
                    {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                    {kop_act});
"""

class ConvPoolReluKernelSQ8(ConvPoolReluKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_ConvolutionPoolAct_SQ8("{cname}", {gen_ctrl}, {bias_size}, 1,
                           {in_feat}, {out_feat}, {in_width}, {in_height},
                           {kop_conv}, {fcx}, {fcy}, {dcx}, {dcy}, {scx}, {scy}, {conv_pad},
                           {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                           {kop_act});
"""
