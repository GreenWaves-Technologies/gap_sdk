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

from nntool.interpreter.nntool_shell_base import NNToolShellBase, no_history
from nntool.quantization.verify_quantization import verify_quantization

from nntool.graph.verify import verify_graph


class VerifyCommand(NNToolShellBase):
    # VERIFY COMMAND
    # parser_verify = Cmd2ArgumentParser()

    # @with_argparser(parser_verify)
    @no_history
    def do_verify(self, args):
        """
Verify graph structure and quantization."""
        self._check_graph()
        results = verify_graph(self.G)
        if results:
            self.perror("graph structure is not OK - please report this:")
            for result in results:
                self.poutput(result)
            return
        self.pfeedback("graph structure is OK")
        if self.G.quantization is None:
            self.pfeedback("skipping quantization check - graph is not quantized")
            return
        
        results = verify_quantization(self.G)
        if results:
            self.perror("graph quantization is not OK - please report this:")
            for result in results:
                self.poutput(result)
            return
        self.pfeedback("graph quantization is OK")
