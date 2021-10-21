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

import importlib
import json
from abc import ABC, abstractclassmethod, abstractmethod

import numpy as np
from bfloat16 import bfloat16


class JsonSerializable(ABC):
    @abstractmethod
    def _encapsulate(self):
        raise NotImplementedError("JsonSerializable  must implement _encapsulate()")

    @abstractclassmethod
    def _dencapsulate(cls, val):
        raise NotImplementedError("JsonSerializable  must implement _dencapsulate()")

    @classmethod
    def dencapsulate(cls, val):
        return cls._dencapsulate(val)

    def to_dict(self):
        return {
            '__type': 'JsonSerializable',
            '__module_name': self.__class__.__module__,
            '__class_name': self.__class__.__name__,
            '__contents': self._encapsulate()
        }

    @classmethod
    def from_dict(cls: 'JsonSerializable', val):
        assert val['__type'] == 'JsonSerializable', 'no a JsonSerializable'
        json_module = importlib.import_module(val['__module_name'])
        json_class = getattr(json_module, val['__class_name'])
        return json_class.dencapsulate(val['__contents'])


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

    def prepare(self, obj):
        if isinstance(obj, dict):
            # if we have non string keys then encode as a list
            if any(not isinstance(k, str) for k in obj.keys()):
                return {
                    '__type': 'dict',
                    '__module_name': obj.__class__.__module__,
                    '__class_name': obj.__class__.__name__,
                    '__contents': [(self.prepare(k), self.prepare(v)) for k, v in obj.items()]
                }
            else:
                return {k: self.prepare(v) for k, v in obj.items()}
        if isinstance(obj, (list, tuple)):
            return [self.prepare(v) for v in obj]
        if isinstance(obj, (str, bool, float, int)) or obj is None:
            return obj
        if isinstance(obj, bfloat16):
            return float(obj)
        return self.default(obj)

    def iterencode(self, obj, **kwargs):
        return super(JsonSerializableStateEncoder, self).iterencode(self.prepare(obj), **kwargs)

class JsonSerializableStateDecoder(json.JSONDecoder):
    def __init__(self, *args, object_hook=None, **kwargs):
        if object_hook is None:
            super(JsonSerializableStateDecoder, self).__init__(object_hook=self.object_hook, *args, **kwargs)
        else:
            super(JsonSerializableStateDecoder, self).__init__(object_hook=object_hook, *args, **kwargs)

# pylint: disable=no-self-use, method-hidden
    def object_hook(self, obj):
        if '__type' in obj:
            if obj['__type'] == 'dict':
                json_module = importlib.import_module(obj['__module_name'])
                json_class = getattr(json_module, obj['__class_name'])
                return json_class(tuple(elem) for elem in obj['__contents'])
            if obj['__type'] == 'numpy.ndarray':
                return np.array(obj['__contents'], dtype=np.dtype(obj['__dtype']))
            if obj['__type'] == 'JsonSerializable':
                return JsonSerializable.from_dict(obj)
        return obj

