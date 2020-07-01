#
# Copyright (C) 2020  GreenWaves Technologies, SAS
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

import vp_core as vp
import re
import ctypes

class component(vp.component):

    implementation = 'vp.trace_domain_impl'

    max_path_len = 32

    def reg_path(self, path):
        length = len(path)

        if length > self.max_path_len:
            self.max_path_len = length

    def dump_msg(self, path, str):
        if path == '':
            path = '/'
        print ('0: 0: [' + vp.bcolors.OKBLUE + path.ljust(self.max_path_len, ' ') + vp.bcolors.ENDC + '] ' + str)

    def dump_error(self, path, str):
        if path == '':
            path = '/'
        print ('0: 0: [' + vp.bcolors.FAIL + path + vp.bcolors.ENDC + '] ' + str)

    def reg_traces(self, traces, event):

        if event == 0:
          for trace in traces:
            self.traces.append(re.compile(trace))

        implem_traces = (ctypes.c_char_p * len(traces))()
        for i in range(0, len(traces)):
          implem_traces[i] = traces[i].encode('utf-8')
        self.impl.module.vp_trace_add_paths(self.impl.instance, event, len(traces), implem_traces)


    def build(self):
        self.new_service('trace')
        self.new_port('out')
        self.traces = []

        config = self.get_json()
        self.impl.module.vp_trace_add_paths.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        self.impl.module.vp_trace_level.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.impl.module.vp_trace_exchange_max_path_len.argtypes = [ctypes.c_void_p, ctypes.c_int]
        if config is not None:
          self.impl.module.vp_trace_level.argtypes = [ctypes.c_void_p, ctypes.c_char_p]

          self.impl.module.vp_trace_level(self.impl.instance, config.get_child_str('traces/level').encode('utf-8'))
          self.reg_traces(config.get('trace').get_dict(), 0)
          self.reg_traces(config.get('event').get_dict(), 1)


    def is_active(self, path):
      for trace in self.traces:
        if trace.search(path):
          return True
      return False
