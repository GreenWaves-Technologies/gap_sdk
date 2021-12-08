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

from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 NodeBindingList)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from graph.types import SSDDetectorParameters

LOG = logging.getLogger("nntool." + __name__)


@paramstype(SSDDetectorParameters)
@ktype("float")
class SSDDetectorFloatGenerator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList("Node {}", node.name)
        )
        gen.bindings.append(
            NodeBindingList(cname,
                            GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(in_eparams[2]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            GNodeArgEdge(out_eparams[1], "GNA_OUT"),
                            GNodeArgEdge(out_eparams[2], "GNA_OUT"))
        )
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        gen.kernels.append(SSDPostProcessKernel(node.name, cname, node))
        return True


class SSDPostProcessKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_SSD_PostProcess_fp16("{cname}", {gen_ctrl}, {n_anchors}, {n_classes}, {n_outboxes}, {max_bb_before_nms}, {DecScoreThr:.3f}, {NMSThr:.3f});
"""

    def __init__(self, node_name, cname, params, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        in_dims = params.in_dims
        out_dims = params.out_dims

        attrs = {
            'n_anchors': in_dims[0].shape[0],
            'n_classes': in_dims[1][1],
            'n_outboxes': out_dims[0][0],
            'max_bb_before_nms': params.max_bb_before_nms,
            'DecScoreThr': params.nms_score_threshold,
            'NMSThr': params.nms_iou_threshold
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
