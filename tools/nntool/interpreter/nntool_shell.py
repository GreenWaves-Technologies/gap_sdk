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

from interpreter.nntool_shell_base import NNToolShellBase

# pylint: disable=wildcard-import,unused-wildcard-import
from .commands import *

LOG = logging.getLogger("nntool")

VALID_LOG_LEVELS = [
    "INFO",
    "DEBUG",
    "WARNING"
]

COMMANDS = NNToolShellBase.__subclasses__()


class CommandMixer(type):
    def __new__(cls, name, base, ns):
        return type.__new__(cls, name, tuple(COMMANDS + list(base)), ns)


class NNToolShell(NNToolShellBase, metaclass=CommandMixer):
    intro = 'Welcome to NNTOOL. Type help or ? to list commands.\n'
    prompt = '(NNT) '
