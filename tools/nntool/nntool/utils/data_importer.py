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
import os
from collections.abc import Iterator
from glob import glob
from typing import Sequence, Union

import numpy as np
import scipy.io.wavfile as wav
from nntool.utils.at_norm import at_norm
from PIL import Image

LOG = logging.getLogger(__name__)

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
        if len(img_in.shape) == 3:
            if c == 1:
                img_in = img_in.reshape((c, h, w))
            else:
                img_in = img_in.transpose((2, 0, 1)).copy()
        elif len(img_in.shape) == 2:
            img_in = img_in.transpose((1, 0)).copy()
        else:
            raise ValueError(
                f"Don't know how to automatically transpose input of shape {img_in.shape}, \
                     maybe you need to do it offline before passing it to nntool")
    # elif c == 1:
    #     img_in = img_in.reshape((c, h, w))

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
        if isinstance(norm_func, str):
            g_env = {}.update(np.__dict__)
    # pylint: disable=eval-used
            compiled_norm_func = eval('lambda ' + norm_func, g_env)
        elif callable(norm_func):
            compiled_norm_func = norm_func
        else:
            raise ValueError('expect norm_func to be callable or a string of the form "x: f(x)"')
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

    if kwargs.get('zero_pad'):
        padding = np.array(kwargs.get("zero_pad")).reshape(-1, 2)
        img_in = np.pad(img_in, padding)

    if kwargs.get('slices'):
        slices = np.array(kwargs.get("slices")).reshape(-1, 3)
        slices = [slice(s[0], s[1], s[2]) for s in slices]
        img_in = img_in[tuple(slices)]

    return img_in

def import_image_data(filename, **kwargs):
    img_in = Image.open(filename)
    if 'width' not in kwargs or kwargs['width'] is None or kwargs['width'] == -1:
        width = img_in.width
    else:
        width = kwargs['width']

    if 'height' not in kwargs or kwargs['height'] is None or kwargs['height'] == -1:
        height = img_in.height
    else:
        height = kwargs['height']

    if width != img_in.width or height != img_in.height:
        img_in = img_in.resize((width, height))

    if 'mode' in kwargs and kwargs['mode'] is not None:
        img_in = img_in.convert(mode=kwargs['mode'])

    if 'nptype' in kwargs and kwargs['nptype'] is not None:
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
        return postprocess(in_arr, in_arr.shape[0], 1, 1, **kwargs)
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

class FileImporter(Iterator):
    """ Data generator for data from files
    """
    def __init__(self, files, **importer_args) -> None:
        self._importer_args = importer_args
        self._files = files
        self._idx = 0

    @classmethod
    def from_wildcard(cls, wildcard: str, **importer_args):
        return cls(glob(wildcard), **importer_args)

    @classmethod
    def from_wildcards(cls, wildcards: Sequence[Union[str, Sequence[str]]], **importer_args):
        files = zip(glob(wildcard) if isinstance(wildcard, str) else wildcard for wildcard in wildcards)
        return cls(list(files), **importer_args)

    def val(self):
        return next(self)

    def __getitem__(self, idx):
        files = self._files[idx]
        if isinstance(files, str):
            files = [files]
        return [import_data(file, **self._importer_args) for file in files]

    def __iter__(self):
        self._idx = 0
        return self

    def __next__(self):
        idx = self._idx
        if idx >= len(self._files):
            raise StopIteration()
        self._idx += 1
        return self[idx]
