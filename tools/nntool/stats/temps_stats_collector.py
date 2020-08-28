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

from utils.node_id import NodeId

from .stats_collector import StatsCollector

class TempsStatsCollector(StatsCollector):
    def __init__(self, qrecs=None):
        self._qrecs = qrecs

    def _prepare(self, G):
        pass

    def _collect(self, G, step_idx):
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
