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
from interpreter.shell_utils import output_table, table_options
from quantization.symmetric.symmetric_quantizer import SymmetricQuantizer
from reports.quantization_reporter import QuantizationReporter
from stats.activation_stats_collector import ActivationStatsCollector
from stats.fake_filter_stats_collector import FakeFilterStatsCollector
from utils.stats_funcs import STATS_BITS

class FquantCommand(NNToolShellBase):
    #FQUANT COMMAND
    parser_fquant = Cmd2ArgumentParser()
    parser_fquant.add_argument('-f', '--force_width',
                               choices=STATS_BITS, default=8, type=int, help='force all layers to this width')
    table_options(parser_fquant, default_width=140)

    @with_argparser(parser_fquant)
    def do_fquant(self, args: argparse.Namespace):
        """
Attempt to calculate a fake quantization for graph using random tensors and parameters.
This is intended to allow code generation for performance testing even if no real
weights and input data are avalaible."""
        self._check_graph()
        self.G.constant_store.fake = True
        stats_collector = ActivationStatsCollector()
        input_tensors = [np.random.normal(0, 0.2, input.dims.shape)
                         for input in self.G.input_nodes()]
        stats_collector.collect_stats(self.G, input_tensors)
        astats = stats_collector.reduce_stats()
        stats_collector = FakeFilterStatsCollector()
        fstats = stats_collector.collect_stats(self.G)
        quantizer = SymmetricQuantizer(astats, fstats,
                                       force_width=args.force_width)
        qrecs = quantizer.quantize(self.G)
        self.G.quantization = qrecs
        tab = QuantizationReporter().report(self.G, qrecs)
        output_table(tab, args)
        self.G.constant_store.fake = False
