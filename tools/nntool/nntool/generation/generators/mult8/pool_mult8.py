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
from functools import reduce

import numpy as np
from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.generators.helpers.global_names import INFOS
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.act_infos import gen_act_infos
from nntool.generation.generators.helpers.fusions import (SQ8ActInfos, custom_activation_infos,
                                                       decompose_fusions,
                                                       gen_activation_attrs,
                                                       gen_pool_attrs)
from nntool.generation.generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from nntool.graph.dim import PadDim
from nntool.graph.types import (ActivationFusionNode, ActivationNodeBase,
                         GlobalAveragePoolNode, GlobalPoolingNodeBase,
                         GlobalSumPoolNode, PoolingNodeBase)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.quantization.qtype import QType
from nntool.utils.largest_factor import balanced_divisors

LOG = logging.getLogger(__name__)

# Also handles standalone activations
@paramstype(ActivationFusionNode, GlobalPoolingNodeBase, PoolingNodeBase, ActivationNodeBase)
@ktype('scaled')
class PoolActGenerator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        # Only handle outer fusion
        if pnode and fnode:
            return True
        act_node, act_qrec = None, None
        if isinstance(pnode, (GlobalPoolingNodeBase, PoolingNodeBase)):
            infos = {
                'ACTSCALE': qrec.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
                'ACTSCALEN': qrec.cache['scale_mul_biases_q'].qnorms.astype(np.uint8)
            }
            comment = "no activation"
            pool_node = pnode
            pool_qrec = qrec
        elif isinstance(pnode, ActivationFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, pnode, (GlobalPoolingNodeBase, PoolingNodeBase), (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (pool_node, act_node), (pool_qrec, act_qrec) = cnodes, quants
            infos, comment = gen_act_infos(act_node, act_qrec)
        elif isinstance(pnode, ActivationNodeBase):
            pool_node = None
            infos, comment = gen_act_infos(node, qrec)
        else:
            return False

        if pool_node and isinstance(pool_node, (GlobalSumPoolNode, GlobalAveragePoolNode)):
            if isinstance(pnode, (GlobalPoolingNodeBase, PoolingNodeBase)):
                infos['ACTSCALE'] = infos['ACTSCALEN'] = np.int8(0)
            infos['GLOBAL_SUM_SCALE'] = pool_qrec.cache['scale_mul_biases_q'].qbiases
            infos['GLOBAL_SUM_SCALEN'] = pool_qrec.cache['scale_mul_biases_q'].qnorms

        infos_encoder = SQ8ActInfos()
        contents, new_comment = infos_encoder.gen_infos_array('DIM', **infos)
        comment += new_comment

        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=comment))
        custom_activation_infos(gen, pnode, fnode, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, (GlobalPoolingNodeBase, PoolingNodeBase), (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (_, act_node), (_, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            cls.set_in_out_infos_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec, has_custom=custom_infos)
            return True
        elif isinstance(node, (GlobalPoolingNodeBase, PoolingNodeBase, ActivationNodeBase)):
            cls.set_in_out_infos_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (GlobalPoolingNodeBase, PoolingNodeBase)):
            pool_kernel = PoolActKernel if isinstance(
                node, PoolingNodeBase) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, node, qrec, None, None,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, (GlobalPoolingNodeBase, PoolingNodeBase), (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (pool_node, act_node), (pool_qrec, act_qrec) = cnodes, quants
            pool_kernel = PoolActKernel if isinstance(
                pool_node, PoolingNodeBase) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, pool_node, pool_qrec, act_node, act_qrec,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationNodeBase):
            gen.kernels.append(PoolActKernel(node.name, cname, None, None, node, qrec,
                                             force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))

        return True


class PoolActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_PoolAct_SQ8("{cname}", {gen_ctrl},
                {feat}, {width}, {height},
                {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
                {kop_act});
"""

    def __init__(self, node_name, cname, pool_params, pool_qrec, act_params, act_qrec, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        pad_compatibilities = []
        attrs = {}
        attrs.update(gen_pool_attrs(pool_params, pad_compatibilities))
        if pool_params is not None:
            if pool_params.ker_in_order and pool_params.ker_in_order[0] == ["h", "w", "c"]:
                gen_ctrl.hwc = 1
            in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            in_q = pool_qrec.in_qs[0]
            out_q = pool_qrec.out_qs[0]
        else:
            if act_params is None:
                raise ValueError('neither pool nor activation is set')
            out_dim = in_dim = None

        attrs.update(
            gen_activation_attrs(
                act_params,
                act_qrec,
                gen_ctrl,
                force_relu=force_relu))

        if act_params is not None:
            if in_dim is None:
                in_q = act_qrec.in_qs[0]
                in_dim = act_params.in_dims[0].expand_to_chw()
            if out_dim is None:
                out_dim = act_params.out_dims[0].expand_to_chw()
            out_q = act_qrec.out_qs[0]

        if pad_compatibilities:
            reduction = PadDim.pad_compatibility_reduce(*pad_compatibilities,
                                                        "convolution padding is not compatible with pool padding")
            if not reduction[2]:  # default is balanced pad left
                at_pad_ctrl = next(i for i, v in enumerate(reduction) if v)
                LOG.debug("%s: generating pad control block", node_name)
                gen_ctrl.PadType = at_pad_ctrl

        if not out_q.signed:
            gen_ctrl.output_datasize = -out_q.dtype_bits//8
        if not in_q.signed:
            gen_ctrl.input_datasize = -in_q.dtype_bits//8

        attrs.update({
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'feat': in_dim.c,
            'width': in_dim.w,
            'height': in_dim.h,
        })

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


class GlobalPoolActKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_GlobalPoolAct_SQ8("{cname}", {gen_ctrl},
                      {feat}, {width}, {height},
                      {kop_pool}, {kop_act});
"""

    def __init__(self, node_name, cname, pool_params, pool_q, act_params, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = pool_params.in_dims[0]
        out_dim = pool_params.out_dims[0]
        in_q = pool_q.in_qs[0]
        out_q = pool_q.out_qs[0]
        reduce_sz = reduce(lambda x, y: x * y, (sz for idx, sz in enumerate(in_dim.shape)
                                                if idx not in pool_params.axis), 1)

        feat = reduce_sz
        height, width = balanced_divisors(in_dim.size()/reduce_sz)

        attrs = gen_activation_attrs(
            act_params,
            act_q,
            gen_ctrl,
            force_relu=force_relu)

        if act_params is not None:
            if out_dim is None:
                out_dim = act_params.out_dims[0].expand_to_chw()
            out_q = act_q.out_qs[0]

        if pool_params.pool_type == "average":
            pool_op = "KOP_GLOBAL_AVGPOOL"
        elif pool_params.pool_type == "max":
            pool_op = "KOP_GLOBAL_MAXPOOL"
        elif pool_params.pool_type == "sum":
            pool_op = "KOP_GLOBAL_SUMPOOL"
        else:
            raise NotImplementedError(
                f'global pool type {pool_params.pool_type} has no AT kernel')

        attrs.update({
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'feat': feat,
            'width': width,
            'height': height,
            'kop_pool': pool_op,
        })

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
