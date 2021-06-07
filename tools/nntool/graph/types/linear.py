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

import logging

import numpy as np
from quantization.multiplicative.mulbias import reorder_filter_weigths

from ..dim import Dim
from .base import (MultiplicativeBiasParameters, SingleInputAndOutput,
                   cls_op_name)

LOG = logging.getLogger("nntool." + __name__)


@cls_op_name('linear')
class FcParameters(MultiplicativeBiasParameters, SingleInputAndOutput):
    def __init__(self, *args, batch_size=1, keep_dims=False, has_bias=True,
                 ker_in_order=None, ker_out_order=None, **kwargs):

        super(FcParameters, self).__init__(*args, has_bias=True, **kwargs)
        self._keep_dims = keep_dims
        self._batch_size = batch_size
        self._batch_minor = False
        self._ker_in_order = [['c'], [
            'out_c', 'in_c'], ['out_c']] if ker_in_order is None else ker_in_order
        self._ker_out_order = [
            ['c']] if ker_out_order is None else ker_out_order

    @property
    def graph_anon_label(self):
        return ['Filt']

    @property
    def graph_label(self):
        if self.batch_size > 1:
            batch = [f'Batches {self.batch_size} {"minor" if self._batch_minor else "major"}']
        else:
            batch = []
        return [self.name, f'Filter {self.filter}'] + batch

    @property
    def batch_size(self):
        return self._batch_size

    @property
    def batch_minor(self):
        return self._batch_minor

    @batch_minor.setter
    def batch_minor(self, val):
        self._batch_minor = val

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        if self._batch_size > 1:
            if self._batch_minor:
                out_dim = Dim.named(c=self.filter.out_c,
                                    n=self._batch_size, order=True)
            else:
                out_dim = Dim.named(n=self._batch_size,
                                    c=self.filter.out_c, order=True)
        else:
            out_dim = Dim.named(c=self.filter.out_c, order=True)
        return [out_dim]

    def transpose_filter_in(self, trans: list, dim: Dim, weights_node):
        if self.batch_size > 1:
            raise ValueError(
                'transposing batched linear filter is not supported')
        weights = weights_node.value
        assert dim.size() * self.filter.out_c == weights.size, "weights do not match transpose shape"
        # add the output channel dimension to the transpose (which is only for the input)
        real_transpose = [0] + [t + 1 for t in trans]
        # transpose the filter dimensions
        if len(self.filter) > 2:
            assert len(self.filter) == len(
                real_transpose), "strange filter length"
            self.filter.transpose(real_transpose)
        if self.in_dims_hint and self.in_dims_hint[0] and len(self.in_dims_hint[0]) > 1:
            assert len(self.in_dims_hint[0]) == len(
                trans), "strange in hints length"
            # if there is an input hint then transpose the hint
            self.in_dims_hint[0] = [self.in_dims_hint[0][i] for i in trans]
        # now transpose the weights to match the new input order
        weights_node.value = np.reshape(np.transpose(np.reshape(weights, [int(self.filter.out_c)] + dim.shape),
                                                     real_transpose),
                                        [self.filter.out_c, dim.size()])

    def transpose_filter_out(self, trans: list, dim: Dim, qrec, weights_node, biases_node):
        if self.batch_size > 1:
            raise ValueError(
                'transposing batched linear filter is not supported')
        weights = weights_node.value
        biases = biases_node.value
        rest = self.filter.size()//self.filter.out_c
        assert dim.size() * rest == weights.size, "weights do not match transpose shape"
        temp = np.reshape(weights, dim.shape + [rest])
        temp = np.transpose(temp, list(trans) + [len(trans)])
        weights_node.value = np.reshape(temp, [self.filter.out_c, rest])
        temp = np.reshape(biases, dim.shape)
        temp = np.transpose(temp, trans)
        biases_node.value = np.reshape(temp, [self.filter.out_c])
        # no need to transpose filter or output dimension since it will just be [out_c]
        if qrec and qrec.ktype.startswith('scaled'):
            reorder_filter_weigths(qrec, trans, dim)

    @property
    def can_equalize(self):
        return True

    def compute_load(self):
        return self.in_dims[0].size() * self.out_dims[0].c

    def __str__(self):
        return "F {} B {} {}".format(self.filter, self.batch_size, self.at_options or "")
