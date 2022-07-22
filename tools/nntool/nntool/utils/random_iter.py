# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from typing import Sequence
import typing
import numpy as np

if typing.TYPE_CHECKING:
    from nntool.graph.nngraph import NNGraph
class RandomIter():
    """ Value generator using a randomm number source
    """
    def __init__(self,
                 count: int,
                 shapes: Sequence[Sequence[int]],
                 ranges: Sequence[Sequence[float]],
                 gen: np.random.Generator=None) -> None:
        """_summary_

        Args:
            count (int): Number of samples
            shapes (Sequence[Sequence[int]]): Shapes of inputs. G.inputs() should be used to get input order
            ranges (Sequence[Sequence[float]]): Ranges of onputs as a pair of floats for each input
            gen (np.random.Generator, optional): Numpy random number generator to use. Defaults to None.
        """
        self._vars = list(zip(shapes, ranges))
        self._gen = gen
        self._count = count
        self._cur = count
        if self._gen is None:
            self._gen = np.random.default_rng()

    @classmethod
    def fake(cls, G: 'NNGraph', gen: np.random.Generator=None) -> 'RandomIter':
        """ Create instance that matches graph G inputs

        Args:
            G (NNGraph): Graph to create input values for
            gen (np.random.Generator, optional): Numpy random number generator to use. Defaults to None.

        Returns:
            RandomIter: A random input generator that produces inputs for G
        """
        shapes = [node.out_dims[0].shape for node in G.input_nodes()]
        ranges = [(-1, 1)] * len(shapes)
        return cls(1, shapes, ranges, gen=gen)

    def __iter__(self):
        self._cur = self._count
        return self

    def __next__(self):
        if self._cur <= 0:
            raise StopIteration()
        self._cur -= 1
        return self.val()

    def val(self):
        res = []
        for shape, (minv, maxv) in self._vars:
            res.append((self._gen.random(shape) * (maxv - minv)) + minv)
        return res
