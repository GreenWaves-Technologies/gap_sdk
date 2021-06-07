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
from generation.at_types.tc_arg_info import (GlobalArgInfo, GlobalResetArgInfo,
                                             InputArgInfo)
from generation.generator_decorators import (QREC_FLOAT, QREC_MULT8, QREC_POW2,
                                             generation_function)
from graph.types import ConstantInputParameters
from graph.types.fusions import ConvFusionParameters
from graph.types.linear import FcParameters
from utils.node_id import NodeId


@generation_function("globals", (ConstantInputParameters,), qrec_types=(QREC_POW2, QREC_MULT8, QREC_FLOAT))
def constant_input_globals_generator(gen, node, qrec, pnode, fnode) -> bool:
    del node, fnode
    if not pnode.generate_value:
        return True
    # the name cache will be updated when all the edges are analysed by local_generator
    # the name of the constant is attached to the output edge so find it
    out_edge = gen.G.out_edges(pnode.name)[0]
    eparams = out_edge.params
    cname = gen.naming_convension.get_edge_name(eparams.creating_node,
                                                eparams.creating_step,
                                                eparams.edge_type,
                                                eparams.edge_order)
    if not pnode.is_constant:
        # This is an initializer which may have a reset
        if pnode.reset_name and not next((tc for tc in gen.globals
                                          if tc.arg_name == pnode.reset_name), None):
            gen.globals.append(GlobalResetArgInfo(
                pnode.reset_name, 'AT_MEM_L2', 'AT_MEM_UNDEF'))
        if pnode.is_global:
            home_location = gen.opts['default_input_home_location']
            exec_location = gen.opts['default_input_exec_location']
            gen.globals.append(InputArgInfo(qrec.out_qs[0].ctype, cname,
                                            home_location=home_location,
                                            exec_location=exec_location,
                                            allocate=pnode.at_options.allocate,
                                            is_inout=pnode.is_mutated))
    elif pnode.is_global:
        file_name = os.path.join(gen.opts['tensor_directory'],
                                 cname+".tensor")
        value = pnode.value_as(qrec.out_qs[0], generation=True)
        if pnode.concated_nodes:
            values = [value]
            concated_qrecs = [gen.G.quantization.get(NodeId(pn, None))
                              for pn in pnode.concated_nodes]
            for other_node, concated_qrec in zip(pnode.concated_nodes, concated_qrecs):
                values += [other_node.value_as(
                    concated_qrec.out_qs[0], generation=True)]
            value = np.hstack(tuple(values))

        if qrec.out_qs[0].attr.ne16_order:
            to_node = gen.G.out_edges(pnode.name)[0].to_node
            if isinstance(to_node, FcParameters) or (isinstance(to_node, ConvFusionParameters) and to_node.fusion_type == "linear_active"):
                value = value
                # value = ne16_linear_weight_layout(
                #     value, w_bits=qrec.out_qs[0].bits)
            else:
                value = value.transpose((0, 3, 1, 2))
                #if isinstance(to_node, ConvFusionParameters):
                #    is_dw = to_node.contained_nodes()[0].is_depthwise_conv()
                #else:
                #    is_dw = to_node.is_depthwise_conv()
                #value = ne16_conv_weight_layout(
                #    value, w_bits=qrec.out_qs[0].bits)
            numeric_format = "fixed"
        else:
            if qrec.out_qs[0].attr.ne16_biases:
                to_node = gen.G.out_edges(pnode.name)[0].to_node
                if isinstance(to_node, ConvFusionParameters):
                    cnodes = to_node.contained_nodes()
                    quants = [gen.G.quantization[NodeId(
                        to_node, fnode)] for fnode in cnodes]
                    filter_qrec = quants[0]
                else:
                    filter_qrec = gen.G.quantization[NodeId(to_node)]
                mul_qbiases = filter_qrec.cache['mul_biases_q'].qbiases
                mul_qnorms = filter_qrec.cache['mul_biases_q'].qnorms
                value = value * mul_qbiases + \
                    (1 << (mul_qnorms-1).astype(np.int32))

            numeric_format = "fixed"

        const_info = ConstantInfo(file_name, qrec.out_qs[0], contents=value,
                                  numeric_format=numeric_format)
        gen.globals.append(GlobalArgInfo(qrec.out_qs[0].ctype, cname,
                                         gen.opts['default_global_home_location'],
                                         gen.opts['default_global_exec_location'],
                                         const_info=const_info))
    return True

def ne16_conv_weight_layout(W, w_bits, is_depthwise=False):
    if W.shape[1:3] == (3, 3):
        if is_depthwise:
            return ne16_conv_dw_3x3_weight_layout(W, w_bits)
        return ne16_conv_3x3_weight_layout(W, w_bits)
    elif W.shape[1:3] == (1, 1):
        return ne16_conv_1x1_weight_layout(W, w_bits)
    raise NotImplementedError(f"NE16 weights generation for {W.shape} weight")

def ne16_conv_3x3_weight_layout(W, w_bits=8, is_depthwise=False):
    tp_in = 16
    # tp_out = 32
    chan_out = W.shape[0]
    chan_in = W.shape[3]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    # w_binary_1 = np.zeros((chan_out, subtile_nb_ki, w_bits, 3 * 3, tp_in//8), dtype=np.uint8)
    # for k_out in range(chan_out):
    #     for k_in_major in range(subtile_nb_ki):
    #         for q in range(w_bits):
    #             for fy in range(3):
    #                 for fx in range(3):
    #                     for k_in_minor in range(tp_in):
    #                         if k_in_major*tp_in+k_in_minor >= chan_in:
    #                             w_val = 0
    #                         else:
    #                             w_val = W[k_out, fy, fx, k_in_major*tp_in+k_in_minor]
    #                         w_binary_1[k_out, k_in_major, q, fy*3+fx, k_in_minor//8] |= (0x1 & (w_val >> q)) << (k_in_minor%8)

    w_binary = np.zeros((chan_out*subtile_nb_ki*w_bits, 3 *
                         3, 8, tp_in//8), dtype=np.uint8)
    for k_out in range(chan_out):
        for k_in_major in range(subtile_nb_ki):
            for filt_h in range(3):
                for filt_w in range(3):
                    for k_in_minor in range(tp_in):
                        w_layout_idx = k_out * subtile_nb_ki * w_bits + k_in_major * w_bits
                        if k_in_major*tp_in+k_in_minor >= chan_in:
                            for i in range(w_bits):
                                w_binary[w_layout_idx+i, filt_h*3+filt_w,
                                         k_in_minor % 8, k_in_minor//8] = 0
                        else:
                            for i in range(w_bits):
                                w_binary[w_layout_idx+i, filt_h*3+filt_w, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                                    (W[k_out, filt_h, filt_w, k_in_major*tp_in+k_in_minor]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((chan_out, subtile_nb_ki, w_bits, 3, 3, 2))
    return w_layout

def ne16_conv_dw_3x3_weight_layout(W, w_bits=8):
    tp_in = 16
    chan_in = W.shape[0]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    w_binary = np.zeros((subtile_nb_ki*w_bits, 3*3, 8, tp_in//8), dtype=np.uint8)
    for k_in_major in range(subtile_nb_ki):
        for filt_h in range(3):
            for filt_w in range(3):
                for k_in_minor in range(tp_in):
                    w_layout_idx = k_in_major * w_bits
                    if k_in_major*tp_in+k_in_minor >= chan_in:
                        for i in range(w_bits):
                            w_binary[w_layout_idx+i, filt_h*3+filt_w,
                                     k_in_minor % 8, k_in_minor//8] = 0
                    else:
                        for i in range(w_bits):
                            w_binary[w_layout_idx+i, filt_h*3+filt_w, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                                (W[k_in_major*tp_in+k_in_minor, filt_h, filt_w, 0]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((1, subtile_nb_ki, w_bits, 3, 3, 2))
    return w_layout

def ne16_conv_1x1_weight_layout(W, w_bits=8):
    tp_in = 16
    # tp_out = 32
    chan_out = W.shape[0]
    chan_in = W.shape[3]
    subtile_nb_ki = chan_in // tp_in + (1 if (chan_in % tp_in) else 0)
    w_binary = np.zeros((chan_out*subtile_nb_ki, w_bits,
                         8, tp_in//8), dtype=np.uint8)
    for k_out in range(chan_out):
        for k_in_major in range(subtile_nb_ki):
            for k_in_minor in range(tp_in):
                w_layout_idx = k_out * subtile_nb_ki + k_in_major
                if k_in_major*tp_in+k_in_minor >= chan_in:
                    for i in range(w_bits):
                        w_binary[w_layout_idx, i, k_in_minor %
                                 8, k_in_minor//8] = 0
                else:
                    for i in range(w_bits):
                        w_binary[w_layout_idx, i, k_in_minor % 8, k_in_minor//8] = 0x1 & (
                            (W[k_out, 0, 0, k_in_major*tp_in+k_in_minor]) >> i)
    space = np.logspace(0, 7, num=8, base=2, dtype=np.int32).reshape((8, 1))
    w_layout = np.sum(w_binary * space, axis=2, dtype=np.uint8)
    w_layout = w_layout.reshape((chan_out, subtile_nb_ki, w_bits, 2))
    return w_layout

def ne16_linear_weight_layout(W, w_bits=8):
    chan_out = W.shape[0]
    chan_in = W.shape[1]
    nb_ki = chan_in // 16 + (1 if chan_in % 16 != 0 else 0)
    w_binary = np.zeros((chan_out * w_bits * nb_ki, 2), dtype=np.uint8)
    for ko in range(chan_out):
        for ki in range(chan_in):
            kimaj = ki // 16
            kimin = ki % 16
            byte = kimin // 8
            shift = kimin % 8
            for q in range(w_bits):
                index = ko*nb_ki*w_bits + q*nb_ki + kimaj
                w_binary[index, byte] = w_binary[index, byte] | (
                    1 << shift if (W[ko, ki] & (1 << q) != 0) else 0)
    return w_binary
