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
from graph.types import SSDDetectorParameters

class SSDSetterCommand(NNToolShellBase):
    # SSD Setter command to set the ssd post process layer parameters

    parser_ssdsetter = Cmd2ArgumentParser()
    parser_ssdsetter.add_argument('parameter',
                                  choices=('max_bb_before_nms', 'nms_score_threshold'),
                                  help='Set this parameter')
    parser_ssdsetter.add_argument('value', type=int, default=None,
                                  help='Set to this value')

    @with_argparser(parser_ssdsetter)
    def do_ssd_setter(self, args):
        assert self.G.has_ssd_postprocess, "No SSD postprocess layer present in the graph"
        node = [node for node in self.G.nodes() if isinstance(node, SSDDetectorParameters)][0]
        if args.value is None:
            print('{} is set to {}'.format(args.parameter, getattr(node, args.parameter)))
        try:
            setattr(node, args.parameter, args.value)
        except:
            raise ValueError('Invalid parameter {}'.format(args.parameter))
