import numpy as np
from graph.dim import Dim

def rgb565_rgb888(input_tensor: np.ndarray, in_dim: Dim, out_dim: Dim):
    assert in_dim.is_named and in_dim.c == 1 and out_dim.is_named and out_dim.c == 3
    input_tensor = np.repeat(input_tensor.transpose(in_dim.transpose_to_order(("h", "w", "c"))), 3, axis=2)
    input_tensor[:, :, 1] = (input_tensor[:, :, 0] & (63 << 5)) >> 3
    input_tensor[:, :, 2] = (input_tensor[:, :, 0] & 31) << 3
    input_tensor[:, :, 0] = (input_tensor[:, :, 0] & (31 << 11)) >> 8
    return input_tensor.astype(np.uint8).transpose(out_dim.transpose_from_order(("h", "w", "c")))

def from_hwc(input_tensor: np.ndarray, in_dim: Dim, out_dim: Dim):
    del in_dim
    return input_tensor.astype(np.uint8).transpose(out_dim.transpose_from_order(("h", "w", "c")))

def no_change(input_tensor: np.ndarray, in_dim: Dim, out_dim: Dim):
    del in_dim
    return input_tensor.astype(np.uint8).reshape(out_dim.shape)

def out_int16(input_tensor: np.ndarray):
    return input_tensor.astype(np.int16) << 7

def shift_int8(input_tensor):
    return (input_tensor >> 1).astype(np.int8)

def offset_int8(input_tensor):
    return (input_tensor.astype(np.int16) - 128).astype(np.int8)

FORMAT_CHANGES = {
    "RGB565_RGB888": rgb565_rgb888,
    "RGB888": from_hwc,
    "RGB16": from_hwc,
    "BW8": no_change,
    "BW16": no_change
}

NORMALIZATIONS = {
    "SHIFT_INT8": shift_int8,
    "OFFSET_INT8": offset_int8,
    "OUT_INT16": out_int16
}
