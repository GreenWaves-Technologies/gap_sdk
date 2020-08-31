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

import argparse
import logging
import pickle

import numpy as np
from PIL import Image, ImageDraw
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from graph.types import ConvFusionParameters, FilterParameters, SSDDetectorParameters
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import (glob_input_files,
                                     input_options)
from utils.data_importer import import_data
from utils.node_id import NodeId
from utils.at_norm import set_do_rounding, get_do_rounding

LOG = logging.getLogger('nntool.'+__name__)


def format_dump_file(G, outputs, quantized, dequantize, quantize_step):
    # simplify the output since we only have one for now and add weights
    foutputs = []
    for idx, out in enumerate(outputs):
        if quantize_step == idx:
            step_is_quantized = True
            step_is_dequantized = True
        elif quantized:
            step_is_quantized = True
            step_is_dequantized = dequantize
        else:
            step_is_quantized = False
            step_is_dequantized = False

        tensors = [out[0]]
        node = G.graph_state.steps[idx]['node']
        if isinstance(node, ConvFusionParameters):
            for filt in node.contained_filters():
                if step_is_quantized:
                    qrec = G.quantization[NodeId(node, filt)]
                    if G.has_quantized_parameters:
                        if step_is_dequantized:
                            qrec = G.quantization[NodeId(node, filt)]
                            tensors.append(qrec.weights_q.get_dequantized(filt.weights))
                            tensors.append(qrec.biases_q.get_dequantized(filt.biases))
                        else:
                            tensors.append(np.copy(filt.weights))
                            tensors.append(qrec.biases_q.get_quantized(filt.biases))
                    else:
                        if step_is_dequantized:
                            tensors.append(np.copy(filt.weights))
                            tensors.append(np.copy(filt.biases))
                        else:
                            tensors.append(qrec.weights_q.quantize(filt.weights))
                            tensors.append(qrec.biases_q.quantize(filt.biases))
                else:
                    if G.has_quantized_parameters:
                        qrec = G.quantization[NodeId(node, filt)]
                        tensors.append(qrec.weights_q.get_dequantized(filt.weights))
                        tensors.append(qrec.biases_q.get_dequantized(filt.biases))
                    else:
                        tensors.append(np.copy(filt.weights))
                        tensors.append(np.copy(filt.biases))
        elif isinstance(node, FilterParameters):
            if step_is_quantized:
                qrec = G.quantization[NodeId(node, None)]
                if G.has_quantized_parameters:
                    if step_is_dequantized:
                        tensors.append(qrec.weights_q.get_dequantized(node.weights))
                        tensors.append(qrec.biases_q.get_dequantized(node.biases))
                    else:
                        tensors.append(np.copy(node.weights))
                        tensors.append(qrec.biases_q.get_quantized(node.biases))
                else:
                    if step_is_dequantized:
                        tensors.append(np.copy(node.weights))
                        tensors.append(np.copy(node.biases))
                    else:
                        tensors.append(qrec.weights_q.quantize(node.weights))
                        tensors.append(qrec.biases_q.quantize(node.biases))
            else:
                if G.has_quantized_parameters:
                    qrec = G.quantization[NodeId(node, None)]
                    tensors.append(qrec.weights_q.dequantize(node.weights))
                    tensors.append(qrec.biases_q.dequantize(node.biases))
                else:
                    tensors.append(np.copy(node.weights))
                    tensors.append(np.copy(node.biases))
        else:
            tensors.append(None)
            tensors.append(None)
        foutputs.append(tuple(tensors))
    return foutputs


class DumpCommand(NNToolShellBase):
    # DUMP COMMAND
    parser_dump = Cmd2ArgumentParser()
    parser_dump.add_argument('-s', '--step',
                             type=int, help='step to dump output of', default=None)
    parser_dump.add_argument('-w', '--number_width',
                             type=int, help='width of numbers', default=8)
    parser_dump.add_argument('-p', '--precision',
                             type=int, help='number of decimal places', default=4)
    parser_dump.add_argument('-c', '--channel',
                             type=int, help='channel to dump', default=None)
    parser_dump.add_argument('-d', '--dequantize',
                             action='store_true', help='dequantize result')
    parser_dump.add_argument('--quantize_and_dequantize',
                             action='store_true', help='quantize and dequantize float results')
    parser_dump_group = parser_dump.add_mutually_exclusive_group(required=False)
    parser_dump_group.add_argument('-q', '--quantize', action='store_true',
                                   help='quantize the graph (must have already set quantization)')
    parser_dump_group.add_argument('-Q', '--quantize_step', type=int,
                                   help='quantize a step of the graph (must have already' +
                                   ' set quantization)',
                                   default=None)
    parser_dump_group.add_argument('-A', '--quantize_all_steps',
                                   action='store_true',
                                   help='quantize all steps of the graph feeding' +
                                   ' unquantized float data into each step')
    parser_dump.add_argument('-P', '--pickle',
                             completer_method=Cmd.path_complete,
                             help='pickle all the outputed tensors to this file')
    parser_dump.add_argument('-S', '--save',
                             help='save the tensor to the tensors list')
    parser_dump.add_argument('-v', '--visualize_detection',
                             action='store_true', help='visualize input images and detection predictions')
    input_options(parser_dump)

    @with_argparser(parser_dump)
    def do_dump(self, args: argparse.Namespace):
        """
Dump the activations resulting from running an input file through the graph.
You can use the current quantization settings and can also just quantify one
specific step of the graph."""
        self._check_graph()
        dequantize = args.dequantize if args.dequantize is not None\
            else not (args.pickle or args.save)
        if args.quantize or args.quantize_step or args.quantize_all_steps:
            self._check_quantized()
            if args.quantize:
                if dequantize:
                    qmode = QuantizationMode.all_dequantize()
                else:
                    qmode = QuantizationMode.all()
            elif args.quantize_all_steps:
                qmode = QuantizationMode.step_all()
                dequantize = True
            else:
                qmode = QuantizationMode.step(args.quantize_step)
        elif args.quantize_and_dequantize:
            qmode = QuantizationMode.all_float_quantize_dequantize()
        else:
            qmode = QuantizationMode.none()
        if args.step is not None:
            step = args.step
            num_steps = len(self.G.graph_state.steps)
            if step < 0:
                step = num_steps + step
            if step < 0 or step > num_steps:
                self.perror("step must be from {} to {}".format(-num_steps, num_steps))
                return
        else:
            step = None

        input_args = self._get_input_args(args)

        pickles = []

        for file_per_input in glob_input_files(args.input_files, self.G.num_inputs):
            LOG.info("input file %s", file_per_input)
            data = [import_data(input_file, **input_args) for input_file in file_per_input]
            executer = GraphExecuter(self.G, qrecs=self.G.quantization)
            outputs = executer.execute(data, step_idx_limit=step,
                                       qmode=qmode)

            if args.pickle or self._in_py or args.save:
                pickles.append(format_dump_file(self.G, outputs, not qmode.is_none,
                                                args.dequantize, args.quantize_step))
            else:
                self.G.print_intermediates(outputs, limit=step, width=args.number_width,
                                           precision=args.precision, channel=args.channel,
                                           order=['c', 'h', 'w'])

            if args.visualize_detection:
                img_in = Image.open(file_per_input[0]).convert('RGBA')

                bboxes, classes, scores, _ = [outputs[graph_out.step_idx][0] for graph_out in self.G.outputs()]
                height = img_in.size[1] if input_args['height'] == -1 else input_args['height']
                width = img_in.size[0] if input_args['width'] == -1 else input_args['width']

                img_in = img_in.resize((width, height))
                draw = ImageDraw.Draw(img_in, 'RGBA')

                for box, score, class_id in zip(bboxes, scores, classes):
                    if args.quantize and not args.dequantize:
                        ssd_node = [node for node in self.G.nodes() if isinstance(node, SSDDetectorParameters)][0]
                        ssd_qrec = self.G.quantization[NodeId(ssd_node)]
                        x0, x1 = int(box[1] * width * ssd_qrec.out_qs[0].scale), int(box[3] * width * ssd_qrec.out_qs[0].scale)
                        y0, y1 = int(box[0] * height * ssd_qrec.out_qs[0].scale), int(box[2] * height * ssd_qrec.out_qs[0].scale)
                        score = score * ssd_qrec.out_qs[2].scale
                    else:
                        x0, x1 = int(box[1] * width), int(box[3] * width)
                        y0, y1 = int(box[0] * height), int(box[2] * height)
                    rect_points = (x0, y0), (x1, y0), (x1, y1), (x0, y1), (x0, y0)
                    draw.line(rect_points, fill='red', width=2)
                    txt = '{}@{}%'.format(class_id, int(score*100))
                    draw.text([x0, y0-10], txt, fill=(0, 255, 0))
                img_in.show()

        if args.pickle or args.save or self._in_py:
            if not pickles:
                self.perror("no input files found")
                return
            if len(args.input_files) == 1:
                pickles = pickles[0]
            if args.pickle:
                with open(args.pickle, 'wb') as pickle_fp:
                    pickle.dump(pickles, pickle_fp)
            if args.save:
                self.tensor_store[args.save] = pickles

        if self._in_py:
            self.last_result = pickles


class RoundingCommand(NNToolShellBase):
    # ROUNDING COMMAND
    parser_round = Cmd2ArgumentParser()
    parser_round.add_argument('round',
                              choices=['on', 'off'],
                              nargs=(0, 1),
                              help='switch rounding on or off')

    @with_argparser(parser_round)
    def do_rounding(self, args: argparse.Namespace):
        """
Switch rounding on and off in quantized calculations."""
        if args.round is not None:
            set_do_rounding(args.round == 'on')
        LOG.info("rounding is %s", 'on' if get_do_rounding() else 'off')
