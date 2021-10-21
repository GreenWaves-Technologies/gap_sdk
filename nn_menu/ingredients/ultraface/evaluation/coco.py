import logging
import warnings

import numpy as np
import json
from pycocotools.coco import COCO
from pycocotools.cocoeval import COCOeval


class COCOEvaluator(object):
    def __init__(self, classes, overlap_thresh=0.5):
        self.classes = classes
        self.overlap_thresh = overlap_thresh
        self.categories = [{"id": id, "name": cls} for id, cls in enumerate(self.classes, start=1)]
        self.reset()

    def reset(self):
        """Reset accumulated statistics."""
        self.images = []
        self.targets = []
        self.detections = []
        self.s_counter = 0

    def add_sample(self, gt_boxes, all_dets):

        img_id = self.s_counter
        self.s_counter += 1

        self.images.append({"file_name": str(img_id), "id": img_id})
        for sample in gt_boxes:
            # ignore __background__
            if sample[-1] == 0:
                continue
            if ((sample > 0) & (sample < 2)).all():
                warnings.warn("GT bounding boxes are normalized. mAP values may be invalid")

            bbox = [sample[0], sample[1], sample[2] - sample[0] + 1, sample[3] - sample[1] + 1]
            #print("target_bbox:", bbox)

            self.targets.append(
                {
                    "bbox": bbox,
                    "image_id": img_id,
                    "id": len(self.targets) + 1,
                    "category_id": sample[-1] + 1,
                    "area": (sample[2] - sample[0] + 1) * (sample[3] - sample[1] + 1),
                    "iscrowd": False,
                }
            )
        for class_idx in range(1, len(self.classes)):
            for det in all_dets[class_idx]:
                if ((det > 0) & (det < 2)).all():
                    warnings.warn("Predicted bounding boxes are normalized. mAP values may be invalid")
                det_bbox = [float(det[0]), float(det[1]), float(det[2] - det[0] + 1), float(det[3] - det[1] + 1)]
                #print("det_bbox:", det_bbox)

                self.detections.append(
                    {
                        "bbox": det_bbox,
                        "image_id": img_id,
                        "id": len(self.detections) + 1,
                        "category_id": class_idx + 1,
                        "area": (det[2] - det[0] + 1) * (det[3] - det[1] + 1),
                        "iscrowd": False,
                        "score": float(det[-1]),
                    }
                )

    def evaluate(self):
        gts = {"images": self.images, "categories": self.categories, "annotations": self.targets}
        targets_file = "targets.json"
        targets_writer = open(targets_file, "w")
        targets_writer = json.dump(gts, targets_writer)
        coco_targets = COCO(targets_file)

        dets = {"images": self.images, "categories": self.categories, "annotations": self.detections}
        detections_file = "dets.json"
        det_writer = open(detections_file, "w")
        det_writer = json.dump(dets, det_writer)
        coco_detections = COCO(detections_file)

        coco_eval = COCOeval(coco_targets, coco_detections, "bbox")
        coco_eval.params.imgIds = range(len(self.images))
        coco_eval.evaluate()
        coco_eval.accumulate()

        def _get_thr_ind(coco_eval, thr):
            ind = np.where((coco_eval.params.iouThrs > thr - 1e-5) & (coco_eval.params.iouThrs < thr + 1e-5))[0][0]
            iou_thr = coco_eval.params.iouThrs[ind]
            assert np.isclose(iou_thr, thr)
            return ind

        ind_lo = _get_thr_ind(coco_eval, self.overlap_thresh)
        ind_hi = _get_thr_ind(coco_eval, self.overlap_thresh)

        aps = []
        for cls_ind, cls in enumerate(self.classes):
            if cls == "__background__":
                continue
            #print("cls_ind")
            #print(cls_ind)
            #print("cls")
            #print(cls)
            precision = coco_eval.eval["precision"][ind_lo : (ind_hi + 1), :, cls_ind, 0, 2]
            #print("precision")
            #print(precision)
            ap = np.mean(precision[precision > -1]) * 100
            #print("ap")
            #print(ap)
            aps.append(ap)
            logging.info("AP %s %f", cls, ap)

        logging.info("mAP %f", np.mean(aps))
        logging.info("~~~~ Summary metrics ~~~~")
        coco_eval.summarize()

        return aps, self.classes[1:]
