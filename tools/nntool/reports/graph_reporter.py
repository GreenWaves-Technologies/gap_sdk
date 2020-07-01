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

from graph.nngraph import NNGraph
from utils.tabular import Tabular, TabularColumn, TabularColumnGroup

from .reporter import Reporter

def scale_num(num):
    if not num:
        return ""
    if num > 1000000000:
        return "{:>.2f}G".format(num/1000000000)
    elif num > 1000000:
        return "{:>.2f}M".format(num/1000000)
    elif num > 1000:
        return "{:>.2f}K".format(num/1000)
    else:
        return "{:>d}".format(num)

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

def str_dims(node, dim_name):
    return "\n".join([str(dim) for dim in getattr(node, dim_name) if dim is not None])

def get_dim(dims, idx):
    if idx < len(dims):
        return dims[idx]
    return None

def graph_walk(steps, liveness):
    active = 0

    for i, step in enumerate(steps):
        node = step['node']

        params_size = node.get_parameter_size()

        for t_name in step['start']:
            live = liveness[t_name]
            active += live['dims'].size()

        ops = node.compute_load()

        yield i, node, active, params_size, ops

        for t_name in step['end']:
            live = liveness[t_name]
            active -= live['dims'].size()

def get_opname(node):
    operation = getattr(node, 'op_name')
    return operation

def get_graph_memory_usage(steps, liveness):
    max_active = 0
    tot_params = 0
    for _, _, active, params_size, _ in graph_walk(steps, liveness):
        tot_params += params_size
        if active > max_active:
            max_active = active
    return max_active, tot_params

class GraphReporter(Reporter):
    def __init__(self, do_totals=False, split_dims=False, step=None):
        super().__init__()
        self._do_totals = do_totals
        self._split_dims = split_dims
        self._step = step

    def report(self, G: NNGraph, stats) -> Tabular:
        del stats
        steps = G.graph_state.steps
        liveness = G.graph_state.liveness
        first_node = steps[0]['node']
        active_order = "x".join(first_node.out_dims[0].order)
        tab = Tabular()
        self.do_headers(active_order, tab)

        max_active = 0
        tot_params = 0
        tot_ops = 0

        for i, node, active, params_size, ops in graph_walk(steps, liveness):
            if self._step is not None and self._step != node.step_idx:
                continue
            tot_params += params_size
            if ops:
                tot_ops += ops
            if active > max_active:
                max_active = active

            self.do_operation(node, G, tab, i, active, params_size, ops)

        if self._step is None:
            self.check_do_totals(tab, max_active, tot_params, tot_ops)
        return tab

    def do_operation(self, node, G, tab, i, active, params_size, ops):
        operation = get_opname(node)

        in_edges = G.in_edges(node.name)
        in_edges.sort(key=lambda edge: edge.to_idx)
        in_steps = [edge.from_node.step_idx for edge in in_edges]

        if self._split_dims:
            s_in = str_dim_xl(node, 'in_dims', 0)
            s_out = str_dim_xl(node, 'out_dims', 0)

            tab.add_row([
                i,
                node.name,
                operation,
                get_dim(s_in, 0),
                get_dim(s_in, 1),
                get_dim(s_in, 2),
                get_dim(s_out, 0),
                get_dim(s_out, 1),
                get_dim(s_out, 2),
                ",".join([str(in_step) for in_step in in_steps]),
                active,
                params_size,
                scale_num(ops),
                str(node),
                node.hints_str
            ])
        else:
            tab.add_row([
                i,
                node.name,
                operation,
                str_dims(node, 'in_dims'),
                str_dims(node, 'out_dims'),
                "\n".join([str(in_step) for in_step in in_steps]),
                active,
                params_size,
                scale_num(ops),
                str(node),
                node.hints_str
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
            TabularColumn("Ops", fmt=">s"),
            TabularColumn("Params"),
            TabularColumn("Hints")
        ])

        tab.add_row(headers)

    def check_do_totals(self, tab, max_active, tot_params, tot_ops):
        if self._do_totals:
            if self._split_dims:
                tab.add_row(["", "Totals (#)\nMax active/Total params", "", "", "", "",
                             "", "", "", "", max_active, tot_params, scale_num(tot_ops), "", ""])
                tab.add_row(["", "Totals (#)\nMax mem usage", "", "", "", "", "", "", "",
                             "", "", max_active + tot_params, scale_num(tot_ops), "", ""])
            else:
                tab.add_row(["", "Totals (#)\nMax active/Total params",
                             "", "", "", "", max_active, tot_params, scale_num(tot_ops), "", ""])
                tab.add_row(["", "Totals (#)\nMax mem usage", "", "", "",
                             "", "", max_active + tot_params, scale_num(tot_ops), "", ""])
