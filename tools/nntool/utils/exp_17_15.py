# #define Abs(a)      (((int)(a)<0)?(-(a)):(a))
# #define Min(a, b)   (((a)<(b))?(a):(b))
# #define Max(a, b)   (((a)>(b))?(a):(b))

import numpy as np

# static unsigned short int IntegerExpLUT[] =
# {
INTEGER_EXP_LUT = np.array([0x0001, 0x0002, 0x0007, 0x0014, 0x0036, 0x0094,
                            0x0193, 0x0448, 0x0BA4, 0x1FA7, 0x560A, 0xE9E2], dtype=np.uint16)
# };

# static unsigned short int FractionExpLUT[] =
# {
FRACTION_EXP_LUT = np.array([0x0000, 0x5BF1, 0x31CD, 0x0AF3, 0x4C90, 0x34E2,
                             0x36E3, 0x510B, 0x7A9F, 0x0ABE, 0x3B9F, 0x1224], dtype=np.uint16)
# };

# /* 17.15 fixed point format */
# static unsigned short int ExpCoeffLUT[] =
# {
EXP_COEFF_LUT = np.array([0x7FFF, 0x7FFF, 0x4000, 0x1555, 0x0555, 0x0111, 0x002E, 0x0007, 0x0001])
# };


def gap_bitextractu(x, size, off):
    mask = (np.array([1], dtype=np.uint32) << size) - 1
    return (x >> off) & mask


def gap_mulsRN(x, y, n):
    rounding = np.array([1], dtype=np.int32) << (n - 1)
    return (np.multiply(x.astype(np.int16),
                        y.astype(np.int16), dtype=np.int32) + rounding) >> n


def gap_mulRN(x, y, n):
    rounding = np.array([1], dtype=np.int32) << (n - 1)
    return (np.multiply(x.astype(np.uint16),
                        y.astype(np.uint16), dtype=np.int32) + rounding) >> n


def gap_roundnorm(x, scale):
    rounding = np.array([1], dtype=np.int32) << (scale - 1)
    return (x.astype(np.int32) + rounding) >> scale


def exp_fp_17_15(X):
    X = X.astype(np.uint32)
    result = np.zeros(X.shape, dtype=np.int32)
    zero_mask = X == 0
    result[zero_mask] = 0x8000
    non_zero_mask = np.logical_not(zero_mask)
    Y = np.ndarray(X.shape, dtype=np.int32)
    Y[non_zero_mask] = np.abs(X[non_zero_mask].astype(np.int32))

    int_x = np.ndarray(X.shape, dtype=np.int32)
    int_x[non_zero_mask] = Y[non_zero_mask] >> 15

    overflow_mask = np.logical_and(int_x >= (len(INTEGER_EXP_LUT) - 1), non_zero_mask)
    result[np.logical_and(X == Y, overflow_mask)] = 0x7FFFFFF
    result[np.logical_and(X != Y, overflow_mask)] = 0

    non_zero_mask[overflow_mask] = False

    fract_x = np.ndarray(X.shape, dtype=np.int32)
    fract_x[non_zero_mask] = Y[non_zero_mask] & 0x7FFF

    bit_extract_mask = np.logical_and(non_zero_mask, gap_bitextractu(fract_x, 1, 14))
    fract_x[bit_extract_mask] -= 0x8000
    int_x[bit_extract_mask] += 1

    scaled_int = np.ndarray(X.shape, dtype=np.int32)
    scaled_int[non_zero_mask] = INTEGER_EXP_LUT[int_x[non_zero_mask]]

    scaled_fract = np.ndarray(X.shape, dtype=np.uint16)
    scaled_fract[non_zero_mask] = FRACTION_EXP_LUT[int_x[non_zero_mask]]

    fract_x_s = fract_x.astype(np.int16)
    z_s = fract_x.astype(np.int16)
    for i in range(1, len(EXP_COEFF_LUT)):
        result[non_zero_mask] += z_s[non_zero_mask].astype(np.int32) * EXP_COEFF_LUT[i]
        z_s[non_zero_mask] = gap_mulsRN(z_s[non_zero_mask], fract_x_s[non_zero_mask], 15)

    result[non_zero_mask] = gap_roundnorm(result[non_zero_mask], 15) + EXP_COEFF_LUT[0]

    unsigned_res = result.astype(np.uint16)

    result[non_zero_mask] = (gap_mulRN(unsigned_res[non_zero_mask],
                                       scaled_fract[non_zero_mask], 15)\
                                           + unsigned_res[non_zero_mask] * scaled_int[non_zero_mask])
    neg_mask = np.logical_and(np.logical_and(non_zero_mask, result != 0), X > 0x7FFFFFFF)
    result[neg_mask] = ((0x7FFFFFFF)//result[neg_mask]) >> 1

    return result.astype(np.uint32)
