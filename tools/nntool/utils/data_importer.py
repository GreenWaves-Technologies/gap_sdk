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

import os
import logging

import numpy as np
from PIL import Image

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

VALID_IMAGE_EXTENSIONS = ['.pgm', '.png', '.ppm']
VALID_SOUND_EXTENSIONS = ['.raw', '.pcm']

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
        img_in.convert(mode=kwargs['mode'])

    if 'nptype' in kwargs:
        nptype = getattr(np, kwargs['nptype'])
    else:
        nptype = np.uint8

    channels = MODES[img_in.mode]
    # TODO - this needs to be smarter for different image pixel types
    img_in = np.array(img_in, dtype=nptype)

    if kwargs.get('transpose'):
        if channels == 1:
            img_in = img_in.transpose((1, 0)).reshape((channels, height, width))
        else:
            img_in = img_in.transpose((2, 0, 1)).copy()
    elif channels == 1:
        img_in = img_in.reshape((channels, width, height))

    divisor = kwargs.get('divisor') or 1
    offset = kwargs.get('offset') or 0
    shift = kwargs.get('shift') or 0

    if shift:
        if shift < 0:
            img_in = img_in >> int(-shift)
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


    return img_in

def import_sound_data(filename, **kwargs):
    raise NotImplementedError()

def import_data(filename, **kwargs):
    _, ext = os.path.splitext(filename)
    if ext in VALID_IMAGE_EXTENSIONS:
        return import_image_data(filename, **kwargs)
    if ext in VALID_SOUND_EXTENSIONS:
        return import_sound_data(filename, **kwargs)
    LOG.debug("no import tool for file %s with extension %s", filename, ext)
    raise NotImplementedError('unknown file extension for import data')
