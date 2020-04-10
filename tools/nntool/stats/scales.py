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
