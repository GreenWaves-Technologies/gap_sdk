# Copyright (C) 2021  GreenWaves Technologies, SAS

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

import numpy as np


def interleave(*args):
    num_args = len(args)
    assert all(len(arg.shape) == 1 for arg in args)
    assert all(arg.shape[0] == args[0].shape[0] for arg in args[1::])
    res = np.empty(sum(arg.size for arg in args), dtype=args[0].dtype)
    for i in range(num_args):
        res[i::num_args] = args[i]
    return res


def packbits(value, bits):
    return np.packbits(
        np.unpackbits(
            value.flatten(),
            bitorder='little'
        ).reshape(
            value.size, 8
        )[:, 0:bits:].flatten(),
        bitorder='little'
    )
