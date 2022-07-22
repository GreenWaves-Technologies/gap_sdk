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
from nntool.interpreter.nntool_shell_base import NODE_SELECTOR_HELP, NNToolShellBase
from nntool.graph.manipulations.extract import extract_node

class ExtractCommand(NNToolShellBase):
    # EXTRACT COMMAND
    parser_extract = Cmd2ArgumentParser()
    parser_extract.add_argument('step',
                              help='step to extract. ' + NODE_SELECTOR_HELP,
                              completer_method=NNToolShellBase.node_step_or_name_completer(allow_comma=True))

    @with_argparser(parser_extract)
    def do_extract(self, args):
        """
Extracts a single step out of a graph and forms a new graph with inputs (and constants) and outputs to this step."""
        self._check_graph()
        nodes, _ = self.get_node_step_or_name(args.step, allow_comma=True)
        if not nodes:
            self.perror('node not found')
        if len(nodes) > 1:
            self.perror('only one node can be extracted')
        extract_node(self.G, nodes[0])
