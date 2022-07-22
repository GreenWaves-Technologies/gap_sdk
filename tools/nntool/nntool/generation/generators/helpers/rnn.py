# Copyright (C) 2021, 2022  GreenWaves Technologies, SAS

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

from nntool.graph.types.rnn import RNNNodeBase


def num_sequences(node: RNNNodeBase):
    n1 = min(node.n_input_cells, node.n_cells - node.n_output_cells)
    n3 = node.n_cells - max(node.n_input_cells,
                            node.n_cells - node.n_output_cells)
    n2 = node.n_cells - (n1 + n3)
    return sum(1 if n > 0 else 0 for n in [n1, n2, n3])
