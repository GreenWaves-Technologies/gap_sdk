# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import numpy as np

from graph.types import Conv2DParameters, FcParameters

class Ranges():
    @staticmethod
    def get_ranges(maxes):
        max_range = max(maxes) * 2
        ranges = [i * 2 for i in maxes]
        return (ranges, max_range)

    @classmethod
    def conv2d_range_input(cls, node, weights=None):
        if weights is None:
            weights = node.weights
        maxes = [None] * node.in_dims[0].c
        if node.tf_depthwise:
            for in_c in range(node.in_dims[0].c):
                in_c_w = weights[node.filter.srange(\
                    out_c=[in_c * node.multiplier, node.multiplier, 1])]
                maxes[in_c] = np.amax(np.abs(in_c_w))
        else:
            assert node.groups == 1,\
                "this needs checking for grouped convs"
            for in_c in range(node.in_dims[0].c):
                in_c_w = weights[node.filter.srange(in_c=in_c)]
                maxes[in_c] = np.amax(np.abs(in_c_w))

        return cls.get_ranges(maxes)

    @classmethod
    def conv2d_range_output(cls, node, weights=None):
        if weights is None:
            weights = node.weights
        maxes = [None] * node.filter.out_c
        assert node.groups == 1 or node.tf_depthwise,\
            "this needs checking for grouped convs"
        for out_c in range(node.filter.out_c):
            out_c_w = weights[node.filter.srange(out_c=out_c)]
            maxes[out_c] = np.amax(np.abs(out_c_w))

        return cls.get_ranges(maxes)

    @classmethod
    def linear_range_input(cls, node, weights=None):
        if weights is None:
            weights = node.weights
        in_c_weights = weights.reshape(node.filter.shape)
        maxes = [np.max(np.abs(in_c_weights[node.filter.srange(in_c=i)]))
                 for i in range(node.in_dims[0].c)]

        return cls.get_ranges(maxes)

    @classmethod
    def linear_range_output(cls, node, weights=None):
        if weights is None:
            weights = node.weights
        filt = node.filter.get_filter_dims()
        maxes = [np.max(np.abs(weights[node.filter.srange(out_c=i)]))
                 for i in range(filt.out_c)]

        return cls.get_ranges(maxes)

    @classmethod
    def range_input(cls, node, weights=None):
        if isinstance(node, Conv2DParameters):
            return cls.conv2d_range_input(node, weights)
        if isinstance(node, FcParameters):
            return cls.linear_range_input(node, weights)
        raise ValueError()

    @classmethod
    def range_output(cls, node, weights=None):
        if isinstance(node, Conv2DParameters):
            return cls.conv2d_range_output(node, weights)
        if isinstance(node, FcParameters):
            return cls.linear_range_output(node, weights)
        raise ValueError()
