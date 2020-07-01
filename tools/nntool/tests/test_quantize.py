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

from quantization.qtype import QType
from quantization.quantized_ndarray import QuantizedNdarray
from utils.stats_funcs import bits

TEST_QTYPE = QType(bits=8, q=7, signed=True)

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

def test_instantiation():
    shape = (10, 2, 5)
    subclass = QuantizedNdarray(np.zeros(shape), qtype=TEST_QTYPE)

    assert isinstance(subclass, np.ndarray)
    assert subclass.qtype == TEST_QTYPE
    assert subclass.shape == shape
    assert  np.array_equal(subclass, np.zeros(shape))

def test_view_casting():
    shape = (10, 2, 5)
    assert isinstance(np.zeros(shape).view(QuantizedNdarray), QuantizedNdarray)

def test_new_from_template():
    shape = (10, 2, 5)
    subclass = QuantizedNdarray(np.zeros(shape), qtype=TEST_QTYPE)
    bla = subclass[3, :]
    assert isinstance(bla, QuantizedNdarray)
    assert bla.qtype == TEST_QTYPE

def test_np_min():
    shape = (10, 2, 5)
    subclass = QuantizedNdarray(np.zeros(shape), qtype=TEST_QTYPE)
    assert np.min(subclass) == 0

def test_ufuncs():
    shape = (10, 2, 5)
    subclass = QuantizedNdarray(np.zeros(shape), qtype=TEST_QTYPE)
    subclass += 2
    assert np.all(subclass == 2)
    subclass = subclass + np.ones(shape)
    assert np.all(subclass == 3)
    np.multiply.at(subclass, slice(0, 2), 2)
    assert np.all(subclass[:2] == 6)
    assert np.all(subclass[2:] == 3)
#pylint: disable=no-member
    assert subclass.qtype == TEST_QTYPE
    assert isinstance(subclass.reshape((10, 10)), QuantizedNdarray)

def test_output():
    shape = (10, 2, 5)
    subclass = QuantizedNdarray(np.ones(shape), qtype=TEST_QTYPE)
    bla = np.ones(shape)
    bla *= 2
    np.multiply(bla, bla, out=subclass)
    assert np.all(subclass == 4)
    assert subclass.qtype == TEST_QTYPE
