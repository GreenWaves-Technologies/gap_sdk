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

def find_seq(trans):
    last = None
    for idx, dim in enumerate(trans):
        if last is not None and dim == last + 1:
            return idx - 1
        last = dim
    return None

def remove_sequences(shape, trans):
    seq_idx = find_seq(trans)
    while seq_idx is not None:
        seq_start = trans[seq_idx]
        shape = shape[:seq_start] + [shape[seq_start]*shape[seq_start+1]] + shape[seq_start+2:]
        trans = [idx if idx < seq_start else idx - 1 for idx in trans if idx != seq_start]
        seq_idx = find_seq(trans)
    return shape, trans

def remove_unit_dims(shape, trans):
    unit_idx = next((idx for idx, dim in enumerate(shape) if dim == 1), None)
    while unit_idx is not None:
        shape = shape[0:unit_idx] + shape[unit_idx+1:]
        trans = [idx if idx < unit_idx else idx - 1 for idx in trans if idx != unit_idx]
        unit_idx = next((idx for idx, dim in enumerate(shape) if dim == 1), None)
    return shape, trans

def real_transpose(shape, trans):
    shape, trans = remove_unit_dims(list(shape), list(trans))
    shape, trans = remove_sequences(shape, trans)
    return tuple(shape), tuple(trans)
