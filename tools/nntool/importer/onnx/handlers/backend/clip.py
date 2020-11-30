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

import numpy as np
from graph.types.activations import ReluActivationParameters
from ..backend_handler import BackendHandler
from ..handler import onnx_op
from .math_mixin import BasicMathMixin


@onnx_op("Clip")
class Clip(BasicMathMixin, BackendHandler):

    @classmethod
    def args_version(cls, node, **kwargs):
        inputs = [kwargs['all_nodes'][inp] for inp in node.input]
        min_val = cls.get_constant(inputs[1]) if len(inputs) > 1 and inputs[1] else -3.402823e+38
        max_val = cls.get_constant(inputs[2]) if len(inputs) > 2 and inputs[2] else 3.402823e+38
        return super(Clip, cls)._common(node,
                                        params_class=ReluActivationParameters,
                                        params_args={
                                            'lower_bound': min_val,
                                            'upper_bound': max_val
                                        },
                                        constant_operation=lambda x: np.minimum(np.maximum(x, min_val), max_val),
                                        **kwargs)

    @classmethod
    def attr_version(cls, node, **kwargs):
        min_val = node.attrs.get('min', -3.402823e+38)
        max_val = node.attrs.get('max', 3.402823e+38)
        return super(Clip, cls)._common(node,
                                        params_class=ReluActivationParameters,
                                        params_args={
                                            'lower_bound': min_val,
                                            'upper_bound': max_val
                                        },
                                        constant_operation=lambda x: np.minimum(np.maximum(x, min_val), max_val),
                                        **kwargs)

    @classmethod
    def version_1(cls, node, **kwargs):
        return cls.attr_version(node, **kwargs)

    @classmethod
    def version_6(cls, node, **kwargs):
        return cls.attr_version(node, **kwargs)

    @classmethod
    def version_11(cls, node, **kwargs):
        return cls.args_version(node, **kwargs)

    @classmethod
    def version_12(cls, node, **kwargs):
        return cls.args_version(node, **kwargs)
