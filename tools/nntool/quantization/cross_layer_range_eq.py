# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

# Implements weight equalization as per https://arxiv.org/abs/1906.04721

import logging
import math
import numpy as np

from graph.types import FilterParameters, FusionParameters
from stats.ranges import Ranges
from stats.scales import Scales

LOG = logging.getLogger('nntool.'+__name__)

def discover_groups(G):
    groups = []
    group = []
    neurons = []
    last_neuron = None
    for step in G.graph_state.steps:
        node = step['node']
        # nodes cannot have multiple outputs
        if len([G.successors(node.name)]) > 1:
            last_neuron = None
            group = add_group(group, groups, neurons)
            continue
        # can equalize indicates that the node can be included in the group
        if not node.can_equalize:
            last_neuron = None
            group = add_group(group, groups, neurons)
            continue

        if isinstance(node, FilterParameters):
            last_neuron = add_neuron(node.name, node, last_neuron, neurons, group)
            continue

        if isinstance(node, FusionParameters):
            filters = node.contained_filters()
            if len(filters) == 1:
                last_neuron = add_neuron(node.name, filters[0], last_neuron, neurons, group)

    if group:
        add_group(group, groups, neurons)

    return groups, neurons

def add_group(group, groups, neurons):
    if group:
        groups.append(group)
        neurons.append(group[-1][1])
        group = []
    return group

def add_neuron(node_name, node, last_neuron, neurons, group):
    new_neuron = {'name': node_name, 'node': node,
                  'weights': None, 'biases': None}
    if last_neuron is not None:
        neurons.append(last_neuron)
        group.append((last_neuron, new_neuron))
    last_neuron = new_neuron
    return last_neuron

def calculate_s(range_1, range_2):
    assert len(range_1) == len(range_2)
    # note: the paper is wrong. It should be 1/range2 not 1/range1
    return [(1/range_2[i]) * math.sqrt(range_1[i] * range_2[i]) for i in range(len(range_1))]

class QuantizationError(Exception):
    pass

def calculate_precisions(step):
    nn_0 = step[0]
    nn_1 = step[1]
    ranges_0, max_0 = Ranges.range_output(nn_0['node'], weights=nn_0['weights'])
    ranges_1, max_1 = Ranges.range_input(nn_1['node'], weights=nn_1['weights'])
    prec_0 = ranges_0/max_0
    prec_1 = ranges_1/max_1
    return prec_0, prec_1

def process_group(group, threshold):
    total_precision = 0
    cycles = 0
    # Keep going until we converge
    while True:
        precisions = []
        cycles += 1
        if cycles > 50:
            raise QuantizationError("Weight scaling has failed to converge")
        for step in group:
            prec_0, prec_1 = calculate_precisions(step)
            precisions.append(np.sum(prec_0 * prec_1))

        new_total_precision = sum(precisions)
        # end when the precision change drops below threshold
        if abs(new_total_precision - total_precision) < threshold:
            LOG.info("group has converged under %f after %d cycles", threshold, cycles)
            break
        total_precision = new_total_precision

        # note: traversing in reverse order. Not sure that it makes any difference.
        for step in reversed(group):
            nn_0 = step[0]
            nn_1 = step[1]
            # get the ranges of the output channels of layer 0 and input channels of layer 2
            ranges_0, _ = Ranges.range_output(nn_0['node'], weights=nn_0['weights'])
            ranges_1, _ = Ranges.range_input(nn_1['node'], weights=nn_1['weights'])
            scale = calculate_s(ranges_0, ranges_1)
            # now apply the scale to the output and input channels
            nn_0['weights'], nn_0['biases'] =\
                Scales.scale_output(nn_0['node'], scale, nn_0['weights'], nn_0['biases'])
            nn_1['weights'] = Scales.scale_input(nn_1['node'], scale, nn_1['weights'])

def process_groups(groups, threshold=0.01):
    for group in groups:
        LOG.info("processing group")
        process_group(group, float(threshold))

def update_parameters(neurons):
    for neuron in neurons:
        params = neuron['node']
        params.weights = neuron['weights']
        if neuron['biases'] is not None:
            params.biases = neuron['biases']

def weight_equalization(G, threshold=0.01):
    LOG.info("discovering groups")
    groups, neurons = discover_groups(G)
    if groups and neurons:
        LOG.info("found %d groups and %d neurons", len(groups), len(neurons))
        process_groups(groups, threshold)
        update_parameters(neurons)
        G.graph_identity.set_equalized(threshold)
    else:
        LOG.warning("no groups to equalize found")
