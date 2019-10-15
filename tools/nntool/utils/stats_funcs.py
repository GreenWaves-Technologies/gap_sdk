# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import math

import numpy as np

from quantization.qtype import QType

STATS_BITS = [8, 16, 32]

def bits(min_num, max_num):
    num = math.floor(max(math.fabs(min_num), math.fabs(max_num))+0.5)
    if num:
        return math.ceil(math.log(num) / math.log(2)) + 2
    return 1

def astats(npa, do_bits=True):
    """Extracts statistics from a tensor
    """
    mean = np.mean(npa)
    std = np.std(npa)
    amax = np.amax(npa)
    amin = np.amin(npa)
    quant1_3 = np.quantile(npa, [0.25, 0.75])
    iqr = quant1_3[1] - quant1_3[0]
    weak_min = (npa < quant1_3[0] - 1.5 * iqr)
    weak_max = (npa > quant1_3[1] + 1.5 * iqr)
    strong_min = (npa < quant1_3[0] - 3 * iqr)
    strong_max = (npa > quant1_3[1] + 3 * iqr)
    weak_count = (weak_min | weak_max).sum()
    strong_count = (strong_min|strong_max).sum()
    if weak_count:
        min_out = np.min(np.abs(npa[weak_min|weak_max]))
        if strong_count:
            max_out = np.max(np.abs(npa[strong_min|strong_max]))
        else:
            max_out = np.max(np.abs(npa[weak_min|weak_max]))
    else:
        min_out = max_out = 0

    ret = {
        'mean': mean,
        'std': std,
        'min': amin,
        'max': amax,
        'size': npa.size,
        'wols': weak_count,
        'sols': strong_count,
        'min_out' : min_out,
        'max_out' : max_out,
    }
    if do_bits:
        ret['ibits'] = bits(amax, amin)
    return ret

def max_error(orig, quant):
    qerr = orig - quant
    return np.max(np.abs(qerr))

def qsnr(orig, quant):
    """Calculate the QSNR between two tensors
    """
    qerr = orig - quant
    sum_err = np.sum(qerr * qerr)
    sum_orig = np.sum(orig * orig)
    if sum_err > 0:
        if sum_orig < sum_err:
            # Means error is larger than signal
            return -int(round(10 * math.log10(sum_err/sum_orig), 0))
        # Error portion of signal
        return int(round(10 * math.log10(sum_orig/sum_err), 0))
    # Means no error
    return math.inf

def calculate_qsnr(npa, bit_size, frac_bits):
    """Calculate the QSNR when a tensor is quantized
    """
    qnpa = np.floor((npa * 2.0 ** frac_bits) + 0.5)
    max_value = 2**(bit_size - 1) - 1
    min_value = -max_value - 1
    qnpa = np.clip(qnpa, min_value, max_value)
    qnpa = (qnpa / 2.0 ** frac_bits)
    return qsnr(npa, qnpa)

def calculate_qsnrs(npa, ideal_ibits, force_ideal=True):
    """"Walk away from the ideal whole bit representation to see if
        there is something better around it.
    """
    store = {}
    def get_qsnr(npa, bit_size, frac_bits):
        """ Don't recalculate values we already have
        """
        nonlocal store

        if frac_bits < 0 or frac_bits > bit_size:
            return -math.inf
        if frac_bits not in store:
            store[frac_bits] = calculate_qsnr(npa, bit_size, frac_bits)
        return store[frac_bits]

    qstats = {}

    # Already quantized
    if not np.issubdtype(npa.dtype, np.floating):
        return qstats

    for bit_size in STATS_BITS:

        frac_bits = max(bit_size - ideal_ibits, 0)

        if force_ideal:
            get_qsnr(npa, bit_size, frac_bits)
        else:
            while True:
                t_low = get_qsnr(npa, bit_size, frac_bits - 1)
                t_mid = get_qsnr(npa, bit_size, frac_bits)
                t_high = -100 if frac_bits + 1 >= bit_size\
                    else get_qsnr(npa, bit_size, frac_bits + 1)
                if t_low > t_mid:
                    frac_bits -= 1
                elif t_high > t_mid:
                    frac_bits += 1
                else:
                    break

        qstats[bit_size] = {'q': frac_bits, 'qsnr': store[frac_bits]}
        store.clear()

    return qstats

def closest_greater(v):
    for i in STATS_BITS:
        if v <= i:
            return i
    return STATS_BITS[-1]

def get_quantization(stats, min_qsnr, force_width):
    qstats = stats['qstats']
    if force_width is not None:
        return QType(bits=force_width, q=qstats[force_width]['q'], signed=True)
    for width in STATS_BITS:
        if qstats[width]['qsnr'] > min_qsnr:
            return QType(bits=width, q=qstats[width]['q'], signed=True)
    raise ValueError("no solution for this QSNR could be found")

def get_current_qsnr(stats, width):
    return stats['qstats'][width]['qsnr']
