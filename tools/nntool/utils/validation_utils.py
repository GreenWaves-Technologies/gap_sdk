import json
import os
from abc import ABC, abstractmethod
import numpy as np

SUPPORTED_PREDICTION = {'classification'} #add 'object-detection'

class ValidateBase(ABC):
    def __init__(self, type_of_prediction='classification'):
        if type_of_prediction not in SUPPORTED_PREDICTION:
            raise NotImplementedError("type_of_prediction must be in %r, %s not supported" %SUPPORTED_PREDICTION, type_of_prediction)
        self.labels = []
        self.predictions = []

    @abstractmethod
    def validate(self, input_name, predicted):
        pass

class ValidateFromClass(ValidateBase):
    def __init__(self, class_number, type_of_prediction='classification'):
        super().__init__(type_of_prediction=type_of_prediction)
        self._class_number = class_number

#the label are all the same
    def validate(self, input_name, predicted):
        predicted = predicted.flatten()
        class_predicted = int(np.argmax(predicted))
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
        class_predicted = int(np.argmax(predicted))
        margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.predictions.append(class_predicted)
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin

class ValidateFromJSON(ValidateBase):
    def __init__(self, json_file):
        super().__init__()
        with open(json_file) as file:
            self.annotations = json.load(file)

    def validate(self, input_name, predicted):
        #num_classes = predicted.size
        _, file = os.path.split(input_name)
        label = self.annotations[file]
        predicted = predicted.flatten()
        class_predicted = int(np.argmax(predicted))
        self.predictions.append(class_predicted)
        margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin

class ValidateFromVWWInstances(ValidateBase):
    def __init__(self, instances_file):
        super().__init__()
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
        class_predicted = int(np.argmax(predicted))
        self.predictions.append(class_predicted)
        margin = predicted[class_predicted] - np.average(np.delete(predicted, [class_predicted]))
        self.labels.append(label)
        return class_predicted == label, class_predicted, label, margin
