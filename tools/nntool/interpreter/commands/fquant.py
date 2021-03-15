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

import numpy as np
from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import table_options
from quantization.unified_quantizer import UnifiedQuantizer
from utils.stats_funcs import STATS_BITS

from graph.matches.equalize_sym_mult_concats import \
    EqualizeSymmetricMultiplicativeQuantivedConcats
from graph.matches.propagate_softmax_sym_mult_qrec import \
    PropagateSoftmaxSymQrec
from stats.activation_ranges_collector import ActivationRangesCollector

QUANTIZATION_SCHEMES = ['SQ8', 'POW2']


class FquantCommand(NNToolShellBase):
    # FQUANT COMMAND
    parser_fquant = Cmd2ArgumentParser()
    parser_fquant.add_argument('-f', '--force_width',
                               choices=STATS_BITS, default=8, type=int, help='force all layers to this width')
    parser_fquant.add_argument('-s', '--scheme',
                               type=str, choices=QUANTIZATION_SCHEMES, default='SQ8',
                               help='quantize with scaling factors (TFlite quantization-like) [default] or POW2')
    parser_fquant.add_argument('-d', '--quant_dimension',
                               choices=['tensor', 'channel'], default='channel')
    parser_fquant.add_argument('--uniform',
                               type=float, default=0.0,
                               help='Use uniform distribution for input with the specified max value')
    parser_fquant.add_argument('--num_inference',
                               type=int, default=1,
                               help='How many inferences')
    parser_fquant.add_argument('-n', '--no_narrow_weights',
                               action='store_true', help='Don\'t quantize weights uniformly over negative/positive ' +
                               'range. i.e. Avoid -128 vs 127')
    table_options(parser_fquant, default_width=140)

    @with_argparser(parser_fquant)
    def do_fquant(self, args: argparse.Namespace):
        """
Attempt to calculate a fake quantization for graph using random tensors and parameters.
This is intended to allow code generation for performance testing even if no real
weights and input data are avalaible."""
        self._check_graph()
        self.G.constant_store.fake = True
        stats_collector = ActivationRangesCollector()
        for _ in range(args.num_inference):
            if args.uniform:
                input_tensors = [np.random.uniform(-args.uniform, args.uniform, inp.dims.shape)
                                 for inp in self.G.input_nodes()]
            else:
                input_tensors = [np.random.normal(0, 0.2, inp.dims.shape)
                                 for inp in self.G.input_nodes()]
            stats_collector.collect_stats(self.G, input_tensors)
        if args.scheme == 'SQ8':
            bits = 8
        else:
            bits = args.force_width
        astats = stats_collector.stats

        quantizer = UnifiedQuantizer(args.scheme, astats,
                                     quantized_dimension=args.quant_dimension,
                                     narrow_weights=not args.no_narrow_weights,
                                     bits=bits)
        self._record_stats(astats)
        qrecs = quantizer.quantize(self.G)
        self.G.quantization = qrecs
        if args.scheme == 'SQ8':
            concats_matcher = EqualizeSymmetricMultiplicativeQuantivedConcats()
            concats_matcher.match(self.G, set_identity=False)
            softmax_qrec_matcher = PropagateSoftmaxSymQrec()
            softmax_qrec_matcher.match(self.G, set_identity=False)
        self.G.constant_store.fake = False
