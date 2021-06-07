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
import re
import shutil
import subprocess
import tempfile
import texttable

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
                    overwrite=args.overwrite, performance=True)
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

    @with_argparser(parser_perf)
    @no_history
    def do_performance(self, args):
        """
Generates and compiles the current model with random input and runs it on gvsoc to analyse its performance.
This command can take a few minutes to complete."""
        self._check_graph()
        self._check_quantized()
        self._check_adjusted()
        with tempfile.TemporaryDirectory() as tempdir:
            self.pfeedback('generating project from graph')
            gen_project(self.G,
                        self.settings,
                        tempdir,
                        self._cmd_history[self._graph_idx].copy(),
                        performance=True)
            if not args.jobs:
                jobs = int(subprocess.getoutput('nproc --all'))
            else:
                jobs = args.jobs
            self.pfeedback(f'compiling and running project with {jobs} threads')
            with open(os.path.join(tempdir, 'run_make.sh'), 'w') as fp:
                fp.write(make_script(tempdir, jobs))

            res = subprocess.run(['/bin/bash', os.path.join(tempdir, 'run_make.sh')],
                                 capture_output=True, text=True,
                                 check=False, shell=False, cwd=tempdir)
            if res.returncode:
                self.perror(
                    f'compile and run returned error code {res.returncode}')
                self.ppaged(res.stderr)
                return

            match_perf = r" +((?:S\d+|Tota)[^:]+): *Cycles: +(\d+)[^:]+: +(\d+)[^:]+: +([\d<.]+)"
            matcher = re.compile(match_perf)
            perf = matcher.findall(res.stdout)
            if not perf:
                self.perror('unable to find performance data in output')
                self.ppaged(res.stdout)
                return

            self.pfeedback('network performance')
            table = TexttableEx()
            table.header(['Layer', 'Cycles', 'Ops', 'Ops/Cycle'])
            table.set_header_align(['l', 'c', 'c', 'c'])
            table.set_cols_align(['l', 'r', 'r', 'r'])
            table.set_max_width(0)
            for row in [[elem[0], int(elem[1]), int(elem[2]), float(elem[3])] for elem in perf]:
                table.add_row(row)
            self.ppaged(table.draw())


def make_script(tempdir, jobs=1):
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
        f'make -j {jobs} clean all platform=gvsoc',
        'make run platform=gvsoc'
    ])
    return '\n'.join(script) + '\n'


def gen_project(G, settings, project_folder, script_commands, overwrite=False, performance=False):
    code_gen = CodeGenerator(
        G, DefaultNamingConvension(G), settings)

    if not os.path.exists(project_folder):
        os.mkdir(project_folder)
    main = os.path.join(project_folder, f"{code_gen.project_name}")
    main_c = main + '.c'
    main_h = main + '.h'
    common_mk = os.path.join(project_folder, "common.mk")
    nntool_script = os.path.join(project_folder, "nntool_script")
    if overwrite or not os.path.exists(main_c):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.c"), "w") as output_fp:
            output_fp.write(generate_main_appl_template(G, code_gen))
    if overwrite or not os.path.exists(main_h):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.h"), "w") as output_fp:
            output_fp.write(generate_main_appl_header(G, code_gen))
    if overwrite or not os.path.exists(common_mk):
        with open(os.path.join(project_folder, "common.mk"), "w") as output_fp:
            output_fp.write(generate_main_appl_make(G, code_gen))
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
    shutil.copytree('generation/project_template', project_folder,
                    dirs_exist_ok=True, ignore=ignore_function)
