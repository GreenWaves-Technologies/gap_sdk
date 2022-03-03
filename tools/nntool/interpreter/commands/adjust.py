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


class AdjustCommand(NNToolShellBase):
    # ADJUST COMMAND
    parser_adjust = Cmd2ArgumentParser()
    parser_adjust_choices = parser_adjust.add_mutually_exclusive_group()
    parser_adjust_choices.add_argument(
        '-n', '--no_postprocess',
        action='store_true', help='Don\'t try to eliminate transposes')
    parser_adjust_choices.add_argument(
        '-o', '--one_step',
        action='store_true', help='Do one step of post processing')
    parser_adjust_choices.add_argument(
        '-s', '--steps',
        type=int, help='Do fixed number of steps of post processing')
    parser_adjust_choices.add_argument(
        '-i', '--individual_step',
        action='store_true', help='Do the post processing step by step')

    @with_argparser(parser_adjust)
    def do_adjust(self, args):
        """
Adjust activation and parameter tensors to match AutoTiler order.
Must be run before generating code."""
        self._check_graph()
        if args.one_step:
            steps = 1
        elif args.steps:
            steps = args.steps
        else:
            steps = None
        self.G.adjust_order(
            no_postprocess=args.no_postprocess, steps=steps, single_step=args.individual_step)
        self.G.add_dimensions()
