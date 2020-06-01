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

from utils.stats_funcs import bits

IMAGE_FILE = "examples/0/136.pgm"

def test_bits():
    assert bits(0.081599854, -0.07628916) == 1
    assert bits(1.081599854, -0.07628916) == 2
    assert bits(2.081599854, -0.07628916) == 3
    assert bits(256.081599854, -0.07628916) == 10
    assert bits(75, -9) == 8
    assert bits(0, 0) == 1
    assert bits(-1, 0) == 1
    assert bits(-1, 1) == 2
