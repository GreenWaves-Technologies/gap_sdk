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

import csv
from utils.node_id import NodeId
from execution.quantization_mode import QuantizationMode
from execution.graph_executer import GraphExecuter
from io import StringIO
import logging
import os
import re
import shutil
import subprocess
import tempfile
import numpy as np
import texttable
import json

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from generation.code_generator import CodeGenerator
from generation.default_appl_main_template import (generate_main_appl_header,
                                                   generate_main_appl_make,
                                                   generate_main_appl_template)
from generation.naming_convension import DefaultNamingConvension
from interpreter.nntool_shell_base import NNToolShellBase, no_history

LOG = logging.getLogger("nntool")


def skip_existing_files(destdir):
    def fn(_, contents):
        return [file for file in contents
                if os.path.exists(os.path.join(destdir, file)) and
                not os.path.isdir(os.path.join(destdir, file))]
    return fn


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
                                 help='generate fake inputs (in the quantization range and generate a check of the results')
    parser_gen_proj.add_argument('--save_inputs',
                                 action='store_true',
                                 help='if test_results, save the inputs in files also')

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
        gen_project(self.G,
                    self.settings,
                    args.project_folder,
                    self._cmd_history[self._graph_idx].copy(),
                    overwrite=args.overwrite, performance=True,
                    quantized=self.settings['load_quantization'],
                    test_results=args.test_results,
                    save_inputs=args.save_inputs    )
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
        with tempfile.TemporaryDirectory() as tempdir:
            self.pfeedback('generating project from graph')
            gen_project(self.G,
                        self.settings,
                        tempdir,
                        self._cmd_history[self._graph_idx].copy(),
                        performance=True,
                        quantized=self.settings['load_quantization'])
            if not args.jobs:
                jobs = int(subprocess.getoutput('nproc --all'))
            else:
                jobs = args.jobs
            self.pfeedback(
                f'compiling and running project with {jobs} threads')
            with open(os.path.join(tempdir, 'run_make.sh'), 'w') as fp:
                fp.write(make_script(tempdir, jobs, platform=args.platform))

            res = subprocess.run(['/bin/bash', os.path.join(tempdir, 'run_make.sh')],
                                 capture_output=True, text=True,
                                 check=False, shell=False, cwd=tempdir)

            if args.show_stdout:
                self.ppaged(res.stdout)

            if args.show_stderr:
                self.ppaged(res.stderr)

            if res.returncode:
                self.ppaged(f'STDERR\n\n{res.stderr}')
                self.perror(
                    f'compile and run returned error code {res.returncode}')
                return

            match_perf = r" +((?:S\d+|Tota)[^:]+): *Cycles: +(\d+)[^:]+: +(\d+)[^:]+: +([\d<.]+)"
            matcher = re.compile(match_perf)
            perf = matcher.findall(res.stdout)
            if not perf:
                self.perror('unable to find performance data in output')
                self.ppaged(res.stdout)
                return

            total_cycles = int(perf[-1][1])
            perf = [[n, int(c), int(o), float(p), int(c) * 100/total_cycles] for n, c, o, p in perf]


            if args.format == 'json':
                self.pfeedback(
                    f'\n--- PERFORMANCE START ---\n{json.dumps(perf)}\n--- PERFORMANCE END ---\n')
            elif args.format == 'csv':
                with StringIO() as fp:
                    writer = csv.writer(fp)
                    writer.writerows(perf)
                    self.pfeedback(
                        f'\n--- PERFORMANCE START ---\n{fp.getvalue()}\n--- PERFORMANCE END ---\n')
            else:
                self.pfeedback('network performance')
                table = TexttableEx()
                table.header(['Layer', 'Cycles', 'Ops', 'Ops/Cycle', '% cycles'])
                table.set_header_align(['l', 'c', 'c', 'c', 'c'])
                table.set_cols_align(['l', 'r', 'r', 'r', 'r'])
                table.set_max_width(0)
                for row in perf:
                    table.add_row(row)
                self.ppaged(table.draw())


def make_script(tempdir, jobs=1, platform=None):
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
        f'make -j {jobs} clean all platform={platform}',
        f'make run platform={platform}'
    ])
    return '\n'.join(script) + '\n'

def get_rand(shape, low_high=None):
    if low_high:
        return (np.random.randint(low_high[0] * 127, low_high[1] * 127, size=shape).astype(np.float32))/127
    return np.random.randn(*shape).astype(np.float32)

def gen_project(G, settings, project_folder, script_commands, overwrite=False, performance=False, quantized=False, test_results=False, save_inputs=False):
    code_gen = CodeGenerator(
        G, DefaultNamingConvension(G), settings)

    if not os.path.exists(project_folder):
        os.mkdir(project_folder)

    finputs = None
    qoutputs = None
    if test_results:
        np.random.seed(12345)
        finputs = []
        for i, node in enumerate(G.input_nodes()):
            out_q = G.quantization[NodeId(node)].out_qs[0]
            finput = get_rand(node.out_dims[0].shape, low_high=(out_q.min_val, out_q.max_val))
            finputs.append(finput)
        executer = GraphExecuter(G, qrecs=G.quantization)
        qoutput_tensors = executer.execute(finputs.copy(),
                                           qmode=QuantizationMode.all())
        qoutputs = []
        for params in G.outputs():
            outp = qoutput_tensors[params.step_idx][0]
            qoutputs.append(outp)
        qinputs = []
        for params in G.input_nodes():
            inp = qoutput_tensors[params.step_idx][0]
            qinputs.append(inp)
            if save_inputs:
                nodeq = G.quantization[NodeId(node, None)].out_qs[0]
                np.save(os.path.join(project_folder, f"fake_input_{i}.npy"), nodeq.dequantize(inp))

    main = os.path.join(project_folder, f"{code_gen.project_name}")
    main_c = main + '.c'
    main_h = main + '.h'
    common_mk = os.path.join(project_folder, "common.mk")
    nntool_script = os.path.join(project_folder, "nntool_script")
    if overwrite or not os.path.exists(main_c):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.c"), "w") as output_fp:
            output_fp.write(generate_main_appl_template(G, code_gen, finputs, qoutputs))
    if overwrite or not os.path.exists(main_h):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.h"), "w") as output_fp:
            output_fp.write(generate_main_appl_header(G, code_gen))
    if overwrite or not os.path.exists(common_mk):
        with open(os.path.join(project_folder, "common.mk"), "w") as output_fp:
            output_fp.write(generate_main_appl_make(G, code_gen, quantized))
    if overwrite or not os.path.exists(nntool_script):
        with open(nntool_script, 'w') as fp:
            # NOTE - gen_template_project is excluded so that tests work. Normally it will not be in the
            # history.
            fp.writelines(
                [f"{line}\n" for line in script_commands
                    if not (line.startswith('open') or line.startswith('gen_project') or line.startswith('performance'))])
            if performance:
                for setting in ['set graph_produce_node_names true',
                                'set graph_produce_operinfos true',
                                'set graph_monitor_cycles true']:
                    fp.write(f'{setting}\n')
            if script_commands[-1] != "save_state":
                fp.write('save_state\n')
    ignore_function = None if overwrite else skip_existing_files(
        project_folder)
    shutil.copytree(os.path.join(os.environ.get("NNTOOL_PATH"), 'generation/project_template'), project_folder,
                    dirs_exist_ok=True, ignore=ignore_function)

    try:
        shutil.copy(G.graph_identity.filename, os.path.join(project_folder, os.path.split(G.graph_identity.filename)[1]))
    except shutil.SameFileError:
        pass
