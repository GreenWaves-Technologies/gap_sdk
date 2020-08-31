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
import argparse
from itertools import chain
from cmd2 import Cmd2ArgumentParser, with_argparser
from utils.node_id import NodeId
from interpreter.nntool_shell_base import NNToolShellBase
from graph.types import InputOutputParameters


LOG = logging.getLogger("nntool")


class FreezeCommand(NNToolShellBase):
    def inputs_and_outputs(self):
        if self.G is None:
            return []
        return [node.name for node in chain(self.G.inputs_and_constants(), self.G.outputs())]

    # FREEZE COMMAND
    parser_freeze = Cmd2ArgumentParser("toggle freezing of channel order of inputs or outputs")
    parser_freeze.add_argument('node_names',
                               nargs='+',
                               choices_method=inputs_and_outputs,
                               help='input or output node names to toggle freeze')

    @with_argparser(parser_freeze)
    def do_freeze(self, args: argparse.Namespace):
        """
Toggle freezing of channel order on inputs and outputs. When graph is adjusted frozen nodes
 will not change channel order."""
        self._check_graph()
        nodes = [self.G.node(node_name) for node_name in args.node_names]
        if not all([isinstance(node, InputOutputParameters) for node in nodes]):
            self.perror("all nodes should be inputs or outputs")
            return

        for node in nodes:
            if node.fixed_order:
                LOG.info("node %s is unfrozen", node.name)
                node.fixed_order = False
            else:
                LOG.info("node %s is frozen", node.name)
                node.fixed_order = True
            self.G.node_options[NodeId(node)] = node.at_options
