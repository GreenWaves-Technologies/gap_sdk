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

import math

import numpy as np
import scipy.spatial.distance as dis

from quantization.qtype import QType

STATS_BITS = [8, 16, 32]

def range_twos_complement(bits):
    return (math.pow(-2, bits - 1), math.pow(2, bits - 1) - 1)

def calc_bits(num, signed=True):
    abs_num = math.floor(math.fabs(num))
    if num < 0 and abs_num > 0:
        abs_num -= 1
    if abs_num == 0:
        return 1
    # calculate number of bits to represent absolute number
    return math.floor(math.log(abs_num) / math.log(2)) + 2

def bits(max_num, min_num, signed=True):
    assert signed or (max_num >= 0 and min_num >= 0), "numeric error"
    return max(calc_bits(min_num), calc_bits(max_num))

def do_stat(npa, do_bits=True, channel_dim=None, all_channel_range=None):
    mean = float(np.mean(npa))
    std = float(np.std(npa))
    amax = float(np.amax(npa))
    amin = float(np.amin(npa))
    quant1_3 = np.quantile(npa, [0.25, 0.75])
    iqr = quant1_3[1] - quant1_3[0]
    weak_min = (npa < quant1_3[0] - 1.5 * iqr)
    weak_max = (npa > quant1_3[1] + 1.5 * iqr)
    strong_min = (npa < quant1_3[0] - 3 * iqr)
    strong_max = (npa > quant1_3[1] + 3 * iqr)
    weak_count = int((weak_min | weak_max).sum())
    strong_count = int((strong_min|strong_max).sum())
    if weak_count:
        min_out = float(np.min(np.abs(npa[weak_min|weak_max])))
        if strong_count:
            max_out = float(np.max(np.abs(npa[strong_min|strong_max])))
        else:
            max_out = float(np.max(np.abs(npa[weak_min|weak_max])))
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
    # all_channel_range must not be 0
    if all_channel_range and npa.size > 1:
        if channel_dim is not None:
            # there is no point to this if there is only one item per channel
            if not all(npa.shape[axis] == 1 if axis != channel_dim else True for axis in range(len(npa.shape))):
                dims = tuple(dim for dim in range(len(npa.shape)) if dim != channel_dim)
                ret['avg_prec'] = np.average(np.ptp(npa, axis=dims)/all_channel_range)
        else:
            ret['avg_prec'] = np.ptp(npa)/all_channel_range

    return ret

def astats(npa, do_bits=True, channel_dim=None, channel_details=None):
    """Extracts statistics from a tensor
    """
    all_channel_range = np.ptp(npa)
    ret = do_stat(npa, do_bits=do_bits, channel_dim=channel_dim, all_channel_range=all_channel_range)
    if channel_details and channel_dim is not None:
        idx = [slice(None) for dim in npa.shape]
        channel_data = []
        for channel in range(npa.shape[channel_dim]):
            idx[channel_dim] = slice(channel, channel + 1)
            channel_data.append(do_stat(npa[tuple(idx)], do_bits=True, all_channel_range=all_channel_range))
        ret['channel_stats'] = channel_data
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
            if sum_orig == 0:
                return -math.inf
            else:
                # Means error is larger than signal
                return -int(round(10 * math.log10(sum_err/sum_orig), 0))
        # Error portion of signal
        return int(round(10 * math.log10(sum_orig/sum_err), 0))
    # Means no error
    return math.inf

def cos_similarity(x, y):
    x = x.flatten()
    y = y.flatten()
    if np.sum(np.abs(x)) == 0 or np.sum(np.abs(y)) == 0:
        x = np.add(x, 1e-5)
        y = np.add(y, 1e-5)
    return 1 - dis.cosine(x, y)

def calculate_qsnr(npa, bit_size, frac_bits):
    """Calculate the QSNR when a tensor is quantized
    """
    qnpa = np.floor((npa * 2.0 ** frac_bits) + 0.5)
    max_value = 2**(bit_size - 1) - 1
    min_value = -max_value - 1
    qnpa = np.clip(qnpa, min_value, max_value)
    qnpa = (qnpa / 2.0 ** frac_bits)
    return qsnr(npa, qnpa)

def calculate_qsnrs(npa, ideal_ibits, force_ideal=False):
    """"Walk away from the ideal whole bit representation to see if
        there is something better around it.
    """
    store = {}
    def get_qsnr(npa, bit_size, frac_bits):
        """ Don't recalculate values we already have
        """
        nonlocal store

        if frac_bits < 0 or frac_bits >= bit_size:
            return -math.inf
        if frac_bits not in store:
            store[frac_bits] = calculate_qsnr(npa, bit_size, frac_bits)
        return store[frac_bits]

    qstats = {}

    # Already quantized
    if not np.issubdtype(npa.dtype, np.floating):
        return qstats

    for bit_size in STATS_BITS:

        frac_bits = min(max(bit_size - ideal_ibits, 0), bit_size -  1)

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
