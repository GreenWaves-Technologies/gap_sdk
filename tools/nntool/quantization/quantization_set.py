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

from collections.abc import MutableMapping
from copy import deepcopy
from typing import Sequence

from graph.types import Parameters
from utils.json_serializable import JsonSerializable
from utils.node_id import NodeId, convert_keys_to_str, convert_str_to_keys
from quantization.quantization_record_base import QuantizationRecordBase

class QuantizationSet(MutableMapping, JsonSerializable):
    def __init__(self, *args, unwrap=False, init: dict = None, **kwargs):
        super(QuantizationSet, self).__init__(*args, **kwargs)
        if init is None:
            self._init = {
                'unwrap': unwrap,
                'qset': {}
            }
        else:
            self._init = init

    @property
    def qset(self):
        return self._init['qset']

    def __delitem__(self, key):
        del self.qset[key]

    def __getitem__(self, key):
        item = self.qset[key]
        if self.unwrap:
            item.unwrap = self.unwrap
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
        node_ids = [NodeId(pnode, fnode) for _, pnode, _, fnode in G.nodes_iterator()]
        return [(nid, self.qset[nid]) if nid in self.qset else (nid, None) for nid in node_ids]

    @classmethod
    def _dencapsulate(cls, val):
        return cls(init=convert_str_to_keys(val))

    @property
    def unwrap(self):
        return self._init['unwrap']

    @unwrap.setter
    def unwrap(self, val):
        self._init['unwrap'] = val

    def verify_quantization(self, G):
        """Verify that all nodes have a quantization record"""
        return all(NodeId(pnode, fnode) in self.qset for _, pnode, _, fnode in G.nodes_iterator())

    def get_all(self, nodes: Sequence[Parameters]) -> Sequence[QuantizationRecordBase]:
        """Get all the quantization records for a sequence of nodes"""
        if self.all_have_quantization(nodes):
            return [self.qset[NodeId(node)] for node in nodes]
        return None

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
                transit_node_qrec.out_qs = [deepcopy(qtype)] * len(transit_node_qrec.out_qs)
                if edge.to_node != to_node:
                    self.propagate(G, edge.to_node, to_node, qtype=qtype)

    def move_to_fusion(self, node: Parameters, new_pnode: Parameters):
        nid = NodeId(node)
        if nid in self.qset:
            self.qset[NodeId(new_pnode, node)] = self.qset[nid]
            del self.qset[nid]

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
