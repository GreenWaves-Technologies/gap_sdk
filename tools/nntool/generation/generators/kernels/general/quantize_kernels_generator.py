# # Copyright (C) 2020  GreenWaves Technologies, SAS

# # This program is free software: you can redistribute it and/or modify
# # it under the terms of the GNU Affero General Public License as
# # published by the Free Software Foundation, either version 3 of the
# # License, or (at your option) any later version.

# # This program is distributed in the hope that it will be useful,
# # but WITHOUT ANY WARRANTY; without even the implied warranty of
# # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# # GNU Affero General Public License for more details.
# # You should have received a copy of the GNU Affero General Public License
# # along with this program.  If not, see <https://www.gnu.org/licenses/>.

# import logging
# import math
# import numpy as np
# from generation.at_types.gen_ctrl import GenCtrl
# from generation.code_block import CodeBlock
# from generation.generator_decorators import generation_function
# from graph.types.others import QuantizeParameters

# from ..autotiler_kernel import AutotilerKernel

# LOG = logging.getLogger("nntool." + __name__)

# @generation_function("kernels", (QuantizeParameters,))
# def quantize_kernel_generator(gen, node, qrec, in_eparams, out_eparams, cname):
#     del in_eparams, out_eparams
#     in_q = qrec.in_qs[0]
#     out_q = qrec.out_qs[0]
#     if in_q.dtype == np.int8 and out_q.dtype == np.uint8:
#         if not np.allclose(in_q.scale, out_q.scale):
#             LOG.warning('quantize kernel generator only supports type changes not scale')
#             return False
#         if not np.all(in_q.zero_point == (out_q.zero_point - 128)):
#             LOG.warning('quantize kernel generator cannot move zero point')
#             return False
#         qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_ZEROPOINT'
#         gen.kernels.append(
#             ConversionKernel(node.name, cname, 1, -1, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP_ZEROPOINT')
#         )
#         return True
#     if in_q.dtype == np.uint8 and out_q.dtype == np.int8:
#         if not np.all((in_q.zero_point - 128) == out_q.zero_point):
#             LOG.warning('quantize kernel generator cannot move zero point')
#             return False
#         qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_ZEROPOINT'
#         gen.kernels.append(
#             ConversionKernel(node.name, cname, -1, 1, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP_ZEROPOINT')
#         )
#         return True
#     if in_q.dtype == np.int8 and out_q.dtype == np.int16:
#         if np.allclose(in_q.scale, (out_q.scale * np.power(2, 8)), atol=0.0001):
#             qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP'
#             gen.kernels.append(
#                 ConversionKernel(node.name, cname, 1, 2, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP')
#             )
#         else:
#             qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_SCALE'
#             gen.kernels.append(
#                 ConversionKernel(node.name, cname, 1, 2, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP_SCALE')
#             )
#         return True
#     if in_q.dtype == np.int16 and out_q.dtype == np.int8:
#         if np.allclose(out_q.scale, (in_q.scale * np.power(2, 8)), atol=0.0001):
#             qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP'
#             gen.kernels.append(
#                 ConversionKernel(node.name, cname, 2, 1, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP')
#             )
#         else:
#             qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_SCALE'
#             gen.kernels.append(
#                 ConversionKernel(node.name, cname, 2, 1, node.in_dims[0].size(), 'KOP_CONVERT_FP_FP_SCALE')
#             )
#         return True


        

#     return True

# class ConversionKernel(AutotilerKernel):
#     def __init__(self, node_name, cname, in_type, out_type, size, kop, at_ver=3, gen_ctrl=None):
#         if gen_ctrl is None:
#             self.gen_ctrl = gen_ctrl = GenCtrl(None, cname=cname)
#         else:
#             gen_ctrl.cname = cname
#             self.gen_ctrl = gen_ctrl

#         self.cname = cname
#         self.node_name = node_name
#         self.at_ver = at_ver
#         self.in_type = in_type
#         self.out_type = out_type
#         self.size = size
#         self.kop = kop

#     def code(self, code_block=None):
#         if code_block is None:
#             code_block = CodeBlock()

#         code_block.comment("generator for {}", self.node_name)
#         code_block.write(f'CNN_Convert("{self.cname}", {self.in_type}, {self.out_type}, {self.size}, {self.kop});')

#         return code_block
