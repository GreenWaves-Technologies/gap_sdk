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
from nntool.graph.types import SSDDetectorNode
from nntool.graph.types.ssd import NMSNode
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.multiplicative.mulbias import set_ssd_scales
from nntool.quantization.new_qrec import QRec
from nntool.utils.at_norm import at_norm
from nntool.utils.exp_17_15 import exp_fp_17_15
from nntool.utils.ssd_utils import (CNTX_IDX, CNTY_IDX, H_IDX, W_IDX, XMAX_IDX,
                             XMIN_IDX, YMAX_IDX, YMIN_IDX, convert_cnts2cors,
                             convert_cors2cnts, rect_intersect_area,
                             rect_union_area)


@params_type(SSDDetectorNode)
@qrec_type('scaled')
class SSDDetectorSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="symmetric")
        offsets = in_tensors[0]
        scores = in_tensors[1]
        anchors = in_tensors[2]
        # decoded_bboxes: Q14
        # valid_scores: Q7
        anchors_type = "centers"
        if anchors_type == 'centers':
            anchors_cnts = anchors
        else:
            anchors_cnts = convert_cors2cnts(anchors)
        set_ssd_scales(qrec, params)
        scores_q = qrec.in_qs[1]
        score_threshold = scores_q.quantize(params.nms_score_threshold)
        decoded_bboxes = []
        for j in range(1, scores.shape[1]):
            for i in range(scores.shape[0]):
                if len(decoded_bboxes) > params.max_bb_before_nms:
                    break
                if scores[i, j] <= score_threshold:
                    continue
                offset = offsets[i]
                anchor = anchors[i]
                #  xcnt, ycnt --> Q14
                #  xcnt = (So*O * Sa*Aw)/params.x_scale + Sa*Ax = So*Sa/params.x_scale (O*Aw + x_scale/So * Ax) =
                #           (scale_x * (O*Aw + (scale_x_anc*Ax)>>scale_x_ancNorm))>>scale_xNorm =
                #           at_norm(scale_x*(O*Aw + at_norm(scale_x_anc*Ax, scale_x_ancNorm)), scale_xNorm)
                xcenter = qrec.cache['scale_x_q'].apply_scales(
                    np.multiply(offset[CNTX_IDX], anchor[W_IDX], dtype=np.int32) +
                    qrec.cache['scale_x_anc_q'].apply_scales(anchor[CNTX_IDX])
                )
                ycenter = qrec.cache['scale_y_q'].apply_scales(
                    np.multiply(offset[CNTY_IDX], anchor[H_IDX], dtype=np.int32) +
                    qrec.cache['scale_y_anc_q'].apply_scales(anchor[CNTY_IDX])
                )

                #  half_h, half_w --> Q14
                #  half_h = exp(So*Off / params.h_scale) * Sa*A = Sa/So * exp(So/params.h_scale *O) * A =
                #           (scale_ao * (A* exp17.15(scale_h*O<<15-scale_hNorm))>>scale_aoNorm) =
                #           at_norm(scale_ao*(A*exp17.15(scale_h*O<<15-scale_hNorm)), scale_aoNorm)
                norm_h = 15 - qrec.cache['scale_h_q'].qnorms
                norm_w = 15 - qrec.cache['scale_w_q'].qnorms
                exp_h = exp_fp_17_15(np.multiply(offset[H_IDX], int(
                    qrec.cache['scale_h_q'].qbiases), dtype=np.int32) << norm_h)
                exp_w = exp_fp_17_15(np.multiply(offset[W_IDX], int(
                    qrec.cache['scale_w_q'].qbiases), dtype=np.int32) << norm_w)
                half_h = qrec.cache['scale_ao_q'].apply_scales(np.multiply(
                    exp_h, anchor[H_IDX], dtype=np.int32)) >> 1
                half_w = qrec.cache['scale_ao_q'].apply_scales(np.multiply(
                    exp_w, anchor[W_IDX], dtype=np.int32)) >> 1

                decoded_bboxes.append({
                                       "bbox": [ycenter-half_h, xcenter-half_w, ycenter+half_h, xcenter+half_w],
                                       "score": scores[i, j],
                                       "class": j,
                                       "alive": True
                                      })

        # Bubble sort to sort the scores
        changed = True
        while changed:
            changed = False
            for i in range(len(decoded_bboxes)-1):
                if decoded_bboxes[i]['score'] < decoded_bboxes[i+1]['score']:
                    temp = decoded_bboxes[i]
                    decoded_bboxes[i] = decoded_bboxes[i+1]
                    decoded_bboxes[i+1] = temp
                    changed = True

        # NMS
        for idx in range(len(decoded_bboxes)):
            for idx_int in range(idx+1, len(decoded_bboxes)):
                if (not decoded_bboxes[idx_int]['alive']) or (decoded_bboxes[idx]['class'] != decoded_bboxes[idx_int]['class']):
                    continue
                intersection = rect_intersect_area(decoded_bboxes[idx]['bbox'], decoded_bboxes[idx_int]['bbox'])
                union = rect_union_area(decoded_bboxes[idx]['bbox'], decoded_bboxes[idx_int]['bbox'])
                if intersection >= at_norm(scores_q.quantize(params.nms_iou_threshold) * union, 7):
                    decoded_bboxes[idx_int]['alive'] = False

        out_boxes = np.zeros((params.max_detections, 4), dtype=qrec.out_qs[0].dtype)
        out_classes = np.zeros(params.max_detections, dtype=qrec.out_qs[1].dtype)
        out_scores = np.zeros(params.max_detections, dtype=qrec.out_qs[2].dtype)
        out_idx = 0
        for i in range(len(decoded_bboxes)):
            if out_idx >= params.max_detections:
                break
            bbox = decoded_bboxes[i]
            if bbox['alive']:
                out_boxes[out_idx] = bbox['bbox']
                out_classes[out_idx] = bbox['class']
                out_scores[out_idx] = bbox['score']
                out_idx += 1
        # decoded_bboxes, valid_scores = cls.decoder(
        #     params, qrec, offsets, anchors, scores, anchors_type='centers')
        # out_boxes, out_scores, out_classes = cls.nms(params, qrec, decoded_bboxes, valid_scores)
        # out_count = np.array([sum(out_classes != 0)])
        outputs = [out_boxes, out_classes, out_scores]
        if params.output_detection_count:
            outputs.append(np.array([out_idx], dtype=np.int32))        
        return qrec.get_outputs(params, outputs, ktype="symmetric")

@params_type(NMSNode)
@qrec_type('scaled')
class NMSKernelSymmetric(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        boxes = in_tensors[0][0] if not params.center_point_box else convert_cnts2cors(in_tensors[0][0])
        scores = in_tensors[1][0]
        n_boxes = len(scores[0])
        n_classes = len(scores)
        scores_q = qrec.in_qs[1]

        indexes = np.zeros((params.max_output_boxes_per_class*n_classes, 3))
        idxs_count = 0
        for class_id in range(n_classes):
            bbox_buff = []
            counter = 0
            for box_id in range(n_boxes):
                class_score = scores[class_id, box_id]
                if class_score > scores_q.quanize(params.nms_score_threshold):
                    bbox_buff.append({
                        "index": box_id,
                        "score": class_score,
                        "box": boxes[box_id],
                        "alive": True
                    })
                    counter += 1

            # Bubble sort to sort the scores
            changed = True
            while changed:
                changed = False
                for i in range(counter-1):
                    if bbox_buff[i]["score"] < bbox_buff[i+1]["score"]:
                        temp = bbox_buff[i]
                        bbox_buff[i] = bbox_buff[i+1]
                        bbox_buff[i+1] = temp
                        changed = True

            # NMS
            for idx in range(counter):
                for idx_int in range(idx+1, counter):
                    if not bbox_buff[idx_int]["alive"]:
                        continue
                    intersection = rect_intersect_area(bbox_buff[idx]["box"], bbox_buff[idx_int]["box"])
                    union = rect_union_area(bbox_buff[idx]["box"], bbox_buff[idx_int]["box"])
                    if intersection >= (scores_q.quanize(params.nms_iou_threshold) * union):
                        bbox_buff[idx_int]["alive"] = False

            class_idxs_count_start = idxs_count
            for bb in bbox_buff:
                if (idxs_count-class_idxs_count_start) > params.max_output_boxes_per_class:
                    break
                if bb["alive"]:
                    indexes[idxs_count] = np.array([0, class_id, bb['index']])
                    idxs_count += 1

        return qrec.get_outputs(params, [np.array(indexes)], ktype="float")
