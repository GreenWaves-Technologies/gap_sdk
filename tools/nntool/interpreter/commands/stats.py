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
from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import output_table, table_options
from reports.filter_reporter import (FilterDetailedStatsReporter,
                                     FilterStatsReporter)
from stats.filter_stats_collector import (FilterDetailedStatsCollector,
                                          FilterStatsCollector)

class StatsCommand(NNToolShellBase):
    # STATS COMMAND
    parser_stats = Cmd2ArgumentParser("display statistics on globals")
    parser_stats.add_argument('-d', '--detailed',
                              action="store_true", help='Dump detailed statistics')
    parser_stats.add_argument('-q', '--qsnr',
                              type=float, default=30.0, help='QSNR threshold')
    parser_stats.add_argument('-s', '--step',
                              type=int,
                              nargs=(1, 2),
                              help='display information by channel for step')
    table_options(parser_stats, default_width=180)

    @with_argparser(parser_stats)
    def do_stats(self, args: argparse.Namespace):
        """
Display statistics on weights and biases"""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        if args.detailed:
            stats_collector = FilterDetailedStatsCollector()
            stats = stats_collector.collect_stats(self.G)
            tab = FilterDetailedStatsReporter().report(self.G, stats)
        else:
            step_idx = args.step
            if step_idx is not None:
                if len(step_idx) == 1:
                    step_idx = step_idx[0]
                else:
                    step_idx = tuple(step_idx)
            stats_collector = FilterStatsCollector()
            stats = stats_collector.collect_stats(self.G, step_idx=step_idx)
            tab = FilterStatsReporter(do_totals=(fmt != "csv"), threshold=args.qsnr, step_idx=step_idx)\
                .report(self.G, stats)
        output_table(tab, args)
