# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
