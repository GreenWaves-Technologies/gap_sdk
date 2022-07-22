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
import json
import logging
import os
import re
import subprocess
import tempfile
from io import StringIO

import texttable
from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from nntool.generation.gen_project.utils import gen_project, make_script
from nntool.interpreter.nntool_shell_base import NNToolShellBase, no_history
from nntool.interpreter.shell_utils import input_options
from nntool.utils.at_tensor_loader import at_map_tensors, at_tensor_loader_int

from nntool.graph.types import InputNode

LOG = logging.getLogger("nntool")



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
                                 help='generate a check for loop in the C code of the results')
    parser_gen_proj.add_argument('--atproject',
                                 action='store_true',
                                 help='generate a native autotiler project with the model already generated')
    parser_gen_proj.add_argument('--dump_tensors',
                                 action='store_true',
                                 help='print generated tensors')
    parser_gen_proj.add_argument('--input_file', nargs='+',
                                 completer_method=Cmd.path_complete, default=None,
                                 help='if test_results, use this file to run inference')
    parser_gen_proj.add_argument('--input_tensors',
                                 type=str,
                                 help='produce input tensors from tensor store with supplied name')
    parser_gen_proj.add_argument('--save_inputs',
                                 action='store_true',
                                 help='deprecated')
    parser_gen_proj.add_argument('--tolerance', type=float, default=0.0,
                                 help="if test_results active, use this tolerance to check the results")
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

        if "GAP_SDK_HOME" not in os.environ or "NNTOOL_PATH" not in os.environ:
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
            for params in self.G.nodes(node_classes=InputNode):
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
                    dump_tensors=args.dump_tensors,
                    input_file=args.input_file,
                    input_tensors=input_tensors,
                    input_args=self._get_input_args(args),
                    gen_atproject=args.atproject,
                    tolerance=args.tolerance)
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
                             default='freertos',
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
                             help=('store the generated project in this directory rather than a temporary one'
                                   ' - will overwrite files in directory'))
    parser_perf.add_argument('--logfile',
                             type=str,
                             help=('store the generated project in this directory rather than a temporary one'
                                   ' - will overwrite files in directory'))

    @with_argparser(parser_perf)
    @no_history
    def do_performance(self, args):
        """
Generates and compiles the current model with random input and runs it on gvsoc to analyse its performance.
This command can take a few minutes to complete."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        if "GAP_SDK_HOME" not in os.environ or "NNTOOL_PATH" not in os.environ:
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
            for params in self.G.nodes(node_classes=InputNode):
                input_tensors.append(store[params.step_idx][0])
        else:
            input_tensors = None

        with tempfile.TemporaryDirectory() as tempdir:
            directory = args.path if args.path else tempdir
            self.pfeedback('generating project from nntool.graph')
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
                fp.write(make_script(directory, jobs,
                                     platform=args.platform, pmsis_os=args.pmsis_os))

            res = subprocess.run(['/bin/bash', os.path.join(directory, 'run_make.sh')],
                                 capture_output=True, text=True,
                                 check=False, shell=False, cwd=directory)

            if args.show_stdout:
                self.ppaged(res.stdout)

            if args.show_stderr:
                self.ppaged(res.stderr)

            if res.returncode:
                if args.logfile:
                    with open(args.logfile, "w") as fp:
                        fp.write(res.stderr)
                else:
                    self.ppaged(f'STDERR\n\n{res.stderr}')
                    self.perror(
                        f'compile and run returned error code {res.returncode}')
                return

            if args.output_tensors:
                with StringIO(res.stdout) as fp:
                    self.tensor_store[args.output_tensors] = at_map_tensors(
                        self.G, at_tensor_loader_int(fp))

            match_perf = r" +((?:S\d+)[^:]+): *Cycles: +(\d+)[^:]+: +(\d+.\d+)[^:]+: +(\d+)[^:]+: +(\d+.\d+)[^:]+: +([\d<.]+)"
            matcher = re.compile(match_perf)
            performance = matcher.findall(res.stdout)
            if not performance:
                self.perror('unable to find performance data in output')
                self.ppaged(res.stdout)
                return

            performance = [[n, int(c), int(o), float(p)]
                           for n, c, _, o, _, p in performance]
            total_cycles = sum(perf[1] for perf in performance)
            total_ops = sum(perf[2] for perf in performance)
            performance = [perf + [perf[1] * 100/total_cycles]
                            for perf in performance]
            performance.append(
                        ['Total', total_cycles, total_ops, total_ops/total_cycles, 100])

            if args.format == 'json':
                self.pfeedback(
                    f'\n--- PERFORMANCE START ---\n{json.dumps(performance)}\n--- PERFORMANCE END ---\n')
            elif args.format == 'csv':
                if args.logfile:
                    with open(args.logfile, "w") as fp:
                        writer = csv.writer(fp)
                        writer.writerows(performance)
                else:
                    with StringIO() as fp:
                        writer = csv.writer(fp)
                        writer.writerows(performance)
                        self.pfeedback(
                            f'\n--- PERFORMANCE START ---\n{fp.getvalue()}\n--- PERFORMANCE END ---\n')
            else:
                self.pfeedback('network performance')
                table = TexttableEx()
                table.header(['Layer', 'Cycles', 'Ops',
                              'Ops/Cycle', '% cycles'])
                table.set_header_align(['l', 'c', 'c', 'c', 'c'])
                table.set_cols_dtype(['t', 'i', 'i', 'f', 'f'])
                table.set_cols_align(['l', 'r', 'r', 'r', 'r'])
                table.set_max_width(0)
                for row in performance:
                    table.add_row(row)
                self.ppaged(table.draw())
