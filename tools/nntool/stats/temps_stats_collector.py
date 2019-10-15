# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from utils.node_id import NodeId

from .stats_collector import StatsCollector

class TempsStatsCollector(StatsCollector):
    def __init__(self, qrecs=None):
        self._qrecs = qrecs

    def _prepare(self, G):
        pass

    def _collect(self, G):
        temps = {}
        temps_list = []
        for edge in G.edges():
            eparams = edge.params
            temp = temps.get(eparams.name)
            if temp is None:
                temp = {
                    'name': eparams.name,
                    'creating_node': eparams.creating_node,
                    'creating_step': eparams.creating_step,
                    'type': eparams.edge_type,
                    'dims': eparams.dims,
                    'count': 1
                }
                if self._qrecs is not None:
                    qrec = self._qrecs[NodeId(edge.from_node, None)]
                    temp['qtype'] = qrec.out_qs[edge.from_idx]
                temps[eparams.name] = temp
                temps_list.append(temp)
            else:
                temp['count'] += 1
        temps_list.sort(key=lambda temp: temp['creating_step'])
        return temps_list
