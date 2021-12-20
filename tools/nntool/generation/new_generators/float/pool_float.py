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

from functools import reduce
from utils.largest_factor import balanced_divisors
from quantization.multiplicative.mulbias import compute_in_out_scale
from generation.new_generators.helpers.in_out_bindings_mixin import InOutBindingsMixin
from graph.types.global_pooling import GlobalAveragePoolParameters, GlobalSumPoolParameters
from graph.dim import PadDim
from generation.new_generators.helpers.act_infos import gen_act_infos
from generation.at_types.at_params import NO_POOL, gen_activation_op, gen_globalpool_at_params, gen_pool_at_params
import logging
from utils.node_id import NodeId

import numpy as np
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.globals.global_names import INFOS
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import (GeneratorBase,
                                                      paramstype, ktype)
from graph.types import GlobalPoolingParameters, PoolingParameters, ActivationFusion
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

@paramstype(ActivationFusion, GlobalPoolingParameters, PoolingParameters)
@ktype('float')
class PoolActGenerator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            if isinstance(cnodes[0], (GlobalPoolingParameters, PoolingParameters)):
                cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname, cnodes[0], qrec)
                return True
            return False
        elif isinstance(node, (GlobalPoolingParameters, PoolingParameters)):
            cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (GlobalPoolingParameters, PoolingParameters)):
            pool_kernel = PoolActKernel if isinstance(node, PoolingParameters) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, node, qrec, None, None,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            pool_kernel = PoolActKernel if isinstance(cnodes[0], PoolingParameters) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        return True

class PoolActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_PoolAct_fp16("{cname}", {gen_ctrl},
                 {feat}, {feat}, {width}, {height},
                 {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                 {kop_act});
"""
    def __init__(self, node_name, cname, pool_params, pool_q, act_params, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl
        if pool_params.ker_in_order and pool_params.ker_in_order[0] == ["h", "w", "c"]:
            hwc = True
            gen_ctrl.hwc = 1

        pad_compatibilities = []
        at_pool_params = gen_pool_at_params(
            pool_params, pad_compatibilities)
        in_dim = pool_params.in_dims[0]
        out_dim = pool_params.out_dims[0]
        in_q = pool_q.in_qs[0]
        out_q = pool_q.out_qs[0]

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

        attrs = {
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'feat': in_dim.c,
            'width': in_dim.w,
            'height': in_dim.h,
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

class GlobalPoolActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_GlobalPoolAct_fp16("{cname}", {gen_ctrl},
                       {feat}, {feat}, {width}, {height},
                       {kop_pool}, {kop_act});
"""
    def __init__(self, node_name, cname, pool_params, pool_q, act_params, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        at_pool_params = gen_globalpool_at_params(pool_params)
        in_dim = pool_params.in_dims[0]
        out_dim = pool_params.out_dims[0]
        in_q = pool_q.in_qs[0]
        out_q = pool_q.out_qs[0]
        reduce_sz = reduce(lambda x, y: x * y, (sz for idx, sz in enumerate(in_dim.shape)
                                                if idx not in pool_params.axis), 1)
        #self.c = in_dim.size()/reduce_sz
        feat = reduce_sz
        height, width = balanced_divisors(in_dim.size()/reduce_sz)

        if act_params is not None:
            act_op = gen_activation_op(
                act_params.activation, force_relu=force_relu, asymmetric=act_q.in_qs[0].zero_point != 0)
            if out_dim is None:
                out_dim = act_params.out_dims[0].expand_to_chw()
            out_q = act_q.out_qs[0]
        else:
            act_op = "KOP_NONE"

        attrs = {
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'feat': feat,
            'width': width,
            'height': height,
            'kop_pool': at_pool_params.GlobalPoolOper,
            'kop_act': act_op
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
