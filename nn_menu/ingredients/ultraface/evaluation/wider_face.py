# **********************************************************************************
#                                XPERIENCE AI CONFIDENTIAL
#                               Copyright 2019 Xperience AI.
# Any usage or modification without written consent from Xperience.ai is prohibited
# **********************************************************************************

import logging
import os

import numpy as np
import cv2
import torch
import scipy.io
from base_dataset import BaseDataset
from custom_augmentations import clip_bbox


class Dataset(BaseDataset):
    """WIDER Face Detection Dataset Object

    input is image, target is annotation

    Arguments:
        root (string): filepath to WIDER Face folder.
        image_set (string): imageset to use (eg. 'train', 'val', 'test')
        transform (callable, optional): transformation to perform on the
            input image
        target_transform (callable, optional): transformation to perform on the
            target `annotation`
            (eg: take in caption string, return tensor of word indices)
        dataset_name (string, optional): which dataset to load
            (default: 'WIDER_FACE')
    """

    def __init__(self, root, image_set, transformation=None, use_clip=True, **kwargs):
        self.root = root
        self.image_set = image_set
        self.transformation = transformation
        self.name = "WIDER_FACE"
        self.ids = []
        self.annotations = []

        self._classes = ["__background__", "face"]
        self.num_classes = len(self._classes)
        self.use_clip = use_clip

        if image_set == "train":
            imgs_path = os.path.join(root, "train", "images")
            annos_path = os.path.join(root, "train", "wider_face_train.mat")
        elif image_set == "test":
            imgs_path = os.path.join(root, "val", "images")
            annos_path = os.path.join(root, "val", "wider_hard_val.mat")
        else:
            logging.critical("set %s doesn't exists", image_set)
            exit(-1)

        mat = scipy.io.loadmat(annos_path)

        self.bboxs = {}
        self.labels = {}
        self.difficult = {}
        for i in range(len(mat["event_list"])):
            event = mat["event_list"][i, 0][0]
            for j in range(len(mat["file_list"][i, 0])):
                file = mat["file_list"][i, 0][j, 0][0]
                filename = "{}.jpg".format(file)
                filepath = os.path.join(imgs_path, event, filename)

                bboxs = mat["face_bbx_list"][i, 0][j, 0]
                bboxs[:, 2:4] = bboxs[:, 2:4] + bboxs[:, 0:2]

                if image_set == "test":
                    gt = mat["gt_list"][i, 0][j, 0] - 1  # matlab like indices reduced by 1
                    if len(gt) > 1:
                        gt = gt.squeeze()
                        bboxs = bboxs[gt, :]
                    elif len(gt) == 1:
                        gt = gt[0]
                        bboxs = bboxs[gt, :]
                    else:
                        continue

                self.ids.append(filepath)
                self.bboxs[filepath] = bboxs.astype(np.float64)
                self.labels[filepath] = np.ones([len(bboxs), 1], dtype=np.float64)

                self.annotations.append(np.concatenate([self.bboxs[filepath], self.labels[filepath]], axis=1))

    def pull_image(self, index):
        """Returns the original image object at index in grayscale

        Note: not using self.__getitem__(), as any transformations passed in
        could mess up this functionality.

        Argument:
            index (int): index of img to show
        Return:
            img(uint8 np.array): OpenCV img grayscale
        """

        img_id = self.ids[index]
        image = cv2.imread(img_id)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image = image.astype(np.float32)

        return image

    def __getitem__(self, index):
        target = self.annotations[index]
        img = self.pull_image(index)

        if not len(target):
            target = [[0, 0, img.shape[1], img.shape[0], 0]]

        if self.use_clip:
            target = clip_bbox(target, img.shape[1], img.shape[0])
        target = np.array([item for item in target if item[2] - item[0] > 0 and item[3] - item[1] > 0])

        if self.transformation is not None:
            img, target = self.transformation(img, target)
        if not len(target):
            target = [[0, 0, 1, 1, 0]]

        img = img.astype(np.float32)
        return torch.from_numpy(img), np.array(target)
