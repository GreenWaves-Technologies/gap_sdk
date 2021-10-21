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

from graph.types.fusions import PaddedAddFusionParameters
import logging
from functools import reduce

import numpy as np
from generation.at_types.at_params import NO_ACTIVATION, gen_active_at_params
from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.globals.mult8_infos_generator import act_infos
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from graph.types import ActivationFusionBase, MatrixAddParameters
from quantization.multiplicative.mulbias import set_add_in_scale
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

@paramstype(MatrixAddParameters, ActivationFusionBase)
@ktype("scaled")
class MatAddSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if isinstance(pnode, PaddedAddFusionParameters):
            return False
        if isinstance(pnode, ActivationFusionBase):
            cnodes = pnode.contained_nodes()
            quants = [gen.G.quantization[NodeId(pnode, cnode)] for cnode in cnodes]
            if isinstance(fnode, MatrixAddParameters):
                set_add_in_scale(quants[0])
                if not quants[0].in_qs[0].signed:
                    add_quant_bias_value1 = np.bitwise_and(quants[0].cache['add_bias_offset'], 0xFF)
                    add_quant_bias_value2 = np.bitwise_and(quants[0].cache['add_bias_offset'], 0xFF00) >> 8
                else:
                    add_quant_bias_value1 = add_quant_bias_value2 = None

                act_infos(gen, pnode, pnode, cnodes[1], quants[1],
                          extra1=quants[0].cache['scale_in_mul_biases_q'].qbiases[0],
                          extra2=quants[0].cache['scale_in_mul_biases_q'].qnorms[0],
                          extra3=quants[0].cache['scale_mul_biases_q'].qbiases[0],
                          extra4=quants[0].cache['scale_mul_biases_q'].qnorms[0],
                          extra5=add_quant_bias_value1,
                          extra6=add_quant_bias_value2)
                return True
            else:
                return False
        set_add_in_scale(qrec)
        if not qrec.in_qs[0].signed:
            add_quant_bias_value1 = np.bitwise_and(qrec.cache['add_bias_offset'], 0xFF)
            add_quant_bias_value2 = np.bitwise_and(qrec.cache['add_bias_offset'], 0xFF00) >> 8
        else:
            add_quant_bias_value1 = add_quant_bias_value2 = None
        act_infos(gen, pnode, pnode, None, None,
                extra1=qrec.cache['scale_in_mul_biases_q'].qbiases[0],
                extra2=qrec.cache['scale_in_mul_biases_q'].qnorms[0],
                extra3=qrec.cache['scale_mul_biases_q'].qbiases[0],
                extra4=qrec.cache['scale_mul_biases_q'].qnorms[0],
                extra5=add_quant_bias_value1,
                extra6=add_quant_bias_value2)
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusionBase):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            if isinstance(cnodes[0], MatrixAddParameters):
                qrec = quants[0]
                out_q = quants[1]
            else:
                return False
        else:
            out_q = qrec

        set_add_in_scale(qrec)
        scaled_idx = qrec.cache['scaled_idx']
        not_scaled_idx = 0 if scaled_idx else 1
        gen.bindings.append(
            CommentBindingList("Node {} in1q {} in2q {} outq {}", cname,
                            qrec.in_qs[scaled_idx], qrec.in_qs[not_scaled_idx], out_q.out_qs[0])
        )
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
        if isinstance(node, ActivationFusionBase):
            cnodes = node.contained_nodes()
            if isinstance(cnodes[0], MatrixAddParameters):
                if in_eparams[0].dims.size() != in_eparams[1].dims.size():
                    raise ValueError(
                        "missing generator: the matrix add generator only handles adds of tensors of the same size")
                act_q = gen.G.quantization[NodeId(node, cnodes[1])]
                add_q = gen.G.quantization[NodeId(node, cnodes[0])]
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
CNN_MatAddAct_SQ8("{cname}", {gen_ctrl}, {feat}, {width}, {height}, KOP_MATADD, {act_oper});
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

        if act_params is not None:
            at_act_params = gen_active_at_params(
                act_params, force_relu=force_relu, asymmetric=act_q.in_qs[0].zero_point != 0)
        else:
            at_act_params = NO_ACTIVATION

        dimensions = make_three_dims(matadd_params.in_dims[0])

        attrs = {
            'feat': dimensions[0],
            'width': dimensions[1],
            'height': dimensions[2],
            'act_oper': at_act_params.ReLUOper
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
