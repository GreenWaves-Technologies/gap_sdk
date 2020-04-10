# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
