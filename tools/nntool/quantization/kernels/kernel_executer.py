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
from typing import Sequence

import numpy as np
from graph.types import Parameters
# pylint: disable=wildcard-import,unused-wildcard-import
from quantization.float32.float32_quantization import Float32QuantizationRecord

from ..float32.kernels import *  # noqa
from ..symmetric.kernels import *  # noqa
from .fusion_inout import *  # noqa
from .kernel_base import KernelBase

LOG = logging.getLogger("nntool." + __name__)


class KernelExecuter():
    def __init__(self, quantization_name) -> None:
        self._quantization = quantization_name
        self._handlers = self.get_all_backend_handlers(quantization_name)

    @staticmethod
    def get_all_backend_handlers(quantization_name):
        handlers = {}
        for handler in KernelBase.__subclasses__():
            if not handler.QUANTIZATION:
                raise ValueError("please set quantization decorator on handler %s" %
                                 handler.__class__)
            if not handler.PARAMS_TYPE:
                raise ValueError("please set params_type decorator on handler %s" %
                                 handler.__class__)
            if quantization_name not in handler.QUANTIZATION and 'any' not in handler.QUANTIZATION:
                continue
            for params_cls in handler.PARAMS_TYPE:
                if handler.QREC_TYPE:
                    for qrec_type_cls in handler.QREC_TYPE:
                        handlers.setdefault(params_cls, {})[qrec_type_cls] = handler
                else:
                    handlers.setdefault(params_cls, {})[handler.QREC_TYPE] = handler
        return handlers

    def execute(self, params: Parameters, input_tensors: Sequence[np.ndarray],
                qrec: QuantizationRecordBase, details: str = None) -> Sequence[np.ndarray]:
        if params.__class__ not in self._handlers:
            raise ValueError("no handler found for %s quantization %s" %
                             (params.__class__.__name__, self._quantization))
        handler = self._handlers[params.__class__]
        if qrec is None and self._quantization == 'float32':
            qrec = Float32QuantizationRecord()
        if qrec.__class__ in handler:
            handler = handler[qrec.__class__]
        else:
            handler = handler[None]
        # LOG.info("Using handler %s", handler.description())
        return handler.execute(params, input_tensors,
                               qrec, details=details,
                               qname=self._quantization)
