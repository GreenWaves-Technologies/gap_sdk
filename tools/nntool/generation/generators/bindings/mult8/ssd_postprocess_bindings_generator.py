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

# from generation.bindings import (CommentBindingList, GNodeArgEdge,
#                                  GNodeArgNode, NodeBindingList)
# from generation.generator_decorators import QREC_MULT8, generation_function
# from generation.generators.globals.global_names import (INFOS, SSD_NORMS,
#                                                         SSD_SCALES)
# from graph.types import SSDDetectorParameters


# @generation_function("bindings", (SSDDetectorParameters, ), qrec_types=(QREC_MULT8, ))
# def ssd_postprocess_bindings_generator(gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
#     set_ssd_postprocess_bindings(
#         gen, in_eparams, out_eparams, cname, node, qrec)
#     return True


# def set_ssd_postprocess_bindings(gen, in_eparams, out_eparams, cname, node, node_q, out_q=None):
#     if out_q is None:
#         out_q = node_q
#     gen.bindings.append(
#         CommentBindingList("Node {} offsetsq {} scoresq {} anchorsq {} outboxesq {}", node.name,
#                            str(node_q.in_qs[0]), str(node_q.in_qs[1]), str(node_q.in_qs[2]), str(out_q.out_qs[0]))
#     )
#     gen.bindings.append(
#         NodeBindingList(cname,
#                         GNodeArgEdge(in_eparams[0]),
#                         GNodeArgEdge(in_eparams[1]),
#                         GNodeArgEdge(in_eparams[2]),
#                         GNodeArgEdge(out_eparams[0], "GNA_OUT"),
#                         GNodeArgEdge(out_eparams[1], "GNA_OUT"),
#                         GNodeArgEdge(out_eparams[2], "GNA_OUT"),
#                         GNodeArgNode(node, SSD_SCALES),
#                         GNodeArgNode(node, SSD_NORMS),
#                         GNodeArgNode(node, INFOS)))
