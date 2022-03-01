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
from collections.abc import MutableMapping
from copy import deepcopy
from typing import Sequence

from graph.types import Parameters
from graph.types.fusions import FusionBase
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys

from quantization.new_qrec import QRec

LOG = logging.getLogger(__name__)

class QuantizationSet(MutableMapping, JsonSerializable):
    def __init__(self, *args, init: dict = None, **kwargs):
        super(QuantizationSet, self).__init__(*args, **kwargs)
        if init is None:
            self._init = {
                'qset': {},
                'schemes_present': set(),
                'scheme_priority': [],
                'stats': None,
                'frozen_qs': None,
                'options': {}
            }
        else:
            self._init = init

    def clear_qrecs(self):
        self._init['qset'] = {}

    @property
    def schemes_present(self):
        return self._init['schemes_present']

    @schemes_present.setter
    def schemes_present(self, val):
        self._init['schemes_present'] = val

    @property
    def scheme_priority(self):
        return self._init['scheme_priority']

    @scheme_priority.setter
    def scheme_priority(self, val):
        self._init['scheme_priority'] = val

    @property
    def frozen_qs(self):
        return self._init['frozen_qs']

    @frozen_qs.setter
    def frozen_qs(self, val):
        self._init['frozen_qs'] = val

    @property
    def stats(self):
        return self._init['stats']

    @stats.setter
    def stats(self, val):
        self._init['stats'] = val

    @property
    def options(self):
        return self._init['options']

    @options.setter
    def options(self, val):
        self._init['options'] = val

    @property
    def qset(self):
        return self._init['qset']

    def __delitem__(self, key):
        del self.qset[key]

    def __getitem__(self, key):
        if key not in self.qset:
            raise KeyError() # @IgnoreException
        item = self.qset[key]
        return item

    def __iter__(self):
        return self.qset.__iter__()

    def __len__(self):
        return len(self.qset)

    def __setitem__(self, key, item):
        self.qset[key] = item

    def _encapsulate(self):
        return convert_keys_to_str(self._init)

    def sorted_iterator(self, G):
        node_ids = [NodeId(pnode, fnode)
                    for _, pnode, _, fnode in G.nodes_iterator()]
        return [(nid, self.qset[nid]) if nid in self.qset else (nid, None) for nid in node_ids]

    def all_out_qs_scale(self):
        return [(nid, qrec.out_qs[0].scale) if qrec.out_qs else None for nid, qrec in self.qset.items()]

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init=convert_str_to_keys(val))

    def copy_qrec(self, from_node, from_dir, from_idx, to_node):
        from_qrec = self.qset.get(NodeId(from_node))
        if from_qrec is None:
            raise ValueError(f'trying to copy qrec from {from_node.name} to {to_node.name} - node has no qrec')
        qtype = deepcopy(getattr(from_qrec, f'{from_dir}_qs')[from_idx])
        self.qset[NodeId(to_node)] = QRec.copy_ktype(from_qrec, in_qs=[qtype], out_qs=[qtype])

    def verify_quantization(self, G):
        """Verify that all nodes have a quantization record"""
        is_quantized = True
        for node in G.nodes():
            if isinstance(node, FusionBase) and not node.dont_quantize_internals:
                for fnode in node.contained_nodes():
                    if NodeId(node, fnode) not in self.qset:
                        LOG.info('%s:%s does not have a quantization record', node.name, fnode.name)
                        is_quantized = False
            if NodeId(node) not in self.qset:
                LOG.info('%s does not have a quantization record', node.name)
                is_quantized = False
        return is_quantized

    def get_all(self, nodes: Sequence[Parameters]) -> Sequence[QRec]:
        """Get all the quantization records for a sequence of nodes"""
        if self.all_have_quantization(nodes):
            return [self.qset[NodeId(node)] for node in nodes]
        return None

    def get_all_stats(self, nodes: Sequence[Parameters]) -> Sequence[QRec]:
        """Get all the stats records for a sequence of nodes"""
        if self.stats is None:
            return None
        all_stats = [self.stats.get(NodeId(node)) for node in nodes]
        if any(stat is None for stat in all_stats):
            return None
        return all_stats

    def all_have_quantization(self, nodes: Sequence[Parameters]) -> bool:
        """Check that a sequence of nodes all have quantization records"""
        return all(NodeId(node) in self.qset for node in nodes)

    def propagate(self, G, from_node, to_node, from_idx=None, qtype=None):
        """propagate the output quantization of from_node's output
        all the way to to_node's output"""
        if qtype is None:
            assert from_idx is not None
            qtype = self.qset[NodeId(from_node)].out_qs[from_idx]
        for edge in G.out_edges(from_node.name):
            if from_idx is None or edge.from_idx == from_idx:
                transit_node_qrec = self.qset[NodeId(edge.to_node)]
                transit_node_qrec.in_qs[edge.to_idx] = deepcopy(qtype)
                transit_node_qrec.out_qs = [
                    deepcopy(qtype)] * len(transit_node_qrec.out_qs)
                if edge.to_node != to_node:
                    self.propagate(G, edge.to_node, to_node, qtype=qtype)

    def move_to_fusion(self, node: Parameters, new_pnode: Parameters):
        nid = NodeId(node)
        fnid = NodeId(new_pnode, node)
        if nid in self.qset:
            self.qset[fnid] = self.qset[nid]
            del self.qset[nid]
        if self.stats and nid in self.stats:
            self.stats[fnid] = self.stats[nid]
        if self.options and nid in self.options:
            pnid = NodeId(new_pnode)
            options = self.options[nid]
            del self.options[nid]
            self.options[fnid] = options
            self.options.setdefault(pnid, {}).update(options)

    def move_to_node(self, node: Parameters, new_pnode: Parameters):
        nid = NodeId(node)
        if nid in self.qset:
            self.qset[NodeId(new_pnode)] = self.qset[nid]
            del self.qset[nid]

    def copy_to_fusion(self, node: Parameters, new_pnode: Parameters, new_fnode: Parameters):
        nid = NodeId(node)
        if nid in self.qset:
            self.qset[NodeId(new_pnode, new_fnode)] = deepcopy(self.qset[nid])

    def copy_to_node(self, node: Parameters, new_pnode: Parameters):
        nid = NodeId(node)
        if nid in self.qset:
            self.qset[NodeId(new_pnode)] = deepcopy(self.qset[nid])

    def remove_node(self, node: Parameters):
        nid = NodeId(node)
        if nid in self.qset:
            del self.qset[nid]
