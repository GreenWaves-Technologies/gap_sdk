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
from generation.code_block import CodeBlock
from generation.generators.generator_decorators import generation_function, QREC_MULT8
from graph.types import SSDDetectorParameters

from ..autotiler_kernel import AutotilerKernel

LOG = logging.getLogger("nntool." + __name__)

GEN_SSD = "CNN_SSD_PostProcess_SQ8"

def gen_at_ssd_parameter(code_block, name, num_anchors, num_classes, out_boxes, max_bb_before_nms,
                         gen_ctrl=None, at_ver=3):
    if gen_ctrl is None:
        gen_ctrl = "0"
    else:
        raise NotImplementedError("genctrl is not yet implemented")

    code_block.write('{}("{}", {}, {}, {}, {}, {});',
                     GEN_SSD, name, gen_ctrl, num_anchors, num_classes, out_boxes, max_bb_before_nms)


@generation_function("kernels", (SSDDetectorParameters, ), qrec_types=(QREC_MULT8, ))
def ssdpostprocess_kernels_generator(gen, node, qrec, in_eparams, out_eparams, cname):
    del in_eparams, out_eparams
    gen.kernels.append(SSDPostProcessKernel(cname, node, qrec, at_ver=gen.opts['at_ver']))
    return True


class SSDPostProcessKernel(AutotilerKernel):
    def __init__(self, cname, params, qrec, gen_ctrl=None, at_ver=3):
        del qrec
        if gen_ctrl is None:
            self.gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname
            self.gen_ctrl = gen_ctrl

        self.num_anchors = params.in_dims[0].shape[0] # num_boxes x 4
        self.num_classes = params.in_dims[1].shape[1] # num_boxes x num_classes
        self.out_boxes = params.out_dims[0].shape[0] # out_boxes x 4
        self.max_bb_before_nms = params.max_bb_before_nms
        self.cname = cname
        self.node_name = params.name
        self.at_ver = at_ver

    def code(self, code_block=None):
        if code_block is None:
            code_block = CodeBlock()

        code_block.comment("generator for {}", self.node_name)

        if not self.gen_ctrl.is_unmodified:
            self.gen_ctrl.gen_ctrl_decl(code_block)

        gen_at_ssd_parameter(code_block, self.cname, self.num_anchors,
                             self.num_classes, self.out_boxes,
                             self.max_bb_before_nms, at_ver=self.at_ver)
        return code_block
