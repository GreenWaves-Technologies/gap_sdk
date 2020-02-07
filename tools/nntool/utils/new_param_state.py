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

import json
import logging
import os
import pickle
from pathlib import Path

from graph.graph_identity import GraphIdentity
from graph.manipulations.extract import extract_node
from graph.matches.matches import get_fusion
from graph.nngraph import NNGraph
from importer.importer import create_graph
from utils.json_serializable import (JsonSerializableStateDecoder,
                                     JsonSerializableStateEncoder)
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys

LOG = logging.getLogger('nntool.'+__name__)


class StateDecoder(JsonSerializableStateDecoder):
    def __init__(self, *args, **kwargs):
        super(StateDecoder, self).__init__(object_hook=self.object_hook, *args, **kwargs)

# pylint: disable=no-self-use, method-hidden
    def object_hook(self, obj):
        obj = JsonSerializableStateDecoder.object_hook(self, obj)
        if isinstance(obj, dict) and 'qstats' in obj:
            # fix qstats int dictionary index. JSON converts to string.
            obj['qstats'] = {int(k): v for k, v in obj['qstats'].items()}
        return obj


STATE_EXTENSION = ".json"
ARRS_EXTENSION = ".nnparam"


def dump_state(G: NNGraph,
               include_parameters=True,
               state_path=None,
               extra=None):

    parameters = get_parameters(G) if include_parameters else {}

    if state_path is not None:
        graph_base, _ = os.path.splitext(state_path)
        if os.path.isdir(graph_base):
            graph_base = os.path.join(graph_base,
                                      os.path.basename(G.graph_identity.filename))
    else:
        graph_base, _ = os.path.splitext(G.graph_identity.filename)

    info_state = {
        'identity': G.graph_identity.identity,
        'info': convert_keys_to_str(G.info),
        'node_options': convert_keys_to_str(G.node_options),
        'load_parameters': include_parameters,
        'name': G.name,
        'extra': extra
    }

    state_filename = graph_base + STATE_EXTENSION
    LOG.info("dumping graph state to %s", state_filename)
    with open(state_filename, "w") as json_fp:
        json.dump(info_state, json_fp, indent=2, cls=JsonSerializableStateEncoder)

    if include_parameters:
        pickle_filename = graph_base + ARRS_EXTENSION
        LOG.info("dumping tensors to %s", pickle_filename)
        with open(pickle_filename, "wb") as param_fp:
            pickle.dump(parameters, param_fp)


def get_parameters(G):
    parameters = {}
    for _, pnode, _, fnode in G.nodes_iterator():
        anode = pnode if not fnode else fnode
        LOG.info("saving parameters for step %s %s", pnode.name, anode.name)
        parameters[NodeId(pnode, fnode)] = anode.get_parameters()
    return parameters


def set_parameters(G, parameters):
    for _, pnode, _, fnode in G.nodes_iterator():
        anode = pnode if not fnode else fnode
        try:
            anode.set_parameters(parameters[NodeId(pnode, fnode)])
        except KeyError as err:
            LOG.warn("can't find parameters for node %s.", anode.name)


def set_options(G, node_options, graph_node_options=None):
    if not node_options:
        return
    node_ids = []
    for nodeid, node_option in node_options.items():
        try:
            node = G.node(nodeid.node_name)
        except KeyError as err:
            if graph_node_options:
                raise err
            continue
        node.at_options.extend(node_option, overwrite=True)
        node_ids.append(nodeid)
    if graph_node_options:
        for nodeid in node_ids:
            graph_node_options[nodeid] = G.node(nodeid.node_name).at_options


def load_state(graph_file: str, value_cache=None, return_extra=False):
    graph_base, _ = os.path.splitext(graph_file)
    state_filename = graph_base + STATE_EXTENSION
    state_file = Path(state_filename)

    LOG.info("loading graph state from %s", state_filename)
    if not state_file.is_file():
        raise ValueError("state file not found")
    with state_file.open('r') as json_fp:
        info_state = json.load(json_fp, cls=StateDecoder)

    info_state['info'] = convert_str_to_keys(info_state['info'])
    if 'node_options' in info_state:
        info_state['node_options'] = convert_str_to_keys(info_state['node_options'])
    else:
        info_state['node_options'] = {}

    if info_state['load_parameters']:
        pickle_filename = graph_base + ARRS_EXTENSION
        LOG.info("loading tensors from %s", pickle_filename)
        arrs_file = Path(pickle_filename)
        if not arrs_file.is_file():
            raise ValueError("arrays file not found")
        with arrs_file.open('rb') as arrs_fp:
            parameters = pickle.load(arrs_fp)
    else:
        parameters = None

    # Here load the orignal graph and replay the transforms that were done to it
    opts = {
        'load_tensors': False,
    }
    # Retrieve the identity of the saved state
    identity = GraphIdentity(None)
    identity.identity = info_state['identity']

    LOG.info("loading graph from %s", identity.filename)
    G = create_graph(identity.filename, opts=opts)
    if 'name' in info_state:
        G.name = info_state['name']
    G.add_dimensions()
    set_options(G, info_state['node_options'])
    if identity.is_adjusted:
        # If weights were saved then don't reshaoe them since it was already done
        # before they were saved
        LOG.info("adjusting dimensions")
        G.adjust_order(reshape_weights=not info_state['load_parameters'])
        G.add_dimensions()

    if identity.is_fused:
        LOG.info("fusing nodes")
        # replay the fusions that were carried out
        for fusion_name in identity.fusions:
            fusion = get_fusion(fusion_name)
            fusion.match(G)
            G.add_dimensions()

    set_parameters(G, parameters)
    # Update the identity to match the saved graph
    G.info = info_state['info']
    G.graph_identity = identity
    G.value_cache = value_cache
    G.node_options = info_state['node_options']
    set_options(G, info_state['node_options'], info_state['node_options'])

    if identity.extracted_step is not None:
        extract_node(G, G.graph_state.steps[identity.extracted_step]['node'])
        G.add_dimensions()

    if return_extra:
        return G, info_state['extra']
    return G
