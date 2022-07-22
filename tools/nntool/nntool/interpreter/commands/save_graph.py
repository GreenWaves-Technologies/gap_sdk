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

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from nntool.graph.nngraph import NNGraph
from nntool.interpreter.nntool_shell_base import NNToolShellBase, no_history

LOG = logging.getLogger("nntool")


class SaveGraphCommand(NNToolShellBase):
    # SAVE_GRAPH COMMAND
    parser_save_graph = Cmd2ArgumentParser()
    parser_save_graph.add_argument('filename',
                                   completer_method=Cmd.path_complete,
                                   help='file to write to')

    @with_argparser(parser_save_graph)
    @no_history
    def do_savegraph(self, args):
        """
Save the graph. A python zip module is written containing the graph and its tensors"""
        self._check_graph()
        filepath = args.filename
        if not filepath.endswith('.zip'):
            filepath = f'{filepath}.zip'
        self.G.write_graph_state(filepath)
        LOG.info(f"saved graph to {filepath}")

class LoadGraphCommand(NNToolShellBase):
    # LOAD_GRAPH COMMAND
    parser_load_graph = Cmd2ArgumentParser()
    parser_load_graph.add_argument('filename',
                                   completer_method=Cmd.path_complete,
                                   help='file to read from')

    @with_argparser(parser_load_graph)
    @no_history
    def do_loadgraph(self, args):
        """
Loads a graph from file written by the save_graph command"""
        filepath = args.filename
        G = NNGraph.read_graph_state(filepath)
        # update the settings with the new graph settings
        self.settings.update(G.settings)
        self.G = G # this has side effect of making G.settings point to nntool shell settings
        LOG.info(f"loaded graph from {filepath}")
