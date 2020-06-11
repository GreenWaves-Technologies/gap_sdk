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


from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from generation.generators.generator_decorators import generation_function, QREC_POW2
from graph.types import MatScaleFusionParameters


@generation_function("bindings", (MatScaleFusionParameters,), qrec_types=(QREC_POW2, ))
def matscale_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    set_matscale_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
    return True


def set_matscale_bindings(gen, in_eparams, out_eparams, cname, params, node_q):
    if params.fusion_type == "vec_scalar":
        gen.bindings.append(
            CommentBindingList("Node {} inq1 {} inq2 {} inq3 {} outq {}", params.name,
                               node_q.in_qs[0].q, node_q.in_qs[1].q,
                               node_q.in_qs[2].q, node_q.out_qs[0].q)
        )
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[1]), GNodeArgEdge(in_eparams[2]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))
    else:
        gen.bindings.append(
            CommentBindingList("Node {} inq1 {} inq2 {} outq {}", params.name,
                               node_q.in_qs[0].q, node_q.in_qs[1].q, node_q.out_qs[0].q)
        )
        gen.bindings.append(
            NodeBindingList(cname, GNodeArgEdge(in_eparams[0]), GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT")))
