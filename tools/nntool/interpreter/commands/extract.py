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
from graph.manipulations.extract import extract_node

class ExtractCommand(NNToolShellBase):
    # EXTRACT COMMAND
    parser_extract = Cmd2ArgumentParser()
    parser_extract.add_argument('step',
                                type=int,
                                help='step number to extract')

    @with_argparser(parser_extract)
    def do_extract(self, args):
        """
Extracts a single step out of a graph and forms a new graph with inputs and outputs to this step."""
        self._check_graph()
        if args.step < 0 or args.step > len(self.G.graph_state.steps):
            self.perror("step must be between 0 and {}".format(len(self.G.graph_state.steps)))
        extract_node(self.G, self.G.graph_state.steps[args.step]['node'])
