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
from functools import reduce

import numpy as np
from bfloat16 import bfloat16
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import (custom_activation_infos,
                                                       decompose_fusions,
                                                       gen_activation_attrs,
                                                       gen_pool_attrs)
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.dim import PadDim
from nntool.graph.types import (ActivationFusionNode, GlobalPoolingNodeBase,
                         PoolingNodeBase)
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.utils.largest_factor import balanced_divisors

LOG = logging.getLogger(__name__)


@paramstype(ActivationFusionNode, GlobalPoolingNodeBase, PoolingNodeBase)
@ktype('float')
class PoolActGenerator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, ActivationFusionNode) and isinstance(fnode, (GlobalPoolingNodeBase, PoolingNodeBase)):
            cnodes, quants = decompose_fusions(
                gen.G, pnode, (GlobalPoolingNodeBase, PoolingNodeBase), (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (_, act_qrec) = cnodes, quants
            custom_activation_infos(
                gen, pnode, linear_node, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node,
                (GlobalPoolingNodeBase, PoolingNodeBase),
                (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (pool_node, act_node), (_, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            cls.set_in_out_bindings(gen, in_eparams, out_eparams, cname,
                                    pool_node, qrec, has_custom=custom_infos)
            return True
        elif isinstance(node, (GlobalPoolingNodeBase, PoolingNodeBase)):
            cls.set_in_out_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (GlobalPoolingNodeBase, PoolingNodeBase)):
            if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            pool_kernel = PoolActKernel if isinstance(
                node, PoolingNodeBase) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, node, qrec, None, None,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node,
                (GlobalPoolingNodeBase, PoolingNodeBase),
                (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (pool_node, act_node), (pool_qrec, act_qrec) = cnodes, quants
            if pool_qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            pool_kernel = PoolActKernel if isinstance(
                pool_node, PoolingNodeBase) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, pool_node, pool_qrec, act_node, act_qrec,
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
            gen_ctrl.hwc = 1

        pad_compatibilities = []

        in_dim = pool_params.in_dims[0]

        attrs = {
            'feat': in_dim.c,
            'width': in_dim.w,
            'height': in_dim.h,
        }

        attrs.update(gen_pool_attrs(pool_params, pad_compatibilities))
        attrs.update(
            gen_activation_attrs(
                act_params,
                act_q,
                gen_ctrl))

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug(f"{node_name}: generating pad control block")
                self.gen_ctrl.PadType = at_pad_ctrl

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

        kop_pool = {
            'average': "KOP_GLOBAL_AVGPOOL",
            'max': "KOP_GLOBAL_MAXPOOL",
            'sum': "KOP_GLOBAL_SUMPOOL"
        }.get(pool_params.pool_type)
        if not kop_pool:
            raise NotImplementedError(
                f'{pool_params.name} unsupported pool type {pool_params.pool_type}')

        in_dim = pool_params.in_dims[0]

        feat = reduce(
            lambda x, y: x * y, (sz for idx, sz in enumerate(in_dim.shape)
                                 if idx not in pool_params.axis),
            1)
        height, width = balanced_divisors(in_dim.size()/feat)

        attrs = {
            'feat': feat,
            'width': width,
            'height': height,
            'kop_pool': kop_pool,
        }

        attrs.update(
            gen_activation_attrs(
                act_params,
                act_q,
                gen_ctrl))

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
