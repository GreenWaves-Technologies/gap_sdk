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

        return os.system(command)


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