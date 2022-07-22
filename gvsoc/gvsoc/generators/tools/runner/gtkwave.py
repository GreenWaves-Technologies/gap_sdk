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

import gtkw_new
import os.path


class Gtkwave_map_file(object):

    def __init__(self, tree, comp, name, width=8):
        self.name = name
        self.values = []
        self.gen = False
        self.tree = tree
        self.width = width
        self.path = os.path.join(tree.get_work_dir(), comp.get_path().replace('/', '.') + '.%s.txt' % name)

    def add_value(self, value, color, text):
        self.values.append([value, text, color])

    def get_path(self):
        if not self.gen:
            self.gen = True

            with open(self.path, 'w') as file:
                file.write(gtkw_new.make_translation_filter(self.values, size=self.width))

        return self.path

class Gtkwave_group(object):

    def __init__(self, name, closed=True):
        self.name = name
        self.closed = closed
        self.childs = []

    def add_child(self, child):
        self.childs.append(child)

    def gen(self, gtkw):
        if self.name is not None and self.has_trace():
            gtkw.begin_group(self.name, self.closed)

        for child in self.childs:
            child.gen(gtkw)

        if self.name is not None and self.has_trace():
            gtkw.end_group(self.name, self.closed)

    def has_trace(self):
        for child in self.childs:
            if child.has_trace():
                return True

        return False

class Gtkwave_trace(object):

    def __init__(self, name, vcd_signal, map_file):
        self.name = name
        self.vcd_signal = vcd_signal
        self.map_file = map_file

    def gen(self, gtkw):
        gtkw.trace(self.vcd_signal, self.name, translate_filter_file=self.map_file)

    def has_trace(self):
        return True

class Gtkwave_vector(object):

    def __init__(self, tree, comp, name, traces=[], map_file=None, vector_map_file=None):
        self.comp = comp
        self.tree = tree
        self.name = name
        self.traces = traces
        self.map_file = map_file
        self.vector_map_file = vector_map_file

    def gen(self, gtkw):
        vector_traces = []

        for trace in self.traces:
            if len(trace) == 2:
                trace.append('')
            for i in range(0, 8):
                vcd_signal = self.tree.get_full_vcd_name(self.comp, trace[1] + trace[2])
                vector_traces.append('(%d)%s' % (i,vcd_signal))

        with self.tree.gtkw.vector(self.name, traces=vector_traces, extraflags=['popcnt', 'closed'], color='green', datafmt='dec', translate_filter_file=self.vector_map_file):

            for trace in self.traces:
                self.tree.gtkw.trace(self.tree.get_full_vcd_name(self.comp, trace[1] + trace[2]), trace[0], translate_filter_file=self.map_file.get_path())


    def has_trace(self):
        return True


class Gtkwave_tree(object):

    def __init__(self, fd, work_dir, gen_full_tree=False, tags=[]):

        self.gtkw = gtkw_new.GTKWSave(fd)
        self.gtkw.dumpfile(os.path.join(work_dir, 'all.vcd'))
        self.work_dir = work_dir
        self.current_groups = []
        self.current_groups.append(Gtkwave_group(None))
        self.gen_full_tree = gen_full_tree
        self.view = 'overview'
        self.tags = tags
        self.activate_traces = []

    def get_work_dir(self):
        return self.work_dir

    def get_full_vcd_name(self, comp, vcd_name):
        return (comp.get_path(gv_path=True) + '/' + vcd_name).replace('/', '.')

    def add_trace(self, comp, name, vcd_signal=None, ext='', map_file=None, map_file_path=None, tag=None, full_vcd_signal=None):
        if full_vcd_signal is None:
            full_vcd_signal = self.get_full_vcd_name(comp, vcd_signal)
        full_vcd_signal = full_vcd_signal + ext

        translate_filter_file = None
        if map_file is not None:
            translate_filter_file = map_file.get_path()
        if map_file_path is not None:
            translate_filter_file = map_file_path

        trace = Gtkwave_trace(name, full_vcd_signal, translate_filter_file)
        self.current_groups[-1].add_child(trace)

        if tag is not None and tag in self.tags:
            self.activate_traces.append('/' + full_vcd_signal.replace('.', '/'))

    def set_view(self, view):
        self.view = view

    def get_view(self):
        return self.view

    def begin_group(self, name, closed=True):
        group = Gtkwave_group(name, closed)
        self.current_groups[-1].add_child(group)
        self.current_groups.append(group)

    def end_group(self, name, closed=True):
        self.current_groups.pop()

    def new_map_file(self, comp, name, width=8):
        return Gtkwave_map_file(self, comp, name, width)

    def add_vector(self, comp, name, traces=[], map_file=None, tag=None):
        vector_filer = os.path.join(self.work_dir, comp.get_path().replace('/', '.') + '.%s_vector.txt' % name)

        with open(vector_filer, 'w') as file:
            for i in range(0, len(traces)+1):
                file.write('%2x ?CadetBlue?ACTIVE\n' % i)

        vector = Gtkwave_vector(self, comp, name, traces, map_file, vector_filer)
        self.current_groups[-1].add_child(vector)

        
    def gen(self):
        self.current_groups[0].gen(self.gtkw)
