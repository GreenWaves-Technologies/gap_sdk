# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from graph.types.base import NNEdge
from graph.types.others import ReshapeParameters
from importer.common.provisional_dim import ProvisionalDim


def check_batchdim(G, x, valid_name):
    x_shape = x[2].shape
    if x_shape[0] is not None:
        if x_shape[0] != 1:
            raise NotImplementedError(
                f'{valid_name} pool is on more than one batch. This is not supported')
        reshape = ReshapeParameters(G.unique_name(f'{valid_name}_reshape'),
                                    old_shape=tuple(x_shape), shape=tuple(x_shape[1:]))
        G.add_edge(NNEdge(from_node=x[0], from_idx=x[1], to_node=reshape))
        x_shape[0] = None
        if len(x) == 3:
            return (reshape, 0, ProvisionalDim(x_shape))
        return (reshape, 0, ProvisionalDim(x_shape), x[3])
    return x
