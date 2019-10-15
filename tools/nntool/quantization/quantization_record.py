# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

from typing import Sequence

from utils.stats_funcs import closest_greater

from .qtype import QType

class QuantizationRecord():
    def __init__(self, in_qs: QType = None, out_qs: QType = None):
        self._info = {}
        self.set_qs(in_qs=in_qs, out_qs=out_qs)

# pylint: disable=unused-argument
    def set_qs(self, **kwargs):
        self.in_qs = kwargs.get('in_qs') or []
        self.out_qs = kwargs.get('out_qs') or []

    @property
    def out_qs(self) -> Sequence[QType]:
        if 'out_qs' in self._info:
            return self._info['out_qs']
        return None

    @out_qs.setter
    def out_qs(self, value: Sequence[QType]):
        self._info['out_qs'] = value

    @property
    def in_qs(self) -> Sequence[QType]:
        if 'in_qs' in self._info:
            return self._info['in_qs']
        return None

    @in_qs.setter
    def in_qs(self, value: Sequence[QType]):
        self._info['in_qs'] = value

    @property
    def has_quantization(self) -> bool:
        return bool(self._info)

    def to_dict(self):
        info = self._info.copy()
        info['__type'] = "QuantizationRecord.{}".format(self.__class__.__name__)
        return info

    @staticmethod
    def from_dict(json_dict):
        class_const = globals()[json_dict['__type'].split('.')[1]]
        del json_dict['__type']
        return class_const(**json_dict)

    def __eq__(self, value):
        # pylint: disable=protected-access
        return value._info == self._info

class FilterQuantizationRecord(QuantizationRecord):
    def __init__(self, in_qs: QType = None, out_qs: QType = None,
                 calc_q: QType = None, acc_q: QType = None,
                 biases_q: QType = None, weights_q: QType = None):
        super().__init__()
        self.set_qs(in_qs=in_qs, out_qs=out_qs, calc_q=calc_q,
                    acc_q=acc_q, biases_q=biases_q, weights_q=weights_q)

    def set_qs(self, **kwargs):
        super().set_qs(**kwargs)
        self._info['calc_q'] = kwargs.get('calc_q')
        self._info['acc_q'] = kwargs.get('acc_q')
        self._info['biases_q'] = kwargs.get('biases_q')
        self._info['weights_q'] = kwargs.get('weights_q')

    @property
    def calc_q(self) -> QType:
        calc_q = self._info.get('calc_q')
        if calc_q:
            return calc_q
        return QType(bits=closest_greater(self.in_qs[0].bits + self.weights_q.bits),
                     q=self.in_qs[0].q + self.weights_q.q,
                     signed=True)
    @property
    def acc_q(self) -> QType:
        return self._info.get('acc_q')

    @property
    def biases_q(self) -> QType:
        return self._info.get('biases_q')

    @property
    def weights_q(self) -> QType:
        return self._info.get('weights_q')

    @calc_q.setter
    def calc_q(self, val: QType):
        self._info['calc_q'] = val

    @acc_q.setter
    def acc_q(self, val: QType):
        self._info['acc_q'] = val

    @biases_q.setter
    def biases_q(self, val: QType):
        self._info['biases_q'] = val

    @weights_q.setter
    def weights_q(self, val: QType):
        self._info['weights_q'] = val
