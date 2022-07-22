#
# Copyright (C) 2020 GreenWaves Technologies
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

import gsystree as st
from platforms.gvsoc import Gvsoc
from tools.runner.gtkwave import Gtkwave_tree

class Runner_target(st.Component):

    def __init__(self, parent, name, boot_mode="default", boot_device=None):

        super(Runner_target, self).__init__(parent, name)

        self.add_properties({
            "runner": {
                "flash_devices": [
                ],
                "boot": {
                    "mode": boot_mode,
                    "device": boot_device
                },
                "modules": {
                }
            }
        })

        self.declare_runner_target()

    def declare_flash(self, path=None):
        self.get_property('runner/flash_devices').append(path)


class Runner(st.Component):

    def __init__(self, parent, name, options={}, target_class=None):

        super(Runner, self).__init__(parent, name, options)

        self.vcd_group(skip=True)

        self.add_properties({
            "runner": {
                "targets": []
            },

            "gapy": {
                "verbose": False
            }
        })

        Gvsoc(self, 'gvsoc')

        if target_class is not None:
            target = target_class(self, 'target')
            target.vcd_group_create = False
            target.is_top = True

    def declare_runner_target(self, path=None):
        self.get_property('runner/targets').append(path)

    def gen_gtkw_script(self, work_dir, path, tags=[], level=0, trace_file=None, gen_full_tree=False):
        
        traces = []

        if trace_file is not None:
            with open(trace_file, 'r') as file:
                for line in file.readlines():
                    trace_path, trace, name = line.split(' ')
                    if trace_path.find('/') == 0:
                        trace_path = trace_path[1:]
                    name = name.replace('\n', '')
                    traces.append([trace_path.split('/'), trace.split('/'), name])

        with open(path, 'w') as fd:
            tree = Gtkwave_tree(fd, work_dir, gen_full_tree=gen_full_tree, tags=tags)

            tree.begin_group('overview', closed=False)
            tree.set_view('overview')
            self.gen_gtkw_tree(tree, traces=traces)
            tree.end_group('overview')

            tree.begin_group('system')
            tree.set_view('system')
            self.gen_gtkw_tree(tree, traces=traces)
            tree.end_group('system')


            tree.gen()

            return tree.activate_traces