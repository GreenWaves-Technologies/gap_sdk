# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from graph.nngraph import NNGraph
from utils.tabular import Tabular, TabularColumn, TabularColumnGroup

from .reporter import Reporter


def str_dim_xl(node, dim_name, dim_idx):
    dim = getattr(node, dim_name)[dim_idx]
    if dim.is_named:
        return [str(dim.c), str(dim.h), str(dim.w)]
    if dim.is_unknown:
        return ["N/A", "N/A", "N/A"]
    return dim.shape

def str_dim(node, dim_name, dim_idx):
    dim = getattr(node, dim_name)[dim_idx]
    return str(dim)

def graph_walk(steps, liveness):
    active = 0

    for i, step in enumerate(steps):
        node = step['node']

        params_size = node.get_parameter_size()

        for t_name in step['start']:
            live = liveness[t_name]
            active += live['dims'].size()

        operation = node.op_name
        if operation == "fusion":
            operation += " " + node.fusion_type

        yield i, node, active, params_size

        for t_name in step['end']:
            live = liveness[t_name]
            active -= live['dims'].size()

def get_opname(node):
    operation = getattr(node, 'op_name')
    if operation == "fusion":
        operation += " " + node.fusion_type
    return operation

def get_graph_memory_usage(steps, liveness):
    max_active = 0
    tot_params = 0
    for _, _, active, params_size in graph_walk(steps, liveness):
        tot_params += params_size
        if active > max_active:
            max_active = active
    return max_active, tot_params

class GraphReporter(Reporter):
    def __init__(self, do_totals=False, split_dims=False):
        super().__init__()
        self._do_totals = do_totals
        self._split_dims = split_dims

    def report(self, G: NNGraph, stats) -> Tabular:
        del stats
        steps = G.graph_state.steps
        liveness = G.graph_state.liveness
        first_node = steps[0]['node']
        active_order = "x".join(first_node.in_dims[0].order)
        tab = Tabular()
        self.do_headers(active_order, tab)

        max_active = 0
        tot_params = 0

        for i, node, active, params_size in graph_walk(steps, liveness):
            tot_params += params_size
            if active > max_active:
                max_active = active

            self.do_operation(node, G, tab, i, active, params_size)

        self.check_do_totals(tab, max_active, tot_params)
        return tab

    def do_operation(self, node, G, tab, i, active, params_size):
        operation = get_opname(node)

        in_steps = [edge.params.creating_step for edge in G.in_edges(node.name)]
        in_steps.sort()

        if self._split_dims:
            s_in = str_dim_xl(node, 'in_dims', 0)
            s_out = str_dim_xl(node, 'out_dims', 0)

            tab.add_row([
                i,
                node.name,
                operation,
                s_in[0],
                s_in[1],
                s_in[2],
                s_out[0],
                s_out[1],
                s_out[2],
                ",".join([str(in_step) for in_step in in_steps]),
                active,
                params_size,
                str(node)
            ])
        else:
            tab.add_row([
                i,
                node.name,
                operation,
                str_dim(node, 'in_dims', 0),
                str_dim(node, 'out_dims', 0),
                ",".join([str(in_step) for in_step in in_steps]),
                active,
                params_size,
                str(node)
            ])

    def do_headers(self, active_order, tab):
        headers = [
            TabularColumn("Step", fmt="^d"),
            TabularColumn("Step name"),
            TabularColumn("Operation"),
        ]
        if self._split_dims:
            headers.append(
                TabularColumnGroup("Input Dims",
                                   [TabularColumn("C", fmt="^s"),
                                    TabularColumn("H", fmt="^s"),
                                    TabularColumn("W", fmt="^s")]))
            headers.append(
                TabularColumnGroup("Output Dims",
                                   [TabularColumn("C", fmt="^s"),
                                    TabularColumn("H", fmt="^s"),
                                    TabularColumn("W", fmt="^s")]))
        else:
            headers.append(
                TabularColumn("Input Dims\n({})".format(active_order), fmt="^s")
            )
            headers.append(
                TabularColumn("Output Dims\n({})".format(active_order), fmt="^s")
            )
        headers.extend([
            TabularColumn("Inputs", fmt="^s"),
            TabularColumn("Active\nsize", fmt="^d"),
            TabularColumn("Params\nsize", fmt="^d"),
            TabularColumn("Params"),
        ])

        tab.add_row(headers)

    def check_do_totals(self, tab, max_active, tot_params):
        if self._do_totals:
            if self._split_dims:
                tab.add_row(["", "Totals (#)\nMax active/Total params", "", "", "", "",
                             "", "", "", "", max_active, tot_params, ""])
                tab.add_row(["", "Totals (#)\nMax mem usage", "", "", "", "", "", "", "",
                             "", "", max_active + tot_params, ""])
            else:
                tab.add_row(["", "Totals (#)\nMax active/Total params",
                             "", "", "", "", max_active, tot_params, ""])
                tab.add_row(["", "Totals (#)\nMax mem usage", "", "", "",
                             "", "", max_active + tot_params, ""])
