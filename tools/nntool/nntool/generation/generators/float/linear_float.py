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
                                                       decompose_fusions,
                                                       gen_activation_attrs)
from nntool.graph.types import LinearNode, LinearFusionNode
from nntool.graph.types.activations import ActivationNodeBase
from nntool.graph.types.expression_fusion import ExpressionFusionNode

LOG = logging.getLogger(__name__)


@paramstype(LinearFusionNode, LinearNode)
@ktype('float')
class LinearActFloatGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, LinearFusionNode) and isinstance(fnode, LinearNode) and pnode.fusion_type == "linear_active":
            cnodes, quants = decompose_fusions(
                gen.G, pnode, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (_, act_qrec) = cnodes, quants
            custom_activation_infos(gen, pnode, linear_node, act_node, act_qrec)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        step_idx = node.step_idx
        if isinstance(node, LinearNode):
            set_fc_bindings(gen, step_idx, in_eparams,
                            out_eparams, cname, node)
        elif isinstance(node, LinearFusionNode):
            cnodes, quants = decompose_fusions(
                gen.G, node, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (_, act_qrec) = cnodes, quants
            if isinstance(act_node, ExpressionFusionNode):
                infos = act_qrec.cache['qfunc_col'].infos
                custom_infos = infos and infos[0].values
            else:
                custom_infos = None
            set_fc_bindings(gen, step_idx, in_eparams, out_eparams,
                            cname, linear_node, has_custom=custom_infos)
        else:
            return False
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, LinearNode):
            if qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(
                LinearActFP16Kernel(
                    node.name, cname, node, None, None, gen_ctrl=node.get_gen_ctrl())
            )
        elif isinstance(node, LinearFusionNode) and node.fusion_type == "linear_active":
            cnodes, quants = decompose_fusions(
                gen.G, node, LinearNode, (ExpressionFusionNode, ActivationNodeBase))
            if not cnodes:
                return False
            (linear_node, act_node), (linear_qrec, act_qrec) = cnodes, quants
            if linear_qrec.in_qs[0].dtype not in [bfloat16, np.float16]:
                return False
            gen.kernels.append(
                LinearActFP16Kernel(
                    node.name, cname, linear_node, act_node, act_qrec, gen_ctrl=node.get_gen_ctrl())
            )
        else:
            return False
        return True


def set_fc_bindings(gen, step_idx, in_eparams, out_eparams, cname,
                    params, has_custom=False):
    del step_idx
    gen.bindings.append(
        CommentBindingList(f"Node {params.name}")
    )
    args = [GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(in_eparams[1]),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")]
    if has_custom:
        args.append(GNodeArgNode(params, CUSTOM_INFOS))
    gen.bindings.append(
        NodeBindingList(cname, *args
                        ))


class LinearActFP16Kernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """// generator for {node_name}
CNN_LinearAct_fp16("{cname}", {gen_ctrl},
                  {in_feat}, {out_feat},
                  KOP_LINEAR, {kop_act});
"""
    def __init__(self, node_name, cname, linear_params, act_params, act_qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = linear_params.in_dims[0]
        out_dim = linear_params.out_dims[0]

        attrs = gen_activation_attrs(
            act_params,
            act_qrec,
            gen_ctrl)

        attrs.update({
            'in_feat': in_dim.size(),
            'out_feat': out_dim.size(),
        })

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
