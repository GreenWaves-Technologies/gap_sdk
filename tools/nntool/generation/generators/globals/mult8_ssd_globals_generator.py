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

import os

import numpy as np

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import SSDDetectorParameters
from quantization.qtype import QType
from quantization.multiplicative.mult_utils import compute_mul_bias

from .mult8_infos_generator import gen_constant
from .global_names import *

@generation_function("globals", (SSDDetectorParameters, ), qrec_types=(QREC_MULT8,))
def mult8_ssd_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    del fnode, pnode
    gen_ssd_globals(gen, node, qrec)
    return True

def gen_ssd_globals(gen, node, qrec):
    qrec.set_scales(node)
    scores_q = qrec.in_qs[1]
    scores_scale, scores_norm = compute_mul_bias(scores_q.scale)

    cname_scales, file_name_scales = gen_constant(gen, node, node, SSD_SCALES)
    contents = np.array([qrec.scale_x_q.qbiases,
                         qrec.scale_x_anc_q.qbiases,
                         qrec.scale_y_q.qbiases,
                         qrec.scale_y_anc_q.qbiases,
                         qrec.scale_h_q.qbiases,
                         qrec.scale_w_q.qbiases,
                         qrec.scale_ao_q.qbiases,
                         scores_scale], dtype=np.int8)
    scale_info = ConstantInfo(file_name_scales, QType(bits=8, q=0, signed=True), contents=contents)

    cname_norms, file_name_norms = gen_constant(gen, node, node, SSD_NORMS)
    contents = np.array([qrec.scale_x_q.qnorms,
                         qrec.scale_x_anc_q.qnorms,
                         qrec.scale_y_q.qnorms,
                         qrec.scale_y_anc_q.qnorms,
                         qrec.scale_h_q.qnorms,
                         qrec.scale_w_q.qnorms,
                         qrec.scale_ao_q.qnorms,
                         scores_norm], dtype=np.int8)
    norms_info = ConstantInfo(file_name_norms, QType(bits=8, q=0, signed=True), contents=contents)

    score_threshold = scores_q.quantize(node.nms_score_threshold)
    cname_infos, file_name_infos = gen_constant(gen, node, node, INFOS)
    contents = np.array([round(node.nms_iou_threshold * 2**7),     # Q7
                         score_threshold,                          # Q0 [0:255]
                         node.max_detections,                      # Q0 [0:255]
                         node.max_classes_per_detection,           # Q0 [0:255]
                         node.max_bb_before_nms >> 8,
                         node.max_bb_before_nms], dtype=np.int8)   # max_bb = Infos[4]<<8 + Infos[5]
    ssd_infos = ConstantInfo(file_name_infos, QType(bits=8, q=0, signed=True), contents=contents)

    gen.globals.append(GlobalArgInfo(qrec.scale_x_q.ctype, cname_scales,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=scale_info))

    gen.globals.append(GlobalArgInfo(qrec.scale_x_q.shift_ctype, cname_norms,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=norms_info))

    gen.globals.append(GlobalArgInfo('uint8', cname_infos,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=ssd_infos))
