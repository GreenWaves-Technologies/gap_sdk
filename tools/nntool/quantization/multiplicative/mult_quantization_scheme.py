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

from ..unified_quantization_scheme_base import UnifiedQuantizationSchemeBase
from .mult_quantization_handler import MultQuantizionHandler
# pylint: disable=wildcard-import, unused-wildcard-import
from .quantizers import *
from .mult_quantization import MultQuantizationRecord, MultQuantizationRecordBase


#pylint: disable=abstract-method
class MultQuantizationScheme(UnifiedQuantizationSchemeBase):
    DEFAULT_QREC = MultQuantizationRecord
    QREC_BASE = MultQuantizationRecordBase
    NAME = "SQ8"
    HANDLERS_BASE = MultQuantizionHandler
