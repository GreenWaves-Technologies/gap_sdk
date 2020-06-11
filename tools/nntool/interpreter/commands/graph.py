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

import argparse
from cmd2 import Cmd2ArgumentParser, with_argparser, CompletionItem
from interpreter.shell_utils import output_table, table_options
from interpreter.nntool_shell_base import NNToolShellBase
from reports.graph_reporter import GraphReporter

class GraphCommand(NNToolShellBase):
    # GRAPH COMMAND

    def other_open_graphs(self, only_open=False):
        items = []
        for graph_idx, graph in enumerate(self._graphs):
            if graph_idx == self._graph_idx:
                continue
            if graph['G'] is None:
                if only_open:
                    continue
                name = "No Graph"
            else:
                name = graph['G'].name
            items.append(CompletionItem(graph_idx, name))
        return items

    parser_graph = Cmd2ArgumentParser("display graph")
    parser_graph.add_argument('graph_number',
                              nargs=argparse.OPTIONAL,
                              type=int,
                              choices_method=other_open_graphs,
                              help='graph to select or nothing to show open graphs')

    @with_argparser(parser_graph)
    def do_graph(self, args: argparse.Namespace):
        """
Select actuve graphs"""
        if args.graph_number is not None:
            if args.graph_number < 0 or args.graph_number >= len(self._graphs):
                self.perror("graph number is invalid")
                return
            self._graph_idx = args.graph_number
            self.pfeedback("selected graph {}".format(self._graph_idx))
            self._update_prompt()
            self.py_locals['G'] = self.G
        else:
            for idx, rec in enumerate(self._graphs):
                self.poutput("{:d} - {}".format(idx, rec['graph_file']))

    # SHOW COMMAND
    parser_show = Cmd2ArgumentParser("display graph")
    table_options(parser_show, default_width=180)
    parser_show.add_argument('step', type=int, nargs=(0, 1), help='Limit to step number')

    @with_argparser(parser_show)
    def do_show(self, args: argparse.Namespace):
        """
Display the structure of the graph"""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        split_dims = fmt == "xls"
        do_totals = fmt != "csv"
        tab = GraphReporter(split_dims=split_dims, do_totals=do_totals,
                            step=args.step).report(self.G, None)
        output_table(tab, args)
