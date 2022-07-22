# Copyright (C) 2022  GreenWaves Technologies, SAS

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

import os
import shutil
from copy import deepcopy

import numpy as np
from nntool.execution.graph_executer import GraphExecuter
from nntool.execution.quantization_mode import QuantizationMode
from nntool.generation.code_generator import CodeGenerator
from nntool.generation.default_appl_main_template import (
    generate_main_appl_header, generate_main_appl_make,
    generate_main_appl_make_atproject, generate_main_appl_template)
from nntool.generation.default_template import (basic_kernel_header_template,
                                                basic_kernel_source_template,
                                                default_template)
from nntool.generation.gen_utils import write_empty
from nntool.generation.naming_convension import DefaultNamingConvension
from nntool.interpreter.commands.aquant import AquantCommand
from nntool.interpreter.commands.open_parser import get_open_parser
from nntool.interpreter.shell_utils import glob_input_files

from nntool.utils.data_importer import import_data


def skip_existing_files(destdir):
    def func(_, contents):
        return [file for file in contents
                if os.path.exists(os.path.join(destdir, file)) and
                not os.path.isdir(os.path.join(destdir, file))]
    return func


def parse_last_open(history):
    args = None
    for command in reversed(history):
        if command.startswith('open'):
            args = get_open_parser().parse_args(command.strip().split(' ')[1:])
            break
    return args


def build_last_open_args(args):
    argnames = [argname for argname in dir(args)
                if not argname.startswith('_') and argname not in [
                    'nnfile', 'new', 'load_quantization', 'orgmodel_path']]
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


def make_script(tempdir, jobs=1, platform=None, pmsis_os="freertos",
                at_loglevel=0, dont_run=False, profile=False, target='gap9_v2',
                do_clean=True):
    if platform is None:
        platform = 'gvsoc'
    tempdir = os.path.abspath(tempdir)
    script = [
        f'cd {tempdir}\n',
    ]
    script.extend([
        f'source {os.path.join(os.environ["GAP_SDK_HOME"], f"configs/{target}.sh")}'
    ])
    script.extend([f'export {var}={os.environ[var]}'
                   for var in os.environ if var.startswith('TILER')])
    script.extend([
            f'export PATH={os.environ["NNTOOL_PATH"]}:$PATH',
            f'export PYTHONPATH={os.environ["NNTOOL_DIR"]}:$PYTHONPATH'
            ])
    if platform == "EMUL":
        script.extend([
            f'make -C {tempdir} -f emul.mk {"clean" if do_clean else ""} model MODEL_GEN_AT_LOGLEVEL={at_loglevel}',
            f'make -C {tempdir} -f emul.mk -j {jobs} all'
        ])
        if not dont_run:
            script.extend([
                f'make -C {tempdir} -f emul.mk run'
            ])
    else:
        script.extend([
            f'make -C {tempdir} {"clean" if do_clean else ""} model PMSIS_OS={pmsis_os}'
        ])
        if not dont_run:
            script.extend([
                f'make -C {tempdir} -j {jobs} {"model" if dont_run else "all"} '
                f'platform={platform} PMSIS_OS={pmsis_os} MODEL_GEN_AT_LOGLEVEL={at_loglevel}',
                f'make -C {tempdir} {"profile" if profile else "run"} platform={platform} PMSIS_OS={pmsis_os}'
            ])
    return '\n'.join(script) + '\n'


def get_rand(shape, low_high=None):
    val = np.random.randn(*shape).astype(np.float32)
    if low_high:
        val = (val * (low_high[1]-low_high[0])) + low_high[0]
    return val


def process_script(script):
    aquant_parser = AquantCommand.parser_aquant
    out_lines = []
    for line in script:
        if any(line.startswith(s) for s in ['open', 'gen_project', 'performance', 'dump', 'tensors']):
            continue
        if line.startswith('aquant'):
            # add abs path for input files and try to remake command
            args = aquant_parser.parse_args(line.rstrip().split(' ')[1:])
            input_files = [os.path.abspath(f)
                           for f in args.input_files if f != '']
#pylint: disable=singleton-comparison
            opts = [f"--{k} {v}" if v != True else f"--{k}" for k, v in vars(args).items()
                    if v and k != 'input_files']
            line = " ".join(['aquant'] + opts + input_files)
        out_lines.append(f'{line}\n')
    return out_lines


def gen_project(G, settings, project_folder, script_commands, overwrite=False, performance=False,
                quantized=False, test_results=False, input_file=None, input_args=None,
                gen_atproject=False, dump_tensors=False, finput_tensors=None, input_tensors=None, tolerance=0.0):
    if not os.path.exists(project_folder):
        os.mkdir(project_folder)

    settings = deepcopy(settings)
    settings['graph_monitor_cycles'] = True
    settings['graph_produce_node_names'] = True
    settings['graph_produce_operinfos'] = True
    if "tensor_directory" in settings and not os.path.exists(os.path.join(project_folder, settings['tensor_directory'])):
        os.makedirs(os.path.join(project_folder, settings['tensor_directory']), mode=0o750, exist_ok=True)

    code_gen = CodeGenerator(
        G, DefaultNamingConvension(), settings)

    qoutput_tensors = None
    if test_results:
        qinput_tensors = []
        if finput_tensors is None:
            finput_tensors = []
            if input_tensors:
                for i, params in enumerate(G.input_nodes()):
                    nodeq = G.quantization[params.name].out_qs[0]
                    finput_tensors.append(nodeq.dequantize(input_tensors[i]))
            else:
                np.random.seed(12345)
                for i, node in enumerate(G.input_nodes()):
                    out_q = G.quantization[node.name].out_qs[0]
                    if input_file:
                        file_per_input = glob_input_files(
                            input_file, G.num_inputs)[0]
                        finput = import_data(file_per_input[i], **input_args)
                    else:
                        min_val = out_q.min if not out_q.is_floating else -1.0
                        max_val = out_q.max if not out_q.is_floating else 1.0
                        finput = get_rand(node.out_dims[0].shape, low_high=(
                            min_val, max_val))
                    finput_tensors.append(finput)

        # Execution on float inputs
        executer = GraphExecuter(G, qrecs=G.quantization)
        qout = executer.execute(finput_tensors.copy(),
                                qmode=QuantizationMode.all())

        qoutput_tensors = []
        for params in G.output_nodes():
            outp = qout[params.step_idx][0]
            qoutput_tensors.append(outp)
        for i, params in enumerate(G.input_nodes()):
            inp = qout[params.step_idx][0]
            qinput_tensors.append(inp)
            nodeq = G.quantization[params.name].out_qs[0]
            np.save(os.path.join(project_folder,
                                 f"fake_input_{i}.npy"), nodeq.dequantize(inp))
        input_tensors = qinput_tensors

    main = os.path.join(project_folder, f"{code_gen.project_name}")
    main_c = main + '.c'
    main_h = main + '.h'
    common_mk = os.path.join(project_folder, "common.mk")
    nntool_script = os.path.join(project_folder, "nntool_script")
    if overwrite or not os.path.exists(main_c):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.c"), "w") as output_fp:
            output_fp.write(generate_main_appl_template(
                G, code_gen, input_tensors, qoutput_tensors, tolerance))
    if overwrite or not os.path.exists(main_h):
        with open(os.path.join(project_folder, f"{code_gen.project_name}.h"), "w") as output_fp:
            output_fp.write(generate_main_appl_header(G, code_gen))

    if input_tensors:
        for i, node in enumerate(code_gen.input_nodes):
            inp = input_tensors[i]
            name = node.name.capitalize()
            if overwrite or not os.path.exists(f"{name}.bin"):
                with open(os.path.join(project_folder, f"{name}.bin"), "wb") as in_f:
                    inp.astype(G.quantization[node.name].out_qs[0].dtype, order='C', casting='unsafe', copy=True)\
                        .tofile(in_f)

    if gen_atproject:
        if dump_tensors:
            code_gen.opts['graph_dump_tensor'] = 7 if isinstance(dump_tensors, bool) else int(dump_tensors)
        with open(os.path.join(project_folder, 'Model.c'), "w") as output_fp:
            output_fp.write(default_template(G, code_generator=code_gen))
        if G.has_expressions:
            with open(os.path.join(project_folder, "Expression_Kernels.c"), "w") as output_fp:
                output_fp.write(basic_kernel_source_template(
                    G, code_generator=code_gen))
            with open(os.path.join(project_folder, "Expression_Kernels.h"), "w") as output_fp:
                output_fp.write(basic_kernel_header_template(
                    G, code_generator=code_gen))
        else:
            write_empty(project_folder, "Expression_Kernels.c",
                        "no expressions used")
            write_empty(project_folder, "Expression_Kernels.h",
                        "no expressions used")

        code_gen.write_constants(tensor_directory=project_folder)
    elif overwrite or not os.path.exists(nntool_script):
        with open(nntool_script, 'w') as fp:
            # NOTE - gen_template_project is excluded so that tests work. Normally it will not be in the
            # history.
            fp.writelines(process_script(script_commands))
            # always add performance since the main template uses it
            for setting in ['set graph_produce_node_names true',
                            'set graph_produce_operinfos true',
                            'set graph_monitor_cycles true']:
                fp.write(f'{setting}\n')
            # always add memory setting
            for setting in ['set l3_flash_device $(MODEL_L3_FLASH)',
                            'set l3_ram_device $(MODEL_L3_RAM)']:
                fp.write(f'{setting}\n')
            if dump_tensors:
                fp.write(f'set graph_dump_tensor {7 if isinstance(dump_tensors, bool) else int(dump_tensors)}\n')

            if script_commands[-1] != "save_state":
                fp.write('save_state\n')

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

    ignore_function = None if overwrite else skip_existing_files(
        project_folder)
    shutil.copytree(os.path.join(os.environ.get("NNTOOL_DIR"), 'nntool/generation/project_template'), project_folder,
                    dirs_exist_ok=True, ignore=ignore_function)

    if not gen_atproject:
        try:
            shutil.copy(G.filename, os.path.join(
                project_folder, os.path.split(G.filename)[1]))
        except shutil.SameFileError:
            pass
