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

QUOTE = lambda s: '"'+s+'"'


class CodeBlock():

    class CommentWriter():
        def __init__(self, cb, max_len) -> None:
            self._cb = cb
            self._max_len = max_len
            self.reset()

        def write(self, comment):
            for elem in comment.split(' '):
                if self._cur_len + len(elem) + 1 > self._max_len:
                    self.end()
                self._cur_line.append(elem)
                self._cur_len += len(elem) + 1

        def end(self):
            self._cb.write(f'// {" ".join(self._cur_line)}')
            self.reset()

        def reset(self):
            self._cur_line = []
            self._cur_len = len(self._cb.get_indent()) + 3

    def __init__(self, starting_indent=0, indent_char="    "):
        self._indent = starting_indent
        self._indent_char = indent_char
        self._lines = []

    @property
    def lines(self):
        return self._lines

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
        if args:
            self._lines.append(fmt.format(*args))
        else:
            self._lines.append(fmt)
        return self

    def write_lines(self, lines):
        indent = self.get_indent()
        for line in lines.split('\n'):
            self._lines.append(indent + line)
        return self

    def write_start(self, fmt, *args):
        fmt = self.get_indent() + fmt
        self._lines.insert(0, fmt.format(*args))
        return self

    def start_long_comment(self, max_len=80):
        return CodeBlock.CommentWriter(self, max_len)

    def comment(self, fmt, *args):
        fmt = self.get_indent() + '// ' + fmt
        if args:
            self._lines.append(fmt.format(*args))
        else:
            self._lines.append(fmt)

        return self

    def __str__(self):
        return '\n'.join(self._lines)
