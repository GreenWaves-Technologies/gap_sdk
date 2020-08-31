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

from graph.dim import Dim
from utils.formatters import FORMAT_CHANGES, NORMALIZATIONS
from utils.ssd_postprocess_decoder import DecodeBboxes
from utils.ssd_postprocess_nms import NonMaxSuppression

from .base import (NoSizeChangeParameters, Parameters,
                   SameNumberOfDimensionsForInputs, SensitiveToOrder,
                   SingleInputAndOutput, Transposable)

LOG = logging.getLogger("nntool." + __name__)

class ImageFormatParameters(Parameters, SingleInputAndOutput, SensitiveToOrder):
    op_name = "image_format"
    NORMALIZATIONS = NORMALIZATIONS
    FORMAT_CHANGES = FORMAT_CHANGES

    def __init__(self, *args, norm_func=None, format_change=None, **kwargs):
        self._norm_func = None
        self._format_change = None
        super(ImageFormatParameters, self).__init__(*args, **kwargs)
        self.norm_func = norm_func
        self.format_change = format_change

    @property
    def input_channels(self):
        if self.format_change in ("RGB565_RGB888", "BW8", "BW16"):
            return 1
        if self.format_change in ("RGB888", "RGB16"):
            return 3
        return None

    @property
    def input_dtype(self):
        if self.format_change == "RGB565_RGB888":
            return np.int16
        if self.format_change in ("RGB888", "BW8", "BW16", "RGB16"):
            return np.uint8
        return None

    @property
    def output_channels(self):
        if self.format_change in ("RGB565_RGB888", "RGB888", "RGB16"):
            return 3
        if self.format_change in ("BW8", "BW16"):
            return 1
        return None

    @property
    def output_dtype(self):
        if self.norm_func in ("SHIFT_INT8", "OFFSET_INT8"):
            return np.int8
        if self.norm_func in "OUT_INT16":
            return np.int16
        return None

    @property
    def format_change(self):
        # RGB565_RGB888
        return self._format_change

    @format_change.setter
    def format_change(self, val):
        val = val and val.upper()
        if val is not None and val not in self.FORMAT_CHANGES:
            raise ValueError("format change is not valid")
        self._format_change = val

    @property
    def norm_func(self):
        # None, "shift", "offset"
        return self._norm_func

    @norm_func.setter
    def norm_func(self, val):
        val = val and val.upper()
        if val is not None and val not in self.NORMALIZATIONS:
            raise ValueError("normalization is not valid")
        self._norm_func = val

    def get_parameter_size(self):
        return 0

    def get_output_size(self, in_dims):
        assert len(in_dims) == 1
        self.in_dims = self.clone_dim_with_hints(in_dims, hint_dir='in')
        out_dim = self.clone_dim_with_hints(in_dims, hint_dir='out')[0]
        if self.format_change == "RGB565_RGB888":
            assert out_dim.is_named and out_dim.c == 1
            out_dim.impose_order(self.out_dims_hint[0])
            out_dim.c = 3
        elif self.format_change in ("BW8", "BW16"):
            pass  # no dims change here
        elif self.format_change in ("RGB888", "RGB16"):
            assert out_dim.is_named and out_dim.c == 3
            out_dim.impose_order(self.out_dims_hint[0])
        else:
            raise ValueError("unknow format change")

        return [out_dim]

    @property
    def can_equalize(self):
        return False

    def clone(self, name, groupn=None):
        raise NotImplementedError()

    def __str__(self):
        return "FORMAT_CHANGE Fmt: {} Norm: {}".format(self.format_change, self.norm_func)
