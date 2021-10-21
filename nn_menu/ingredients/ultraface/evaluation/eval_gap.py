import argparse
import numpy as np
import pandas as pd
import argparse
from os import path as osp

from data_utils import Dataset, load_dataset
from augmentor import Augmentor
from coco import COCOEvaluator
from evaluation import evaluate_gap_detector, visualize_evaluation_results, \
    separate_target_and_detections, calc_metrics

from config import SSDModelConfiguration

def main():
    parser = argparse.ArgumentParser(description="Evaluate onnx model")
    parser.add_argument("-mc", "--model_config", default="model_config.json",
        help="Path to model configuration file.")
    parser.add_argument('--csv_dir', required=True, help='Path to csv directory with detections from GAP')
    parser.add_argument("-dp", "--dataset_path", required=True)
    parser.add_argument('-od', '--output_dir', required=True, help='Folder to store the visualizations')
    parser.add_argument('-nms', '--nms_thr', default=0.5, help='NMS threshold value')

    args = parser.parse_args()
    model_config = args.model_config
    dataset_path = args.dataset_path
    csv_dir = args.csv_dir
    nms_thr = args.nms_thr

    model_configuration = SSDModelConfiguration(model_config)

    # Load TFLite model and allocate tensors.
    num_workers = 4

    # Test model on random input data.
    test_dataset = load_dataset(
        Dataset,
        dataset_path,
        model_configuration,
        test_aug=Augmentor(model_configuration.augmentations["test"]),
        test_only=True,
    )

    detections = []
    for file in test_dataset.ids:
        csv_name = osp.basename(file).replace(".jpg", ".csv")
        try:
            df = pd.read_csv(osp.join(csv_dir, csv_name), sep=';', header=None)
            dets = df.values

        except pd.errors.EmptyDataError:
            dets = []
        detections.append(dets)


    evaluator = COCOEvaluator(test_dataset.classes, overlap_thresh=nms_thr)
    _, _, targets_detections = evaluate_gap_detector(
        detections,
        test_dataset,
        evaluator=evaluator,
        num_workers=num_workers
    )

    targets, detections = separate_target_and_detections(targets_detections)

    score_threshold_list=(0.7, 0.75, 0.8)
    results = calc_metrics(targets, detections, score_threshold_list)
    visualize_evaluation_results(args.output_dir, targets, detections, score_threshold_list, results, test_dataset)



if __name__ == "__main__":
    main()