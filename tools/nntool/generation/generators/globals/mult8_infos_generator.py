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
from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generator_decorators import QREC_MULT8, generation_function
from generation.helpers.gen_constant import gen_constant
from generation.helpers.gen_scales import gen_scales
from graph.types import (ActivationFusion, ActivationParameters,
                         ConvFusionParameters, FilterParameters,
                         GlobalPoolParameters, HSigmoidActivationParameters,
                         HSwishActivationParameters, LeakyActivationParameters,
                         MatMulOpFusionParameters, MatMulOpParameters,
                         MatrixAddParameters, MatrixMulParameters,
                         PaddedAddFusionParameters, PoolingParameters,
                         QuantizeParameters, ReluActivationParameters,
                         SigmoidActivationParameters, SoftMaxParameters)
from quantization.multiplicative.mulbias import (compute_in_out_scale,
                                                 set_add_in_scale)
from quantization.qtype import QType
from quantization.symmetric.kernels.activations import (
    hsigmoid_mult_gen_factors, hswish_mult_gen_factors,
    leak_mult_gen_factor_q7)
from utils.node_id import NodeId

# pylint: disable=wildcard-import,unused-wildcard-import
from .global_names import *


@generation_function("globals",
                     (FilterParameters, ConvFusionParameters, ActivationParameters,
                      GlobalPoolParameters, MatrixAddParameters, MatrixMulParameters,
                      ActivationFusion, PoolingParameters, SoftMaxParameters, PaddedAddFusionParameters,
                      QuantizeParameters, MatMulOpFusionParameters, MatMulOpParameters),
                     qrec_types=(QREC_MULT8,))
def mult8_infos_generator(gen, node, qrec, pnode, fnode) -> bool:
    if fnode is not None:
        return False
    if isinstance(pnode, FilterParameters):
        for_ne16 = qrec.cache.get('ne16')
        in_zero_point = qrec.in_qs[0].zero_point
        act_infos(gen, pnode, pnode, None, None, extra1=0,
                  for_ne16=for_ne16, in_zero_point=in_zero_point)
    elif isinstance(pnode, (GlobalPoolParameters, PoolingParameters)):
        compute_in_out_scale(qrec)
        act_infos(gen, pnode, pnode, None, qrec)
    elif isinstance(pnode, ActivationParameters):
        act_infos(gen, pnode, pnode, pnode, gen.G.quantization[NodeId(pnode)])
    elif isinstance(pnode, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        for_ne16 = any([qrec.cache.get('ne16') for qrec in quants])
        in_zero_point = quants[0].in_qs[0].zero_point
        for qrec in quants:
            compute_in_out_scale(qrec)
        if node.fusion_type.startswith('linear') or node.fusion_type.startswith('conv') or node.fusion_type.startswith('pool'):
            if node.fusion_type in ("pool_active"):
                act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1],
                          extra1=0, for_ne16=for_ne16, in_zero_point=in_zero_point)
            elif node.fusion_type in ("conv_active_pool", "conv_active", "linear_active"):
                act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1],
                          extra1=0, for_ne16=for_ne16, in_zero_point=in_zero_point)
            elif node.fusion_type == "conv_pool_active":
                act_infos(gen, pnode, cnodes[0], cnodes[2], quants[2],
                          extra1=0, for_ne16=for_ne16, in_zero_point=in_zero_point)
            elif node.fusion_type == "conv_pool":
                act_infos(gen, pnode, cnodes[0], None,
                          None, extra1=0, for_ne16=for_ne16)
    elif isinstance(pnode, MatrixAddParameters):
        set_add_in_scale(qrec)
        act_infos(gen, pnode, pnode, None, None,
                  extra1=qrec.cache['scale_in_mul_biases_q'].qbiases[0],
                  extra2=qrec.cache['scale_in_mul_biases_q'].qnorms[0],
                  extra3=qrec.cache['scale_mul_biases_q'].qbiases[0],
                  extra4=qrec.cache['scale_mul_biases_q'].qnorms[0])
    elif isinstance(pnode, PaddedAddFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        for qrec in quants:
            compute_in_out_scale(qrec)
        act_node = [cnode for cnode in cnodes if isinstance(
            cnode, ActivationParameters)]
        act_node = act_node[0] if act_node else None
        act_qrec = quants[-1] if act_node else None
        set_add_in_scale(quants[1])
        act_infos(gen, pnode, pnode, act_node, act_qrec,
                  extra1=quants[1].cache['scale_in_mul_biases_q'].qbiases[0],
                  extra2=quants[1].cache['scale_in_mul_biases_q'].qnorms[0],
                  extra3=quants[1].cache['scale_mul_biases_q'].qbiases[0],
                  extra4=quants[1].cache['scale_mul_biases_q'].qnorms[0])
        act_infos(gen, pnode, cnodes[0], act_node, act_qrec, extra_name="Pad",
                  extra1=quants[1].cache['scale_mul_biases_q'].qbiases[0],
                  extra2=quants[1].cache['scale_mul_biases_q'].qnorms[0])
    elif isinstance(pnode, MatrixMulParameters):
        compute_in_out_scale(qrec, in_idx=(0, 1), out_idx=0)
        act_infos(gen, pnode, pnode, None, None,
                  extra1=qrec.cache['scale_mul_biases_q'].qbiases[0],
                  extra2=qrec.cache['scale_mul_biases_q'].qnorms[0])
    elif isinstance(pnode, SoftMaxParameters):
        act_infos(gen, pnode, pnode, pnode, qrec)
    elif isinstance(pnode, ActivationFusion):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        for qrec in quants:
            compute_in_out_scale(qrec)
        if isinstance(cnodes[0], (GlobalPoolParameters, PoolingParameters)):
            act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1])
        elif isinstance(cnodes[0], MatrixAddParameters):
            set_add_in_scale(quants[0])
            act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1],
                      extra1=quants[0].cache['scale_in_mul_biases_q'].qbiases[0],
                      extra2=quants[0].cache['scale_in_mul_biases_q'].qnorms[0],
                      extra3=quants[0].cache['scale_mul_biases_q'].qbiases[0],
                      extra4=quants[0].cache['scale_mul_biases_q'].qnorms[0])
        else:
            return False
        return True
    elif isinstance(pnode, (MatMulOpParameters, MatMulOpFusionParameters)):
        if isinstance(pnode, MatMulOpFusionParameters):
            cnodes = node.contained_nodes()
            quants = [gen.G.quantization[NodeId(
                node, fnode)] for fnode in cnodes]
            mul_node = cnodes[0]
            mul_qrec = quants[0]
            act_node = cnodes[1]
            act_qrec = quants[1]
        else:
            mul_node = pnode
            mul_qrec = qrec
            act_node = None
            act_qrec = None

        if len(pnode.in_dims) == 3 and len(mul_qrec.in_qs[0].scale) > 1:
            gen_scales(gen, pnode, mul_node, mul_qrec)
            extra3 = 0
            extra4 = 0
        else:
            extra3 = mul_qrec.cache['mul_biases_q'].qbiases[0]
            extra4 = mul_qrec.cache['mul_biases_q'].qnorms[0]

        act_infos(gen, pnode, mul_node, act_node, act_qrec,
                  extra3=extra3,
                  extra4=extra4)
    elif isinstance(pnode, QuantizeParameters):
        in_q = qrec.in_qs[0]
        out_q = qrec.out_qs[0]
        if not np.allclose(in_q.scale, out_q.scale):
            raise ValueError(f"don't know how to change scale in {pnode.name}")
        comment = f'in zp: {in_q.zero_point} out_zp: {out_q.zero_point}'
        if in_q.dtype == np.int8 and out_q.dtype == np.uint8:
            contents = ((256 + in_q.zero_point[0] - out_q.zero_point[0]) % 256).astype(np.uint8)
        elif in_q.dtype == np.uint8 and out_q.dtype == np.int8:
            contents = (256 - in_q.zero_point[0] + out_q.zero_point[0]).astype(np.uint8)
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
    else:
        return False
    return True


def act_infos(gen, pnode, fnode, act_params, act_q, extra1=0, extra2=0, extra3=0, extra4=0, extra_name='', for_ne16=False, in_zero_point=0):
    if isinstance(pnode, FilterParameters):
        comment = str.format("BiasQ: {}", extra1)
    elif isinstance(pnode, MatrixAddParameters):
        comment = str.format("In1Scale: {} In1ScaleN: {} OutScale: {} OutScaleN: {}",
                             extra1, extra2, extra3, extra4)
    else:
        comment = ""

    if act_params is None:
        contents = np.array([0, 0, 0, 0, 0, extra1, extra2,
                             extra3, extra4], dtype=np.int8)
    elif isinstance(act_params, ReluActivationParameters):
        actscale = act_q.cache['scale_mul_biases_q'].qbiases[0]
        actscalen = act_q.cache['scale_mul_biases_q'].qnorms[0]
        if act_params.upper_bound is None:  # or fnode is not None:
            if act_q.in_qs[0].zero_point == 0:
                contents = np.array(
                    [actscale, actscalen, 0, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
                if len(comment) == 0:
                    comment = "all 0"
            else:
                fac_1 = act_q.in_qs[0].zero_point
                contents = np.array(
                    [actscale, actscalen, fac_1, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: 0 C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0])
        else:
            if act_q.in_qs[0].zero_point == 0:
                fac_1 = act_q.in_qs[0].quantize(act_params.upper_bound)
                contents = np.array([actscale, actscalen, fac_1, 0, 0, extra1, extra2, extra3, extra4],
                                    dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: 0 C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0])
            else:
                fac_1 = act_q.in_qs[0].zero_point
                fac_2 = act_q.in_qs[0].quantize(act_params.upper_bound)
                contents = np.array([actscale, actscalen, fac_1, fac_2, 0, extra1, extra2, extra3, extra4],
                                    dtype=np.int8)
                comment += str.format("in: {:05f} out: {:05f} A0: {} B0: {} C0: 0",
                                      act_q.in_qs[0].scale[0],
                                      act_q.out_qs[0].scale[0],
                                      fac_1[0],
                                      fac_2[0])
    elif isinstance(act_params, HSigmoidActivationParameters):
        # currently combines all scaling factors into one scale and shift
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        fac_1, upper_bound, _ = hsigmoid_mult_gen_factors(act_params, act_q)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             upper_bound, fac_1, 1, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, HSwishActivationParameters):
        # currently combines all scaling factors into one scale and shift
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        fac_1, upper_bound, _ = hswish_mult_gen_factors(act_q)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             upper_bound, fac_1, 1, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, SoftMaxParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        norm = 15 + np.ceil(np.log2(act_q.in_qs[0].scale))
        contents = np.array(
            [norm, 0, 0, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} NORM: {}",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              int(norm[0]))
    elif isinstance(act_params, LeakyActivationParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        compute_in_out_scale(act_q)
        leak_factor_quant = leak_mult_gen_factor_q7(act_params)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             leak_factor_quant, 0, 0, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: x C0: x",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0],
                              leak_factor_quant)
    elif isinstance(act_params, SigmoidActivationParameters):
        assert act_q.in_qs[0].zero_point == 0 and act_q.out_qs[0].zero_point == 0, "asymmetric not supported"
        compute_in_out_scale(act_q, extra_scale=QType.Pow2(
            bits=32, q=7, signed=True).scale/act_q.in_qs[0].scale)
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             0, 0, 0, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: x B0: x C0: x",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.cache['scale_mul_biases_q'].qbiases[0],
                              act_q.cache['scale_mul_biases_q'].qnorms[0])
    else:
        raise NotImplementedError("activation tye not implemented")

    if isinstance(pnode, (GlobalPoolParameters, PoolingParameters)):
        contents = np.array([act_q.cache['scale_mul_biases_q'].qbiases[0],
                             act_q.cache['scale_mul_biases_q'].qnorms[0],
                             0, 0, 0, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f}",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0])

    if for_ne16:
        # append weights_offset and pad_val for ne16
        # TODO - default config maybe in future
        pad_value = np.array(in_zero_point).astype(np.int16)
        pad_value1 = np.bitwise_and(pad_value, 0xFF)
        pad_value2 = np.bitwise_and(pad_value, 0xFF00) >> 8
        w_offset = np.array(-128).astype(np.int32)
        w_offset1 = np.bitwise_and(w_offset, 0xFF)
        w_offset2 = np.bitwise_and(w_offset, 0xFF00) >> 8
        w_offset3 = np.bitwise_and(w_offset, 0xFF0000) >> 16
        w_offset4 = np.bitwise_and(w_offset, 0xFF000000) >> 24

        contents = np.append(
            contents, [0, pad_value1, pad_value2, w_offset1, w_offset2, w_offset3, w_offset4])

    cname, file_name = gen_constant(gen, pnode, fnode, INFOS, extra_name)
    const_info = ConstantInfo(file_name, QType.Pow2(
        bits=8, q=0, signed=True), contents=contents)

    gen.globals.append(GlobalArgInfo("int8", cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info,
                                     comment=comment))
