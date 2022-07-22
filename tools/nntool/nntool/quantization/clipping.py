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

# import scipy.optimize as opt
# sigma=1.0
# ALPHA_GAUS = {m: opt.minimize_scalar(lambda x: mse_gaussian(x, sigma=sigma, num_bits=m)).x for m in range(2,17,1)}
# b=1.
# ALPHA_LAPLACE = {m: opt.minimize_scalar(lambda x: mse_laplace(x, b=b, num_bits=m)).x for m in range(2,17,1)}

ALPHA_GAUS = {2: 1.7106351863419305, 3: 2.1515927416420935, 4: 2.559136455058456, 5: 2.9362006203824396,
              6: 3.2869143669161147, 7: 3.615114210893466, 8: 3.924034014599462, 9: 4.216330919936089,
              10: 4.494170448727792, 11: 4.759309171709873, 12: 5.013218066309031, 13: 5.2570849373594974,
              14: 5.491968790304721, 15: 5.7186999893215, 16: 5.937970657819115}

ALPHA_LAPLACE = {2: 2.830682989304011, 3: 3.89722946313961, 4: 5.028640140480669, 5: 6.204766334217521,
                 6: 7.413126215019491, 7: 8.645619949475485, 8: 9.896759823828738, 9: 11.16268502214751,
                 10: 12.440591336219248, 11: 13.728384769877623, 12: 15.024464757336403, 13: 16.32758309514459,
                 14: 17.6367486184042, 15: 18.95116231019748, 16: 20.270171640301292}
GAUSSIAN_CONST = (0.5 * 0.35) * (1 + (math.pi * math.log(4)) ** 0.5)


def get_alpha_laplace(num_bits, b):
    alpha = ALPHA_LAPLACE[num_bits] * b
    return alpha, b


def get_alpha_gaus(shape, num_bits, stat):
    size = np.prod(shape)
    std = ((stat[1] - stat[0]) * GAUSSIAN_CONST) / \
        ((2 * math.log(size)) ** 0.5)
    alpha = ALPHA_GAUS[num_bits] * std
    return alpha, std


def mse_laplace(b, alpha, num_bits):
    return 2 * (b ** 2) * np.exp(-alpha / b) + ((alpha ** 2) / (3 * 2 ** (2 * num_bits)))


def mse_gaussian(sigma, alpha, num_bits):
    clipping_err = (sigma ** 2 + (alpha ** 2)) * (1 - math.erf(alpha / (sigma * np.sqrt(2.0)))) - \
        np.sqrt(2.0 / np.pi) * alpha * sigma * \
        (np.e ** ((-1) * (0.5 * (alpha ** 2)) / sigma ** 2))
    quant_err = (alpha ** 2) / (3 * (2 ** (2 * num_bits)))
    return clipping_err + quant_err


# def alpha2DeltaOffset(self, alpha, max_value, min_value, mean):
#     max_range = max_value - min_value
#     if alpha <= 0 or alpha >= max_range / 2:
#         delta = max_range
#     else:
#         delta = 2 * alpha
#         min_value = max(min_value, mean - delta / 2)

#     return delta, min_value

def get_whiskers(percentiles, whisker_mult=1.5):
    iqr = percentiles[0] - percentiles[2]
    margin = whisker_mult * iqr
    return percentiles[2] - margin, percentiles[0] + margin


def get_clip(shape, num_bits, stat, clip_type):
    min_max = (stat['min'], stat['max'])
    if clip_type == "laplace":
        alpha, _ = get_alpha_laplace(num_bits, stat['b'])
    elif clip_type == "gaus":
        alpha, _ = get_alpha_gaus(shape, num_bits, min_max)
    elif clip_type == "mix":
        alpha_laplace, b = get_alpha_laplace(num_bits, stat['b'])
        alpha_gaus, std = get_alpha_gaus(shape, num_bits, min_max)
        # catch zero range
        if (alpha_gaus, std) == (0.0, 0.0) or (alpha_laplace, b) == (0.0, 0.0):
            return min_max
        else:
            mse_est_laplace = mse_laplace(b, alpha_laplace, num_bits)
            mse_est_gaus = mse_gaussian(std, alpha_gaus, num_bits)
            if mse_est_laplace < mse_est_gaus:
                alpha = alpha_laplace
            else:
                alpha = alpha_gaus
    # elif clip_type == "whiskers":
    #     return get_whiskers(stat['percentiles'])
    elif clip_type.startswith("std"):
        offset = float(clip_type[3:]) * stat['std']
        return stat['mean'] - offset, stat['mean'] + offset 
    elif clip_type == "none":
        return min_max
    else:
        raise ValueError(f'unknown clip type {clip_type}')
    max_range = min_max[1] - min_max[0]
    if alpha <= 0 or alpha >= max_range / 2:
        return min_max[0], min_max[1]

    min_value = max(min_max[0], min_max[1] - alpha)
    max_value = min_value + 2 * alpha

    return min_value, max_value

CLIP_TYPE_OPTION = {
    'name': 'clip_type',
    'type': str,
    'choices': ['laplace', 'gaus', 'mix', 'std3', 'std5', 'none'],
    'help': """Clipping method for any node that modifies its input:
none - the minimum and maximum observed values are used.
laplace, gaus - Values chosen based on laplace or gaussian distribution
mix - MSE is used to estimate if distribution is laplace or gaussian
std3, std5 - 3 or 5 times standard deviation from mean""",
    'default': 'none'
}
