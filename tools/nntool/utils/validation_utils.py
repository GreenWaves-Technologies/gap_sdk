import json
import os
from abc import ABC, abstractmethod
import numpy as np

SUPPORTED_PREDICTION = {'classification'} #add 'object-detection'

class ValidateBase(ABC):
    def __init__(self, class_thr=0, binary_classification=False, type_of_prediction='classification'):
        if type_of_prediction not in SUPPORTED_PREDICTION:
            raise NotImplementedError("type_of_prediction must be in %r, %s not supported" %SUPPORTED_PREDICTION, type_of_prediction)
        self.class_thr = class_thr
        self.binary_classification = binary_classification
        self.labels = []
        self.predictions = []

    @abstractmethod
    def validate(self, input_name, predicted):
        pass

class ValidateFromClass(ValidateBase):
    def __init__(self, class_number, type_of_prediction='classification', **kargs):
        super().__init__(type_of_prediction=type_of_prediction, **kargs)
        self._class_number = class_number

#the label are all the same
    def validate(self, input_name, predicted):
        predicted = predicted.flatten()
        if self.binary_classification:
            class_predicted = int(predicted > self.class_thr)
            margin = abs(predicted - self.class_thr)
        else:
            class_predicted = int(np.argmax(predicted)) if np.amax(predicted) > self.class_thr else 0
            margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.predictions.append(class_predicted)
        self.labels.append(self._class_number)
        return class_predicted == self._class_number, class_predicted, self._class_number, margin

class ValidateFromName(ValidateBase):
#the label are the last digits in the filename
    def validate(self, input_name, predicted):
        num_classes = predicted.size
        filename, _ = os.path.splitext(input_name)
        num_classes_digits = len(str(num_classes-1))
        label = int(filename[-(num_classes_digits):])
        predicted = predicted.flatten()
        if self.binary_classification:
            class_predicted = int(predicted > self.class_thr)
            margin = abs(predicted - self.class_thr)
        else:
            class_predicted = int(np.argmax(predicted)) if np.amax(predicted) > self.class_thr else 0
            margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.predictions.append(class_predicted)
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin

class ValidateFromJSON(ValidateBase):
    def __init__(self, json_file, **kargs):
        super().__init__(**kargs)
        with open(json_file) as file:
            self.annotations = json.load(file)

    def validate(self, input_name, predicted):
        #num_classes = predicted.size
        _, file = os.path.split(input_name)
        label = self.annotations[file]
        predicted = predicted.flatten()
        if self.binary_classification:
            class_predicted = int(predicted > self.class_thr)
            margin = abs(predicted - self.class_thr)
        else:
            class_predicted = int(np.argmax(predicted)) if np.amax(predicted) > self.class_thr else 0
            margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.predictions.append(class_predicted)
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin

class ValidateFromVWWInstances(ValidateBase):
    def __init__(self, instances_file, **kargs):
        super().__init__(**kargs)
        with open(instances_file) as file:
            self.instances = json.load(file)

    def validate(self, input_name, predicted):
        _, file_name = os.path.split(input_name)
        for image in self.instances['images']:
            if image['file_name'] == file_name:
                idx = image['id']
                label = self.instances['annotations'][str(idx)]['label']
                break
        predicted = predicted.flatten()
        if self.binary_classification:
            class_predicted = int(predicted > self.class_thr)
            margin = abs(predicted - self.class_thr)
        else:
            class_predicted = int(np.argmax(predicted)) if np.amax(predicted) > self.class_thr else 0
            margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.predictions.append(class_predicted)
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin
