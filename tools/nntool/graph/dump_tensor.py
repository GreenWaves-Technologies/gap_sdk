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

from abc import ABC, abstractmethod
import numpy as np

class Dumper(ABC):

    @abstractmethod
    def group(self, tensor):
        pass

    @abstractmethod
    def element(self, elem):
        pass

class PrintDumper(Dumper):

    def __init__(self, tensor, width=12, precision=6):
        if np.issubdtype(tensor.dtype, np.floating):
            self.fmt = " {:{width}.{precision}f} "
            self.fmt_args = {"width": width, "precision": precision}
        elif np.issubdtype(tensor.dtype, np.unsignedinteger) or\
            np.issubdtype(tensor.dtype, np.signedinteger):
            self.fmt = " {:{width}d} "
            self.fmt_args = {"width": width}

    def group(self, tensor):
        print()

    def element(self, elem):
        print(self.fmt.format(elem, **self.fmt_args), end="")

def dump_tensor(tensor, dumper):

    if len(tensor.shape) > 1:
        for i in range(tensor.shape[0]):
            ten = tensor[i]
            dump_tensor(ten, dumper)
            dumper.group(ten)
        return
    for i in range(tensor.shape[0]):
        dumper.element(tensor[i])

def print_tensor(tensor):

    dump_tensor(tensor, PrintDumper(tensor))
