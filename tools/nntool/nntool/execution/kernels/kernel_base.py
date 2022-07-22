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

from typing import Sequence

import numpy as np
from nntool.graph.types import NNNodeBase
from nntool.quantization.new_qrec import QRec


class KernelBase():
    PARAMS_TYPE = None
    QREC_TYPE = None

    @classmethod
    def execute(cls, params: NNNodeBase, in_tensors: Sequence[np.ndarray],
                qrec: QRec, **kwargs):
        pass

    @staticmethod
    def qrec_type(*args):
        return KernelBase.property_register("QREC_TYPE", args)

    @staticmethod
    def params_type(*args):
        return KernelBase.property_register("PARAMS_TYPE", args)

    @staticmethod
    def property_register(name, value):

        def deco(cls):
            setattr(cls, name, value)
            return cls

        return deco

    @classmethod
    def description(cls):
        return f"execute {cls}: {cls.PARAMS_TYPE} {cls.QREC_TYPE}"


params_type = KernelBase.params_type
qrec_type = KernelBase.qrec_type
