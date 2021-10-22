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

from copy import deepcopy
import csv
import json
import logging
import os
import re
import shutil
import subprocess
import tempfile
from io import StringIO

import numpy as np
import texttable
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode
from generation.code_generator import CodeGenerator
from generation.default_appl_main_template import (
    generate_main_appl_header, generate_main_appl_make,
    generate_main_appl_make_atproject, generate_main_appl_template)
from generation.default_template import (basic_kernel_header_template,
                                         basic_kernel_source_template,
                                         default_template, dynamic_template,
                                         header_template)
from generation.naming_convension import DefaultNamingConvension
from interpreter.commands.aquant import AquantCommand
from interpreter.commands.open import OpenCommand
from interpreter.nntool_shell_base import NNToolShellBase, no_history
from interpreter.shell_utils import input_options
from utils.at_tensor_loader import at_map_tensors, at_tensor_loader_int
from utils.data_importer import import_data
from utils.node_id import NodeId

from graph.types.input_output import InputParameters

LOG = logging.getLogger("nntool")


def skip_existing_files(destdir):
    def fn(_, contents):
        return [file for file in contents
                if os.path.exists(os.path.join(destdir, file)) and
                not os.path.isdir(os.path.join(destdir, file))]
    return fn


def parse_last_open(history):
    args = None
    for command in reversed(history):
        if command.startswith('open'):
            args = OpenCommand.parser_open.parse_args(command.split(' ')[1:])
            break
    return args

def build_last_open_args(args):
    argnames = [argname for argname in dir(args) if not argname.startswith('_') and argname not in ['nnfile', 'new', 'load_quantization', 'orgmodel_path']]
    strargs = []
    for argname in argnames:
        attr = getattr(args, argname)
        if attr is None:
            continue
        if isinstance(attr, bool):
            if attr:
                strargs.append(f'--{argname}')
        elif isinstance(attr, dict):
            if attr:
                kvs = [f'{k}={v}' for k, v in attr.items()]
                strargs.append(f'--{argname} {" ".join(kvs)}')
        elif isinstance(attr, list):
            if attr:
                strargs.append(f'--{argname} {" ".join(attr)}')
        else:
            strargs.append(f'--{argname} {attr}')
    return ' '.join(strargs)

class GenProjectCommand(NNToolShellBase):
    # GEN PROJECT COMMAND
    parser_gen_proj = Cmd2ArgumentParser()
    parser_gen_proj.add_argument('project_folder',
                                 completer_method=Cmd.path_complete,
                                 help='project folder to create or update')
    parser_gen_proj.add_argument('-o', '--overwrite',
                                 action='store_true',
                                 help='overwrite existing files')
    parser_gen_proj.add_argument('--test_results',
                                 action='store_true',
                                 help='generate fake inputs in the quantization range and generate a check of the results')
    parser_gen_proj.add_argument('--atproject',
                                 action='store_true',
                                 help='generate a native autotiler project with the model already generated')
    parser_gen_proj.add_argument('--dump_tensors',
                                 action='store_true',
                                 help='print generated tensors')
    parser_gen_proj.add_argument('--input_file',
                                 completer_method=Cmd.path_complete, default=None,
                                 help='if test_results, use this file to run inference')
    parser_gen_proj.add_argument('--input_tensors',
                                type=str,
                                help='produce input tensors from tensor store with supplied name')
    parser_gen_proj.add_argument('--save_inputs',
                                 action='store_true',
                                 help='if test_results, save the inputs in files also')
    input_options(parser_gen_proj)

    @with_argparser(parser_gen_proj)
    @no_history
    def do_gen_project(self, args):
        """
Generate a project for the current graph including Makefiles, template main file and nntool script. The
script will be generated from the command history with open commands excluded and a final save_state command
added."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()

        if args.input_tensors:
            if args.input_tensors not in self.tensor_store:
                self.perror(
                    f'input tensor {args.input_tensors} not found in store')
                return
            store = self.tensor_store[args.input_tensors]
            input_tensors = []
            for params in self.G.nodes(node_classes=InputParameters):
                input_tensors.append(store[params.step_idx][0])
        else:
            input_tensors = None

        gen_project(self.G,
                    self.settings,
                    args.project_folder,
                    self._cmd_history[self._graph_idx].copy(),
                    overwrite=args.overwrite, performance=True,
                    quantized=self.settings['load_quantization'],
                    test_results=args.test_results,
                    save_inputs=args.save_inputs,
                    dump_tensors=args.dump_tensors,
                    input_file=args.input_file,
                    input_tensors=input_tensors,
                    input_args=self._get_input_args(args),
                    gen_atproject=args.atproject)
        self.pfeedback(f'project generated in {args.project_folder}')


class TexttableEx(texttable.Texttable):
    @classmethod
    def _fmt_int(cls, x, **kw):
        return f'{x:,}'


class PerformanceCommand(NNToolShellBase):
    # PERFORMANCE COMMAND
    parser_perf = Cmd2ArgumentParser()
    parser_perf.add_argument('-j', '--jobs',
                             type=int,
                             help='number of jobs to use building the network')
    parser_perf.add_argument('-s', '--show_stdout',
                             action='store_true',
                             help='show stdout of run')
    parser_perf.add_argument('-e', '--show_stderr',
                             action='store_true',
                             help='show stderr of run')
    parser_perf.add_argument('-f', '--format',
                             choices=['table', 'json', 'csv'],
                             help='output json')
    parser_perf.add_argument('-p', '--platform',
                             default='gvsoc',
                             help='GAP SDK platform to run the resulting model on')
    parser_perf.add_argument('--pmsis_os',
                             default='pulpos',
                             help='GAP SDK pmsis os [freertos (default), pulpos]')
    parser_perf.add_argument('--atproject',
                             action='store_true',
                             help='generate a native autotiler project with the model already generated')
    parser_perf.add_argument('-t', '--output_tensors',
                             type=str,
                             help='load tensors dumped by gvsoc run into tensor store with supplied name')
    parser_perf.add_argument('--input_tensors',
                             type=str,
                             help='produce input tensors from tensor store with supplied name')
    parser_perf.add_argument('--path',
                             type=str,
                             help='store the generated project in this directory rather than a temporary one - will overwrite files in directory')
    parser_perf.add_argument('--logfile',
                             type=str,
                             help='store the generated project in this directory rather than a temporary one - will overwrite files in directory')

    @with_argparser(parser_perf)
    @no_history
    def do_performance(self, args):
        """
Generates and compiles the current model with random input and runs it on gvsoc to analyse its performance.
This command can take a few minutes to complete."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        if "GAP_SDK_HOME" not in os.environ:
            self.perror(
                'you must run "source sourceme.sh" in the GAP SDK before using this command')
            return
        if args.input_tensors:
            if args.input_tensors not in self.tensor_store:
                self.perror(
                    f'input tensor {args.input_tensors} not found in store')
                return
            store = self.tensor_store[args.input_tensors]
            input_tensors = []
            for params in self.G.nodes(node_classes=InputParameters):
                input_tensors.append(store[params.step_idx][0])
        else:
            input_tensors = None

        with tempfile.TemporaryDirectory() as tempdir:
            directory = args.path if args.path else tempdir
            self.pfeedback('generating project from graph')
            gen_project(self.G,
                        self.settings,
                        directory,
                        self._cmd_history[self._graph_idx].copy(),
                        performance=True,
                        quantized=self.settings['load_quantization'],
                        gen_atproject=args.atproject,
                        dump_tensors=args.output_tensors,
                        input_tensors=input_tensors,
                        overwrite=True)
            if not args.jobs:
                jobs = int(subprocess.getoutput('nproc --all'))
            else:
                jobs = args.jobs
            self.pfeedback(
                f'compiling and running project with {jobs} threads')
            with open(os.path.join(directory, 'run_make.sh'), 'w') as fp:
                fp.write(make_script(directory, jobs, platform=args.platform, pmsis_os=args.pmsis_os))

            res = subprocess.run(['/bin/bash', os.path.join(directory, 'run_make.sh')],
                                 capture_output=True, text=True,
                                 check=False, shell=False, cwd=directory)

            if args.show_stdout:
                self.ppaged(res.stdout)

            if args.show_stderr:
                self.ppaged(res.stderr)

            if res.returncode:
                if args.logfile:
                    with open(args.logfile, "w") as f:
                        f.write(res.stderr)
                else:
                    self.ppaged(f'STDERR\n\n{res.stderr}')
                    self.perror(
                        f'compile and run returned error code {res.returncode}')
                return

            if args.output_tensors:
                with StringIO(res.stdout) as fp:
                    self.tensor_store[args.output_tensors] = at_map_tensors(
                        self.G, at_tensor_loader_int(fp))

            match_perf = r" +((?:S\d+|Tota)[^:]+): *Cycles: +(\d+)[^:]+: +(\d+)[^:]+: +([\d<.]+)"
            matcher = re.compile(match_perf)
            perf = matcher.findall(res.stdout)
            if not perf:
                self.perror('unable to find performance data in output')
                self.ppaged(res.stdout)
                return

            total_cycles = int(perf[-1][1])
            perf = [[n, int(c), int(o), float(p), int(c) * 100/total_cycles]
                    for n, c, o, p in perf]

            if args.format == 'json':
                self.pfeedback(
                    f'\n--- PERFORMANCE START ---\n{json.dumps(perf)}\n--- PERFORMANCE END ---\n')
            elif args.format == 'csv':
                if args.logfile:
                    with open(args.logfile, "w") as fp:
                        writer = csv.writer(fp)
                        writer.writerows(perf)
                else:
                    with StringIO() as fp:
                        writer = csv.writer(fp)
                        writer.writerows(perf)
                        self.pfeedback(
                            f'\n--- PERFORMANCE START ---\n{fp.getvalue()}\n--- PERFORMANCE END ---\n')
            else:
                self.pfeedback('network performance')
                table = TexttableEx()
                table.header(['Layer', 'Cycles', 'Ops',
                              'Ops/Cycle', '% cycles'])
                table.set_header_align(['l', 'c', 'c', 'c', 'c'])
                table.set_cols_align(['l', 'r', 'r', 'r', 'r'])
                table.set_max_width(0)
                for row in perf:
                    table.add_row(row)
                self.ppaged(table.draw())


def make_script(tempdir, jobs=1, platform=None, pmsis_os="freertos"):
    if platform is None:
        platform = 'gvsoc'
    script = [
        f'cd {tempdir}\n',
    ]
    script.extend([
        f'source {os.path.join(os.environ["GAP_SDK_HOME"], "configs/gap9_v2.sh")}'
    ])
    script.extend([f'export {var}={os.environ[var]}'
                   for var in os.environ if var.startswith('TILER')])
    script.extend([
        f'export PATH={os.environ["NNTOOL_DIR"]}:$PATH',
        f'make -C {tempdir} clean PMSIS_OS={pmsis_os}',
        f'make -C {tempdir} -j {jobs} all platform={platform} PMSIS_OS={pmsis_os}',
        f'make -C {tempdir} run platform={platform} PMSIS_OS={pmsis_os}'
    ])
    return '\n'.join(script) + '\n'


def get_rand(shape, low_high=None):
    if low_high:
        return (np.random.randint(low_high[0] * 127, low_high[1] * 127, size=shape).astype(np.float32))/127
    return np.random.randn(*shape).astype(np.float32)


def process_script(script):
    aquant_parser = AquantCommand.parser_aquant
    out_lines = []
    for line in script:
        if any(line.startswith(s) for s in ['open', 'gen_project', 'performance', 'dump', 'tensors']):
            continue
        if line.startswith('aquant'):
            # add abs path for input files and try to remake command
            args = aquant_parser.parse_args(line.split(' ')[1:])
            input_files = [os.path.abspath(f) for f in args.input_files]
            opts = [f"--{k} {v}" if v != True else f"--{k}" for k, v in vars(args).items()
                    if v and k != 'input_files']
            line = " ".join(['aquant'] + opts + input_files)
        out_lines.append(f'{line}\n')
    return out_lines


def gen_project(G, settings, project_folder, script_commands, overwrite=False, performance=False,
                quantized=False, test_results=False, save_inputs=False, input_file=None, input_args=None,
                gen_atproject=False, dump_tensors=False, input_tensors=None):
    settings = deepcopy(settings)
    settings['graph_monitor_cycles'] = True
    settings['graph_produce_node_names'] = True
    settings['graph_produce_operinfos'] = True

    code_gen = CodeGenerator(
        G, DefaultNamingConvension(G), settings)

    if not os.path.exists(project_folder):
        os.mkdir(project_folder)

    qoutputs = None
    if test_results:
        np.random.seed(12345)
        finput_tensors = []
        input_tensors = []
        for i, node in enumerate(G.input_nodes()):
            out_q = G.quantization[NodeId(node)].out_qs[0]
            if input_file:
                finput = import_data(input_file, **input_args)
            else:
                min_val = out_q.min_val if not out_q.is_floating else -1.0
                max_val = out_q.max_val if not out_q.is_floating else 1.0
                finput = get_rand(node.out_dims[0].shape, low_high=(
                    min_val, max_val))
            finput_tensors.append(finput)
        executer = GraphExecuter(G, qrecs=G.quantization)
        qoutput_tensors = executer.execute(finput_tensors.copy(),
                                           qmode=QuantizationMode.all())
        qoutputs = []
        for params in G.outputs():
            outp = qoutput_tensors[params.step_idx][0]
            qoutputs.append(outp)
        for i, params in enumerate(G.input_nodes()):
            inp = qoutput_tensors[params.step_idx][0]
            input_tensors.append(inp)
            if save_inputs:
                nodeq = G.quantization[NodeId(params, None)].out_qs[0]
                np.save(os.path.join(project_folder,
                                     f"fake_input_{i}.npy"), nodeq.dequantize(inp))

    main = os.path.join(project_folder, f"{code_gen.project_name}")
    main_c = main + '.c'
    main_h = main + '.h'
    common_mk = os.path.join(project_folder, "common.mk")
    nntool_script = os.path.join(project_folder, "nntool_script")
    if overwrite or not os.path.exists(main_c):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.c"), "w") as output_fp:
            output_fp.write(generate_main_appl_template(
                G, code_gen, input_tensors, qoutputs))
    if overwrite or not os.path.exists(main_h):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.h"), "w") as output_fp:
            output_fp.write(generate_main_appl_header(G, code_gen))
    if overwrite or not os.path.exists(common_mk):
        open_args = parse_last_open(script_commands)
        open_args = build_last_open_args(open_args) if open_args else ""
        with open(os.path.join(project_folder, "common.mk"), "w") as output_fp:
            if gen_atproject:
                output_fp.write(generate_main_appl_make_atproject(
                    G, code_gen, quantized, 'Model.c'))
            else:
                output_fp.write(generate_main_appl_make(
                    G, code_gen, quantized, open_args=open_args))
    if overwrite or not os.path.exists(nntool_script):
        with open(nntool_script, 'w') as fp:
            # NOTE - gen_template_project is excluded so that tests work. Normally it will not be in the
            # history.
            fp.writelines(process_script(script_commands))
            # always add performance since the main template uses it
            for setting in ['set graph_produce_node_names true',
                            'set graph_produce_operinfos true',
                            'set graph_monitor_cycles true']:
                fp.write(f'{setting}\n')
            if dump_tensors:
                fp.write('set graph_dump_tensor 7\n')

            if script_commands[-1] != "save_state":
                fp.write('save_state\n')
    if gen_atproject:
        code_gen = CodeGenerator(G, DefaultNamingConvension(G), settings)
        with open(os.path.join(project_folder, 'Model.c'), "w") as output_fp:
            output_fp.write(default_template(G, code_generator=code_gen))
        if G.has_expressions:
            with open(os.path.join(project_folder, "Expression_Kernels.c"), "w") as output_fp:
                output_fp.write(basic_kernel_source_template(
                    G, code_generator=code_gen))
            with open(os.path.join(project_folder, "Expression_Kernels.h"), "w") as output_fp:
                output_fp.write(basic_kernel_header_template(
                    G, code_generator=code_gen))
        code_gen.write_constants(tensor_directory=project_folder)
    ignore_function = None if overwrite else skip_existing_files(
        project_folder)
    shutil.copytree(os.path.join(os.environ.get("NNTOOL_PATH"), 'generation/project_template'), project_folder,
                    dirs_exist_ok=True, ignore=ignore_function)

    if not gen_atproject:
        try:
            shutil.copy(G.graph_identity.filename, os.path.join(
                project_folder, os.path.split(G.graph_identity.filename)[1]))
        except shutil.SameFileError:
            pass
