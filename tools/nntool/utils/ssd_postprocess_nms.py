import copy
import json
import os
from typing import NamedTuple
import numpy as np

class ValException(Exception):
    pass

class Detection(NamedTuple):
    bboxes: np.array
    scores: np.array
    classes: np.array

class NonMaxSuppression(object):
    '''
    Parameters that should be initialized:

    - max_detections_per_class
    - max_classes_per_detection
    - max_total_detections
    - iou_threshold
    - score_threshold
    - regular_nms

    '''

    def __init__(self, nms_configuration, use_regular_nms=True):
        # pylint: disable=no-member,bare-except
        '''
        # default values of parameters required for nms:
        max_classes_per_detection: Maximum number of classes to output per detection box.
        detections_per_class: Number of anchors used per class in Regular Non-Max-Suppression.
        use_regular_nms: 'Flag to set postprocessing op to use Regular NMS instead of Fast NMS.'
        '''
        self._max_detections_per_class = 10
        self._max_classes_per_detection = 1
        self._max_total_detections = 100
        self._iou_threshold = 0.3
        self._score_threshold = 0.5
        self._num_classes = None

        self.nms_configuration = nms_configuration

        # selection nms method: fast or regular?
        if use_regular_nms:
            self.nms_core = self._regular_nms
        else:
            self.nms_core = self._fast_nms

        # all allowed keys will be initialized as class attributes
        allowed_keys = set(
            ['using_json_config', 'using_pipeline_config', 'using_params'])
        # initialize all allowed keys to false
        self.__dict__.update((key, None) for key in allowed_keys)
        # and update the given keys by their given values
        self.__dict__.update(
            (key, value) for key, value in nms_configuration.items() if key in allowed_keys)

        'loading params values'
        if self.using_json_config['INCLUDE']:
            json_config_path = self.using_json_config['json_config_path']
            if not os.path.isfile(json_config_path):
                raise ValException(
                    '\n\n the path to the json file is not valid.')
            with open(json_config_path) as json_file:
                json_data = json.load(json_file)

            try:
                self.max_detections_per_class = json_data['detections_per_class']
            except:
                self.max_detections_per_class = self._max_detections_per_class
                print('Warning: deafutl assignement for max_detections_per_class: {value} .'
                      .format(value=self._max_detections_per_class))
            try:
                self.max_classes_per_detection = json_data['max_classes_per_detection']
            except:
                self.max_classes_per_detection = self._max_classes_per_detection
                print('Warning: deafutl assignement for max_classes_per_detection: {value} .'
                      .format(value=self._max_classes_per_detection))
            try:
                self.max_total_detections = json_data['max_detections']
            except:
                self.max_total_detections = self._max_total_detections
                print('Warning: deafutl assignement for max_total_detections: {value} .'
                      .format(value=self._max_total_detections))
            try:
                self.iou_threshold = json_data['nms_iou_threshold']
            except:
                self.iou_threshold = self._iou_threshold
                print('Warning: deafutl assignement for iou_threshold: {value} .'
                      .format(value=self._iou_threshold))
            try:
                self.score_threshold = json_data['nms_score_threshold']
            except:
                self.score_threshold = self._score_threshold
                print('Warning: deafutl assignement for score_threshold: {value} .'
                      .format(value=self._score_threshold))
            try:
                self.num_classes = json_data['num_classes']
            except:
                self.num_classes = self._self.num_classes
                print('Warning: deafutl assignement for num_classes: {value} .'
                      .format(value=self._self.num_classes))
            self._output_quantized = json_data['_output_quantized']
            self._support_output_type_float_in_quantized_op = json_data[
                '_support_output_type_float_in_quantized_op']

        elif self.using_pipeline_config['INCLUDE']:
            raise ValException('Need to import object_detection from tf')

        elif self.using_params['INCLUDE']:
            params = self.using_params['params']
            # max_detection_per_class is left default
            self.max_detections_per_class = self._max_detections_per_class
            if isinstance(params['max_classes_per_detection'], int) and params['max_classes_per_detection'] > 0:
                self.max_classes_per_detection = params['max_classes_per_detection']
            else:
                raise ValException('\n\n no valid max_classes_per_detection is assigned, shoulde be postive integer ...')
            if isinstance(params['max_detections'], int) and params['max_detections'] > 0:
                self.max_total_detections = params['max_detections']
            else:
                raise ValException('\n\n no valid max_total_detections is assigned, shoulde be postive float/integer ...')
            if isinstance(params['nms_iou_threshold'], float) and params['nms_iou_threshold'] > 0 and \
                                                                  params['nms_iou_threshold'] < 1.:
                self.iou_threshold = params['nms_iou_threshold']
            else:
                raise ValException('\n\n no valid iou_threshold is assigned, shoulde be float in range of (0.,1.).')
            if isinstance(params['nms_score_threshold'], float) and params['nms_score_threshold'] > 0 and  \
                                                                    params['nms_score_threshold'] < 1.:
                self.score_threshold = params['nms_score_threshold']
            else:
                raise ValException('\n\n no valid score_threshold is assigned, shoulde be float in range of (0.,1.).')
            if isinstance(params['num_classes'], int) and params['num_classes'] > 0:
                self.num_classes = params['num_classes']
            else:
                raise ValException('\n\n no valid num_classes is assigned, shoulde be postive integer ...')
        else:
            raise ValException(
                '\n\n no method is chosen to assigne non-max-suppression ...')

    def calculate_iou(self, base_bbox, rest_bboxes):
        'calculate iou between a base bounding box and an array of bboxes'
        ymin, xmin, ymax, xmax = 0, 1, 2, 3
        # base anchor
        base_bbox_cor = copy.deepcopy(base_bbox)
        base_bbox_area = (base_bbox_cor[ymax] - base_bbox_cor[ymin]).astype(np.int32) *\
            (base_bbox_cor[xmax] - base_bbox_cor[xmin]).astype(np.int32)
        # an array of anchors
        rest_bboxes_cor = copy.deepcopy(rest_bboxes)
        # rest anchors area
        rest_bboxes_area = (rest_bboxes_cor[:, ymax] - rest_bboxes_cor[:, ymin]) *\
            (rest_bboxes_cor[:, xmax] - rest_bboxes_cor[:, xmin])
        # inter section coordinates
        inter_ymin = np.maximum(rest_bboxes_cor[:, ymin], base_bbox_cor[ymin])
        inter_xmin = np.maximum(rest_bboxes_cor[:, xmin], base_bbox_cor[xmin])
        inter_ymax = np.minimum(rest_bboxes_cor[:, ymax], base_bbox_cor[ymax])
        inter_xmax = np.minimum(rest_bboxes_cor[:, xmax], base_bbox_cor[xmax])
        # TODO: do we need to keep pixel border horizons???
        # inter section width and height
        inter_w = np.maximum(inter_xmax - inter_xmin, 0)
        inter_h = np.maximum(inter_ymax - inter_ymin, 0)
        inter_area = inter_w * inter_h
        iou = inter_area / (rest_bboxes_area + base_bbox_area - inter_area)
        return iou

    def greedy_non_maximum_suppression(self, bboxes, confidences):
        '''
        This function performs greedy non maximum suppression algorithm on a dataframe of anchors.

        step 1) keep anchors with a confidence more than the assigned confidence threshold.  
        step 2) sort derived anchors from step 1 according to their confidences.
        step 3) apply greedy nms over the sorted anchors by evaluation of ious.
        step 4) return a dataframe including the anchors, their confidences and class_id

        '''
        # default anchors indices before appplying any sort
        bboxes_indices = np.arange(bboxes.shape[0])

        # apply confidence threshold
        valid_confs_indices = bboxes_indices[confidences >=
                                             self.score_threshold]
        valid_confs = confidences[valid_confs_indices]

        # sort the confidences freater than the confidence threshold
        args = np.argsort(valid_confs)[::-1]  # sort the confidences and
        inds = valid_confs_indices[args]     # back to the original indices

        # greedy search
        kept_indices = []
        while inds.size:
            base_ind = inds[0]
            # print('base_ind_confidence: ', confidences[base_ind])
            rest_inds = inds[1:]
            # lets store the base anchor in the bank
            kept_indices.append(base_ind)

            base_bbox = copy.deepcopy(bboxes[base_ind, :])
            rest_bboxes = copy.deepcopy(bboxes[rest_inds, :])
            iou = self.calculate_iou(base_bbox, rest_bboxes)

            # print('before: rest_inds', rest_inds.shape)
            # lets keep bboxes having iou less than iou_threshold for the next itteration evaluation
            inds = rest_inds[iou < self.iou_threshold]
            # print('after: rest_inds', rest_inds.shape)
        # return the valid anchors
        valid_bboxes = bboxes[kept_indices, :]
        valid_bboxes_confidence = confidences[kept_indices]
        # to know how many classes are predicted for within each anchor: max_classes_per_detection
        valid_anchors_inds = kept_indices
        return valid_bboxes, valid_bboxes_confidence, np.array(valid_anchors_inds)

    def _regular_nms(self, bboxes, confidences):
        # num classes
        num_classes = confidences.shape[1]

        # predictions
        predicted_bboxes = []  # bboxes coordinates
        predicted_confidences = []  # bboxes confidences
        predicted_class_ids = []  # to which class the bbox belongs to?
        predicted_anchors_indides = []  # the bbox indices in the reference anchor array

        # loop over classes apart from the background
        for class_id in range(1, num_classes):

            # a one-dimentionsal array, including confidences for the class_id
            class_confidences = confidences[:, class_id]
            # a copy of transformed anchor boxes
            class_bboxes = copy.deepcopy(bboxes)

            # greedy nsm over the anchors, the output df has two new columns: confidence and class_id
            valid_class_bboxes, valid_class_confidences, valid_anchors_indices = self.greedy_non_maximum_suppression(
                class_bboxes, class_confidences)

            # if there are detections
            if len(valid_class_bboxes) > 0:
                num_detections = valid_class_bboxes.shape[0]

                'apply <max_detections_per_class>'
                to_keep = min(num_detections, self.max_detections_per_class)
                # add decoded anchors to the bank
                predicted_bboxes.append(valid_class_bboxes[:to_keep, :])
                predicted_confidences.append(valid_class_confidences[:to_keep])
                valid_class_ids = np.zeros_like(
                    valid_class_confidences[:to_keep])+class_id
                predicted_class_ids.append(valid_class_ids)
                predicted_anchors_indides.append(
                    valid_anchors_indices[:to_keep])

        'now concatenate predictions for all classes.'
        predicted_bboxes = np.concatenate(predicted_bboxes)
        predicted_confidences = np.concatenate(predicted_confidences)
        predicted_class_ids = np.concatenate(predicted_class_ids)
        predicted_anchors_indides = np.concatenate(predicted_anchors_indides)

        'apply <max_classes_per_detection>'
        # sort: descending order
        sorted_confidences_args = np.argsort(-predicted_confidences, axis=0)
        sorted_detections_bboxes = predicted_bboxes[sorted_confidences_args, :]
        sorted_detections_confidences = predicted_confidences[sorted_confidences_args]
        sorted_detections_class_ids = predicted_class_ids[sorted_confidences_args]
        sorted_detections_anchors_indides = predicted_anchors_indides[sorted_confidences_args]

        're-write detections accordingly'
        predicted_bboxes = []
        predicted_confidences = []
        predicted_class_ids = []
        predicted_anchors_indides = []
        anchorS_indices = np.unique(sorted_detections_anchors_indides)
        # loop over anchors
        for i, ind in enumerate(anchorS_indices):
            anchor_indices = sorted_detections_anchors_indides == ind
            temp_detected_bboxes = sorted_detections_bboxes[anchor_indices, :]
            temp_detected_confidences = sorted_detections_confidences[anchor_indices]
            temp_detected_class_ids = sorted_detections_class_ids[anchor_indices]
            # all values must be equal to the <ind>
            temp_detected_anchors_indides = sorted_detections_anchors_indides[anchor_indices]

            # apply max_classes_per_detection:
            limited_number = min(
                len(temp_detected_anchors_indides), self.max_classes_per_detection)
            predicted_bboxes.append(temp_detected_bboxes[:limited_number, :])
            predicted_confidences.append(
                temp_detected_confidences[:limited_number])
            predicted_class_ids.append(
                temp_detected_class_ids[:limited_number])
            predicted_anchors_indides.append(
                temp_detected_anchors_indides[:limited_number])

        'now concatenate predictions for all anchors.'
        predicted_bboxes = np.concatenate(predicted_bboxes)
        predicted_confidences = np.concatenate(predicted_confidences)
        predicted_class_ids = np.concatenate(predicted_class_ids)
        predicted_anchors_indides = np.concatenate(predicted_anchors_indides)

        'apply <max_total_detections>'
        max_confidences_args = np.argsort(predicted_confidences, axis=0)
        total_detections_confidences = predicted_confidences[
            max_confidences_args[-self.max_total_detections:]]
        total_detections_bboxes = predicted_bboxes[max_confidences_args[-self.max_total_detections:], :]
        total_detections_class_ids = predicted_class_ids[
            max_confidences_args[-self.max_total_detections:]]
        #total_detections_anchors_indides = predicted_anchors_indides[max_confidences_args[-self.max_total_detections:]]
        # return total_detections_bboxes, total_detections_confidences, total_detections_class_ids, total_detections_anchors_indides
        return total_detections_bboxes, total_detections_confidences, total_detections_class_ids

    def fast_greedy_non_maximum_suppression(self, bboxes, confidences, labels_score, labels, anchors_indices):
        '''
        applying greedy_non_maximum_suppression in a fast way!!!

        '''
        # default anchors indices before appplying any sort
        bboxes_indices = np.arange(bboxes.shape[0])
        # sort the confidences freater than the confidence threshold
        args = np.argsort(labels_score)[::-1]  # sort the confidences and
        inds = bboxes_indices[args]     # back to the original indices
        # greedy search
        kept_indices = []
        while inds.size:
            base_ind = inds[0]
            rest_inds = inds[1:]
            # lets store the base anchor in the bank
            kept_indices.append(base_ind)
            base_bbox = copy.deepcopy(bboxes[base_ind, :])
            rest_bboxes = copy.deepcopy(bboxes[rest_inds, :])
            iou = self.calculate_iou(base_bbox, rest_bboxes)
            # lets keep bboxes having iou less than iou_threshold for the next itteration evaluation
            inds = rest_inds[iou < self.iou_threshold]
        # return the valid anchors
        valid_bboxes = bboxes[kept_indices, :]
        valid_confidences = confidences[kept_indices, :]
        valid_labels_score = labels_score[kept_indices]
        valid_labels_labels = labels[kept_indices]
        valid_anchors_indices = anchors_indices[kept_indices]
        return valid_bboxes, valid_confidences, valid_labels_score, valid_labels_labels, valid_anchors_indices

    def _fast_nms(self, bboxes, confidences):
        # num classes
        num_classes = confidences.shape[1]
        anchors_indices = np.arange(bboxes.shape[0])
        # predictions
        predicted_bboxes = []  # bboxes coordinates
        predicted_labels = []  # bboxes confidences
        predicted_labels_score = []  # to which class the bbox belongs to?
        predicted_anchor_indices = []  # anchor indices, for debugging

        # convert one-hot labels into ordinal labels with their confidences
        labels = np.argmax(confidences, axis=1)  # the ordinal labels
        labels_score = np.amax(confidences, axis=1)  # the scores of labels

        # non-background labels
        non_background_indices = labels != 0

        # non-background (nb) bboxes
        nb_bboxes = bboxes[non_background_indices, :]          # bboxes
        nb_confidences = confidences[non_background_indices]   # one-hot scores
        # labels with highest confidences
        nb_labels = labels[non_background_indices]
        # the highest confidences
        nb_labels_score = labels_score[non_background_indices]
        # anchors indices
        nb_anchors_indices = anchors_indices[non_background_indices]

        # non-background (nb) bboxes that have scores/confidences higher than score_threshold
        valid_nb_bboxes_inidces = nb_labels_score >= self.score_threshold
        # bboxes
        valid_nb_bboxes = nb_bboxes[valid_nb_bboxes_inidces, :]
        # one-hot scores
        valid_nb_confidences = nb_confidences[valid_nb_bboxes_inidces]
        # labels with highest confidences
        valid_nb_labels = nb_labels[valid_nb_bboxes_inidces]
        # the highest confidences
        valid_nb_labels_score = nb_labels_score[valid_nb_bboxes_inidces]
        valid_nb_anchors_indices = nb_anchors_indices[valid_nb_bboxes_inidces]

        # for a given iou threshod, evaluate iou between the bboxes
        if not self.iou_threshold is None:
            valid_nb_bboxes, valid_nb_confidences, valid_nb_labels_score, valid_nb_labels, valid_nb_anchor_indices =\
                self.fast_greedy_non_maximum_suppression(
                    valid_nb_bboxes, valid_nb_confidences, valid_nb_labels_score, valid_nb_labels, valid_nb_anchors_indices)

        'apply <max_detections_per_class>'
        for class_id in range(1, num_classes):
            # select class information
            class_indices = valid_nb_labels == class_id
            if np.sum(class_indices > 0):  # be sure that at least one bbox does exist
                temp_bboxes = valid_nb_bboxes[class_indices, :]
                temp_labels = valid_nb_labels[class_indices]
                temp_labels_score = valid_nb_labels_score[class_indices]
                temp_anchor_indices = valid_nb_anchor_indices[class_indices]
                # sort according to the scores
                max_args = np.argsort(-temp_labels_score, axis=0)
                temp_bboxes = temp_bboxes[max_args, :]
                temp_labels = temp_labels[max_args]
                temp_labels_score = temp_labels_score[max_args]
                temp_anchor_indices = temp_anchor_indices[max_args]

                to_keep = min(len(temp_labels), self.max_detections_per_class)
                predicted_bboxes.append(temp_bboxes[:to_keep, :])
                predicted_labels.append(temp_labels[:to_keep])
                predicted_labels_score.append(temp_labels_score[:to_keep])
                predicted_anchor_indices.append(temp_anchor_indices[:to_keep])
        'now concatenate predictions for all classes.'
        predicted_bboxes = np.concatenate(predicted_bboxes)
        predicted_labels = np.concatenate(predicted_labels)
        predicted_labels_score = np.concatenate(predicted_labels_score)
        predicted_anchor_indices = np.concatenate(predicted_anchor_indices)
        'apply <max_total_detections>'
        to_keep = min(len(predicted_labels), self.max_total_detections)
        max_args = np.argsort(-predicted_labels_score, axis=0)
        predicted_bboxes = predicted_bboxes[max_args[:to_keep], :]
        predicted_labels = predicted_labels[max_args[:to_keep]]
        predicted_labels_score = predicted_labels_score[max_args[:to_keep]]
        predicted_anchor_indices = predicted_anchor_indices[max_args[:to_keep]]
        return predicted_bboxes, predicted_labels_score, predicted_labels

    def __call__(self, decoded_bboxes, scores):
        '''
        use nms core to decode predictions

        decoded_bboxes: [batch_size, number_anchors, 4] 
        scores: [batch_size, number_anchors, number of classes] 

        '''

        # add batch dimension, if not included
        if decoded_bboxes.ndim == 2:
            decoded_bboxes = decoded_bboxes.reshape([1, -1, decoded_bboxes.shape[1]])
        if scores.ndim == 2:
            scores = scores.reshape([1, -1, scores.shape[1]])
        # for all batch items
        detections = [[]] * decoded_bboxes.shape[0]
        for i in range(len(detections)):
            bboxes, scores, classes = self.nms_core(decoded_bboxes[i, :, :], scores[i, :, :])
            detections[i] = Detection(bboxes, scores, classes)
        return detections
