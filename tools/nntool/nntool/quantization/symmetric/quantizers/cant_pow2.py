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
from nntool.graph.types import (RNNNodeBase,
                         SSDDetectorNode)
from nntool.quantization.unified_quantization_handler import params_type

from ..pow2_quantization_handler import Pow2QuantizionHandler

LOG = logging.getLogger('nntool.' + __name__)

@params_type(RNNNodeBase, SSDDetectorNode)
class CantQuantizePow2(Pow2QuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        LOG.error(f'no support for activation {params.__class__.__name__} in POW2 quantizer')
        return None
