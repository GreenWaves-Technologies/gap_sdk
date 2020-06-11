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

from graph.matches.matches import get_fusion, get_fusions, get_pow2_match_group, get_scale8_match_group
from interpreter.nntool_shell_base import NNToolShellBase


class FusionsCommand(NNToolShellBase):
    # FUSIONS COMMAND
    def fusions_list(self):
        return [elem[0] for elem in get_fusions()]

    parser_fusions = Cmd2ArgumentParser("apply fusions to graph")
    parser_fustions_exclusive = parser_fusions.add_mutually_exclusive_group()
    parser_fustions_exclusive.add_argument('-l', '--list',
                                           action='store_true',
                                           help='list available fusions')
    parser_fustions_exclusive.add_argument('-a', '--apply',
                                           type=str,
                                           nargs='+',
                                           choices_method=fusions_list,
                                           help='apply a fusion')
    parser_fustions_exclusive.add_argument('--pow2',
                                           action='store_true',
                                           help='apply standard fusions for AutoTiler POW2 kernels')
    parser_fustions_exclusive.add_argument('--scale8',
                                           action='store_true',
                                           help='apply standard fusions for AutoTiler SQ8 kernels')

    @with_argparser(parser_fusions)
    def do_fusions(self, args):
        """
Carry out the default set of fusions on the graph"""
        self._check_graph()
        if args.list:
            self.ppaged("\n".join(["%s - %s" % (name, desc) for name, desc in get_fusions()]))
            return
        if args.apply:
            fusions = [get_fusion(name) for name in args.apply]
            if not fusions:
                self.perror('fusion %s not found' % args.apply)
                return
        elif args.pow2:
            fusions = [get_pow2_match_group()]
        elif args.scale8:
            fusions = [get_scale8_match_group()]
        else:
            self.perror("No fusion set selected. Nothing to do. Select --pow2 or --scale8.")
            return
        for fusion in fusions:
            fusion.match(self.G)
        self.G.add_dimensions()
        if self.G.quantization and not self.G.quantization.verify_quantization(self.G):
            self.G.quantization = None
