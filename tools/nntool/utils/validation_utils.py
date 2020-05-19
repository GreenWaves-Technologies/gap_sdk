import numpy as np
import logging
import json
import os
from abc import ABC, abstractmethod

SUPPORTED_PREDICTION = {'classification'} #add 'object-detection', 'segmentation'

class ValidateBase(ABC):
	def __init__(self, type_of_prediction='classification'):
		if type_of_prediction not in SUPPORTED_PREDICTION:
			raise NotImplementedError("type_of_prediction must be in %r, %s not supported" %SUPPORTED_PREDICTION, type_of_prediction)
		self.labels = []
		self.predictions = []

	@abstractmethod
	def validate(self, input_name, predicted):
		pass

class ValidateFromName(ValidateBase):
	#the label are the last digits in the filename
	def validate(self, input_name, predicted):
		num_classes = predicted.size
		filename, _ = os.path.splitext(input_name)
		num_classes_digits = len(str(num_classes))
		label = int(filename[-(num_classes_digits):])
		class_predicted = int(np.argmax(predicted))
		self.predictions.append(class_predicted)
		self.labels.append(label)
		return class_predicted == label, label

class ValidateFromJSON(ValidateBase):
	def __init__(self, json_file):
		super().__init__()
		with open(json_file) as file:
			self.annotations = json.load(file)

	def validate(self, input_name, predicted):
		num_classes = predicted.size
		path, file = os.path.split(input_name)
		label = self.annotations[file]
		class_predicted = int(np.argmax(predicted))
		self.predictions.append(class_predicted)
		self.labels.append(label)
		return class_predicted == label, label



