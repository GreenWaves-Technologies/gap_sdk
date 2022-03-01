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
from generation.at_types.at_params import (gen_activation_op,
                                           gen_globalpool_at_params,
                                           gen_pool_at_params)
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.gen_ctrl import GenCtrl
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.globals.global_names import INFOS
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from generation.new_generators.helpers.act_infos import gen_act_infos
from generation.new_generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from generation.new_generators.mult8.conv_pool_mult8 import SQ8ActInfos
from graph.dim import PadDim
from graph.types import (ActivationFusion, ActivationParameters,
                         GlobalPoolingParameters, GlobalSumPoolParameters,
                         PoolingParameters)
from quantization.qtype import QType
from utils.largest_factor import balanced_divisors
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)


@paramstype(ActivationFusion, GlobalPoolingParameters, PoolingParameters, ActivationParameters)
@ktype('scaled')
class PoolActGenerator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, (GlobalPoolingParameters, PoolingParameters, GlobalSumPoolParameters)):
            infos = {
                'ACTSCALE': qrec.cache['scale_mul_biases_q'].qbiases.astype(np.uint8),
                'ACTSCALEN': qrec.cache['scale_mul_biases_q'].qnorms.astype(np.uint8)
            }
            comment = "no activation"
            pool_node = pnode
            pool_q = qrec
        elif isinstance(pnode, ActivationFusion):
            cnodes = pnode.contained_nodes()
            pool_node = cnodes[0]
            if isinstance(pool_node, (GlobalPoolingParameters, PoolingParameters)) and isinstance(cnodes[1], ActivationParameters):
                if fnode is not None:
                    return True
                quants = [gen.G.quantization[NodeId(
                    pnode, fnode)] for fnode in cnodes]
                pool_q = quants[0]
                infos, comment = gen_act_infos(cnodes[1], quants[1])
            else:
                return False
        elif isinstance(pnode, ActivationParameters):
            pool_node = None
            infos, comment = gen_act_infos(node, qrec)
        else:
            return False

        if pool_node and isinstance(pool_node, GlobalSumPoolParameters):
            # compute_in_out_scale(pool_q, in_idx=0, out_idx=0)
            infos['ACTSCALE'] = infos['ACTSCALEN'] = np.int8(0)
            infos['GLOBAL_SUM_SCALE'] = pool_q.cache['scale_mul_biases_q'].qbiases
            infos['GLOBAL_SUM_SCALEN'] = pool_q.cache['scale_mul_biases_q'].qnorms

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
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            if isinstance(cnodes[0], (GlobalPoolingParameters, PoolingParameters, ActivationParameters)):
                cls.set_in_out_infos_bindings(
                    gen, in_eparams, out_eparams, cname, node, qrec)
                return True
            return False
        elif isinstance(node, (GlobalPoolingParameters, PoolingParameters, ActivationParameters)):
            cls.set_in_out_infos_bindings(
                gen, in_eparams, out_eparams, cname, node, qrec)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (GlobalPoolingParameters, PoolingParameters)):
            pool_kernel = PoolActKernel if isinstance(
                node, PoolingParameters) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, node, qrec, None, None,
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(
                node, fnode)] for fnode in cnodes]
            pool_kernel = PoolActKernel if isinstance(
                cnodes[0], PoolingParameters) else GlobalPoolActKernel
            gen.kernels.append(pool_kernel(node.name, cname, cnodes[0], quants[0], cnodes[1], quants[1],
                                           force_relu=gen.force_relu, gen_ctrl=node.get_gen_ctrl()))
        elif isinstance(node, ActivationParameters):
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

    def __init__(self, node_name, cname, pool_params, pool_q, act_params, act_q, force_relu, gen_ctrl=None):
        if gen_ctrl is None:
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        pad_compatibilities = []
        if pool_params is not None:
            if pool_params.ker_in_order and pool_params.ker_in_order[0] == ["h", "w", "c"]:
                gen_ctrl.hwc = 1
            at_pool_params = gen_pool_at_params(
                pool_params, pad_compatibilities)
            pool_pad_settings = {
                'fpx': at_pool_params.Fpx,
                'fpy': at_pool_params.Fpy,
                'dpx': at_pool_params.Dpx,
                'dpy': at_pool_params.Dpy,
                'spx': at_pool_params.Spx,
                'spy': at_pool_params.Spy,
                'pool_pad': at_pool_params.PoolPad,
            }
            in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
            in_q = pool_q.in_qs[0]
            out_q = pool_q.out_qs[0]
            if pool_params.pool_type == "average":
                pool_op = "KOP_AVGPOOL"
            elif pool_params.pool_type == "max":
                pool_op = "KOP_MAXPOOL"
            else:
                raise NotImplementedError()
        else:
            # only activation
            pool_op = "KOP_NONE"
            if act_params is None:
                raise ValueError('neither pool nor activation is set')
            out_dim = in_dim = None

        if act_params is not None:
            act_op = gen_activation_op(
                act_params.activation, force_relu=force_relu, asymmetric=act_q.in_qs[0].zero_point != 0)
            if in_dim is None:
                in_q = act_q.in_qs[0]
                in_dim = act_params.in_dims[0].expand_to_chw()
                pool_pad_settings = {k: 0 for k in ['fpx', 'fpy', 'dpx', 'dpy',
                                                    'spx', 'spy', 'pool_pad']}
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
            'kop_pool': pool_op,
            'kop_act': act_op
        }

        attrs.update(pool_pad_settings)

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
            self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

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

        if pool_params.pool_type == "average":
            pool_op = "KOP_GLOBAL_AVGPOOL"
        elif pool_params.pool_type == "max":
            pool_op = "KOP_GLOBAL_MAXPOOL"
        elif pool_params.pool_type == "sum":
            pool_op = "KOP_GLOBAL_SUMPOOL"
        else:
            raise NotImplementedError(
                f'global pool type {pool_params.pool_type} has no AT kernel')

        attrs = {
            'in_size': in_q.dtype_bits//8 if in_q.signed else -in_q.dtype_bits//8,
            'out_size': out_q.dtype_bits//8 if out_q.signed else -out_q.dtype_bits//8,
            'feat': feat,
            'width': width,
            'height': height,
            'kop_pool': pool_op,
            'kop_act': act_op
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
