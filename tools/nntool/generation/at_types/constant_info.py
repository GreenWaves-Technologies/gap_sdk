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

# typedef struct {
# 	char *FileName;	/* Name of the file containing the initial values */
# 	int Format;	/* Float or fixed point */
# 	int Binary;	/* 1: file content is binary, 0: file content is text */
# 	int Size;	/* When Format is Fract Size in bytes of the container */
# 	int Fract;	/* When format is fract position of the point */
# } ConstInit_T;

import numpy as np

from quantization.qtype_base import QTypeBase


class ConstantInfo():
    FMT_TYPES = {
        'float': 0,
        'fixed': 1
    }

    def __init__(self, file_name: str, qtype: QTypeBase, numeric_format: str = "fixed", is_binary: bool = True, contents: np.ndarray = None):
        self._file_name = file_name
        self._qtype = qtype
        assert numeric_format in self.FMT_TYPES, "invalid numeric format"
        self._numeric_format = numeric_format
        self._is_binary = is_binary
        self._contents = contents

    @property
    def contents(self):
        return self._contents

    @contents.setter
    def contents(self, val):
        self._contents = val

    @property
    def file_name(self):
        return self._file_name

    @property
    def qtype(self):
        return self._qtype

    def __str__(self):
        return 'ConstInfo("{0}", {1}, 1, {2}, {3})'.format(self._file_name,
                                                           self.FMT_TYPES[self._numeric_format],
                                                           self._qtype.bits//8,
                                                           self._qtype.q)
