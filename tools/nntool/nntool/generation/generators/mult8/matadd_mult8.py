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
from functools import reduce

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
from nntool.generation.generators.helpers.fusions import gen_activation_attrs
from nntool.graph.types import (ActivationFusionNodeBase, BroadcastableActivationFusionNode,
                         MatrixAddNode)
from nntool.graph.types.fusions import PaddedAddFusionNode
from nntool.quantization.qtype import QType


LOG = logging.getLogger(__name__)

@paramstype(MatrixAddNode, ActivationFusionNodeBase, BroadcastableActivationFusionNode)
@ktype("scaled")
class MatAddSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, PaddedAddFusionNode):
            return False
        if isinstance(pnode, (ActivationFusionNodeBase, BroadcastableActivationFusionNode)):
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[(pnode.name, cnode.name)] for cnode in cnodes]
            add_node = cnodes[0]
            if isinstance(add_node, MatrixAddNode):
                if fnode:
                    return True
                infos, acomments = gen_act_infos(cnodes[1], quants[1])
                add_quant = quants[0]
            else:
                return False
        else:
            add_node = pnode
            add_quant = qrec
            infos = {}
            acomments = "no activation - "

        infos.update({
            'IN1SCALE': add_quant.cache['scale_in_mul_biases_q'].qbiases,
            'IN1SCALEN': add_quant.cache['scale_in_mul_biases_q'].qnorms,
            'OUTSCALE': add_quant.cache['scale_mul_biases_q'].qbiases,
            'OUTSCALEN': add_quant.cache['scale_mul_biases_q'].qnorms
        })
        if not add_quant.in_qs[0].signed:
            infos['ADD_BIAS'] = add_quant.cache['add_bias_offset']
            infos_len = 'ASYM_ADD_DIM'
        else:
            infos_len = 'DIM'


        infos_encoder = SQ8ActInfos()
        contents, new_comment = infos_encoder.gen_infos_array(infos_len, **infos)
        comments = acomments + new_comment

        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)
        gen.globals.append(GlobalArgInfo("int8", cname,
                                        gen.opts['default_global_home_location'],
                                        gen.opts['default_global_exec_location'],
                                        const_info=const_info,
                                        comment=comments))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, (ActivationFusionNodeBase, BroadcastableActivationFusionNode)):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[(node.name, fnode.name)] for fnode in cnodes]
            if isinstance(cnodes[0], MatrixAddNode):
                qrec = quants[0]
                out_q = quants[1]
            else:
                return False
        else:
            out_q = qrec

        # set_add_in_scale(qrec)
        scaled_idx = qrec.cache['scaled_idx']
        not_scaled_idx = 0 if scaled_idx else 1
        gen.bindings.extend([
            CommentBindingList(
                f"Node {cname} in1q {qrec.in_qs[scaled_idx]}"),
            CommentBindingList(
                f"  in2q {qrec.in_qs[not_scaled_idx]}"),
            CommentBindingList(
                f"  outq {out_q.out_qs[0]} scaled input 0 is node input {scaled_idx}")
        ])
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[scaled_idx]),
                            GNodeArgEdge(in_eparams[not_scaled_idx]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            GNodeArgNode(node, 'infos')
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del out_eparams
        if isinstance(node, (ActivationFusionNodeBase, BroadcastableActivationFusionNode)):
            cnodes = node.contained_nodes()
            if isinstance(cnodes[0], MatrixAddNode):
                if in_eparams[0].dims.size() != in_eparams[1].dims.size():
                    raise ValueError(
                        "missing generator: the matrix add generator only handles adds of tensors of the same size")
                act_q = gen.G.quantization[(node.name, cnodes[1].name)]
                add_q = gen.G.quantization[(node.name, cnodes[0].name)]
                gen.kernels.append(MatAddKernel(node.name, cname, cnodes[0], cnodes[1], add_q, act_q, force_relu=gen.force_relu))
                return True
            return False
        if in_eparams[0].dims.size() != in_eparams[1].dims.size():
            raise ValueError(
                "missing generator: the matrix add generator only handles adds of tensors of the same size")
        gen.kernels.append(MatAddKernel(node.name, cname, node, None, qrec, None, force_relu=gen.force_relu))
        return True


def balanced_factors(num):
    factors = [(x, num//x) for x in range(2, int(num/2)+1) if num % x == 0]
    differences = [abs(x[0] - x[1]) for x in factors]
    min_idx = differences.index(min(differences))
    return factors[min_idx]


def make_three_dims(dims):
    if len(dims) == 1:
        factors = balanced_factors(dims[0])
        return (1, factors[0], factors[1])
    if len(dims) == 2:
        return (1, dims[0], dims[1])
    if len(dims) == 3:
        return dims
    prod = reduce(lambda x, y: x * y, dims[1:])
    factors = balanced_factors(prod)
    return (dims[0], factors[0], factors[1])

class MatAddKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatAddAct_SQ8("{cname}", {gen_ctrl}, {feat}, {width}, {height}, KOP_MATADD, {kop_act});
"""

    def __init__(self, node_name, cname, matadd_params, act_params=None, add_q=None, act_q=None, force_relu=True, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if not add_q.out_qs[0].signed:
            gen_ctrl.output_datasize = -add_q.out_qs[0].dtype_bits//8
        if not add_q.in_qs[0].signed:
            gen_ctrl.input_datasize = -add_q.in_qs[0].dtype_bits//8

        dimensions = make_three_dims(matadd_params.in_dims[0])

        attrs = {
            'feat': dimensions[0],
            'width': dimensions[1],
            'height': dimensions[2]
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
