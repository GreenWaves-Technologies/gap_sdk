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
import os, sys

from cmd2 import ansi

from generation.code_generator import CodeGenerator
from generation.naming_convension import DefaultNamingConvension
from generation.default_template import default_template, dynamic_template, header_template

from utils.new_param_state import load_state

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

def generate_code(args):
    LOG.propagate = False
    handler = GeneratorShellLogHandler()
    formatter = logging.Formatter('%(module)s - %(message)s')
    handler.setFormatter(formatter)
    LOG.addHandler(handler)
    if args.log_level:
        LOG.setLevel(args.log_level.upper())
    else:
        LOG.setLevel('WARN')

    LOG.info("Loading %s", args.graph_file)
    G, opts = load_state(args.graph_file, return_extra=True)
    if args.model_file:
        opts['model_file'] = args.model_file
    if args.model_directory:
        opts['model_directory'] = args.model_directory
    if args.tensor_directory:
        opts['tensor_directory'] = args.tensor_directory

    os.makedirs(os.path.abspath(opts['model_directory']), mode=0o750, exist_ok=True)
    os.makedirs(os.path.abspath(opts['tensor_directory']), mode=0o750, exist_ok=True)

    model_path = os.path.join(opts['model_directory'], opts['model_file'])
    LOG.info("Saving model to %s", model_path)
    code_gen = CodeGenerator(G, DefaultNamingConvension(G), opts)
    if args.template_file:
        code_template = dynamic_template(args.template_file)
    else:
        code_template = default_template
    model = code_template(G, code_generator=code_gen)
    if not model:
        LOG.error("error generating model code")
        sys.exit(1)
    with open(model_path, "w") as output_fp:
        output_fp.write(model)
    if args.header_file:
        with open(os.path.join(opts['model_directory'], args.header_file), "w") as output_fp:
            output_fp.write(header_template(G, code_generator=code_gen))
    if not args.dont_dump_tensors:
        LOG.info("Writing constants to %s", opts['model_directory'])
        code_gen.write_constants()
