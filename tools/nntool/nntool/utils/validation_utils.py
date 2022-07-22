# Copyright (C) 2022  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import json
import os
from abc import ABC, abstractmethod, abstractproperty
from typing import Sequence

import numpy as np

SUPPORTED_PREDICTION = {'classification'}  # add 'object-detection'


class ValidationResultBase(ABC):
    """Base class for validation results
    """

    @abstractproperty
    def validated(self) -> bool:
        """ Is result valid. Overriden in derived class.

        Returns:
            bool: True if result is valid
        """
        raise NotImplementedError('should be implemented in derived class')

    @property
    def margin(self) -> float:
        """ What is the margin of the result. Value between 0 and 1.
            Can be overriden in derived class. Return 0.0 if not used.

        Returns:
            float: Margin of result
        """
        return 0.0


class ValidateBase(ABC):

    def validate(self,
                 input_tensors: Sequence[np.ndarray],
                 output_tensors: Sequence[Sequence[np.ndarray]],
                 input_name: str = None) -> ValidationResultBase:
        """ Validate an execution result

        Args:
            input_tensors (Sequence[np.ndarray]): Input tensors provided to execute
            output_tensors (Sequence[Sequence[np.ndarray]]): Output tensors provided by execute
            input_name (str, optional): Name of input file. Defaults to None.

        Returns:
            ValidationResultBase: Result of validation
        """
        return self._validate(input_tensors, output_tensors, input_name=input_name)

    @abstractmethod
    def _validate(self,
                  input_tensors: Sequence[np.ndarray],
                  output_tensors: Sequence[Sequence[np.ndarray]],
                  input_name: str = None) -> ValidationResultBase:
        """ Validate an execution result. Overriden in derived class.

        Args:
            input_tensors (Sequence[np.ndarray]): Input tensors provided to execute
            output_tensors (Sequence[Sequence[np.ndarray]]): Output tensors provided by execute
            input_name (str, optional): Name of input file. Defaults to None.

        Returns:
            ValidationResultBase: Result of validation
        """
        raise NotImplementedError('should be implemented in derived class')


class ClassifierValidationResult(ValidationResultBase):
    def __init__(self, class_predicted, actual_class, margin) -> None:
        super().__init__()
        self._result = (class_predicted, actual_class, margin)

    @property
    def result(self):
        return self._result

    @property
    def validated(self):
        return self._result[0] == self._result[1]

    @property
    def margin(self):
        return self._result[2]


class ClassifierValidateBase(ValidateBase):
    def __init__(self, class_thr=0, binary_classification=False, type_of_prediction='classification', out_idx=-1):
        if type_of_prediction not in SUPPORTED_PREDICTION:
            raise NotImplementedError(
                "type_of_prediction must be in %r, %s not supported" % SUPPORTED_PREDICTION, type_of_prediction)
        self.class_thr = class_thr
        self.binary_classification = binary_classification
        self.labels = []
        self.predictions = []
        self.out_idx = out_idx
        self.last_prediction = None

    def _validate(self,
                  input_tensors: Sequence[np.ndarray],
                  output_tensors: Sequence[Sequence[np.ndarray]],
                  input_name: str = None) -> ValidationResultBase:
        del input_tensors
        predicted = output_tensors[self.out_idx][0].flatten()
        actual_class = self._label(input_name, predicted)
        if self.binary_classification:
            class_predicted = int(predicted > self.class_thr)
            margin = abs(predicted - self.class_thr)
        else:
            class_predicted = int(np.argmax(predicted)) if np.amax(
                predicted) > self.class_thr else 0
            margin = predicted[class_predicted] - \
                np.average(np.delete(predicted, [class_predicted]))

        self.predictions.append(class_predicted)
        self.labels.append(actual_class)
        self.last_prediction = (class_predicted, actual_class, margin)
        return ClassifierValidationResult(class_predicted, actual_class, margin)

    def _label(self, input_name: str, predicted: np.ndarray) -> int:
        raise NotImplementedError()


class ValidateFromClass(ClassifierValidateBase):
    def __init__(self, class_number, type_of_prediction='classification', **kargs):
        super().__init__(type_of_prediction=type_of_prediction, **kargs)
        self._class_number = class_number

    def _label(self, input_name, predicted):
        return self._class_number


class ValidateFromName(ClassifierValidateBase):
    # the label are the last digits in the filename

    def _label(self, input_name, predicted):
        num_classes = predicted.size
        filename, _ = os.path.splitext(input_name)
        num_classes_digits = len(str(num_classes-1))
        return int(filename[-(num_classes_digits):])


class ValidateFromJSON(ClassifierValidateBase):
    def __init__(self, json_file, **kargs):
        super().__init__(**kargs)
        with open(json_file) as file:
            self.annotations = json.load(file)

    def _label(self, input_name, predicted):
        _, file = os.path.split(input_name)
        return self.annotations[file]


class ValidateFromVWWInstances(ClassifierValidateBase):
    def __init__(self, instances_file, **kargs):
        super().__init__(**kargs)
        with open(instances_file) as file:
            self.instances = json.load(file)

    def _label(self, input_name, predicted):
        _, file_name = os.path.split(input_name)
        for image in self.instances['images']:
            if image['file_name'] == file_name:
                idx = image['id']
                return self.instances['annotations'][str(idx)]['label']
        raise ValueError(f'filename {file_name} not found')
