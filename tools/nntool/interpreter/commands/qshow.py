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

import logging

from cmd2 import Cmd2ArgumentParser, with_argparser

from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import output_table, table_options
from reports.quantization_reporter import QuantizationReporter

LOG = logging.getLogger('nntool.'+__name__)

class QshowCommand(NNToolShellBase):
    # QSHOW COMMAND
    parser_qshow = Cmd2ArgumentParser()
    table_options(parser_qshow)
    parser_qshow.add_argument('step', type=int, nargs=(0, 1), help='Limit to step number')
    parser_qshow.add_argument('-s', '--show_wrapped',
                              action='store_true',
                              help='show original quantization parameters on multiplicative quantization')

    @with_argparser(parser_qshow)
    def do_qshow(self, args):
        """
Show current quantization settings."""
        self._check_graph()
        self._check_quantized()
        tab = QuantizationReporter(step=args.step,
                                   emit_wrapped=args.show_wrapped).report(self.G,
                                                                          self.G.quantization)
        output_table(tab, args)
