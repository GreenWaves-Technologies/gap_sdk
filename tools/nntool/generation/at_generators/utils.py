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

def at_bits(qtype):
    if qtype is None:
        return 0
    # 1: byte, 2: half word, 4: word
    if qtype.bits == 8:
        return 1
    if qtype.bits == 16:
        return 2
    if qtype.bits == 32:
        return 4
    raise NotImplementedError("unsupported number of bits")


def at_q(qtype):
    if qtype is None:
        return 0
    return qtype.q


def at_bits_and_q(qtype):
    return "{}, {}".format(at_bits(qtype), qtype.q)