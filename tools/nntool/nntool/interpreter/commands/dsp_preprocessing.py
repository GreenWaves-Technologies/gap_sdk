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

from nntool.graph.types.base import NNEdge
import json
import logging

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from nntool.interpreter.nntool_shell_base import NNToolShellBase

from nntool.graph.dim import Dim
from nntool.graph.types import DSPNodeBase, InputNode, ReshapeNode
from nntool.graph.types.dsp_preprocessing import (MFCCPreprocessingNode,
                                           RFFT2DPreprocessingNode)

LOG = logging.getLogger("nntool")


class DSPPreprocessingCommand(NNToolShellBase):
    # GEN COMMAND
    parser_compile = Cmd2ArgumentParser()

    def inputs_choices(self):
        if self.G is None:
            return []
        return [node.name for node in self.G.inputs()]

    def dsp_types(self):
        return [clas.__name__ for clas in DSPNodeBase.__subclasses__()]

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
        config_dict = {}
        if args.config_json:
            with open(args.config_json) as json_file:
                config_dict = json.load(json_file)
            config_dict["frame_size"] = config_dict.get("frame_size", frame_size)
            config_dict["frame_step"] = config_dict.get("frame_step", frame_step)
            config_dict["n_fft"] = config_dict.get("n_fft", n_fft)
            config_dict["magsquared"] = config_dict.get("magsquared", magsquared)
            config_dict["window"] = config_dict.get("window", win_fn)
            config_dict["preemp_factor"] = config_dict.get("preemp_factor", preemp_factor)
            config_dict["n_frames"] = config_dict.get("n_frames", n_frames)
            config_dict["n_fbanks"] = config_dict.get("n_fbanks", n_fbanks)
        if args.dsp_node_type == "MFCCPreprocessingParameters":
            dsp_node_type = "mfcc"
            dsp_node_name = "MfccPreprocessing"
        elif args.dsp_node_type == "RFFT2DPreprocessingParameters":
            dsp_node_type = "fft"
            dsp_node_name = "RfftPreprocessing"
        else:
            raise ValueError(f"{args.dsp_node_type} dsp type not supported")

        input_node = self.G[args.input_node]
        self.G.insert_dsp_preprocessing(input_node, dsp_node_type, dsp_node_name, config_dict)
