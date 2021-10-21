import copy
import logging

import albumentations
import torchvision.transforms as tv_transforms
import custom_augmentations
import omegaconf


class Augmentor:
    def __init__(self, augmentations, engine="albumentations", min_visible_area_fraction=0.4):
        assert engine in ["albumentations", "torchvision"]
        self.engine = engine
        augmentations = self._parse_dict(augmentations)

        if self.engine == "albumentations":
            self.aug = albumentations.Compose(
                augmentations,
                bbox_params={"format": "albumentations", "min_visibility": min_visible_area_fraction, "min_area": 0.0},
                p=1,
            )
        else:
            self.aug = tv_transforms.Compose(augmentations)

    def __call__(self, img, objects=None):
        if objects is None:
            objects = []
        if self.engine == "albumentations":
            objects = albumentations.normalize_bboxes(objects, img.shape[0], img.shape[1])
            data = self.aug(image=img, bboxes=objects)
            return data["image"], data["bboxes"]
        else:
            return self.aug(img)

    def _parse_dict(self, dict_list):
        def find_class(name):
            if self.engine == "albumentations":
                if name in dir(custom_augmentations):
                    aug_class = getattr(custom_augmentations, name)
                elif name in dir(albumentations):
                    aug_class = getattr(albumentations, name)
                else:
                    logging.critical("Invalid augmentation name:%s", name)
                    exit(-1)
            else:  # torchvision
                if name in dir(tv_transforms):
                    aug_class = getattr(tv_transforms, name)
                else:
                    logging.critical("Invalid augmentation name:%s", name)
                    exit(-1)
            return aug_class

        # Convert to the python internal type
        if isinstance(dict_list, (omegaconf.ListConfig, omegaconf.DictConfig)):
            dict_list = omegaconf.OmegaConf.to_container(dict_list)
        res = []
        for dict_item in dict_list:
            class_name, class_params = list(copy.deepcopy(dict_item).items())[0]
            cls = find_class(class_name)
            if class_name in ["OneOrOther", "OneOf"]:
                assert self.engine == "albumentations"
                transforms = self._parse_dict(class_params["transforms"])
                class_params.update({"transforms": transforms})
            res.append(cls(**class_params))
        return res
