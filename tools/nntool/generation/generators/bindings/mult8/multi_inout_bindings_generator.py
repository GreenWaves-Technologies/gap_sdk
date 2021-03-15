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
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import ExpressionFusionParameters


@generation_function("bindings", (ExpressionFusionParameters, ), qrec_types=(QREC_MULT8, ))
def multi_in_out_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    set_multi_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec)


def set_multi_in_out_bindings(gen, in_eparams, out_eparams, cname, node, qrec):
    gen.bindings.append(
        CommentBindingList("Node {} in_qs [{}] out_qs [{}]", node.name,
                           ",".join(str(in_q) for in_q in qrec.in_qs),
                           ",".join(str(out_q) for out_q in qrec.out_qs))
    )

    # the input order of the generated function can vary since it passes through a set
    params = [GNodeArgEdge(in_eparams[idx]) for idx in node.input_shuffle] + \
        [GNodeArgEdge(out_eparams[idx], "GNA_OUT") for idx in node.output_shuffle]
    gen.bindings.append(
        NodeBindingList(cname, *params)
    )
