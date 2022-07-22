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

import numpy as np

class Rounding:
    DO_ROUNDING = True

def set_do_rounding(do_rounding: bool):
    Rounding.DO_ROUNDING = do_rounding

def get_do_rounding():
    return Rounding.DO_ROUNDING

def at_norm(val, norm):
    if isinstance(norm, np.ndarray):
        if np.any(norm < 0):
            raise ValueError("negative normalization")
        if Rounding.DO_ROUNDING:
            return np.where(norm == 0, val, (val + np.left_shift(1, norm - 1, dtype=val.dtype)) >> norm)
            # broadcast = np.broadcast(val, norm)
            # res = np.empty(broadcast.shape, dtype=val.dtype)
            # res.flat = [(v + (1 << n - 1)) >> n if n > 0 else v for v, n in broadcast]
            # return res
        return val >> norm
    else:
        if norm < 0:
            raise ValueError("negative normalization")
        if Rounding.DO_ROUNDING and norm > 0:
            return val if norm == 0 else (val + (1 << (norm - 1))) >> norm
        return val >> norm
