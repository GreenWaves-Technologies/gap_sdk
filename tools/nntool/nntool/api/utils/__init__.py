# Copyright (C) 2022  GreenWaves Technologies, SAS

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

from typing import Sequence, Union

import numpy as np
from nntool.utils.data_importer import import_data as __import_data
from nntool.utils.data_importer import FileImporter as __FileImporter
from nntool.utils.stats_funcs import qsnr, cos_similarity as cos_sim
from nntool.utils.graph import GraphView
from nntool.utils.random_iter import RandomIter
from nntool.utils.quantization_options import quantization_options
from nntool.utils.model_settings import model_settings

def import_data(
        filepath: str,
        transpose=False,
        norm_func: Union[str, callable] = None,
        rgb888_rgb565=False,
        width: int = None,
        height: int = None,
        slices: Sequence[Sequence[int]] = None,
        zero_pad: Union[Sequence[int], int] = None,
        mode: str = None,
        nptype: str = None) -> np.ndarray:
    """Imports data from image, sound or data files such as numpy npy files.

    File type is detected by file extension. Valid extensions are:

    * Images: .pgm, .png, .ppm, .jpg, .jpeg
    * Sounds: .wav, .raw, .pcm
    * Data: .npy, .dat

    Args:
        filepath (str):
            Full path to file
        transpose (bool, optional):
            Transpose images from HWC to CHW. Defaults to False.
        norm_func (Union[str, callable], optional):
            Normalization function to apply. This can be a function or a string as in the NNTool
            command interfreter. Defaults to None.
        rgb888_rgb565 (bool, optional):
            Reformat RGB565 images to RGB888. Defaults to False.
        width (int, optional):
            Width for image. Defaults to None.
        height (int, optional):
            Height for image. Defaults to None.
        slices (Sequence[Sequence[int]]):
            Slices to take of input. As ((start, stop, step), ...).
            Defaults to None.
        zero_pad: (Sequence[int], int):
            Amount of zero padding to add. As numpy.pad.
            Defaults to None.
        mode (str, optional):
            Modes for image import.
            Can be one of::

                '1':        1-bit pixels, black and white, stored with one pixel per byte
                'L':        8-bit pixels, black and white
                'P':		8-bit pixels, mapped to any other mode using a color palette
                'RGB':		3x8-bit pixels, true color
                'RGBA':		4x8-bit pixels, true color with transparency mask
                'CMYK':		4x8-bit pixels, color separation
                'YCbCr':    3x8-bit pixels, color video format
                'LAB':		3x8-bit pixels, the L*a*b color space
                'HSV':		3x8-bit pixels, Hue, Saturation, Value color space
                'I':		32-bit signed integer pixels
                'F':		32-bit floating point pixels

            Defaults to None.
        nptype (str, optional):
            Set imput tensor as named numpy dtype for example "int16". Defaults to None.

    Returns:
        np.ndarray: Returns imported tensor
    """
    return __import_data(filepath, transpose=transpose, norm_func=norm_func,
                         rgb888_rgb565=rgb888_rgb565, width=width, height=height,
                         mode=mode, nptype=nptype, slices=slices, zero_pad=zero_pad)


class FileImporter(__FileImporter):
    def __init__(self,
                 files: Union[str, Sequence[str]],
                 transpose=False,
                 norm_func: Union[str, callable] = None,
                 rgb888_rgb565=False,
                 width: int = None,
                 height: int = None,
                 mode: str = None,
                 nptype: str = None) -> None:
        """
        Imports data from image, sound or data files such as numpy npy files.
        File type is detected by file extension. Valid extensions are:

        * Images: .pgm, .png, .ppm, .jpg, .jpeg
        * Sounds: .wav, .raw, .pcm
        * Data: .npy, .dat

        Args:
            files (Union[Sequence[str], Sequence[Sequence[str]]]):
                list of files (or list of list of files if multiple inputs) to import
            transpose (bool, optional):
                Transpose images from HWC to CHW. Defaults to False.
            norm_func (Union[str, callable], optional):
                Normalization function to apply. This can be a function or a string as in the NNTool
                command interfreter. Defaults to None.
            rgb888_rgb565 (bool, optional):
                Reformat RGB565 images to RGB888. Defaults to False.
            width (int, optional):
                Width for image. Defaults to None.
            height (int, optional):
                Height for image. Defaults to None.
            mode (str, optional):
                Modes for image import.
                Can be one of::

                    '1':    1-bit pixels, black and white, stored with one pixel per byte
                    'L':    8-bit pixels, black and white
                    'P':    8-bit pixels, mapped to any other mode using a color palette
                    'RGB':	3x8-bit pixels, true color
                    'RGBA':	4x8-bit pixels, true color with transparency mask
                    'CMYK':	4x8-bit pixels, color separation
                    'YCbCr':3x8-bit pixels, color video format
                    'LAB':	3x8-bit pixels, the L*a*b color space
                    'HSV':	3x8-bit pixels, Hue, Saturation, Value color space
                    'I':	32-bit signed integer pixels
                    'F':	32-bit floating point pixels

                Defaults to None.
            nptype (str, optional):
                Set imput tensor as named numpy dtype for example "int16". Defaults to None.

        Returns:
            np.ndarray: Returns imported tensor
        """
        super().__init__(files, transpose=transpose, norm_func=norm_func,
                         rgb888_rgb565=rgb888_rgb565, width=width, height=height,
                         mode=mode, nptype=nptype)


def qsnrs(
        tensors1: Sequence[Sequence[np.ndarray]],
        tensors2: Sequence[Sequence[np.ndarray]],
        idx: int = 0) -> Sequence[float]:
    """Convenience method to calculate the QSNR between two sets of output tensors from the execute method

    Args:
        tensors1 (Sequence[Sequence[np.ndarray]]): Output from execute
        tensors2 (Sequence[Sequence[np.ndarray]]): Output from execute
        idx (int, optional): Output index. Defaults to 0.

    Returns:
        Sequence[float]: QSNRs
    """
    return tuple([qsnr(t1[idx], t2[idx])
                  if (len(t1) > idx and len(t2) > idx and
                      t1[idx] is not None and t2[idx] is not None) else None
                  for t1, t2 in zip(tensors1, tensors2)])


def cos_sims(
        tensors1: Sequence[Sequence[np.ndarray]],
        tensors2: Sequence[Sequence[np.ndarray]],
        idx: int = 0) -> Sequence[float]:
    """Convenience method to calculate the Cosine similarity between two sets of output tensors from
        the execute method

    Args:
        tensors1 (Sequence[Sequence[np.ndarray]]): Output from execute
        tensors2 (Sequence[Sequence[np.ndarray]]): Output from execute
        idx (int, optional): Output index. Defaults to 0.

    Returns:
        Sequence[float]: Cosine similarity
    """
    return tuple([cos_sim(t1[idx], t2[idx])
                  if (len(t1) > idx and len(t2) > idx and
                      t1[idx] is not None and t2[idx] is not None) else None
                  for t1, t2 in zip(tensors1, tensors2)])


__all__ = [
    'import_data',
    'qsnr',
    'cos_sim',
    'qsnrs',
    'cos_sims',
    'GraphView',
    'quantization_options',
    'model_settings',
    'FileImporter',
    'RandomIter'
]
