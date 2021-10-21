import numpy as np
from typing import Union, Tuple

import albumentations as A
import albumentations.augmentations.functional as F

from albumentations.augmentations.bbox_utils import denormalize_bbox, normalize_bbox
from box_utils import jaccard_np


class CustomImageTranspose(A.ImageOnlyTransform):
    def __init__(self, shape):
        super(CustomImageTranspose, self).__init__(True, 1.0)
        self.new_shape = shape

    def apply(self, image, **params):
        return image.transpose(self.new_shape)


def cutout_with_color(img, x_start, y_start, widths, heights, color):
    # Make a copy of the input image since we don't want to modify it directly
    img = img.copy()
    height, width = img.shape[:2]

    for x, y, cut_width, cut_height in zip(x_start, y_start, widths, heights):
        y1 = np.clip(y, 0, height)
        y2 = np.clip(y + cut_height, 0, height)
        x1 = np.clip(x, 0, width)
        x2 = np.clip(x + cut_width, 0, width)

        img[y1:y2, x1:x2] = color
    return img


def clip_bbox(bboxes, width, height):
    bboxes = np.array(bboxes)
    bboxes[:, 0:4:2] = np.clip(bboxes[:, 0:4:2], 0, width)
    bboxes[:, 1:4:2] = np.clip(bboxes[:, 1:4:2], 0, height)
    return bboxes


def cutout_bbox(bbox, x_starts, y_starts, widths, heights, rows, cols):
    bbox = denormalize_bbox(bbox, rows, cols)
    crops_coords = np.vstack([x_starts, y_starts, x_starts + widths, y_starts + heights]).T
    x_min, y_min, x_max, y_max = bbox
    intersections = np.vstack(
        [
            np.maximum(x_min, crops_coords[:, 0]),
            np.maximum(y_min, crops_coords[:, 1]),
            np.minimum(x_max, crops_coords[:, 2]),
            np.minimum(y_max, crops_coords[:, 3]),
        ]
    ).T
    intersection_widths = intersections[:, 2] - intersections[:, 0]
    intersection_heights = intersections[:, 3] - intersections[:, 1]
    has_intersection = np.asarray((intersection_heights > 0) & (intersection_widths > 0))
    if np.any(has_intersection):
        intersection_areas = intersection_heights[has_intersection] * intersection_widths[has_intersection]
        box_area = (x_max - x_min) * (y_max - y_min)
        if np.any(intersection_areas / box_area > 0.6):
            return [0, 0, 0, 0]
    bbox = normalize_bbox(bbox, rows, cols)
    return bbox


def check_intersection(center_x, center_y, size_x, size_y, gt_boxes):
    x_min, y_min = center_x - size_x / 2, center_y - size_y / 2
    x_max, y_max = center_x + size_x / 2, center_y + size_y / 2

    in_bbox = np.array([[x_min, y_min, x_max, y_max]])
    gt_boxes = np.array(gt_boxes)

    if np.any(jaccard_np(gt_boxes, in_bbox)) > 0:
            return False

    return True



class Cutout(A.DualTransform):
    """CoarseDropout of the square regions in the image.

    Args:
        num_holes (int): number of regions to zero out
        width_range (list): list of two values showing the range cutout width should belong to
        aspect_range (list): list of two values showing the range cutout aspect ration should belong to

    Targets:
        image

    Image types:
        uint8, float32

    Reference:
    |  https://arxiv.org/abs/1708.04552
    |  https://github.com/uoguelph-mlrg/Cutout/blob/master/util/cutout.py
    |  https://github.com/aleju/imgaug/blob/master/imgaug/augmenters/arithmetic.py

    """

    def __init__(
        self, num_holes=8, max_x=0, max_y=0, width_range=(8, 8), aspect_range=(1, 1), color=0, always_apply=False, p=0.5
    ):
        super(Cutout, self).__init__(always_apply, p)
        self.num_holes = num_holes
        self.width_range = width_range
        self.aspect_range = aspect_range
        self.color = color
        self.max_x = max_x
        self.max_y = max_y
        self.p = p

    def apply(self, image, x_start=[], y_start=[], widths=[], heights=[], **params):
        return cutout_with_color(image, x_start, y_start, widths, heights, self.color)

    def apply_to_bbox(self, bbox, x_start=[], y_start=[], widths=[], heights=[], rows=0, cols=0, **params):
        bbox = cutout_bbox(bbox, x_start, y_start, widths, heights, rows, cols)
        return bbox

    def get_params(self):
        y_values = np.random.randint(0, self.max_y, self.num_holes).T
        x_values = np.random.randint(0, self.max_x, self.num_holes).T
        holes_widths = np.random.randint(self.width_range[0], self.width_range[1], self.num_holes).T
        holes_aspects = (
            self.aspect_range[0]
            + np.random.uniform(0, abs(self.aspect_range[1] - self.aspect_range[0]), self.num_holes).T
        )
        holes_heights = (holes_widths * holes_aspects).astype(int)

        return {"x_start": x_values, "y_start": y_values, "widths": holes_widths, "heights": holes_heights}
