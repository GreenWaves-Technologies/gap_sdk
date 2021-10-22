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
from copy import deepcopy
from utils.pow_sqrt import LN_10_INV_Q10, LN_2_1F15, LOG10_2, gap_fl1, logn_17_15, sqrt_17_15

import numpy as np
from graph.types import MFCCPreprocessingParameters, RFFT2DPreprocessingParameters
from quantization.kernels.kernel_base import KernelBase, params_type, qrec_type
from quantization.multiplicative.mulbias import (apply_multiplicative_bias,
                                                 apply_zero_offset_bias)
from quantization.new_qrec import QRec
from utils.at_norm import at_norm
from utils.fft_quant import Rad2_FFT_DIF_Fix16, Rad4_FFT_DIF_Fix16, RFFT_Step_Fix16, SwapSamples

# pylint: disable=invalid-name

LOG = logging.getLogger("nntool." + __name__)
SQRT_2_Q15 = 0xb504

class DSPKernelBasePow2(KernelBase):
    @classmethod
    def preemphasis(cls, params, in_data, qfft_in):
        # if params.preemp_factor:
            # need preemp here
        max_in = np.max(np.abs(in_data))
        shift = int(qfft_in - np.ceil(np.log2(max_in))) if max_in else 0
        if shift > 0:
            in_data = in_data << shift
        else:
            in_data = in_data >> (-shift)
        return in_data, shift

    @classmethod
    def windowing(cls, params, in_data, win_lut, win_q):
        return at_norm(np.multiply(in_data, win_lut, dtype=np.int32), win_q)

    @classmethod
    def fft_step(cls, params, in_cfft, fft_twiddles, swap_table):
        if params.is_radix4():
            out_cfft = Rad4_FFT_DIF_Fix16(in_cfft, fft_twiddles, params.n_cfft)
        else:
            out_cfft = Rad2_FFT_DIF_Fix16(in_cfft, fft_twiddles, params.n_cfft)
        return SwapSamples(out_cfft, swap_table)

    @classmethod
    def spectrogram_step(cls, params, in_data, shift, fft_out_q):
        spectrogram = (np.uint32(in_data.real) ** 2) + (np.uint32(in_data.imag) ** 2)
        QP = 2*(fft_out_q + shift) - 15
        if params.magsquared:
            # spectrogram = spectrogram >> 2*shift if shift > 0 else spectrogram << (-2*shift)
            spectrogram = spectrogram >> QP if QP > 0 else spectrogram << (-QP)
        else:
            MUL = SQRT_2_Q15 >> 5 if QP % 2 else 1 << 10
            if QP > 0:
                spectrogram = sqrt_17_15(spectrogram, True) >> (QP//2)
                spectrogram = (spectrogram * MUL) >> 11
            else:
                spectrogram = sqrt_17_15(spectrogram, True) << (-QP//2)
                spectrogram = (spectrogram * MUL) >> 10
        return spectrogram.astype(np.uint32)

    @classmethod
    def melspectrogram_step(cls, params, in_data, filterbanks_sparsity, filterbank_coeff, mel_coeff_q):
        melspectrogram = []
        shift_buff = []
        for i, (start, nonzero_items, base) in enumerate(filterbanks_sparsity):
            max_in = np.max(in_data[start:start+nonzero_items])
            logn_items = gap_fl1(nonzero_items)
            shift0 = gap_fl1(max_in) if max_in else 0
            shift = shift0 + mel_coeff_q + logn_items - 31 if shift0 + mel_coeff_q + logn_items > 31 else 0
            melbin = 0
            for j in range(start, start+nonzero_items):
                melbin += filterbank_coeff[base+j-start] * (in_data[j] >> shift)
            melspectrogram.append(melbin if melbin != 0 else 1)
            shift_buff.append(shift)
        return np.array(melspectrogram).astype(np.uint32), np.array(shift_buff)

    @classmethod
    def norm_clip_32_melspect(cls, params, in_data, shift_buff):
        norm = (30 - shift_buff) - 16
        return np.where(norm < 32, in_data >> norm, 0).astype(np.int32)

    @classmethod
    def log_step(cls, params, in_data, mel_coeff_q, shift_buff, fft_out_q, shift, norm):
        if params.log_offset:
            raise NotImplementedError()

        # if params.magsquared:
        #     qformat = mel_coeff_q - 2 - shift_buff + 2*fft_out_q + 2*shift
        # else:
        qformat = 30 - shift_buff

        if params.log_type == "db":
            return np.clip(at_norm(10 * ((logn_17_15(in_data, True) * LN_10_INV_Q10 >> 10) - (qformat - 15) * LOG10_2), norm), -(1<<15), (1<<15)-1).astype(np.int16)
        return np.clip(at_norm(logn_17_15(in_data, True) - (qformat - 15) * LN_2_1F15, norm), -(1<<15), (1<<15)-1).astype(np.int16)


@params_type(RFFT2DPreprocessingParameters)
@qrec_type('scaled', 'symmetric')
class Rfft2DSymmetric(DSPKernelBasePow2):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        in_data = in_tensors[0]
        fft_twiddles = in_tensors[2]
        swap_table = in_tensors[3]
        rfft_twiddles = in_tensors[4]

        spectrograms = []
        for frame_idx in range(params.n_frames):
            in_data = in_tensors[0][params.frame_step*frame_idx:params.frame_step*frame_idx+params.frame_size]
            in_data, shift = cls.preemphasis(params, in_data, 12 if params.is_radix4() else 13)
            if params.win_fn:
                win_lut = in_tensors[1]
                in_data = cls.windowing(params, in_data, win_lut, qrec.in_qs[1].q)

            in_cfft = np.stack([in_data[::2], in_data[1::2]], axis=0)
            out_cfft = cls.fft_step(params, in_cfft, fft_twiddles, swap_table)   
            out_data = RFFT_Step_Fix16(out_cfft, rfft_twiddles, params.n_fft)

            out_data = out_data[0] + 1j*out_data[1]
            spectrograms.append(cls.spectrogram_step(params, out_data, shift, qrec.cache['fft_out_q'].q))

        return [np.array(spectrograms)]


@params_type(MFCCPreprocessingParameters)
@qrec_type('symmetric')
class Mfcc2DSymmetric(DSPKernelBasePow2):
    @classmethod
    def execute(cls, params,
                in_tensors,
                qrec: QRec,
                **kwargs):
        fft_twiddles = in_tensors[2]
        swap_table = in_tensors[3]
        rfft_twiddles = in_tensors[4]

        mel_filterbank_sparsity_mat = in_tensors[5]
        mel_filterbank_coeff = in_tensors[6]
        if params.n_dct:
            dct_matrix = in_tensors[7]

        result = []
        for frame_idx in range(params.n_frames):
            in_data = in_tensors[0][params.frame_step*frame_idx:params.frame_step*frame_idx+params.frame_size]
            in_data, shift = cls.preemphasis(params, in_data, 12 if params.is_radix4() else 13)
            if params.win_fn:
                win_lut = in_tensors[1]
                in_data = cls.windowing(params, in_data, win_lut, qrec.in_qs[1].q)

            in_cfft = np.stack([in_data[::2], in_data[1::2]], axis=0)
            out_cfft = cls.fft_step(params, in_cfft, fft_twiddles, swap_table)   
            out_data = RFFT_Step_Fix16(out_cfft, rfft_twiddles, params.n_fft)

            out_data = out_data[0] + 1j*out_data[1]
            spectrogram = cls.spectrogram_step(params, out_data, shift, qrec.cache['fft_out_q'].q)

            melspect, shift_buff = cls.melspectrogram_step(params, spectrogram, mel_filterbank_sparsity_mat, mel_filterbank_coeff, qrec.in_qs[6].q)
            if params.mel_type == "melspectrogram":
                result.append(cls.norm_clip_32_melspect(params, melspect, shift_buff))
                continue

            logmelspect = cls.log_step(params, melspect, qrec.in_qs[6].q, shift_buff, qrec.cache["fft_out_q"], shift, params.quant_norm)
            if params.mel_type == "logmelspectrogram":
                result.append(logmelspect)
                continue

            if params.n_dct:
                mfcc = np.clip(at_norm(np.dot(dct_matrix, logmelspect), 14), -(1<<15), (1<<15)-1)
                result.append(mfcc)

        return [np.array(result)]
