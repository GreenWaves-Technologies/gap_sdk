# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import json
import logging
import os
import pickle
from pathlib import Path

from generation.memory_device_info import MemoryDeviceInfos
from graph.graph_identity import GraphIdentity
from graph.matches.matches import get_fusion
from graph.nngraph import NNGraph
from importer.importer import create_graph
from quantization.qtype import QType
from quantization.quantization_record import QuantizationRecord
from utils.node_id import NodeId

LOG = logging.getLogger('nntool.'+__name__)

class StateEncoder(json.JSONEncoder):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
# pylint: disable=no-self-use, method-hidden
    def default(self, o):
        if isinstance(o, NodeId):
            return {'__type': 'NodeId', 'val': o.to_json_str()}
        if isinstance(o, QuantizationRecord):
            return o.to_dict()
        if isinstance(o, QType):
            return {'__type': 'QType', 'bits': o.bits, 'q': o.q, 'signed': o.signed}
        if isinstance(o, MemoryDeviceInfos):
            return {'__type': 'MemoryDeviceInfos', 'infos': o.todict()}
        # Let the base class default method raise the TypeError
        return json.JSONEncoder.default(self, o)

class StateDecoder(json.JSONDecoder):
    def __init__(self, *args, **kwargs):
        json.JSONDecoder.__init__(self, object_hook=self.object_hook, *args, **kwargs)

# pylint: disable=no-self-use, method-hidden
    def object_hook(self, obj):
        if '__type' in obj:
            if obj['__type'] == 'QType':
                return QType(bits=obj['bits'], q=obj['q'], signed=obj['signed'])
            if obj['__type'] == 'MemoryDeviceInfos':
                return MemoryDeviceInfos.fromdict(obj['infos'])
            if obj['__type'] == 'NodeId':
                return NodeId.from_json_str(obj['val'])
            if obj['__type'].startswith('QuantizationRecord'):
                return QuantizationRecord.from_dict(obj)

        if 'qstats' in obj:
            # fix qstats int dictionary index. JSON converts to string.
            obj['qstats'] = {int(k):v for k, v in obj['qstats'].items()}
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
        'load_parameters': include_parameters,
        'name': G.name,
        'extra': extra
    }

    state_filename = graph_base + STATE_EXTENSION
    LOG.info("dumping graph state to %s", state_filename)
    with open(state_filename, "w") as json_fp:
        json.dump(info_state, json_fp, indent=2, cls=StateEncoder)

    if include_parameters:
        pickle_filename = graph_base + ARRS_EXTENSION
        LOG.info("dumping tensors to %s", pickle_filename)
        with open(pickle_filename, "wb") as param_fp:
            pickle.dump(parameters, param_fp)

def convert_keys_to_str(info):
    if isinstance(info, list):
        return [convert_keys_to_str(elem) for elem in info]
    if isinstance(info, dict):
        if info and isinstance(list(info.keys())[0], NodeId):
            return {'NodeId_' + k.to_json_str():convert_keys_to_str(v) for k, v in info.items()}
        else:
            return {k:convert_keys_to_str(v) for k, v in info.items()}
    return info

def convert_str_to_keys(info):
    if isinstance(info, list):
        return [convert_keys_to_str(elem) for elem in info]
    if isinstance(info, dict):
        return {(NodeId.from_json_str(k[7:]) if k.startswith('NodeId_') else k): \
            convert_str_to_keys(v) for k, v in info.items()}
    return info

def get_parameters(G):
    parameters = {}
    for _, pnode, _, fnode in G.nodes_iterator():
        anode = pnode if not fnode else fnode
        parameters[NodeId(pnode, fnode)] = anode.get_parameters()
    return parameters

def set_parameters(G, parameters):
    for _, pnode, _, fnode in G.nodes_iterator():
        anode = pnode if not fnode else fnode
        anode.set_parameters(parameters[NodeId(pnode, fnode)])

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
    if return_extra:
        return G, info_state['extra']
    return G
