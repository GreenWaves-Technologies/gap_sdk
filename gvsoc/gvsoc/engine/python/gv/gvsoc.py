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
import argparse
import json_tools as js
import errors
import gv.gtkwave


def appendArgs(parser: argparse.ArgumentParser, runnerConfig: js.config) -> None:
    """
    Append specific runner arguments.
    """
    
    parser.add_argument("--trace",
                        dest="traces",
                        default=[],
                        action="append",
                        help="Specify gvsoc trace")

    parser.add_argument("--trace-level",
                        dest="trace_level",
                        default=None,
                        help="Specify trace level")

    parser.add_argument("--vcd", dest="vcd", action="store_true", help="Activate VCD traces")

    parser.add_argument("--event", dest="events", default=[], action="append", help="Specify gvsoc event (for VCD traces)")

    parser.add_argument("--event-tags", dest="event_tags", default=[], action="append", help="Specify gvsoc event through tags(for VCD traces)")

    parser.add_argument("--event-format", dest="format", default=None, help="Specify events format (vcd or fst)")

    parser.add_argument("--gtkw", dest="gtkw", action="store_true", help="Dump events to pipe and open gtkwave in interactive mode")





class Runner(runner.default_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)
        self.__process_args()


    def __gen_debug_info(self, full_config, gvsoc_config):
        debug_binaries = []

        if self.args.binary is not None:
            debug_binaries.append(self.args.binary)

        rom_binary = full_config.get_str('**/soc/rom/binary')

        if rom_binary is not None:
            try:
                rom_binary = eval(rom_binary)
            except:
                rom_binary = rom_binary
                raise
            
            if os.path.exists(rom_binary):
                debug_binaries.append(rom_binary)

        for binary in debug_binaries:
            if os.system('pulp-pc-info --file %s --all-file %s' % (binary, binary + '.debugInfo')) != 0:
                raise errors.InputError('Error while generating debug symbols information, make sure the toolchain and the binaries are accessible ')

            full_config.set('**/debug_binaries', binary + '.debugInfo')


    def exec(self):

        os.chdir(self.config.get_str('gapy/work_dir'))

        config_path = os.path.join(os.getcwd(), 'config.json')
        with open(config_path, 'w') as file:
            file.write(self.config.dump_to_string())

        full_config =  js.import_config(self.config.get_dict(), interpret=True, gen=True)

        gvsoc_config = full_config.get('gvsoc')

        self.__gen_debug_info(full_config, gvsoc_config)

        debug_mode = gvsoc_config.get_bool('debug-mode') or \
            gvsoc_config.get_bool('traces/enabled') or \
            gvsoc_config.get_bool('events/enabled') or \
            len(gvsoc_config.get('traces/include_regex')) != 0 or \
            len(gvsoc_config.get('events/include_regex')) != 0

        gvsoc_config.set("debug-mode", debug_mode)

        gv.gtkwave.gen_gtkw_files(full_config, gvsoc_config)

        if debug_mode:
            launcher = gvsoc_config.get_str('launchers/debug')
        else:
            launcher = gvsoc_config.get_str('launchers/default')

        gvsoc_config_path = os.path.join(os.getcwd(), 'gvsoc_config.json')

        with open(gvsoc_config_path, 'w') as file:
            file.write(full_config.dump_to_string())

        os.environ['PULP_CONFIG_FILE'] = gvsoc_config_path

        command = [launcher, '--config=' + gvsoc_config_path]

        if self.verbose:
            print ('Launching GVSOC with command: ')
            print (' '.join(command))

        return os.execvp(launcher, command)


    def __process_args(self):
        for trace in self.args.traces:
            self.config.set('gvsoc/traces/include_regex', trace)

        if self.args.trace_level is not None:
            self.config.set('gvsoc/traces/level', self.args.trace_level)

        if self.args.vcd:
            self.config.set('gvsoc/events/enabled', True)

        for event in self.args.events:
            self.config.set('gvsoc/events/include_regex', event)

        for tag in self.args.event_tags:
            self.config.set('gvsoc/events/tags', tag)

        if self.args.format is not None:
            self.config.set('gvsoc/events/format', args.format)

        if self.args.gtkw:
            self.config.set('gvsoc/events/gtkw', True)
