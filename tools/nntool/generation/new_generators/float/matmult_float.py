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

from generation.at_types.at_params import gen_activation_op
from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from graph.types import MatMulOpFusionParameters, MatMulOpParameters
from graph.types.activations import (ActivationParameters,
                                     ReluActivationParameters)

LOG = logging.getLogger("nntool." + __name__)


@paramstype(MatMulOpFusionParameters, MatMulOpParameters)
@ktype("float")
class MatMulFloatGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:

        gen.bindings.append(
            CommentBindingList("Node {} inq1 {} inq2 {} outq {} biasesq {}", cname,
                               qrec.in_qs[0], qrec.in_qs[1], qrec.out_qs[0], qrec.in_qs[2])
        )
        gen.bindings.append(
            NodeBindingList(cname,
                            GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[2]),
                            GNodeArgEdge(out_eparams[0], direction="GNA_OUT")
                            ))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        aparams = None
        params = None
        if isinstance(node, MatMulOpFusionParameters):
            for fnode in node.contained_nodes():
                if isinstance(fnode, ActivationParameters):
                    aparams = fnode
                elif isinstance(fnode, MatMulOpParameters):
                    params = fnode
                else:
                    raise ValueError(
                        f'unexpected node type in matmul fusion {node.name} {fnode.op_type}')
        else:
            params = node

        gen.kernels.append(
            MatMulFloatKernel(
                node.name, cname, params, aparams, qrec)
        )
        return True


class MatMulFloatKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatMulAct_fp16("{cname}", {gen_ctrl}, {ColM1}, {LineM1}, {ColM2},
                   {LineM2}, {Width}, {Height}, {Scx}, {Scy}, KOP_MATMUL, {actoper});
"""

    def __init__(self, node_name, cname, params, aparams, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if isinstance(aparams, ReluActivationParameters) and aparams.upper_bound:
            gen_ctrl.ReluN = aparams.upper_bound

        in_dims = params.in_dims

        attrs = {
            'ColM1': in_dims[0][1],
            'LineM1': in_dims[0][0],
            'ColM2': in_dims[1][1],
            'LineM2': in_dims[1][0],
            'Width': 0,
            'Height': 0,
            'Scx': 1,
            'Scy': 1,
            'actoper': gen_activation_op(aparams.activation if aparams else "none")
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
