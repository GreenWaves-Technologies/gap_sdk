# SPDX-License-Identifier: MIT

import onnxruntime as ort
import argparse
import numpy as np
from box_utils import predict

import argparse
from os import path as osp

import numpy as np
from tqdm import tqdm

from data_utils import Dataset, load_dataset
from augmentor import Augmentor
from coco import COCOEvaluator
from evaluation import evaluate_onnx_detector, visualize_evaluation_results, \
    separate_target_and_detections, calc_metrics

from config import SSDModelConfiguration


def main():
    parser = argparse.ArgumentParser(description="Evaluate onnx model")
    parser.add_argument("--onnx_model", required=True, help="Path to onnx model")
    parser.add_argument("-mc", "--model_config", default="model_config.json",
        help="Path to model configuration file.")
    parser.add_argument("-dp", "--dataset_path", required=True)
    parser.add_argument('-od', '--output_dir', required=True, help='Folder to store the visualizations')
    parser.add_argument('-nms', '--nms_thr', default=0.5, help='NMS threshold value')

    args = parser.parse_args()

    onnx_model_name = args.onnx_model
    model_config = args.model_config
    dataset_path = args.dataset_path
    nms_thr = args.nms_thr

    model_configuration = SSDModelConfiguration(model_config)

    # Load TFLite model and allocate tensors.
    num_workers = 4
    face_detector = ort.InferenceSession(onnx_model_name)

    # Test model on random input data.
    test_dataset = load_dataset(
        Dataset,
        dataset_path,
        model_configuration,
        test_aug=Augmentor(model_configuration.augmentations["test"]),
        test_only=True,
    )

    score_threshold_list=(0.7, 0.75, 0.8)

    evaluator = COCOEvaluator(test_dataset.classes, overlap_thresh=nms_thr)
    _, _, targets_detections_matched = evaluate_onnx_detector(
        face_detector,
        test_dataset,
        evaluator=evaluator,
        num_workers=num_workers,
    )
    targets, detections = separate_target_and_detections(targets_detections_matched)
    results = calc_metrics(targets, detections, score_threshold_list)
    visualize_evaluation_results(args.output_dir, targets, detections, score_threshold_list, results, test_dataset)



if __name__ == "__main__":
    main()