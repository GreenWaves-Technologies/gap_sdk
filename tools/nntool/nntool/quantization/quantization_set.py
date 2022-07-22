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

from nntool.graph.types import NNNodeBase
from nntool.graph.types.fusions import FusionNodeBase
from nntool.graph.types.misc import QuantizeNode
from nntool.utils.json_serializable import JsonSerializable

from nntool.quantization.new_qrec import QRec

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
                'graph_options': {},
                'node_options': {}
            }
        else:
            self._init = init

    def clear_qrecs(self):
        self._init['qset'] = {}

    def init_stats(self):
        self.stats = {}

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
    def stats(self):
        return self._init['stats']

    @stats.setter
    def stats(self, val):
        self._init['stats'] = val

    @property
    def graph_options(self):
        return self._init['graph_options']

    @graph_options.setter
    def graph_options(self, val):
        self._init['graph_options'] = val

    @property
    def node_options(self):
        return self._init['node_options']

    @node_options.setter
    def node_options(self, val):
        self._init['node_options'] = val

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
        assert isinstance(key, str) or (isinstance(key, tuple) and len(key) == 2 and all(isinstance(elem, str) for elem in key))
        self.qset[key] = item

    def _encapsulate(self):
        node_options = {}
        for nid, opt in self.node_options.items():
            if isinstance(nid, str):
                node_options[f'{nid}###'] = opt
            else:
                node_options[f'{nid[0]}###{nid[1]}'] = opt
        init = self._init.copy()
        init['node_options'] = node_options
        return init

    def sorted_iterator(self, G):
        node_ids = [(pnode.name, fnode.name) if fnode else pnode.name
                    for _, pnode, _, fnode in G.nodes_iterator()]
        return [(nid, self.qset[nid]) if nid in self.qset else (nid, None) for nid in node_ids]

    def all_out_qs_scale(self):
        return [(nid, qrec.out_qs[0].scale) if qrec.out_qs else None for nid, qrec in self.qset.items()]

    @classmethod
    def _dencapsulate(cls, val):
        node_options = {}
        for nid, opt in val['node_option'].items():
            nid = nid.split("###")
            if len(nid) == 1 or not nid[1]:
                node_options[nid[0]] = opt
            else:
                node_options[(nid[0], nid[1])] = opt
        val['node_options'] = node_options
        return cls(init=val)

    def copy_qrec(self, from_node, from_dir, from_idx, to_node):
        from_qrec = self.qset.get(from_node.name)
        if from_qrec is None:
            raise ValueError(f'trying to copy qrec from {from_node.name} to {to_node.name} - node has no qrec')
        qtype = deepcopy(getattr(from_qrec, f'{from_dir}_qs')[from_idx])
        self.qset[to_node.name] = QRec.copy_ktype(from_qrec, in_qs=[qtype], out_qs=[qtype])

    def verify_quantization(self, G):
        """Verify that all nodes have a quantization record"""
        is_quantized = True
        for node in G.nodes():
            if isinstance(node, FusionNodeBase) and not node.dont_quantize_internals:
                for fnode in node.contained_nodes():
                    if (node.name, fnode.name) not in self.qset:
                        LOG.info('%s:%s does not have a quantization record', node.name, fnode.name)
                        is_quantized = False
            if node.name not in self.qset:
                LOG.info('%s does not have a quantization record', node.name)
                is_quantized = False
        return is_quantized

    def get_all(self, nodes: Sequence[NNNodeBase]) -> Sequence[QRec]:
        """Get all the quantization records for a sequence of nodes"""
        if self.all_have_quantization(nodes):
            return [self.qset[node.name] for node in nodes]
        return None

    def get_all_stats(self, nodes: Sequence[NNNodeBase]) -> Sequence[QRec]:
        """Get all the stats records for a sequence of nodes"""
        if self.stats is None:
            return None
        all_stats = [self.stats.get(node.name) for node in nodes]
        if any(stat is None for stat in all_stats):
            return None
        return all_stats

    def all_have_quantization(self, nodes: Sequence[NNNodeBase]) -> bool:
        """Check that a sequence of nodes all have quantization records"""
        return all(node.name in self.qset for node in nodes)

    def propagate(self, G, from_node, to_node, from_idx=None, qtype=None, stats=None):
        """propagate the output quantization of from_node's output
        all the way to to_node's output"""
        if qtype is None:
            assert from_idx is not None
            qtype = self.qset[from_node.name].out_qs[from_idx]
        for edge in G.out_edges(from_node.name):
            if from_idx is None or edge.from_idx == from_idx:
                transit_node = edge.to_node
                transit_node_out_len = len(transit_node.out_dims)
                transit_node_qrec = self.qset[transit_node.name]
                transit_node_qrec.in_qs[edge.to_idx] = deepcopy(qtype)
                transit_node_qrec.out_qs = [deepcopy(qtype)] * transit_node_out_len
                if stats and self.stats and transit_node.name in self.stats:
                    to_stat = self.stats[transit_node.name]
                    to_stat['range_in'][edge.to_idx] = stats.copy()
                    to_stat['range_out'] = [stats.copy()] * transit_node_out_len
                if isinstance(transit_node, QuantizeNode):
                    transit_node.from_qtype = transit_node.to_qtype = deepcopy(qtype)
                if transit_node != to_node:
                    self.propagate(G, edge.to_node, to_node, qtype=qtype, stats=stats)

    def move_to_fusion(self, node: NNNodeBase, new_pnode: NNNodeBase):
        nid = node.name
        fnid = (new_pnode.name, node.name)
        if nid in self.qset:
            self.qset[fnid] = self.qset[nid]
            del self.qset[nid]
        if self.stats and nid in self.stats:
            self.stats[fnid] = self.stats[nid]
        if self.node_options and nid in self.node_options:
            pnid = new_pnode.name
            options = self.node_options[nid]
            del self.node_options[nid]
            self.node_options[fnid] = options
            self.node_options.setdefault(pnid, {}).update(options)

    def move_to_node(self, node: NNNodeBase, new_pnode: NNNodeBase):
        nid = node.name
        if nid in self.qset:
            self.qset[new_pnode.name] = self.qset[nid]
            del self.qset[nid]

    def copy_to_fusion(self, node: NNNodeBase, new_pnode: NNNodeBase, new_fnode: NNNodeBase):
        nid = node.name
        if nid in self.qset:
            self.qset[(new_pnode.name, new_fnode.name)] = deepcopy(self.qset[nid])

    def copy_to_node(self, node: NNNodeBase, new_pnode: NNNodeBase):
        nid = node.name
        if nid in self.qset:
            self.qset[new_pnode.name] = deepcopy(self.qset[nid])

    def remove_node(self, node: NNNodeBase):
        nid = node.name
        if nid in self.qset:
            del self.qset[nid]
