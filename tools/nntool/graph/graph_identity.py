# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import json

import xxhash


# This class tracks any changes to the graph that render it incompatible with a value cache entry
class GraphIdentity():
    def __init__(self, filename):
        self._identity = {'filename': filename, 'fusions': []}

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
