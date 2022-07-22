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

from nntool.importer.tflite2.tflite_schema_head.SplitOptions import SplitOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op
from .split_mixin import SplitMixin


@tflite_op("SPLIT")
class Split(SplitMixin, BackendHandler):

    @classmethod
    def _common(cls, node, **kwargs):
        node_opts = node.get_options(SplitOptions)
        all_nodes = kwargs['all_nodes']
        inputs = [all_nodes[t] for t in node.input]
        # yes, for this one operator the axis is on input[0] and the input on input[1]
        axis = int(cls._verify_constant(inputs[0]))
        node.input[0].used = True
        return super(Split, cls)._common(
            node,
            axis=axis,
            num_splits=node_opts.NumSplits(),
            input_idx=1,
            **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls._common(node, **kwargs)
