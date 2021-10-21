import os
import cv2
import matplotlib.pyplot as plt
import numpy as np
import albumentations

import torch.utils.data as data

from collections import defaultdict
from os import path as osp
from tqdm import tqdm
from coco import COCOEvaluator
from box_utils import predict


def calculate_overlap(BBGT, bb):
    ixmin = np.maximum(BBGT[0], bb[0])
    iymin = np.maximum(BBGT[1], bb[1])
    ixmax = np.minimum(BBGT[2], bb[2])
    iymax = np.minimum(BBGT[3], bb[3])

    iw = np.maximum(ixmax - ixmin + 1.0, 0.0)
    ih = np.maximum(iymax - iymin + 1.0, 0.0)

    inters = iw * ih

    uni = (bb[2] - bb[0] + 1.0) * (bb[3] - bb[1] + 1.0) + (BBGT[2] - BBGT[0] + 1.0) * (BBGT[3] - BBGT[1] + 1.0) - inters

    overlap = inters / uni
    return overlap

def calculate_accuracy(gt_bboxes, detections, overlap_thresh, conf_thresh):
    TP = 0
    FN = 0
    FP = 0
    gt_bboxes = gt_bboxes.tolist()
    detections = detections.tolist()
    # print(f'GT list: {gt_bboxes}')
    # print(f'BB list: {detections}')
    for gt_bbox in gt_bboxes:
        is_detected = False
        for bbox in detections:
            if calculate_overlap(gt_bbox, bbox) > 0.5 and bbox[4] > conf_thresh:
                TP += 1
                detections.remove(bbox)
                is_detected = True
                # print(f'CUT: {detections}')
                break
        FN += not is_detected
    # print(f'AFTER: {detections}, len: {len(detections)}')
    FP += len(detections)
    # print(f'TP: {TP}, FP: {FP}, FN: {FN}')
    accuracy = TP / (TP + FP + FN)
    # print(f'Accuracy: {accuracy}')
    return accuracy


def get_matches(gt_boxes, det_boxes, overlap_thr=0.5):
    matches = []
    for i in range(len(gt_boxes)):
        for j in range(len(det_boxes)):
            if len(gt_boxes) > 0:

                iou = calculate_overlap(gt_boxes[i], det_boxes[j])
                # print("iou: ", iou)

                if iou > overlap_thr:
                    matches.append([i, j, iou])

    matches = np.array(matches)
    false_positives = 0
    false_negatives = 0
    if matches.shape[0] > 0:
        # sort list of matches by descending IOU so we can remove duplicate detections
        # while keeping the highest IOU
        matches = matches[matches[:, 2].argsort()[::-1][: len(matches)]]

        # remove duplicate detections
        matches = matches[np.unique(matches[:, 1], return_index=True)[1]]

        # sort the list again by descending IOU and remove duplicates that don't preserve
        # our previous sort
        matches = matches[matches[:, 2].argsort()[::-1][: len(matches)]]

        # remove duplicate ground truths from the list
        matches = matches[np.unique(matches[:, 0], return_index=True)[1]]

    for i in range(len(gt_boxes)):
        if matches.shape[0] > 0 and matches[matches[:, 0] == i].shape[0] == 1:
            continue
        else:
            false_negatives += 1

    for i in range(len(det_boxes)):
        if matches.shape[0] > 0 and matches[matches[:, 1] == i].shape[0] == 0:
            false_positives += 1

    return matches, false_positives, false_negatives


def evaluate_onnx_detector(
    detector, test_dataset, evaluator, num_workers 
):
    batch_iterator = iter(
        data.DataLoader(test_dataset, shuffle=False, num_workers=num_workers, drop_last=False, pin_memory=True)
    )

    all_boxes = {}
    all_scores = {}
    for index, (images, _) in enumerate(tqdm(batch_iterator, total=len(batch_iterator))):
        image = images.cpu().detach().numpy()
        input_name = detector.get_inputs()[0].name
        scores, boxes = detector.run(None, {input_name: image})

        boxes, _, scores = predict(images.shape[3], images.shape[2], scores, boxes, 0.7)
        boxes = np.expand_dims(boxes, 0)
        scores = np.expand_dims(scores, 0)

        np.set_printoptions(threshold=np.inf)
        image_id = test_dataset.get_image_id(index)

        if len(boxes.shape) <= 1 or boxes.shape[1] < 1:
            boxes = np.zeros((1, 1, 4), dtype=np.float32)
            scores = np.zeros((1, 1), dtype=np.float32)

        image_id = test_dataset.get_image_id(index)
        image_id = osp.basename(image_id)
        image_id = image_id[ : image_id.find(".")]
        all_boxes[image_id] = boxes
        all_scores[image_id] = scores

    aps, classes, targets_detections = evaluate_detection_results(
        all_boxes, all_scores, test_dataset, num_workers, evaluator
    )
    return aps, classes, targets_detections


def evaluate_gap_detector(
    detections, test_dataset, evaluator, num_workers
):
    batch_iterator = iter(
        data.DataLoader(test_dataset, shuffle=False, num_workers=num_workers, drop_last=False, pin_memory=True)
    )

    all_boxes = {}
    all_scores = {}
    for index, (images, _) in enumerate(tqdm(batch_iterator, total=len(batch_iterator))):
        boxes = np.expand_dims(detections[index][:, :4], 0)
        scores = np.expand_dims(detections[index][:, 4:], 0)
        boxes, _, scores = predict(images.shape[3], images.shape[2], scores, boxes, 0.7)
        boxes = np.expand_dims(boxes, 0)
        scores = np.expand_dims(scores, 0)
        if len(boxes.shape) <= 1 or boxes.shape[1] < 1:
            boxes = np.zeros((1, 1, 4), dtype=np.float32)
            scores = np.zeros((1, 1), dtype=np.float32)

        image_id = test_dataset.get_image_id(index)
        image_id = osp.basename(image_id)
        image_id = image_id[ : image_id.find(".")]
        all_boxes[image_id] = boxes
        all_scores[image_id] = scores

    aps, classes, targets_detections = evaluate_detection_results(
        all_boxes, all_scores, test_dataset, num_workers, evaluator
    )
    return aps, classes, targets_detections


def evaluate_detection_results(
    boxes,
    scores,
    test_dataset,
    num_workers,
    evaluator,
    skip_missed=True,
):
    batch_iterator = iter(data.DataLoader(test_dataset, shuffle=False, num_workers=num_workers, drop_last=False))

    targets_detections = []
    for index, (images, targets) in enumerate(tqdm(batch_iterator, total=len(test_dataset))):
        detections_by_class = defaultdict(list)
        image_id = test_dataset.get_image_id(index)
        image_id = osp.basename(image_id)
        image_id = image_id[ : image_id.find(".")]
        if image_id not in boxes:
            if skip_missed:
                continue
            else:
                boxes[image_id] = [[[]] for _ in test_dataset.classes]
                scores[image_id] = [[[]] for _ in test_dataset.classes]
        for class_id, class_name in enumerate(test_dataset.classes):
            detections_by_class[class_id] = []
            if class_name == "__background__":
                continue
            try:
                for box, score in zip(boxes[image_id][0], scores[image_id][0]):
                    box_with_score = np.hstack([box, score])
                    detections_by_class[class_id].append(box_with_score)

                detections_by_class[class_id] = np.array([np.array(arr) for arr in detections_by_class[class_id]])
            except:
                print(index, len(test_dataset))
                print(len(boxes), len(scores))
                raise

        targets = targets.squeeze(0).cpu().numpy()
        targets = np.array(albumentations.denormalize_bboxes(targets, images.shape[2], images.shape[3]))
        evaluator.add_sample(targets, detections_by_class)
        targets_detections.append((targets, detections_by_class))
    aps, classes = evaluator.evaluate()

    return aps, classes, targets_detections

def separate_target_and_detections(targets_detections):
    final_targets = []
    final_detections = []
    for item in targets_detections:
        targets = item[0]
        filtered_targets = targets[targets[:, 4] == 1]
        final_targets.append(filtered_targets)
        detections = np.array(item[1][1], np.float32)
        final_detections.append(detections)

    return final_targets, final_detections


def calc_metrics(final_targets, final_detections, score_threshold_list):
    results = {}
    print("Calculating metrics...")
    for conf_thr in score_threshold_list:
        fppi = 0
        fnpi = 0
        num_all_matched = 0
        average_recall = []
        for i, (gt_boxes, det_boxes) in enumerate(zip(final_targets, final_detections)):

            det_boxes = det_boxes[det_boxes[:, 4] > conf_thr]

            if len(gt_boxes) == 0 and len(det_boxes) == 0:
                num_all_matched += 1
                continue
            elif len(gt_boxes) == 0:
                recall = 1
                average_recall.append(recall)
                continue
            elif len(det_boxes) == 0:
                recall = 0
                average_recall.append(recall)
                continue
            overlap_thresh = 0.5
            matches, fp, fn = get_matches(gt_boxes, det_boxes, overlap_thresh)
            fppi += fp
            fnpi += fn
            recall = len(matches) / float(len(gt_boxes))
            average_recall.append(recall)

            if len(gt_boxes) == len(det_boxes):
                num_all_matched += 1

        recall = sum(average_recall) / float(len(average_recall)) * 100
        accuracy = num_all_matched / float(len(average_recall)) * 100
        fppi = fppi / float(len(average_recall))
        fnpi = fnpi / float(len(average_recall))

        results[conf_thr] = {
            "AR": [recall],
            "Accuracy": [accuracy],
            "fppi": [fppi],
            "fnpi": [fnpi]
        }
    print("Metrics calculated")
    return results


def visualize_evaluation_results(output_dir, final_targets, final_detections, score_threshold_list, results, test_dataset, resize_ratio=1):

    for conf_thr in score_threshold_list:
        print("Confidence threshold:", conf_thr)
        diff = []
        for i, (gt_boxes, det_boxes) in enumerate(zip(final_targets, final_detections)):
            image = test_dataset.pull_image(i)
            image = cv2.resize(image, (320, 240))
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
            det_boxes = det_boxes[det_boxes[:, 4] > conf_thr]
            for box in det_boxes:
                image = cv2.rectangle(image, (int(resize_ratio * box[0]), int(resize_ratio * box[1])), (int(resize_ratio * box[2]), int(resize_ratio * box[3])), (0, 0, 255), 1)
            for box in gt_boxes:
                image = cv2.rectangle(image, (int(resize_ratio * box[0]), int(resize_ratio * box[1])), (int(resize_ratio * box[2]), int(resize_ratio * box[3])), (0, 255, 0), 1)

            current_diff = len(det_boxes) - len(gt_boxes)
            diff_path = os.path.join(output_dir + "_" + str(conf_thr), f"diff_{str(current_diff)}")
            img_path = os.path.basename(test_dataset.ids[i])
            os.makedirs(diff_path, exist_ok=True)
            cv2.imwrite(os.path.join(diff_path, img_path), image)

            diff.append(current_diff)

        for metric, value in results[conf_thr].items():
            print(f"{metric}: {value[0]:0.2f}")

        diff = np.array(diff)
        print(f"min diff: {diff.min()}")
        print(f"max diff: {diff.max()}")
        num_bins = int(diff.max() - diff.min() + 1)
        hist, _ = np.histogram(diff, range=(diff.min(), diff.max()), bins=num_bins)
        print(f"diff hist:{hist}")
        print()
        # the histogram of the data
        plt.hist(diff, num_bins, facecolor="blue", alpha=0.5, edgecolor="k")
        xticks = range(diff.min(), diff.max() + 1)
        plt.xticks(xticks)
        plt.savefig(os.path.join(output_dir + "_" + str(conf_thr), "histogram_conf_{}.png".format(conf_thr)))
        plt.close()

