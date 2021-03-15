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
from quantization.quantization_set import QuantizationSet
from quantization.tune_type import tune_type
from quantization.tuneq import tuneq


class QtuneCommand(NNToolShellBase):
    def node_step_or_name_completer(self, text: str, line: str, begidx: int, endidx: int):
        if text.isnumeric() or text == "*":
            return text
        return [node.name for node in self.G.nodes() if node.name.startswith(text)]

    def get_node_step_or_name(self, arg):
        if arg.isnumeric():
            arg = int(arg)
            if arg < 0 or arg > len(self.G.steps):
                self.perror('step number not in graph')
                return [], ""
            node = self.G.steps[arg]['node']
            return [node], f'step {arg} {node.name}'
        elif arg == '*':
            return self.G.nodes(), 'all nodes'
        elif arg in self.G:
            return [self.G[arg]], arg
        else:
            self.perror('node name not in graph')
            return []

    # QTUNE COMMAND
    parser_tune = Cmd2ArgumentParser()
    parser_tune.add_argument('step',
                             help='step to tune',
                             completer_method=node_step_or_name_completer)
    parser_tune_sub = parser_tune.add_subparsers(help="different elements that can be tuned",
                                                 required=True)
    parser_tune_q = parser_tune_sub.add_parser('param', help='tune quantization parameters')
    parser_tune_q.set_defaults(tune='q')
    parser_tune_q.add_argument('parameter',
                               choices=['acc', 'calc', 'weights', 'biases', 'dp', 'out'],
                               help='which parameter to tune')
    parser_tune_q.add_argument('X',
                               nargs='?',
                               default=0,
                               type=int, help='X of QX.Y')
    parser_tune_q.add_argument('Y',
                               nargs='?',
                               default=0,
                               type=int, help='Y of QX.Y')
    parser_tune_q.add_argument('index',
                               nargs='?',
                               default=0,
                               type=int, help='edge index')
    parser_tune_q.add_argument('-f',
                               '--sub_step_fusion',
                               type=int,
                               help='index of the subnode for qtune inside of a fused one')
    parser_tune_flt = parser_tune_sub.add_parser('set', help='set quantization type for node')
    parser_tune_flt.set_defaults(tune='set')
    parser_tune_flt.add_argument('type',
                                 choices=['ieee32', 'ieee16', 'bfloat16'],
                                 help='different container types (limited currently to float types)')

    @with_argparser(parser_tune)
    def do_qtune(self, args):
        """
Tune quantization of graph."""
        self._check_graph()
        nodes, node_descr = self.get_node_step_or_name(args.step)
        if not nodes:
            return
        if args.tune == 'q':
            self._check_quantized()
            for node in nodes:
                tuneq(self.G, self.G.quantization, node,
                      args.parameter, args.X, args.Y, index=args.index)
        elif args.tune == 'set':
            if not self.G.quantization:
                self.G.quantization = QuantizationSet()
            for node in nodes:
                tune_type(self.G, self.G.quantization, node, args.type)
            self.pfeedback(f'quantization changed to {args.type} on {node_descr}')
        else:
            self.perror(f'{args.tune} subcommand invalid')
