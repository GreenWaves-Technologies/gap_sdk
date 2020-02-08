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
            nodes = params.contained_nodes()
            if params.fusion_type == "conv_active_pool":
                return "S{}_Conv2d_{}_{}Pool_{}_{}"\
                    .format(step_idx, nodes[0].filter,
                            nodes[2].pool_type.capitalize(), nodes[2].filter,
                            nodes[1].activation.capitalize())
            if params.fusion_type == "conv_pool_active":
                return "S{}_Conv2d_{}_{}Pool_{}_{}"\
                    .format(step_idx, nodes[0].filter,
                            nodes[1].pool_type.capitalize(), nodes[1].filter,
                            nodes[2].activation.capitalize())
            if params.fusion_type == "conv_active":
                return "S{}_Conv2d_{}_{}"\
                    .format(step_idx, nodes[0].filter,
                            nodes[1].activation.capitalize())
            if params.fusion_type == "conv_pool":
                return "S{}_Conv2d_{}_{}Pool_{}"\
                    .format(step_idx, nodes[0].filter,
                            nodes[1].pool_type.capitalize(), nodes[1].filter)
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
