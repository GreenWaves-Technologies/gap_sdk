# **********************************************************************************
#                                XPERIENCE AI CONFIDENTIAL
#                               Copyright 2019 Xperience AI.
# Any usage or modification without written consent from Xperience.ai is prohibited
# **********************************************************************************

import argparse
import pandas as pd
import seaborn as sns
import os
import cv2
from albumentations import Compose, Resize, Normalize

import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm
from augmentor import Augmentor
from config import SSDModelConfiguration 
from data_utils import Dataset, load_dataset


def image_targets_dataset(val_dataset):
    dataset = []
    heights = []
    widths = []

    for img, target in tqdm(val_dataset):
        _, height, width = img.shape
        #print("width: ", width)
        #print("height: ", height)
        xmin, ymin, xmax, ymax, _ = target[0]
        #print("box: ", xmin, ymin, xmax, ymax)
        #print("scaled box: ", xmin * width, ymin * height, xmax * width, ymax * height)

        heights.append(ymax - ymin)
        widths.append(xmax - xmin)

        dataset.append([xmax - xmin, ymax - ymin])

    return np.array(dataset), np.array(heights), np.array(widths)

def folder_dataset(path_to_dataset):
    max_int = []
    min_int = []
    mean_int = []
    augmentations = Compose([Resize(width=320, height=240), Normalize(mean=[127, 127, 127], std=[128, 128, 128], max_pixel_value=1)])
    for path, _, names in tqdm(os.walk(path_to_dataset), desc='Processing validation data'):
        for image_name in names:
            image = cv2.imread(os.path.join(path, image_name))
            #print(image_name)
            #print(image)
            for augmentation in augmentations:
                image_augmented = augmentation(image=image)["image"]

            max_int.append(image_augmented.max())
            min_int.append(image_augmented.min())
            mean_int.append(image_augmented.mean())

    return max_int, min_int, mean_int

def draw_hist(data, title, color, name):
    plt.title(title, fontsize=10)
    sns.histplot(data=data, bins=10, color=color)
    plt.savefig(name)
    plt.show()
    plt.cla()

def draw_all_on_one(data):
    fig, plot = plt.subplots(3, sharex="col", constrained_layout=True)
    fig.suptitle("Height & width histograms", fontsize=12)
    plot[0].set_title("Height", fontsize=10)
    (n, bins, _) = plot[0].hist(data[:, 1], density=False, bins="auto", alpha=0.8, rwidth=0.85, facecolor="dodgerblue", linewidth=1)
    print("n: ", n)
    print("bins: ", bins)
    print("bin number: ", len(bins))
    plot[0].tick_params(axis="y", which="major", labelsize=9)

    plot[1].set_title("Width", fontsize=10)
    (n, bins, _) = plot[1].hist(data[:, 0], density=False, bins="auto", alpha=0.8, rwidth=0.85, facecolor="dodgerblue", linewidth=1)
    print("n: ", n)
    print("bins: ", bins)
    print("bin number: ", len(bins))
    plot[1].tick_params(axis="y", which="major", labelsize=9)

    plot[2].set_title("Square root H*W histogram", fontsize=10)
    (n, bins, _) = plot[2].hist(
        np.sqrt(data[:, 1] * data[:, 0]),
        density=False,
        bins="auto",
        alpha=0.8,
        rwidth=0.85,
        facecolor="dodgerblue",
        linewidth=1,
    )
    print("n: ", n)
    print("bins: ", bins)
    print("bin number: ", len(bins))
    plot[2].tick_params(axis="y", which="major", labelsize=9)

    plt.xticks(np.arange(0, max(np.sqrt(data[:, 1] * data[:, 0])), 10))
    plt.xticks(fontsize=8, rotation=90)
    #fig.subplots_adjust(bottom=0.10, hspace=0.3)
    plt.show()


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Calculate clusters for bounding boxes")
    parser.add_argument("-pd", "--path_to_dataset", required=True, help="Path to dataset")
    parser.add_argument("-mc", "--model_config", default="model_config.json",
        help="Path to model configuration file.")
    parser.add_argument("--target_stats", action="store_true", help="To draw target statistics")
    parser.add_argument("--pixel_stats", action="store_true", help="To draw pixel intensity statistics")

    args = parser.parse_args()
    dataset_path = args.path_to_dataset
    model_config = args.model_config
    target_stats = args.target_stats
    pixel_stats = args.pixel_stats

    if target_stats:
        model_configuration = SSDModelConfiguration(model_config)
        # Test model on random input data.
        test_dataset = load_dataset(
            Dataset,
            dataset_path,
            model_configuration,
            test_aug=Augmentor(model_configuration.augmentations["test"]),
            test_only=True,
        )
        data, height_data, width_data = image_targets_dataset(test_dataset)
        draw_all_on_one(data)

    if pixel_stats:
        #max_arr, min_arr, mean_arr = image_dataset(test_dataset)
        max_arr, min_arr, mean_arr = folder_dataset(dataset_path)
        draw_hist(min_arr, "Min Intensities", "mediumslateblue", "val_min_stats.png")
        draw_hist(max_arr, "Max Intensities", "mediumturquoise", "val_max_stats.png")
        draw_hist(mean_arr, "Mean Intensities", "mediumvioletred", "val_mean_stats.png")
