import importlib
import logging
from wider_face import Dataset


def load_dataset(dataset_type, dataset_path, model_configuration, test_only=False, test_aug=None, train_aug=None):
    logging.info("Loading Dataset...")

    test_dataset = Dataset(
        dataset_path,
        image_set="test",
        transformation=test_aug,
        use_clip=model_configuration.clip_bbox,
    )
    if test_only:
        return test_dataset

    train_dataset = Dataset(
        dataset_path,
        image_set="train",
        transformation=train_aug,
        use_clip=model_configuration.clip_bbox,
    )

    return train_dataset, test_dataset


def load_simple_test_dataset(dataset_type, dataset_path, test_aug=lambda *arguments: arguments):
    logging.info("Loading Dataset...")

    spec = importlib.util.find_spec("detective.data." + dataset_type.name.lower())
    if spec is None:
        logging.critical("Can't find the module for dataset: %", dataset_type)
        exit(-1)
    else:
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        test_dataset = module.Dataset(
            dataset_path,
            image_set="test",
            transformation=test_aug,
        )

        return test_dataset
