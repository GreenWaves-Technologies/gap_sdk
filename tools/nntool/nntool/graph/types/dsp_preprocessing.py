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
import math

import numpy as np
from nntool.graph.types import ConstantInputNode

from nntool.utils.gen_melfilterbank import GenMelFilterbank_Sparse
from nntool.utils.gen_twiddles import (gen_fft_swaptable_lut, gen_fft_twiddles_lut,
                                gen_rfft_twiddles_lut)

from ..dim import Dim
from .base import (NNNodeBase, SensitiveToOrderMixin, SingleInputAndOutputMixin,
                   cls_op_name, nargs)

LOG = logging.getLogger(__name__)


class DSPNodeBase(NNNodeBase, SingleInputAndOutputMixin, SensitiveToOrderMixin):

    def __init__(self, name: str, n_fft=None, frame_size=None, frame_step=None, preemp_factor=0.0,
                 magsquared=True, window="hanning", window_size=None, pad_type="right", n_frames=1,
                 **kwargs):
        super(DSPNodeBase, self).__init__(name, **kwargs)
        self._win_fn = window
        self._magsquared = magsquared
        self._preemp_factor = preemp_factor
        self._force_rad2 = False
        self._n_fft = n_fft
        self._n_cfft = n_fft // 2
        self._frame_size = frame_size if frame_size is not None else n_fft
        self._window_size = window_size if window_size is not None else self._frame_size
        self._frame_step = frame_step
        self._n_frames = n_frames
        self._pad_type = pad_type

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        if not self.magsquared:
            super_attrs['magquared'] = self.magsquared
        if self.win_fn != "hanning":
            super_attrs['window'] = self.win_fn
        if self.frame_size != self.n_fft:
            super_attrs['frame_size'] = self.frame_size
        if self.window_size != self.frame_size:
            super_attrs['window_size'] = self.window_size
        if self.pad_type != "right":
            super_attrs['pad_type'] = self.pad_type
        if self.n_frames != 1:
            super_attrs['n_frames'] = self.n_frames
        super_attrs.update({k: getattr(self, k) for k in [
            "n_fft", "frame_step", "preemp_factor", "magsquared"
        ] if getattr(self, k)})
        return super_attrs

    @property
    def preemp_factor(self):
        return self._preemp_factor

    @preemp_factor.setter
    def preemp_factor(self, val):
        self._preemp_factor = val

    @property
    def frame_size(self):
        return self._frame_size

    @frame_size.setter
    def frame_size(self, val):
        self._frame_size = val

    @property
    def window_size(self):
        return self._window_size

    @window_size.setter
    def window_size(self, val):
        self._window_size = val

    @property
    def frame_step(self):
        return self._frame_step

    @frame_step.setter
    def frame_step(self, val):
        self._frame_step = val

    @property
    def n_frames(self):
        return self._n_frames

    @n_frames.setter
    def n_frames(self, val):
        self._n_frames = val

    @property
    def n_fft(self):
        return self._n_fft

    @n_fft.setter
    def n_fft(self, val):
        self._n_fft = val

    @property
    def n_cfft(self):
        return self._n_cfft

    @n_cfft.setter
    def n_cfft(self, val):
        self._n_cfft = val

    @property
    def win_fn(self):
        return self._win_fn

    @win_fn.setter
    def win_fn(self, val):
        self._win_fn = val

    @property
    def magsquared(self):
        return self._magsquared

    @magsquared.setter
    def magsquared(self, val):
        self._magsquared = val

    @property
    def force_rad2(self):
        return self._force_rad2

    @force_rad2.setter
    def force_rad2(self, val):
        self._force_rad2 = val

    @property
    def pad_type(self):
        return self._pad_type

    @pad_type.setter
    def pad_type(self, val):
        self._pad_type = val

    def is_radix4(self):
        if self.force_rad2:
            return False
        return round(math.log(self.n_cfft, 4)) == math.log(self.n_cfft, 4) and self.n_cfft > 64

    def get_parameter_size(self):
        is_rad4 = self.is_radix4()
        n_fft_lut = int(self.n_cfft * 3 /
                        4) if is_rad4 else self.n_cfft // 2
        n_params = 2*n_fft_lut + self.n_cfft + 2*(self.n_fft//2)
        if self.win_fn:
            n_params += self.frame_size
        return n_params

    def get_output_size(self, _):
        return [Dim([self._n_frames, self.n_fft//2+1])]

    @property
    def can_equalize(self):
        return False

    def compute_load(self):
        return 0

    def __str__(self):
        return f"RFFT: NFFT={self.n_fft}, Win={self.win_fn}, Rad4={self.is_radix4()}"

    def gen_fft_twiddles(self):
        is_rad4 = self.is_radix4()
        n_fft_lut = int(self.n_cfft * 3 /
                        4) if is_rad4 else self.n_cfft // 2
        if self.win_fn:
            pad_before = (self.frame_size - self.window_size) // 2
            pad_after = (self.frame_size - self.window_size) - pad_before
            window_arr = np.pad(getattr(np, self.win_fn)(
                self.window_size), (pad_before, pad_after))
            win_lut = ConstantInputNode(
                self.name + "_WinLUT", value=window_arr, dims=Dim.unnamed([self.frame_size]))
        else:
            win_lut = None
        fft_twiddles = ConstantInputNode(self.name + "_FFT_Twiddles", value=gen_fft_twiddles_lut(
            self.n_cfft, rad4=is_rad4), dims=Dim.unnamed([2*n_fft_lut]))
        swaptable = ConstantInputNode(self.name + "_SwapTable", value=gen_fft_swaptable_lut(
            self.n_cfft, rad4=is_rad4), dims=Dim.unnamed([self.n_cfft]))
        rfft_twiddles = ConstantInputNode(self.name + "_RFFT_Twiddles", value=gen_rfft_twiddles_lut(
            self.n_fft), dims=Dim.unnamed([2*self.n_fft//2]))
        return win_lut, fft_twiddles, swaptable, rfft_twiddles


@cls_op_name("RFFT2D")
@nargs({4, 5})
class RFFT2DPreprocessingNode(DSPNodeBase):
    def __init__(self, name: str, n_frames=None, n_fft=None, frame_size=None, frame_step=None, preemp_factor=0.0, magsquared=True, window="hanning", window_size=None, pad_type="right", **kwargs):
        super().__init__(name, n_frames=n_frames, n_fft=n_fft, frame_size=frame_size, frame_step=frame_step,
                         preemp_factor=preemp_factor, magsquared=magsquared,
                         window=window, window_size=window_size, pad_type=pad_type, **kwargs)


@cls_op_name("MFCC")
@nargs({6, 7, 8})
class MFCCPreprocessingNode(DSPNodeBase):

    def __init__(self, name: str, n_frames=None, frame_size=None, frame_step=None, n_fft=None, window="hanning",
                 magsquared=True, preemp_factor=0.0, sample_rate=16000, fmin=None, fmax=None, log_type="natural",
                 n_fbanks=None, librosa_norm="slaney", mel_filterbanks_file=None, fbank_type=None, quant_norm=5,
                 log_offset=0, n_dct=None, dct_type=2, dct_norm=None, window_size=None, pad_type="right", **kwargs):
        super(MFCCPreprocessingNode, self).__init__(
            name, n_fft=n_fft, n_frames=n_frames, frame_size=frame_size, frame_step=frame_step,
            preemp_factor=preemp_factor, magsquared=magsquared, window=window,
            window_size=window_size, pad_type=pad_type, **kwargs)
        self._sample_rate = sample_rate
        self._librosa_norm = librosa_norm
        self._mel_filterbanks_file = mel_filterbanks_file
        self._fbank_type = fbank_type
        self._n_fbanks = n_fbanks
        self._fmin = fmin
        self._fmax = fmax
        self._log_type = log_type
        self._log_offset = log_offset
        self._quant_norm = quant_norm
        self._n_dct = n_dct
        self._dct_type = dct_type
        self._dct_norm = dct_norm

    @property
    def attrs(self) -> dict:
        super_attrs = super().attrs
        super_attrs.update({k: getattr(self, k) for k in [
            "sample_rate", "librosa_norm", "mel_filterbanks_file", "fbank_type",
            "n_fbanks", "fmin", "fmax", "log_type", "log_offset", "quant_norm",
            "n_dct", "dct_type", "dct_norm"
        ] if getattr(self, k)})
        return super_attrs

    @property
    def sample_rate(self):
        return self._sample_rate

    @sample_rate.setter
    def sample_rate(self, val):
        self._sample_rate = val

    @property
    def fmin(self):
        return self._fmin

    @fmin.setter
    def fmin(self, val):
        self._fmin = val

    @property
    def fmax(self):
        return self._fmax

    @fmax.setter
    def fmax(self, val):
        self._fmax = val

    @property
    def fbank_type(self):
        return self._fbank_type

    @fbank_type.setter
    def fbank_type(self, val):
        self._fbank_type = val

    @property
    def librosa_norm(self):
        return self._librosa_norm

    @librosa_norm.setter
    def librosa_norm(self, val):
        self._librosa_norm = val

    @property
    def mel_filterbanks_file(self):
        return self._mel_filterbanks_file

    @mel_filterbanks_file.setter
    def mel_filterbanks_file(self, val):
        self._mel_filterbanks_file = val

    @property
    def n_fbanks(self):
        return self._n_fbanks

    @n_fbanks.setter
    def n_fbanks(self, val):
        self._n_fbanks = val

    @property
    def log_type(self):
        return self._log_type

    @log_type.setter
    def log_type(self, val):
        self._log_type = val

    @property
    def log_offset(self):
        return self._log_offset

    @log_offset.setter
    def log_offset(self, val):
        self._log_offset = val

    @property
    def quant_norm(self):
        return self._quant_norm

    @quant_norm.setter
    def quant_norm(self, val):
        self._quant_norm = val

    @property
    def n_dct(self):
        return self._n_dct

    @n_dct.setter
    def n_dct(self, val):
        self._n_dct = val

    @property
    def dct_type(self):
        return self._dct_type

    @dct_type.setter
    def dct_type(self, val):
        self._dct_type = val

    @property
    def dct_norm(self):
        return self._dct_norm

    @dct_norm.setter
    def dct_norm(self, val):
        self._dct_norm = val

    @property
    def mel_type(self):
        if self.n_dct == 0:
            if self.log_type:
                return "logmelspectrogram"
            return "melspectrogram"
        return "mfcc"

    def get_parameter_size(self):
        is_rad4 = self.is_radix4()
        n_fft_lut = int(self.n_cfft * 3 /
                        4) if is_rad4 else self.n_cfft // 2
        n_params = 2*n_fft_lut + self.n_cfft + 2*(self.n_fft//2)
        if self.win_fn:
            n_params += self.frame_size
        n_params += sum(self.get_melfilter_size())
        n_params += self.n_dct * self.n_dct
        return n_params

    def get_output_size(self, _):
        if self._n_dct:
            return [Dim.unnamed([self._n_frames, self._n_dct])]
        return [Dim.unnamed([self._n_frames, self._n_fbanks])]

    def gen_melfilter(self):
        if self.fbank_type == "tensorflow":
            from nntool.utils.gen_melfilterbank import GenMelFilters_FB_tf
            filters = GenMelFilters_FB_tf(
                self.n_fft, self.n_fbanks, Fmin=self.fmin, Fmax=self.fmax, sample_rate=self.sample_rate)
        elif self.fbank_type == "librosa":
            from nntool.utils.gen_melfilterbank import GenMelFilters_FB_librosa
            filters = GenMelFilters_FB_librosa(
                self.n_fft, self.n_fbanks, Fmin=self.fmin, Fmax=self.fmax, sample_rate=self.sample_rate, norm=self.librosa_norm)
        elif self.fbank_type == "custom":
            filters = np.load(self.mel_filterbanks_file)
        else:
            from nntool.utils.gen_melfilterbank import GenMelFilters_FB
            filters = GenMelFilters_FB(
                self.n_fft, self.n_fbanks, Fmin=self.fmin, Fmax=self.fmax, sample_rate=self.sample_rate)
        melfilt_coeff_sparse, melfilt_sparsity = GenMelFilterbank_Sparse(
            filters)
        ncoeff = len(melfilt_coeff_sparse)
        melfilt_coeff_sparse_node = ConstantInputNode(
            self.name + "_MelFiltCoeff", value=melfilt_coeff_sparse, dims=Dim.unnamed([ncoeff]))
        melfilt_sparsity_node = ConstantInputNode(
            self.name + "_MelFiltSparsityMat", value=melfilt_sparsity, dims=Dim.unnamed([self.n_fbanks, 3]))
        return melfilt_coeff_sparse_node, melfilt_sparsity_node

    def get_melfilter_size(self):
        melfilt_coeff, melfilt_sparsity = self.gen_melfilter()
        return melfilt_coeff.dqvalue.size, melfilt_sparsity.dqvalue.size

    def gen_dct_matrix(self):
        norm_factor = np.ones((self.n_dct, self.n_dct))
        if self.dct_norm == "ortho" and self.dct_type == 2:
            norm_factor *= np.sqrt(1/(2*self.n_dct))
            norm_factor[0] = np.sqrt(1/(4*self.n_dct))
        if self.dct_norm == "ortho" and self.dct_type == 3:
            norm_factor *= np.sqrt(1/(2*self.n_dct))
            norm_factor[0] = np.sqrt(1/(self.n_dct))
        DCT_Coeff = np.zeros((self.n_dct, self.n_dct))
        for k in range(self.n_dct):
            for i in range(self.n_dct):
                if self.dct_type == 2:
                    coeff = 2*np.cos(np.pi / (2*self.n_dct) * k * (2*i + 1))
                elif self.dct_type == 3:
                    coeff = 1 if i == 0 else 2 * \
                        np.cos(np.pi / (2*self.n_dct) * i * (2*k + 1))
                else:
                    raise NotImplementedError(
                        "DCT type 2 and 3 only supported")
            DCT_Coeff[k, i] = coeff
        return ConstantInputNode(self.name + "_DCT_Matrix", value=DCT_Coeff*norm_factor, dims=Dim.unnamed([self.n_dct, self.n_dct]))

    def __str__(self):
        return f"{self.mel_type.capitalize()}: NFFT={self.n_fft}, Win={self.win_fn}, Rad4={self.is_radix4()}, MagSquared={self.magsquared}, Log={self.log_type}, NDCT={self.n_dct}"
