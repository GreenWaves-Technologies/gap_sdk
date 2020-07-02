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
from graph.types import (ConcatParameters, Conv2DParameters, FcParameters,
                         SoftMaxParameters, ConvFusionParameters, PoolingParameters,
                         ActivationParameters, MatrixAddParameters, ActivationFusion,
                         MatrixMulParameters, GlobalPoolParameters, ConstantInputParameters)

class NamingConvension(ABC):

    def __init__(self, G):
        self.G = G

    @abstractmethod
    def get_node_name(self, node_name, step_idx, params):
        pass

    @abstractmethod
    def get_edge_name(self, node, step_idx, edge_type, edge_order=None):
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
        return "S{}_{}".format(step_idx, gtype.capitalize())

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
        if isinstance(params, ConvFusionParameters):
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
            return "S{}_Act_{}".format(step_idx, params.activation.capitalize())
        if isinstance(params, MatrixAddParameters):
            return "S{}_MatAdd_{}".format(step_idx, str(params.out_dims[0]))
        if isinstance(params, MatrixMulParameters):
            return "S{}_MatMul_{}".format(step_idx, str(params.out_dims[0]))
        if isinstance(params, ConstantInputParameters):
            if params.short_name:
                return "S{}_{}".format(step_idx, params.short_name)
            return "S{}_Op_{}".format(step_idx, node_name)
        if isinstance(params, ActivationFusion):
            nodes = params.contained_nodes()
            if isinstance(nodes[0], MatrixAddParameters):
                return "S{}_MatAdd_{}_{}".format(step_idx, str(nodes[0].out_dims[0]),
                                              nodes[1].activation.capitalize())
            if isinstance(nodes[0], (PoolingParameters)):
                return "S{}_{}Pool_{}_{}".format(step_idx, nodes[0].pool_type.capitalize(),
                                                 nodes[0].filter, nodes[1].activation.capitalize())
            if isinstance(nodes[0], (GlobalPoolParameters)):
                return "S{}_{}Pool_{}_{}".format(step_idx, nodes[0].pool_type.capitalize(),
                                                 nodes[0].out_dims[0], nodes[1].activation.capitalize())
            if isinstance(nodes[0], MatrixMulParameters):
                return "S{}_MatMul_{}_{}".format(step_idx, str(nodes[0].out_dims[0]),
                                              nodes[1].activation.capitalize())
        return "S{}_Op_{}".format(step_idx, node_name)

    def get_edge_name(self, node, step_idx, edge_type, edge_order=None):
        node_name = node.name
        if hasattr(node, 'short_name') and node.short_name:
            ename = "S{}_{}".format(step_idx, node.short_name.capitalize())
            return ename
        if edge_type == "in":
            return node_name.capitalize()
        if edge_type == "out":
            if self.G.num_out_edges(node_name):
                return self.G.out_edges(node_name)[0].to_node.name.capitalize()
            return node_name.capitalize()
        if edge_type == "in_out":
            ename = "S{}_Output".format(step_idx)
            return ename
        assert False, "unknown edge type"
        return None
