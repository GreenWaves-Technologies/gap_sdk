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

from typing import Mapping, Sequence

def calculate_liveness(G, steps: Sequence[Mapping]) -> Mapping[str, Mapping]:
    liveness = {}
    for i, step in enumerate(steps):
        node = step['node']
        # initialize the steps arrays for the analysis below
        step['start'] = []
        step['end'] = []
        # input nodes create tensors
        if G.is_input(node):
            edges = G.out_edges(node.name)
            assert len(edges) == 1
            eparams = edges[0].params
            liveness[eparams.name] = {
                'start': i, 'end': i,
                'is_input':True, 'is_output': False, 'dims': eparams.dims
            }
        else:
            # check what we consume
            for edge in G.in_edges(node.name):
                eparams = edge.params
                live = liveness.get(eparams.name)
                assert live is not None, "Inputs to node must have already been created"
                if live['end'] < i:
                    live['end'] = i
                if G.is_output(node):
                    live['is_output'] = True
            # check what we create
            for edge in G.out_edges(node.name):
                eparams = edge.params
                live = liveness.get(eparams.name)
                if live is None:
                    live = {
                        'start': i, 'end': i, 'is_input':False,
                        'is_output': False, 'dims': eparams.dims,
                    }
                    liveness[eparams.name] = live

    # update the tensors created in each step
    for edge_name, live in liveness.items():
        steps[live['start']]['start'].append(edge_name)
        steps[live['end']]['end'].append(edge_name)
    return liveness
