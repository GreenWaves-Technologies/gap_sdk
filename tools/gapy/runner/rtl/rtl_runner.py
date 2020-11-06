#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from elftools.elf.elffile import ELFFile
import runner.default_runner
import os
import os.path
import errors
import argparse
import json_tools as js
try:
    import gv.gvsoc
except:
    pass
from errors import FatalError


def appendArgs(parser: argparse.ArgumentParser, runnerConfig: js.config) -> None:
    """
    Append specific module arguments.

    """
    parser.add_argument("--gui",
                        dest="gui",
                        action="store_true",
                        help="Launch in gui mode")

    parser.add_argument("--cov",
                        dest="cov",
                        action="store_true",
                        help="Launch in coverage mode")

    parser.add_argument("--extend-traces",
                        dest="extend_traces",
                        action="store_true",
                        help="Extend instruction traces")

    parser.add_argument("--no-run",
                        dest="no_run",
                        action="store_true",
                        help="Don't run simulation")

    try:
        gv.gvsoc.appendArgs(parser, runnerConfig)
    except:
        pass


class Runner(runner.default_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)

        self.__process_args()

        try:
            gv.gvsoc.process_args(args, config)
        except:
            pass

        self.areas = []
        self.cmd_args = []
        self.plt_args = []
        self.env = {}
        self.platform_path = None
        self.platform_tool = self.config.get_str('rtl/simulator')

        if os.environ.get('GAPY_RTL_SIMULATOR') is not None:
            self.platform_tool = os.environ.get('GAPY_RTL_SIMULATOR')

        self.plt_config = self.config.get('rtl/' + self.platform_tool)
        self.plt_profile_config = self.plt_config.get('profiles/rtl')

        if self.config.get('**/runner/peripherals') is not None:
            self.set_arg('-gCONFIG_FILE=rtl_config.json')

            if os.environ.get('INSTALL_DIR') is not None:
                dpi_path = '%s/lib/libpulpdpi' % (os.environ.get('INSTALL_DIR'))
                if not os.path.exists(dpi_path + '.so'):
                    raise FatalError('Did no find DPI models: ' + dpi_path + '.so')

                self.set_arg('-sv_lib %s' % dpi_path)

        #
        # Co-simulation with GVSOC
        #
        if self.config.get_bool('**/runner/gvsoc_dpi/enabled'):
            dpi_path = '%s/lib/libgvsocdpi' % (os.environ.get('INSTALL_DIR'))

            if not os.path.exists(dpi_path + '.so'):
                    raise FatalError('Did no find DPI models: ' + dpi_path + '.so')
                
            if self.platform_tool == 'vsim':
                self.set_arg('-sv_lib %s' % dpi_path)
            else:
                self.set_cmd_arg('-sv_lib %s' % dpi_path)


            full_config, gvsoc_config_path = gv.gvsoc.gen_config(args, self.config)
            gv.gvsoc.prepare_exec(self.config, full_config)
            gv.gvsoc.dump_config(full_config, gvsoc_config_path)
        
            if self.platform_tool == 'vsim':
                self.set_arg('+DPI_CONFIG_FILE=%s' % gvsoc_config_path)
            else:
                self.set_cmd_arg('+DPI_CONFIG_FILE=%s' % gvsoc_config_path)

        else:
            if os.environ.get('TARGET_CHIP') == 'GAP9_V2':
                dpi_path = os.path.join(self.__get_platform_path(), 'ips_inputs/dpi/libchipwrapper')

                if self.platform_tool == 'vsim':
                    self.set_arg('-sv_lib %s' % dpi_path)
                else:
                    self.set_cmd_arg('-sv_lib %s' % dpi_path)

        if self.args.cov or os.environ.get('GAPY_RTL_COVERAGE') is not None:
            test_name = os.environ.get('PLPTEST_NAME')
            if test_name is None:
                test_name = 'test'
            test_name = test_name.replace(':', '.').replace('/', '.')

            self.set_cmd_arg('-covoverwrite -covworkdir %s/cov_work -covtest %s' % (os.environ.get('XCSIM_PATH'), test_name))


        self.full_config =  js.import_config(self.config.get_dict(), interpret=True, gen=True)


    def exec(self):
        os.chdir(self.config.get_str('gapy/work_dir'))

        plt_path = self.__get_platform_path()

        if self.platform_tool == 'vsim':

            self.__create_symlink(plt_path, 'boot')
            self.__create_symlink(plt_path, 'modelsim.ini')
            self.__create_symlink(plt_path, 'work')
            self.__create_symlink(plt_path, 'tcl_files')
            self.__create_symlink(plt_path, 'waves')
            self.__create_symlink(plt_path, 'models')
            self.__create_symlink(plt_path, 'ips_inputs')

        else:

            sim_path = os.environ.get('SIM_PATH')

            self.__create_symlink(plt_path, 'boot')
            self.__create_symlink(plt_path, 'ips_inputs')
            self.__create_symlink(plt_path, 'models')
            self.__create_symlink(plt_path, 'tcl_files')
            self.__create_symlink(plt_path, 'cds.lib')
            self.__create_symlink(plt_path, 'hdl.var')
            self.__create_symlink(plt_path, 'waves')
            self.__create_symlink(plt_path, 'xcsim_libs')
            self.__create_symlink(plt_path, 'min_access.txt')
            self.__create_symlink(plt_path, 'scripts')
            self.__create_symlink(sim_path, 'scripts', 'sim_scripts')

            if os.system('xmsdfc models/s27ks0641/bmod/s27ks0641.sdf'):
                return -1

            if os.system('xmsdfc models/s26ks512s/bmod/s26ks512s.sdf'):
                return -1

            if os.system('xmsdfc models/s26ks512s/bmod/s26ks512s.sdf'):
                return -1

        command = self.__get_platform_cmd()

        with open('rtl_config.json', 'w') as file:
            file.write(self.full_config.dump_to_string())
        
        print ('Launching simulator with command:')
        print (command)

        self.gen_stim_txt()

        if not self.args.no_run:
            status = os.system(command)
        else:
            status = 0

        if self.args.extend_traces:
            traces = ['trace_core_1f_0.log', 'trace_core_00_0.log', 'trace_core_00_1.log', 'trace_core_00_2.log', 'trace_core_00_3.log', 'trace_core_00_4.log', 'trace_core_00_5.log', 'trace_core_00_6.log', 'trace_core_00_7.log', 'trace_core_00_8.log']
            binary = self.config.get_str('runner/boot-loader')
            rom_binary = '%s/boot/boot-gap9' % self.__get_platform_path()

            for trace in traces:
                if os.path.exists(trace):
                    trace_cmd = 'gap-rtl-trace-extend --binary %s --binary %s --input %s --output extended_%s' % (binary, rom_binary, trace, trace)
                    if os.system(trace_cmd):
                        return -1

        return status


    def gen_stim_txt(self):

        binary = self.config.get_str('runner/boot-loader')

        self.gen_stim_slm_64('vectors/stim.txt', [binary])



    def __gen_stim_slm(self, filename, width):

        #self.dump('  Generating to file: ' + filename)

        try:
            os.makedirs(os.path.dirname(filename))
        except:
            pass

        with open(filename, 'w') as file:
            for key in sorted(self.mem.keys()):
                file.write('%X_%0*X\n' % (int(key), width*2, self.mem.get(key)))



    def __add_mem_word(self, base, size, data, width):

        aligned_base = base & ~(width - 1)

        shift = base - aligned_base
        iter_size = width - shift
        if iter_size > size:
            iter_size = size

        value = self.mem.get(str(aligned_base))
        if value is None:
            value = 0

        value &= ~(((1<<width) - 1) << (shift*8))
        value |= int.from_bytes(data[0:iter_size], byteorder='little') << (shift*8)

        self.mem[str(aligned_base)] = value

        return iter_size


    def __add_mem(self, base, size, data, width):

        while size > 0:

            iter_size = self.__add_mem_word(base, size, data, width)

            size -= iter_size
            base += iter_size
            data = data[iter_size:]


    def __parse_binaries(self, width, binaries):

        self.mem = {}

        for binary in binaries:

            with open(binary, 'rb') as file:
                elffile = ELFFile(file)

                for segment in elffile.iter_segments():

                    if segment['p_type'] == 'PT_LOAD':

                        data = segment.data()
                        addr = segment['p_paddr']
                        size = len(data)

                        load = True
                        if len(self.areas) != 0:
                            load = False
                            for area in self.areas:
                                if addr >= area[0] and addr + size <= area[1]:
                                    load = True
                                    break

                        if load:

                            #self.dump('  Handling section (base: 0x%x, size: 0x%x)' % (addr, size))

                            self.__add_mem(addr, size, data, width)

                            if segment['p_filesz'] < segment['p_memsz']:
                                addr = segment['p_paddr'] + segment['p_filesz']
                                size = segment['p_memsz'] - segment['p_filesz']
                                #self.dump('  Init section to 0 (base: 0x%x, size: 0x%x)' % (addr, size))
                                self.__add_mem(addr, size, [0] * size, width)

                        else:
                            pass
                            #self.dump('  Bypassing section (base: 0x%x, size: 0x%x)' % (addr, size))


    def gen_stim_slm_64(self, stim_file, binaries):

        self.__parse_binaries(8, binaries)

        self.__gen_stim_slm(stim_file, 8)


    def __process_args(self):
        if self.args.gui:
            self.config.set('rtl/mode', 'gui')


    def __get_platform_path(self):

        if self.platform_path is None:

            if self.platform_tool == 'vsim':
                plt_path_name = 'VSIM_PATH'
                plt_path = os.environ.get('VSIM_PATH')
            else:
                plt_path_name = 'XCSIM_PATH'
                plt_path = os.environ.get('XCSIM_PATH')

            platform_path_name = self.plt_config.get_str('path_envvar')
            if platform_path_name is not None:
                envvar = os.environ.get(platform_path_name)
                if envvar is not None:
                    plt_path = envvar
                    plt_path_name = plt_path_name

            if plt_path is None:
                if platform_path_name is None:
                    raise errors.InputError("No platform found, VSIM_PATH is not defined.")
                else:
                    raise errors.InputError("No platform found, neither %s nor VSIM_PATH is defined." % (platform_path_name))

            if not os.path.exists(plt_path):
                raise errors.InputError("ERROR: %s=%s path does not exist" % (plt_path_name, plt_path))

            self.platform_path = plt_path

            os.environ['PULP_PATH'] = plt_path
            os.environ['TB_PATH']   = plt_path
            os.environ['VSIM_PLATFORM'] = plt_path
            os.environ['VSIM_PLATFORM'] = plt_path

            if self.platform_tool == 'vsim':
                pass
            else:
                os.environ['XCSIM_PLATFORM'] = plt_path

            self.platform_path = plt_path


        return self.platform_path


    def __get_platform_cmd(self):

        plt_path = self.__get_platform_path()

        self.set_env('VSIM_RUNNER_FLAGS', ' '.join(self.get_args()))
        
        command = []

        for key, value in self.env.items():
            command.append('export %s="%s" &&' % (key, value))

        command.append(self.plt_profile_config.get_str("commands/%s/executable" % self.config.get_str("rtl/mode")))

        commands = self.plt_profile_config.get("commands/%s" % self.config.get_str("rtl/mode"))

        command += self.cmd_args

        for arg in commands.get("args_eval").get_dict():
            command.append(eval(arg))

        return ' '.join(command)



    def __create_symlink(self, rtl_path, name, symname=None):

        if symname is None:
            symname = name

        if os.path.islink(symname):
          os.remove(symname)

        os.symlink(os.path.join(rtl_path, name), symname)


    def set_env(self, key, value):
        self.env[key] = value

    def set_arg(self, arg):
        self.plt_args.append(arg)

    def set_cmd_arg(self, arg):
        self.cmd_args.append(arg)

    def get_args(self):
        return self.plt_args