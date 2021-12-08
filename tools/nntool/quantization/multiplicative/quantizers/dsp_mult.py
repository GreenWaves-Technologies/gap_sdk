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

from graph.types.dsp_preprocessing import MFCCPreprocessingParameters
import numpy as np
from graph.types import RFFT2DPreprocessingParameters
from quantization.new_qrec import QRec
from quantization.qtype import QType
from quantization.unified_quantization_handler import params_type, in_qs_constraint, needs_stats

from ..mult_quantization_handler import MultQuantizionHandler

WINDOW_Q = 15
FFT_TWIDDLES_Q = 15
MFCC_COEFF_Q = 15
DCT_TWIDDLE_Q = 7

class DSPMultQuantHandler(MultQuantizionHandler):
    @classmethod
    def get_spectrogram_in_out_q(cls, in_q, params):
        win_q = QType.Pow2(bits=16, signed=True, q=WINDOW_Q)
        fft_twiddles_q = QType.Pow2(bits=16, signed=True, q=FFT_TWIDDLES_Q)
        rfft_twiddles_q = QType.Pow2(bits=16, signed=True, q=FFT_TWIDDLES_Q)
        swap_table_q = QType.Pow2(bits=16, signed=False, q=0)
        in_q = QType.Pow2(bits=16, signed=True, q=int(-np.ceil(np.log2(in_q.scale))), forced=True)
        if params.is_radix4():
            #in_q = QType.Pow2(bits=16, signed=True, q=12)
            fft_out_q = in_q.q - 2*(int(np.log2(params.n_cfft) / 2) - 2) - 1
        else:
            #in_q = QType.Pow2(bits=16, signed=True, q=13)
            fft_out_q = in_q.q - (int(np.log2(params.n_cfft)) - 3) - 1

        fft_out_q = QType.Pow2(bits=16, signed=True, q=fft_out_q)
        if not params.magsquared:
            out_q = QType.Pow2(bits=32, signed=False, q=15)
        else:
            out_q = QType.Pow2(bits=32, signed=False, q=15) #fft_out_q.q*2)
        return in_q, win_q, fft_twiddles_q, swap_table_q, rfft_twiddles_q, fft_out_q, out_q


@params_type(RFFT2DPreprocessingParameters)
@in_qs_constraint({'dtype': np.int16})
@needs_stats(False)
class RFFTPreprocessingMult(DSPMultQuantHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None

        in_q, win_q, fft_twiddles_q, swap_table_q, rfft_twiddles_q, fft_out_q, out_q = cls.get_spectrogram_in_out_q(in_qs[0], params)
        return QRec.symmetric(in_qs=[in_q, win_q, fft_twiddles_q, swap_table_q, rfft_twiddles_q], out_qs=[out_q], fft_out_q=fft_out_q)


@params_type(MFCCPreprocessingParameters)
@in_qs_constraint({'dtype': np.int16})
@needs_stats(False)
class MFCCPreprocessingMult(DSPMultQuantHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, _ = cls.get_mult_opts(**kwargs)
        force_out_q = force_out_qs and force_out_qs[0]
        if force_out_q:
            return None

        in_q, win_q, fft_twiddles_q, swap_table_q, rfft_twiddles_q, fft_out_q, spect_q = cls.get_spectrogram_in_out_q(in_qs[0], params)
        melcoeff_q = QType.Pow2(bits=16, signed=True, q=MFCC_COEFF_Q)
        mel_sparsity_table_q = QType.Pow2(bits=16, signed=False, q=0)
        dctmat_q = QType.Pow2(bits=16, signed=True, q=DCT_TWIDDLE_Q)
        if params.mel_type == "melspectrogram":
            out_q = QType.Pow2(bits=32, signed=True, q=16)
        elif params.mel_type == "logmelspectrogram":
            out_q = QType.Pow2(bits=16, signed=True, q=15-params.quant_norm)
        else:
            out_q = QType.Pow2(bits=16, signed=True, q=15-params.quant_norm-DCT_TWIDDLE_Q)

        return QRec.symmetric(
            in_qs=[in_q, win_q, fft_twiddles_q, swap_table_q, rfft_twiddles_q, mel_sparsity_table_q, melcoeff_q, dctmat_q],
            out_qs=[out_q],
            fft_out_q=fft_out_q
            )

