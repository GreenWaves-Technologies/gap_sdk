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

import collections
import numpy as np
from nntool.graph.types.ssd import NMSNode, SSDDetectorNode
from nntool.execution.kernels.kernel_base import KernelBase, params_type, qrec_type
from nntool.quantization.new_qrec import AllFloatQRec, QRec
from nntool.utils.ssd_utils import (CNTX_IDX, CNTY_IDX, H_IDX, W_IDX, XMAX_IDX,
                             XMIN_IDX, YMAX_IDX, YMIN_IDX, convert_cnts2cors, convert_cors2cnts,
                             rect_intersect_area, rect_union_area)


@params_type(SSDDetectorNode)
@qrec_type('float')
class SSDDetectorFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        if qrec is None:
            qrec = AllFloatQRec()
        in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float")
        offsets = in_tensors[0]
        scores = in_tensors[1]
        anchors = in_tensors[2]

        anchors_type = "centers"
        if anchors_type == 'centers':
            anchors_cnts = anchors
        else:
            anchors_cnts = convert_cors2cnts(anchors)
        score_threshold = params.nms_score_threshold
        decoded_bboxes = []
        for j in range(1, scores.shape[1]):
            for i in range(scores.shape[0]):
                if len(decoded_bboxes) > params.max_bb_before_nms:
                    break
                if scores[i, j] <= score_threshold:
                    continue
                offset = offsets[i]
                anchor = anchors[i]
                xcenter = (offset[CNTX_IDX]/params.x_scale) * anchor[W_IDX] + anchor[CNTX_IDX]
                ycenter = (offset[CNTY_IDX]/params.y_scale) * anchor[H_IDX] + anchor[CNTY_IDX]

                half_h = 0.5 * np.exp(offset[H_IDX]/params.h_scale) * anchor[H_IDX]
                half_w = 0.5 * np.exp(offset[W_IDX]/params.w_scale) * anchor[W_IDX]

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
                if intersection >= (params.nms_iou_threshold * union):
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
        # out_boxes, out_scores, out_classes = cls.nms(
        #     params, qrec, decoded_bboxes, valid_scores)
        # out_count = np.array([sum(out_classes != 0)])
        outputs = [out_boxes, out_classes, out_scores]
        if params.output_detection_count:
            outputs.append(np.array([out_idx]))
        return qrec.get_outputs(params, outputs, ktype="float")

@params_type(NMSNode)
@qrec_type('float')
class NMSKernelFloat32(KernelBase):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        if qrec is None:
            qrec = AllFloatQRec()
        boxes = in_tensors[0][0] if not params.center_point_box else convert_cnts2cors(in_tensors[0][0])
        scores = in_tensors[1][0]
        n_boxes = len(scores[0])
        n_classes = len(scores)

        indexes = np.zeros((params.max_output_boxes_per_class*n_classes, 3))
        idxs_count = 0
        for class_id in range(n_classes):
            bbox_buff = []
            counter = 0
            for box_id in range(n_boxes):
                class_score = scores[class_id, box_id]
                if class_score > params.nms_score_threshold:
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
                    if intersection >= (params.nms_iou_threshold * union):
                        bbox_buff[idx_int]["alive"] = False

            class_idxs_count_start = idxs_count
            for bb in bbox_buff:
                if (idxs_count-class_idxs_count_start) > params.max_output_boxes_per_class:
                    break
                if bb["alive"]:
                    indexes[idxs_count] = np.array([0, class_id, bb['index']])
                    idxs_count += 1

        return qrec.get_outputs(params, [indexes], ktype="float")
