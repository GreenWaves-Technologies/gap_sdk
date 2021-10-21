from collections import ChainMap

import cv2
import numpy as np
import torch
import torch.utils.data as data
from custom_augmentations import clip_bbox


class BaseDataset(data.Dataset):
    def __getitem__(self, index):
        target = self.annotations[index]
        img = self.pull_image(index)

        if self.use_clip:
            target = clip_bbox(target, img.shape[1], img.shape[0])
        target = np.array([item for item in target if item[2] - item[0] > 0 and item[3] - item[1] > 0])

        if self.transformation is not None:
            img, target = self.transformation(img, target)

        if not len(target):
            target = [[0, 0, 1, 1, 0]]

        return torch.from_numpy(img), np.array(target)

    def get_image_id(self, index):
        return self.ids[index]

    def __len__(self):
        return len(self.ids)

    def pull_image(self, index):
        """Returns the original image object at index in RGB

        Note: not using self.__getitem__(), as any transformations passed in
        could mess up this functionality.

        Argument:
            index (int): index of img to show
        Return:
            img(uint8 np.array): OpenCV img RGB
        """

        img_id = self.ids[index]
        img = cv2.imread(img_id, cv2.IMREAD_UNCHANGED)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        return img

    def pull_tensor(self, index):
        """Returns the original image at an index in tensor form

        Note: not using self.__getitem__(), as any transformations passed in
        could mess up this functionality.

        Argument:
            index (int): index of img to show
        Return:
            tensorized version of img, squeezed
        """

        return torch.Tensor(self.pull_image(index)).unsqueeze_(0)

    @staticmethod
    def detection_collate(batch):
        """Custom collate fn for dealing with batches of images that have a different
        number of associated object annotations (bounding boxes).

        Arguments:
            batch: (tuple) A tuple of tensor images and lists of annotations

        Return:
            A tuple containing:
                1) (tensor) batch of images stacked on their 0 dim
                2) (list of tensors) annotations for a given image are stacked on 0 dim
        """
        targets = []
        imgs = []
        for _, sample in enumerate(batch):
            for _, tup in enumerate(sample):
                if torch.is_tensor(tup):
                    imgs.append(tup)
                elif isinstance(tup, type(np.empty(0))):
                    annos = torch.from_numpy(tup).float()
                    targets.append(annos)

        return torch.stack(imgs, 0), targets

    @property
    def classes(self):
        return self._classes
