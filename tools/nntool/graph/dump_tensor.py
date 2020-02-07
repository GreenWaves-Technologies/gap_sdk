# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
