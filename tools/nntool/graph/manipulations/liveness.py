# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
