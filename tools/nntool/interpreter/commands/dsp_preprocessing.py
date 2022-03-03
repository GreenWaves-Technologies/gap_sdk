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

from graph.types.base import NNEdge
from graph.types.others import ReshapeParameters
import json
import logging

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase

from graph.dim import Dim
from graph.types import DSPParameters, InputParameters
from graph.types.dsp_preprocessing import (MFCCPreprocessingParameters,
                                           RFFT2DPreprocessingParameters)

LOG = logging.getLogger("nntool")


class DSPPreprocessingCommand(NNToolShellBase):
    # GEN COMMAND
    parser_compile = Cmd2ArgumentParser()

    def inputs_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.inputs()]

    def dsp_types(self):
        return [clas.__name__ for clas in DSPParameters.__subclasses__()]

    parser_dsp = Cmd2ArgumentParser(
        "inserts dsp preprocessing node into graphs")
    parser_dsp.add_argument('input_node',
                            choices_method=inputs_choices,
                            help='input node name to format')
    parser_dsp.add_argument('dsp_node_type',
                            choices_method=dsp_types,
                            help='input node name to format')
    parser_dsp.add_argument('--config_json', completer_method=Cmd.path_complete, default=None,
                            help='path to the config file for mfcc')
    parser_dsp.add_argument('--n_fft', type=int,
                            help="n_fft bins")
    parser_dsp.add_argument('--n_frames', type=int,
                            help="number of frames")
    parser_dsp.add_argument('--n_fbanks', type=int,
                            help="number of filter banks")
    parser_dsp.add_argument('--frame_size', default=None, type=int,
                            help='frame size in samples')
    parser_dsp.add_argument('--frame_step', default=None, type=int,
                            help='frame size in samples')
    parser_dsp.add_argument('--preemp_factor', default=0.0, type=float,
                            help='preemp_factor')
    parser_dsp.add_argument('--magsquared', action="store_false",
                            help='preemp_factor')
    parser_dsp.add_argument('--window_fn', default="hanning", type=str,
                            help='numpy window function')

    @with_argparser(parser_dsp)
    def do_dsp_preprocessing(self, args):
        frame_size = args.frame_size
        frame_step = args.frame_step
        n_fft = args.n_fft
        magsquared = args.magsquared
        win_fn = args.window_fn
        preemp_factor = args.preemp_factor
        n_frames = args.n_frames
        n_fbanks = args.n_fbanks
        config_dict = None
        if args.config_json:
            with open(args.config_json) as json_file:
                config_dict = json.load(json_file)
            frame_size = config_dict.get("frame_size", frame_size)
            frame_step = config_dict.get("frame_step", frame_step)
            n_fft = config_dict.get("n_fft", n_fft)
            magsquared = config_dict.get("magsquared", magsquared)
            win_fn = config_dict.get("window_fn", win_fn)
            preemp_factor = config_dict.get("preemp_factor", preemp_factor)
            n_frames = config_dict.get("n_frames", n_frames)
            n_fbanks = config_dict.get("n_fbanks", n_fbanks)

        assert frame_step, "frame_step is required"
        assert n_fbanks and n_frames, "n_frames and n_fbanks are required"
        org_input_dim = self.G[args.input_node].out_dims[0]
        if org_input_dim.size() != (n_frames * n_fbanks):
            raise ValueError(
                f"Next layer has dimension {org_input_dim} (size: {org_input_dim.size()}) while you are trying to insert a DSP params with output size of {n_fbanks*n_frames} ({n_frames}x{n_fbanks})")
        LOG.info(f"N FRAMES: {n_frames}")
        new_input_size = frame_step * (n_frames - 1) + frame_size
        if args.dsp_node_type == "MFCCPreprocessingParameters":
            dsp_params = MFCCPreprocessingParameters(
                "MfccPreprocessing", conf_dict=config_dict)
            win_lut, fft_twiddles, swaptable, rfft_twiddles = dsp_params.gen_fft_twiddles()
            melfilt_coeff_sparse_node, melfilt_sparsity_node = dsp_params.gen_melfilter()
            dct_matrix_node = dsp_params.gen_dct_matrix()(
                self.G) if dsp_params.n_dct else None
            dsp_params(None, win_lut(self.G) if win_lut else win_lut, fft_twiddles(self.G), swaptable(self.G), rfft_twiddles(
                self.G), melfilt_sparsity_node(self.G), melfilt_coeff_sparse_node(self.G), dct_matrix_node)
        elif args.dsp_node_type == "RFFT2DPreprocessingParameters":
            dsp_params = RFFT2DPreprocessingParameters(
                "RfftPreprocessing", conf_dict=config_dict)
            win_lut, fft_twiddles, swaptable, rfft_twiddles = dsp_params.gen_fft_twiddles()
            dsp_params(None, win_lut(self.G) if win_lut else win_lut, fft_twiddles(
                self.G), swaptable(self.G), rfft_twiddles(self.G))

        new_input_node = InputParameters(
            args.input_node, dims=Dim.unnamed([new_input_size]))
        input_node_edge = self.G.out_edges(args.input_node)[0]
        input_node_edge.from_node.in_dims[0] = Dim.unnamed([new_input_size])
        self.G.insert_node_at_edge(dsp_params, input_node_edge)
        self.G.replace_node(self.G[args.input_node], new_input_node)
        dsp_out_dim = dsp_params.get_output_size([Dim.unnamed([new_input_size])])[0]
        if dsp_out_dim != org_input_dim:
            reshape = ReshapeParameters("reshape_dsp", old_shape=dsp_out_dim.shape, shape=org_input_dim.shape)
            self.G.insert_node_after(
                    dsp_params, reshape, from_idx=0, edge_class=NNEdge)
        self.G.add_dimensions()
