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
from quantization.handlers_helpers import get_all_subclasses
from quantization.new_qrec import AllFloatQRec, QRec

# pylint: disable=wildcard-import,unused-wildcard-import
from .float import *  # noqa
from .quant import *  # noqa
from .fusion_inout import *  # noqa
from .kernel_base import KernelBase

LOG = logging.getLogger("nntool." + __name__)


def get_all_backend_handlers():
    handlers = {}
    for handler in get_all_subclasses(KernelBase):
        if not handler.QREC_TYPE:
            continue
        if not handler.PARAMS_TYPE:
            continue
        for params_cls in handler.PARAMS_TYPE:
            for qrec_type_name in handler.QREC_TYPE:
                handlers.setdefault(params_cls, {})[qrec_type_name] = handler
    return handlers


HANDLERS = get_all_backend_handlers()


class KernelExecuter():
    @classmethod
    def execute(cls, params: Parameters, input_tensors: Sequence[np.ndarray],
                qrec: QRec, details: str = None) -> Sequence[np.ndarray]:
        if params.__class__ not in HANDLERS:
            raise ValueError(
                f"no handlers found for {params.__class__.__name__}")
        handlers = HANDLERS[params.__class__]
        if qrec is None:
            qrec = AllFloatQRec()
        handler = handlers.get(qrec.ktype)
        if handler is None:
            handler = handlers.get('any')
        if handler is None:
            raise ValueError(
                f"no handlers found for {params.__class__.__name__} quantization {qrec.ktype}")

        output_tensors = handler.execute(params, input_tensors,
                                         qrec, details=details,
                                         qname=qrec.ktype)

        return output_tensors
