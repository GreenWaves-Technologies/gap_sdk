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
import hashlib

from graph.types import (ActivationFusion, ActivationParameters,
                         ConcatParameters, ConstantInputParameters,
                         Conv2DParameters, ConvFusionParameters, FcParameters,
                         GlobalPoolingParameters, MatrixAddParameters,
                         MatrixMulParameters, PoolingParameters,
                         SoftMaxParameters)


class NamingConvension(ABC):

    def __init__(self, G):
        self.G = G
        self.multi_out_edges = {}

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

    def __init__(self, *args, anonymise=False, **kwargs):
        super(DefaultNamingConvension, self).__init__(*args, **kwargs)
        self._anonymise = anonymise
        self._name_cache = set()

    def _get_step_name(self, name, step_idx):
        if self._anonymise:
            return f'S{step_idx}_{hashlib.sha1(name.encode("UTF-8")).hexdigest()[:10]}'
        return f'S{step_idx}_{name}'

    def get_project_name(self):
        return self.G.name

    def get_global_name(self, name, step_idx, params, gtype):
        return self._get_step_name(gtype.capitalize(), step_idx)

    def get_node_name(self, node_name, step_idx, params):
        return self._get_step_name(self.__get_node_name(node_name, params), step_idx)

# pylint: disable=too-many-return-statements
    def __get_node_name(self, node_name, params):
        if isinstance(params, ConcatParameters):
            return "Concat"
        if isinstance(params, Conv2DParameters):
            return f"Conv2d_{params.filter}"
        if isinstance(params, FcParameters):
            return f"Linear_{params.filter}"
        if isinstance(params, SoftMaxParameters):
            return "SoftMax"
        if isinstance(params, ConvFusionParameters):
            nodes = params.contained_nodes()
            if params.fusion_type == "conv_active_pool":
                return (f"Conv2d_{nodes[0].filter}_{nodes[2].pool_type.capitalize()}"
                        f"Pool_{nodes[2].filter}_{nodes[1].activation.capitalize()}")
            if params.fusion_type == "conv_pool_active":
                return (f"Conv2d_{nodes[0].filter}_{nodes[1].pool_type.capitalize()}"
                        f"Pool_{nodes[1].filter}_{nodes[2].activation.capitalize()}")
            if params.fusion_type == "conv_active":
                return f"Conv2d_{nodes[0].filter}_{nodes[1].activation.capitalize()}"
            if params.fusion_type == "conv_pool":
                return f"Conv2d_{nodes[0].filter}_{nodes[1].pool_type.capitalize()}Pool_{nodes[1].filter}"
        if isinstance(params, PoolingParameters):
            return f"{params.pool_type.capitalize()}Pool_{params.filter}"
        if isinstance(params, ActivationParameters):
            return f"Act_{params.activation.capitalize()}"
        if isinstance(params, MatrixAddParameters):
            return f"MatAdd_{params.out_dims[0]}"
        if isinstance(params, MatrixMulParameters):
            return f"MatMul_{params.out_dims[0]}"
        if isinstance(params, ConstantInputParameters):
            if params.short_name:
                return params.short_name
            return f"Op_{node_name}"
        if isinstance(params, ActivationFusion):
            nodes = params.contained_nodes()
            if isinstance(nodes[0], MatrixAddParameters):
                return f"MatAdd_{nodes[0].out_dims[0]}_{nodes[1].activation.capitalize()}"
            if isinstance(nodes[0], (PoolingParameters)):
                return f"{nodes[0].pool_type.capitalize()}Pool_{nodes[0].filter}_{nodes[1].activation.capitalize()}"
            if isinstance(nodes[0], (GlobalPoolingParameters)):
                return (f"{nodes[0].pool_type.capitalize()}Pool_{nodes[0].out_dims[0]}_"
                        f"{nodes[1].activation.capitalize()}")
            if isinstance(nodes[0], MatrixMulParameters):
                return f"MatMul_{nodes[0].out_dims[0]}_{nodes[1].activation.capitalize()}"
        return f"Op_{node_name}"

    def get_edge_name(self, node, step_idx, edge_type, edge_order=None):
        node_name = node.name
        if hasattr(node, 'short_name') and node.short_name:
            return self._get_step_name(node.short_name.capitalize(), step_idx)
        if edge_type == "in":
            return f"{node_name.capitalize()}"
        if edge_type == "out":
            if self.G.num_out_edges(node_name):
                return self.G.out_edges(node_name)[0].to_node.name.capitalize()
            return node_name.capitalize()
        if edge_type == "in_out":
            ename = "S{}_Output".format(step_idx)
            if edge_order is not None:
                return ename + "_{}".format(edge_order)
            return ename
        raise ValueError("unknown edge type")
