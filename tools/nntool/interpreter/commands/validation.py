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

import numpy as np
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser

from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from execution.execution_progress import ExecutionProgress
from interpreter.nntool_shell_base import NNToolShellBase
from interpreter.shell_utils import (glob_input_files,
                                     input_options)
from utils.data_importer import import_data
from utils.validation_utils import ValidateFromJSON, ValidateFromName, ValidateFromClass, ValidateFromVWWInstances

LOG = logging.getLogger('nntool.'+__name__)


class ValidationCommand(NNToolShellBase):
    # VAL COMMAND
    parser_val = Cmd2ArgumentParser()
    parser_val.add_argument('-q', '--quantize', action='store_true',
                            help='quantize the graph (must have already set quantization)')
    parser_val.add_argument('-s', '--silent', action='store_true',
                            help='do not print progress for each input')
    parser_val.add_argument('--dataset_dir',
                            completer_method=Cmd.path_complete,
                            help='path to the directory of samples for test')
    parser_val_group = parser_val.add_mutually_exclusive_group(required=False)
    parser_val_group.add_argument('--label_json',
                                  default=None,
                                  completer_method=Cmd.path_complete,
                                  help='path to the .json object containing labels annotation \
                                        { "filename0" : label0, "filename1": label1, ... }')
    parser_val_group.add_argument('--class_number',
                                  default=None,
                                  type=int,
                                  help='Number of a single class that all should match')
    parser_val.add_argument('--progress_every',
                            default=100,
                            type=int,
                            help='print accuracy every n computed predictions')
    parser_val_group.add_argument('--vww_instances_file',
                                  default=None,
                                  completer_method=Cmd.path_complete,
                                  help='path to the .json object containing labels instances\
                                        with the visualwakewords format:\
                                        instances = {images, annotations, categories}\
                                        instances["images"] = { file_name:.., image_id:.. }\
                                        instances["annotations"] = { image_id:.., label:..}')
    input_options(parser_val)

    @with_argparser(parser_val)
    def do_validate(self, args: argparse.Namespace):
        """
Validate the model (quantized [-q] or not) in terms of prediction accuracy rate on a given dataset (images
folder). Ground truth labels can be embedded in files names ("filename_03.[png, ppm, pgm]", the number of
digits must be coherent with the number of networks outputs: e.g. in a 1000 classes problem the last digits
must be 3, "file_45.png" will raise an error) or can be written in a .json object (example: {'file0':label0,
'file1':label1, ...}) and given to the function with --label_json
"""
        self._check_graph()
        if args.quantize:
            self._check_quantized()
            qmode = QuantizationMode.all_dequantize()
        else:
            qmode = QuantizationMode.none()

        LOG.info("quantization mode - %s", qmode)
        input_args = self._get_input_args(args)

        good_predictions = []
        good_margin = 0
        bad_margin = 0

        number_samples = sum(1 for _ in glob_input_files(args.input_files))

        if args.vww_instances_file:
            validation = ValidateFromVWWInstances(args.vww_instances_file)
        elif args.label_json:
            validation = ValidateFromJSON(args.label_json)
        elif args.class_number is not None:
            validation = ValidateFromClass(args.class_number)
        else:
            validation = ValidateFromName()

        try:
            ExecutionProgress.start()
            for i, file_per_input in enumerate(glob_input_files(args.input_files, self.G.num_inputs)):
                if not args.silent:
                    LOG.info("input file %s", file_per_input)            
                data = [import_data(input_file, **input_args) for input_file in file_per_input]

                executer = GraphExecuter(self.G, qrecs=self.G.quantization)
                outputs = executer.execute(data, qmode=qmode, silent=args.silent)

                good_prediction, class_predicted, real_class, margin = validation.validate(
                    file_per_input[0], np.asarray(outputs[-1]))
                good_predictions.append(good_prediction)
                if good_prediction:
                    good_margin += margin
                else:
                    bad_margin += margin

                if not args.silent:
                    LOG.info('Prediction is %s predicted %s correct %s margin %s',
                             good_prediction, class_predicted, real_class, margin)
                if not i % args.progress_every and i > 0:
                    LOG.info('ACCURACY: %.3f %%', 100*sum(good_predictions)/len(good_predictions))

                ExecutionProgress.progress(i, number_samples)
            ExecutionProgress.end()

        except (KeyboardInterrupt, SystemExit):
            pass

        self.py_locals['labels'] = validation.labels
        self.py_locals['predictions'] = validation.predictions
        cnt = len(good_predictions)
        if cnt:
            ngood = sum(good_predictions)
            nbad = cnt - ngood
            if nbad:
                LOG.info("%s out of %s predicted falsly with %s average margin",
                         nbad, cnt, bad_margin / nbad)
            if ngood:
                LOG.info("%s out of %s predicted correctly with %s average margin",
                         ngood, cnt, good_margin / ngood)
            accuracy_rate = 100*sum(good_predictions)/len(good_predictions)
            LOG.info('Total accuracy: %.3f %%', accuracy_rate)
