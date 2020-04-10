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
from typing import Mapping
from .tflite.new_tflite_graph_all import TfliteImporter, NNGraph

GRAPH_IMPORTERS = {
    'tflite': {'matches':[r".*\.tflite$"], 'importer':TfliteImporter, 'loader': None},
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

def create_graph(filename: str, graph_format: str = None, opts: Mapping = None) -> NNGraph:
    if opts is None:
        opts = {}
    if graph_format is not None:
        assert graph_format in GRAPH_IMPORTERS, "Unknown format"
        return GRAPH_IMPORTERS[graph_format].importer(filename, opts)

    for k, v in GRAPH_IMPORTERS.items():
        for match in v['matches']:
            if re.search(match, filename):
                importer = v['importer']()
                graph = importer.create_graph(filename, opts)
                graph.set_load_function(v['loader'])
                return graph

    raise ValueError("Graph importer not found")
