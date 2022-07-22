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
from nntool.generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from nntool.generation.generators.general.autotiler_kernel import NewAutoTilerKernel
from nntool.generation.generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from nntool.generation.generators.helpers.fusions import gen_activation_attrs
from nntool.graph.types import MatScaleFusionNode

LOG = logging.getLogger(__name__)


@paramstype(MatScaleFusionNode)
@ktype("symmetric")
class MatScalePOW2Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if node.fusion_type == "vec_scalar":
            gen.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} inq3 {} outq {}", node.name,
                                qrec.in_qs[0].q, qrec.in_qs[1].q,
                                qrec.in_qs[2].q, qrec.out_qs[0].q)
            )
            gen.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                                GNodeArgEdge(in_eparams[1]), GNodeArgEdge(
                                    in_eparams[2]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        else:
            gen.bindings.append(
                CommentBindingList("Node {} inq1 {} inq2 {} outq {}", node.name,
                                qrec.in_qs[0].q, qrec.in_qs[1].q, qrec.out_qs[0].q)
            )
            gen.bindings.append(
                NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]),
                                GNodeArgEdge(out_eparams[0], "GNA_OUT")))
        return True


    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.kernels.append(MatrixScaleKernel(cname, node, qrec, gen_ctrl=node.get_gen_ctrl()))
        return True

class MatrixScaleKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_MatScale("{cname}", {gen_ctrl},
             {at_bits(other_q)}, {at_bits(vector_q)}, {at_bits(scalar_q)}, {at_bits(out_q)},
             {other_q.q}, {vector_q.q}, {scalar_q.q}, {out_q.q},
             1, 1, 1, 1, {in_dim.shape[0]}, {out_dim.shape[0]}, {in_dim.shape[2]}, {in_dim.shape[1]},
             {kop_matscale}, {kop_act});
"""


    def __init__(self, cname, params, matscale_qrec, gen_ctrl=None, force_relu=False):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dim = params.in_dims[0]
        out_dim = params.out_dims[0]
        assert in_dim.shape[0] == out_dim.shape[0]
        if params.fusion_type == "vec_scalar":
            attrs = {
                'otherq': matscale_qrec.in_qs[0],
                'vectorq': matscale_qrec.in_qs[1],
                'scalarq': matscale_qrec.in_qs[2],
                'kop_matscale': 'KOP_MATSCALE_VECTOR_SCALAR'
            }
        elif params.fusion_type == "vector":
            attrs = {
                'otherq': matscale_qrec.in_qs[1],
                'vectorq': matscale_qrec.in_qs[2],
                'scalarq': None,
                'kop_matscale': 'KOP_MATSCALE_VECTOR'
            }
        elif params.fusion_type == "scalar":
            attrs = {
                'otherq': matscale_qrec.in_qs[0],
                'vectorq': None,
                'scalarq': matscale_qrec.in_qs[1],
                'kop_matscale': 'KOP_MATSCALE_SCALAR'
            }
        else:
            raise NotImplementedError(
                "unknown fusion type %s" % params.fusion_type)

        attrs.update({
            "in_dim": in_dim,
            "out_dim": out_dim,
        })

        attrs.update(gen_activation_attrs(
            params,
            None,
            None))

        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
