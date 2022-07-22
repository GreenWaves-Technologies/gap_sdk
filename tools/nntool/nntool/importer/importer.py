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

import re
import typing
from importlib import import_module
from typing import Any, Mapping
from onnx import ModelProto as OnnxModelProto

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph

GRAPH_IMPORTERS = {
    'onnx': {'matches':[r".*\.onnx$"], 'importer_class': 'OnnxImporter', 'path': 'nntool.importer.onnx.onnx', 'model': OnnxModelProto},
    'tflite': {'matches':[r".*\.tflite$"], 'importer_class': 'TFLiteImporter', 'path': 'nntool.importer.tflite2.tflite', 'model': None},
}

class ImportException(Exception):
    pass

def match_importer(filename):
    for _, v in GRAPH_IMPORTERS.items():
        for match in v['matches']:
            if re.search(match, filename):
                return v
    return None

def requires_tensor_file(filename: str) -> bool:
    importer = match_importer(filename)
    if importer is None:
        raise ValueError()
    return importer['loader'] is not None

def create_graph(filename_or_model: Any, graph_format: str = None, opts: Mapping = None) -> 'NNGraph':
    if opts is None:
        opts = {}
    model = None
    if graph_format is not None:
        assert graph_format in GRAPH_IMPORTERS, "Unknown format"
    else:
        for k, v in GRAPH_IMPORTERS.items():
            if v['model'] and isinstance(filename_or_model, v['model']):
                model = filename_or_model
                graph_format = k
                break
            if isinstance(filename_or_model, str):
                stripped_name = filename_or_model.strip()
                for match in v['matches']:
                    if re.search(match, stripped_name, re.IGNORECASE):
                        graph_format = k
                        model = stripped_name
                        break

    if graph_format is None:
        raise ImportException("Graph importer not found")

    importer = GRAPH_IMPORTERS[graph_format]
    importer = getattr(import_module(importer['path']), importer['importer_class'])

    graph = importer().create_graph(model, opts)
    return graph
