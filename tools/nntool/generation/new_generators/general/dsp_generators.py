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

import numpy as np
from graph.types.dsp_preprocessing import MFCCPreprocessingParameters
import logging

from generation.at_types.gen_ctrl import GenCtrl
from generation.bindings import (CommentBindingList, GNodeArgEdge,
                                 GNodeArgNode, Imm, NodeBindingList)
from generation.generators.kernels.autotiler_kernel import NewAutoTilerKernel
from generation.new_generators.generator_base import GeneratorBase, paramstype
from generation.new_generators.helpers.in_out_bindings_mixin import \
    InOutBindingsMixin
from graph.types import RFFT2DPreprocessingParameters
from bfloat16 import bfloat16

LOG = logging.getLogger("nntool." + __name__)

DSP_DTYPE = {
    np.int16: "FIX16",
    np.int32: "FIX16",
    np.uint32: "FIX16",
    bfloat16: "FLOAT16",
    np.float16: "FLOAT16",
    np.float32: "FLOAT32",
}

@paramstype(RFFT2DPreprocessingParameters)
class GenRfftParameters(GeneratorBase):
    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList(
                f"Node {cname} input_q {qrec.in_qs[0].q} window_q {qrec.in_qs[1].q} fft_twiddles_q {qrec.in_qs[2].q} rfft_twiddles {qrec.in_qs[4].q} -> out_q {qrec.out_qs[0]}")
        )
        binding_list = [
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(in_eparams[4]),
            GNodeArgEdge(in_eparams[3])
        ]
        if node.win_fn is not None:
            binding_list.append(GNodeArgEdge(in_eparams[1]))

        gen.bindings.append(NodeBindingList(cname, *binding_list))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        gen.kernels.append(RFFTKernel(cname, node, qrec))
        return True


class RFFTKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}: N_FRAMES={n_frames}, FRAME_SIZE={frame_size}, FRAME_STEP={frame_stride}, N_FFT={n_fft}, PREEMP_FACTOR={preemp_factor}, SKIP_PREEMP={skip_preemp}, NO_WIN={no_window}, OUT_FFT={out_fft}, SPECT_MAGSQUARED={magsquared}, DTYPE={data_type}
RFFT_2D_Generator("{cname}", {gen_ctrl}, {n_frames}, {frame_size}, {frame_stride}, {n_fft}, {preemp_factor}, {skip_preemp}, {no_window}, {out_fft}, {magsquared}, {data_type});
'''

    def __init__(self, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if qrec.out_qs[0].is_floating:
            gen_ctrl.float_dump = 1

        attrs = {
            'n_frames': params.n_frames,
            'frame_size': params.frame_size,
            'frame_stride': params.frame_step,
            'n_fft': params.n_fft,
            'preemp_factor': params.preemp_factor,
            'skip_preemp': 0,
            'no_window': int(params.win_fn is None),
            'out_fft': 0,
            'magsquared': int(params.magsquared),
            'data_type': DSP_DTYPE[qrec.out_qs[0].dtype],
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)


@paramstype(MFCCPreprocessingParameters)
class GenMfccParameters(GeneratorBase):
    @classmethod
    def globals_generator(cls, gen, node, qrec, pnode, fnode) -> bool:
        return True

    @classmethod
    def bindings_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        gen.bindings.append(
            CommentBindingList(
                f"Node {cname} input_q {qrec.in_qs[0].q} window_q {qrec.in_qs[1].q} fft_twiddles_q {qrec.in_qs[2].q} rfft_twiddles {qrec.in_qs[4].q} -> out_q {qrec.out_qs[0]}")
        )
        binding_list = [
            GNodeArgEdge(in_eparams[0]),
            GNodeArgEdge(out_eparams[0], "GNA_OUT"),
            GNodeArgEdge(in_eparams[2]),
            GNodeArgEdge(in_eparams[4]),
            GNodeArgEdge(in_eparams[3]),
            GNodeArgEdge(in_eparams[5]),
            GNodeArgEdge(in_eparams[6])
        ]
        if node.win_fn is not None:
            binding_list.insert(5, GNodeArgEdge(in_eparams[1]))
        if not qrec.out_qs[0].dtype in [np.float16, np.float32, bfloat16]:
            binding_list.append(Imm(node.quant_norm))
        if node.n_dct:
            binding_list.append(GNodeArgEdge(in_eparams[7]))

        gen.bindings.append(NodeBindingList(cname, *binding_list))
        return True

    @classmethod
    def kernel_generator(cls, gen, node, qrec, in_eparams, out_eparams, cname) -> bool:
        del in_eparams, out_eparams
        gen.kernels.append(MFCCKernel(cname, node, qrec))
        return True


class MFCCKernel(NewAutoTilerKernel):
    CALL_TEMPLATE = '''
// generator for {node_name}: N_FRAMES={n_frames}, FRAME_SIZE={frame_size}, FRAME_STEP={frame_stride}, N_FFT={n_fft}, N_MELBANKS={n_melbanks}, SIZE_MEL_COEFF={size_mel_coeff}, N_DCT={n_dct}, PREEMP_FACTOR={preemp_factor}, NO_WIN={no_window}, LIFTER_COEFF={lift_coeff}, SPECT_MAGSQUARED={magsquared}, DTYPE={data_type}, LOG_TYPE={log_type}, OUT_FFT={out_fft}
MFCC_Generator("{cname}", {gen_ctrl}, {n_frames}, {frame_size}, {frame_stride}, {n_fft}, {n_melbanks}, {size_mel_coeff}, {n_dct}, {preemp_factor}, {no_window}, {lift_coeff}, {magsquared}, {data_type}, {log_type}, {out_fft});
'''

    def __init__(self, cname, params, qrec, gen_ctrl=None):
        if gen_ctrl is None:
            gen_ctrl = GenCtrl(None, cname=cname)
        else:
            gen_ctrl.cname = cname

        if qrec.out_qs[0].is_floating:
            gen_ctrl.float_dump = 1

        if params.log_type is not None and params.log_offset:
            gen_ctrl.mfcc_log_offset = int(np.round(params.log_offset * 2**(30)))

        attrs = {
            'n_frames': params.n_frames,
            'frame_size': params.frame_size,
            'frame_stride': params.frame_step,
            'n_fft': params.n_fft,
            'n_melbanks': params.n_fbanks,
            'size_mel_coeff': params.get_melfilter_size()[1],
            'n_dct': params.n_dct,
            'preemp_factor': params.preemp_factor,
            'no_window': int(params.win_fn is None),
            'lift_coeff': 0,
            'magsquared': int(params.magsquared),
            'data_type': DSP_DTYPE[qrec.out_qs[0].dtype],
            'log_type': 0 if not params.log_type else (2 if params.log_type == "db" else 1),
            'out_fft': 0,
        }

        # other attributes
        extra_attrs = {
            'cname': cname,
            'node_name': params.name
        }

        super().__init__(attrs, extra_attrs, gen_ctrl=gen_ctrl)
