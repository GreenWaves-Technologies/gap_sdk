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
import os

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from generation.code_generator import CodeGenerator
from generation.default_template import (basic_kernel_header_template,
                                         basic_kernel_source_template,
                                         default_template, dynamic_template,
                                         header_template)
from generation.gen_utils import write_empty
from generation.naming_convension import DefaultNamingConvension
from interpreter.nntool_shell_base import NNToolShellBase, no_history
from utils.data_importer import import_data

LOG = logging.getLogger("nntool")


class GenCommand(NNToolShellBase):
    # GEN COMMAND
    parser_gen = Cmd2ArgumentParser()
    parser_gen.add_argument('model_file',
                            completer_method=Cmd.path_complete,
                            nargs=argparse.OPTIONAL,
                            help='file to write to, otherwise output to terminal')
    parser_gen.add_argument('-T', '--tensor_directory',
                            completer_method=Cmd.path_complete,
                            help='path to tensor directory. full path will be created' +
                            ' if it doesn\'t exist. If this parameter is given it will' +
                            'update the settings saved with the graph state.')
    parser_gen.add_argument('-M', '--model_directory',
                            completer_method=Cmd.path_complete,
                            help='path to model directory. full path will be created' +
                            ' if it doesn\'t exist. If this parameter is given it will' +
                            'update the settings saved with the graph state.')
    parser_gen.add_argument('-t', '--output_tensors',
                            action='store_true',
                            help='write constants (weights, biases)')
    parser_gen.add_argument('--anonymise',
                            action='store_true',
                            help='make variable and function names cryptic to hide function')
    parser_gen.add_argument('-c', '--checksums',
                            completer_method=Cmd.path_complete,
                            help='generate checksum tests in code for the given file')
    parser_gen.add_argument('--header_file',
                            completer_method=Cmd.path_complete,
                            help='generate header file with layers information')
    parser_gen.add_argument('--basic_kernel_source_file',
                            completer_method=Cmd.path_complete,
                            default="Expression_Kernels.c",
                            help='file to write to, otherwise output to terminal')
    parser_gen.add_argument('--basic_kernel_header_file',
                            completer_method=Cmd.path_complete,
                            default="Expression_Kernels.h",
                            help='file to write to, otherwise output to terminal')

    @with_argparser(parser_gen)
    @no_history
    def do_gen(self, args):
        """
Generate AutoTiler model C code and optionally dump tensors. If no destination file is
given the generated code will be outputed to the screen. Check the 'set' command for
settings related to code generation."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        if args.checksums:
            input_args = self._get_input_args(None)
            LOG.info("input file %s", args.checksums)
            data = import_data(args.checksums, **input_args)
            executer = GraphExecuter(self.G, qrecs=self.G.quantization)
            executer.execute([data], qmode=QuantizationMode.all())
            self.settings['checksum_file'] = args.checksums
            self.settings['generate_checksums'] = True

        if args.tensor_directory:
            self.settings['tensor_directory'] = args.tensor_directory
        if args.model_directory:
            self.settings['model_directory'] = args.model_directory
        self.settings['basic_kernel_source_file'] = args.basic_kernel_source_file
        self.settings['basic_kernel_header_file'] = args.basic_kernel_header_file
        self.settings['anonymise'] = args.anonymise
        os.makedirs(os.path.abspath(
            self.settings['model_directory']), mode=0o750, exist_ok=True)
        os.makedirs(os.path.abspath(
            self.settings['tensor_directory']), mode=0o750, exist_ok=True)
        code_gen = CodeGenerator(self.G, DefaultNamingConvension(anonymise=args.anonymise), self.settings)

        if self.settings['template_file']:
            code_template = dynamic_template(self.settings['template_file'])
        else:
            code_template = default_template

        if args.model_file:
            with open(os.path.join(self.settings['model_directory'],
                                   args.model_file), "w") as output_fp:
                output_fp.write(code_template(self.G, code_generator=code_gen))
            if self.G.has_expressions:
                with open(os.path.join(self.settings['model_directory'],
                                       args.basic_kernel_source_file), "w") as output_fp:
                    output_fp.write(basic_kernel_source_template(
                        self.G, code_generator=code_gen))
                with open(os.path.join(self.settings['model_directory'],
                                       args.basic_kernel_header_file), "w") as output_fp:
                    output_fp.write(basic_kernel_header_template(
                        self.G, code_generator=code_gen))
            else:
                write_empty(self.settings['model_directory'],
                            args.basic_kernel_source_file, "no expressions used")
                write_empty(self.settings['model_directory'],
                            args.basic_kernel_header_file, "no expressions used")
        else:
            self.ppaged(code_template(self.G, code_generator=code_gen))
            if self.G.has_expressions:
                self.ppaged(basic_kernel_source_template(
                    self.G, code_generator=code_gen))
                self.ppaged(basic_kernel_header_template(
                    self.G, code_generator=code_gen))
        if args.output_tensors:
            code_gen.write_constants()

        if args.header_file:
            with open(os.path.join(self.settings['model_directory'], args.header_file), "w") as output_fp:
                output_fp.write(header_template(
                    self.G, code_generator=code_gen))
