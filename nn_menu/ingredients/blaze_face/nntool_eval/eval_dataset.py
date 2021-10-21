import sys
import argparse
from pathlib import Path
from collections import namedtuple
import tensorflow as tf
import cv2

from anchors import Anchor, load_face_anchors

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
import numpy as np
import os
import glob
import ntpath
import random
import logging
import pandas as pd
from PIL import Image
import cv2
from matplotlib import pyplot as plt

#Accuracy of eval dataset detects 170 bbs in float and 150 in fixed point
#Accuracy of fixed point vs Float is 88% 

DUMP_QUANT = True
DUMP_FLOAT = True
threshold = 0.5
nms_threshold = 0.4
NUM_FILES = 21
score_clipping_threshold=100
dataset_files = glob.glob("eval_dataset/*")
quantization_result_path = "quantized_output"
non_quantization_result_path ="non_quantized_output"

class_names = ("null","person")


Rect = namedtuple('Rect', ['x', 'y', 'width', 'height'])
Point = namedtuple('Point', ['x', 'y'])
Detection = namedtuple('Detection', ['class_id', 'score', 'bounding_box', 'keypoints'])

RED_COLOR = (0, 0, 255, 255)
GREEN_COLOR = (0, 255, 0, 255)
BLUE_COLOR = (255, 0, 0, 255)
YELLOW_COLOR = (0, 255, 255, 255)

quantized_list= list()
non_quantized_list= list()


def resize_aspect_fit(src: np.array, target_side_size: int) -> tuple:
    src_height, src_width = src.shape[:2]
    max_side = max(src_width, src_height)
    scale = target_side_size / max_side
    target_width = int(round(scale * src_width))
    target_height = int(round(scale * src_height))
    interpolation_type = cv2.INTER_AREA if scale < 1.0 else cv2.INTER_LINEAR
    dst = cv2.resize(src, (target_width, target_height), interpolation=interpolation_type)
    top_border = int((target_side_size - target_height) / 2)
    bottom_border = target_side_size - target_height - top_border
    left_border = int((target_side_size - target_width) / 2)
    right_border = target_side_size - target_width - left_border
    dst = cv2.copyMakeBorder(dst, top_border, bottom_border, left_border,
                             right_border, cv2.BORDER_CONSTANT)
    return dst, Rect(left_border, top_border, target_width, target_height)


class BlazeFace_decoder:
	
	# Initilizer
    def __init__(self, raw_boxes,raw_classes):
        
        self.number_of_keypoints = 6
        self.number_of_boxes = 896
        self.number_of_coordinates = 16
        self.number_of_classes = 1
        self.box_coordinates_offset = 0
        self.keypoints_coordinates_offset = 4
        self.number_of_values_per_keypoint = 2
        self.min_score_threshold = 0.5
        self.min_suppression_threshold = 0.4
        self.score_clipping_threshold = 100.
        self.use_score_clipping_threshold = True
        self.use_sigmoid_score = True
        self.is_input_vertically_flipped = False
        self.is_output_is_reversed = True
        self.input_size=1
        self.raw_boxes=raw_boxes
        self.raw_classes=raw_classes
        self.x_scale = 128
        self.y_scale = 128
        self.h_scale = 128
        self.w_scale = 128
        self.anchors = load_face_anchors()



    def postprocess(self, image_width: int, image_height: int, ) -> list:
        boxes, keypoints = self._decode_boxes_and_keypoints()
        scores, classes = self._decode_score_and_classes()
        
        resulting_boxes_indices = cv2.dnn.NMSBoxes(boxes, scores, self.min_score_threshold, self.min_suppression_threshold)
        indices = np.squeeze(resulting_boxes_indices, axis=-1) #if resulting_boxes_indices else []
        detections = []
        for index in indices:
            box = boxes[index]
            bounding_box = Rect(
                int(max(image_width  * (self.input_size * box.x ), 0)),
                int(max(image_height * (self.input_size * box.y ), 0)),
                int(image_width * (self.input_size * box.width) ),
                int(image_height * (self.input_size * box.height))
            )
            detection_keypoints = [
                Point(
                    int(image_width * (self.input_size * keypoint.x )),
                    int(image_height * (self.input_size * keypoint.y))
                )
                for keypoint in keypoints[index]
            ]
            detections.append(Detection(
                classes[index], scores[index], bounding_box, detection_keypoints
            ))
        return detections

    def _decode_boxes_and_keypoints(self) -> tuple:
        #regressors_output = self.interpreter.get_tensor(self.regressors_output_index)
        if self.is_output_is_reversed:
            box_offset_y = 1
            box_offset_x = 0
            box_offset_height = 3
            box_offset_width = 2
            keypoint_offset_y = 1
            keypoint_offset_x = 0
        else:
            box_offset_y = 0
            box_offset_x = 1
            box_offset_height = 2
            box_offset_width = 3
            keypoint_offset_y = 0
            keypoint_offset_x = 1
        boxes = []
        keypoints = []
        for regression_data, anchor in zip(self.raw_boxes, self.anchors):
            center_y = regression_data[self.box_coordinates_offset +
                                       box_offset_y] / self.y_scale * anchor.height + anchor.center_y
            center_x = regression_data[self.box_coordinates_offset +
                                       box_offset_x] / self.x_scale * anchor.width + anchor.center_x
            height = regression_data[self.box_coordinates_offset +
                                     box_offset_height] / self.h_scale * anchor.height
            width = regression_data[self.box_coordinates_offset +
                                    box_offset_width] / self.w_scale * anchor.width

            min_x = center_x - 0.5 * width
            min_y = center_y - 0.5 * height
            boxes.append(Rect(min_x, min_y, width, height))

            box_keypoints = []
            for i in range(self.number_of_keypoints):
                keypoint_idx = i * self.number_of_values_per_keypoint + self.keypoints_coordinates_offset
                keypoint_y = regression_data[keypoint_idx + keypoint_offset_y] / self.y_scale * anchor.height + anchor.center_y
                keypoint_x = regression_data[keypoint_idx + keypoint_offset_x] / self.x_scale * anchor.width + anchor.center_x
                box_keypoints.append(Point(keypoint_x, keypoint_y))
            keypoints.append(box_keypoints)
        return boxes, keypoints

    def _decode_score_and_classes(self) -> tuple:
        #classificators_data = self.interpreter.get_tensor(self.classificators_output_index)
        class_scores = self.raw_classes
        
        if self.use_score_clipping_threshold:
            class_scores = np.clip(class_scores, -self.score_clipping_threshold,
                                   self.score_clipping_threshold)
        if self.use_sigmoid_score:
            class_scores = 1.0 / (1.0 + np.exp(-class_scores))
        classes = np.argmax(class_scores, axis=1)
        scores = np.take_along_axis(class_scores,
                                    np.expand_dims(classes, -1), axis=1).squeeze(axis=-1)
        #output = "{:.7f}".format(class_scores)
        #np.set_printoptions(precision=8)
        #np.set_printoptions(suppress=True)
        #print(class_scores)
        return scores, classes

def get_eyes(detection):

    eye_width = int(detection.bounding_box.width / 4)
    eye_height = int(detection.bounding_box.height / 4)

    left_eye_x = int(detection.keypoints[0].x - eye_width / 2)
    left_eye_y = int(detection.keypoints[0].y - eye_height / 2)

    right_eye_x = int(detection.keypoints[1].x - eye_width / 2)
    right_eye_y = int(detection.keypoints[1].y - eye_height / 2)

    left_eye_box = Rect(left_eye_x, left_eye_y, eye_width, eye_height)
    right_eye_box = Rect(right_eye_x, right_eye_y, eye_width, eye_height)

    return left_eye_box, right_eye_box

def crop_rect(frame, rect):
    return frame[rect.y : (rect.y + rect.height), rect.x : (rect.x + rect.width)]

def crop_eyes(frame, detection):
    left_eye, right_eye = get_eyes(detection)

    left_eye_frame = crop_rect(frame, left_eye)
    right_eye_frame = crop_rect(frame, right_eye)

    return left_eye_frame, right_eye_frame

def draw_detections(frame: np.array, detections: list) -> np.array:
	num_detections=0
	for detection in detections:
		num_detections=num_detections+1
		print(f'BOUNDING BOX: {detection.bounding_box}')
		#print(f'WIDTH: {detection.bounding_box.width}')
		#print(f'HEIGHT: {detection.bounding_box.height}')
		#print(f'LEFT EYE: {detection.keypoints[0].x}')
		#left_eye_box, right_eye_box = get_eyes(detection)
		frame = cv2.rectangle(frame, detection.bounding_box, YELLOW_COLOR, 2)
        
	return frame, num_detections

def preprocess(resized_inputs):
    return  resized_inputs/128 - 1.0

def ExtractBBoxes(bboxes, bclasses, bscores, im_width, im_height, threshold):
	bbox = []
	print(bscores)
	for idx in range(len(bboxes)):
		if bclasses[idx] == 1:
			if bscores[idx] >= threshold:
				
				y_min = int(bboxes[idx][0] * im_height)
				x_min = int(bboxes[idx][1] * im_width)
				y_max = int(bboxes[idx][2] * im_height)
				x_max = int(bboxes[idx][3] * im_width)
				class_label = 'person'
				bbox.append([x_min, y_min, x_max, y_max, class_label, float(bscores[idx])])
	return bbox

def decode_bounding_boxes(bboxes, bclasses, im_width, im_height, threshold, nms_threshold):

	decode = BlazeFace_decoder(bboxes,bclasses)
	detections = decode.postprocess(128,128)
	return detections


def main():

	if not os.path.exists(quantization_result_path):
		os.makedirs(quantization_result_path)

	if not os.path.exists(non_quantization_result_path):
		os.makedirs(non_quantization_result_path)

	executer = GraphExecuter(G, qrecs=G.quantization)

	q_dets=0
	dets=0
	for j, file in enumerate(dataset_files):
		name= file.split('/')
		image_name= name[1]
		print(image_name)
		original_img = cv2.imread(file,0)
		image_gray,null = resize_aspect_fit(original_img,128)
		q_image_gray = image_gray.copy()
		image = np.stack((image_gray,image_gray,image_gray),axis=2)
		print("Input Image: "+image_name+" - shape: "+str(image.shape))
		image_q = image
		
		if DUMP_FLOAT:
			print("Float")
			data =[image]
			#Calling nntool float executer on input data
			outputs  = executer.execute(data, qmode=None, silent=True)
			bboxes   = np.concatenate((np.array(outputs[137][0]),np.array(outputs[143][0])))# taking just one dimension
			bclasses = np.concatenate((np.array(outputs[125][0]),np.array(outputs[131][0])))
			#bclasses = np.array(outputs[125][0])

			detections = decode_bounding_boxes(bboxes, bclasses, 128, 128, threshold, nms_threshold)

			frame,numd = draw_detections(image_gray,detections)
			cv2.imwrite(non_quantization_result_path + '/' + image_name, frame )
			dets=dets+numd

		if DUMP_QUANT:
			print("Quantized")
			data   = [image_q]
			#Calling nntool quantized executer on input data
			outputs  = executer.execute(data, qmode=QuantizationMode.all_dequantize(), silent=True)
			bboxes   = np.concatenate((np.array(outputs[137][0]),np.array(outputs[143][0])))# taking just one dimension
			bclasses = np.concatenate((np.array(outputs[125][0]),np.array(outputs[131][0])))

			detections = decode_bounding_boxes(bboxes, bclasses, 128, 128, threshold, nms_threshold)
			q_frame, q_numd = draw_detections(q_image_gray,detections)
			cv2.imwrite(quantization_result_path + '/' + image_name, q_frame )
			q_dets = q_dets+q_numd

		print("")

	print("Detected Float: " + str(dets))
	print("Detected Fixed: " + str(q_dets))
#if __name__ == '__main__':
#    sys.exit(main())
main()
