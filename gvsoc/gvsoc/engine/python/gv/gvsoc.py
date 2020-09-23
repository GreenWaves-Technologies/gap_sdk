#
# Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
#                    University of Bologna
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

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
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


def process_args(args, config):
    for trace in args.traces:
        config.set('gvsoc/traces/include_regex', trace)

    if args.trace_level is not None:
        config.set('gvsoc/traces/level', args.trace_level)

    if args.vcd:
        config.set('gvsoc/events/enabled', True)

    for event in args.events:
        config.set('gvsoc/events/include_regex', event)

    for tag in args.event_tags:
        config.set('gvsoc/events/tags', tag)

    if args.format is not None:
        config.set('gvsoc/events/format', args.format)

    if args.gtkw:
        config.set('gvsoc/events/gtkw', True)


def prepare_exec(config, full_config, gen=False):

    pass

def gen_config(args, config):

    full_config =  js.import_config(config.get_dict(), interpret=True, gen=True)


    gvsoc_config = full_config.get('gvsoc')

    debug_mode = gvsoc_config.get_bool('debug-mode') or \
        gvsoc_config.get_bool('traces/enabled') or \
        gvsoc_config.get_bool('events/enabled') or \
        len(gvsoc_config.get('traces/include_regex')) != 0 or \
        len(gvsoc_config.get('events/include_regex')) != 0

    gvsoc_config.set("debug-mode", debug_mode)

    if debug_mode:
        debug_binaries = []

        if args.binary is not None:
            debug_binaries.append(args.binary)

        rom_binary = full_config.get_str('**/soc/rom/binary')

        if rom_binary is not None:
            
            if os.path.exists(rom_binary):
                debug_binaries.append(rom_binary)

        for binary in debug_binaries:
            full_config.set('**/debug_binaries', binary + '.debugInfo')


    gvsoc_config_path = os.path.join(config.get_str('gapy/work_dir'), 'gvsoc_config.json')

    return full_config, gvsoc_config_path


def dump_config(full_config, gvsoc_config_path):
    with open(gvsoc_config_path, 'w') as file:
        file.write(full_config.dump_to_string())

class Runner(runner.default_runner.Runner):

    def __init__(self, args, config):
        super(Runner, self).__init__(args, config)
        process_args(args, config)

    def conf(self):
        runner.default_runner.Runner.conf(self)

        self.full_config, self.gvsoc_config_path = gen_config(self.args, self.config)


    def __gen_debug_info(self, full_config, gvsoc_config):
        for binary in full_config.get('**/debug_binaries').get_dict():
            if os.system('pulp-pc-info --file %s --all-file %s' % (binary.replace('.debugInfo', ''), binary)) != 0:
                raise errors.InputError('Error while generating debug symbols information, make sure the toolchain and the binaries are accessible ')



    def exec_prepare(self):
        os.chdir(self.config.get_str('gapy/work_dir'))

        prepare_exec(self.config, self.full_config, gen=True)

        dump_config(self.full_config, self.gvsoc_config_path)

        return 0

    def exec(self):

        os.chdir(self.config.get_str('gapy/work_dir'))

        prepare_exec(self.config, self.full_config)

        gvsoc_config = self.full_config.get('gvsoc')

        if gvsoc_config.get_bool('events/gen_gtkw'):
            gv.gtkwave.gen_gtkw_files(self.full_config, gvsoc_config)

        self.__gen_debug_info(self.full_config, self.full_config.get('gvsoc'))

        dump_config(self.full_config, self.gvsoc_config_path)

        os.environ['PULP_CONFIG_FILE'] = self.gvsoc_config_path

        if gvsoc_config.get("debug-mode"):
            launcher = gvsoc_config.get_str('launchers/debug')
        else:
            launcher = gvsoc_config.get_str('launchers/default')

        command = [launcher, '--config=' + self.gvsoc_config_path]

        if self.verbose:
            print ('Launching GVSOC with command: ')
            print (' '.join(command))

        return os.execvp(launcher, command)

