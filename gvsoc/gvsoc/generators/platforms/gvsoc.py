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
import os

class Gvsoc(st.Component):

    def __init__(self, parent, name):

        super(Gvsoc, self).__init__(parent, name)

        self.add_property("proxy/enabled", False)
        self.add_property("proxy/port", 42951)

        self.add_property("events/enabled", False)
        self.add_property("events/include_raw", [])
        self.add_property("events/include_regex", [])
        self.add_property("events/exclude_regex", [])
        self.add_property("events/format", "fst")
        self.add_property("events/active", False)
        self.add_property("events/all", True)
        self.add_property("events/gtkw", False)
        self.add_property("events/gen_gtkw", False)
        self.add_property("events/files", [ ])
        self.add_property("events/traces", {})
        self.add_property("events/tags", [ "overview" ])
        self.add_property("events/gtkw", False)

        self.add_properties({
            "description": "GAP simulator.",

            "runner_module": "gv.gvsoc",
        
            "cycles_to_seconds": "int(max(cycles * nb_cores / 5000000, 600))",
        
            "verbose": True,
            "debug-mode": False,
            "sa-mode": True,
        
            "launchers": {
                "default": "gvsoc_launcher",
                "debug": "gvsoc_launcher_debug"
            },
        
            "traces": {
                "level": "debug",
                "format": "long",
                "enabled": False,
                "include_regex": [],
                "exclude_regex": []
            }
        })


    def __append_to_trace_groups(self, trace_groups, groups, vcd_name, trace):

        if len(groups) == 0:
            trace_groups[vcd_name] = trace
        else:
            group = groups[0]

            if trace_groups.get(group) is None:
                trace_groups[group] = {}

            self.__append_to_trace_groups(trace_groups[group], groups[1:], vcd_name, trace)


    def __dump_traces(self, tree, trace_groups):
        for group, value in trace_groups.items():
            if type(value) == list:
                tree.add_trace(self, group, full_vcd_signal=value[1], ext=value[2], map_file_path=value[3], tag=value[0])
            else:
                tree.begin_group(group)
                self.__dump_traces(tree, value)
                tree.end_group(group)



    def gen_gtkw(self, tree, comp_traces):
        traces = self.get_property("events/traces")

        if traces is not None:

            trace_groups = {}

            for name, trace in traces.items():
                view_path = trace.get('view_path')

                if view_path.find('.') == -1:
                    parent = None
                    name = view_path
                else:
                    parent, name = view_path.rsplit('.', 1)

                #if parent == path:
                tag = trace.get('tag')
                vcd_path = trace.get('vcd_path')
                trace_filter = trace.get('filter')
                if trace_filter is not None:
                    trace_filter = os.path.join(os.getcwd(), trace_filter)

                width = ''
                if vcd_path.find('[') != -1:
                    vcd_path, width = vcd_path.split('[')
                    width = '[' + width

                vcd_name = vcd_path.split('.')[-1]

                tree_view, tree_path = view_path.split('.', 1)

                if tree_view == tree.get_view():

                    self.__append_to_trace_groups(trace_groups, tree_path.split('.'), vcd_name, [tag, vcd_path, width, trace_filter])

            self.__dump_traces(tree, trace_groups)


    def gen_gtkw_conf(self, tree, traces):
        self.vcd_group(self, skip=True)
