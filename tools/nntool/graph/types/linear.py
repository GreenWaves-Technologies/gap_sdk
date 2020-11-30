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
from quantization.multiplicative.mult_quantization import \
    MultScalableFilterQuantizationRecord

from ..dim import Dim
from .base import MultiplicativeBiasParameters, SingleInputAndOutput

LOG = logging.getLogger("nntool." + __name__)


class FcParameters(MultiplicativeBiasParameters, SingleInputAndOutput):
    op_name = "linear"
    def __init__(self, *args, keep_dims=False, **kwargs):

        super(FcParameters, self).__init__(*args, **kwargs)
        LOG.debug("created LINEAR %s", str(self))
        self._keep_dims = keep_dims

    def get_parameter_size(self):
        return self.filter.size() + (self.has_bias and self.filter.out_c or 0)

    def get_output_size(self, in_dims):

        assert len(in_dims) == 1
        self.in_dims = in_dims

        out_dim = Dim.named(c=self.filter.out_c, order=True)
        return [out_dim]

    def transpose_filter_in(self, trans: list, dim: Dim):
        if self.weights is None:
            return
        assert dim.size() * self.filter.out_c == self.weights.size, "weights do not match transpose shape"
        # add the output channel dimension to the transpose (which is only for the input)
        real_transpose = [0] + [t + 1 for t in trans]
        # transpose the filter dimensions
        if len(self.filter) > 2:
            assert len(self.filter) == len(real_transpose), "strange filter length"
            self.filter.transpose(real_transpose)
        if self.in_dims_hint and self.in_dims_hint[0] and len(self.in_dims_hint[0]) > 1:
            assert len(self.in_dims_hint[0]) == len(trans), "strange in hints length"
            # if there is an input hint then transpose the hint
            self.in_dims_hint[0] = [self.in_dims_hint[0][i] for i in trans]
        # now transpose the weights to match the new input order
        self.weights = np.reshape(np.transpose(np.reshape(self.weights, [int(self.filter.out_c)] + dim.shape),
                                               real_transpose),
                                  [self.filter.out_c, dim.size()])

    def transpose_filter_out(self, trans: list, dim: Dim, qrec):
        if self.weights is None:
            return
        rest = self.filter.size()//self.filter.out_c
        assert dim.size() * rest == self.weights.size, "weights do not match transpose shape"
        temp = np.reshape(self.weights, dim.shape + [rest])
        temp = np.transpose(temp, list(trans) + [len(trans)])
        self.weights = np.reshape(temp, [self.filter.out_c, rest])
        temp = np.reshape(self.biases, dim.shape)
        temp = np.transpose(temp, trans)
        self.biases = np.reshape(temp, [self.filter.out_c])
        # no need to transpose filter or output dimension since it will just be [out_c]
        if isinstance(qrec, MultScalableFilterQuantizationRecord):
            qrec.reorder_weigths(trans, dim)

    @property
    def can_equalize(self):
        return True

    def clone(self, name, groupn=None):
        return FcParameters(name, filt=self.filter.clone(), has_bias=self.has_bias)

    def compute_load(self):
        return self.in_dims[0].size() * self.out_dims[0].c

    def __str__(self):
        return "F {} {}".format(self.filter, self.at_options or "")
