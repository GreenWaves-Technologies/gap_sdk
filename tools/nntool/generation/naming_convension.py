# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from abc import ABC, abstractmethod
from graph.types import (ConcatParameters, Conv2DParameters, FcParameters,
                         SoftMaxParameters, FusionParameters, PoolingParameters,
                         ActivationParameters)

class NamingConvension(ABC):

    def __init__(self, G):
        self.G = G

    @abstractmethod
    def get_node_name(self, node_name, step_idx, params):
        pass

    @abstractmethod
    def get_edge_name(self, node_name, step_idx, edge_type, edge_order=None):
        pass

    @abstractmethod
    def get_global_name(self, name, step_idx, params, gtype):
        pass

    @abstractmethod
    def get_project_name(self):
        pass


class DefaultNamingConvension(NamingConvension):

    def get_project_name(self):
        return self.G.name

    def get_global_name(self, name, step_idx, params, gtype):
        return "Step{}{}".format(step_idx, gtype.capitalize())

# pylint: disable=too-many-return-statements
    def get_node_name(self, node_name, step_idx, params):
        if isinstance(params, ConcatParameters):
            return "S{}_Concat".format(step_idx)
        if isinstance(params, Conv2DParameters):
            return "S{}_Conv2d_{}".format(step_idx, str(params.filter))
        if isinstance(params, FcParameters):
            return "S{}_Linear_{}".format(step_idx, str(params.filter))
        if isinstance(params, SoftMaxParameters):
            return "S{}_SoftMax".format(step_idx)
        if isinstance(params, FusionParameters):
            if params.fusion_type == "conv_active_pool":
                return "S{}_Conv2d_{}_{}Pool_{}_{}"\
                    .format(step_idx, params.params[0].filter,
                            params.params[2].pool_type.capitalize(), params.params[2].filter,
                            params.params[1].activation.capitalize())
            if params.fusion_type == "conv_pool_active":
                return "S{}_Conv2d_{}_{}Pool_{}_{}"\
                    .format(step_idx, params.params[0].filter,
                            params.params[1].pool_type.capitalize(), params.params[1].filter,
                            params.params[2].activation.capitalize())
            if params.fusion_type == "conv_active":
                return "S{}_Conv2d_{}_{}"\
                    .format(step_idx, params.params[0].filter,
                            params.params[1].activation.capitalize())
            if params.fusion_type == "conv_pool":
                return "S{}_Conv2d_{}_{}Pool_{}"\
                    .format(step_idx, params.params[0].filter,
                            params.params[1].pool_type.capitalize(), params.params[1].filter)
        if isinstance(params, PoolingParameters):
            return "S{}_{}Pool_{}".format(step_idx, params.pool_type.capitalize(), params.filter)
        if isinstance(params, ActivationParameters):
            return "S{}_{}".format(step_idx, params.activation.capitalize())
        return node_name

    def get_edge_name(self, node_name, step_idx, edge_type, edge_order=None):
        if edge_type == "in":
            return node_name.capitalize()
        if edge_type == "out":
            if self.G.num_out_edges(node_name):
                return self.G.out_edges(node_name)[0].to_node.name.capitalize()
            return node_name.capitalize()
        if edge_type == "in_out":
            ename = "OutputStep{}".format(step_idx)
            return ename
        assert False, "unknown edge type"
        return None
