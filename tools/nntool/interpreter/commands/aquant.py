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
from interpreter.nntool_shell_base import (NNToolShellBase,
                                           store_once_in_history)
from interpreter.shell_utils import glob_input_files, input_options
from quantization.handlers_helpers import (add_options_to_parser,
                                           get_options_from_args)
from quantization.quantizer.new_quantizer import NewQuantizer
from utils.data_importer import import_data
from utils.stats_funcs import STATS_BITS

from graph.types import ConstantInputParameters
from stats.activation_ranges_collector import ActivationRangesCollector

LOG = logging.getLogger('nntool.'+__name__)

QUANTIZATION_SCHEMES = ['SQ8', 'POW2', 'FLOAT']


class AquantCommand(NNToolShellBase):
    # AQUANT COMMAND
    parser_aquant = Cmd2ArgumentParser()
    parser_aquant.add_argument('-f',
                               '--force_width',
                               choices=STATS_BITS, type=int, default=0,
                               help='force all layers to this bit-width in case of POW2 scheme, ' +
                               'SQ8 will automatically force 8-bits')
    parser_aquant.add_argument('-s', '--scheme',
                               type=str, choices=QUANTIZATION_SCHEMES, default='SQ8',
                               help='quantize with scaling factors (TFlite quantization-like) [default] or POW2')
    add_options_to_parser(parser_aquant)
    input_options(parser_aquant)

    @with_argparser(parser_aquant)
    @store_once_in_history
    def do_aquant(self, args: argparse.Namespace):
        """
Attempt to calculate quantization for graph using one or more sample input files."""
        self._check_graph()
        stats_collector = ActivationRangesCollector()
        # if replaying state file then load the activation stats if they are present
        opts = get_options_from_args(args)
        state = ConstantInputParameters.save_compression_state(self.G)
        try:
            if self.replaying_history and self.history_stats:
                astats = self.history_stats
            else:
                input_args = self._get_input_args(args)
                processed_input = False
                for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
                    LOG.info("input file %s", file_per_input)
                    processed_input = True
                    data = [import_data(input_file, **input_args)
                            for input_file in file_per_input]
                    stats_collector.collect_stats(self.G, data)
                if not processed_input:
                    self.perror("No input files found")
                    return
                astats = stats_collector.stats
                self._record_stats(astats)

            if args.force_width:
                opts['bits'] = args.force_width

            quantizer = NewQuantizer(self.G, reset_all=True)
            quantizer.schemes.append(args.scheme)
            quantizer.set_stats(astats, opts)
            quantizer.quantize()

            self.G.add_dimensions()
            LOG.info("Quantization set. Use qshow command to see it.")
        finally:
            ConstantInputParameters.restore_compression_state(self.G, state)
