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
import configparser
import os
from functools import partial
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import filter_dirs
from generation.autotiler_options import DEFAULT_GEN_OPTS
from utils.new_param_state import dump_state

class SaveStateCommand(NNToolShellBase):
    # SAVE_STATE COMMAND
    parser_save_state = Cmd2ArgumentParser()
    parser_save_state.add_argument('output',
                                   completer_method=Cmd.path_complete,
                                   nargs=argparse.OPTIONAL,
                                   help='file to write to')

    @with_argparser(parser_save_state)
    def do_save_state(self, args):
        """
Save the state of the transforms and quantization of the graph.
This state file can be used to generate the model file as part of
a build script. If no argument is given then the state files
will be saved in the same directory as the graph. If a directory is
given then the state files will be saved in it with the graph
basename. If a filename is given, its basename will be used to
save the state files."""
        self._check_graph()
        self._check_quantized()
        gen_opts = {k: self.settings[k] for k in DEFAULT_GEN_OPTS}
        dump_state(self.G, state_path=args.output, extra=gen_opts)
