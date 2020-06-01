# Copyright 2020 GreenWaves Technologies, SAS
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
from abc import ABC, abstractmethod, abstractclassmethod
import numpy as np
import importlib

class JsonSerializable(ABC):
    @abstractmethod
    def _encapsulate(self):
        pass

    @abstractclassmethod
    def _dencapsulate(cls, val):
        pass

    def to_dict(self):
        return {
            '__type': 'JsonSerializable',
            '__module_name': self.__module__,
            '__class_name': self.__class__.__name__,
            '__contents': self._encapsulate()
        }

    @classmethod
    def from_dict(cls: 'JsonSerializable', val):
        assert val['__type'] == 'JsonSerializable', 'no a JsonSerializable'
        json_module = importlib.import_module(val['__module_name'])
        json_class = getattr(json_module, val['__class_name'])
        return json_class._dencapsulate(val['__contents'])


class JsonSerializableStateEncoder(json.JSONEncoder):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
# pylint: disable=no-self-use, method-hidden

    def default(self, o):
        if isinstance(o, JsonSerializable):
            return o.to_dict()
        if isinstance(o, np.integer):
            return int(o)
        if isinstance(o, np.floating):
            return float(o)
        if isinstance(o, np.ndarray):
            return {
                '__type': 'numpy.ndarray',
                '__contents': o.tolist(),
                '__dtype': o.dtype.name
            }

        # Let the base class default method raise the 
        try:
            return json.JSONEncoder.default(self, o)
        except TypeError as err:
            raise err


class JsonSerializableStateDecoder(json.JSONDecoder):
    def __init__(self, *args, object_hook=None, **kwargs):
        if object_hook is None:
            super(JsonSerializableStateDecoder, self).__init__(object_hook=self.object_hook, *args, **kwargs)
        else:
            super(JsonSerializableStateDecoder, self).__init__(object_hook=object_hook, *args, **kwargs)

# pylint: disable=no-self-use, method-hidden
    def object_hook(self, obj):
        if '__type' in obj:
            if obj['__type'] == 'numpy.ndarray':
                return np.array(obj['__contents'], dtype=np.dtype(obj['__dtype']))
            if obj['__type'] == 'JsonSerializable':
                return JsonSerializable.from_dict(obj)
        return obj

