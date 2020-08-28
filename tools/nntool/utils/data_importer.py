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
import logging

import numpy as np
from PIL import Image
import scipy.io.wavfile as wav
from utils.at_norm import at_norm

LOG = logging.getLogger('nntool.'+__name__)

MODES = {
    '1': 1,     # (1-bit pixels, black and white, stored with one pixel per byte)
    'L': 1,     # (8-bit pixels, black and white)
    'P': 1,     # (8-bit pixels, mapped to any other mode using a color palette)
    'RGB': 3,   # (3x8-bit pixels, true color)
    'RGBA': 4,  # (4x8-bit pixels, true color with transparency mask)
    'CMYK': 4,  # (4x8-bit pixels, color separation)
    'YCbCr': 3, # (3x8-bit pixels, color video format)
    'LAB': 3,   # (3x8-bit pixels, the L*a*b color space)
    'HSV': 3,   # (3x8-bit pixels, Hue, Saturation, Value color space)
    'I': 1,     # (32-bit signed integer pixels)
    'F': 1,     # (32-bit floating point pixels)
}

VALID_IMAGE_EXTENSIONS = ['.pgm', '.png', '.ppm', '.jpg', '.jpeg']
VALID_SOUND_EXTENSIONS = ['.wav', '.raw', '.pcm']
VALID_DATA_IMPORT_EXTENSIONS = ['.npy', '.dat']

def postprocess(img_in, h, w, c, **kwargs):
    if kwargs.get('transpose'):
        if c == 1:
            img_in = img_in.transpose((1, 0)).reshape((c, h, w))
        else:
            img_in = img_in.transpose((2, 0, 1)).copy()
    elif c == 1:
        img_in = img_in.reshape((c, w, h))

    divisor = kwargs.get('divisor') or 1
    offset = kwargs.get('offset') or 0
    shift = kwargs.get('shift') or 0

    if shift:
        if shift < 0:
            img_in = at_norm(img_in, int(-shift))
        else:
            img_in = img_in << int(shift)

    img_in = np.array(img_in)

    norm_func = kwargs.get('norm_func')
    if norm_func:
        g_env = {}.update(np.__dict__)
# pylint: disable=eval-used
        compiled_norm_func = eval('lambda ' + norm_func, g_env)
        img_in = compiled_norm_func(img_in)
        img_in = np.array(img_in, dtype=np.float)
    else:
        img_in = (img_in.astype(np.float) / divisor) + offset

    if kwargs.get('rgb888_rgb565'):
        r = np.bitwise_and(img_in[:, :, 0].flatten().astype(np.int16), 0xf8) << 8
        g = np.bitwise_and(img_in[:, :, 1].flatten().astype(np.int16), 0xfc) << 3
        b = np.bitwise_and(img_in[:, :, 2].flatten().astype(np.int16), 0xf8) >> 3
        img_565 = r + g + b
        img_in = np.array(img_565, dtype=np.int16)

    return img_in

def import_image_data(filename, **kwargs):
    img_in = Image.open(filename)
    if 'width' not in kwargs or kwargs['width'] == -1:
        width = img_in.width
    else:
        width = kwargs['width']

    if 'height' not in kwargs or kwargs['height'] == -1:
        height = img_in.height
    else:
        height = kwargs['height']

    if width != img_in.width or height != img_in.height:
        img_in = img_in.resize((width, height))

    if 'mode' in kwargs:
        img_in = img_in.convert(mode=kwargs['mode'])

    if 'nptype' in kwargs:
        nptype = getattr(np, kwargs['nptype'])
    else:
        nptype = np.uint8

    channels = MODES[img_in.mode]
    # TODO - this needs to be smarter for different image pixel types
    img_in = np.array(img_in, dtype=nptype)
    return postprocess(img_in, height, width, channels, **kwargs)

def import_tensor_data(filename, **kwargs):
    _, ext = os.path.splitext(filename)
    ext = ext.lower()
    if ext == '.npy':
        img_in = np.load(filename)
    else:
        if 'nptype' in kwargs:
            nptype = getattr(np, kwargs['nptype'])
        else:
            nptype = np.float32
            LOG.warning("input datatype is default float32")
        img_in = np.fromfile(filename, dtype=nptype)

    if len(img_in.shape) == 4 and img_in.shape[0] == 1:
        img_in = img_in.reshape(img_in.shape[1:])
    if len(img_in.shape) > 3:
        raise NotImplementedError('Too many input dimensions ( > 3 )')
    while len(img_in.shape) < 3:
        img_in = np.expand_dims(img_in, axis=-1)
    return postprocess(img_in, img_in.shape[0], img_in.shape[1], img_in.shape[2], **kwargs)

def import_sound_data(filename, **kwargs):
    _, ext = os.path.splitext(filename)
    ext = ext.lower()
    if ext == '.wav':
        rate, sig = wav.read(filename)
        LOG.info("Input signal samplerate of %d", rate)
        in_arr = np.array(sig)
        while len(in_arr.shape) < 3:
            in_arr = np.expand_dims(in_arr, axis=-1)
        return postprocess(in_arr, in_arr.shape[0], in_arr.shape[1], in_arr.shape[2], **kwargs)
    raise NotImplementedError()

def import_data(filename, **kwargs):
    _, ext = os.path.splitext(filename)
    ext = ext.lower()
    if ext in VALID_IMAGE_EXTENSIONS:
        return import_image_data(filename, **kwargs)
    if ext in VALID_SOUND_EXTENSIONS:
        return import_sound_data(filename, **kwargs)
    if ext in VALID_DATA_IMPORT_EXTENSIONS:
        return import_tensor_data(filename, **kwargs)
    LOG.debug("no import tool for file %s with extension %s", filename, ext)
    raise NotImplementedError('unknown file extension for import data')
