# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from utils.stats_funcs import STATS_BITS, bits, get_quantization, closest_greater
from .qtype import QType

# pylint: disable=too-many-locals
def calculate_filter_q(node, in_q, min_qsnr, force_width, acc_as_calc=False, bias_as_out=True):
    fstats = node.stats
    w_q = get_quantization(fstats['weights'], min_qsnr, force_width)
    o_q = get_quantization(node.activation_stats, min_qsnr, force_width)
    calc_width = closest_greater(in_q.bits + w_q.bits)
    calc_q = in_q.q + w_q.q

    acc_bits = bits(node.activation_stats['max_acc'], node.activation_stats['min_acc'])
    act_bits = bits(node.activation_stats['max'], node.activation_stats['min'])
    act_acc_bits = max(acc_bits, act_bits)

    calc_int_bits = calc_width - calc_q
    if calc_int_bits < act_acc_bits:
        missing_bits = act_acc_bits - calc_int_bits
        assert w_q.q >= missing_bits, "no space in weights to reduce precision"
        w_q.q = w_q.q - missing_bits
        calc_q = in_q.q + w_q.q

    c_q = QType(bits=calc_width, q=calc_q, signed=True)

    if 'biases' in fstats:
        b_q = get_quantization(fstats['biases'], min_qsnr, force_width)
        if bias_as_out:
            o_q.q = min(b_q.q, o_q.q)
            b_q.q = o_q.q
    else:
        b_q = o_q.q

    if acc_as_calc or acc_bits > o_q.bits - o_q.q:
        acc_q = c_q
    else:
        acc_q = o_q

    norm = c_q.q - o_q.q
    node.quantization = {"input_q": in_q, "weights_q": w_q,
                         "biases_q": b_q, "norm": norm, "calc_q": c_q,
                         "acc_q": acc_q}
    return o_q

def filter_adjust_inputq(node, in_q: QType):
    if node.in_qs[0] == in_q:
        return
    node.in_qs[0] = in_q.clone()
    if in_q.q + node.weights_q.q + node.calc_q.bits - node.calc_q.q > node.calc_q.bits:
        assert node.calc_q.bits < STATS_BITS[-1]
        factor = 2
    else:
        factor = 1

    node.calc_q = QType(bits=node.calc_q.bits * factor, q=in_q.q + node.weights_q, signed=True)
