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
from quantization.tuneq import tuneq

class QtuneCommand(NNToolShellBase):
    # QTUNE COMMAND
    parser_tune = Cmd2ArgumentParser()
    parser_tune.add_argument('step',
                             type=int, help='step to tune')
    parser_tune.add_argument('parameter',
                             choices=['acc', 'calc', 'weights', 'biases', 'dp', 'out'],
                             help='which parameter to tune')
    parser_tune.add_argument('X',
                             nargs='?',
                             default=0,
                             type=int, help='X of QX.Y')
    parser_tune.add_argument('Y',
                             nargs='?',
                             default=0,
                             type=int, help='Y of QX.Y')
    parser_tune.add_argument('index',
                             nargs='?',
                             default=0,
                             type=int, help='edge index')
    parser_tune.add_argument('-f',
                             '--sub_step_fusion',
                             type=int,
                             help='index of the subnode for qtune inside of a fused one')

    @with_argparser(parser_tune)
    def do_qtune(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        self._check_quantized()

        tuneq(self.G, self.G.quantization, args.step,
              args.parameter, args.X, args.Y, index=args.index)
