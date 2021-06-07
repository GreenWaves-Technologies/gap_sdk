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
import distutils.ccompiler as compiler
import logging
import os
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from interpreter.nntool_shell_base import NNToolShellBase

LOG = logging.getLogger("nntool")

TILER_INC                    = os.environ.get('TILER_INC')
TILER_EMU_INC                = os.environ.get('TILER_EMU_INC')
TILER_CNN_GENERATOR_PATH     = os.environ.get('TILER_CNN_GENERATOR_PATH')
TILER_CNN_KERNEL_PATH        = os.environ.get('TILER_CNN_KERNEL_PATH')
TILER_CNN_GENERATOR_PATH_SQ8 = os.environ.get('TILER_CNN_GENERATOR_PATH_SQ8')
TILER_CNN_KERNEL_PATH_SQ8    = os.environ.get('TILER_CNN_KERNEL_PATH_SQ8')
TILER_CNN_GENERATOR_PATH_FP16 = os.environ.get('TILER_CNN_GENERATOR_PATH_FP16')
TILER_CNN_KERNEL_PATH_FP16    = os.environ.get('TILER_CNN_KERNEL_PATH_FP16')
#NNTOOL_GENERATOR_PATH        = os.environ.get('NNTOOL_GENERATOR_PATH')
#NNTOOL_KERNEL_PATH           = os.environ.get('NNTOOL_KERNELS_PATH')


class CompileCommand(NNToolShellBase):
    # GEN COMMAND
    parser_compile = Cmd2ArgumentParser()
    parser_compile.add_argument('model_file',
                                completer_method=Cmd.path_complete,
                                help='Path to autotiler model to compile')
    parser_compile.add_argument('--model_dir',
                                completer_method=Cmd.path_complete, default=None,
                                help='Path to the compilation output dir')
    parser_compile.add_argument('--executable', completer_method=Cmd.path_complete, default=None,
                                help='path to the executable, if not specified $MODEL_DIR/GenTile')
    parser_compile.add_argument('--add_sdl', action='store_true',
                                help="add sdl library during compilation")

    @with_argparser(parser_compile)
    def do_compile(self, args):
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        if args.model_dir:
            model_file = os.path.join(args.model_dir, args.model_file)
            model_dir = args.model_dir
        else:
            model_dir = os.path.dirname(os.path.realpath(args.model_file))
            model_file = args.model_file

        LOG.info(model_dir)
        exe = args.executable if args.executable else os.path.join(model_dir, "GenTile")
        cc = compiler.new_compiler(verbose=1)
        at_lib = os.environ.get('TILER_LIB')

        cc.add_include_dir(TILER_INC)
        cc.add_include_dir(TILER_EMU_INC)
        cc.add_include_dir(TILER_CNN_GENERATOR_PATH)
        cc.add_include_dir(TILER_CNN_KERNEL_PATH)
        # cc.add_include_dir(NNTOOL_GENERATOR_PATH)
        # cc.add_include_dir(NNTOOL_KERNEL_PATH)
        if "SQ8" in self.G.quantization.schemes_present:
            cc.add_include_dir(TILER_CNN_GENERATOR_PATH_SQ8)
            cc.add_include_dir(TILER_CNN_KERNEL_PATH_SQ8)
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            cc.add_include_dir(TILER_CNN_GENERATOR_PATH_FP16)
            cc.add_include_dir(TILER_CNN_KERNEL_PATH_FP16)
        if args.add_sdl:
            cc.add_library("SDL2")
            cc.add_library("SDL2_ttf")
        
        srcs = [model_file]
        at_gen_srcs = [os.path.join(TILER_CNN_GENERATOR_PATH, "CNN_Generator_Util.c")]
        at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH, "CNN_Copy_Generators.c"))
        # at_gen_srcs.append(os.path.join(NNTOOL_GENERATOR_PATH, "nntool_extra_generators.c"))
        if "SQ8" in self.G.quantization.schemes_present:
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH_SQ8, "CNN_Generators_SQ8.c"))
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH_SQ8, "RNN_Generators_SQ8.c"))
        if "POW2" in self.G.quantization.schemes_present:
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH, "CNN_Generators.c"))
        if any(qrec.ktype == "float" for qrec in self.G.quantization.values()):
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH_FP16, "CNN_Generators_fp16.c"))
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH_FP16, "RNN_Generators_fp16.c"))
        if self.G.has_ssd_postprocess:
            at_gen_srcs.append(os.path.join(TILER_CNN_GENERATOR_PATH, "SSD_Generators.c"))

        objs = cc.compile(
            srcs + at_gen_srcs,
            debug=1,
            extra_preargs=["-g"]
        )
        objs += [at_lib]
        cc.link_executable(objs, exe)
