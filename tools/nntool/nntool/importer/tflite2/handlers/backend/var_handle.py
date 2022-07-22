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

from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.tflite_schema_head.VarHandleOptions import \
    VarHandleOptions
from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description


@tflite_op("VAR_HANDLE")
@partial_support(True)
@ps_description('Supported at import. Variables are folded into the graph. Values must be constants.')
class VarHandle(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        all_nodes = kwargs['all_nodes']
        node_opts = node.get_options(VarHandleOptions)
        container = node_opts.Container()
        shared_name = node_opts.SharedName()
        all_nodes[node.output[0]] = ((container, shared_name), 0, None)
        return None

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
