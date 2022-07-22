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

import os
import typing
from typing import Tuple

from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.generators.helpers.global_names import CUSTOM_INFOS
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.conv2d import Conv2DNode
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import FusionNodeBase
from nntool.graph.types.pooling import PoolingNodeBase
from nntool.quantization.qtype import QType
from nntool.utils.diag_collector import thread_singleton
from nntool.utils.process_header import InfosBase

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph



def decompose_fusions(G: 'NNGraph', fusion: FusionNodeBase, *clses) -> Tuple:
    """Decompose fusion into internal nodes matching clses sequence and extract qrecs from nntool.quantization

    Args:
        G (NNGraph): Graph containing nodes
        fusion (FusionBase): Fusion to decompose
        clses (Sequence[Union[Class, Sequence[Class]]]): classes to check. Sequence can include None to skip.

    Returns:
        (Tuple): Sequence of nodes and sequence of qrecs
    """
    nodes = fusion.contained_nodes()
    skips = [isinstance(cls, (tuple, list)) and None in cls for cls in clses]
    stripped = [tuple(set(cls) - {None}) if skips[idx] else cls for idx, cls in enumerate(clses)]
    qrecs = []
    cls_idx = 0
    for fnode in nodes:
        while not isinstance(fnode, stripped[cls_idx]):
            if skips[cls_idx]:
                cls_idx += 1
            else:
                return None, None
        cls_idx += 1
        qrecs.append(G.quantization[(fusion.name, fnode.name)])
    return nodes, qrecs


def need_padding_in_at(params):
    same_h, same_w = params.padding.calculate_same_h_w(
        params.in_dims[0], params.filter_dim, params.stride, dilation=getattr(params.padding, "dilation", None))
    return params.padding.h >= same_h and params.padding.w >= same_w


def gen_conv_attrs(
        params: Conv2DNode,
        pad_compatibilities,
        pow2_dp=False):
    if params is None:
        return  {
        'kop_conv': 'KOP_NONE',
        'fcx': 0,
        'fcy': 0,
        'dcx': 0,
        'dcy': 0,
        'scx': 0,
        'scy': 0,
        'conv_pad': 0
    }

    pad_compatibilities.append(params.padding.pad_compatibility)
    attrs = {
        'fcx': params.filter_dim.w,
        'fcy': params.filter_dim.h,
        'dcx': params.dilation.w,
        'dcy': params.dilation.h,
        'scx': params.stride.w,
        'scy': params.stride.h,
        'conv_pad': params.has_at_zero_pad() and need_padding_in_at(params) and 1 or 0
    }
    if params.is_depthwise_conv():
        assert params.multiplier == 1, "Multiplier not supported"
        attrs['kop_conv'] = "KOP_CONV_DWDP" if pow2_dp else "KOP_CONV_DW"
    else:
        attrs['kop_conv'] = "KOP_CONV_DP" if pow2_dp else "KOP_CONV"
        if params.is_grouped_conv():
            attrs.update({
                'group_in': params.in_dims[0].c // params.groups,
                'group_out': params.filter_dim.out_c // params.groups,
            })
    return attrs


def gen_pool_attrs(
        params: PoolingNodeBase,
        pad_compatibilities):
    if params is None:
        return {
            'kop_pool': 'KOP_NONE',
            'fpx': 0,
            'fpy': 0,
            'dpx': 0,
            'dpy': 0,
            'spx': 0,
            'spy': 0,
            'pool_pad': 0
        }
    pad_compatibilities.append(params.padding.pad_compatibility)
    attrs = {
        'fpx': params.filter_dim.w,
        'fpy': params.filter_dim.h,
        'dpx': 1,
        'dpy': 1,
        'spx': params.stride.w,
        'spy': params.stride.h,
        'pool_pad': params.has_at_zero_pad() and need_padding_in_at(params) and 1 or 0
    }

    if params.pool_type == "average":
        attrs['kop_pool'] = "KOP_AVGPOOL"
    elif params.pool_type == "max":
        attrs['kop_pool'] = "KOP_MAXPOOL"
    else:
        raise NotImplementedError()
    return attrs


def gen_activation_attrs(
        params: ActivationNodeBase,
        act_q: QType,
        gen_ctrl: GenCtrl,
        force_relu=False):
    if params is None or params.activation == "none":
        aop = "KOP_NONE"
    else:
        activation = params.activation
        asymmetric = False if not act_q or act_q.in_qs[0].is_floating else act_q.in_qs[0].zero_point != 0
        if activation is None or activation == "none":
            aop = "KOP_NONE"
        elif activation == "relu":
            aop = "KOP_RELUM" if asymmetric else "KOP_RELU"
        elif activation == "relu6":
            if asymmetric:
                aop = "KOP_RELUMN" if not force_relu else "KOP_RELUM"
            else:
                aop = "KOP_RELUN" if not force_relu else "KOP_RELU"
        elif activation == "relun":
            if asymmetric:
                aop = "KOP_RELUMN" if not force_relu else "KOP_RELUM"
            else:
                aop = "KOP_RELUN" if not force_relu else "KOP_RELU"
        elif activation == "hsigmoid":
            aop = "KOP_HSIGMOID"
        elif activation == "htanh":
            aop = "KOP_HTANH"
        elif activation == "swish" or activation == "hswish":
            aop = "KOP_HSWISH"
        elif activation == "leaky":
            aop = "KOP_LEAKYRELU"
        elif activation == "sigmoid":
            aop = "KOP_SIGMOID"
        elif activation == "tanh":
            aop = "KOP_TANH"
        elif activation == "custom" and act_q:
            aop = "KOP_CUSTOM"
            infos = act_q.cache['qfunc_col'].infos
            if infos:
                gen_ctrl.custom_activation_name = infos[0].name
                gen_ctrl.custom_activation_infos = infos[0].get_padded_len(
                )
            else:
                gen_ctrl.custom_activation_name = params.name
        else:
            raise NotImplementedError(
                f"Cannot generate model for activation {activation}")
    return {
        'kop_act': aop
    }

@thread_singleton
class SQ8ActInfos(InfosBase):
    def __init__(self) -> None:
        super().__init__('CNN_Infos_SQ8.h', [
            os.environ.get('TILER_CNN_KERNEL_PATH_SQ8')])

def custom_activation_infos(gen, pnode, fnode, act_node, act_qrec):
    if isinstance(act_node, ExpressionFusionNode):
        infos = act_qrec.cache['qfunc_col'].infos
        if infos and infos[0].values:
            cname, file_name = gen_constant(
                gen, pnode, fnode, CUSTOM_INFOS)
            infos, idx = infos
            contents, comment = infos.get_padded_vals(idx)
            const_info = ConstantInfo(file_name, QType.Pow2(
                bits=8, q=0, signed=True), contents=contents)
            gen.globals.append(GlobalArgInfo("int8", cname,
                                                gen.opts['default_global_home_location'],
                                                gen.opts['default_global_exec_location'],
                                                const_info=const_info,
                                                comment=comment))

