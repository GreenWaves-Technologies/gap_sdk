# Copyright (C) 2020  GreenWaves Technologies, SAS

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
from nntool.quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType

import numpy as np

from nntool.generation.at_types.constant_info import ConstantInfo
from nntool.generation.at_types.gen_ctrl import GenCtrl
from nntool.generation.at_types.tc_arg_info import GlobalArgInfo
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from nntool.generation.generators.helpers.global_names import INFOS
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.helpers.gen_constant import gen_constant
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.act_infos import gen_act_infos
from nntool.generation.generators.mult8.conv_pool_mult8 import SQ8ActInfos
from nntool.generation.generators.mult8.matadd_mult8 import make_three_dims
from nntool.generation.generators.helpers.fusions import gen_activation_attrs
from nntool.graph.types import (ActivationNodeBase, MatrixAddNode,
                         PaddedAddFusionNode)
from nntool.quantization.qtype import QType

LOG = logging.getLogger(__name__)

@paramstype(PaddedAddFusionNode)
@ktype("scaled")
class PaddedMatAddSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        cnodes = pnode.contained_nodes()
        quants = [gen.G.quantization[(pnode.name, cnode.name)] for cnode in cnodes]

        if len(cnodes) == 3:
            assert isinstance(cnodes[2], ActivationNodeBase)
            infos, acomments = gen_act_infos(cnodes[2], quants[2])
        else:
            infos, acomments = {}, "no activation"

        # TODO: Why separate infos? Ask Marco

        infos1 = infos.copy()
        infos1.update({
            'IN1SCALE': quants[1].cache['scale_in_mul_biases_q'].qbiases,
            'IN1SCALEN': quants[1].cache['scale_in_mul_biases_q'].qnorms,
            'OUTSCALE': quants[1].cache['scale_mul_biases_q'].qbiases,
            'OUTSCALEN': quants[1].cache['scale_mul_biases_q'].qnorms
        })
        infos_encoder = SQ8ActInfos()
        contents, comments = infos_encoder.gen_infos_array('DIM', **infos1)

        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                        gen.opts['default_global_home_location'],
                                        gen.opts['default_global_exec_location'],
                                        const_info=const_info,
                                        comment=comments))

        # Padded part needs to apply out scale of the matadd + act scale
        double_scale = MultMulBiasScaleQType(
            dtype=np.uint8,
            scale=quants[1].cache['scale_mul_biases_q'].scale * quants[2].cache['scale_mul_biases_q'].scale \
                if len(cnodes) == 3 else \
                  quants[1].cache['scale_mul_biases_q'].scale
        )
        infos['actscale'] = double_scale.qbiases
        infos['actscalen'] = double_scale.qnorms
        infos_encoder = SQ8ActInfos()
        contents, comments = infos_encoder.gen_infos_array('DIM', **infos)

        cname, file_name = gen_constant(gen, pnode, cnodes[0], INFOS, extra_name='Pad')
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                        gen.opts['default_global_home_location'],
                                        gen.opts['default_global_exec_location'],
                                        const_info=const_info,
                                        comment=comments))
        return True


    @classmethod
    def bindings_generator(cls, gen, pnode, qrec, in_eparams, out_eparams, cname) -> bool:
        cnodes = pnode.contained_nodes()
        quants = [gen.G.quantization[(pnode.name, fnode.name)] for fnode in cnodes]
        add_node = [node for node in cnodes if isinstance(
            node, MatrixAddNode)]
        if add_node:
            quants = [gen.G.quantization[(
                pnode.name, fnode.name)] for fnode in cnodes]

        scaled_idx = quants[1].cache['scaled_idx']
        not_scaled_idx = 0 if scaled_idx else 1
        gen.bindings.append(
            CommentBindingList("Node {} in1q {} in2q {} outq {}", cname,
                               quants[1].in_qs[scaled_idx], quants[1].in_qs[not_scaled_idx], quants[-1].out_qs[0])
        )
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[scaled_idx]),
                            GNodeArgEdge(in_eparams[not_scaled_idx]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            GNodeArgNode(pnode, 'infos'),
                            GNodeArgNode(cnodes[0], 'infos')
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        cnodes = node.contained_nodes()
        if len(cnodes) > 2:
            act_node = cnodes[2]
        else:
            act_node = None
        pad_node = cnodes[0]
        gen.kernels.append(PaddedMatAddKernel(node.name, cname, node, pad_node,
                                              act_node, at_ver=gen.opts['at_ver'], force_relu=gen.force_relu))
        return True


class PaddedMatAddKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatAddPaddedAct_SQ8("{cname}", {gen_ctrl}, {feat}, {width}, {height}, {padtop}, {padbot}, {padded_idx}, KOP_MATADD, {kop_act});
"""

    def __init__(self, node_name, cname, matrixadd_params, pad_params, act_params, at_ver=3, gen_ctrl=None, force_relu=True):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        padtop = pad_params.padding[0][0]
        padbot = pad_params.padding[0][1]
        padded_idx = 0 if matrixadd_params.in_dims[0].size(
        ) > matrixadd_params.in_dims[1].size() else 1
        dimensions0 = make_three_dims(matrixadd_params.in_dims[0])
        dimensions1 = make_three_dims(matrixadd_params.in_dims[1])

        attrs = {
            'feat': max(dimensions0[0], dimensions1[0]),
            'width': dimensions0[1],
            'height': dimensions0[2],
            'padded_idx': padded_idx,
            'padtop': padtop,
            'padbot': padbot
        }

        attrs.update(gen_activation_attrs(
            act_params,
            None,
            gen_ctrl,
            force_relu=force_relu))

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
