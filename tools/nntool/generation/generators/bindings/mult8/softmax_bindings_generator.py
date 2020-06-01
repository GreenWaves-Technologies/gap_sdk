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
import numpy as np

from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import SoftMaxParameters


@generation_function("bindings", (SoftMaxParameters,), qrec_types=(QREC_MULT8,))
def softmax_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
    set_softmax_bindings(gen, in_eparams, out_eparams, cname, node, qrec)
    return True


def set_softmax_bindings(gen, in_eparams, out_eparams, cname, params, node_q):
    in_q = -np.ceil(np.log2(node_q.in_qs[0].scale))
    out_q = -np.ceil(np.log2(node_q.out_qs[0].scale))
    gen.bindings.append(
        CommentBindingList("Node {} inq {} outq {}",
                           params.name, int(in_q[0]), int(out_q[0]))
    )
    gen.bindings.append(
        NodeBindingList(cname, GNodeArgEdge(in_eparams[0]),
                        GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                        GNodeArgNode(params, 'infos')
                        ))
