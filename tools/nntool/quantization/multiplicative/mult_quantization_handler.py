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
from ..unified_quantization_handler import QuantizionHandler, options
from .mult_quantization import MultQuantizationRecordBase


#pylint: disable=abstract-method
@options(
    {
        'name': 'bits',
        'type': int,
        'choices': [8],
        'help': 'bits for inputs and outputs',
        'default': 8
    })
class MultQuantizionHandler(QuantizionHandler):
    DEFAULT_QREC = MultQuantizationRecordBase
    NAME = "SQ8"

    BITS_TO_DTYPE = {
        8: np.int8,
        16: np.int16
    }

    @classmethod
    def get_mult_opts(cls, **kwargs):
        force_out_qs = kwargs.get('force_out_qs', None)
        opts = kwargs.get('opts', {})
        bits = opts.get('bits', 8)
        return force_out_qs, cls.BITS_TO_DTYPE[bits]