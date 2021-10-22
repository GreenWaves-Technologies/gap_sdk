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

import numpy as np
from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from quantization.handlers_helpers import (add_options_to_parser,
                                           get_options_from_args)
from quantization.quantizer.new_quantizer import NewQuantizer


from stats.activation_ranges_collector import ActivationRangesCollector

QUANTIZATION_SCHEMES = ['SQ8', 'POW2', 'FLOAT']
from utils.stats_funcs import STATS_BITS

LOG = logging.getLogger('nntool.'+__name__)


class FquantCommand(NNToolShellBase):
    # FQUANT COMMAND
    parser_fquant = Cmd2ArgumentParser()
    parser_fquant.add_argument('-f',
                               '--force_width',
                               choices=STATS_BITS, type=int, default=0,
                               help='force all layers to this bit-width in case of POW2 scheme, ' +
                               'SQ8 will automatically force 8-bits')
    parser_fquant.add_argument('-s', '--scheme',
                               type=str, choices=QUANTIZATION_SCHEMES, default='SQ8',
                               help='quantize with scaling factors (TFlite quantization-like) [default] or POW2')
    parser_fquant.add_argument('--uniform',
                               type=float, default=0.0,
                               help='Use uniform distribution for input with the specified max value')
    parser_fquant.add_argument('--num_inference',
                               type=int, default=1,
                               help='How many inferences')
    parser_fquant.add_argument('--seed',
                               type=int, default=0,
                               help='numpy random seed, default not set and inputs change every time')
    add_options_to_parser(parser_fquant)

    @with_argparser(parser_fquant)
    def do_fquant(self, args: argparse.Namespace):
        """
Attempt to calculate a fake quantization for graph using random tensors and parameters.
This is intended to allow code generation for performance testing even if no real
weights and input data are avalaible."""
        self._check_graph()
        opts = get_options_from_args(args)
        if self.replaying_history and self.history_stats:
            astats = self.history_stats
        else:
            if args.seed:
                np.random.seed(args.seed)
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
            astats = stats_collector.stats
            self._record_stats(astats)
            self.G.constant_store.fake = False

        if args.force_width:
            opts['bits'] = args.force_width

        quantizer = NewQuantizer(self.G, reset_all=True)
        quantizer.options = opts
        quantizer.schemes.append(args.scheme)
        quantizer.set_stats(astats)
        quantizer.quantize()
        LOG.info("Quantization set. Use qshow command to see it.")
