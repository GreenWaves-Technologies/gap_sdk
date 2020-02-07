# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from graph.types import Conv2DParameters, FcParameters

class Scales():

    @staticmethod
    def conv2d_scale_output(node, scale, weights=None, biases=None):
        if weights is None:
            weights = node.weights
        if biases is None and node.has_bias:
            biases = node.biases
        assert node.groups == 1 or node.tf_depthwise,\
            "this needs checking for grouped convs"
        for out_c in range(node.filter.out_c):
            weights[node.filter.srange(out_c=out_c)] /= scale[out_c]
            if biases is not None:
                biases[out_c] /= scale[out_c]
        return weights, biases

    @staticmethod
    def conv2d_scale_input(node, scale, weights=None):
        if weights is None:
            weights = node.weights
        if node.tf_depthwise:
            for in_c in range(node.in_dims[0].c):
                in_c_start = in_c * node.multiplier
                weights[node.filter.srange(out_c=\
                    [in_c_start, in_c_start + node.multiplier, 1])] *= scale[in_c]
        else:
            assert node.groups == 1,\
                "this needs checking for grouped convs"
            for in_c in range(node.in_dims[0].c):
                weights[node.filter.srange(in_c=in_c)] *= scale[in_c]
        return weights

    @staticmethod
    def linear_scale_output(node, scale, weights=None, biases=None):
        if weights is None:
            weights = node.weights
        if biases is None and node.has_bias:
            biases = node.biases
        filt = node.filter.get_filter_dims()
        for out_c in range(filt.out_c):
            weights[node.filter.srange(out_c=out_c)] /= scale[out_c]
            if biases is not None:
                biases[out_c] /= scale[out_c]

        return weights, biases

    @staticmethod
    def linear_scale_input(node, scale, weights=None):
        if weights is None:
            weights = node.weights
        in_c_weights = weights.reshape(node.filter)
        for in_c in range(node.in_dims[0].c):
            in_c_weights[node.filter.srange(in_c=in_c, use_order=node.filter.order)] *= scale[in_c]
        return weights

    @classmethod
    def scale_input(cls, node, scale, weights=None):
        if isinstance(node, Conv2DParameters):
            return cls.conv2d_scale_input(node, scale, weights)
        if isinstance(node, FcParameters):
            return cls.linear_scale_input(node, scale, weights)
        raise ValueError()

    @classmethod
    def scale_output(cls, node, scale, weights=None, biases=None):
        if isinstance(node, Conv2DParameters):
            return cls.conv2d_scale_output(node, scale, weights, biases)
        if isinstance(node, FcParameters):
            return cls.linear_scale_output(node, scale, weights, biases)
        raise ValueError()
