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

# TODO - This is not mappable onto our current kernels. To add if needed by a customer

# import numpy as np
# from graph.types.activations import ReluActivationParameters
# from importer.onnx.handlers.backend.math_mixin import BasicMathMixin

# from ..backend_handler import BackendHandler
# from ..handler import onnx_op


# @onnx_op("ThresholdedRelu")
# class ThresholdedRelu(BasicMathMixin, BackendHandler):

#     @classmethod
#     def _common(cls, node, **kwargs):
#         alpha = node.attrs.get('alpha', 1.0)
#         return super(ThresholdedRelu, cls)._common(node,
#                                         params_class=ReluActivationParameters,
#                                         constant_operation=lambda x: np.clip(x, alpha, np.inf),
#                                         params_args={'lower_limit': alpha},
#                                         **kwargs)

#     @classmethod
#     def version_10(cls, node, **kwargs):
#         return cls._common(node, **kwargs)
