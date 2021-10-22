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
from generation.at_types.at_params import NO_ACTIVATION, gen_activation_op, gen_active_at_params
from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.globals.mult8_infos_generator import act_infos
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from graph.types import ActivationFusion, MatrixAddParameters
from quantization.multiplicative.mulbias import set_add_in_scale
from utils.node_id import NodeId

LOG = logging.getLogger("nntool." + __name__)

@paramstype(MatrixAddParameters, ActivationFusion)
@ktype("float")
class MatAddSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
            if isinstance(cnodes[0], MatrixAddParameters):
                qrec = quants[0]
                out_q = quants[1]
            else:
                return False
        else:
            out_q = qrec

        gen.bindings.append(
            CommentBindingList("Node {} in1q {} in2q {} outq {}", cname,
                            qrec.in_qs[0], qrec.in_qs[1], out_q.out_qs[0])
        )
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if isinstance(node, ActivationFusion):
            cnodes = node.contained_nodes()
            if isinstance(cnodes[0], MatrixAddParameters):
                gen.kernels.append(MatAddKernel(node.name, cname, cnodes[0], cnodes[1], force_relu=gen.force_relu))
                return True
            return False
        gen.kernels.append(MatAddKernel(node.name, cname, node, None, force_relu=gen.force_relu))
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
CNN_MatAddAct_fp16("{cname}", {gen_ctrl}, {feat}, {feat}, {width}, {height}, KOP_MATADD, {act_oper});
"""

    def __init__(self, node_name, cname, matadd_params, act_params=None, force_relu=True, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if act_params is not None:
            at_act_params = gen_activation_op(act_params.activation, force_relu=force_relu)
        else:
            at_act_params = "KOP_NONE"

        dimensions = make_three_dims(matadd_params.in_dims[0])

        attrs = {
            'feat': dimensions[0],
            'width': dimensions[1],
            'height': dimensions[2],
            'act_oper': at_act_params
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
