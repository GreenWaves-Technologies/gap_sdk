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

from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import (glob_input_files, input_options,
                                     output_table, table_options)
from utils.data_importer import import_data
from stats.step_error_stats_collector import StepErrorStatsCollector
from stats.error_stats_collector import ErrorStatsCollector
from reports.error_reporter import ErrorReporter


class QerrorCommand(NNToolShellBase):
    # QERROR COMMAND
    parser_qerror = Cmd2ArgumentParser()
    parser_qerror.add_argument('-s', '--step',
                               action='store_true',
                               help='evaluate quantization per step. i.e.\
                                    individually quantize each layer')
    parser_qerror.add_argument('--compare_quantized',
                               action='store_true',
                               help='quantize and dequantize the float output \
                                   to give it the same error as the quantized output of the layer')
    parser_qerror.add_argument('-r', '--report_lowest',
                               type=int, help='QSNR threshold below which to report filename')
    table_options(parser_qerror, default_width=140)
    input_options(parser_qerror)

    @with_argparser(parser_qerror)
    def do_qerror(self, args):
        """
Show quantization error introduced by processing one or more input files."""
        self._check_graph()
        self._check_quantized()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        input_args = self._get_input_args(args)
        if args.step:
            stats_collector = StepErrorStatsCollector(quant_compare=args.compare_quantized)
        else:
            stats_collector = ErrorStatsCollector(quant_compare=args.compare_quantized)
        cnt = 0
        for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
            cnt += 1

            data = [import_data(input_file, **input_args) for input_file in file_per_input]
            stat = stats_collector.collect_stats(self.G, data)
            if args.report_lowest is not None:
                lowest = min((elem['qsnr'] for elem in stat.values()))
                if lowest < args.report_lowest:
                    self.pfeedback("{} had QSNR below threshold".format(file_per_input))
        if not cnt:
            self.perror("no files to process")
            return
        tab = ErrorReporter(do_totals=(fmt != "csv"), one_input=cnt <= 1, with_chan=args.step)\
            .report(self.G, stats_collector.reduce_stats())
        output_table(tab, args)
