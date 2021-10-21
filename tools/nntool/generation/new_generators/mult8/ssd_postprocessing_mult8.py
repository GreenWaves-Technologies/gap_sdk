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

from graph.types.input_output import ConstantInputParameters
from generation.helpers.gen_constant import gen_constant
from quantization.qtype import QType
import logging
import numpy as np

from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge, GNodeArgNode,
                                 NodeBindingList)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import (GeneratorBase, ktype,
                                                      paramstype)
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from graph.types import SSDDetectorParameters
from quantization.multiplicative.mulbias import set_ssd_scales
from quantization.multiplicative.mult_utils import compute_mul_bias

LOG = logging.getLogger("nntool." + __name__)


@paramstype(SSDDetectorParameters)
@ktype("scaled")
class SSDDetectorSQ8Generator(GeneratorBase):

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        set_ssd_scales(qrec, node)
        scores_q = qrec.in_qs[1]
        scores_scale, scores_norm = compute_mul_bias(scores_q.scale)

        cname_scales, file_name_scales = gen_constant(gen, node, node, "ssd_scales")
        contents = np.array([qrec.cache['scale_x_q'].qbiases,
                            qrec.cache['scale_x_anc_q'].qbiases,
                            qrec.cache['scale_y_q'].qbiases,
                            qrec.cache['scale_y_anc_q'].qbiases,
                            qrec.cache['scale_h_q'].qbiases,
                            qrec.cache['scale_w_q'].qbiases,
                            qrec.cache['scale_ao_q'].qbiases,
                            scores_scale], dtype=np.int8)
        scale_info = ConstantInfo(file_name_scales, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)

        cname_norms, file_name_norms = gen_constant(gen, node, node, "ssd_norms")
        contents = np.array([qrec.cache['scale_x_q'].qnorms,
                            qrec.cache['scale_x_anc_q'].qnorms,
                            qrec.cache['scale_y_q'].qnorms,
                            qrec.cache['scale_y_anc_q'].qnorms,
                            qrec.cache['scale_h_q'].qnorms,
                            qrec.cache['scale_w_q'].qnorms,
                            qrec.cache['scale_ao_q'].qnorms,
                            scores_norm], dtype=np.int8)
        norms_info = ConstantInfo(file_name_norms, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)


        gen.globals.append(GlobalArgInfo(qrec.cache['scale_x_q'].ctype, cname_scales,
                                        gen.opts['default_global_home_location'],
                                        gen.opts['default_global_exec_location'],
                                        const_info=scale_info))

        gen.globals.append(GlobalArgInfo(qrec.cache['scale_x_q'].shift_ctype, cname_norms,
                                        gen.opts['default_global_home_location'],
                                        gen.opts['default_global_exec_location'],
                                        const_info=norms_info))
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList("Node {} offsetsq {} scoresq {} anchorsq {} outboxesq {}", node.name,
                            str(qrec.in_qs[0]), str(qrec.in_qs[1]), str(qrec.in_qs[2]), str(qrec.out_qs[0]))
        )
        gen.bindings.append(
            NodeBindingList(cname,
                            GNodeArgEdge(in_eparams[0]),
                            GNodeArgEdge(in_eparams[1]),
                            GNodeArgEdge(in_eparams[2]),
                            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
                            GNodeArgEdge(out_eparams[1], "GNA_OUT"),
                            GNodeArgEdge(out_eparams[2], "GNA_OUT"),
                            GNodeArgNode(node, "ssd_scales"),
                            GNodeArgNode(node, "ssd_norms"),)
        )
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        gen.kernels.append(SSDPostProcessKernelSQ8(node.name, cname, node))
        return True


class SSDPostProcessKernelSQ8(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_SSD_PostProcess_SQ8("{cname}", {gen_ctrl}, {n_anchors}, {n_classes}, {n_outboxes}, {max_bb_before_nms}, {DecScoreThr:.3f}, {NMSThr:.3f});
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
