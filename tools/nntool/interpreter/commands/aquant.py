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

# from graph.matches.propagate_softmax_sym_mult_qrec import PropagateSoftmaxSymQrec
# from graph.matches.propagate_rnn_sym_mult_qrec import PropagateUpRNNInputQ
# from graph.matches.propagate_sig_tan_mult_qrec import PropagateUpSigSwishInputQ
# from graph.matches.equalize_sym_mult_concats import EqualizeSymmetricMultiplicativeQuantivedConcats
from interpreter.nntool_shell_base import NNToolShellBase, store_once_in_history
from interpreter.shell_utils import (glob_input_files, input_options)
from quantization.unified_quantizer import UnifiedQuantizer
from stats.activation_ranges_collector import ActivationRangesCollector
from utils.data_importer import import_data
from utils.stats_funcs import STATS_BITS

LOG = logging.getLogger('nntool.'+__name__)

QUANTIZATION_SCHEMES = ['SQ8', 'POW2']


class AquantCommand(NNToolShellBase):
    # AQUANT COMMAND
    parser_aquant = Cmd2ArgumentParser()
    parser_aquant_group = parser_aquant.add_mutually_exclusive_group(required=False)
    parser_aquant_group.add_argument('-f', '--force_width',
                                     choices=STATS_BITS, type=int, default=16,
                                     help='force all layers to this bit-width in case of POW2 scheme, ' +
                                     'SQ8 will automatically force 8-bits')
    parser_aquant.add_argument('-s', '--scheme',
                               type=str, choices=QUANTIZATION_SCHEMES, default='SQ8',
                               help='quantize with scaling factors (TFlite quantization-like) [default] or POW2')
    parser_aquant.add_argument('-d', '--quant_dimension',
                               choices=['tensor', 'channel'], default='channel')
    parser_aquant.add_argument('-n', '--no_narrow_weights',
                               action='store_true', help='Don\'t quantize weights uniformly over negative/positive ' +
                               'range. i.e. Avoid -128 vs 127')
    input_options(parser_aquant)

    @with_argparser(parser_aquant)
    @store_once_in_history
    def do_aquant(self, args: argparse.Namespace):
        """
Attempt to calculate quantization for graph using one or more sample input files."""
        self._check_graph()
        stats_collector = ActivationRangesCollector()
        # if replaying state file then load the activation stats if they are present
        if args.scheme == 'SQ8':
            bits = 8
        else:
            bits = args.force_width
        if self.replaying_history and self.history_stats:
            astats = self.history_stats
        else:
            input_args = self._get_input_args(args)
            processed_input = False
            for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
                LOG.info("input file %s", file_per_input)
                processed_input = True
                data = [import_data(input_file, **input_args) for input_file in file_per_input]
                stats_collector.collect_stats(self.G, data)
            if not processed_input:
                self.perror("No input files found")
                return
            astats = stats_collector.stats
            self._record_stats(astats)

        quantizer = UnifiedQuantizer(args.scheme, astats,
                                     quantized_dimension=args.quant_dimension,
                                     narrow_weights=not args.no_narrow_weights,
                                     bits=bits)

        qrecs = quantizer.quantize(self.G)
        self.G.quantization = qrecs
        # These should now be unnecessary
        # if args.scheme == 'SQ8':
        #     concats_matcher = EqualizeSymmetricMultiplicativeQuantivedConcats()
        #     concats_matcher.match(self.G, set_identity=False)
        #     rnns_matcher = PropagateUpRNNInputQ()
        #     rnns_matcher.match(self.G, set_identity=False)
        #     softmax_qrec_matcher = PropagateSoftmaxSymQrec()
        #     softmax_qrec_matcher.match(self.G, set_identity=False)
        #     sig_swish_qrec_matcher = PropagateUpSigSwishInputQ()
        #     sig_swish_qrec_matcher.match(self.G, set_identity=False)
        LOG.info("Quantization set. Use qshow command to see it.")
