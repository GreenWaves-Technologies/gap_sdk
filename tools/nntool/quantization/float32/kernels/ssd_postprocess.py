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

from quantization.float32.float32_quantization import Float32QuantizationRecord
from utils.ssd_utils import (convert_cors2cnts, rect_union_area, rect_intersect_area,
                             CNTX_IDX, CNTY_IDX, H_IDX, W_IDX,
                             YMAX_IDX, YMIN_IDX, XMAX_IDX, XMIN_IDX)

def ssd_postprocess(params, in_tensors, qrec, details=None):
    del details
    if qrec is None:
        qrec = Float32QuantizationRecord()
    in_tensors = qrec.prepare_inputs(params, in_tensors, ktype="float32")
    offsets = in_tensors[0]
    scores = in_tensors[1]
    anchors = in_tensors[2]
    decoded_bboxes, valid_scores = decoder(params, qrec, offsets, anchors, scores, anchors_type='centers')
    out_boxes, out_scores, out_classes = nms(params, qrec, decoded_bboxes, valid_scores)
    out_count = np.array([len(out_classes)])
    return qrec.get_outputs(params, [out_boxes, out_classes, out_scores, out_count], ktype="float32")

def decoder(params, qrec, offsets, anchors, scores, anchors_type='centers'):
    del qrec
    if anchors_type == 'centers':
        anchors_cnts = anchors
    else:
        anchors_cnts = convert_cors2cnts(anchors)

    # keep only the offsets with score > threshold
    bboxes_indices = np.arange(offsets.shape[0])
    valid_indices = bboxes_indices[np.any(scores > params.nms_score_threshold, axis=1)]
    valid_scores = scores[valid_indices]
    valid_offsets = offsets[valid_indices]
    valid_anchors = anchors_cnts[valid_indices]

    # apply offsets
    ycenter = (valid_offsets[:, CNTY_IDX]/params.y_scale) * valid_anchors[:, H_IDX] + valid_anchors[:, CNTY_IDX]
    xcenter = (valid_offsets[:, CNTX_IDX]/params.x_scale) * valid_anchors[:, W_IDX] + valid_anchors[:, CNTX_IDX]
    half_h = 0.5 * np.exp(valid_offsets[:, H_IDX]/params.h_scale) * valid_anchors[:, H_IDX]
    half_w = 0.5 * np.exp(valid_offsets[:, W_IDX]/params.w_scale)  * valid_anchors[:, W_IDX]

    # min-max or corners format: required for nms
    decoded_anchors = np.zeros_like(valid_anchors)
    decoded_anchors[:, YMIN_IDX] = ycenter - half_h
    decoded_anchors[:, YMAX_IDX] = ycenter + half_h
    decoded_anchors[:, XMIN_IDX] = xcenter - half_w
    decoded_anchors[:, XMAX_IDX] = xcenter + half_w
    return decoded_anchors, valid_scores

def nms(params, qrec, decoded_bboxes, valid_scores):
    del qrec
    score_threshold = params.nms_score_threshold
    # loop over classes apart the background
    num_classes = valid_scores.shape[1]
    out_boxes = []
    out_scores = []
    out_classes = []
    for class_idx in range(1, num_classes):
        class_scores = valid_scores[:, class_idx]
        # default anchors indices before appplying any sort
        bboxes_indices = np.arange(decoded_bboxes.shape[0])
        # apply confidence threshold
        valid_scores_indices = bboxes_indices[class_scores > score_threshold]
        class_scores = class_scores[valid_scores_indices]

        # sort the confidences freater than the confidence threshold
        args = np.argsort(class_scores)[::-1] # sort the confidences and
        inds = valid_scores_indices[args]     # back to the original indices
        sorted_bboxes = decoded_bboxes[inds]
        sorted_scores = class_scores[args]
        liveness = [True] * sorted_bboxes.shape[0]
        for i, box_a in enumerate(sorted_bboxes):
            if not liveness[i]:
                continue
            for j in range(i+1, len(sorted_bboxes)):
                box_b = sorted_bboxes[j]
                intersection = rect_intersect_area(box_a, box_b)
                union = rect_union_area(box_a, box_b)
                if intersection >= (params.nms_iou_threshold * union):
                    # iou > threshold ->
                    # discard second box which is dominated by the first because they are sorted
                    liveness[j] = False
        if sum(liveness) > 0:
            out_boxes.append(sorted_bboxes[liveness])
            out_scores.append(sorted_scores[liveness])
            out_classes.append([class_idx] * sum(liveness))
    out_boxes = np.concatenate(out_boxes)
    out_classes = np.concatenate(out_classes)
    out_scores = np.concatenate(out_scores)
    # keep only the max_detection most probables
    args = np.argsort(out_scores)[::-1]
    args = args[:params.max_detections]
    return out_boxes[args], out_scores[args], out_classes[args]
