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

import json
import os

import xxhash


# This class tracks any changes to the graph that render it incompatible with a value cache entry
class GraphIdentity():
    def __init__(self, filename):
        if filename is not None:
            filename = os.path.abspath(filename)
        self._identity = {'filename': filename, 'fusions': []}

    @property
    def extracted_step(self):
        return self._identity.get('extracted_step')

    @extracted_step.setter
    def extracted_step(self, val):
        self._identity['extracted_step'] = val

    @property
    def identity(self):
        return self._identity

    @identity.setter
    def identity(self, val):
        self._identity = val

    @property
    def filename(self):
        return self._identity['filename']

    @property
    def is_fused(self):
        return bool(self._identity['fusions'])

    @property
    def fusions(self):
        return self._identity['fusions']

    @property
    def is_adjusted(self):
        return 'is_adjusted' in self._identity and self._identity['is_adjusted']

    @is_adjusted.setter
    def is_adjusted(self, val):
        self._identity['is_adjusted'] = val

    @property
    def is_dequantized(self):
        return 'is_dequantized' in self._identity and self._identity['is_dequantized']

    @is_dequantized.setter
    def is_dequantized(self, val):
        self._identity['is_dequantized'] = val

    @property
    def is_equalized(self):
        return 'equalization' in self._identity

    def set_equalized(self, threshold):
        self._identity['equalization'] = threshold

    @property
    def tflite_quantization(self):
        return self._identity.get('tflite_quantization')

    @tflite_quantization.setter
    def tflite_quantization(self, val: bool):
        self._identity['tflite_quantization'] = val

    @property
    def quantization_type(self):
        return self._identity.get('quantization_type')

    @quantization_type.setter
    def quantization_type(self, val: str):
        self._identity['quantization_type'] = val

    @property
    def hexdigest(self):
        h = xxhash.xxh64()
        h.update(json.dumps(self._identity, sort_keys=True))
        return h.hexdigest()
