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
from elftools.elf.elffile import ELFFile


def appendArgs(parser: argparse.ArgumentParser, runnerConfig: js.config) -> None:
    """
    Append specific module arguments.

    """




class Runner(runner.default_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)


    def exec(self):

        openocd = self.config.get_str("openocd/path")
        cable = self.config.get_str('openocd/cable')
        script = self.config.get_str('openocd/script')
        binary = self.config.get_str('runner/boot-loader')

        with open(binary, 'rb') as file:
            elffile = ELFFile(file)
            entry = elffile.header['e_entry']

        if self.config.get_bool("openocd/remote/enabled"):

            url = self.config.get_str('openocd/remote/url')
            path = self.config.get_str('openocd/remote/path')

            cmd = 'ssh %s "mkdir -p %s"' % (url, path)
            print ('Creating remote path with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            cmd = 'scp %s %s:%s/test' % (binary, url, path)
            print ('Copying binary with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            cmd = 'ssh -t %s "%s -c \\"script %s; script %s; load_and_start_binary %s/test 0x%x\\""' % (url, openocd, cable, script, path, entry)
            print ('Launching execution with command:')
            print (cmd)
            if os.system(cmd) != 0:
                return -1

            return 0

        else:

            cmd = '%s -c "script %s; script %s; load_and_start_binary %s 0x%x"' % (openocd, cable, script, binary, entry)

            print ('Launching execution with command:')
            print (cmd)

            if os.system(cmd) != 0:
                return -1

            return 0