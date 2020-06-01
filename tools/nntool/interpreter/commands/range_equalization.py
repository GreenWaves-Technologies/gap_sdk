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
from interpreter.shell_utils import glob_input_files, input_options
from utils.data_importer import import_data
from stats.step_error_stats_collector import StepErrorStatsCollector
from quantization.cross_layer_range_eq import adjust_biases, weight_equalization

class WeightEqualizationCommand(NNToolShellBase):
    # WEIGHT_EQUALIZATION COMMAND
    parser_we = Cmd2ArgumentParser()
    parser_we.add_argument('threshold',
                           type=float, default=0.1,
                           help='convergence threshold')

    def execute_weight_equalization(self, threshold):
        if not (threshold > 0 and threshold < 10):
            self.perror("threshold should be 10 > x > 0")
        weight_equalization(self.G, threshold=threshold)

    @with_argparser(parser_we)
    def do_weight_equalization(self, args: argparse.Namespace):
        """
Run weight equalization on graph. This reduces variance between weight
channels and may improve quantization accuracy."""
        self._check_graph()
        self.execute_weight_equalization(args.threshold)

class BalanceFiltersCommand(NNToolShellBase):
    # BALANCE_FILTERS COMMAND
    parser_bf = Cmd2ArgumentParser()
    parser_bf.add_argument('-s', '--step',
                           type=int, help='step to balance. should be a convolution')
    parser_bf.add_argument('-t', '--threshold',
                           default=0.20,
                           type=float, help='precision threshold of weights below which a layer should be balanced')

    @with_argparser(parser_bf)
    def do_balance_filters(self, args: argparse.Namespace):
        """
Balance filter weights. THis will reduce variance in weights and will result in
a more balanced quantization at the expense of a multiplicative bias calculation."""
        self._check_graph()
        self.G.balance_filters(step_idx=args.step, precision_threshold=args.threshold)
        self.G.quantization = None

class BcorrCommand(NNToolShellBase):
    # BCORR COMMAND
    parser_bcorr = Cmd2ArgumentParser()
    input_options(parser_bcorr)

    @with_argparser(parser_bcorr)
    def do_bcorr(self, args):
        """
Correct biases with average quantization error."""
        self._check_graph()
        self._check_quantized()
        stats_collector = StepErrorStatsCollector()
        input_args = self._get_input_args(args)
        cnt = 0
        for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
            cnt += 1
            data = [import_data(filename, **input_args) for filename in file_per_input]
            stats_collector.collect_stats(self.G, data)

        adjust_biases(self.G, stats_collector.reduce_stats())
