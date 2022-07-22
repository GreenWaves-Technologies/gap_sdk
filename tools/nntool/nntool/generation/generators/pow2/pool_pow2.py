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
from nntool.graph.types import ConvFusionNode
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import ActivationFusionNode
from nntool.graph.types.pooling import PoolingNodeBase

LOG = logging.getLogger(__name__)


@paramstype(ConvFusionNode, ActivationFusionNode, ActivationNodeBase, PoolingNodeBase)
@ktype('symmetric')
class PoolActPOW2Generator(GeneratorBase, InOutBindingsMixin):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(node, (ActivationNodeBase, PoolingNodeBase)):
            return True
        cnodes, quants = decompose_fusions(
            gen.G, pnode, PoolingNodeBase, (ExpressionFusionNode, ActivationNodeBase))
        if not cnodes:
            cnodes, quants = decompose_fusions(
                gen.G, pnode, ActivationNodeBase)
            if not cnodes:
                return False
            return True
        (_, act_node), (_, act_qrec) = cnodes, quants
        custom_activation_infos(
            gen, pnode, fnode, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        custom_infos = None
        if isinstance(node, (ConvFusionNode, ActivationFusionNode)):
            cnodes, quants = decompose_fusions(
                gen.G, node, PoolingNodeBase, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                cnodes, quants = decompose_fusions(
                    gen.G, node, ActivationNodeBase)
                if not cnodes:
                    return False
            else:
                (_, act_node), (_, act_qrec) = cnodes, quants
                if isinstance(act_node, ExpressionFusionNode):
                    infos = act_qrec.cache['qfunc_col'].infos
                    custom_infos = infos and infos[0].values
        elif not isinstance(node, (ActivationNodeBase, PoolingNodeBase)):
            return False
        cls.set_in_out_bindings(
            gen, in_eparams, out_eparams, cname, node, qrec, has_custom=custom_infos)
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        act_node = act_qrec = pool_node = pool_qrec = None
        if isinstance(node, (ConvFusionNode, ActivationFusionNode)):
            cnodes, quants = decompose_fusions(
                gen.G, node, PoolingNodeBase, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                cnodes, quants = decompose_fusions(
                    gen.G, node, ActivationNodeBase)
                if not cnodes:
                    return False
                act_node, act_qrec = cnodes[0], quants[0]
            else:
                (pool_node, act_node), (pool_qrec, act_qrec) = cnodes, quants
        elif isinstance(node, ActivationNodeBase):
            act_node, act_qrec = node, qrec
        elif isinstance(node, PoolingNodeBase):
            if node.pool_type == "sum":
                return False
            pool_node, pool_qrec = node, qrec
        else:
            return False
        gen.kernels.append(PoolReluKernel(node.name, cname, pool_node, pool_qrec, act_node, act_qrec,
                                          gen_ctrl=node.get_gen_ctrl()))
        return True


class PoolReluKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}
CNN_PoolReLU("{cname}", {gen_ctrl},
             {at_bits(in_qtype)}, {at_bits(out_qtype)},
             {in_qtype.q}, {out_qtype.q},
             1, 1, {in_c}, {out_c}, {in_w}, {in_h},
             {kop_pool}, {fpx}, {fpy}, {dpx}, {dpy}, {spx}, {spy}, {pool_pad},
             {kop_act});
'''

    def __init__(self, node_name, cname,
                 pool_params, pool_qrec, act_params, act_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_q = out_q = None
        in_dim = out_dim = None
        pad_compatibilities = []

        attrs = {}
        attrs.update(gen_pool_attrs(pool_params, pad_compatibilities))
        if pool_params is not None:
            in_dim = pool_params.in_dims[0]
            out_dim = pool_params.out_dims[0]
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
            # Expression is expecting output Q of pool
            if not isinstance(act_params, ExpressionFusionNode):
                out_q = act_qrec.out_qs[0]
            if act_params.activation == "relun":
                gen_ctrl.ReluN = act_params.activation_params

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
        })

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
