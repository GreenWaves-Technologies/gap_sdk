# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

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
