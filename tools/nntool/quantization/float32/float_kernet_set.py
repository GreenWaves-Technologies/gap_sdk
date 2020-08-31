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

from quantization.float32.kernels.activations import (hsigmoid, hswish, leaky,
                                                      relu)
from quantization.float32.kernels.fast_conv import faster_conv
from quantization.float32.kernels.image_format import image_format
from quantization.float32.kernels.linear import linear
from quantization.float32.kernels.matrix_operations import matscale, piecewise
from quantization.float32.kernels.pad import pad
from quantization.float32.kernels.pool import (av_global_pool, av_pool, sum_global_pool,
                                               max_global_pool, max_pool)
from quantization.float32.kernels.rnn import rnn
from quantization.float32.kernels.softmax import softmax
from quantization.float32.kernels.ssd_postprocess import ssd_postprocess
from quantization.float32.kernels.tensor_functions import (cast, concat,
                                                           constant_input,
                                                           copy, graph_input,
                                                           graph_output,
                                                           reshape, split,
                                                           strided_slice,
                                                           transpose)
from quantization.kernels.kernel_function import (KernelFunction,
                                                  KernelFunctionSetBase)


class Float32KernelSet(KernelFunctionSetBase):
    @property
    def graph_input(self) -> KernelFunction:
        return graph_input

    @property
    def graph_output(self) -> KernelFunction:
        return graph_output

    @property
    def constant_input(self) -> KernelFunction:
        return constant_input

    @property
    def leaky(self) -> KernelFunction:
        return leaky

    @property
    def hswish(self) -> KernelFunction:
        return hswish

    @property
    def hsigmoid(self) -> KernelFunction:
        return hsigmoid

    @property
    def relu(self) -> KernelFunction:
        return relu

    @property
    def matscale(self) -> KernelFunction:
        return matscale

    @property
    def matadd(self) -> KernelFunction:
        return piecewise

    @property
    def matsub(self) -> KernelFunction:
        return piecewise

    @property
    def matdiv(self) -> KernelFunction:
        return piecewise

    @property
    def matmul(self) -> KernelFunction:
        return piecewise

    @property
    def conv2d(self) -> KernelFunction:
        return faster_conv

    @property
    def linear(self) -> KernelFunction:
        return linear

    @property
    def softmax(self) -> KernelFunction:
        return softmax

    @property
    def reshape(self) -> KernelFunction:
        return reshape

    @property
    def transpose(self) -> KernelFunction:
        return transpose

    @property
    def concat(self) -> KernelFunction:
        return concat

    @property
    def av_pool(self) -> KernelFunction:
        return av_pool

    @property
    def av_global_pool(self) -> KernelFunction:
        return av_global_pool

    @property
    def sum_global_pool(self) -> KernelFunction:
        return sum_global_pool

    @property
    def max_pool(self) -> KernelFunction:
        return max_pool

    @property
    def max_global_pool(self) -> KernelFunction:
        return max_global_pool

    @property
    def pad(self) -> KernelFunction:
        return pad

    @property
    def image_format(self) -> KernelFunction:
        return image_format

    @property
    def ssd_postprocess(self) -> KernelFunction:
        return ssd_postprocess

    @property
    def rnn(self) -> KernelFunction:
        return rnn

    @property
    def strided_slice(self) -> KernelFunction:
        return strided_slice

    @property
    def cast(self) -> KernelFunction:
        return cast

    @property
    def split(self) -> KernelFunction:
        return split

    @property
    def copy(self) -> KernelFunction:
        return copy
