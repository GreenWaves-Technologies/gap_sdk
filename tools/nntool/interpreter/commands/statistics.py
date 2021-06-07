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

from graph.verify import graph_statistics
from interpreter.nntool_shell_base import NNToolShellBase, no_history

class StatisticsCommand(NNToolShellBase):
    # STATISTICS COMMAND
    # parser_statistics = Cmd2ArgumentParser()

    # @with_argparser(parser_statistics)
    @no_history
    def do_statistics(self, args):
        """
graph structure stats."""
        self._check_graph()
        results = graph_statistics(self.G)
        for res in results:
            self.pfeedback(res)
        self.pfeedback("")
