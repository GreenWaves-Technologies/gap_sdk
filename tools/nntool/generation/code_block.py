# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

quote = lambda s: '"'+s+'"'

class CodeBlock():
    def __init__(self, starting_indent=0, indent_char="    "):
        self._indent = starting_indent
        self._indent_char = indent_char
        self._lines = []

    def indent(self):
        self._indent += 1
        return self

    def deindent(self):
        self._indent -= 1
        return self

    def get_indent(self):
        return self._indent_char * self._indent

# pylint: disable=invalid-name
    def nl(self):
        self._lines.append("")
        return self

    def append_last(self, fmt, *args):
        last = self._lines[-1].rstrip()
        self._lines[-1] = last + fmt.format(*args)

    def write(self, fmt, *args):
        fmt = self.get_indent() + fmt
        self._lines.append(fmt.format(*args))
        return self

    def write_start(self, fmt, *args):
        fmt = self.get_indent() + fmt
        self._lines.insert(0, fmt.format(*args))
        return self

    def comment(self, fmt, *args):
        fmt = self.get_indent() + '// ' + fmt
        self._lines.append(fmt.format(*args))
        return self

    def __str__(self):
        return '\n'.join(self._lines)
