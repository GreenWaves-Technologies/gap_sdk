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
import math

import numpy as np
from bfloat16 import bfloat16
from generation.at_generators.utils import at_bits
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, NodeBindingList)
from generation.generators.globals.global_names import INFOS
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.helpers.gen_constant import gen_constant
from generation.new_generators.generator_base import GeneratorBase, paramstype
from graph.types.others import QuantizeParameters
from quantization.multiplicative.scaling_qtypes import MultMulBiasScaleQType
from quantization.qtype import QType

LOG = logging.getLogger("nntool." + __name__)

def to_unsigned(dtype):
    if dtype == np.int8:
        return np.uint8
    if dtype == np.int16:
        return np.uint16
    return dtype

@paramstype(QuantizeParameters)
class GenQuantizeParameters(GeneratorBase):
    @classmethod
    def cache_values(cls, node, qrec):
        if all(k in qrec.cache for k in ['kernel_type', 'in_at_size', 'out_at_size']):
            return True

        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]
        qrec.cache['float_conversion'] = float_conversion = in_q.is_floating or out_q.is_floating
        qrec.cache['bit_conversion'] = bit_conversion = in_q.bits != out_q.bits
        if float_conversion:
            if in_q.is_floating:
                qrec.cache['kernel_type'] = 'KOP_CONVERT_FL_FP'
            else:
                qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FL'
            qrec.cache['in_at_size'] = at_bits(in_q)
            qrec.cache['out_at_size'] = at_bits(out_q)
            return True
        else:
            qrec.cache['in_at_size'] = at_bits(in_q)
            qrec.cache['out_at_size'] = at_bits(out_q)
            qrec.cache['same_sign'] = same_sign = in_q.signed == out_q.signed
            input_precision = math.pow(2, -(in_q.bits - (1 if in_q.signed else 0)))
            if in_q.bits > out_q.bits:
                bit_diff = in_q.bits - out_q.bits
                same_scale = np.allclose(
                    (in_q.scale * np.power(2, bit_diff)).astype(np.float32), out_q.scale.astype(np.float32), atol=input_precision)
                same_zeropoint = np.all(
                    in_q.zero_point >> bit_diff == out_q.zero_point)
            elif out_q.bits > in_q.bits:
                bit_diff = out_q.bits - in_q.bits
                same_scale = np.allclose(
                    (out_q.scale * np.power(2, bit_diff)).astype(np.float32), in_q.scale.astype(np.float32), atol=input_precision)
                same_zeropoint = np.all(
                    in_q.zero_point == out_q.zero_point >> bit_diff)
            else:
                same_scale = np.allclose(out_q.scale.astype(np.float32), in_q.scale.astype(np.float32))
                same_zeropoint = np.all(in_q.zero_point == out_q.zero_point)
            qrec.cache['same_scale'] = same_scale
            qrec.cache['same_zeropoint'] = same_zeropoint
            if same_scale and same_sign and bit_conversion and same_zeropoint:
                qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP'
            elif same_scale and not bit_conversion:
                qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_ZEROPOINT'
            else:
                qrec.cache['kernel_type'] = 'KOP_CONVERT_FP_FP_SCALE'
            return True
        return False

    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        if not cls.cache_values(node, qrec):
            return False
        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]
        comment = f'in q: {in_q} out_q: {out_q}'
        if qrec.cache['kernel_type'] == 'KOP_CONVERT_FP_FP_ZEROPOINT':
            bits = 8 if in_q.dtype in [np.int8, np.uint8] else 16
            if in_q.signed:
                offset = ((int(math.pow(2, bits)) + in_q.zero_point[0] -
                          out_q.zero_point[0]) % int(math.pow(2, bits))).astype(out_q.dtype)
            else:
                offset = (int(math.pow(2, bits)) - in_q.zero_point[0] +
                          out_q.zero_point[0]).astype(out_q.dtype)
            contents = np.array(list(offset.tobytes()) + ([0] * 7), dtype=np.uint8)
        elif qrec.cache['kernel_type'] == 'KOP_CONVERT_FP_FP':
            # no infos needed
            return True
        elif qrec.cache['kernel_type'] == 'KOP_CONVERT_FP_FP_SCALE':
            scale = in_q.scale / out_q.scale
            in_abs_zp = in_q.zero_point.astype(np.int32)
            out_abs_zp = out_q.zero_point.astype(np.int32)
            if out_q.bits > in_q.bits:
                zero_adjust = (np.round(-in_abs_zp * scale) + out_abs_zp).astype(np.int32)
            else:
                zero_adjust = (-in_abs_zp + np.round(out_abs_zp * 1/scale)).astype(np.int32)

            zero_adjust = list(zero_adjust.tobytes())

            if len(scale) > 1:
                raise NotImplementedError('multiscale conversion not supported')
            scale = scale[0]
            if in_q.dtype_bits == 8 and out_q.dtype_bits == 16:
                # scale Q16 * Q8 OK
                scale_adjust = MultMulBiasScaleQType(scale=scale, dtype=np.int16, available_bits=16)
            else:
                scale_adjust = MultMulBiasScaleQType(scale=scale, dtype=np.int8, available_bits=8)
            qbias = list(scale_adjust.qbiases.tobytes())
            qbias = qbias + [0] * (2 - len(qbias))
            qnorm = list(scale_adjust.qnorms.tobytes())
            contents = np.array(zero_adjust + qbias + qnorm + [0], dtype=np.int8)
        elif qrec.cache['kernel_type'] == 'KOP_CONVERT_FL_FP':
            qbias = list((1/out_q.scale).astype(np.float32).tobytes())
            zero_adjust = list((out_q.zero_point.astype(np.int32) * out_q.scale).astype(np.float32).tobytes())
            contents = np.array(zero_adjust + qbias, dtype=np.int8)
        elif qrec.cache['kernel_type'] == 'KOP_CONVERT_FP_FL':
            qbias = list((in_q.scale).astype(np.float32).tobytes())
            zero_adjust = list((-in_q.zero_point.astype(np.int32)).astype(np.float32).tobytes())
            contents = np.array(zero_adjust + qbias, dtype=np.int8)
        else:
            raise ValueError(f"strange dtype change in {pnode.name}")
        cname, file_name = gen_constant(gen, pnode, pnode, INFOS)
        const_info = ConstantInfo(file_name, QType.Pow2(
            bits=8, q=0, signed=True), contents=contents)

        gen.globals.append(GlobalArgInfo("int8", cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info,
                                         comment=comment))

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if not cls.cache_values(node, qrec):
            return False
        gen.bindings.append(
            CommentBindingList(
                f"Node {node.name} inq {qrec.in_qs[0]} outq {qrec.out_qs[0]}")
        )
        bindings = [
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT")]
        if qrec.cache['kernel_type'] != 'KOP_CONVERT_FP_FP':
            bindings.append(GNodeArgNode(node, "infos"))
        gen.bindings.append(
            NodeBindingList(cname, *bindings))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        if not cls.cache_values(node, qrec):
            return False
        del in_eparams, out_eparams
        gen.kernels.append(
            ConversionKernel(
                node.name, cname, qrec.cache['in_at_size'], qrec.cache['out_at_size'],
                node.in_dims[0].size(), qrec.cache['kernel_type'])
        )
        return True


class ConversionKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = """
// generator for {node_name}
CNN_Convert("{cname}", {in_type}, {out_type}, {size}, {kop});
"""

    def __init__(self, node_name, cname, in_type, out_type, size, kop):
        attrs = {
            'in_type': in_type,
            'out_type': out_type,
            'size': size,
            'kop': kop,
        }

        extra_attrs = {
            'cname': cname,
            'node_name': node_name
        }
        super().__init__(attrs, extra_attrs)
