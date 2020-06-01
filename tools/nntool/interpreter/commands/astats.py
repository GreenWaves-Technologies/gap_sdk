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
import logging
from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import (output_table, table_options, input_options,
                                     glob_input_files)
from reports.activation_reporter import ActivationReporter
from stats.activation_stats_collector import ActivationStatsCollector
from utils.data_importer import import_data

LOG = logging.getLogger("nntool")

class AstatsCommand(NNToolShellBase):   
    # ASTATS COMMAND
    parser_astats = Cmd2ArgumentParser()
    parser_astats.add_argument('-q', '--qsnr',
                               type=float, default=30.0, help='QSNR threshold')
    parser_astats.add_argument('-d', '--detail',
                               action="store_true", help='Show fusions detail')
    parser_astats.add_argument('-s',
                               '--step',
                               type=int,
                               nargs=(1, 2),
                               help='display information by channel for step. You can indicate a fusion step with two values. The step_idx and the idx of the node in the fusion.')
    table_options(parser_astats, default_width=180)
    input_options(parser_astats)

    @with_argparser(parser_astats)
    def do_astats(self, args: argparse.Namespace):
        """
Calculate activation statistics on one or more imput files."""
        self._check_graph()
        input_args = self._get_input_args(args)
        stats_collector = ActivationStatsCollector()
        step_idx = args.step
        if step_idx is not None:
            if len(step_idx) == 1:
                step_idx = step_idx[0]
            else:
                step_idx = tuple(step_idx)
        if len(args.input_files) == 0:
            self.perror("You must enter some files to process")
            return
        for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
            LOG.info("input file %s", file_per_input)
            data = [import_data(input_file, **input_args) for input_file in file_per_input]
            stats_collector.collect_stats(self.G, data)

        fmt = ('tab' if args.output is None else args.output['fmt'])
        tab = ActivationReporter(do_totals=(fmt != "csv"),
                                 threshold=args.qsnr,
                                 yield_fusions=args.detail or isinstance(step_idx, tuple)).report(self.G,
                                                                                                  stats_collector.reduce_stats())
        output_table(tab, args)
