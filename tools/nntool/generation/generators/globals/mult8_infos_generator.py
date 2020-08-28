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

import os

import numpy as np

from generation.at_types.constant_info import ConstantInfo
from generation.at_types.tc_arg_info import GlobalArgInfo
from generation.generators.generator_decorators import (QREC_MULT8,
                                                        generation_function)
from graph.types import (ConvFusionParameters, FilterParameters,
                         GlobalPoolParameters, HSigmoidActivationParameters,
                         HSwishActivationParameters, PoolingParameters,
                         SoftMaxParameters, ActivationFusion, MatrixMulParameters,
                         ReluActivationParameters, MatrixAddParameters, ActivationParameters)
from quantization.qtype import QType
from quantization.symmetric.kernels.activations import (
    hsigmoid_mult_gen_factors, hswish_mult_gen_factors)
from utils.node_id import NodeId
from .global_names import *


@generation_function("globals",
                     (FilterParameters, ConvFusionParameters, ActivationParameters,
                      GlobalPoolParameters, MatrixAddParameters, MatrixMulParameters,
                      ActivationFusion, PoolingParameters, SoftMaxParameters),
                     qrec_types=(QREC_MULT8,))
def mult8_infos_generator(gen, node, qrec, pnode, fnode) -> bool:
    if fnode is not None:
        return False
    if isinstance(pnode, FilterParameters):
        if pnode.has_bias:
            bias_q = qrec.biases_q.q
        else:
            bias_q = qrec.biases_q.q
        act_infos(gen, pnode, pnode, None, None, extra1=bias_q)
    elif isinstance(pnode, (GlobalPoolParameters, PoolingParameters)):
        qrec.set_scale()
        act_infos(gen, pnode, pnode, None, qrec)
    elif isinstance(pnode, ActivationParameters):
        act_infos(gen, pnode, pnode, pnode, gen.G.quantization[NodeId(pnode)])
    elif isinstance(pnode, ConvFusionParameters):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if node.fusion_type.startswith('linear') or node.fusion_type.startswith('conv'):
            if cnodes[0].has_bias:
                bias_q = quants[0].biases_q.q
            else:
                bias_q = quants[0].out_qs[0].q
            if node.fusion_type in ("conv_active_pool", "conv_active", "linear_active"):
                act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1], extra1=bias_q)
            elif node.fusion_type == "conv_pool_active":
                act_infos(gen, pnode, cnodes[0], cnodes[2], quants[2], extra1=bias_q)
            elif node.fusion_type == "conv_pool":
                act_infos(gen, pnode, cnodes[0], None, None, extra1=bias_q)
    elif isinstance(pnode, MatrixAddParameters):
        qrec.set_add_scale()
        act_infos(gen, pnode, pnode, None, None,
                  extra1=qrec.scale_in_mul_biases_q.qbiases[0],
                  extra2=qrec.scale_in_mul_biases_q.qnorms[0],
                  extra3=qrec.scale_mul_biases_q.qbiases[0],
                  extra4=qrec.scale_mul_biases_q.qnorms[0])
    elif isinstance(pnode, MatrixMulParameters):
        qrec.set_scale(in_idx=(0, 1), out_idx=0)
        act_infos(gen, pnode, pnode, None, None,
                  extra1=qrec.scale_mul_biases_q.qbiases[0],
                  extra2=qrec.scale_mul_biases_q.qnorms[0])
    elif isinstance(pnode, SoftMaxParameters):
        act_infos(gen, pnode, pnode, pnode, qrec)
    elif isinstance(pnode, ActivationFusion):
        cnodes = node.contained_nodes()
        quants = [gen.G.quantization[NodeId(node, fnode)] for fnode in cnodes]
        if isinstance(cnodes[0], (GlobalPoolParameters, PoolingParameters)):
            act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1])
        elif isinstance(cnodes[0], MatrixAddParameters):
            quants[0].set_add_scale()
            act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1],
                      extra1=quants[0].scale_in_mul_biases_q.qbiases[0],
                      extra2=quants[0].scale_in_mul_biases_q.qnorms[0],
                      extra3=quants[0].scale_mul_biases_q.qbiases[0],
                      extra4=quants[0].scale_mul_biases_q.qnorms[0])
        elif isinstance(cnodes[0], MatrixMulParameters):
            qrec.set_scale(in_idx=(0, 1), out_idx=0)
            act_infos(gen, pnode, cnodes[0], cnodes[1], quants[1],
                      extra1=qrec.scale_mul_biases_q.qbiases[0],
                      extra2=qrec.scale_mul_biases_q.qnorms[0])
        else:
            return False
        return True
    else:
        return False
    return True


def gen_constant(gen, pnode, cache_node, const_type):
    cname = gen.naming_convension.get_global_name(pnode.name, pnode.step_idx,
                                                  pnode, const_type)
    gen.name_cache.set(cache_node, const_type, cname)
    file_name = os.path.join(gen.opts['tensor_directory'],
                             cname+".tensor")
    return cname, file_name


def act_infos(gen, pnode, fnode, act_params, act_q, extra1=0, extra2=0, extra3=0, extra4=0):
    if isinstance(pnode, FilterParameters):
        comment = str.format("BiasQ: {}", extra1)
    elif isinstance(pnode, MatrixAddParameters):
        comment = str.format("In1Scale: {} In1ScaleN: {} OutScale: {} OutScaleN: {}",
                             extra1, extra2, extra3, extra4)
    else:
        comment = ""

    if act_params is None:
        contents = np.array([0, 0, 0, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
    elif isinstance(act_params, ReluActivationParameters):
        if act_params.upper_bound is None: # or fnode is not None:
            contents = np.array([0, 0, 0, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
            if len(comment) == 0:
                comment = "all 0"
        else:
            fac_1 = act_q.in_qs[0].quantize(act_params.upper_bound)
            contents = np.array([0, 0, fac_1, 0, 0, extra1, extra2, extra3, extra4],
                                dtype=np.int8)
            comment += str.format("in: {:05f} out: {:05f} A0: {} B0: 0 C0: 0",
                                  act_q.in_qs[0].scale[0],
                                  act_q.out_qs[0].scale[0],
                                  fac_1[0])
    elif isinstance(act_params, HSigmoidActivationParameters):
        # currently combines all scaling factors into one scale and shift
        fac_1, upper_bound, _ = hsigmoid_mult_gen_factors(act_params, act_q)
        contents = np.array([act_q.scale_mul_biases_q.qbiases[0],
                             act_q.scale_mul_biases_q.qnorms[0],
                             upper_bound, fac_1, 1, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.scale_mul_biases_q.qbiases[0],
                              act_q.scale_mul_biases_q.qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, HSwishActivationParameters):
        # currently combines all scaling factors into one scale and shift
        fac_1, upper_bound, _ = hswish_mult_gen_factors(act_q)
        contents = np.array([act_q.scale_mul_biases_q.qbiases[0],
                             act_q.scale_mul_biases_q.qnorms[0],
                             upper_bound, fac_1, 1, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} qbias: {} qnorm: {} A0: {} B0: {} C0: 1",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              act_q.scale_mul_biases_q.qbiases[0],
                              act_q.scale_mul_biases_q.qnorms[0],
                              upper_bound[0], fac_1[0])
    elif isinstance(act_params, SoftMaxParameters):
        norm = 15 + np.ceil(np.log2(act_q.in_qs[0].scale))
        contents = np.array([norm, 0, 0, 0, 0, extra1, extra2, extra3, extra4], dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f} NORM: {}",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0],
                              int(norm[0]))
    else:
        raise NotImplementedError("activation tye not implemented")

    if isinstance(pnode, (GlobalPoolParameters, PoolingParameters)):
        contents = np.array([act_q.scale_mul_biases_q.qbiases[0],
                             act_q.scale_mul_biases_q.qnorms[0],
                             0, 0, 0, extra1, extra2, extra3, extra4],
                            dtype=np.int8)
        comment += str.format("in: {:05f} out: {:05f}",
                              act_q.in_qs[0].scale[0],
                              act_q.out_qs[0].scale[0])

    cname, file_name = gen_constant(gen, pnode, fnode, INFOS)
    const_info = ConstantInfo(file_name, QType(bits=8, q=0, signed=True), contents=contents)

    gen.globals.append(GlobalArgInfo("int8", cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info,
                                     comment=comment))
