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
from graph.types.ssd import SSDDetectorParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.new_qrec import AllFloatQRec, QRec
from utils.ssd_utils import (CNTX_IDX, CNTY_IDX, H_IDX, W_IDX, XMAX_IDX,
                             XMIN_IDX, YMAX_IDX, YMIN_IDX, convert_cors2cnts,
                             rect_intersect_area, rect_union_area)


@params_type(SSDDetectorParameters)
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
        for i in range(scores.shape[0]):
            for j in range(scores.shape[1]):
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
        return qrec.get_outputs(params, [out_boxes, out_classes, out_scores], ktype="float")

    # @classmethod
    # def decoder(cls, params, qrec, offsets, anchors, scores, anchors_type='centers'):
    #     del qrec
    #     if anchors_type == 'centers':
    #         anchors_cnts = anchors
    #     else:
    #         anchors_cnts = convert_cors2cnts(anchors)

    #     # keep only the offsets with score > threshold
    #     bboxes_indices = np.arange(offsets.shape[0])
    #     valid_indices = bboxes_indices[np.any(
    #         scores > params.nms_score_threshold, axis=1)]
    #     valid_scores = scores[valid_indices]
    #     valid_offsets = offsets[valid_indices]
    #     valid_anchors = anchors_cnts[valid_indices]

    #     # apply offsets
    #     ycenter = (valid_offsets[:, CNTY_IDX]/params.y_scale) * \
    #         valid_anchors[:, H_IDX] + valid_anchors[:, CNTY_IDX]
    #     xcenter = (valid_offsets[:, CNTX_IDX]/params.x_scale) * \
    #         valid_anchors[:, W_IDX] + valid_anchors[:, CNTX_IDX]
    #     half_h = 0.5 * \
    #         np.exp(valid_offsets[:, H_IDX] /
    #                params.h_scale) * valid_anchors[:, H_IDX]
    #     half_w = 0.5 * \
    #         np.exp(valid_offsets[:, W_IDX] /
    #                params.w_scale) * valid_anchors[:, W_IDX]

    #     # min-max or corners format: required for nms
    #     decoded_anchors = np.zeros_like(valid_anchors)
    #     decoded_anchors[:, YMIN_IDX] = ycenter - half_h
    #     decoded_anchors[:, YMAX_IDX] = ycenter + half_h
    #     decoded_anchors[:, XMIN_IDX] = xcenter - half_w
    #     decoded_anchors[:, XMAX_IDX] = xcenter + half_w
    #     return decoded_anchors, valid_scores

    # @classmethod
    # def nms(cls, params, qrec, decoded_bboxes, valid_scores):
    #     del qrec
    #     score_threshold = params.nms_score_threshold
    #     # loop over classes apart the background
    #     num_classes = valid_scores.shape[1]
    #     out_boxes = []
    #     out_scores = []
    #     out_classes = []
    #     for class_idx in range(1, num_classes):
    #         class_scores = valid_scores[:, class_idx]
    #         # default anchors indices before appplying any sort
    #         bboxes_indices = np.arange(decoded_bboxes.shape[0])
    #         # apply confidence threshold
    #         valid_scores_indices = bboxes_indices[class_scores >
    #                                               score_threshold]
    #         class_scores = class_scores[valid_scores_indices]

    #         # sort the confidences freater than the confidence threshold
    #         args = np.argsort(class_scores)[::-1]  # sort the confidences and
    #         # back to the original indices
    #         inds = valid_scores_indices[args]
    #         sorted_bboxes = decoded_bboxes[inds]
    #         sorted_scores = class_scores[args]
    #         liveness = [True] * sorted_bboxes.shape[0]
    #         for i, box_a in enumerate(sorted_bboxes):
    #             if not liveness[i]:
    #                 continue
    #             for j in range(i+1, len(sorted_bboxes)):
    #                 box_b = sorted_bboxes[j]
    #                 intersection = rect_intersect_area(box_a, box_b)
    #                 union = rect_union_area(box_a, box_b)
    #                 if intersection >= (params.nms_iou_threshold * union):
    #                     # iou > threshold ->
    #                     # discard second box which is dominated by the first because they are sorted
    #                     liveness[j] = False
    #         if sum(liveness) > 0:
    #             out_boxes.append(sorted_bboxes[liveness])
    #             out_scores.append(sorted_scores[liveness])
    #             out_classes.append([class_idx] * sum(liveness))
    #     if len(out_boxes) > 0:
    #         out_boxes = np.concatenate(out_boxes)
    #         out_classes = np.concatenate(out_classes)
    #         out_scores = np.concatenate(out_scores)
    #         # keep only the max_detection most probables
    #         args = np.argsort(out_scores)[::-1]
    #         if len(args) < params.max_detections:
    #             return np.pad(out_boxes[args], ((0, params.max_detections-len(args)), (0, 0)), 'constant'), \
    #                 np.pad(out_scores[args], (0, params.max_detections-len(args)), 'constant'), \
    #                 np.pad(out_classes[args], (0, params.max_detections-len(args)), 'constant')
    #         args = args[:params.max_detections]
    #         return out_boxes[args], out_scores[args], out_classes[args]
    #     return np.zeros((params.max_detections, 4)), np.zeros(params.max_detections), np.zeros(params.max_detections)
