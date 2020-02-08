# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import re
from typing import Mapping
from .tflite.new_tflite_graph_all import create_graph as create_tflite_graph
from .tflite.new_tflite_graph_all import NNGraph

# from .darknet.darknet_graph import create_graph as create_darknet_graph
# from .darknet.darknet_tensor import load_tensors as load_dark_net_tensors

GRAPH_IMPORTERS = {
    'tflite': {'matches':[r".*\.tflite$"], 'importer':create_tflite_graph, 'loader': None},
    # 'darknet': {'matches':[r".*\.cfg$"], 'importer':create_darknet_graph,\
    #     'loader': load_dark_net_tensors}
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
                graph = v['importer'](filename, opts)
                graph.set_load_function(v['loader'])
                return graph

    raise ValueError("Graph importer not found")
