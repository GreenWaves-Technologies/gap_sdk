# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import typing
from abc import ABC, abstractmethod
from typing import Tuple, Union

from nntool.graph.types.fusions import FusionInputNode

from nntool.quantization.clipping import get_clip
from nntool.quantization.qtype import DTYPES

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph
    from nntool.quantization.quantization_set import QuantizationSet


class RangesMissingError(Exception):
    pass


class RangeProviderBase(ABC):

    class RangeProviderWrapper():
        def __init__(self, provider: 'RangeProviderBase', nid) -> None:
            self._provider = provider
            self._nid = nid

        def get_range_in(self, idx: int, dont_throw=False) -> Tuple[float]:
            return self._provider.get_range(self._nid, 'in', idx, dont_throw=dont_throw)

        def get_range_in_min_max(self, idx: int, dont_throw=False) -> Tuple[float]:
            range_in = self._provider.get_range(
                self._nid, 'in', idx, dont_throw=dont_throw)
            return {'min': range_in[0], 'max': range_in[1]}

        def get_range_out(self, idx: int, bits=None, dtype=None, dont_throw=False) -> Tuple[float]:
            return self._provider.get_range(self._nid, 'out', idx, bits=bits, dtype=dtype,
                                            dont_throw=dont_throw)

        def get_range_out_min_max(self, idx: int, dont_throw=False) -> Tuple[float]:
            range_out = self._provider.get_range(
                self._nid, 'out', idx, dont_throw=dont_throw)
            return {'min': range_out[0], 'max': range_out[1]}

        def get_stats_key(self, key):
            return self._provider.get_stats_key(self._nid, key)

        def get_stats_key_out(self, idx: int, key):
            return self._provider.get_stats_key_direction(self._nid, 'out', idx, key)

    @abstractmethod
    def get_range(self, nid: Union[str, Tuple], direction: str, idx: int, dont_throw=False) -> Tuple[float]:
        pass

    @abstractmethod
    def has_ranges(self, nid: Union[str, Tuple]) -> Tuple[float]:
        pass

    def get(self, nid: Union[str, Tuple]):
        if self.has_ranges(nid):
            return RangeProviderBase.RangeProviderWrapper(self, nid)
        return None

    @abstractmethod
    def add_quantizer_stat(self, nid: Union[str, Tuple], in_range: Tuple[float], out_range: Tuple[float]):
        pass

    @abstractmethod
    def remove_quantizer_stat(self, nid: Union[str, Tuple]):
        pass

    @abstractmethod
    def get_stats_key(self, nid: Union[str, Tuple], key: str):
        pass

    @abstractmethod
    def get_stats_key_direction(self, nid: Union[str, Tuple], direction: str, idx: int, key: str):
        pass


class StatsRangeProvider(RangeProviderBase):
    def __init__(self,
                 G: 'NNGraph',
                 qrecs: 'QuantizationSet',
                 stats: dict,
                 graph_options: dict,
                 node_options: dict,
                 test_mode=False) -> None:
        self._qrecs = qrecs
        self._stats = stats
        self._graph_options = graph_options
        self._node_options = node_options
        self._G = G
        self._test_mode = test_mode

    @classmethod
    def get_for_test(cls, stat):
        range_provider = StatsRangeProvider(
            None, None, {'test': stat}, None, None, test_mode=True)
        return range_provider.get('test')

    def get_clip_option(self, nid):
        if self._test_mode:
            return "none"
        if nid in self._node_options:
            opts = self._node_options[nid]
            if 'clip_type' in opts:
                return opts['clip_type']
        return self._graph_options.get('clip_type', 'none')

    def get_stat(self, nid):
        if nid in self._stats and self._stats[nid]:
            return self._stats[nid]
        if nid in self._qrecs:
            qrec = self._qrecs[nid]
            return {
                'range_in': [{'min': qtype.min_val, 'max': qtype.max_val} if qtype else None
                             for qtype in qrec.in_qs],
                'range_out': [{'min': qtype.min_val, 'max': qtype.max_val} if qtype else None
                              for qtype in qrec.out_qs],
            }
        raise RangesMissingError(f'no ranges found for {nid}')

    def get_range_in_fusion(self, pnode_name: str, fnode_name: str, idx: int) -> Tuple[float]:
        pnode = self._G[pnode_name]
        in_edge = pnode.subgraph.indexed_in_edges(fnode_name)[idx]
        in_node = in_edge.from_node
        if isinstance(in_node, FusionInputNode):
            in_edge = self._G.indexed_in_edges(pnode)[in_node.idx]
            clip_nid = in_edge.from_node.name
        else:
            clip_nid = (pnode_name, in_edge.from_node.name)
        stat = self.get_stat(clip_nid)['range_out'][in_edge.from_idx]
        qtype = self._qrecs[clip_nid].out_qs[in_edge.from_idx]
        shape = in_edge.from_node.out_dims[in_edge.from_idx].shape
        return get_clip(shape, qtype.bits, stat, self.get_clip_option(clip_nid))

    def get_range_in(self, nid: Union[str, Tuple], idx: int) -> Tuple[float]:
        if self._test_mode:
            return (self._stats[nid]['range_in'][idx]['min'], self._stats[nid]['range_in'][idx]['max'])
        if isinstance(nid, tuple):
            pnode_name, fnode_name = nid
        else:
            pnode_name = nid
            fnode_name = ""
        if fnode_name:
            return self.get_range_in_fusion(pnode_name, fnode_name, idx)

        in_edge = self._G.indexed_in_edges(pnode_name)[idx]
        clip_nid = in_edge.from_node.name
        stat = self.get_stat(clip_nid)['range_out'][in_edge.from_idx]
        qtype = self._qrecs[clip_nid].out_qs[in_edge.from_idx]
        shape = in_edge.from_node.out_dims[in_edge.from_idx].shape
        return get_clip(shape, qtype.bits, stat, self.get_clip_option(clip_nid))

    def get_range_out(self, nid: Union[str, Tuple], idx: int, bits=None, dtype=None) -> Tuple[float]:
        stat = self.get_stat(nid)['range_out'][idx]
        if self._test_mode:
            return stat['min'], stat['max']
        clip_type = self.get_clip_option(nid)
        if dtype:
            bits = DTYPES[dtype][0]
        elif bits is None:
            if nid in self._qrecs:
                qtype = self._qrecs[nid].out_qs[idx]
                bits = qtype.bits
            else:  # no bits explicitly provided and no qrec so no clip options
                clip_type = "none"
        if isinstance(nid, tuple):
            pnode = self._G[nid[0]]
            node = pnode.subgraph[nid[1]]
        else:
            node = self._G[nid]
        shape = node.out_dims[idx].shape
        return get_clip(shape, bits, stat, clip_type)

    def get_range(self, nid: Union[str, Tuple], direction: str, idx: int,
                  bits=None, dtype=None, dont_throw=False) -> Tuple[float]:
        try:
            if direction == 'in':
                return self.get_range_in(nid, idx)
            return self.get_range_out(nid, idx, bits=bits, dtype=dtype)
        except (TypeError, KeyError) as ex:
            if dont_throw:
                return None
            raise RangesMissingError(f'ranges not present for {nid.id[0]}'
                                     f'{f":{nid.id[1]}" if nid.id[1] else ""}') from ex

    def has_ranges(self, nid: Union[str, Tuple]) -> Tuple[float]:
        return nid in self._stats

    def add_quantizer_stat(self, nid: Union[str, Tuple], in_range: Tuple[float], out_range: Tuple[float]):
        self._stats[nid] = {
            "range_in": [{'min': in_range[0], 'max': in_range[1]}],
            "range_out": [{'min': out_range[0], 'max': out_range[1]}],
        }

    def remove_quantizer_stat(self, nid: Union[str, Tuple]):
        del self._stats[nid]

    def get_stats_key(self, nid: Union[str, Tuple], key: str):
        stat = self._stats.get(nid, None)
        if stat:
            return stat.get(key, None)
        return None

    def get_stats_key_direction(self, nid: Union[str, Tuple], direction: str, idx: int, key: str):
        try:
            return self._stats[nid][f'range_{direction}'][idx][key]
        except KeyError:
            return None
