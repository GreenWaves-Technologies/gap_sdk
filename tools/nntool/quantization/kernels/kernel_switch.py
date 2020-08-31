# Copyright (C) 2020  GreenWaves Technologies, SAS

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

from abc import ABC, abstractmethod
from typing import Sequence

import numpy as np

from graph.types import (CastParameters, ConcatParameters,
                         ConstantInputParameters, Conv2DParameters,
                         CopyParameters, FcParameters, FusionInputParameters,
                         FusionOutputParameters, GlobalPoolParameters,
                         HSigmoidActivationParameters,
                         HSwishActivationParameters, ImageFormatParameters,
                         InputParameters, LeakyActivationParameters,
                         LSTMParameters, MatrixAddParameters,
                         MatrixDivParameters, MatrixMulParameters,
                         MatrixSubParameters, MatScaleFusionParameters,
                         OutputParameters, PadParameters, Parameters,
                         PoolingParameters, ReluActivationParameters,
                         ReshapeParameters, RNNParameters, SoftMaxParameters,
                         SplitParameters, SSDDetectorParameters,
                         StridedSliceParameters, TransposeParameters)
from quantization.quantization_record_base import QuantizationRecordBase


class KernelSwitchBase(ABC):
    @abstractmethod
    def execute(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        pass


class DefaultKernelSwitch(KernelSwitchBase):
    FUNCTION_MAPPING = {
        Conv2DParameters: "conv2d",
        FcParameters: "linear",
        PoolingParameters: "pool_switch",
        InputParameters: "graph_input",
        OutputParameters: "graph_output",
        ReluActivationParameters: "relu",
        LeakyActivationParameters: "leaky",
        HSigmoidActivationParameters: "hsigmoid",
        HSwishActivationParameters: "hswish",
        PadParameters: "pad",
        SoftMaxParameters: "softmax",
        ReshapeParameters: "reshape",
        MatrixAddParameters: "matadd",
        MatrixDivParameters: "matdiv",
        MatrixMulParameters: "matmul",
        MatrixSubParameters: "matsub",
        ConcatParameters: "concat",
        TransposeParameters: "transpose",
        ConstantInputParameters: "constant_input",
        MatScaleFusionParameters: "matscale",
        GlobalPoolParameters: "globalpool_switch",
        ImageFormatParameters: "image_format",
        SSDDetectorParameters: "ssd_postprocess",
        LSTMParameters: "rnn",
        RNNParameters: "rnn",
        StridedSliceParameters: "strided_slice",
        CastParameters: "cast",
        SplitParameters: "split",
        CopyParameters: "copy",
        FusionInputParameters: "fusion_noop",
        FusionOutputParameters: "fusion_noop"
    }

    def __init__(self, kernel_functions):
        super(DefaultKernelSwitch, self).__init__()
        self._kernel_functions = kernel_functions

    def execute(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        if params.__class__ in self.FUNCTION_MAPPING:
            func = self.FUNCTION_MAPPING[params.__class__]
            if hasattr(self, func):
                return getattr(self, func)(params, input_tensors, qrec, details=details)
            if hasattr(self._kernel_functions, func):
                return getattr(self._kernel_functions, func)(params, input_tensors,
                                                             qrec, details=details)
            raise NotImplementedError("Implementation for %s not found" % func)
        raise NotImplementedError("Unknown parameter type %s" % params.__class__.__name__)

    def pool_switch(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                    qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        if params.pool_type == "average":
            return self._kernel_functions.av_pool(params, input_tensors, qrec, details=details)
        if params.pool_type == "max":
            return self._kernel_functions.max_pool(params, input_tensors, qrec, details=details)
        raise NotImplementedError("unknown pool type %s" % params.pool_type)

    def globalpool_switch(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                          qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        if params.pool_type == "average":
            return self._kernel_functions.av_global_pool(params, input_tensors, qrec, details=details)
        if params.pool_type == "max":
            return self._kernel_functions.max_global_pool(params, input_tensors, qrec, details=details)
        if params.pool_type == "sum":
            return self._kernel_functions.sum_global_pool(params, input_tensors, qrec, details=details)
        raise NotImplementedError("unknown pool type %s" % params.pool_type)

    def fusion_noop(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                     qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        return input_tensors
