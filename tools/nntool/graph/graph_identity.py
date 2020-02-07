# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import json

import xxhash


# This class tracks any changes to the graph that render it incompatible with a value cache entry
class GraphIdentity():
    def __init__(self, filename):
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
    def hexdigest(self):
        h = xxhash.xxh64()
        h.update(json.dumps(self._identity, sort_keys=True))
        return h.hexdigest()
