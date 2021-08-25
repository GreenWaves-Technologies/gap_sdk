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

from graph.types import GlobalPoolingParameters, NNEdge
from importer.common.provisional_dim import ProvisionalDim


class GlobalPoolMixin(object):

    @classmethod
    def _common(cls, node, pool_type="max", **kwargs):
        all_nodes = kwargs['all_nodes']
        G = kwargs['G']
        valid_name = kwargs['valid_name']
        inputs = [all_nodes[inp] for inp in node.input]
        x = inputs[0]
        x_shape = x[2].shape
        unknown_dims = sum(1 if dim is None else 0 for dim in x_shape)
        params = GlobalPoolingParameters(
            valid_name,
            pool_type=pool_type,
            axis=tuple(range(1, len(x_shape) - unknown_dims)),
            keep_dims=True
        )
        pout_dims = ProvisionalDim([x_shape[0], x_shape[1]] + ([1] * (len(x_shape) - 2)))
        G.add_edge(NNEdge(from_node=x[0], to_node=params, from_idx=x[1], to_idx=0))
        all_nodes[node.output[0]] = (params, 0, pout_dims)
        return params
