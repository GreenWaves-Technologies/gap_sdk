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
import os
import sys

from cmd2 import ansi

from nntool.generation.code_generator import CodeGenerator
from nntool.generation.default_template import (basic_kernel_header_template,
                                         basic_kernel_source_template,
                                         default_template, dynamic_template,
                                         header_template)
from nntool.generation.gen_utils import write_empty
from nntool.generation.naming_convension import DefaultNamingConvension
from nntool.interpreter.nntool_shell import NNToolShell

LOG = logging.getLogger("nntool")


class GeneratorShellLogHandler(logging.Handler):
    def emit(self, record: logging.LogRecord):
        output = self.format(record)
        if record.levelno >= logging.ERROR:
            print(ansi.style_error(output))
        elif record.levelno >= logging.WARN:
            print(ansi.style_warning(output))
        else:
            print(ansi.style_success(output))


def write_template(G, code_gen, model_directory, model_file, template, template_name):
    model_path = os.path.join(model_directory, model_file)
    LOG.info("Saving %s to %s", template_name, model_path)
    model = template(G, code_generator=code_gen)
    if not model:
        LOG.error("error generating %s code", template_name)
        sys.exit(1)
    with open(model_path, "w") as output_fp:
        output_fp.write(model)

def generate_code(args):
    LOG.propagate = False

    nntool_shell = NNToolShell(args,
                               allow_cli_args=False)

    # handler = GeneratorShellLogHandler()
    # formatter = logging.Formatter('%(module)s - %(message)s')
    # handler.setFormatter(formatter)
    # LOG.addHandler(handler)
    # if args.log_level:
    #     LOG.setLevel(args.log_level.upper())
    # else:
    #     LOG.setLevel('WARN')

    LOG.info("Loading %s", args.graph_file)

    nntool_shell.load_state_file(args.graph_file)
    G = nntool_shell.G
    opts = nntool_shell.settings

    if args.model_file:
        opts['model_file'] = args.model_file
    if args.model_directory:
        opts['model_directory'] = args.model_directory
    if args.tensor_directory:
        opts['tensor_directory'] = args.tensor_directory
    opts['basic_kernel_header_file'] = args.basic_kernel_header_file
    opts['basic_kernel_source_file'] = args.basic_kernel_source_file

    os.makedirs(os.path.abspath(opts['model_directory']), mode=0o750, exist_ok=True)
    os.makedirs(os.path.abspath(opts['tensor_directory']), mode=0o750, exist_ok=True)

    code_gen = CodeGenerator(G, DefaultNamingConvension(anonymise=opts.get('anonymise')), opts)
    if args.template_file:
        code_template = dynamic_template(args.template_file)
    else:
        code_template = default_template
    write_template(G, code_gen, opts['model_directory'], opts['model_file'], code_template, "model")
    if G.has_expressions:
        write_template(G, code_gen, opts['model_directory'],
                       opts['basic_kernel_header_file'], basic_kernel_header_template, "kernel headers")
        write_template(G, code_gen, opts['model_directory'],
                       opts['basic_kernel_source_file'], basic_kernel_source_template, "kernel source")
    else:
        write_empty(opts['model_directory'], opts['basic_kernel_header_file'], "no expressions used")
        write_empty(opts['model_directory'], opts['basic_kernel_source_file'], "no expressions used")

    if args.header_file:
        with open(os.path.join(opts['model_directory'], args.header_file), "w") as output_fp:
            output_fp.write(header_template(G, code_generator=code_gen))
    if not args.dont_dump_tensors:
        LOG.info("Writing constants to %s", opts['model_directory'])
        code_gen.write_constants()
