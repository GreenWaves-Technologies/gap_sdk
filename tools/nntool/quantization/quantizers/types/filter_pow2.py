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
from typing import Sequence

from graph.types import (FilterParameters,
                         MultiplicativeBiasParameters)
from quantization.qtype import QType
from quantization.symmetric.symmetric_quantization import (
    SymmetricFilterQuantizationRecord,
    SymmetricScalableFilterQuantizationRecord)
from utils.stats_funcs import bits

LOG = logging.getLogger('nntool.' + __name__)

def pow2_filter(quantizer,
                node: FilterParameters,
                in_qs: Sequence[QType],
                opts=None,
                force_out=None):

    w_q = QType.from_array(node.weights, opts['default_width'])
    in_q = in_qs[0]

    calc_width = 32
    calc_q = in_q.q + w_q.q

    acc_bits = bits(quantizer.astats['max_acc'], quantizer.astats['min_acc'])
    act_bits = bits(quantizer.astats['max'], quantizer.astats['min'])
    act_acc_bits = max(acc_bits, act_bits)

    calc_int_bits = calc_width - calc_q
    if calc_int_bits < act_acc_bits:
        # we don't have enough space for the integer portion so reduce the precision of
        # the weights
        missing_bits = act_acc_bits - calc_int_bits
        # TODO - This needs improving
        assert w_q.q >= missing_bits, "no space in weights to reduce precision"
        w_q.q = w_q.q - missing_bits
        calc_q = in_q.q + w_q.q
        calc_int_bits = calc_width - calc_q

    c_q = acc_q = QType(bits=calc_width, q=calc_q, signed=True)

    if opts['out_as_acc']:
        o_q = c_q
        if node.has_biases:
            b_q = QType.from_array(node.biases, acc_bits)
        else:
            b_q = o_q
    else:
        # The output size is requested to be force_out_width size
        if force_out and force_out.bits:
            # The output fixed point position is also forced
            if force_out.q:
                if (force_out.bits - force_out.q) < act_acc_bits:
                    # clipping so cannot completely satisfy
                    o_q = QType(bits=force_out.bits,
                                q=force_out.bits - act_acc_bits,
                                signed=True)
                else:
                    if force_out.q > calc_q:
                        # We cannot shift left in the kernel
                        # TODO - This should try to increase the input q
                        # Unlikely to happen
                        raise NotImplementedError()
                    # We can satisfy the force
                    o_q = QType(bits=force_out.bits,
                                q=force_out.q,
                                signed=True)
            else:
                # Only the width is forced
                o_q = QType(bits=force_out.bits,
                            q=quantizer.astats[force_out.bits]['q'],
                            signed=True)
        else:
            # The output width is not forced so calculate the output q normally
            o_q = QType(bits=opts['default_width'],
                            q=quantizer.astats[opts['default_width']]['q'],
                            signed=True)
            if force_out and force_out.q:
                # The output fixed point position is forced
                if force_out.q > calc_q:
                    # We cannot shift left in the kernel
                    # TODO - This should try to increase the input q
                    # Unlikely to happen
                    raise NotImplementedError()
                o_q.q = force_out.q

        if node.has_biases:
            if opts['biases_bits_as_acc']:
                b_q = QType.from_array(node.biases, calc_width)
            else:
                # if we are forcing width then match the output size which might
                # have been promoted if the activation didn't fit
                b_q = QType.from_array(node.biases, o_q.bits)
        else:
            b_q = o_q
    # make sure that the biases are not stored more precisily than the accumulator. It's pointless and will
    # cause a negative shift
    if b_q.q > acc_q.q:
        b_q.q = acc_q.q

    if isinstance(node, MultiplicativeBiasParameters):
        if node.has_mul_bias:
            mb_q = QType.from_array(node.mul_biases, opts['default_width'])
        else:
            mb_q = None
        qrec = SymmetricScalableFilterQuantizationRecord(in_qs=[in_q], out_qs=[o_q], calc_q=c_q,
                                                         acc_q=acc_q, biases_q=b_q, weights_q=w_q,
                                                         mul_biases_q=mb_q,
                                                         constants_are_quantized=False)
    else:
        qrec = SymmetricFilterQuantizationRecord(in_qs=[in_q], out_qs=[o_q], calc_q=c_q,
                                                 acc_q=acc_q, biases_q=b_q, weights_q=w_q,
                                                 constants_are_quantized=False)

    LOG.debug("filter %s qrec %s", node.name, qrec)
    return qrec
