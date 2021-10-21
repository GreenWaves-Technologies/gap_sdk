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

import numpy as np
from graph.types import MFCCPreprocessingParameters, RFFT2DPreprocessingParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import (apply_multiplicative_bias,
                                                 apply_zero_offset_bias)
from quantization.new_qrec import QRec
from utils.at_norm import at_norm

# pylint: disable=invalid-name

LOG = logging.getLogger("nntool." + __name__)

class DSPKernelBaseFloat(KernelBase):
    @classmethod
    def preemphasis(cls, params, in_data):
        # if params.preemp_factor:
            # need preemp here
        return in_data

    @classmethod
    def windowing(cls, params, in_data, win_lut, qrec):
        return in_data * win_lut

    @classmethod
    def spectrogram_step(cls, params, in_data, qrec):
        spectrogram = (in_data.real ** 2) + (in_data.imag ** 2)
        if not params.magsquared:
            spectrogram = np.sqrt(spectrogram)
        return spectrogram

    @classmethod
    def melspectrogram_step(cls, params, in_data, filterbanks_sparsity, filterbank_coeff):
        melspectrogram = []
        for i, (start, nonzero_items, base) in enumerate(filterbanks_sparsity.astype(np.int16)):
            melbin = 0
            for j in range(start, start+nonzero_items):
                melbin += filterbank_coeff[base+j-start] * in_data[j]
            melspectrogram.append(melbin)
        return np.array(melspectrogram)

    @classmethod
    def log_step(cls, params, in_data):
        if params.log_offset:
            in_data += params.log_offset
        in_data = np.where(in_data < 1e-6, 1e-6, in_data)
        if params.log_type == "db":
            return 10*np.log10(in_data)
        return np.log(in_data)


@params_type(RFFT2DPreprocessingParameters)
@qrec_type('float')
class Rfft2Dfloat(DSPKernelBaseFloat):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):

        spectrograms = []
        for frame_idx in range(params.n_frames):
            in_data = in_tensors[0][params.frame_step*frame_idx:params.frame_step*frame_idx+params.frame_size]
            in_data = cls.preemphasis(params, in_data)
            if params.win_fn:
                win_lut = in_tensors[1]
                in_data = cls.windowing(params, in_data, win_lut, qrec)

            fft_out = np.fft.rfft(in_data, params.n_fft)
            spectrograms.append(cls.spectrogram_step(params, fft_out, qrec))
        return [np.array(spectrograms)]


@params_type(MFCCPreprocessingParameters)
@qrec_type('float')
class Mfcc2Dfloat(DSPKernelBaseFloat):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        mel_filterbank_sparsity_mat = in_tensors[5]
        mel_filterbank_coeff = in_tensors[6]
        if params.n_dct:
            dct_matrix = in_tensors[7]

        result = []
        for frame_idx in range(params.n_frames):
            in_data = in_tensors[0][params.frame_step*frame_idx:params.frame_step*frame_idx+params.frame_size]
            in_data = cls.preemphasis(params, in_data)
            if params.win_fn:
                win_lut = in_tensors[1]
                in_data = cls.windowing(params, in_data, win_lut, qrec)

            fft_out = np.fft.rfft(in_data, params.n_fft)
            spectrogram = cls.spectrogram_step(params, fft_out, qrec)

            melspect = cls.melspectrogram_step(params, spectrogram, mel_filterbank_sparsity_mat, mel_filterbank_coeff)
            if params.mel_type == "melspectrogram":
                result.append(melspect)
                continue

            logmelspect = cls.log_step(params, melspect)
            if params.mel_type == "logmelspectrogram":
                result.append(logmelspect)
                continue

            if params.n_dct:
                mfcc = np.dot(dct_matrix, logmelspect)
                result.append(mfcc)

        return [np.array(result)]
