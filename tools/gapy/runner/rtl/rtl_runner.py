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
import errors
import argparse
import json_tools as js


def appendArgs(parser: argparse.ArgumentParser, runnerConfig: js.config) -> None:
    """
    Append specific module arguments.

    """
    parser.add_argument("--gui",
                        dest="gui",
                        action="store_true",
                        help="Launch in gui mode")




class Runner(runner.default_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)

        self.__process_args()

        self.plt_args = []
        self.env = {}
        self.platform_path = None
        self.platform_tool = self.config.get_str('rtl/simulator')

        self.plt_config = self.config.get('rtl/' + self.platform_tool)
        self.plt_profile_config = self.plt_config.get('profiles/rtl')

        if os.environ.get('INSTALL_DIR') is not None:
            dpi_path = '%s/lib/libpulpdpi' % (os.environ.get('INSTALL_DIR'))
            if os.path.exists(dpi_path + '.so'):
                self.set_arg('-sv_lib %s' % dpi_path)

        self.set_arg('-gCONFIG_FILE=rtl_config.json')

        full_config =  js.import_config(self.config.get_dict(), interpret=True, gen=True)

        with open('rtl_config.json', 'w') as file:
            file.write(full_config.dump_to_string())


    def exec(self):
        command = self.__get_platform_cmd()

        print ('Launching simulator with command:')
        print (command)

        return os.system(command)


    def __process_args(self):
        if self.args.gui:
            self.config.set('rtl/mode', 'gui')


    def __get_platform_path(self):

        if self.platform_path is None:

            plt_path_name = 'VSIM_PATH'
            plt_path = os.environ.get('VSIM_PATH')

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

            os.environ['VSIM_PATH'] = plt_path
            os.environ['PULP_PATH'] = plt_path
            os.environ['TB_PATH']   = plt_path
            os.environ['VSIM_PLATFORM'] = plt_path

            self.__create_symlink(plt_path, 'boot')
            self.__create_symlink(plt_path, 'modelsim.ini')
            self.__create_symlink(plt_path, 'work')
            self.__create_symlink(plt_path, 'tcl_files')
            self.__create_symlink(plt_path, 'waves')
            self.__create_symlink(plt_path, 'models')

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

        for arg in commands.get("args_eval").get_dict():
            command.append(eval(arg))

        return ' '.join(command)



    def __create_symlink(self, rtl_path, name):
        if os.path.islink(name):
          os.remove(name)

        os.symlink(os.path.join(rtl_path, name), name)


    def set_env(self, key, value):
        self.env[key] = value

    def set_arg(self, arg):
        self.plt_args.append(arg)

    def get_args(self):
        return self.plt_args