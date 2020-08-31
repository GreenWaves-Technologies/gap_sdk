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
from .commands.dump import DumpCommand, RoundingCommand
from .commands.tensors import TensorsCommand
from .commands.validation import ValidationCommand
from .commands.gen import GenCommand
from .commands.nodeoption import NodeoptionCommand
from .commands.adjust import AdjustCommand
from .commands.freeze import FreezeCommand
from .commands.fusions import FusionsCommand
from .commands.graph import GraphCommand
from .commands.imageformat import ImageFormatCommand
from .commands.open import OpenCommand
from .commands.save_state import SaveStateCommand
from .commands.aquant import AquantCommand
from .commands.fquant import FquantCommand
from .commands.qerror import QerrorCommand
from .commands.qtune import QtuneCommand
from .commands.range_equalization import (
    BalanceFiltersCommand, BcorrCommand, WeightEqualizationCommand)
from .commands.qshow import QshowCommand
from .commands.astats import AstatsCommand
from .commands.temps import TempsCommand
from .commands.ssd_setter import SSDSetterCommand

LOG = logging.getLogger("nntool")


VALID_LOG_LEVELS = [
    "INFO",
    "DEBUG",
    "WARNING"
]

COMMANDS = [
    AquantCommand,
    QshowCommand,
    DumpCommand,
    TensorsCommand,
    ValidationCommand,
    WeightEqualizationCommand,
    OpenCommand,
    SaveStateCommand,
    GenCommand,
    NodeoptionCommand,
    AdjustCommand,
    FreezeCommand,
    FusionsCommand,
    GraphCommand,
    FquantCommand,
    QerrorCommand,
    BalanceFiltersCommand,
    BcorrCommand,
    AstatsCommand,
    TempsCommand,
    RoundingCommand,
    QtuneCommand,
    ImageFormatCommand,
    SSDSetterCommand
]


class CommandMixer(type):
    def __new__(cls, name, base, ns):
        return type.__new__(cls, name, tuple(COMMANDS + list(base)), ns)


class NNToolShell(NNToolShellBase, metaclass=CommandMixer):
    intro = 'Welcome to NNTOOL. Type help or ? to list commands.\n'
    prompt = '(NNT) '
