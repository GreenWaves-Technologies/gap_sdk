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

import argparse
import logging

from cmd2 import Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from quantization.symmetric.kernels.activations import (get_force_relu,
                                                        set_force_relu)

LOG = logging.getLogger('nntool.'+__name__)

class NntoolForceReluCommand(NNToolShellBase):
    # NNOOL FORCE RELU during execution COMMAND

    parser_force_relu = Cmd2ArgumentParser()
    parser_force_relu.add_argument('force_relu',
                                   choices=['on', 'off'],
                                   nargs=(0, 1),
                                   help='switch force_relu on or off')

    @with_argparser(parser_force_relu)
    def do_force_relu(self, args: argparse.Namespace):
        """
Switch force_relu on and off in quantized calculations."""
        if args.force_relu is not None:
            set_force_relu(args.force_relu == 'on')
        LOG.info("force relu is %s", 'on' if get_force_relu() else 'off')
