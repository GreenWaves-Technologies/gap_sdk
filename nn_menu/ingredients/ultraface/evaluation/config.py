import json
import logging
import os
from collections import OrderedDict

from jsonschema import validate


class Configuration(object):
    def __init__(self, config_file_path):
        self.from_json(config_file_path)

    def from_json(self, load_fp):
        if os.path.exists(load_fp):
            with open(load_fp) as j_file:
                self.update(json.load(j_file))
        else:
            logging.critical("Invalid config path \n All variables initialized with None")

    def update(self, new_dict):
        self._validate_args(new_dict)
        self.__dict__.update(new_dict)

    def to_json(self, save_fp):
        if save_fp:
            with open(save_fp, "w") as j_file:
                json.dump(self.__dict__, j_file, sort_keys=False, indent=4)

    def _validate_args(self, new_dict):
        logging.critical("Please implement the _validate_args method")
        exit(-1)


class SSDModelConfiguration(Configuration):
    def __init__(self, config_file_path):
        self.augmentations = None
        self.clip_bbox = None
        super().__init__(config_file_path)

    def _validate_args(self, new_dict):
        props = {
            "augmentations": {"type": "object"},
            "clip_bbox": {"type": "boolean"}
        }
        all_props = props.copy()
        schema = {"properties": all_props, "required": list(props.keys())}

        validate(new_dict, schema)

    def update(self, new_dict):
        super().update(new_dict)