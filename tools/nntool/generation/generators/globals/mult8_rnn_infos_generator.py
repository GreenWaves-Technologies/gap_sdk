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
from graph.types import (RNNParameters, LSTMParameters)
from quantization.qtype import QType
from utils.node_id import NodeId
from .global_names import *
from .mult8_infos_generator import gen_constant


@generation_function("globals",
                     (RNNParameters, LSTMParameters),
                     qrec_types=(QREC_MULT8,))
def mult8_rnn_infos_generator(gen, node, qrec, pnode, fnode) -> bool:
    del pnode
    if fnode is not None:
        return False
    if isinstance(node, RNNParameters):
        rnn_infos(gen, node, qrec)
    elif isinstance(node, LSTMParameters):
        lstm_infos(gen, node, qrec)
    else:
        raise ValueError()
    return True

# LSTM_F_SCALE 0
# LSTM_F_SCALEN 1
# LSTM_F_A0 2
# LSTM_F_B0 3
# LSTM_F_C0 4
# LSTM_F_ASCALE 5
# LSTM_F_ASCALEN


def sigmoid_infos(gate_name, mult_qtype, qtype):
    scale = mult_qtype.qbiases[0]
    scale_n = mult_qtype.qnorms[0]
    three = qtype.quantize(np.array([3]))[0]
    sixth = qtype.quantize(np.array([1/6]))[0]
    six = qtype.quantize(np.array([6]))[0]
    actn = qtype.q
    comment = str.format("{0}_scale: {1} {0}_scale_n: {2} A0: {3} B0: {4} C0: {5}",
                         gate_name, scale, scale_n, six, three, sixth, 1, actn)
    contents = np.array([scale, scale_n, six, three, sixth, 1, actn], dtype=np.int8)
    return contents, comment


def htanh_infos(gate_name, mult_qtype, qtype):
    scale = mult_qtype.qbiases[0]
    scale_n = mult_qtype.qnorms[0]
    one = qtype.quantize(np.array([1]))[0]
    comment = str.format("{0}_scale: {1} {0}_scale_n: {2} A0: {3} B0: {4}",
                         gate_name, scale, scale_n, -one, one)
    contents = np.array([scale, scale_n, -one, one], dtype=np.int8)
    return contents, comment


def scale_infos(gate_name, mult_qtype):
    scale = mult_qtype.qbiases[0]
    scale_n = mult_qtype.qnorms[0]
    comment = str.format("{0}_scale: {1} {0}_scale_n: {2}",
                         gate_name, scale, scale_n)
    contents = np.array([scale, scale_n], dtype=np.int8)
    return contents, comment


LSTM_INFOS_ORDER = {
    'f': 'sigmoid',
    'i': 'sigmoid',
    'c': 'htanh',
    'o': 'sigmoid',
}

INFOS_FUNCS = {
    'sigmoid': sigmoid_infos,
    'htanh': htanh_infos
}

def highb(x):
    return (x >> 8) & 0xff

def lowb(x):
    return x & 0xff

def lstm_infos(gen, node, qrec):
    internal_qtype = qrec.internal_qtype
    contents = []
    comments = []
    for k, v in LSTM_INFOS_ORDER.items():
        info, comment = scale_infos(k, getattr(qrec, "r_2_%s_q" % k))
        contents.append(info)
        comments.append(comment)
    cin_scale = qrec.cell_in_q.qbiases[0]
    cin_scalen = qrec.cell_in_q.qnorms[0]
    cout_scale = qrec.cell_out_q.qbiases[0]
    cout_scalen = qrec.cell_out_q.qnorms[0]
    out_scale = qrec.state_out_q.qbiases[0]
    out_scalen = qrec.state_out_q.qnorms[0]
    comments.append(str.format("cin_scale: {} cin_scale_n: {} cout_scale: {} cout_scale_n: {}",
                               cin_scale, cin_scalen, cout_scale, cout_scalen,))

    comments.append(str.format("out_scale: {} out_scale_n: {}",
                               out_scale, out_scalen))
    contents.append(np.array([cin_scale, cin_scalen, cout_scale, cout_scalen,
                              out_scale, out_scalen], dtype=np.int8))

    three = internal_qtype.quantize(np.array([3]))[0]
    six = internal_qtype.quantize(np.array([6]))[0]
    sixth = internal_qtype.quantize(np.array([1/6]))[0]

    comments.append(str.format("int_q: {} A0: {} B0: {} C0: {}",
                               internal_qtype.q, six, three, sixth))
    contents.append(np.array([internal_qtype.q, lowb(six), highb(six),
                              lowb(three), highb(three),
                              lowb(sixth), highb(sixth)],
                             dtype=np.int8))

    cname, file_name = gen_constant(gen, node, node, INFOS)
    const_info = ConstantInfo(file_name, QType(bits=8, q=0, signed=True),
                              contents=np.hstack(tuple(contents)))

    gen.globals.append(GlobalArgInfo("int8", cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info,
                                     comment=" ".join(comments)))


def rnn_infos(gen, node, qrec):
    i_state_q = qrec.in_qs[node.INPUT_NAMES.index('i_state')]

    contents, comment = htanh_infos("f", qrec.s_2_s_q, i_state_q)
    cname, file_name = gen_constant(gen, node, node, INFOS)
    const_info = ConstantInfo(file_name, QType(bits=8, q=0, signed=True), contents=contents)

    gen.globals.append(GlobalArgInfo("int8", cname,
                                     gen.opts['default_global_home_location'],
                                     gen.opts['default_global_exec_location'],
                                     const_info=const_info,
                                     comment=comment))
