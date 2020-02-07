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
