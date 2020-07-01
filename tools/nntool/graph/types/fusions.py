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

import logging
from copy import deepcopy
from ..dim import Dim
from .base import (Parameters, NodeOptions, FilterParameters, SingleInputAndOutput)

LOG = logging.getLogger("nntool." + __name__)

class FusionBase(Parameters):
    fusion_op_name = "!!NOT SET!!"

    def __init__(self, name, fusion_type, subgraph):
        super(FusionBase, self).__init__(name)
        self._subgraph = subgraph
        nodes = self.contained_nodes()
        self.in_dims_hint = nodes[0].in_dims_hint
        self.out_dims_hint = nodes[-1].out_dims_hint
        self.fusion_type = fusion_type

    @property
    def op_name(self):
        return self.fusion_op_name + '_' + self.fusion_type

    @property
    def subgraph(self):
        return self._subgraph

    def contained_nodes(self):
        return [node for node in self.subgraph.dfs()]

    def get_contained_node(self, name):
        return next((n for n in self.contained_nodes() if n.name == name), None)

    @property
    def can_equalize(self):
        return all([param.can_equalize for param in self.contained_nodes()])

    def clone(self, name, groupn=None):
        return self.__class__(name, self.fusion_type, self._subgraph)

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):

        out_dims = in_dims

        for node in self.contained_nodes():
            out_dims = node.get_output_size([out_dim.clone() for out_dim in out_dims])
            node.out_dims = out_dims

        return out_dims

    def __str__(self):
        return "{}".format(", ".join([str(node).strip() for node in self.contained_nodes()]))


class MatScaleFusionParameters(FusionBase):
    fusion_op_name = "matscale"

    def __init__(self, *args, activation=None, **kwargs):
        self.activation = activation
        super(MatScaleFusionParameters, self).__init__(*args, **kwargs)

    def get_output_size(self, in_dims):
        return [Dim.broadcast(in_dims)]

class ConvFusionParameters(FusionBase, SingleInputAndOutput):
    '''Fusion of operators. At present restricted to single input and output but
    this could be removed perhaps'''

    fusion_op_name = "conv_fusion"

    def _init_at_options(self):
        if self._at_options is None:
            self._at_options = NodeOptions(None)
        self._at_options.extend(*[node.at_options for node in self.contained_nodes()])

    @property
    def at_options(self):
        self._init_at_options()
        return self._at_options

    @at_options.setter
    def gen_ctrl(self, val):
        self._init_at_options()
        self._at_options = val

    def contained_filters(self):
        return [x for x in self.contained_nodes() if isinstance(x, FilterParameters)]

    def get_parameter_size(self):
        return sum([node.get_parameter_size() for node in self.contained_nodes()])

    def __str__(self):
        return "{} {}".format(", ".join([str(node).strip() for node in self.contained_nodes()]), self.gen_ctrl or "")

    def compute_load(self):
        return sum([load if load else 0 for load in [node.compute_load()
                                                     for node in self.contained_nodes()]])

class ActivationFusion(FusionBase):
    fusion_op_name = "activation_fusion"
