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

import importlib


from nntool.importer.tflite2.common import TFLiteImportException
from nntool.importer.tflite2.common.tflite_node import TFLiteNode
from nntool.importer.tflite2.tflite_schema_head.CallOnceOptions import \
    CallOnceOptions

from ..backend_handler import BackendHandler
from ..handler import tflite_op, partial_support, ps_description

@tflite_op("CALL_ONCE")
@partial_support(True)
@ps_description('Supported at import time. Must reduce to variable assignments of constants.')
class CallOnce(BackendHandler):

    @classmethod
    def _common(cls, node: TFLiteNode, **kwargs):
        node_opts = node.get_options(CallOnceOptions)
        G = kwargs['G']
        opts = kwargs['opts']
        variables=kwargs['variables']
        importer = kwargs['importer']
        nngraph_class = importlib.import_module('nntool.graph.nngraph').NNGraph
        subgraph = importer.import_tflite_subgraph(
            nngraph_class(name=node.name),
            G.model,
            node_opts.InitSubgraphIndex(),
            variables,
            opts)
        if subgraph:
            raise TFLiteImportException(f'{node.name} CALL_ONCE graph still has unreduced nodes')
        return None

    @classmethod
    def version_1(cls, node: TFLiteNode, **kwargs):
        return cls._common(node, **kwargs)
