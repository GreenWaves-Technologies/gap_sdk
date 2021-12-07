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

from cmd2 import with_argparser

from interpreter.nntool_shell_base import NNToolArguementParser, NNToolShellBase, NODE_SELECTOR_HELP, no_history
from interpreter.shell_utils import output_table, table_options
from reports.quantization_reporter import QuantizationReporter

LOG = logging.getLogger('nntool.'+__name__)


class QshowCommand(NNToolShellBase):
    # QSHOW COMMAND
    parser_qshow = NNToolArguementParser()
    table_options(parser_qshow)
    parser_qshow.add_argument('step', nargs=(0, 1),
                              help='step to tune. ' + NODE_SELECTOR_HELP,
                              completer_method=NNToolShellBase.node_step_or_name_completer(allow_comma=True))

    @with_argparser(parser_qshow)
    @no_history
    def do_qshow(self, args):
        """
Show current quantization settings."""
        self._check_graph()
        self._check_quantized()
        if args.step:
            nodes, _ = self.get_node_step_or_name(args.step, allow_comma=True)
            if not nodes:
                return
        else:
            nodes = None
        tab = QuantizationReporter().report(
            self.G, self.G.quantization, nodes)
        output_table(tab, args)
